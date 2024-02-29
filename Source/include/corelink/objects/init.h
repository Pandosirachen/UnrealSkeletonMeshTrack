/**
 * @file init.h
 * @brief Handles one time initialization of the client data.
 * Also handles passing error messages to the wrapper.
 */
#ifndef CORELINK_OBJECTS_INIT_H
#define CORELINK_OBJECTS_INIT_H

#include "corelink/headers/header.h"
#include "corelink/objects/initialization_data.h"

/*
 * Initialization logic here.
 */
namespace CorelinkDLL {
    /**
     * Data stored for handling startup of the dll.
     */
    extern CorelinkDLL::Object::initialization_data initData;

    extern "C" {
        /**
         * Call this once in the beginning.
         * TODO: test on different platforms and find out if it is necessary to explicitly initialize initData.
         * Otherwise, can leave it default initialized
         */
        EXPORTED void DLLInit();

        /**
         * Used to check if the client is on.
         */
        EXPORTED bool getClientState();

        /**
         * Gets the state for components the client will initialize with.
         * @return Currently assigned state value.
         */
        EXPORTED int getInitState();

        /**
         * Sets the state for components the client will initialize with.
         * @param state State determining which components to use.
         * @exception ERROR_CODE_VALUE if state value is invalid.
         */
        EXPORTED void setInitState(int state, int& errorID);

        /**
         * Gets the certificate path for the local server.
         * @param len Stores the length of the data.
         * @return Certificate path currently assigned.
         */
        EXPORTED char* getInitLocalCertPath(int& len);

        /**
         * Gets the length of the certificate path for the local server.
         * @return Length of the certificate path.
         */
        EXPORTED int getInitLocalCertPathLen();

        /**
         * Gets the certificate path for the local server.
         * @param buffer Buffer to store the certificate path.
         */
        EXPORTED void getInitLocalCertPathStr(char* buffer);

        /**
         * Sets the local server certificate path.
         * @param path New path value.
         * @param len Length of the path to copy.
         */
        EXPORTED void setInitLocalCertPath(const char* path, int len);

        /**
         * Gets the certificate path for the server.
         * @param len Stores the length of the data.
         * @return Certificate path currently assigned.
         */
        EXPORTED char* getInitServerCertPath(int& len);

        /**
         * Gets the length of the certificate path for the server.
         * @return Length of the certificate path.
         */
        EXPORTED int getInitServerCertPathLen();

        /**
         * Gets the certificate path for the server.
         * @param buffer Buffer to store the certificate path.
         */
        EXPORTED void getInitServerCertPathStr(char* buffer);

        /**
         * Sets the server certificate path.
         * @param path New path value.
         * @param len Length of the path to copy.
         */
        EXPORTED void setInitServerCertPath(const char* path, int len);

        /**
         * Sets the Corelink server login credentials.
         * @param username Corelink username credentials.
         * @param password Corelink password credentials.
         */
        EXPORTED void setServerCredentials(const char* username, const char* password);

        /**
         * Sets the callback for Corelink server dropped messages.
         * @param func Reference to a function that will handle the message.
         */
        EXPORTED void setDroppedCallbackHandler(void (*func)(const int&));

        /**
         * Sets the callback for Corelink server stale messages.
         * @param func Reference to a function that will handle the message.
         */
        EXPORTED void setStaleCallbackHandler(void (*func)(const int&));

        /**
         * Sets the callback for Corelink server subscribe messages.
         * @param func Reference to a function that will handle the message.
         */
        EXPORTED void setSubscribeCallbackHandler(void (*func)(const int&, const int&));

        /**
         * Sets the callback for Corelink server update messages.
         * @param func Reference to a function that will handle the message.
         */
        EXPORTED void setUpdateCallbackHandler(void (*func)(const int&, const int&));
    }
}

/*
 * Error handling logic here.
 */
namespace CorelinkDLL {
    unsigned int addError(const std::string& error, int errorCode);

    extern "C" {
        /**
         * Gets error message from the dll to the client
         * @param errorID ID of the error to get.
         * @param errorLen Length of the error message.
         * @return error message.
         */
        EXPORTED char* getError(const unsigned int& errorID, int& errorLen);

        /**
         * Gets length of the error message from the dll to the client
         * @param errorID ID of the error length to get.
         * @return length of error message.
         */
        EXPORTED int getErrorLen(const unsigned int& errorID);

        /**
         * Gets error message from the dll to the client
         * @param errorID ID of the error to get.
         * @param buffer Buffer to store the error message.
         * @return Succeed in retrieving error message.
         */
        EXPORTED bool getErrorStr(const unsigned int& errorID, char* buffer);
    }
}

#endif