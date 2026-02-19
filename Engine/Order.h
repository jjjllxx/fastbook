#pragma once

#include <cstdint>

namespace fb
{
enum class Side : uint8_t
{
    Buy,
    Sell
};

struct Order
{
    Side     side;
    uint64_t id;
    int64_t  price; // use integers to represent prices (e.g., in cents).
    int64_t  quantity;
    uint64_t timestamp; // local timestamp
};

struct OrderView
{
    uint64_t id;
    int64_t  price;
    int64_t  quantity;
};
} // namespace fb