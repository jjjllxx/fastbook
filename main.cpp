#include "Engine/MatchingEngine.h"
#include "Feed/FeedPublisher.h"
#include "Feed/MessageSender.h"
#include "Utils.h"
#include <iostream>

int main()
{
    fb::MatchingEngine me;
    fb::FeedPublisher  fb;
    fb::MessageSender  ms;

    fb.setSender(&ms);
    me.setPublisher(&fb);

    const fb::Order S100AT10 { .side = fb::Side::Sell, .id = 1, .price = 10, .quantity = 100, .timestamp = 1 };
    const fb::Order S50AT11 { .side = fb::Side::Sell, .id = 2, .price = 11, .quantity = 50, .timestamp = 2 };
    const fb::Order B120AT10 { .side = fb::Side::Buy, .id = 11, .price = 10, .quantity = 120, .timestamp = 101 };

    me.onNewOrder(S100AT10);
    me.onNewOrder(S50AT11);
    me.onNewOrder(B120AT10);


    std::cout << fb::Utils::unixNowMS() << '\n';
    return 0;
}