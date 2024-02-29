#include "corelink/headers/network.h"

namespace CorelinkDLL {
    void setRecvSocketOpts(SOCKET& sock) {
        int recvBufferSize = SOCKET_RECV_BUFFER_SIZE;
        setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (SOCK_PTR)&recvBufferSize, sizeof(recvBufferSize));
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (SOCK_PTR)& recvTimeout, sizeof(recvTimeout));
    }
}