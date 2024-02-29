/**
 * @file client_main.h
 * @brief Class for handling initialization and destruction of Corelink client.
 * Stores information necessary for client.
 */
#ifndef CORELINK_OBJECTS_CLIENTMAIN_H
#define CORELINK_OBJECTS_CLIENTMAIN_H

#include "corelink/headers/header.h"
#include "corelink/objects/initialization_data.h"
#include "corelink/objects/streams/comm_data_base.h"
#include "corelink/objects/streams/comm_main_base.h"
#include "corelink/objects/streams/stream_data.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class comm_main_base;
        }
    }
}

namespace CorelinkDLL {
    namespace Object {
        class client_main {
        public:
            /**
             * Data from initialization.
             * Contains a mutex to use for locking.
             * Message queue is used for passing server response from dll to client.
             */
            initialization_data data;

            /// IP address of Corelink server specified by user.
            std::string serverIP;
            /// IPv4 address of Corelink server used in connection. Necessary for raw UDP/TCP.
            std::string serverIPEffective;
            /// Port of Corelink server specified by user.
            int serverPort;

            /// Token string appended to main socket commands.
            std::string token;
            /// IP of the client.
            std::string clientIP;

            /// Reference to main communication component.
            CorelinkDLL::Object::Stream::comm_main_base* mainComm;

            /// Array of references to all corelink data streams
            CorelinkDLL::Object::Stream::comm_data_base** dataStreams;

            /// Map of each stream to its data to make retrieval easier and faster.
            std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data> mapStreamData;

            /// Lock to ensure thread safety of mapStreamData.
            std::mutex streamLock;

        public:
            client_main(const initialization_data& data, int& errorID);
            ~client_main();

            /**
             * Initializes data streams for client use.
             * @param effectiveIP IPv4 server IP address.
             */
            void initDataStreams(const std::string& effectiveIP, int& errorID);

            /**
             * Clears all of the data streams.
             */
            void cleanDataStreams();

            /**
             * Adds stream for client to handle.
             * @param streamData New stream data added to client.
             * @param protocol Stream state protocol.
             * @param port Server port to connect to.
             */
            void addStream(const CorelinkDLL::Object::Stream::stream_data& streamData, int protocol, int port);

            /**
             * Removes stream from client.
             * @param streamID Stream to remove.
             */
            void rmStream(const STREAM_ID& streamID);

            /**
             * Gets the stream ref for the specified stream.
             * @param streamID Stream to obtain reference of.
             * @return ref to streamID or -1 if not found.
             */
            int getStreamRef(const STREAM_ID& streamID);

            /**
             * Gets the state for the specified stream.
             * @param streamID Stream to obtain state of.
             * @return protocol or STREAM_STATE_NONE if not found.
             */
            int getStreamState(const STREAM_ID& streamID);

            /**
             * Gets the compacted streamData for the specified stream.
             * @param streamID Stream to obtain data of.
             * @return streamData or "" if not found.
             */
            std::string getStreamData(const STREAM_ID& streamID);

            /**
             * Checks if the stream is a type of stream.
             * @param streamID Stream to obtain reference of.
             * @param streamState State to compare stream to.
             * @return Is the stream the following type.
             */
            bool streamIsType(const STREAM_ID& streamID, int streamState);

            /**
             * Gets all streams on the client.
             * @return List of streams on the client.
             */
            std::vector<int> getStreams();

            /**
             * Gets all sources for a stream on the client.
             * @param streamID Stream to obtain sources from.
             * @return List of streams on the client.
             */
            std::vector<int> getStreamSources(const STREAM_ID& streamID);

            /**
             * Adds a source to a target stream on the client.
             * @param targetID ID to add source to.
             * @param source ID to add.
             */
            void addSource(const STREAM_ID& targetID, const STREAM_ID& sourceID);

            /**
             * Removes a source from a all streams on the client.
             * @param source ID to remove.
             */
            void rmSource(const STREAM_ID& sourceID);

            /**
             * Removes a source from a target stream on the client.
             * @param targetID ID to remove source from.
             * @param source ID to remove.
             */
            void rmSource(const STREAM_ID& targetID, const STREAM_ID& sourceID);

        private:
            client_main(const client_main&) = delete;
            client_main& operator=(const client_main&) = delete;
        };
    }
}
#endif