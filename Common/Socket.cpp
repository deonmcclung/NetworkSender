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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include <cassert>

namespace Common
{

//-----------------------------------------------------------------------------
Socket::Socket(const std::string& ipAddr)
    : mAddr(ipAddr)
{
   mSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (mSocket < 0)
   {
       std::ostringstream str;
       str << "Failure to create the socket object: " << std::strerror(errno);
       throw Exception(ipAddr, str.str());
   }

   // Check the address for validity here, for early failure.
   
    struct sockaddr_in addr = {0};
    if (inet_aton(mAddr.c_str(), &addr.sin_addr) == 0)
    {
        std::ostringstream str;
        str << "Invalid IPv4 address: " << ipAddr;
        throw Exception(ipAddr, str.str());
    }
}

//-----------------------------------------------------------------------------
Socket::~Socket()
{
    // Close the socket handle
    close(mSocket);

    // Even though this object is going out of existence, explicitely set this
    // value to invalid to ensure a stray pointer does not try to access it.
    mSocket = -1;
}

//-----------------------------------------------------------------------------
void Socket::bind(uint16_t port)
{
    // Format the address information to the sockaddr_in structure.
    // Validity of mAddr and the results of inet_aton should have been verified
    // in the constructor. Verified here with an assert.
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    auto inet_result = inet_aton(mAddr.c_str(), &addr.sin_addr);
    assert(inet_result != 0);

    // Bind the address and port to the socket
    if (::bind(mSocket, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        throw Exception(mAddr, port, std::strerror(errno));
    }

    // Assign the port member to non-zero as part of indicating a successfully bound socket
    mPort = port;
}

//-----------------------------------------------------------------------------
void Socket::listen(int backlog)
{
    // Put the socket into listening mode.
    if (::listen(mSocket, backlog) < 0)
    {
        throw Exception(mAddr, mPort, std::strerror(errno));
    }

    // Reflect that the socket is in listen mode
    mListening = true;
}

//-----------------------------------------------------------------------------
std::optional<Socket> Socket::accept()
{
    // First, ensure that we are bound and listening. The accept would fail anyway,
    // but these checks may be more clear as to why. This would be considered a logic failure.
    if (mPort == 0)
    {
        throw Exception(mAddr, "The socket must first be bound to a port before calling accept()");
    }

    if (!mListening)
    {
        throw Exception(mAddr, mPort, "The socket must be in listening mode before calling accept()");
    }

    std::optional<Socket> result;

    struct sockaddr_in addr = {0};
    socklen_t len = sizeof(addr);
    if (::accept(mSocket, reinterpret_cast<sockaddr*>(&addr), &len) < 0)
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
        
        mAddr = inet_ntoa(addr.sin_addr);

        result = Socket(mAddr);
        result.value().mPort = ntohs(addr.sin_port);
    }

    return result;
}

//-----------------------------------------------------------------------------
void Socket::write(const void* buffer, size_t len)
{
    // Check for logic failure
    if (mListening)
    {
        throw Exception(mAddr, mPort, "Cannot write to this socket because it is in listen mode.");
    }

    if (mPort == 0)
    {
        throw Exception(mAddr, "Cannot write to this socket because it is either unbound or disconnected.");
    }

    if (::write(mSocket, buffer, len) < 0)
    {
        std::ostringstream str;
        str << "Error while writing: " << std::strerror(errno);
        throw Exception(mAddr, mPort, str.str());
    }
}

//-----------------------------------------------------------------------------
std::optional<size_t> Socket::read(void* buffer, size_t len)
{
    // Check for logic failure
    if (mListening)
    {
        throw Exception(mAddr, mPort, "Cannot write to this socket because it is in listen mode.");
    }

    if (mPort == 0)
    {
        throw Exception(mAddr, "Cannot write to this socket because it is either unbound or disconnected.");
    }

    std::optional<size_t> result;

    auto readResult = ::read(mSocket, buffer, len);
    if (readResult == static_cast<ssize_t>(len))
    {
        // On success...
        result = static_cast<size_t>(readResult);
    }
    // Else, interpret the failure...
    else if (readResult < 0)
    {
        if (errno == ECONNABORTED)
        {
            // Fall out unset as part of disconnection logic...
        }
        else
        {
            // Other errors...
            std::ostringstream str;
            str << "Failure while reading: " << std::strerror(errno);
            throw Exception(mAddr, mPort, str.str());
        }
    }

    return result;
}

} // namespace Common

