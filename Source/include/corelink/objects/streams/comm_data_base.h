/**
 * @file comm_data_base.h
 * @brief Base class for data communication with corelink server.
 */
#ifndef CORELINK_OBJECTS_STREAMS_COMMDATABASE_H
#define CORELINK_OBJECTS_STREAMS_COMMDATABASE_H

#include "corelink/headers/header.h"
#include "corelink/objects/generics/safe_queue.h"
#include "corelink/objects/generics/stream_map.h"
#include "corelink/objects/init.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class comm_data_base {
            public:
                virtual ~comm_data_base() = default;

                /**
                 * Adds a new stream to the client.
                 * @param streamID Stream to add to storage.
                 * @param ip IP address of server.
                 * @param port Port to connect to server.
                 */
                virtual void addStream(const STREAM_ID& streamID, const std::string& ip, int port) = 0;

                /**
                 * Gets an integer handle to quickly reference the stream needed.
                 * @param streamID ID of stream to retrieve reference
                 * @return Reference to the stream id.
                 */
                virtual int getStreamRef(const STREAM_ID& streamID) = 0;

                /**
                 * Removes a stream from the client.
                 * @param streamID Stream to add to remove.
                 */
                virtual void rmStream(const STREAM_ID& streamID) = 0;

                /**
                 * Packages the data in a format to send to the server.
                 * @param streamID Stream sending the data.
                 * @param federationID Server to send data to.
                 * @param msg String message to send to server.
                 * @param json Json string to attach with the message.
                 * @param serverCheck Should server check the json.
                 * @return String that the server is able reciever and interpret.
                 */
                static std::string packageSend(const STREAM_ID& streamID, const int& federationID, const std::string& msg, const std::string& json = "", bool serverCheck = false);
            };
        }
    }
}
#endif