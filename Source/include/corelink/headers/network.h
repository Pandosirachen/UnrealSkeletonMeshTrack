/**
 * @file network.h
 * @brief Header containing libraries for networking.
 * TODO: Look into switching to the ASIO library.
 */

#ifndef CORELINK_HEADERS_NETWORK_H
#define CORELINK_HEADERS_NETWORK_H

/**
 * Includes for networking code.
 */
# ifdef _WIN32
#include <WS2tcpip.h>
#pragma comment (lib,"ws2_32.lib")
# else
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
# endif

#define INVALID_PORT htons(0)

/**
 * Macros for crossplatform networking.
 * Converts unix to windows equivalent.
 */
# ifdef _WIN32
#define SOCKET_ERROR_CODE WSAGetLastError()
#define SOCK_PTR const char*
static const DWORD recvTimeout = 1 * 1000;
#define socklen_t int
# else
#define SOCKET_ERROR_CODE errno
#define SOCK_PTR const void*
#define SOCKET int
#define INVALID_SOCKET 0
#define SOCKET_ERROR SO_ERROR
#define SD_RECEIVE SHUT_RD
#define SD_SEND SHUT_WR
#define SD_BOTH SHUT_RDWR
#define closesocket(socket) close(socket)

static timeval CreateTime(int seconds, int microseconds) {
    timeval t;
    t.tv_sec = seconds;
    t.tv_usec = microseconds;
    return t;
}

static const timeval recvTimeout = CreateTime(1, 0);

# endif

#define SOCKET_RECV_BUFFER_SIZE 65535

namespace CorelinkDLL {
    void setRecvSocketOpts(SOCKET& sock);
}

#endif