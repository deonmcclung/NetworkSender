/**
 * @brief A class to handle sending data over a socket.
 *
 * @file Sender.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

// Source header
#include "Sender.h"

// Project headers
#include "Common/Socket.h"
#include "Common/SocketException.h"

// Standard headers
#include <cstring>
#include <cmath>
#include <iostream>
#include <array>
#include <thread>
#include <chrono>

using namespace std::literals::chrono_literals;


//-----------------------------------------------------------------------------
Sender::Sender(const std::string& addr, uint16_t port)
    : mSocket{addr, port}
{
}


//-----------------------------------------------------------------------------
Sender::~Sender() = default;


//-----------------------------------------------------------------------------
void Sender::connect(int retries)
{
    // Add one to the number of "retries" to get our total number of attempts
    for (int retry = 0; retry < retries+1; ++retry)
    {
        try
        {
            mSocket.connect();
            break;
        }
        catch (const Common::Socket::ConnectionRefusalException& e)
        {
            std::cout << "Cannot connect to server. Retrying..." << std::endl;

            // Wait a second before trying again
            std::this_thread::sleep_for(1s);
        }
    }

    if (!mSocket.isConnected())
    {
        throw Exception("Failed to connect to server.");
    }
}


//-----------------------------------------------------------------------------
Sender::CommandLineData Sender::parseCommandLine(int argc, const char* const* argv)
{
    CommandLineData data;

    for (int input = 1; input < argc; ++input)
    {
        if (std::strcmp(argv[input], "-") == 0)
        {
            data.readStdin = true;

            // No files after '-'
            break;
        }
        else
        {
            data.filesToSend.emplace_back(argv[input]);
        }
    }

    return data;
}


//-----------------------------------------------------------------------------
void Sender::sendStream(std::istream& input)
{
    if (!mSocket.isConnected())
    {
        throw Exception("Socket is not connected.");
    }

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
        mSocket.send(line.data(), dataToSend);
    }
}
