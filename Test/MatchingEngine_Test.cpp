#include "Engine/MatchingEngine.h"
#include "Engine/Order.h"

#include "gtest/gtest.h"

namespace fb_test
{
namespace MatchingEngine
{
    const fb::Order S100AT10 { .side = fb::Side::Sell, .id = 1, .price = 10, .quantity = 100, .timestamp = 1 };
    const fb::Order S50AT10 { .side = fb::Side::Sell, .id = 2, .price = 10, .quantity = 50, .timestamp = 2 };
    const fb::Order S50AT9 { .side = fb::Side::Sell, .id = 3, .price = 9, .quantity = 50, .timestamp = 3 };
    const fb::Order S100AT11 { .side = fb::Side::Sell, .id = 4, .price = 11, .quantity = 100, .timestamp = 1 };
    const fb::Order S50AT10LATE { .side = fb::Side::Sell, .id = 10, .price = 10, .quantity = 50, .timestamp = 10 };

    const fb::Order B100AT10 { .side = fb::Side::Buy, .id = 11, .price = 10, .quantity = 100, .timestamp = 101 };
    const fb::Order B70AT10 { .side = fb::Side::Buy, .id = 12, .price = 10, .quantity = 70, .timestamp = 102 };

} // namespace MatchingEngine
} // namespace fb_test

GTEST_TEST(MatchingEngine, match_buy_and_sell_fully)
{
    fb::MatchingEngine engine;
    EXPECT_TRUE(engine.onNewOrder(fb_test::MatchingEngine::S100AT10).empty());
    const std::vector<fb::Trade> trades = engine.onNewOrder(fb_test::MatchingEngine::B100AT10);
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].buyOrderId, fb_test::MatchingEngine::B100AT10.id);
    EXPECT_EQ(trades[0].sellOrderId, fb_test::MatchingEngine::S100AT10.id);
    EXPECT_EQ(trades[0].quantity, fb_test::MatchingEngine::S100AT10.quantity);
    EXPECT_EQ(trades[0].price, fb_test::MatchingEngine::S100AT10.price);

    EXPECT_FALSE(engine.getBook().getBestAsk().has_value());
    EXPECT_FALSE(engine.getBook().getBestBid().has_value());
}

GTEST_TEST(MatchingEngine, match_partially_leaves_resting_order)
{
    fb::MatchingEngine engine;
    EXPECT_TRUE(engine.onNewOrder(fb_test::MatchingEngine::S100AT10).empty());

    const std::vector<fb::Trade> trades = engine.onNewOrder(fb_test::MatchingEngine::B70AT10);
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].buyOrderId, fb_test::MatchingEngine::B70AT10.id);
    EXPECT_EQ(trades[0].sellOrderId, fb_test::MatchingEngine::S100AT10.id);
    EXPECT_EQ(trades[0].quantity, fb_test::MatchingEngine::B70AT10.quantity);
    EXPECT_EQ(trades[0].price, fb_test::MatchingEngine::S100AT10.price);

    const fb::OrderView curBestAskOrder = engine.getBook().getBestAskOrder().value();
    EXPECT_EQ(curBestAskOrder.id, fb_test::MatchingEngine::S100AT10.id);
    EXPECT_EQ(curBestAskOrder.quantity,
              fb_test::MatchingEngine::S100AT10.quantity - fb_test::MatchingEngine::B70AT10.quantity);

    EXPECT_FALSE(engine.getBook().getBestBid().has_value());
}

GTEST_TEST(MatchingEngine, match_price_priority)
{
    fb::MatchingEngine engine;
    EXPECT_TRUE(engine.onNewOrder(fb_test::MatchingEngine::S50AT9).empty());
    EXPECT_TRUE(engine.onNewOrder(fb_test::MatchingEngine::S50AT10).empty());

    const std::vector<fb::Trade> trades = engine.onNewOrder(fb_test::MatchingEngine::B100AT10);
    EXPECT_EQ(trades.size(), 2);
    EXPECT_EQ(trades[0].buyOrderId, fb_test::MatchingEngine::B100AT10.id);
    EXPECT_EQ(trades[0].sellOrderId, fb_test::MatchingEngine::S50AT9.id);
    EXPECT_EQ(trades[0].quantity, fb_test::MatchingEngine::S50AT9.quantity);
    EXPECT_EQ(trades[0].price, fb_test::MatchingEngine::S50AT9.price);

    EXPECT_EQ(trades[1].buyOrderId, fb_test::MatchingEngine::B100AT10.id);
    EXPECT_EQ(trades[1].sellOrderId, fb_test::MatchingEngine::S50AT10.id);
    EXPECT_EQ(trades[1].quantity, fb_test::MatchingEngine::S50AT10.quantity);
    EXPECT_EQ(trades[1].price, fb_test::MatchingEngine::S50AT10.price);

    EXPECT_FALSE(engine.getBook().getBestAsk().has_value());
    EXPECT_FALSE(engine.getBook().getBestBid().has_value());
}

GTEST_TEST(MatchingEngine, match_fifo_within_same_price_level)
{
    fb::MatchingEngine engine;
    EXPECT_TRUE(engine.onNewOrder(fb_test::MatchingEngine::S50AT10).empty());
    EXPECT_TRUE(engine.onNewOrder(fb_test::MatchingEngine::S50AT10LATE).empty());

    const std::vector<fb::Trade> trades = engine.onNewOrder(fb_test::MatchingEngine::B70AT10);

    EXPECT_EQ(trades.size(), 2);
    EXPECT_EQ(trades[0].buyOrderId, fb_test::MatchingEngine::B70AT10.id);
    EXPECT_EQ(trades[0].sellOrderId, fb_test::MatchingEngine::S50AT10.id);
    EXPECT_EQ(trades[0].quantity, fb_test::MatchingEngine::S50AT10.quantity);
    EXPECT_EQ(trades[0].price, fb_test::MatchingEngine::S50AT10.price);

    EXPECT_EQ(trades[1].buyOrderId, fb_test::MatchingEngine::B70AT10.id);
    EXPECT_EQ(trades[1].sellOrderId, fb_test::MatchingEngine::S50AT10LATE.id);
    EXPECT_EQ(trades[1].quantity,
         fb_test::MatchingEngine::B70AT10.quantity - fb_test::MatchingEngine::S50AT10.quantity);
    EXPECT_EQ(trades[1].price, fb_test::MatchingEngine::S50AT10LATE.price);

    const fb::OrderView curBestAskOrder = engine.getBook().getBestAskOrder().value();
    EXPECT_EQ(curBestAskOrder.id, fb_test::MatchingEngine::S50AT10LATE.id);
    EXPECT_EQ(curBestAskOrder.quantity,
              fb_test::MatchingEngine::S50AT10LATE.quantity
                  + fb_test::MatchingEngine::S50AT10.quantity
                  - fb_test::MatchingEngine::B70AT10.quantity);

    EXPECT_FALSE(engine.getBook().getBestBid().has_value());
}

GTEST_TEST(MatchingEngine, no_match_when_price_not_crossed)
{
    fb::MatchingEngine engine;
    EXPECT_TRUE(engine.onNewOrder(fb_test::MatchingEngine::S100AT11).empty());
    EXPECT_TRUE(engine.onNewOrder(fb_test::MatchingEngine::B100AT10).empty());

    EXPECT_EQ(engine.getBook().getBestAskOrder().value().id, fb_test::MatchingEngine::S100AT11.id);
    EXPECT_EQ(engine.getBook().getBestBidOrder().value().id, fb_test::MatchingEngine::B100AT10.id);
    EXPECT_EQ(engine.getBook().getBestAsk().value(), fb_test::MatchingEngine::S100AT11.price);
    EXPECT_EQ(engine.getBook().getBestBid().value(), fb_test::MatchingEngine::B100AT10.price);
}

GTEST_TEST(MatchingEngine, cancel_order)
{
    fb::MatchingEngine engine;
    engine.onNewOrder(fb_test::MatchingEngine::S100AT10);
    EXPECT_EQ(engine.getBook().getBestAskOrder().value().id, fb_test::MatchingEngine::S100AT10.id);
    engine.onCancel(fb_test::MatchingEngine::S100AT10.id);
    EXPECT_FALSE(engine.getBook().getBestAsk().has_value());
}

GTEST_TEST(MatchingEngine, reduce_order_partial)
{
    fb::MatchingEngine engine;
    engine.onNewOrder(fb_test::MatchingEngine::S100AT10);
    EXPECT_EQ(engine.getBook().getBestAskOrder().value().id, fb_test::MatchingEngine::S100AT10.id);
    const int64_t qtyToReduce = 30;
    engine.onReduce(fb_test::MatchingEngine::S100AT10.id, qtyToReduce);
    const fb::OrderView curBestAskOrder = engine.getBook().getBestAskOrder().value();
    EXPECT_EQ(curBestAskOrder.id, fb_test::MatchingEngine::S100AT10.id);
    EXPECT_EQ(curBestAskOrder.quantity,fb_test::MatchingEngine::S100AT10.quantity - qtyToReduce);
}

GTEST_TEST(MatchingEngine, reduce_order_exceeds_remaining)
{
    fb::MatchingEngine engine;
    engine.onNewOrder(fb_test::MatchingEngine::S100AT10);
    EXPECT_EQ(engine.getBook().getBestAskOrder().value().id, fb_test::MatchingEngine::S100AT10.id);
    const int64_t qtyToReduce = fb_test::MatchingEngine::S100AT10.quantity + 30;
    engine.onReduce(fb_test::MatchingEngine::S100AT10.id, qtyToReduce);
    EXPECT_FALSE(engine.getBook().getBestAsk().has_value());
}