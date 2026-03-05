#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "Message.h"
#include <cstring>
#include <iostream>

int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in addr {};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(9000);

    if (bind(sockfd, (sockaddr*) &addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(sockfd);
        return 1;
    }

    while (true)
    {
        char    buf[1024];
        ssize_t n = recvfrom(sockfd, buf, sizeof(buf), 0, nullptr, nullptr);

        std::cout << "received " << n << " bytes\n";

        const fb::Header* hdr = reinterpret_cast<const fb::Header*>(buf);

        std::cout << "type = " << static_cast<int>(hdr->type) << '\n';
        std::cout << "length = " << hdr->length << '\n';
        std::cout << "seq = " << hdr->seq << '\n';
        std::cout << "ts = " << hdr->timestamp << '\n';

        if (hdr->type == fb::MessageType::Trade)
        {
            const fb::TradeMessage* tm = reinterpret_cast<const fb::TradeMessage*>(buf);
            std::cout << "tradeId = " << tm->tradeId << '\n';
            std::cout << "buyOrderId = " << tm->buyOrderId << '\n';
            std::cout << "sellOrderId = " << tm->sellOrderId << '\n';
            std::cout << "price = " << tm->price << '\n';
            std::cout << "quantity = " << tm->quantity << '\n';
        }
        else if (hdr->type == fb::MessageType::Book)
        {
            const fb::BookMessage* bm = reinterpret_cast<const fb::BookMessage*>(buf);
            std::cout << "bestBidPrice = " << bm->bestBidPrice << '\n';
            std::cout << "bestBidQty = " << bm->bestBidQty << '\n';
            std::cout << "bestAskPrice = " << bm->bestAskPrice << '\n';
            std::cout << "bestAskQty = " << bm->bestAskQty << '\n';
        }
        else
        {
            std::cerr << "Invalid Message Type!\n";
        }
    }

    close(sockfd);
    return 0;
}
