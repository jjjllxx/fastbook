#include <benchmark/benchmark.h>

#include "Engine/MatchingEngine.h"
#include "Engine/Order.h"
#include "Feed/FeedPublisher.h"
#include "Feed/MessageSender.h"

#include <iostream>

static void BM_MatchingEngine_OnNewOrder(benchmark::State& state)
{
    const int64_t          n = state.range(0);
    std::vector<fb::Order> orders;
    orders.reserve(n);
    for (int64_t i = 0; i < n; ++i)
    {
        fb::Order o {};
        o.id        = i + 1;
        o.timestamp = i;
        o.quantity  = 100;
        o.price     = 100 + (i % 10);
        o.side      = i % 2 == 0 ? fb::Side::Buy : fb::Side::Sell;
        orders.push_back(o);
    }

    for (auto _ : state)
    {
        fb::MatchingEngine engine;
        std::size_t        tradeCnt = 0;

        for (const fb::Order& o : orders)
        {
            tradeCnt += engine.onNewOrder(o).size();
        }

        benchmark::DoNotOptimize(tradeCnt);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * n);
}

BENCHMARK(BM_MatchingEngine_OnNewOrder)
    ->Arg(10'000)
    ->Arg(50'000)
    ->Arg(200'000);

static void BM_MatchingEngine_OnNewOrder_WithFeed(benchmark::State& state)
{
    const int64_t          n = state.range(0);
    std::vector<fb::Order> orders;
    orders.reserve(n);
    for (int64_t i = 0; i < n; ++i)
    {
        fb::Order o {};
        o.id        = i + 1;
        o.timestamp = i;
        o.quantity  = 100;
        o.price     = 100 + (i % 10);
        o.side      = i % 2 == 0 ? fb::Side::Buy : fb::Side::Sell;
        orders.push_back(o);
    }

    fb::FeedPublisher pub;
    fb::MessageSender sender;
    pub.setSender(&sender);

    std::size_t tradeCnt = 0;

    for (auto _ : state)
    {
        fb::MatchingEngine engine;

        engine.setPublisher(&pub);

        for (const fb::Order& o : orders)
        {
            tradeCnt += engine.onNewOrder(o).size();
        }

        benchmark::DoNotOptimize(tradeCnt);
        benchmark::ClobberMemory();
    }

    if (state.thread_index() == 0)
    {
        std::cout
            << "orders=" << (state.iterations() * n)
            << " trades=" << tradeCnt
            << " trade_msgs=" << pub.tradeCount()
            << " book_msgs=" << pub.bookCount()
            << " packets=" << sender.successTimes()
            << " send_fail=" << sender.failureTimes()
            << "\n";
    }

    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * n);
}

BENCHMARK(BM_MatchingEngine_OnNewOrder_WithFeed)
    ->Arg(10'000)
    ->Arg(50'000)
    ->Arg(200'000);