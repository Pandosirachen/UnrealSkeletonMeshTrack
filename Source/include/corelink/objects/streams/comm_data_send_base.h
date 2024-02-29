/**
 * @file comm_data_send_base.h
 * @brief Base class for sender data communication with corelink server.
 */
#ifndef CORELINK_OBJECTS_STREAMS_COMMDATASENDBASE_H
#define CORELINK_OBJECTS_STREAMS_COMMDATASENDBASE_H

#include "corelink/objects/streams/comm_data_base.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class comm_data_send_base : public comm_data_base {
            public:
                virtual ~comm_data_send_base() = default;

                /**
                 * Sends data from the client to the server.
                 * @param ref Reference to quickly access stream.
                 * @param streamID Stream to verify that the correct stream was retrieved.
                 * @param federationID Target server to send data to.
                 * @param msg Message to send to server.
                 * @return Stream successfully found.
                 */
                virtual bool sendMsg(int ref, const STREAM_ID& streamID, const int& federationID, const std::string& msg) = 0;
                
                /**
                 * Sends data from the client to the server.
                 * @param ref Reference to quickly access stream.
                 * @param streamID Stream to verify that the correct stream was retrieved.
                 * @param federationID Target server to send data to.
                 * @param msg Message to send to server.
                 * @param json Json to attach with message.
                 * @param serverCheck Should server check the json.
                 * @return Stream successfully found.
                 */
                virtual bool sendMsg(int ref, const STREAM_ID& streamID, const int& federationID, const std::string& msg, const std::string& json, bool serverCheck) = 0;
            private:
            protected:
            };
        }
    }
}

#endif