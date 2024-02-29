/**
 * @file dataStream.h
 * @brief Header containing all of the Corelink builtin commands excluding the ones necessary for communication.
 * 
 */
#ifndef CORELINK_CLIENT_DATASTREAM_H
#define CORELINK_CLIENT_DATASTREAM_H

#include "corelink/client/core.h"
#include "corelink/objects/streams/comm_data_recv_base.h"

namespace CorelinkDLL {
    extern "C" {
        /**
         * Gets reference number for a stream if it exists. References may have the same values though.
         * @param streamID Stream to obtain reference of.
         * @return Reference assigned to. Assigns -1 if streamid is not in the client streams.
         */
        EXPORTED int getStreamRef(const STREAM_ID& streamID);

        /**
         * Gets the state for the specified stream.
         * @param streamID Stream to obtain state of.
         * @return protocol or STREAM_STATE_NONE if not found.
         */
        EXPORTED int getStreamState(const STREAM_ID& streamID);

        /**
         * Gets the data stored for the specified stream.
         * @param streamID Stream to obtain data of.
         * @param len store length of data or 0 if not found.
         * @return Stream compacted data or nullptr.
         */
        EXPORTED const char* getStreamData(const STREAM_ID& streamID, int& len);

        /**
         * Gets the length of the data stored for the specified stream.
         * @param streamID Stream to obtain data of.
         * @return length of data or 0 if not found.
         */
        EXPORTED int getStreamDataLen(const STREAM_ID& streamID);

        /**
         * Gets the data stored for the specified stream.
         * @param streamID Stream to obtain data of.
         * @param buffer Buffer to store the stream data.
         */
        EXPORTED void getStreamDataStr(const STREAM_ID& streamID, char* buffer);

        /**
         * Adds data to send.
         * @param protocol Type of sender stream.
         * @param ref Stream to obtain reference of stream.
         * @param streamID Used to check if stream ref is correct.
         * @param federationID Specific server to send data to. Default to -1 if there is no target server.
         * @param msg Message to send on stream.
         * @return Whether streamid is in the client stream.
         */
        EXPORTED bool sendMsg(int protocol, int ref, const STREAM_ID& streamID, int federationID, const char* msg, int msgLen);

        /**
         * Adds data and json header to send.
         * @param protocol Type of sender stream.
         * @param ref Stream to obtain reference of stream.
         * @param streamID Used to check if stream ref is correct.
         * @param federationID Specific server to send data to. Default to -1 if there is no target server.
         * @param msg Message to send on stream.
         * @param json Json string to send on stream with msg.
         * @param serverCheck Should server check the json.
         * @return Whether streamid is in the client stream.
         */
        EXPORTED bool sendMsgJson(int protocol, int ref, const STREAM_ID& streamID, int federationID, const char* msg, int msgLen, const char* json, int jsonLen, bool serverCheck);

        /**
         * Sets the callback for receiver stream.
         * @param protocol Type of receiver stream.
         * @param ref Stream to obtain reference of stream.
         * @param streamID Used to check if stream ref is correct.
         * @param func Callback function.
         * @return Extra data created by the wrapper or nullptr if none.
         */
        EXPORTED void* setOnRecv(int protocol, int ref, const STREAM_ID& streamID, CorelinkDLL::Object::Stream::Callback func, void* funcData);

        /**
         * Checks whether a stream is a sender on the client.
         * This does not check for streams on the server.
         * @param streamID Stream to check if is sender.
         * @returns True if sender and false if it isn't.
         */
        EXPORTED bool isSendStream(int streamID);

        /**
         * Checks whether a stream is a receiver on the client.
         * This does not check for streams on the server.
         * @param streamID Stream to check if is receiver.
         * @returns True if receiver and false if it isn't.
         */
        EXPORTED bool isRecvStream(int streamID);

        /**
         * Gets all streams on the client.
         * @return List of streams on the client casted to char array.
         */
        EXPORTED char* getClientStreams();

        /**
         * Gets all streams on the client.
         * Data is pushed onto the main comm for retrieval to avoid changes in length of the data.
         * @param commID ID used to store the data.
         */
        EXPORTED void getClientStreamsPushed(int& commID);

        /**
         * Gets the sources for a particular stream on the client.
         * @param streamID Stream to retrieve sources for.
         * @return List of streams for the client sources casted to char array.
         */
        EXPORTED char* getClientStreamSources(int streamID);

        /**
         * Gets the sources for a particular stream on the client.
         * Data is pushed onto the main comm for retrieval to avoid changes in length of the data.
         * @param streamID Stream to retrieve sources for.
         * @param commID ID used to store the data.
         */
        EXPORTED void getClientStreamSourcesPushed(int streamID, int& commID);
    }
}

#endif