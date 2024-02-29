/**
 * @file comm_data_send_tcp.h
 * @brief Implementation of TCD sender data communication with corelink server.
 */
#ifndef CORELINK_OBJECTS_STREAMS_COMMDATASENDTCP_H
#define CORELINK_OBJECTS_STREAMS_COMMDATASENDTCP_H

/**
 * TODO:
 * Should compare performance with using individual thread, sockets, and queues per sender.
 * Each socket can also have it's own destination port to reduce the data put in the queue.
 * 
 * Move DataSenderTCP to its own class.
 */

#include "corelink/objects/streams/comm_data_send_base.h"
#include "corelink/objects/generics/stream_map.h"
#include "corelink/objects/generics/safe_queue.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class comm_data_send_tcp : public comm_data_send_base {
            public:
                comm_data_send_tcp();
                ~comm_data_send_tcp();

                void addStream(const STREAM_ID& streamID, const std::string& ip, int port) override;
                int getStreamRef(const STREAM_ID& streamID) override;
                void rmStream(const STREAM_ID& streamID) override;

                bool sendMsg(int ref, const STREAM_ID& streamID, const int& federationID, const std::string& msg) override;
                bool sendMsg(int ref, const STREAM_ID& streamID, const int& federationID, const std::string& msg, const std::string& json, bool serverCheck) override;
            private:
                /**
                 * @private
                 * TCP Sender data stored by dll for a stream.
                 */
                struct DataSenderTCP {
                    SOCKET sock;
                    sockaddr_in hint;

                    DataSenderTCP(const std::string& serverIP = "0.0.0.0", int port = 0) {
                        sock = INVALID_SOCKET;
                        hint.sin_family = AF_INET;
                        inet_pton(AF_INET, serverIP.c_str(), &hint.sin_addr);
                        hint.sin_port = htons(port);
                        if (port > 0 && port < 65535) {
                            sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                            if (sock != INVALID_SOCKET) {
                                //not sure if this actually does anything. missing unix header
                                //int flag = 1;
                                //setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
                                connect(sock, (sockaddr*)&hint, sizeof(hint));
                            }
                        }
                    }
                    ~DataSenderTCP() {}
                };
                /**
                 * Maps stream ids to their respective sender.
                 * Also used to quickly reference streamids.
                 */
                CorelinkDLL::Object::Generic::stream_map<DataSenderTCP> streamMap;

                /**
                 * Socket for sending UDP packets.
                 */
                SOCKET sock = INVALID_SOCKET;

                /**
                 * Passes data (socket, message pair) from client to send thread.
                 */
                CorelinkDLL::Object::Generic::safe_queue<std::pair<SOCKET, std::string>> sendQueue;

                /**
                 * Sender thread for sendFunc.
                 */
                std::thread sendThread;

                /**
                 * Polls data from the send queue and sends the data.
                 */
                void sendFunc();

                /**
                 * Used to indicate that the thread should close.
                 */
                bool running;
            protected:
            };
        }
    }
}

#endif