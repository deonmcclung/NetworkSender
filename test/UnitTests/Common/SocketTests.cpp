/**
 * @brief Unit tests for the Socket class
 *
 * @file SocketTests.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

// Fakes
#include "Fakes/socketFake.h"

// Class under test
#include "Common/Socket.cpp"


#include <gtest/gtest.h>
#include <fff/fff.h>

#include <memory>

DEFINE_FFF_GLOBALS

#define RESET_FAKES \
    RESET_SOCKET_FAKES \

class SocketTests : public testing::Test
{
protected: // Definitions
    static constexpr const char* const TEST_IP = "127.0.0.2";           // Valid IP, but not actual
    static constexpr uint16_t TEST_PORT = 51234;

protected: // Methods
    explicit SocketTests(bool noConstruct = false)
    {
        FFF_RESET_HISTORY()
        RESET_FAKES

        if (!noConstruct)
        {
            mTestObj = std::make_unique<Common::Socket>(TEST_IP, TEST_PORT);
        }
    }

    virtual ~SocketTests() = default;

protected: // Members
    std::unique_ptr<Common::Socket> mTestObj;
};

class SocketTestsNC : public SocketTests
{
protected: // Methods
    SocketTestsNC()
        : SocketTests(true)
    {
    }
};

// Test for handling of valid and invalid arguments to the constuctor.
TEST_F(SocketTestsNC, TestConstruct)
{
    EXPECT_NO_THROW(Common::Socket(TEST_IP, TEST_PORT));
    EXPECT_THROW(Common::Socket("345.001.002.003", TEST_PORT), Common::Socket::Exception);
}

// Test that the Socket object initially reflects as disconnected.
TEST_F(SocketTests, TestInitiallyDisconnected)
{
    EXPECT_FALSE(mTestObj->isConnected());
}

TEST_F(SocketTests, TestBind)
{
    // Test
    EXPECT_NO_THROW(mTestObj->bind());

    // Verify
    EXPECT_GT(bind_fake.call_count, 0);
}

TEST_F(SocketTests, TestBindFail)
{
    // Setup
    bind_fake.return_val = -1;

    // Test/Verify
    EXPECT_THROW(mTestObj->bind(), Common::Socket::Exception);
}

TEST_F(SocketTests, TestListenFailNotBound)
{
    // Test
    EXPECT_THROW(mTestObj->listen(), Common::Socket::Exception);

    // Verify
    EXPECT_EQ(listen_fake.call_count, 0);
}

TEST_F(SocketTests, TestListen)
{
    // Setup
    ASSERT_NO_THROW(mTestObj->bind());

    // Test
    EXPECT_NO_THROW(mTestObj->listen());

    // Verify
    EXPECT_GT(listen_fake.call_count, 0);
}
