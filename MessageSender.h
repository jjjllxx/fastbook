#pragma once

#include <arpa/inet.h>
#include <cstddef>

namespace fb
{
class MessageSender
{
    int         sockfd_ { socket(AF_INET, SOCK_DGRAM, 0) };
    std::size_t successTimes_ = 0;
    std::size_t failureTimes_ = 0;
    sockaddr_in addr_ {};

public:
    MessageSender();
    MessageSender(const fb::MessageSender& other)                = delete;
    fb::MessageSender& operator=(const fb::MessageSender& other) = delete;
    ~MessageSender();

    bool send(const void*       data,
              const std::size_t len);

    std::size_t successTimes() const;
    std::size_t failureTimes() const;
};
} // namespace fb