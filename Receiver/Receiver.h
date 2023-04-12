/**
 * @brief A class to handle receiving data over a socket.
 *
 * @file Receiver.h
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

#pragma once

#include "Common/Socket.h"

#include <string>
#include <stdint.h>
#include <functional>
#include <memory>

class Receiver
{
    Receiver(const Receiver&) = delete;
    Receiver& operator =(const Receiver&) = delete;

public: // Definitions
    using Handler = std::function<void(const void* buffer, size_t len)>;

public: // Methods
    Receiver() = default;
    virtual ~Receiver() = default;

    /**
     * @brief Execute the receive operation
     * @param[in] addr      - The IP address on which to listen
     * @param[in] port      - The port on which to listen
     * @param[in] handler   - A handler function to be called repeatedly received data
     */
    void execute(const std::string& addr, uint16_t port, Handler handler);

private: // Definitions
    struct ConnThreadData
    {
        ConnThreadData(Common::Socket&& _recvSocket, const Handler& _handler)
            : recvSocket(std::move(_recvSocket))
            , handler(_handler)
        {
        }

        Common::Socket recvSocket;
        Handler handler;
    };

private: // Methods
    static void _connectionThread(std::unique_ptr<ConnThreadData> handler);
};

