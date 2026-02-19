#include "MatchingEngine.h"
#include "Feed/FeedPublisher.h"
#include "Order.h"
#include "Utils.h"
#include <optional>

BestState fb::MatchingEngine ::snapshotState_() const
{
    BestState bs;
    if (const std::optional<fb::OrderView> bid = orderBook_.getBestBidOrder())
    {
        bs.bestBidPrice = bid->price;
        bs.bestBidQty   = bid->quantity;
    }

    if (const std::optional<fb::OrderView> ask = orderBook_.getBestAskOrder())
    {
        bs.bestAskPrice = ask->price;
        bs.bestAskQty   = ask->quantity;
    }

    return bs;
}

void fb::MatchingEngine::publish_(const bool                    hasBookUpdated,
                                  const std::vector<fb::Trade>& trades) const
{
    if (pub_ == nullptr)
    {
        return;
    }

    if (hasBookUpdated)
    {
        pub_->onBookUpdate(orderBook_);
    }

    for (const fb::Trade& t : trades)
    {
        pub_->onTrade(t);
    }
}

void fb::MatchingEngine::setPublisher(fb::FeedPublisher* pub)
{
    pub_ = pub;
}

std::vector<fb::Trade> fb::MatchingEngine::onNewOrder(const fb::Order& order)
{
    int64_t                qty    = order.quantity;
    const BestState        before = fb::MatchingEngine::snapshotState_();
    std::vector<fb::Trade> trades;

    if (order.side == fb::Side::Buy)
    {
        while (orderBook_.getBestAsk().value_or(INT64_MAX) <= order.price && qty > 0)
        {
            if (const fb::OrderView ov = orderBook_.getBestAskOrder().value();
                qty >= ov.quantity)
            {
                orderBook_.removeOrder(ov.id);
                trades.emplace_back(tradeId_++,
                                    order.id,
                                    ov.id,
                                    ov.price,
                                    ov.quantity,
                                    fb::Utils::steadyNowNS());
                qty -= ov.quantity;
            }
            else
            {
                orderBook_.reduceOrder(ov.id, qty);
                trades.emplace_back(tradeId_++,
                                    order.id,
                                    ov.id,
                                    ov.price,
                                    qty,
                                    fb::Utils::steadyNowNS());
                qty = 0;
            }
        }
    }
    else
    {
        while (orderBook_.getBestBid().value_or(INT64_MIN) >= order.price && qty > 0)
        {
            if (const fb::OrderView ov = orderBook_.getBestBidOrder().value();
                qty >= ov.quantity)
            {
                orderBook_.removeOrder(ov.id);
                trades.emplace_back(tradeId_++,
                                    ov.id,
                                    order.id,
                                    ov.price,
                                    ov.quantity,
                                    fb::Utils::steadyNowNS());
                qty -= ov.quantity;
            }
            else
            {
                orderBook_.reduceOrder(ov.id, qty);
                trades.emplace_back(tradeId_++,
                                    ov.id,
                                    order.id,
                                    ov.price,
                                    qty,
                                    fb::Utils::steadyNowNS());
                qty = 0;
            }
        }
    }

    if (qty > 0)
    {
        fb::Order orderToAdd = order;
        orderToAdd.quantity  = qty;
        orderBook_.addOrder(orderToAdd);
    }
    const BestState after = fb::MatchingEngine::snapshotState_();
    fb::MatchingEngine::publish_(before != after, trades);

    return trades;
}

void fb::MatchingEngine::onCancel(const uint64_t orderId)
{
    const BestState before = fb::MatchingEngine::snapshotState_();
    orderBook_.removeOrder(orderId);
    const BestState after = fb::MatchingEngine::snapshotState_();

    fb::MatchingEngine::publish_(before != after, {});
}

void fb::MatchingEngine::onReduce(const uint64_t orderId,
                                  const int64_t  reduceQty)
{
    if (reduceQty <= 0)
    {
        return;
    }

    const BestState before = fb::MatchingEngine::snapshotState_();
    orderBook_.reduceOrder(orderId, reduceQty);
    const BestState after = fb::MatchingEngine::snapshotState_();

    fb::MatchingEngine::publish_(before != after, {});
}

const fb::OrderBook& fb::MatchingEngine::getBook() const
{
    return orderBook_;
}