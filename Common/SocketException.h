/**
 * @brief Exceptions for the Socket class
 *
 * @file SocketException.h
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

#pragma once

#include "Socket.h"

#include <exception>
#include <sstream>

namespace Common
{
    /**
     * @brief General Socket exception class
     */
    class Socket::Exception : public std::exception
    {
    public: // Methods

        /**
         * @brief Create an Exception object
         * @param[in] ipAddr         - The IPv4 address desired for the socket
         * @param[in] port           - The port for the socket (if 0, it is omitted in the description)
         * @param[in] errorStr       - A descriptive error message
         */
        Exception(const std::string& ipAddr, uint16_t port, const std::string& errorStr)
        {
            std::ostringstream str;
            str << "Error on socket: " << ipAddr;
            if (port > 0)
            {
                str << ", port: " << port;
            }

            str << " - " << errorStr;
            mMessage = str.str();
        }

        /**
         * @brief Create an Exception object
         * @param[in] ipAddr         - The IPv4 address desired for the socket
         * @param[in] errorStr       - A descriptive error message
         */
        Exception(const std::string& ipAddr, const std::string& errorStr)
            : Exception(ipAddr, 0, errorStr)
        {
        }

        /**
         * @brief Override of std::exception::what
         */
        virtual const char* what() const noexcept override
        {
            return mMessage.c_str();
        }

    private: // Members

        std::string mMessage;                  ///< This is required because we must return a non-static string as a const char*

    }; // Socket::Exception

    /**
     * @brief Specialized socket exception for connection refusal
     */
    class Socket::ConnectionRefusalException : public Exception
    {
    public: // Methods
        /**
         * @brief Create an ConnectionRefusalException object
         * @param[in] ipAddr         - The IPv4 address desired for the socket
         * @param[in] port           - The port for the socket (if 0, it is omitted in the description)
         */
        ConnectionRefusalException(const std::string& ipAddr, uint16_t port)
            : Exception(ipAddr, port, "Connection Refused")
        {
        }
    };

} // namespace Common
