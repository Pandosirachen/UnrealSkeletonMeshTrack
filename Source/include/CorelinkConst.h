/**
 * @file CorelinkConst.h
 * Copies the constants from the DLL to wrapper
 */
#ifndef CORELINKCONST_H
#define CORELINKCONST_H
#pragma once

/*
Use if it is preferable allocating data from the wrapper side.
May come in handy when dealing with languages other than c++.
Performance is better if this is off, though difference should be negligible.
*/
#define WRAPPER_ALLOC

#include "corelink/corelink_dll.h"

namespace Corelink {
    /**
     * Gets bit index of state.
     * @returns -1 if not a single stream state. Else, corresponding StreamStateBitIndex value.
     */
    static int streamStateToBitIndex(int state) {
        return CorelinkDLL::streamStateToBitIndex(state);
    }

    static std::string streamStateName(int state) {
    #ifdef WRAPPER_ALLOC
        int len;
        char* buffer;
        std::string name;
        len = CorelinkDLL::streamStateNameLen(state);
        if (len == 0) { return ""; }
        buffer = new char[len];
        CorelinkDLL::streamStateNameStr(state, buffer);
        // get rid of null character at end of the string
        name = std::string(buffer, len - 1);
        delete[] buffer;
        return name;
    #else
        return std::string(CorelinkDLL::streamStateName(state));
    #endif
    }

    /**
     * Gets the name of the corresponding error code.
     * It is faster to retrieve the value using Corelink::ErrorCodeString[]
     * @returns "" if not a valid error code. Else, name of the error code.
     */
    static std::string errorCodeName(int errorCode) {
    #ifdef WRAPPER_ALLOC
        int len;
        char* buffer;
        std::string name;
        len = CorelinkDLL::errorCodeNameLen(errorCode);
        if (len == 0) { return ""; }
        buffer = new char[len];
        CorelinkDLL::errorCodeNameStr(errorCode, buffer);
        // get rid of null character at end of the string
        name = std::string(buffer, len - 1);
        delete[] buffer;
        return name;
    #else
        return std::string(CorelinkDLL::errorCodeName(errorCode));
    #endif
    }
}

namespace Corelink {
    namespace Const {
        static const int STREAM_STATE_NONE = CorelinkDLL::STREAM_STATE_NONE;
        static const int STREAM_STATE_SEND_UDP = CorelinkDLL::STREAM_STATE_SEND_UDP;
        static const int STREAM_STATE_RECV_UDP = CorelinkDLL::STREAM_STATE_RECV_UDP;
        static const int STREAM_STATE_SEND_TCP = CorelinkDLL::STREAM_STATE_SEND_TCP;
        static const int STREAM_STATE_RECV_TCP = CorelinkDLL::STREAM_STATE_RECV_TCP;
        static const int STREAM_STATE_SEND_WS = CorelinkDLL::STREAM_STATE_SEND_WS;
        static const int STREAM_STATE_RECV_WS = CorelinkDLL::STREAM_STATE_RECV_WS;
        static const int STREAM_STATE_UDP = CorelinkDLL::STREAM_STATE_UDP;
        static const int STREAM_STATE_TCP = CorelinkDLL::STREAM_STATE_TCP;
        static const int STREAM_STATE_WS = CorelinkDLL::STREAM_STATE_WS;
        static const int STREAM_STATE_SEND = CorelinkDLL::STREAM_STATE_SEND;
        static const int STREAM_STATE_RECV = CorelinkDLL::STREAM_STATE_RECV;
        static const int STREAM_STATE_ALL = CorelinkDLL::STREAM_STATE_ALL;

        static const int ERROR_CODE_NONE = CorelinkDLL::ERROR_CODE_NONE;
        static const int ERROR_CODE_STATE = CorelinkDLL::ERROR_CODE_STATE;
        static const int ERROR_CODE_VALUE = CorelinkDLL::ERROR_CODE_VALUE;
        static const int ERROR_CODE_SOCKET = CorelinkDLL::ERROR_CODE_SOCKET;
        static const int ERROR_CODE_COMM = CorelinkDLL::ERROR_CODE_COMM;
        static const int ERROR_CODE_NO_TOKEN = CorelinkDLL::ERROR_CODE_NO_TOKEN;

        static const int CALLBACK_DROPPED = CorelinkDLL::CALLBACK_DROPPED;
        static const int CALLBACK_STALE = CorelinkDLL::CALLBACK_STALE;
        static const int CALLBACK_SUBSCRIBE = CorelinkDLL::CALLBACK_SUBSCRIBE;
        static const int CALLBACK_UPDATE = CorelinkDLL::CALLBACK_UPDATE;

        static const std::string ErrorCodeString[6] = {
            errorCodeName(0),
            errorCodeName(1),
            errorCodeName(2),
            errorCodeName(3),
            errorCodeName(4),
            errorCodeName(5)
        };
    }
}

#endif