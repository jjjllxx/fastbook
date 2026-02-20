#include "MessageSender.h"

#include <stdexcept>
#include <unistd.h>

fb::MessageSender::MessageSender()
{
    if (sockfd_ < 0)
    {
        throw std::runtime_error("Failed to create sockect!");
    }

    addr_.sin_family = AF_INET;
    addr_.sin_port   = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &addr_.sin_addr);
}

fb::MessageSender::~MessageSender()
{
    close(sockfd_);
}

bool fb::MessageSender::send(const void*       data,
                             const std::size_t len)
{
    if (sendto(sockfd_, data, len, 0, (sockaddr*) &addr_, sizeof(addr_)) < 0) // NOLINT
    {
        ++failureTimes_;
        return false;
    }

    ++successTimes_;
    return true;
}

std::size_t fb::MessageSender::successTimes() const
{
    return successTimes_;
}

std::size_t fb::MessageSender::failureTimes() const
{
    return failureTimes_;
}