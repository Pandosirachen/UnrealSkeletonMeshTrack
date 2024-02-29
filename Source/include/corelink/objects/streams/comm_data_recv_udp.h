/**
 * @file comm_data_recv_udp.h
 * @brief Implementation of UDP reciever for receiver data communication with corelink server.
 */

#ifndef CORELINK_OBJECTS_STREAMS_COMMDATARECVUDP_H
#define CORELINK_OBJECTS_STREAMS_COMMDATARECVUDP_H

#include "corelink/objects/streams/comm_data_recv_base.h"
#include "corelink/objects/generics/stream_map.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class recv_stream_data_udp : public recv_stream_data_base {
            public:
                int nsPort;
                std::thread listener;
                SOCKET sock;

                recv_stream_data_udp(int port = 0);
                recv_stream_data_udp(const recv_stream_data_udp& rhs);
                ~recv_stream_data_udp();
                recv_stream_data_udp& operator=(const recv_stream_data_udp& rhs);
                recv_stream_data_udp& operator=(recv_stream_data_udp&& rhs);
            };

            class comm_data_recv_udp : public comm_data_recv_base {
            public:
                comm_data_recv_udp();
                ~comm_data_recv_udp();

                void addStream(const STREAM_ID& streamID, const std::string&, int port) override;
                int getStreamRef(const STREAM_ID& streamID) override;
                void rmStream(const STREAM_ID& streamID) override;

            private:

                /**
                 * Waits for socket to recieve data before running the callback function.
                 * TODO: replace use of streamMap with udp recv. Data is recieved in packets so it is either an entire message or none.
                 * TODO: Use select() to improve performance of socket blocking and reduce number of threads.
                 * TODO: Place this function inside the struct and use this instead of streamMap.at(ref) for clarity and minor performance improvement.
                 * @param ref Quick refecrence that this thread is running on.
                 * @param ip Server ip address.
                 */
                void recvFunc(int ref, const std::string& ip);
            protected:
            };
        }
    }
}

#endif
