/**
 * @file comm_data_recv_base.h
 * @brief Base class for receiver data communication with corelink server.
 */
#ifndef CORELINK_OBJECTS_STREAMS_COMMDATARECVBASE_H
#define CORELINK_OBJECTS_STREAMS_COMMDATARECVBASE_H

#include "corelink/objects/streams/comm_data_base.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            /**
             * receiver ID of receiver.
             * source ID data came from.
             * msg Pointer to data in bytes.
             * jsonLen Length of json data.
             * msgLen Length of data.
             * extra Data added in addition to the callback.
             */
            typedef void(*Callback)(STREAM_ID receiver, STREAM_ID source, const char* msg, int jsonLen, int msgLen, void* extra);
            
            /**
             * @class recv_stream_data_base
             * Base class for receiver stream data.
             */
            class recv_stream_data_base {
            public:
                recv_stream_data_base();

                recv_stream_data_base(const recv_stream_data_base& rhs);

                virtual ~recv_stream_data_base();

                recv_stream_data_base& operator=(const recv_stream_data_base& rhs);

                recv_stream_data_base& operator=(recv_stream_data_base&& rhs);

                /**
                 * Switches callback function and returns extra data.
                 */
                void* changeFunc(Callback funcPointer, void* funcExtra);

                /**
                 * Calls the callback function.
                 */
                void callFunc(const STREAM_ID& recvID, const STREAM_ID& sendID, const char* data, const int& jsonLen, const int& msgLen);

            private:
                Callback funcPointer;
                void* funcExtra;
                std::mutex funcLock;
            };

            class comm_data_recv_base : public comm_data_base {
            public:
                comm_data_recv_base();
                virtual ~comm_data_recv_base() = default;
 
                /**
                 * Sets the callback to use along with any wrapper side data needed to add in the callback.
                 * @return Wrapper side data (or nullptr) for the wrapper to clean up.
                 */
                void* setRecvCallback(int ref, const STREAM_ID& streamID, Callback recvCallback, void* callbackData);

            private:
            protected:
                /**
                 * Maps stream ids to their respective sender.
                 * Also used to quickly reference streamids.
                 */
                CorelinkDLL::Object::Generic::stream_map<recv_stream_data_base*> streamMap;
            };
        }
    }
}

#endif