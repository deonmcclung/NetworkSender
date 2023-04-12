/**
 * @brief Common socket code for network communication
 *
 * @file Socket.h
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>

// Using .h version of the include here because cstdint requires std:: prefixes
// on all of the types, which is cumbersome.
#include <stdint.h>

#include <string>
#include <chrono>
#include <optional>


namespace Common
{
    /**
     * @brief A class to abstract socket function in a more C++ friendly way.
     */
    class Socket
    {
        Socket(const Socket&) = delete;
        Socket& operator =(const Socket&) = delete;

    public: // Definitions
        class Exception;
        class ConnectionRefusalException;

        /// This is the default maximum connections to queue for the socket 
        static constexpr int DEFAULT_BACKLOG = 10;       

    public: // Methods

        /// @brief Construct a Socket
        /// @param[in] ipAddr   - The IPv4 address to use for the socket
        /// @param[in] port     - The port to use for the socket
        /// @throws Socket::Exception on failure
        explicit Socket(const std::string& ipAddr, uint16_t port);

        /// @brief Move construction is supported
        Socket(Socket&& rhs) noexcept;

        /// @brief Move assignment is supported
        Socket& operator =(Socket&& rhs) noexcept;

        virtual ~Socket();

        /// @brief Bind the socket to the address and port
        /// @throws Socket::Exception on failure
        void bind();

        /// @brief Set the socket to listen mode
        /// @param[in] backlog  - The maximum number of connections to queue on the socket
        /// @throws Socket::Exception on failure
        void listen(int backlog = DEFAULT_BACKLOG);

        /// @brief Accept a connection from the listening queue
        /// @return A connected Socket object if successful, or an empty result if
        ///           the socket was terminated.
        /// @throws Socket::Exception on failure
        std::optional<Socket> accept();

        /// @brief Connect to a listening socket
        /// @throws Socket::ConnectionException on refusal
        /// @throws Socket::Exception on failure
        void connect();

        /// @brief Determine whether the socket is connected
        /// @return True if the socket is in the connected state; otherwise false.
        bool isConnected() const noexcept;

        /// @brief Write some bytes to the socket
        /// @param[in] buffer   - A pointer to the buffer to write, should be at least 'len' bytes.
        /// @param[in] len      - The length of the buffer pointed to by 'buffer', in bytes.
        /// @throws Socket::Exception on failure
        /// @details Note: This is a blocking operation (which may be relevant if 'len' is large).
        void send(const void* buffer, size_t len);

        /// @brief Read data from the socket and place it in a buffer
        /// @param[out] buffer  - A pointer to the buffer to receive the data, should be at
        ///                         least 'len' bytes
        /// @param[in]  len     - The length of the buffer pointed to by 'buffer', in bytes.
        /// @return The number of bytes read, or unset if disconnected.
        /// @throws Socket::Exception on failure
        /// @details Note: This function blocks while waiting for desired length of data.
        std::optional<size_t> recv(void* buffer, size_t len);

    private: // Definitions
        enum class State
        {
            Created,
            Bound,
            Listening,
            Connected,
            Destroyed,
        };

    private: // Methods
        Socket(const sockaddr_in& addr, int socketFd);

    private: // Members
        int                 mSocket{-1};
        std::string         mAddr;
        uint16_t            mPort{0};
        State               mState{State::Created};
        struct sockaddr_in  mSockAddrIn;

    }; // class Socket


    //-----------------------------------------------------------------------------
    inline bool Socket::isConnected() const noexcept
    {
        return mState == State::Connected;
    }

} // namespace Common
