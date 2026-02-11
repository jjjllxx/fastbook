#pragma once

#include <cstdint>

namespace fb
{
enum class MessageType : uint8_t
{
    Trade = 1,
    Book  = 2
};

struct Header
{
    fb::MessageType type;

    uint16_t length;
    uint64_t seq;
    uint64_t timestamp;
};

struct TradeMessage
{
    fb::Header hdr;

    uint64_t tradeId;
    uint64_t buyOrderId;
    uint64_t sellOrderId;
    int64_t  price;
    int64_t  quantity;
};

struct BookMessage
{
    fb::Header hdr;

    int64_t bestBidPrice;
    int64_t bestBidQty;
    int64_t bestAskPrice;
    int64_t bestAskQty;
};
} // namespace fb