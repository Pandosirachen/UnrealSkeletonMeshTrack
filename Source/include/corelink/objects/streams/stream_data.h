/**
 * @file stream_data.h
 * @brief Base class for data stream data.
 */
#ifndef CORELINK_OBJECTS_STREAMS_STREAMDATA_H
#define CORELINK_OBJECTS_STREAMS_STREAMDATA_H

#include "corelink/headers/header.h"

/* TODO: determine which parts of the class can be marked const.
 */
namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class stream_data {
            public:
                /// ID of the stream seen by the server.
                int streamID;
                /// Stores the type of the stream (UDP, TCP, WS & SEND, RECV).
                int state;
                /// Quick access to the state index value.
                int stateIndex;
                /// Maximum transmission unit. Max size of packets transferable at a time. Not actually used as of now.
                int mtu;
                /// Username of the creator of the stream.
                std::string user;
                /// Workspace stream belongs to in the server.
                std::string workspace;
                /// Additional data attached to the stream
                std::string meta;
                /// Type of the stream. Sender may only have a single type while Receiver has arbritary amount.
                std::vector<std::string> type;
                /// Sources for recievers. Always empty for sender (may want to implement number of listeners).
                std::unordered_set<int> sources;
                /// String of compacted data to send to wrapper. Sources will need to be retrieved seperately as it is dynamic.
                std::string streamData;

            private:
            public:
                stream_data();
                stream_data(const int& streamID, const int& state, const int& mtu, 
                    const std::string& user, const std::string& workspace, const std::string& meta, 
                    const std::vector<std::string>& type, const std::unordered_set<int>& sources);
                stream_data(const stream_data& rhs);
                ~stream_data();

                stream_data& operator=(const stream_data& rhs);

                static std::string compact(const int& streamID, const int& state, const int& mtu, 
                    const std::string& user, const std::string& workspace, const std::string& meta, 
                    const std::vector<std::string>& type);
            };
        }
    }
}

#endif