/**
 * @brief Common socket code for network communication
 *
 * @file Socket.h
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved.
 */

#pragma once

// Using .h version of the include here because cstdint requires std:: prefixes
// on all of the types, which is cumbersome.
#include <stdint.h>

#include <string>
#include <chrono>
#include <optional>


namespace Common
{
    class Socket
    {
        Socket(const Socket&) = delete;
        Socket& operator =(const Socket&) = delete;

    public:
        class Exception;

    public:
        /// @brief Construct a Socket
        /// @param[in] ipAddr   - The IPv4 address to use for the socket
        /// @throws Socket::Exception on failure
        explicit Socket(const std::string& ipAddr);

        /// @brief Move construction is supported
        Socket(Socket&& rhs) noexcept = default;

        /// @brief Move assignment is supported
        Socket& operator =(Socket&& rhs) noexcept = default;

        virtual ~Socket();

        /// @brief Bind the socket to the address and port
        /// @param[in] port     - The port to use for the socket
        /// @throws Socket::Exception on failure
        void bind(uint16_t port);

        /// @brief Set the socket to listen mode
        /// @param[in] backlog  - The maximum number of connections to queue on the socket
        /// @throws Socket::Exception on failure
        void listen(int backlog);

        /// @brief Accept a connection from the listening queue
        /// @return A connected Socket object if successful, or an empty result if
        ///           the socket was terminated.
        /// @throws Socket::Exception on failure
        std::optional<Socket> accept();

        /// @brief Write some bytes to the socket
        /// @param[in] buffer   - A pointer to the buffer to write, should be at least 'len' bytes.
        /// @param[in] len      - The length of the buffer pointed to by 'buffer', in bytes.
        /// @throws Socket::Exception on failure
        /// @details Note: This is a blocking operation (which may be relevant if 'len' is large).
        void write(const void* buffer, size_t len);

        /// @brief Read data from the socket and place it in a buffer
        /// @param[out] buffer  - A pointer to the buffer to receive the data, should be at
        ///                         least 'len' bytes
        /// @param[in]  len     - The length of the buffer pointed to by 'buffer', in bytes.
        /// @return The number of bytes read, or unset if disconnected.
        /// @throws Socket::Exception on failure
        /// @details Note: This function blocks while waiting for desired length of data.
        std::optional<size_t> read(void* buffer, size_t len);

    private:
        int                 mSocket{-1};
        std::string         mAddr;
        uint16_t            mPort{0};
        bool                mListening{false};

    }; // class Socket
    
} // namespace Common
