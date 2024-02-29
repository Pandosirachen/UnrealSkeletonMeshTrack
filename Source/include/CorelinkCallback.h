/**
 * @file CorelinkCallback.h
 * Containers for differenct callback function to handle the dll callbacks.
 */
#ifndef CORELINKCALLBACK_H
#define CORELINKCALLBACK_H

#include "CorelinkClasses.h"

namespace Corelink {
    inline void Callback::RecvCallback(STREAM_ID recvID, STREAM_ID sendID, const char* msg, int jsonLen, int msgLen, void* callback) {
        Callback* callbackFunc = (Callback*)callback;
        if (callbackFunc == nullptr) { return; }
        callbackFunc->Func(RecvData(recvID, sendID, msg, jsonLen, msgLen));
    }

    /**
     * CallbackData
     */

    inline CallbackData::CallbackData(void(*func)(const STREAM_ID&, const STREAM_ID&, const char*, const int&)) {
        this->func = func;
    }

    inline CallbackData::~CallbackData() {}

    inline void CallbackData::Func(const RecvData& recvData) {
        this->func(recvData.recvID, recvData.sendID, recvData.data + recvData.hdrLen, recvData.msgLen);
    }

    inline void CallbackData::RecvCallback(STREAM_ID recvID, STREAM_ID sendID, const char* msg, int jsonLen, int msgLen, void* callback) {
        ((CallbackData*)callback)->func(recvID, sendID, msg + jsonLen, msgLen);
    }

    /**
     * CallbackDataVoid
     */

    inline CallbackDataVoid::CallbackDataVoid(void(*func)(void*, const STREAM_ID&, const STREAM_ID&, const char*, const int&), void* obj) {
        this->func = func;
        this->obj = obj;
    }

    inline CallbackDataVoid::~CallbackDataVoid() {
        this->obj = nullptr;
    }

    inline void CallbackDataVoid::Func(const RecvData& recvData) {
        this->func(this->obj, recvData.recvID, recvData.sendID, recvData.data + recvData.hdrLen, recvData.msgLen);
    }

    inline void CallbackDataVoid::RecvCallback(STREAM_ID recvID, STREAM_ID sendID, const char* msg, int jsonLen, int msgLen, void* callback) {
        ((CallbackDataVoid*)callback)->func(((CallbackDataVoid*)callback)->obj, recvID, sendID, msg + jsonLen, msgLen);
    }

    /**
     * CallbackDataJson
     */

    inline CallbackDataJson::CallbackDataJson(void(*func)(const STREAM_ID&, const STREAM_ID&, const char*, const int&, const rapidjson::Document&)) {
        this->func = func;
    }

    inline CallbackDataJson::~CallbackDataJson() {}

    inline void CallbackDataJson::Func(const RecvData& recvData) {
        rapidjson::Document json;
        json.SetObject();
        if (recvData.hdrLen > 0) {
            json.Parse(recvData.data, recvData.hdrLen);
        }
        this->func(recvData.recvID, recvData.sendID, recvData.data + recvData.hdrLen, recvData.msgLen, json);
    }

    inline void CallbackDataJson::RecvCallback(STREAM_ID recvID, STREAM_ID sendID, const char* msg, int jsonLen, int msgLen, void* callback) {
        rapidjson::Document json;
        json.SetObject();
        if (jsonLen > 0) {
            json.Parse(msg, jsonLen);
        }
        ((CallbackDataJson*)callback)->func(recvID, sendID, msg + jsonLen, msgLen, json);
    }

    /**
     * CallbackDataJsonVoid
     */

    inline CallbackDataJsonVoid::CallbackDataJsonVoid(void(*func)(void*, const STREAM_ID&, const STREAM_ID&, const char*, const int&, const rapidjson::Document&), void* obj) {
        this->func = func;
        this->obj = obj;
    }

    inline CallbackDataJsonVoid::~CallbackDataJsonVoid() {
        this->obj = nullptr;
    }

    inline void CallbackDataJsonVoid::Func(const RecvData& recvData) {
        rapidjson::Document json;
        json.SetObject();
        if (recvData.hdrLen > 0) {
            json.Parse(recvData.data, recvData.hdrLen);
        }
        this->func(this->obj, recvData.recvID, recvData.sendID, recvData.data + recvData.hdrLen, recvData.msgLen, json);
    }

    inline void CallbackDataJsonVoid::RecvCallback(STREAM_ID recvID, STREAM_ID sendID, const char* msg, int jsonLen, int msgLen, void* callback) {
        rapidjson::Document json;
        json.SetObject();
        if (jsonLen > 0) {
            json.Parse(msg, jsonLen);
        }
        ((CallbackDataJsonVoid*)callback)->func(((CallbackDataJsonVoid*)callback)->obj, recvID, sendID, msg + jsonLen, msgLen, json);
    }
}

#endif