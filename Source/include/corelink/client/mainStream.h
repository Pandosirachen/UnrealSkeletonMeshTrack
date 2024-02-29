/**
 * @file mainStream.h
 * @brief Header containing all of the Corelink builtin commands excluding the ones necessary for communication.
 * NOTE: Stream related data will have consistent format.
 * Data format:\n 
 * -(int) Stream ID.\n 
 * -(string) User that owns stream.\n 
 * -(string) Workspace stream is in.\n 
 * -(string) Metadata stored with stream.\n 
 * -(int) State (protocol and direction) of stream. Maps onto a StreamState.\n 
 * -(int) MTU (Maximum Transmission Unit). 0 means infinity.\n 
 * -(int) Source count. Amount of sources added to stream on creation. 0 for senders.\n 
 * -(string[]) Type(s) of data sent/recieved on stream.\n 
 * -(string[]) Source(s) id for steams being listened to on initialization.\n 
 */
#ifndef CORELINK_CLIENT_MAINSTREAM_H
#define CORELINK_CLIENT_MAINSTREAM_H

#include "corelink/client/core.h"

namespace CorelinkDLL {
    /**
     * TODO: consider endianess of integer storage.
     * Adds data to the commID. Compresses a vector of strings to the following format.\n 
     * Data format:\n 
     * -(int) Number of strings.\n 
     * -(int[]) Length of each string.\n
     * -(char[]) Char data where strings are stored.\n
     * @param data Data to store and prepare for retrieval by wrapper.
     * @param commID ID used in the function call.
     */
    void addCommData(const std::vector<std::string>& data, const int& commID);

    /**
     * TODO: consider necessity of validating data formatting.
     * Adds data to the commID.\n 
     * Data format:\n 
     * -(int) Number of strings.\n 
     * -(int[]) Length of each string.\n
     * -(char[]) Char data where strings are stored.\n
     * @param data Data to store and prepare for retrieval by wrapper.
     * @param commID ID used in the function call.
     */
    void addCommData(const std::string& data, const int& commID);

    /**
     * Helper function to get stream state from strings.
     * @param proto Protocol used.
     * @param dir Sender or Receiver.
     * @return Equivalent stream state value.
     */
    int getStreamState(const char* proto, const char* dir);

    extern "C"
    {

        /**
         * Polls for response data for the commID.
         * @param commID ID used in the function call.
         * @param len Stores length of the data.
         * @return Data stored from parsing json response.
         */
        EXPORTED char* getCommData(const int& commID, int& len);

        /**
         * Polls for response data len for the commID.
         * @param commID ID used in the function call.
         * @return length of data stored from parsing json response.
         */
        EXPORTED int getCommDataLen(const int& commID);

        /**
         * Polls for response data for the commID.
         * @param commID ID used in the function call.
         * @param buffer Buffer to store the error message.
         * @return Succeed in retrieving response data.
         */
        EXPORTED bool getCommDataStr(const int& commID, char* buffer);

        /**
         * Gets list of functions available on the server.\n 
         * Data format:\n 
         * -(string[]) Function names.\n 
         * @param commID ID used in the function call.
         * @exception ERROR_CODE_COMM if client is closed or server replies with error.
         * @exception ERROR_CODE_NO_TOKEN if token has not been assigned yet.
         */
        EXPORTED void commListFunctions(int& commID, int& errorID);

        /**
         * Gets function information (Mostly for debugging purposes).\n 
         * Data format:\n 
         * -(string) Name of function.\n 
         * -(string) Function description.\n 
         * -(string) Version info.\n 
         * -(string) Function author.\n 
         * -(string) Email of function author.\n 
         * -(string) Online documentation reference.\n 
         * @param commID ID used in the function call.
         * @exception ERROR_CODE_COMM if client is closed or server replies with error.
         * @exception ERROR_CODE_NO_TOKEN if token has not been assigned yet.
         */
        EXPORTED void commGetFunctionInfo(const char* func, int& commID, int& errorID);

        /**
         * Gets workspaces available.\n 
         * Data format:\n 
         * -(string[]) Workspace names.\n 
         * @param commID ID used in the function call.
         * @exception ERROR_CODE_COMM if client is closed or server replies with error.
         * @exception ERROR_CODE_NO_TOKEN if token has not been assigned yet.
         */
        EXPORTED void commListWorkspaces(int& commID, int& errorID);

        /**
         * Tries to add workspace.
         * Data format:\n 
         * -None.\n 
         * @param workspace Name of workspace to add.
         * @return Success of adding workspace.
         */
        EXPORTED bool commAddWorkspace(const char* workspace);

        /**
         * Tries to remove workspace.
         * Data format:\n 
         * -None.\n 
         * @param workspace Name of workspace to remove.
         * @return Success of removing workspace.
         */
        EXPORTED bool commRmWorkspace(const char* workspace);

        /**
         * Sends a generic message to the server. Should not be used to call a function already implemented in DLL.
         * Data format:\n 
         * -(string) Response.\n 
         * @param msg Json message.
         * @param len Length of message. Default to 0 is using msg only.
         * @param commID ID used in the function call.
         * @exception ERROR_CODE_COMM if client is closed or server replies with error.
         * @exception ERROR_CODE_NO_TOKEN if token has not been assigned yet.
         */
        EXPORTED void commGeneric(const char* msg, const int& len, int& commID, int& errorID);

        /**
         * Gets sender streams available based on parameters.\n 
         * Data format:\n 
         * -(int[]) Stream IDs.\n 
         * @param workspaces List of workspaces to include in data. Leave empty for all.
         * @param types List of types to include in data. Leave empty for all.
         * @param commID ID used in the function call.
         * @exception ERROR_CODE_COMM if client is closed or server replies with error.
         * @exception ERROR_CODE_NO_TOKEN if token has not been assigned yet.
         */
        EXPORTED void commListStreams(const char** workspaces, int workspacesLen, const char** types,
            int typesLen, int& commID, int& errorID);
        
        /**
         * Gets information on stream based on ID.\n 
         * Data format:\n 
         * -Stream Data.
         * @param streamID StreamID to look up.
         * @param commID ID used in the function call.
         * @exception ERROR_CODE_COMM if client is closed or server replies with error.
         * @exception ERROR_CODE_NO_TOKEN if token has not been assigned yet.
         */
        EXPORTED void commGetStreamInfo(const STREAM_ID& streamID, int& commID, int& errorID);

        /**
         * Attempts to create a sender stream.\n 
         * Data format:\n 
         * -Stream Data.\n 
         * @param workspace Workspace being used for stream.
         * @param type Type of information stream is sending.
         * @param meta Metadata attached to stream.
         * @param echo Can client listen to itself.
         * @param alert Recieve server updates for new streams.
         * @param protocol Protocol used by stream.
         * @param commID ID used in the function call.
         * @exception ERROR_CODE_COMM if client is closed or server replies with error.
         * @exception ERROR_CODE_NO_TOKEN if token has not been assigned yet.
         */
        EXPORTED void commAddSender(const char* workspace, const char* type, const char* meta,
            bool echo, bool alert, int protocol, int& commID, int& errorID);

        /**
         * Attempts to create a receiver stream.\n 
         * Data format:\n 
         * -Stream Data.\n 
         *
         * @param workspace workspace being used for stream.
         * @param types types of information stream is listening to.
         * @param typeLen amount of types provided
         * @param meta Metadata attached to stream.
         * @param echo Can client listen to itself.
         * @param alert Recieve server updates for new streams.
         * @param protocol Protocol used by stream.
         * @param commID Identifier used to match data or error to retrieve. 
         * @exception ERROR_CODE_COMM if client is closed or server replies with error.
         * @exception ERROR_CODE_NO_TOKEN if token has not been assigned yet.
         */
        EXPORTED void commAddReceiver(const char* workspace, const char** types, int typeLen, const char* meta,
            bool echo, bool alert, int protocol, int& commID, int& errorID);
        
        /**
         * Subcribes a reciever to a new sender.
         * 
         * @param receiverID Reciever stream to listen to new sender.
         * @param senderID Sender stream to listen to.
         * @return Success or failure upon subscribing to stream.
         */
        EXPORTED bool commSubscribe(const STREAM_ID& receiverID, const STREAM_ID& senderID);

        /**
         * Unsubcribes a reciever from a sender.
         * 
         * @param receiverID Reciever stream to stop listening to sender.
         * @param senderID Sender stream to stop listening to.
         * @return Success or failure upon unsubscribing from stream.
         */
        EXPORTED bool commUnsubscribe(const STREAM_ID& receiverID, const STREAM_ID& senderID);

    }
}
#endif