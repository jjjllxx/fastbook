#include "OrderBook.h"
#include "Order.h"

void fb::OrderBook::reduceAsk_(const uint64_t orderId,
                              const int64_t  price,
                              const int64_t  reduceQty)
{
    if (askBook_.contains(price) == false)
    {
        return;
    }

    for (auto it = askBook_[price].begin(); it != askBook_[price].end(); ++it)
    {
        if (*it == orderId)
        {
            if (int64_t& qty = ordersMap_[orderId].quantity;
                qty <= reduceQty)
            {
                askBook_[price].erase(it);
                if (askBook_[price].empty())
                {
                    askBook_.erase(price);
                }
                ordersMap_.erase(orderId);
            }
            else
            {
                qty -= reduceQty;
            }
            break;
        }
    }
}

void fb::OrderBook::reduceBid_(const uint64_t orderId,
                              const int64_t  price,
                              const int64_t  reduceQty)
{
    if (bidBook_.contains(price) == false)
    {
        return;
    }

    for (auto it = bidBook_[price].begin(); it != bidBook_[price].end(); ++it)
    {
        if (*it == orderId)
        {
            if (int64_t& qty = ordersMap_[orderId].quantity;
                qty <= reduceQty)
            {
                bidBook_[price].erase(it);
                if (bidBook_[price].empty())
                {
                    bidBook_.erase(price);
                }
                ordersMap_.erase(orderId);
            }
            else
            {
                qty -= reduceQty;
            }
            break;
        }
    }
}

void fb::OrderBook::addOrder(const fb::Order& order)
{
    ordersMap_[order.id] = order;
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
    const fb::Order& order = ordersMap_.at(bidBook_.begin()->second.front());
    return fb::OrderView { .id = order.id, .price = order.price, .quantity = order.quantity };
}

std::optional<fb::OrderView> fb::OrderBook::getBestAskOrder() const
{
    if (askBook_.empty())
    {
        return std::nullopt;
    }
    const fb::Order& order = ordersMap_.at(askBook_.begin()->second.front());
    return fb::OrderView { .id = order.id, .price = order.price, .quantity = order.quantity };
}