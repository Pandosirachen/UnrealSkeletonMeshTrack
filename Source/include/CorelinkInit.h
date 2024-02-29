/**
 * @file CorelinkInit.h
 * Copies the initialization from the DLL to wrapper
 */
#ifndef CORELINKINIT_H
#define CORELINKINIT_H
#pragma once

#include "CorelinkConst.h"
#include "CorelinkClasses.h"
#include "CorelinkException.h"

namespace Corelink {
    inline void DLLInit::Init() {
        CorelinkDLL::DLLInit();
        CorelinkDLL::setDroppedCallbackHandler(nullptr);
        CorelinkDLL::setStaleCallbackHandler(nullptr);
        CorelinkDLL::setSubscribeCallbackHandler(nullptr);
        CorelinkDLL::setUpdateCallbackHandler(nullptr);
    }

    inline int DLLInit::getInitState() {
        return CorelinkDLL::getInitState();
    }

    inline void DLLInit::setInitState(int state) {
        int errorID;
        CorelinkDLL::setInitState(state, errorID);
        CorelinkException::GetDLLException(errorID);
    }

    inline std::string DLLInit::getLocalCertPath() {
        char* data;
        std::string path;
        int len;
    #ifdef WRAPPER_ALLOC
        len = CorelinkDLL::getInitLocalCertPathLen();
        if (len == 0) { return ""; }
        data = new char[len];
        CorelinkDLL::getInitLocalCertPathStr(data);
        path = std::string(data, len);
        delete[] data;
        return path;
    #else
        data = CorelinkDLL::getInitLocalCertPath(len);
        path = std::string(data, len);
        CorelinkDLL::freeData(data);
        return path;
    #endif
    }

    inline void DLLInit::setLocalCertPath(const char* path, int len) {
        CorelinkDLL::setInitLocalCertPath(path, len);
    }

    inline void DLLInit::setLocalCertPath(const std::string& path) {
        setLocalCertPath(path.c_str(), (int) path.size());
    }

    inline std::string DLLInit::getServerCertPath() {
        char* data;
        std::string path;
        int len;
    #ifdef WRAPPER_ALLOC
        len = CorelinkDLL::getInitServerCertPathLen();
        if (len == 0) { return ""; }
        data = new char[len];
        CorelinkDLL::getInitServerCertPathStr(data);
        path = std::string(data, len);
        delete[] data;
        return path;
    #else
        data = CorelinkDLL::getInitServerCertPath(len);
        path = std::string(data, len);
        CorelinkDLL::freeData(data);
        return path;
    #endif
    }

    inline void DLLInit::setServerCertPath(const char* path, int len) {
        CorelinkDLL::setInitServerCertPath(path, len);
    }

    inline void DLLInit::setServerCertPath(const std::string& path) {
        setServerCertPath(path.c_str(), (int)path.size());
    }

    inline void DLLInit::setServerCredentials(const std::string& username, const std::string& password) {
        CorelinkDLL::setServerCredentials(username.c_str(), password.c_str());
    }

    inline void DLLInit::setOnDrop(void(*func)(const STREAM_ID&)) {
        CorelinkDLL::setDroppedCallbackHandler(func);
    }

    inline void DLLInit::setOnStale(void(*func)(const STREAM_ID&)) {
        CorelinkDLL::setStaleCallbackHandler(func);
    }

    inline void DLLInit::setOnSubscribe(void(*func)(const STREAM_ID&, const STREAM_ID&)) {
        CorelinkDLL::setSubscribeCallbackHandler(func);
    }

    inline void DLLInit::setOnUpdate(void(*func)(const STREAM_ID&, const STREAM_ID&)) {
        CorelinkDLL::setUpdateCallbackHandler(func);
    }
}
#endif