/**
 * @brief Sender program entry
 *
 * @file main.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

// Project headers
#include "Sender.h"
#include "Common/CommonData.h"

// Standard headers
#include <exception>
#include <fstream>

//-----------------------------------------------------------------------------
int main(int argc, const char* const* argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: sender [<filename_to_send>] [-]" << std::endl;
        return 1;
    }

    int result = 1;

    try
    {
        Sender sender{SERVER_ADDR, SERVER_PORT};

        auto data = sender.parseCommandLine(argc, argv);

        sender.connect();

        // Send requested files
        for (const auto& file : data.filesToSend)
        {
            std::ifstream inputFile(file);
            sender.sendStream(inputFile);
        }

        if (data.readStdin)
        {
            // If requested to read stdin...
            sender.sendStream(std::cin);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return result;
}


