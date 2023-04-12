/**
 * @brief A class to handle receiving data over a socket.
 *
 * @file Receiver.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved.
 */

#include "Receiver.h"

// Project headers
#include "Common/Socket.h"

// Standard headers
#include <thread>
#include <array>
#include <iostream>
#include <list>


//-----------------------------------------------------------------------------
void Receiver::execute(const std::string& addr, uint16_t port, Handler handler)
{
    Common::Socket listenSocket(addr, port);
    listenSocket.bind();
    listenSocket.listen();
    
    std::list<std::thread> threads;

    for (;;)
    {
        auto recvSocket = listenSocket.accept();
        if (recvSocket)
        {
            // Fire off a worker thread to handle the connection

            auto data = std::make_unique<ConnThreadData>(
                std::move(recvSocket.value()),
                handler
            );

            // Ownership of 'data' is transferred to the thread.
            threads.push_back(std::thread(_connectionThread, std::move(data)));
        }
        else
        {
            break;
        }
    }

    // Join all of the threads before exiting.
    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}

//-----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

/// @internal
/// @brief Thread to process the data coming in over the connected socket
/// @param[in] data     - A reference to an object containing information for the thread.
void Receiver::_connectionThread(std::unique_ptr<ConnThreadData> data)
{
    auto& recvSocket = data->recvSocket;
    auto& handler = data->handler;

    try
    {
        for (;;)
        {
            constexpr size_t BUFFER_SIZE = 1024;
            std::array<char, BUFFER_SIZE> buffer;

            auto received = recvSocket.recv(buffer.data(), buffer.size());
            if (!received)
            {
                break;
            }

            handler(buffer.data(), received.value());
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

