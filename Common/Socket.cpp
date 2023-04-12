/**
 * @brief Common socket code for network communication
 *
 * @file Socket.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved.
 */

#include "Socket.h"

#include "SocketException.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include <cassert>
#include <iostream>


namespace Common
{

//-----------------------------------------------------------------------------
Socket::Socket(const std::string& ipAddr, uint16_t port)
    : mAddr(ipAddr)
    , mPort(port)
{
    std::memset(&mSockAddrIn, 0, sizeof(mSockAddrIn));

    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0)
    {
        std::ostringstream str;
        str << "Failure to create the socket object: " << std::strerror(errno);
        throw Exception(ipAddr, str.str());
    }

    // Check the address for validity here, for early failure.
    
    mSockAddrIn.sin_family = AF_INET;
    if (inet_aton(mAddr.c_str(), &mSockAddrIn.sin_addr) == 0)
    {
        std::ostringstream str;
        str << "Invalid IPv4 address";
        throw Exception(mAddr, str.str());
    }

    mSockAddrIn.sin_port = htons(mPort);
}

//-----------------------------------------------------------------------------
Socket::Socket(Socket&& rhs) noexcept
{
    *this = std::move(rhs);
}

//-----------------------------------------------------------------------------
Socket& Socket::operator =(Socket&& rhs) noexcept
{
    mSocket = rhs.mSocket;
    rhs.mSocket = -1;       // The resource is moved
    
    mAddr = std::move(rhs.mAddr);
    mPort = std::move(rhs.mPort);
    mState = std::move(rhs.mState);
    mSockAddrIn = std::move(rhs.mSockAddrIn);

    return *this;
}


//-----------------------------------------------------------------------------
Socket::~Socket()
{
    if (mSocket != -1)
    {
        // If we haven't moved this socket...

        // Close the socket handle
        close(mSocket);
    }

    mSocket = -1;
    mState = State::Destroyed;
}

//-----------------------------------------------------------------------------
void Socket::bind()
{
    if (mState != State::Created)
    {
        throw Exception(mAddr, mPort, "The Socket must be in a created state to bind.");
    }

    // Bind the address and port to the socket
    if (::bind(mSocket, reinterpret_cast<struct sockaddr*>(&mSockAddrIn), sizeof(mSockAddrIn)) < 0)
    {
        std::ostringstream str;
        str << "Failure to bind: " << std::strerror(errno);
        throw Exception(mAddr, mPort, str.str());
    }

    mState = State::Bound;
}

//-----------------------------------------------------------------------------
void Socket::listen(int backlog)
{
    if (mState != State::Bound)
    {
        throw Exception(mAddr, mPort, "The Socket must be in a bound state to enter listen mode.");
    }

    // Put the socket into listening mode.
    if (::listen(mSocket, backlog) < 0)
    {
        throw Exception(mAddr, mPort, std::strerror(errno));
    }

    // Reflect that the socket is in listen mode
    mState = State::Listening;
}

//-----------------------------------------------------------------------------
std::optional<Socket> Socket::accept()
{
    if (mState != State::Listening)
    {
        throw Exception(mAddr, mPort, "The Socket must be in a listening state to accept connections");
    }

    std::optional<Socket> result;

    // Create a new address instance to receive the connection.
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    auto acceptResult = ::accept(mSocket, reinterpret_cast<sockaddr*>(&addr), &len);
    if (acceptResult < 0)
    {
        // On error...

        // If the connection was aborted, we are shutting down. This is not an error state.
        if (errno != ECONNABORTED)
        {
            std::ostringstream str;
            str <<  "Error while attempting to connect the socket: " << std::strerror(errno);
            throw Exception(mAddr, mPort, str.str());
        }

        // ...fall out to the function exit with result unset.
    }
    else
    {
        // On success...
       
        // Initialize the result socket with the connection information.
        result = Socket(addr, acceptResult);
    }

    return result;
}

//-----------------------------------------------------------------------------
void Socket::connect()
{
    if (mState != State::Created)
    {
        throw Exception(mAddr, mPort, "The Socket must be in a Created state in order to form a connection.");
    }

    if (::connect(mSocket, reinterpret_cast<sockaddr*>(&mSockAddrIn), sizeof(mSockAddrIn)) < 0)
    {
        if (errno == ECONNREFUSED)
        {
            // Throw a little addition type information around this condition
            throw ConnectionRefusalException(mAddr, mPort);
        }
        else
        {
            std::ostringstream str;
            str << "Failure to connect: " << std::strerror(errno);
            throw Exception(mAddr, mPort, str.str());
        }
    }
    else
    {
        // On success...
        
        // Set the state to connected
        mState = State::Connected;
    }
}

//-----------------------------------------------------------------------------
void Socket::send(const void* buffer, size_t len)
{
    if (mState != State::Connected)
    {
        throw Exception(mAddr, mPort, "The Socket must be in a connected state to write.");
    }

    if (::send(mSocket, buffer, len, 0) < 0)
    {
        std::ostringstream str;
        str << "Error while writing: " << std::strerror(errno);
        throw Exception(mAddr, mPort, str.str());
    }
}

//-----------------------------------------------------------------------------
std::optional<size_t> Socket::recv(void* buffer, size_t len)
{
    if (mState != State::Connected)
    {
        throw Exception(mAddr, mPort, "The Socket must be in a connected state in order to receive data.");
    }

    std::optional<size_t> result;

    auto readResult = ::recv(mSocket, buffer, len, 0);
    if (readResult <= 0)
    {
        // On failure...
        
        if (readResult == 0 || errno == ECONNABORTED)
        {
            // Fall out with 'result' unset as part of disconnection logic...
        }
        else
        {
            // Other errors...
            std::ostringstream str;
            str << "Failure while reading: " << std::strerror(errno);
            throw Exception(mAddr, mPort, str.str());
        }
    }
    else
    {
        // On success...
        result = static_cast<size_t>(readResult);
    }

    return result;
}

//-----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

/// @internal
/// @brief Construct a Socket based on information from an accepted connection
/// @param[in] addr     - A sockaddr_in with information about the connection
/// @param[in] socketFd - The file descriptor of the connected socket
Socket::Socket(const sockaddr_in& addr, int socketFd)
    : mSocket(socketFd)
    , mSockAddrIn(addr)
    , mState(State::Connected)
{
    mPort = ntohs(mSockAddrIn.sin_addr.s_addr);
    mAddr = inet_ntoa(mSockAddrIn.sin_addr);
}


} // namespace Common

