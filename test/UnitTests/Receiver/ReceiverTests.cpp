/**
 * @brief Unit tests for the Receiver class
 *
 * @file ReceiverTests.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

// Mocks
#include "Common/Mocks/SocketMock.h"

// Code under test
#include "Receiver/Receiver.cpp"

// Library headers
#include <gtest/gtest.h>

// Standard headers
#include <memory>
#include <sstream>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <sstream>
#include <cmath>
#include <cstring>
#include <atomic>

using namespace std::chrono_literals;

using testing::NiceMock;
using testing::_;
using testing::Return;


class ReceiverTests : public testing::Test
{
protected: // Definitions
    static constexpr const char* TEST_IP = "123.210.012.3";
    static constexpr uint16_t TEST_PORT = 12345;

protected: // Methods
    ReceiverTests()
    {
        mTestObj = std::make_unique<Receiver>();
    }

    virtual ~ReceiverTests() = default;

protected: // Members
    std::unique_ptr<Receiver>               mTestObj;
    Common::SocketMockVendor                mSocketMockVendor;
    std::shared_ptr<Common::SocketMock>     mSocketMock{ std::make_shared<NiceMock<Common::SocketMock>>() };
};


// Test expected bahaviors while receiving a connection. This test exercises expected behaviors for
// receiving a connection and receiving a multi-line message that is broken up into multiple send calls.
TEST_F(ReceiverTests, TestReceiveConnection)
{
    auto connSocketMock = std::make_shared<NiceMock<Common::SocketMock>>();
    ON_CALL(*connSocketMock, isConnected()).WillByDefault(Return(true));
    mSocketMockVendor.queueMock(connSocketMock);

    std::condition_variable finishedCv;
    std::mutex finishedMtx;
    bool finished = false;
    constexpr auto TIMEOUT = 10s;

    // Not necessarily central to the test is this being broken up into individual lines (to add complexity).
    // In doing so, we are sending whole lines upon which that we re-apply the newline.
    // Because of that, the following string needs to end in a newline for the string compare at the end of the test.
    std::string testMessage = "This is a long message.\n\nWell, maybe not so long.\nStill going?\nYes, I think so.\n";
    std::istringstream input(testMessage);
    const auto testMessageLen = testMessage.length();

    // Consume connSocketMock
    EXPECT_CALL(*mSocketMock, accept())
        .WillOnce(Return(std::optional<Common::Socket>(Common::Socket(TEST_IP, TEST_PORT))))
        .WillOnce([&]()
            {
                std::unique_lock<decltype(finishedMtx)> lck(finishedMtx);
                finishedCv.wait_for(lck, TIMEOUT, [&finished]{ return finished; });
                return std::optional<Common::Socket>();
            });

    // mSocketMock is consumed by execute
    mSocketMockVendor.queueMock(mSocketMock);

    EXPECT_CALL(*connSocketMock, recv(_, _)).WillRepeatedly([&input](void* buffer, size_t len)
        {
            if (input)
            {
                char line[1024];
                input.getline(line, sizeof(line)-1);
                line[input.gcount()-1] = '\n';
                std::string str(line, input.gcount());
                std::memcpy(buffer, str.c_str(), str.length());
                return std::optional<size_t>(str.length());
            }

            return std::optional<size_t>();
        }
    );

    ON_CALL(*connSocketMock, recv(_, _)).WillByDefault(Return(std::optional<size_t>()));

    std::ostringstream output;
    EXPECT_NO_THROW(mTestObj->execute(TEST_IP, TEST_PORT,
        [&output, &finishedCv, &finishedMtx, &finished, testMessageLen](const void* buffer, size_t len)
        {
            output << std::string(static_cast<const char*>(buffer), len);

            // Test for end of message
            if (output.str().length() >= testMessageLen)
            {
                std::unique_lock<decltype(finishedMtx)> lck(finishedMtx);
                finished = true;
                finishedCv.notify_one();
            }
        }
    ));

    // Verify
    EXPECT_STREQ(testMessage.c_str(), output.str().c_str());
}
