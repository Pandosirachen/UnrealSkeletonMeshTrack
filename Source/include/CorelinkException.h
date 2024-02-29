/**
 * @file CorelinkException.h
 * @brief Handles the underlying DLL error messages
 */
#ifndef CORELINKEXCEPTION_H
#define CORELINKEXCEPTION_H
#pragma once

#include "CorelinkClasses.h"
#include "CorelinkConst.h"

namespace Corelink {

    inline CorelinkException::CorelinkException(const std::string& msg, const int& code) :
        std::runtime_error(Corelink::Const::ErrorCodeString[code] + ": " + msg),
        msg(msg), code(code)
    {}

    inline void CorelinkException::GetDLLException(const unsigned int errorID) {
        if (errorID == 0) { return; }
        int errorLen;
        char* errorMsg;
    #ifdef WRAPPER_ALLOC
        errorLen = CorelinkDLL::getErrorLen(errorID);
        if (errorLen < 0) { throw Corelink::CorelinkException("No message remaining. Client closed", Corelink::Const::ERROR_CODE_NONE); }
        errorMsg = new char[errorLen];
        if (!CorelinkDLL::getErrorStr(errorID, errorMsg)) {
            delete[] errorMsg;
            throw Corelink::CorelinkException("No message remaining. Client closed", Corelink::Const::ERROR_CODE_NONE);
        }
        Corelink::CorelinkException exception(std::string(errorMsg, errorLen - 1), (int)errorMsg[errorLen - 1]);
        delete[] errorMsg;
        throw exception;
    #else
        errorMsg = CorelinkDLL::getError(errorID, errorLen);
        if (errorLen == 0) { throw Corelink::CorelinkException("No message remaining. Client closed", Corelink::Const::ERROR_CODE_NONE); }
        Corelink::CorelinkException exception = Corelink::CorelinkException(std::string(errorMsg, errorLen - 1), (int)errorMsg[errorLen - 1]);
        CorelinkDLL::freeData(errorMsg);
        throw exception;
    #endif
    }

    inline CorelinkException::~CorelinkException() {}

    inline CorelinkException::CorelinkException(const CorelinkException& rhs) :
        std::runtime_error(Corelink::Const::ErrorCodeString[rhs.code] + ": " + rhs.msg),
        msg(rhs.msg), code(rhs.code)
    {}
}

#endif