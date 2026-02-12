#pragma once

#include <cstddef>
#include <cstdint>

namespace fb
{
struct Trade;
class OrderBook;
class MessageSender;

class FeedPublisher
{
    uint64_t    seq_      = 0;
    std::size_t bookCnt_  = 0;
    std::size_t tradeCnt_ = 0;

    fb::MessageSender* sender_ = nullptr;
public:
    void setSender(fb::MessageSender* sender);
    void onTrade(const fb::Trade& trade);
    void onBookUpdate(const fb::OrderBook& ob);

    std::size_t bookCount() const;
    std::size_t tradeCount() const;
};
} // namespace fb