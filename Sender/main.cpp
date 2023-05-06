/**
 * @brief Sender program entry
 *
 * @file main.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

#include "Sender.h"

#include <exception>

//-----------------------------------------------------------------------------
int main(int argc, const char* const* argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: sender [<filename_to_send>]" << std::endl;
        std::cout << "       sender -" << std::endl;
        return 1;
    }

    int result = 1;

    try
    {
        Sender sender;

        sender.execute(argc, argv);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return result;
}


