/**
 * @brief A class to handle sending data over a socket.
 *
 * @file Sender.h
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

#pragma once

#include <iostream>
#include <exception>
#include <string>


// Forward declarations
namespace Common
{
    class Socket;
}

/**
 * @brief A class to handle basic sending of streams over a socket
 */
class Sender
{
    Sender(const Sender&) = delete;
    Sender& operator =(const Sender&) = delete;

public: // Definitions
    class Exception;

public: // Methods
    Sender();
    virtual ~Sender();

    /**
     * @brief Execute the sender
     * @param[in] argc      The command line argc
     * @param[in] argv      The command line argv
     */
    void execute(int argc, const char* const* argv);

private: // Methods
    void _sendStream(Common::Socket& senderSock, std::istream& input);
    void _connectWithRetries(Common::Socket& senderSock, int retries);
    void _sendInput(Common::Socket& senderSock, int argc, const char* const* argv);

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
};
