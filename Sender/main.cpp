/**
 * @brief Sender program entry
 *
 * @file main.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved.
 */

#include "Common/Socket.h"

#include <iostream>
#include <fstream>
#include <array>
#include <cstring>
#include <cmath>


void sendStream(Common::Socket& senderSock, std::istream& input)
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

int main(int argc, const char* const* argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: sender [<filename_to_send>]" << std::endl;
        std::cout << "       sender -" << std::endl;
        return 1;
    }

    try
    {
        Common::Socket senderSock{"127.0.0.1", 56743};

        senderSock.connect();
       
        if (argc > 1)
        {
            if (std::strcmp(argv[1], "-") == 0)
            {
                // Just send stdin
                sendStream(senderSock, std::cin);
            }
            else
            {
                // Send the passed file
                std::ifstream inputFile(argv[1]);
                sendStream(senderSock, inputFile);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}