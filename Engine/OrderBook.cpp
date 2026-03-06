#include "OrderBook.h"
#include "Order.h"

void fb::OrderBook::reduceAsk_(const uint64_t orderId,
                               const int64_t  price,
                               const int64_t  reduceQty)
{
    auto bookIt = askBook_.find(price);
    if (bookIt == askBook_.end())
    {
        return;
    }

    for (auto it = bookIt->second.begin(); it != bookIt->second.end(); ++it)
    {
        if (*it == orderId)
        {
            if (OrderEntry& entry = ordersMap_[orderId];
                entry.remainingQty <= reduceQty)
            {
                bookIt->second.erase(it);
                if (bookIt->second.empty())
                {
                    askBook_.erase(price);
                }
                ordersMap_.erase(orderId);
            }
            else
            {
                entry.remainingQty -= reduceQty;
                entry.status = OrderStatus::PartiallyFilled;
            }
            break;
        }
    }
}

void fb::OrderBook::reduceBid_(const uint64_t orderId,
                               const int64_t  price,
                               const int64_t  reduceQty)
{
    auto bookIt = bidBook_.find(price);
    if (bookIt == bidBook_.end())
    {
        return;
    }

    for (auto it = bookIt->second.begin(); it != bookIt->second.end(); ++it)
    {
        if (*it == orderId)
        {
            if (OrderEntry& entry = ordersMap_[orderId];
                entry.remainingQty <= reduceQty)
            {
                bookIt->second.erase(it);
                if (bookIt->second.empty())
                {
                    bidBook_.erase(price);
                }
                ordersMap_.erase(orderId);
            }
            else
            {
                entry.remainingQty -= reduceQty;
                entry.status = OrderStatus::PartiallyFilled;
            }
            break;
        }
    }
}

void fb::OrderBook::addOrder(const fb::Order& order)
{
    OrderEntry entry {
        .id           = order.id,
        .side         = order.side,
        .price        = order.price,
        .remainingQty = order.quantity,
        .timestamp    = order.timestamp,
        .status       = OrderStatus::New
    };

    ordersMap_.emplace(order.id, entry);
    order.side == fb::Side::Buy
        ? bidBook_[order.price].push_back(order.id)
        : askBook_[order.price].push_back(order.id);
}

void fb::OrderBook::reduceOrder(const uint64_t orderId,
                                const int64_t  reduceQty)
{
    if (ordersMap_.contains(orderId) == false)
    {
        return;
    }

    const fb::Side side  = ordersMap_[orderId].side;
    const int64_t  price = ordersMap_[orderId].price;

    side == fb::Side::Buy
        ? reduceBid_(orderId, price, reduceQty)
        : reduceAsk_(orderId, price, reduceQty);
}

void fb::OrderBook::removeOrder(const uint64_t orderId)
{
    reduceOrder(orderId, INT64_MAX);
}

std::optional<int64_t> fb::OrderBook::getBestBid() const
{
    if (bidBook_.empty())
    {
        return std::nullopt;
    }
    return bidBook_.begin()->first;
}

std::optional<int64_t> fb::OrderBook::getBestAsk() const
{
    if (askBook_.empty())
    {
        return std::nullopt;
    }
    return askBook_.begin()->first;
}

std::optional<fb::OrderView> fb::OrderBook::getBestBidOrder() const
{
    if (bidBook_.empty())
    {
        return std::nullopt;
    }
    const OrderEntry& entry = ordersMap_.at(bidBook_.begin()->second.front());
    return fb::OrderView { .id = entry.id, .price = entry.price, .quantity = entry.remainingQty };
}

std::optional<fb::OrderView> fb::OrderBook::getBestAskOrder() const
{
    if (askBook_.empty())
    {
        return std::nullopt;
    }
    const OrderEntry& entry = ordersMap_.at(askBook_.begin()->second.front());
    return fb::OrderView { .id = entry.id, .price = entry.price, .quantity = entry.remainingQty };
}