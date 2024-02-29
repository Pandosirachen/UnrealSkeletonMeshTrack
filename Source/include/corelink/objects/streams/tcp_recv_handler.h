/**
 * @file tcp_recv_handler.h
 * @brief Cyclic queue data structure to handle tcp socket received data buffering and retrieving.
 */
#ifndef CORELINK_OBJECTS_STREAMS_TCPRECVHANDLER_H
#define CORELINK_OBJECTS_STREAMS_TCPRECVHANDLER_H

#include "corelink/headers/header.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class tcp_recv_handler {
            private:
                static const int BLOCK_SIZE = 64;

                /// TCP socket
                SOCKET sock;
                /// Blocks allocated for storing data from socket.
                char** data;
                /// Number of blocks allocated.
                int blocks;
                /// blocks - 1 to use bitwise and in place of modulo.
                int mask;

                /// Block number of the oldest data (front of the queue)
                int backBlock;
                /// Block index of the oldest data (front of the queue)
                int backIndex;
                /// Block number of the newest data (back of the queue)
                int currBlock;
                /// Block index of the newest data (back of the queue)
                int currIndex;
            public:
                tcp_recv_handler(SOCKET s, int capacity = 2);
                ~tcp_recv_handler();

                /**
                 * Tries to receive data from tcp socket.
                 * @param len Length of message received. If no message, it stores the error code.
                 * @return Pointer to address in buffer where data begins. If no message, nullptr.
                 */
                char* recvData(int& len);

                /**
                 * @return Number of bytes currently allocated in buffer.
                 */
                int size();

                /**
                 * Gets the data of length specified.
                 * @param len Amount of data to retrieve.
                 * @param buffer Amount of trailing 0s in returned array.
                 * @return nullptr if not enough data is in buffer. Otherwise, pointer on heap of length len + buffer containing data requested followed by buffer of 0s.
                 */
                char* getData(int len, int buffer = 0);

            private:
                void resize();
            };

            inline int tcp_recv_handler::size() {
                return ((currBlock < backBlock ? blocks : 0) + currBlock - backBlock) * BLOCK_SIZE +
                    currIndex - backIndex;
            }
        }
    }
}

#endif