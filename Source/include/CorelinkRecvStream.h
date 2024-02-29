/**
 * @file CorelinkRecvStream.h
 * Handler class for the receiver streams.
 * A container for the stream id and necessary data to call associated functions.
 */
#ifndef CORELINKRECVSTREAM_H
#define CORELINKRECVSTREAM_H

#include "CorelinkClasses.h"

namespace Corelink {
    inline RecvStream::RecvStream(const STREAM_ID& stream, int state, int streamRef) :
        streamID(stream), state(state), streamRef(streamRef) 
    {}

    inline RecvStream::RecvStream(const StreamData& streamData) {
        if ((streamData.state & Const::STREAM_STATE_RECV) > 0) {
            streamID = streamData.streamID;
            state = streamData.state;
            streamRef = streamData.streamRef;
        }
        else {
            streamID = STREAM_DEF;
            state = Const::STREAM_STATE_NONE;
            streamRef = -1;
        }
    }

    inline RecvStream::RecvStream(const RecvStream& rhs) :
        streamID(rhs.streamID), state(rhs.state), streamRef(rhs.streamRef)
    {}

    inline RecvStream::RecvStream(RecvStream&& rhs) {
        std::swap(this->streamID, rhs.streamID);
        std::swap(this->state, rhs.state);
        std::swap(this->streamRef, rhs.streamRef);
    }

    inline RecvStream& RecvStream::operator=(const RecvStream& rhs) {
        streamID = rhs.streamID;
        state = rhs.state;
        streamRef = rhs.streamRef;
        return *this;
    }

    inline RecvStream::operator STREAM_ID() const {
        return streamID;
    }

    inline void RecvStream::setOnReceive() {
        Callback* oldData = (Callback*)CorelinkDLL::setOnRecv(state, streamRef, streamID, nullptr, nullptr);
        if (oldData != nullptr) {
            delete oldData;
        }
    }

    inline void RecvStream::setOnReceive(void(*func)(const STREAM_ID&, const STREAM_ID&, const char*, const int&)) {
        Callback* oldData = (Callback*)CorelinkDLL::setOnRecv(state, streamRef, streamID, CallbackData::RecvCallback, new CallbackData(func));
        if (oldData != nullptr) {
            delete oldData;
        }
    }

    inline void RecvStream::setOnReceive(void(*func)(void*, const STREAM_ID&, const STREAM_ID&, const char*, const int&), void* obj) {
        Callback* oldData = (Callback*)CorelinkDLL::setOnRecv(state, streamRef, streamID, CallbackDataVoid::RecvCallback, new CallbackDataVoid(func, obj));
        if (oldData != nullptr) {
            delete oldData;
        }
    }

    inline void RecvStream::setOnReceive(void(*func)(const STREAM_ID&, const STREAM_ID&, const char*, const int&, const rapidjson::Document&)) {
        Callback* oldData = (Callback*)CorelinkDLL::setOnRecv(state, streamRef, streamID, CallbackDataJson::RecvCallback, new CallbackDataJson(func));
        if (oldData != nullptr) {
            delete oldData;
        }
    }

    inline void RecvStream::setOnReceive(void(*func)(void*, const STREAM_ID&, const STREAM_ID&, const char*, const int&, const rapidjson::Document&), void* obj) {
        Callback* oldData = (Callback*)CorelinkDLL::setOnRecv(state, streamRef, streamID, CallbackDataJsonVoid::RecvCallback, new CallbackDataJsonVoid(func, obj));
        if (oldData != nullptr) {
            delete oldData;
        }
    }

    inline std::vector<STREAM_ID> RecvStream::listSources() {
        return (this->streamID == STREAM_DEF) ? std::vector<STREAM_ID>() : StreamData::listStreamSources(this->streamID);
    }
}

#endif