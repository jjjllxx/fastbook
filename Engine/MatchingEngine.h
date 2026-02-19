#pragma once

#include "OrderBook.h"
#include <vector>

namespace
{
struct BestState
{
    int64_t bestBidPrice = 0;
    int64_t bestBidQty   = 0;
    int64_t bestAskPrice = 0;
    int64_t bestAskQty   = 0;

    bool operator==(const BestState& other) const
    {
        return bestBidPrice == other.bestBidPrice
               && bestBidQty == other.bestBidQty
               && bestAskPrice == other.bestAskPrice
               && bestAskQty == other.bestAskQty;
    }

    bool operator!=(const BestState& other) const
    {
        return !(*this == other);
    }
};
} // namespace

namespace fb
{
struct Order;
class FeedPublisher;

struct Trade
{
    uint64_t id;
    uint64_t buyOrderId;
    uint64_t sellOrderId;
    int64_t  price;
    int64_t  quantity;
    uint64_t timestamp;
};

class MatchingEngine
{
    uint64_t           tradeId_ = 0;
    fb::OrderBook      orderBook_;
    fb::FeedPublisher* pub_ = nullptr;

    BestState snapshotState_() const;
    void publish_(const bool                    hasBookUpdated,
                  const std::vector<fb::Trade>& trades) const;

public:
    void setPublisher(fb::FeedPublisher* pub);

    std::vector<fb::Trade> onNewOrder(const fb::Order& order);

    void onCancel(const uint64_t orderId);
    void onReduce(const uint64_t orderId,
                  const int64_t  reduceQty);

    const OrderBook& getBook() const;
};
} // namespace fb