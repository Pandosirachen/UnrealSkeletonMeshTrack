/**
 * @file comm_data_recv_tcp.h
 * @brief Implementation of TCP reciever for receiver data communication with corelink server.
 */

#ifndef CORELINK_OBJECTS_STREAMS_COMMDATARECVTCP_H
#define CORELINK_OBJECTS_STREAMS_COMMDATARECVTCP_H

#include "corelink/objects/streams/comm_data_recv_base.h"
#include "corelink/objects/generics/stream_map.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class recv_stream_data_tcp : public recv_stream_data_base {
            public:
                std::thread listener;
                SOCKET sock;
                sockaddr_in hint;

                recv_stream_data_tcp(const std::string& serverIP = "", int port = 0);
                recv_stream_data_tcp(const recv_stream_data_tcp& rhs);
                ~recv_stream_data_tcp();
                recv_stream_data_tcp& operator=(const recv_stream_data_tcp& rhs);
                recv_stream_data_tcp& operator=(recv_stream_data_tcp&& rhs);
            };

            class comm_data_recv_tcp : public comm_data_recv_base {
            public:
                comm_data_recv_tcp();
                ~comm_data_recv_tcp();

                void addStream(const STREAM_ID& streamID, const std::string&, int port) override;
                int getStreamRef(const STREAM_ID& streamID) override;
                void rmStream(const STREAM_ID& streamID) override;

            private:

                /**
                 * Waits for socket to recieve data before running the callback function.
                 * TODO: Use select() to improve performance of socket blocking and reduce number of threads.
                 * TODO: Place this function inside the struct and use this instead of streamMap.at(ref) for clarity and minor performance improvement.
                 * @param ref Quick refecrence that this thread is running on.
                 */
                void recvFunc(int ref);
            protected:
            };
        }
    }
}

#endif
