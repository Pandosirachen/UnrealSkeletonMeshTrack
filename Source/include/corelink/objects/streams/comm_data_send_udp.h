/**
 * @file comm_data_send_udp.h
 * @brief Implementation of UPD sender data communication with corelink server.
 */
#ifndef CORELINK_OBJECTS_STREAMS_COMMDATASENDUDP_H
#define CORELINK_OBJECTS_STREAMS_COMMDATASENDUDP_H

#include "corelink/objects/streams/comm_data_send_base.h"
#include "corelink/objects/generics/stream_map.h"
#include "corelink/objects/generics/safe_queue.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class comm_data_send_udp : public comm_data_send_base {
            public:
                comm_data_send_udp(const std::string& ip, int& errorID);
                ~comm_data_send_udp();

                void addStream(const STREAM_ID& streamID, const std::string& ip, int port) override;
                int getStreamRef(const STREAM_ID& streamID) override;
                void rmStream(const STREAM_ID& streamID) override;

                bool sendMsg(int ref, const STREAM_ID& streamID, const int& federationID, const std::string& msg) override;
                bool sendMsg(int ref, const STREAM_ID& streamID, const int& federationID, const std::string& msg, const std::string& json, bool serverCheck) override;
            private:
                /**
                 * @private
                 * UDP Sender data stored by dll for a stream.
                 */
                struct DataSenderUDP {
                    int nsPort;
                    DataSenderUDP(int port = 0) {
                        nsPort = INVALID_PORT;
                        if (port > 0 && port <= 65535) {
                            nsPort = htons(port);
                        }
                    }
                    ~DataSenderUDP() {}
                };
                /**
                 * Maps stream ids to their respective sender.
                 * Also used to quickly reference streamids.
                 */
                CorelinkDLL::Object::Generic::stream_map<DataSenderUDP> streamMap;

                /* TODO:
                * Should compare performance with using individual thread, sockets, and queues per sender.
                * Each socket can also have it's own destination port to reduce the data put in the queue.
                */

                /**
                 * Socket for sending UDP packets.
                 */
                SOCKET sock = INVALID_SOCKET;

                /**
                 * Passes data (port, message pair) from client to send thread.
                 */
                CorelinkDLL::Object::Generic::safe_queue<std::pair<int, std::string>> sendQueue;

                /**
                 * Sender thread for sendFunc.
                 */
                std::thread sendThread;

                /**
                 * Polls data from the send queue and sends the data.
                 * @param serverIP ipv4 address of the server. (Currently does not support individual ips per connection)
                 */
                void sendFunc(const std::string& serverIP);

            protected:
            };
        }
    }
}

#endif