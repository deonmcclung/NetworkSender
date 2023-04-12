/**
 * @brief Receiver program entry
 *
 * @file main.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved.
 */

#include "Receiver.h"

#include "Common/CommonData.h"

#include <iostream>
#include <thread>

void printBuffer(const void* buffer, size_t size)
{
    std::cout.write(static_cast<const char*>(buffer), size);
}

int main(int argc, const char* const* argv)
{
    try
    {
        Receiver receiver;

        receiver.execute(SERVER_ADDR, SERVER_PORT, printBuffer);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
