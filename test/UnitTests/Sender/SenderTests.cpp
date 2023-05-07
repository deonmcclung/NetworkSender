/**
 * @brief Unit tests for the Sender class
 *
 * @file SenderTests.cpp
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

// Mocks
#include "Common/Mocks/SocketMock.h"

// Code under test
#include "Sender/Sender.cpp"

// Library headers
#include <gtest/gtest.h>

// Standard headers
#include <memory>

using testing::_;
using testing::Return;


class SenderTests : public testing::Test
{
protected: // Definitions
    static constexpr const char* TEST_IP = "123.210.012.3";
    static constexpr uint16_t TEST_PORT = 12345;

protected: // Methods
    SenderTests()
    {
        mSocketMock = std::make_shared<testing::NiceMock<Common::SocketMock>>();
        mSocketMockVendor.queueMock(mSocketMock);

        // Consume the queued socket mock
        mTestObj = std::make_unique<Sender>(TEST_IP, TEST_PORT);
    }

    virtual ~SenderTests()
    {

    }

protected: // Members
    MockVendor<Common::SocketMock, Common::Socket>  mSocketMockVendor;
    std::shared_ptr<Common::SocketMock>             mSocketMock;
    std::unique_ptr<Sender>                         mTestObj;
};


// Test that we actually call the Socket::connect method
TEST_F(SenderTests, TestConnect)
{
    // Setup
    EXPECT_CALL(*mSocketMock, connect());
    ON_CALL(*mSocketMock, isConnected()).WillByDefault(Return(true));

    // Test
    EXPECT_NO_THROW(mTestObj->connect());
}

// Test that the connect() method correctly calls the conect for the expected number of times
// and ultimately fails with an exception.
TEST_F(SenderTests, TestConnectFail)
{
    // Setup
    constexpr int RETRIES = 1;
    EXPECT_CALL(*mSocketMock, connect())
        .Times(RETRIES + 1)
        .WillRepeatedly([]{ throw Common::Socket::ConnectionRefusalException(TEST_IP, TEST_PORT); });

    // Test
    EXPECT_THROW(mTestObj->connect(RETRIES), Sender::Exception);
}

// Test that the parseCommandLine() method indicates no files an no stdin when no args.
TEST_F(SenderTests, ParseCommandLineNoArgs)
{
    // Setup
    const char* argv[] =
    {
        "AppName",
    };

    // Test
    auto data = mTestObj->parseCommandLine(sizeof(argv)/sizeof(argv[0]), argv);

    // Verify
    EXPECT_FALSE(data.readStdin);
    EXPECT_TRUE(data.filesToSend.empty());
}

// Test that the parseCommandLine() method only indicates stdin when no other files are specified.
TEST_F(SenderTests, ParseCommandLineStdinOnly)
{
    // Setup
    const char* argv[] =
    {
        "AppName",
        "-",
    };

    // Test
    auto data = mTestObj->parseCommandLine(sizeof(argv)/sizeof(argv[0]), argv);

    // Verify
    EXPECT_TRUE(data.readStdin);
    EXPECT_TRUE(data.filesToSend.empty());
}

// Test that the parseCommandLine() method only indicates stdin and files when specified
TEST_F(SenderTests, ParseCommandLineStdinAndFiles)
{
    // Setup
    const char* argv[] =
    {
        "AppName",
        "File1",
        "File2",
        "-",
    };

    // Test
    auto data = mTestObj->parseCommandLine(sizeof(argv)/sizeof(argv[0]), argv);

    // Verify
    EXPECT_TRUE(data.readStdin);
    EXPECT_EQ(2, data.filesToSend.size());
    if (data.filesToSend.size() >= 2)
    {
        EXPECT_STREQ("File1", data.filesToSend[0].c_str());
        EXPECT_STREQ("File2", data.filesToSend[1].c_str());
    }
}

// Test that the parseCommandLine() method only indicates files
TEST_F(SenderTests, ParseCommandLineFiles)
{
    // Setup
    const char* argv[] =
    {
        "AppName",
        "File1",
        "File2",
    };

    // Test
    auto data = mTestObj->parseCommandLine(sizeof(argv)/sizeof(argv[0]), argv);

    // Verify
    EXPECT_FALSE(data.readStdin);
    EXPECT_EQ(2, data.filesToSend.size());
    if (data.filesToSend.size() >= 2)
    {
        EXPECT_STREQ("File1", data.filesToSend[0].c_str());
        EXPECT_STREQ("File2", data.filesToSend[1].c_str());
    }
}

// Test that the parseCommandLine() method only indicates files
TEST_F(SenderTests, ParseCommandLineTrailingFiles)
{
    // Setup
    const char* argv[] =
    {
        "AppName",
        "File1",
        "File2",
        "-",
        "File3",   // Should be ignored
    };

    // Test
    auto data = mTestObj->parseCommandLine(sizeof(argv)/sizeof(argv[0]), argv);

    // Verify
    EXPECT_TRUE(data.readStdin);
    EXPECT_EQ(2, data.filesToSend.size());
    if (data.filesToSend.size() >= 2)
    {
        EXPECT_STREQ("File1", data.filesToSend[0].c_str());
        EXPECT_STREQ("File2", data.filesToSend[1].c_str());
    }
}

// Test the sendStream method sends a multi-line input in individual lines
TEST_F(SenderTests, TestSendStreamMultiLineInput)
{
    // Setup
    const char* lines[] =
    {
        "This is the first line\n",
        "This is the second line\n",
        "This is the third line\n",
    };

    std::ostringstream str;
    for (const auto& line : lines)
    {
        str << line;
    }

    std::istringstream istr(str.str());

    ON_CALL(*mSocketMock, isConnected()).WillByDefault(Return(true));

    EXPECT_CALL(*mSocketMock, send(_, _))
        .WillOnce([&lines](const void* buffer, size_t len)
        {
            auto str = std::string(static_cast<const char*>(buffer), len);
            EXPECT_STREQ(lines[0], str.c_str());
            EXPECT_EQ(std::strlen(lines[0]), len);
        })
        .WillOnce([&lines](const void* buffer, size_t len)
        {
            auto str = std::string(static_cast<const char*>(buffer), len);
            EXPECT_STREQ(lines[1], str.c_str());
            EXPECT_EQ(std::strlen(lines[1]), len);
        })
        .WillOnce([&lines](const void* buffer, size_t len)
        {
            auto str = std::string(static_cast<const char*>(buffer), len);
            EXPECT_STREQ(lines[2], str.c_str());
            EXPECT_EQ(std::strlen(lines[2]), len);
        });

    // for (const auto& line : lines)
    // {
    //     std::cout << line << std::endl;
    //     EXPECT_CALL(*mSocketMock, send(_, _)).WillOnce([&line](const void* buffer, size_t len)
    //     {
    //         auto str = static_cast<const char*>(buffer);
    //         EXPECT_STREQ(line, str);
    //         EXPECT_EQ(std::strlen(line), len);
    //     });
    // }

    EXPECT_NO_THROW(mTestObj->sendStream(istr));
}
