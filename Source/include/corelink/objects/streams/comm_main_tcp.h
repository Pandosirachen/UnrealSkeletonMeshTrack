/**
 * @file comm_main_tcp.h
 * @brief Implementation of TCP main communication with corelink server.
 */
#ifndef CORELINK_OBJECTS_STREAMS_COMMMAINTCP_H
#define CORELINK_OBJECTS_STREAMS_COMMMAINTCP_H

#include "corelink/headers/header.h"
#include "corelink/objects/streams/comm_main_base.h"
#include "corelink/objects/generics/message_handler.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class comm_main_tcp : public comm_main_base {
            private:
                SOCKET sock;
                std::mutex sendLock;
                std::thread recvThread;
                sockaddr_in serverHint;
            public:
                comm_main_tcp(client_main* clientRef);
                ~comm_main_tcp();

                bool sendMsg(char* msg, int len) override;
                std::string connectServer(const std::string& ip, const int& port, int& errorID) override;

            private:
                comm_main_tcp(const comm_main_tcp&) = delete;
                comm_main_tcp& operator=(const comm_main_tcp&) = delete;

                /**
                 * Reciever thread for handling server response and callbacks.
                 */
                void RecvThread();
            };
        }
    }
}

#endif