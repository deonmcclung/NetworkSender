/**
 * @brief A class to handle sending data over a socket.
 *
 * @file Sender.h
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

#pragma once

// Project Headers
#include "Common/Socket.h"

// Standard Headers
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <stdint.h>


/**
 * @brief A class to handle basic sending of streams over a socket
 */
class Sender
{
    Sender(const Sender&) = delete;
    Sender& operator =(const Sender&) = delete;

public: // Definitions
    class Exception;
    struct CommandLineData;

    static constexpr int DEFAULT_RETRIES = 4;

public: // Methods

    /**
     * @brief Construct a Sender object
     * @param[in] addr      The address of the server
     * @param[in] port      The port of the server
     */
    Sender(const std::string& addr, uint16_t port);

    virtual ~Sender();

    /**
     * @brief Connect the sender to the server
     * @param[in] retries   The number of times to retry for a connection if initially unsuccessful (default: 4)
     * @throws Exeception if ultimately unsuccessful
     */
    void connect(int retries = DEFAULT_RETRIES);

    /**
     * @brief Parse the command line
     * @param[in] argc      The command line argc
     * @param[in] argv      The command line argv
     */
    CommandLineData parseCommandLine(int argc, const char* const* argv);

    /**
     * @brief Send the given input over the socket, one line at a time
     * @param[in] input             The stream to send over the socket
     * @throws Exception upon failure
     */
    void sendStream(std::istream& input);

private: // Members
    Common::Socket      mSocket;

}; // class Sender


/**
 * @brief Execeptions on the Sender class
 */
class Sender::Exception : public std::exception
{
public:
    Exception(const std::string& message)
        : mMessage(message)
    {
    }

    virtual ~Exception() = default;

    virtual const char* what() const noexcept override
    {
        return mMessage.c_str();
    }

private:
    std::string     mMessage;

}; // class Sender::Exception

struct Sender::CommandLineData
{
    std::vector<std::string>    filesToSend;
    bool                        readStdin{false};
};
