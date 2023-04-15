/**
 * @brief Mock for the Socket class
 *
 * @file SocketMock.h
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

#pragma once

#include "Common/Socket.h"

#include <gmock/gmock.h>

#include "MockVendor/MockVendor.h"

namespace Common
{

class SocketMock
{
public:
    MOCK_METHOD(void, bind, ());
    MOCK_METHOD(void, listen, (int backlog));
    MOCK_METHOD(std::optional<Socket>, accept, ());
    MOCK_METHOD(void, connect, ());
    MOCK_METHOD(bool, isConnected, (), (const));
    MOCK_METHOD(void, send, (const void* buffer, size_t len));
    MOCK_METHOD(std::optional<size_t>, recv, (void* buffer, size_t len));
};

using SocketMockVendor = MockVendor<testing::NiceMock<SocketMock>, Socket>;

Socket::Socket(const std::string& ipAddr, uint16_t port)
{
    SocketMockVendor::vend(this);
}

Socket::Socket(Socket&& rhs) noexcept
{
    SocketMockVendor::move(this, &rhs);
}

Socket& Socket::operator =(Socket&& rhs) noexcept
{
    SocketMockVendor::move(this, &rhs);
    return *this;
}

Socket::~Socket()
{
    SocketMockVendor::destroy(this);
}

void Socket::bind()
{
    return SocketMockVendor::mock(this)->bind();
}

void Socket::listen(int backlog)
{
    return SocketMockVendor::mock(this)->listen(backlog);
}

std::optional<Socket> Socket::accept()
{
    return SocketMockVendor::mock(this)->accept();
}

void Socket::connect()
{
    return SocketMockVendor::mock(this)->connect();
}

bool Socket::isConnected() const noexcept
{
    return SocketMockVendor::mock(this)->isConnected();
}

void Socket::send(const void* buffer, size_t len)
{
    return SocketMockVendor::mock(this)->send(buffer, len);
}

std::optional<size_t> Socket::recv(void* buffer, size_t len)
{
    return SocketMockVendor::mock(this)->recv(buffer, len);
}

} // namespace Common
