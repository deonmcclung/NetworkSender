cmake_minimum_required(VERSION 3.10)
project(NETWORKSENDER)

set(CMAKE_CXX_STANDARD 20)

add_executable(sender Sender/main.cpp Common/Socket.cpp)
target_include_directories(sender PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


add_executable(receiver Receiver/main.cpp Receiver/Receiver.cpp Common/Socket.cpp)
target_include_directories(receiver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY git@github.com:google/googletest.git
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

function(add_unit_test testName)
    string(REPLACE "/" "__" testTargetName ${testName})
    string(PREPEND testTargetName test_)
    add_executable(${testTargetName} test/UnitTests/${testName}.cpp)
    target_link_libraries(${testTargetName} GTest::gmock_main)
    target_include_directories(${testTargetName}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/MockVendor
    )
    gtest_discover_tests(${testTargetName})
endfunction()

add_unit_test(Common/SocketTests)
add_unit_test(Receiver/ReceiverTests)
