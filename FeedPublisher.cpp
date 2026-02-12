#include "FeedPublisher.h"
#include "Engine/MatchingEngine.h"
#include "Engine/OrderBook.h"
#include "Message.h"
#include "MessageSender.h"
#include "Utils.h"

void fb::FeedPublisher::setSender(fb::MessageSender* sender)
{
    sender_ = sender;
}

void fb::FeedPublisher::onTrade(const fb::Trade& trade)
{
    if (sender_ == nullptr)
    {
        return;
    }

    const fb::Header hdr {
        .type      = fb::MessageType::Trade,
        .length    = sizeof(fb::TradeMessage),
        .seq       = ++seq_,
        .timestamp = fb::Utils::unixNowMS()
    };

    const fb::TradeMessage tm {
        .hdr         = hdr,
        .tradeId     = trade.id,
        .buyOrderId  = trade.buyOrderId,
        .sellOrderId = trade.sellOrderId,
        .price       = trade.price,
        .quantity    = trade.quantity
    };

    ++tradeCnt_;
    sender_->send(&tm, tm.hdr.length);
}

void fb::FeedPublisher::onBookUpdate(const fb::OrderBook& ob)
{
    if (sender_ == nullptr)
    {
        return;
    }

    const std::optional<fb::OrderView> bestAsk = ob.getBestAskOrder();
    const std::optional<fb::OrderView> bestBid = ob.getBestBidOrder();

    if (bestAsk.has_value() && bestBid.has_value())
    {
        const fb::Header hdr {
            .type      = fb::MessageType::Book,
            .length    = sizeof(fb::BookMessage),
            .seq       = ++seq_,
            .timestamp = fb::Utils::unixNowMS()
        };

        const fb::BookMessage bm {
            .hdr          = hdr,
            .bestBidPrice = bestBid->price,
            .bestBidQty   = bestBid->quantity,
            .bestAskPrice = bestAsk->price,
            .bestAskQty   = bestAsk->quantity
        };

        ++bookCnt_;
        sender_->send(&bm, bm.hdr.length);
    }
}

std::size_t fb::FeedPublisher::bookCount() const
{
    return bookCnt_;
}

std::size_t fb::FeedPublisher::tradeCount() const
{
    return tradeCnt_;
}