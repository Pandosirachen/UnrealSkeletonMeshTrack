#include "corelink/objects/init.h"

namespace CorelinkDLL {
    CorelinkDLL::Object::initialization_data initData;
    
    EXPORTED void DLLInit() {
        initData = CorelinkDLL::Object::initialization_data();
    }

    EXPORTED bool getClientState() {
        std::lock_guard<std::mutex> lck(initData.lock);
        return initData.clientInit;
    }

    EXPORTED int getInitState() {
        std::lock_guard<std::mutex> lck(initData.lock);
        return initData.initState;
    }

    EXPORTED void setInitState(int state, int& errorID) {
        errorID = 0;
        if (state <= STREAM_STATE_NONE || state > STREAM_STATE_ALL) {
            errorID = addError("init.cpp setInitState: Invalid state value " + std::to_string(state), ERROR_CODE_VALUE);
            return;
        }
        std::lock_guard<std::mutex> lck(initData.lock);
        initData.initState = state;
    }

    EXPORTED char* getInitLocalCertPath(int& len) {
        char* buffer;
        std::lock_guard<std::mutex> lck(initData.lock);
        len = (int) initData.certClientFileName.size();
        buffer = new char[len];
        memcpy(buffer, initData.certClientFileName.c_str(), len);
        return buffer;
    }

    EXPORTED int getInitLocalCertPathLen() {
        std::lock_guard<std::mutex> lck(initData.lock);
        return (int) initData.certClientFileName.size();
    }

    EXPORTED void getInitLocalCertPathStr(char* buffer) {
        std::lock_guard<std::mutex> lck(initData.lock);
        memcpy(buffer, initData.certClientFileName.c_str(), initData.certClientFileName.size());
    }

    EXPORTED void setInitLocalCertPath(const char* path, int len) {
        std::lock_guard<std::mutex> lck(initData.lock);
        initData.certClientFileName = std::string(path, len);
    }

    EXPORTED char* getInitServerCertPath(int& len) {
        char* buffer;
        std::lock_guard<std::mutex> lck(initData.lock);
        len = (int) initData.certServerFileName.size();
        buffer = new char[len];
        memcpy(buffer, initData.certServerFileName.c_str(), len);
        return buffer;
    }

    EXPORTED int getInitServerCertPathLen() {
        std::lock_guard<std::mutex> lck(initData.lock);
        return (int) initData.certServerFileName.size();
    }

    EXPORTED void getInitServerCertPathStr(char* buffer) {
        std::lock_guard<std::mutex> lck(initData.lock);
        memcpy(buffer, initData.certServerFileName.c_str(), initData.certServerFileName.size());
    }

    EXPORTED void setInitServerCertPath(const char* path, int len) {
        std::lock_guard<std::mutex> lck(initData.lock);
        initData.certServerFileName = std::string(path, len);
    }

    EXPORTED void setServerCredentials(const char* username, const char* password) {
        std::lock_guard<std::mutex> lck(initData.lock);
        initData.username = std::string(username);
        initData.password = std::string(password);
    }

    EXPORTED void setDroppedCallbackHandler(void (*func)(const int&)) {
        std::lock_guard<std::mutex> lck(initData.lock);
        initData.onDropHandler = func;
    }

    EXPORTED void setStaleCallbackHandler(void (*func)(const int&)) {
        std::lock_guard<std::mutex> lck(initData.lock);
        initData.onStaleHandler = func;
    }

    EXPORTED void setSubscribeCallbackHandler(void (*func)(const int&, const int&)) {
        std::lock_guard<std::mutex> lck(initData.lock);
        initData.onSubscribeHandler = func;
    }

    EXPORTED void setUpdateCallbackHandler(void (*func)(const int&, const int&)) {
        std::lock_guard<std::mutex> lck(initData.lock);
        initData.onUpdateHandler = func;
    }

}

namespace CorelinkDLL {
    unsigned int addError(const std::string& error, int errorCode) {
        return initData.msgHandler.add(error + char(errorCode));
    }

    EXPORTED char* getError(const unsigned int& errorID, int& errorLen) {
        std::string error;
        char* buffer;
        error = initData.msgHandler.get(errorID);
        errorLen = (int) error.size();
        buffer = new char[errorLen];
        memcpy(buffer, error.c_str(), errorLen);
        return buffer;
    }

    EXPORTED int getErrorLen(const unsigned int& errorID) {
        return initData.msgHandler.getLen(errorID);
    }
    
    EXPORTED bool getErrorStr(const unsigned int& errorID, char* buffer) {
        bool success = initData.msgHandler.getStr(errorID, buffer);
        initData.msgHandler.remove(errorID);
        return success;
    }
}