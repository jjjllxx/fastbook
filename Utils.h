#pragma once

#include <cstdint>

namespace fb
{
namespace Utils
{
    uint64_t unixNowMS();
    uint64_t steadyNowNS();
}
} // namespace fb