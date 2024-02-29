#include "corelink/objects/streams/tcp_recv_handler.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            tcp_recv_handler::tcp_recv_handler(SOCKET s, int capacity) {
                sock = s;
                backBlock = currBlock = 0;
                backIndex = currIndex = 0;
                //get closest (largest) power of 2 necessary
                blocks = 2;
                while (blocks < capacity) {
                    blocks *= 2;
                }
                mask = blocks - 1;
                data = new char*[blocks];
                for (int i = 0; i < blocks; ++i) {
                    data[i] = new char[BLOCK_SIZE];
                }
            }

            tcp_recv_handler::~tcp_recv_handler() {
                for (int i = 0; i < blocks; ++i) {
                    delete[] data[i];
                }
                delete[] data;
                data = nullptr;
            }

            char* tcp_recv_handler::recvData(int& len) {
                if (((currBlock + 1) & mask) == backBlock) { resize(); }
                char* ret = data[currBlock] + currIndex;
                len = recv(sock, ret, BLOCK_SIZE - currIndex, 0);
                if (len > 0) {
                    currIndex += len;
                    if (currIndex == BLOCK_SIZE) {
                        currIndex = 0;
                        currBlock = (currBlock + 1) & mask;
                    }
                }
                else {
                    len = SOCKET_ERROR_CODE;
                    ret = nullptr;
                }
                return ret;
            }

            char* tcp_recv_handler::getData(int len, int buffer) {
                if (len > size()) { return nullptr; }
                int tmp;
                char* output;
                tmp = len + buffer;
                output = new char[tmp];
                for (int i = len; i < tmp; ++i) {
                    output[i] = 0;
                }
                // buffer is now free to use as temporary variable. Use to store current index.
                tmp = BLOCK_SIZE - backIndex;
                tmp = tmp < len ? tmp : len;
                memcpy(output, data[backBlock] + backIndex, tmp);
                buffer = tmp;
                backIndex += tmp;
                if (backIndex == BLOCK_SIZE) {
                    backBlock = (backBlock + 1) & mask;
                    backIndex = 0;
                }
                while ((tmp = len - buffer) > 0) {
                    if (tmp >= BLOCK_SIZE) {
                        memcpy(output + buffer, data[backBlock], BLOCK_SIZE);
                        backBlock = (backBlock + 1) & mask;
                        buffer += BLOCK_SIZE;
                    }
                    else {
                        memcpy(output + buffer, data[backBlock], tmp);
                        backIndex = tmp;
                        buffer += tmp;
                    }
                }
                if (size() == 0) {
                    backBlock = currBlock = backIndex = currIndex = 0;
                }
                return output;
            }

            void tcp_recv_handler::resize() {
                char** newData;
                newData = new char* [blocks * 2];

                for (int i = 0; i < blocks; ++i) {
                    newData[i] = data[(i + backBlock) & mask];
                    newData[i + blocks] = new char[BLOCK_SIZE];
                }
                backBlock = 0;
                currBlock = blocks - 1;
                blocks *= 2;
                mask = blocks - 1;

                delete[] data;
                data = newData;
                newData = nullptr;
            }
        }
    }
}