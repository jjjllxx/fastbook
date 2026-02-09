#pragma once

#include "Order.h"
#include <list>
#include <map>
#include <optional>
#include <unordered_map>

namespace fb
{
class OrderBook
{
    std::unordered_map<uint64_t, fb::Order> ordersMap_;

    std::map<int64_t, std::list<uint64_t>, std::less<>>    askBook_;
    std::map<int64_t, std::list<uint64_t>, std::greater<>> bidBook_;

    void reduceAsk_(const uint64_t orderId,
                    const int64_t  price,
                    const int64_t  reduceQty);
    void reduceBid_(const uint64_t orderId,
                    const int64_t  price,
                    const int64_t  reduceQty);

public:
    void addOrder(const fb::Order& order);
    void reduceOrder(const uint64_t orderId,
                     const int64_t  reduceQty);
    void removeOrder(const uint64_t orderId);

    std::optional<int64_t> getBestBid() const;
    std::optional<int64_t> getBestAsk() const;

    std::optional<fb::OrderView> getBestBidOrder() const;
    std::optional<fb::OrderView> getBestAskOrder() const;
};
} // namespace fb