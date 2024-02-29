/**
 * @file CorelinkSendStream.h
 * Handler class for the sender streams.
 * A container for the stream id and necessary data to call associated functions.
 */
#ifndef CORELINKSENDSTREAM_H
#define CORELINKSENDSTREAM_H

#include "CorelinkClasses.h"

namespace Corelink {
    inline SendStream::SendStream(const STREAM_ID& stream, int state, int streamRef) :
        streamID(stream), state(state), streamRef(streamRef)
    {}

    inline SendStream::SendStream(const StreamData& streamData) {
        if ((streamData.state & Const::STREAM_STATE_SEND) > 0) {
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

    inline SendStream::SendStream(const SendStream& rhs) :
        streamID(rhs.streamID), state(rhs.state), streamRef(rhs.streamRef)
    {}

    inline SendStream::SendStream(SendStream&& rhs) {
        std::swap(this->streamID, rhs.streamID);
        std::swap(this->state, rhs.state);
        std::swap(this->streamRef, rhs.streamRef);
    }

    inline SendStream& SendStream::operator=(const SendStream& rhs) {
        this->streamID = rhs.streamID;
        this->state = rhs.state;
        this->streamRef = rhs.streamRef;
        return *this;
    }

    inline SendStream::operator STREAM_ID() const {
        return this->streamID;
    }

    inline bool SendStream::send(const std::string& msg, const int& federationID) {
        return CorelinkDLL::sendMsg(this->state, this->streamRef, this->streamID, federationID, msg.c_str(), (int) msg.size());
    }

    inline bool SendStream::send(const std::string& msg, const rapidjson::Document& json, bool serverCheck, const int& federationID) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
        int len = (int) buffer.GetSize();
        return CorelinkDLL::sendMsgJson(this->state, this->streamRef, this->streamID, federationID, msg.c_str(), (int)msg.size(), buffer.GetString(), len, serverCheck);
    }

    inline bool SendStream::send(const char* msg, int msgLen, const int& federationID) {
        return CorelinkDLL::sendMsg(this->state, this->streamRef, this->streamID, federationID, msg, msgLen);
    }

    inline bool SendStream::send(const char* msg, int msgLen, const rapidjson::Document& json, bool serverCheck, const int& federationID) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
        int len = (int)buffer.GetSize();
        return CorelinkDLL::sendMsgJson(this->state, this->streamRef, this->streamID, federationID, msg, msgLen, buffer.GetString(), len, serverCheck);
    }
}

#endif