/**
 * @brief A class to handle sending data over a socket.
 *
 * @file Sender.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

// Source Header
#include "Sender.h"

// Project Headers
#include "Common/Socket.h"
#include "Common/SocketException.h"
#include "Common/CommonData.h"

// Standard Headers
#include <fstream>
#include <array>
#include <cstring>
#include <cmath>
#include <thread>
#include <chrono>

using namespace std::literals::chrono_literals;


//-----------------------------------------------------------------------------
Sender::Sender() = default;


//-----------------------------------------------------------------------------
Sender::~Sender() = default;


//-----------------------------------------------------------------------------
void Sender::execute(int argc, const char* const* argv)
{
    constexpr int RETRIES = 4;

    Common::Socket senderSock{SERVER_ADDR, SERVER_PORT};

    _connectWithRetries(senderSock, RETRIES);

    if (!senderSock.isConnected())
    {
        throw Exception("Failed to connect.");
    }
    else
    {
        _sendInput(senderSock, argc, argv);
    }
}

/**
 * @internal
 * @brief Send the given input over the socket, one line at a time
 * @param[in] senderSock        The socket over which to send the data
 * @param[in] input             The stream to send over the socket
 */
void Sender::_sendStream(Common::Socket& senderSock, std::istream& input)
{
    constexpr size_t BUFFER_SIZE = 1024;
    std::array<char, BUFFER_SIZE> line;
    // Subtract 1 to make room for a newline
    while (input.getline(line.data(), line.size() - 1))
    {
        // Note: line is NOT terminated

        // Add one more character in the send count for the newline
        auto dataToSend = std::min(static_cast<size_t>(input.gcount()), line.size() - 1) + 1;
        line[dataToSend - 1] = '\n';         // Replace the newline as the last character

        // Send it over the connection
        senderSock.send(line.data(), dataToSend);
    }
}


/**
 * @internal
 * @brief Attempt to connect the socket with retries.
 * @param[in] senderSock        The socket over which to send the data
 * @param[in] retries           The number of times to retry to connect before failure.
 * @throws std::exception if complete failure to connect
 */
void Sender::_connectWithRetries(Common::Socket& senderSock, int retries)
{
    // Add one to the number of "retries" to get our total number of attempts
    for (int retry = 0; retry < retries+1; ++retry)
    {
        try
        {
            senderSock.connect();
            break;
        }
        catch (const Common::Socket::ConnectionRefusalException& e)
        {
            std::cout << "Cannot connect to server. Retrying..." << std::endl;

            // Wait a second before trying again
            std::this_thread::sleep_for(1s);
        }
    }
}

//-----------------------------------------------------------------------------
void Sender::_sendInput(Common::Socket& senderSock, int argc, const char* const* argv)
{
    for (int input = 1; input < argc; ++input)
    {
        // Send either a file or stdin

        if (std::strcmp(argv[input], "-") == 0)
        {
            // Just send stdin
            _sendStream(senderSock, std::cin);

            // No files after '-'
            break;
        }
        else
        {
            // Send the passed file
            std::ifstream inputFile(argv[input]);
            _sendStream(senderSock, inputFile);
        }
    }
}
