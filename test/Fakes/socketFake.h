/**
 * @brief Fake functions for C functions in sys/socket.h
 *
 * @file sys/socket.h
 *
 * @author Deon McClung
 * @copyright 2023, Deon McClung, All rights reserved. See LICENSE in the repository root.
 */

#pragma once

#include <sys/socket.h>

#include <fff/fff.h>

#define RESET_SOCKET_FAKES \
    RESET_FAKE(bind) \
    RESET_FAKE(listen) \


FAKE_VALUE_FUNC3(int, bind, int, const struct sockaddr*, socklen_t)
FAKE_VALUE_FUNC2(int, listen, int, int)
