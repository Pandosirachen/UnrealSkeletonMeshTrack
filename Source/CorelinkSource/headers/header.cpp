#include "corelink/headers/header.h"

namespace CorelinkDLL {
    EXPORTED const int STREAM_STATE_NONE = (int) StreamState::NONE;
    EXPORTED const int STREAM_STATE_SEND_UDP = (int) StreamState::SEND_UDP;
    EXPORTED const int STREAM_STATE_RECV_UDP = (int) StreamState::RECV_UDP;
    EXPORTED const int STREAM_STATE_SEND_TCP = (int) StreamState::SEND_TCP;
    EXPORTED const int STREAM_STATE_RECV_TCP = (int) StreamState::RECV_TCP;
    EXPORTED const int STREAM_STATE_SEND_WS = (int) StreamState::SEND_WS;
    EXPORTED const int STREAM_STATE_RECV_WS = (int) StreamState::RECV_WS;
    EXPORTED const int STREAM_STATE_UDP = (int) StreamState::UDP;
    EXPORTED const int STREAM_STATE_TCP = (int) StreamState::TCP;
    EXPORTED const int STREAM_STATE_WS = (int) StreamState::WS;
    EXPORTED const int STREAM_STATE_SEND = (int) StreamState::SEND;
    EXPORTED const int STREAM_STATE_RECV = (int) StreamState::RECV;
    EXPORTED const int STREAM_STATE_ALL = (int) StreamState::ALL;

    EXPORTED const int ERROR_CODE_NONE = (int)ErrorCode::ECNONE;
    EXPORTED const int ERROR_CODE_STATE = (int)ErrorCode::ECSTATE;
    EXPORTED const int ERROR_CODE_VALUE = (int)ErrorCode::ECVALUE;
    EXPORTED const int ERROR_CODE_SOCKET = (int)ErrorCode::ECSOCKET;
    EXPORTED const int ERROR_CODE_COMM = (int)ErrorCode::ECCOMM;
    EXPORTED const int ERROR_CODE_NO_TOKEN = (int)ErrorCode::ECNOTOKEN;

    EXPORTED const int CALLBACK_DROPPED = (int)ServerCallback::DROPPED;
    EXPORTED const int CALLBACK_STALE = (int)ServerCallback::STALE;
    EXPORTED const int CALLBACK_SUBSCRIBE = (int)ServerCallback::SUBSCRIBE;
    EXPORTED const int CALLBACK_UPDATE = (int)ServerCallback::UPDATE;
}

namespace CorelinkDLL {
    bool isPow2(int val) {
        return val > 0 && (val & (val - 1)) == 0;
    }

    static const std::map<std::string, ServerCallback> callbackMap = {
        {"dropped", ServerCallback::DROPPED},
        {"stale", ServerCallback::STALE},
        {"subscriber", ServerCallback::SUBSCRIBE},
        {"update", ServerCallback::UPDATE}
    };

    ServerCallback strToCallback(const std::string& callback) {
        std::map<std::string, ServerCallback>::const_iterator it;
        return (it = callbackMap.find(callback)) != callbackMap.end() ? it->second : ServerCallback::LAST;
    }

    EXPORTED void freeData(char* buffer) {
        delete[] buffer;
    }

    EXPORTED int streamStateToBitIndex(int state) {
        return (isPow2(state) && state < STREAM_STATE_ALL) ? (int)log2(state) : -1;
    }

    static const std::vector<const char*> streamName = {
        "udp",
        "udp",
        "tcp",
        "tcp",
        "ws",
        "ws"
    };

    EXPORTED const char* streamStateName(int state) {
        int index = streamStateToBitIndex(state);
        return index == -1 ? "" : streamName[index];
    }

    EXPORTED int streamStateNameLen(int state) {
        int index = streamStateToBitIndex(state);
        return index == -1 ? 0 : strlen(streamName[index]) + 1;
    }

    EXPORTED void streamStateNameStr(int state, char* buffer) {
        int index = streamStateToBitIndex(state);
        if (index == -1) { return; }
        strcpy(buffer, streamName[index]);
    }

    static const std::map<int, const char*> errorMap = {
        {ERROR_CODE_NONE, "UNKNOWN ERROR"},
        {ERROR_CODE_STATE, "STATE ERROR"},
        {ERROR_CODE_VALUE, "VALUE ERROR"},
        {ERROR_CODE_SOCKET, "SOCKET ERROR"},
        {ERROR_CODE_COMM, "COMMUNICATION ERROR"},
        {ERROR_CODE_NO_TOKEN, "NO TOKEN ERROR"}
    };

    EXPORTED const char* errorCodeName(int errorCode) {
        std::map<int, const char*>::const_iterator it;
        return (it = errorMap.find(errorCode)) != errorMap.end() ? it->second : "";
    }

    EXPORTED int errorCodeNameLen(int errorCode) {
        std::map<int, const char*>::const_iterator it;
        return (it = errorMap.find(errorCode)) != errorMap.end() ? strlen(it->second) + 1 : 0;
    }

    EXPORTED void errorCodeNameStr(int errorCode, char* buffer) {
        std::map<int, const char*>::const_iterator it;
        it = errorMap.find(errorCode);
        if (it == errorMap.end()) { return; }
        strcpy(buffer, it->second);
    }
}
