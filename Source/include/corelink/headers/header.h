/**
 * @file header.h
 * @brief Header combining the header files and listing constants/enums.
 */

#ifndef CORELINK_HEADERS_HEADER_H
#define CORELINK_HEADERS_HEADER_H

#include "corelink/headers/common.h"
#include "corelink/headers/external.h"
#include "corelink/headers/macro.h"
#include "corelink/headers/network.h"

/**
 * Enums are declared here.
 */
namespace CorelinkDLL {
    /**
     * Index of bit for respective StreamState with the same name.
     */
    enum class StreamStateBitIndex {
        SEND_UDP = 0,
        RECV_UDP = 1,
        SEND_TCP = 2,
        RECV_TCP = 3,
        SEND_WS = 4,
        RECV_WS = 5,
        LAST
    };

    /**
     * Determines which componenents of Corelink Library to use.
     * Also determines the type of the current stream being used.
     */
    enum class StreamState
    {
        NONE = 0,
        SEND_UDP = 1 << (int) StreamStateBitIndex::SEND_UDP,
        RECV_UDP = 1 << (int) StreamStateBitIndex::RECV_UDP,
        SEND_TCP = 1 << (int) StreamStateBitIndex::SEND_TCP,
        RECV_TCP = 1 << (int) StreamStateBitIndex::RECV_TCP,
        SEND_WS = 1 << (int) StreamStateBitIndex::SEND_WS,
        RECV_WS = 1 << (int) StreamStateBitIndex::RECV_WS,
        SEND = SEND_UDP | SEND_TCP | SEND_WS,
        RECV = RECV_UDP | RECV_TCP | RECV_WS,
        UDP = SEND_UDP | RECV_UDP,
        TCP = SEND_TCP | RECV_TCP,
        WS = SEND_WS | RECV_WS,
        ALL = SEND | RECV
    };

    /**
     * Error codes for external use.
     */
    enum class ErrorCode {
        // Unknown error/client shutdown.
        ECNONE = 0,
        // Conflicting state within the dll.
        ECSTATE,
        // Invalid values passed from wrapper to dll.
        ECVALUE, 
        // Socket errors. (internal)
        ECSOCKET, 
        // Server protocol errors. (external)
        ECCOMM, 
        // Client not initialized properly (hasn't recieved token from the server yet).
        ECNOTOKEN,
        LAST
    };

    /**
     * Callback codes for external use.
     */
    enum class ServerCallback{
        DROPPED,
        STALE,
        SUBSCRIBE,
        UPDATE,
        LAST
    };
}

/**
 * Constants are declared here.
 */
namespace CorelinkDLL {
    extern "C" {
        // Stream state
        extern EXPORTED const int STREAM_STATE_NONE;
        extern EXPORTED const int STREAM_STATE_SEND_UDP;
        extern EXPORTED const int STREAM_STATE_RECV_UDP;
        extern EXPORTED const int STREAM_STATE_SEND_TCP;
        extern EXPORTED const int STREAM_STATE_RECV_TCP;
        extern EXPORTED const int STREAM_STATE_SEND_WS;
        extern EXPORTED const int STREAM_STATE_RECV_WS;
        extern EXPORTED const int STREAM_STATE_UDP;
        extern EXPORTED const int STREAM_STATE_TCP;
        extern EXPORTED const int STREAM_STATE_WS;
        extern EXPORTED const int STREAM_STATE_SEND;
        extern EXPORTED const int STREAM_STATE_RECV;
        extern EXPORTED const int STREAM_STATE_ALL;

        extern EXPORTED const int ERROR_CODE_NONE;
        extern EXPORTED const int ERROR_CODE_STATE;
        extern EXPORTED const int ERROR_CODE_VALUE;
        extern EXPORTED const int ERROR_CODE_SOCKET;
        extern EXPORTED const int ERROR_CODE_COMM;
        extern EXPORTED const int ERROR_CODE_NO_TOKEN;

        extern EXPORTED const int CALLBACK_DROPPED;
        extern EXPORTED const int CALLBACK_STALE;
        extern EXPORTED const int CALLBACK_SUBSCRIBE;
        extern EXPORTED const int CALLBACK_UPDATE;
    }
}

/**
 * Helper functions with constants are declared here.
 */
namespace CorelinkDLL {
    /**
     * @private
     * Helper function to check if value is a power of 2.
     * @returns If value provided is a power of 2.
     */
    bool isPow2(int val);

    ServerCallback strToCallback(const std::string& callback);

    extern "C" {
        /**
         * Deletes the array to free heap. Only use on certain functions.
         * @param buffer Array to free.
         */
        EXPORTED void freeData(char* buffer);

        /**
         * Gets bit index of state.
         * @param state State of the stream.
         * @returns -1 if not a single stream state. Else, corresponding StreamStateBitIndex value.
         */
        EXPORTED int streamStateToBitIndex(int state);

        /**
         * Gets the name of the stream state.
         * @param state State of the stream.
         * @returns "" if not a single stream state. Else, corresponding name.
         */
        EXPORTED const char* streamStateName(int state);

        /**
         * Gets the length of the stream state.
         * @param state State of the stream.
         * @returns 0 if not a single stream state. Else, corresponding name length.
         */
        EXPORTED int streamStateNameLen(int state);

        /**
         * Gets the name of the stream state.
         * @param state State of the stream.
         * @param buffer Buffer to store the name of the stream.
         */
        EXPORTED void streamStateNameStr(int state, char* buffer);

        /**
         * Gets the name of the corresponding error code.
         * @param errorCode Error code value.
         * @returns "" if not a valid error code. Else, name of the error code.
         */
        EXPORTED const char* errorCodeName(int errorCode);

        /**
         * Gets the length of the name of the corresponding error code.
         * @param errorCode Error code value.
         * @returns "" if not a valid error code. Else, name of the error code.
         */
        EXPORTED int errorCodeNameLen(int errorCode);

        /**
         * Gets the name of the corresponding error code.
         * @param errorCode Error code value.
         * @param buffer Buffer to store the name of the stream.
         */
        EXPORTED void errorCodeNameStr(int errorCode, char* buffer);
    }
}

#endif
