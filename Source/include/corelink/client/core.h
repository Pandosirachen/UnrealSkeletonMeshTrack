/**
 * @file core.h
 * @brief Header containing commands for initialization and communication of Corelink client.
 * Functions necessary for communication between different components will be exposed here.
 */
#ifndef CORELINK_CLIENT_CORE_H
#define CORELINK_CLIENT_CORE_H

#include "corelink/headers/header.h"
#include "corelink/objects/client_main.h"

/**
 * @private
 * Definitions located in core.cpp
 */
namespace CorelinkDLL {
    /**
     * Contains all of the necessary components for dll functioning.
     * All functions will not check if it is initialized unless explicitly stated.
     */
    extern CorelinkDLL::Object::client_main* client;

    /**
     * Helper function to make a string safe for Json messages.
     * TODO: optimize this function if possible.
     * @param str Message to parse.
     * @param len Length of message if known.
     * @return String safe to use directly in string streams.
     */
    std::string safeString(const char* str, int len = -1);

    /**
     * Function exposed to send data to main server.
     * @param msg Data sent to server.
     * @param len Length of message to be sent.
     * @return Success of sending message.
     */
    bool mainCommSend(char* msg, int len);

    /**
     * Reserves a unique commID to use.
     * @return unique value.
     */
    int mainCommGetCommID();

    /**
     * Helper function to send and retrieve json as well as call error messages.
     * @param fileName File name for error source.
     * @param funcName Function name for error.
     * @param msg Json message to send to server.
     * @param json Stores response json.
     * @param commID Unique identifier to retrieve response.
     * @param checkToken Should token be checked.
     * @result Boolean indicating success.
     */
    bool getJsonResponse(const std::string& fileName, const std::string& funcName, const std::string& msg,
            rapidjson::Document& json, const int& commID, int& errorID, bool checkToken = true);

    /**
     * Function exposed for main server communication commands.
     * @param msg Data sent to server.
     * @param len Length of message to be sent.
     * @param commID Unique identifier associated with the command response.
     * @return Json response from the server.
     */
    rapidjson::Document mainCommSendRecv(char* msg, int len, const int& commID);

    /**
     * @private
     * Attempts to connect to the Corelink server using information from client.data.
     */
    void commConnect(int& errorID);

    /**
     * @private
     * Attempts to connect to the Corelink server using information plugin key.
     */
    void commConnectPlugin(const std::string& token, int& errorID);

    extern "C"
    {
        /**
         * Gets the token assigned to the current session.
         * @param len Stores the length of the token.
         * @return Token string.
         */
        EXPORTED char* getToken(int& len);

        /**
         * Gets the length of the token.
         * @return Length of the token.
         */
        EXPORTED int getTokenLen();

        /**
         * Gets the token assigned to the current session.
         * @param buffer Buffer to store the token.
         */
        EXPORTED void getTokenStr(char* buffer);

        /**
         * Tries to start up the client and connect to the server..
         * @param serverIP Corelink server ip.
         * @param errorLen Corelink server port.
         * @exception ERROR_CODE_STATE if client is already initialized.
         * @exception ERROR_CODE_SOCKET if connection issues occur.
         * @exception ERROR_CODE_COMM if credentials are incorrect.
         */
        EXPORTED void corelinkConnect(const char* serverIP, const int serverPort, int& errorID);

        /**
         * Tries to start up the client and connect to the server..
         * @param serverIP Corelink server ip.
         * @param errorLen Corelink server port.
         * @exception ERROR_CODE_STATE if client is already initialized.
         * @exception ERROR_CODE_SOCKET if connection issues occur.
         * @exception ERROR_CODE_COMM if credentials are incorrect.
         * @exception ERROR_CODE_VALUE if environment variables not found.
         */
        EXPORTED void corelinkConnectPlugin(int& errorID);

        /**
         * Closes client and cleans up components.
         * Does nothing if the client is not initialized.
         */
        EXPORTED void corelinkCleanup();

        /**
         * Removes a stream owned by the client. Only removes one stream at a time.
         * TODO: Include implementation that removes specified workspace and type of stream.
         * 
         * @param streamID Stream to remove from the server.
         * @return Success of removal.
         */
        EXPORTED bool commDisconnect(const STREAM_ID& streamID);
    }
}

#endif