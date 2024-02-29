/**
 * @file CorelinkStreamData.h
 * Handles the information associated with data streams in the client.
 */
#ifndef CORELINKSTREAMDATA_H
#define CORELINKSTREAMDATA_H

#include "CorelinkClasses.h"

namespace Corelink {

    inline StreamData::StreamData(const int& streamID, const int& state, const int& mtu,
            const std::string& user, const std::string& workspace, const std::string& meta, 
            int streamRef, const std::vector<std::string>& type) :
        streamID(streamID), state(state), mtu(mtu), user(user), workspace(workspace), meta(meta), streamRef(streamRef), type(type)
    {}

    inline StreamData::StreamData() :
        streamID(STREAM_DEF), state(Const::STREAM_STATE_NONE), mtu(-1), user(), workspace(), meta(), streamRef(-1), type()
    {}

    inline StreamData::~StreamData() {}

    inline StreamData::StreamData(const StreamData& rhs) :
        state(rhs.state), mtu(rhs.mtu), streamRef(rhs.streamRef), streamID(rhs.streamID), user(rhs.user), workspace(rhs.workspace), meta(rhs.meta), type(rhs.type)
    {}

    inline StreamData& StreamData::operator=(const StreamData& rhs) {
        streamID = rhs.streamID;
        user = rhs.user;
        workspace = rhs.workspace;
        meta = rhs.meta;
        state = rhs.state;
        mtu = rhs.mtu;
        streamRef = rhs.streamRef;
        type = rhs.type;
        return *this;
    }

    inline std::ostream& operator<<(std::ostream& os, const StreamData& rhs) {
        os << "Stream: " << rhs.streamID << " User: " << rhs.user << " Workspace: " << rhs.workspace << "\nMeta: " << rhs.meta
            << "\nProtocol:" << streamStateName(rhs.state)
            << " State: " << ((rhs.state & Const::STREAM_STATE_SEND) > 0 ? "sender" : "") << ((rhs.state & Const::STREAM_STATE_RECV) > 0 ? "receiver" : "")
            << "\nTypes: [";
        if (rhs.type.size() > 0) {
            os << rhs.type[0];
            for (size_t i = 1; i < rhs.type.size(); ++i) { os << ", " << rhs.type[i]; }
        }
        os << "]\n";
        if (rhs.streamRef > -1) { os << "Internal identifier: " << rhs.streamRef << "\n"; }
        return os;
    }

    inline StreamData StreamData::parse(const char* data) {
        int* dataCasted;
        int streamID, state, mtu, len, tmp;
        std::string user, workspace, meta;
        std::vector<std::string> types;
        dataCasted = (int*)data;
        
        len = dataCasted[0];
        streamID = dataCasted[len + 1];
        state = dataCasted[len + 2];
        mtu = dataCasted[len + 3];

        types.resize(len - 6);
        tmp = (len + 4) * sizeof(int);
        user = std::string(data + tmp, dataCasted[4]);
        tmp += dataCasted[4];
        workspace = std::string(data + tmp, dataCasted[5]);
        tmp += dataCasted[5];
        meta = std::string(data + tmp, dataCasted[6]);
        tmp += dataCasted[6];
        for (std::size_t i = 0; i < types.size(); ++i) {
            types[i] = std::string(data + tmp, dataCasted[7 + i]);
            tmp += dataCasted[7 + i];
        }

        return StreamData(streamID, state, mtu, user, workspace, meta, CorelinkDLL::getStreamRef(streamID), types);
    }

    inline bool StreamData::isSender() const
    {
        return (streamID & Const::STREAM_STATE_SEND) > 0;
    }

    inline bool StreamData::isSender(const STREAM_ID& streamID) {
        return CorelinkDLL::isSendStream(streamID);
    }

    inline bool StreamData::isReceiver() const
    {
        return (streamID & Const::STREAM_STATE_RECV) > 0;
    }

    inline bool StreamData::isReceiver(const STREAM_ID& streamID) {
        return CorelinkDLL::isRecvStream(streamID);
    }

    inline StreamData StreamData::getStreamData(const STREAM_ID& streamID) {
        char* data;
        int len;
        StreamData stream;
    #ifdef WRAPPER_ALLOC
        len = CorelinkDLL::getStreamDataLen(streamID);
        if (len == 0) { return StreamData(); }
        data = new char[len];
        CorelinkDLL::getStreamDataStr(streamID, data);
        stream = parse(data);
        delete[] data;
        return stream;
    #else
        data = (char*)CorelinkDLL::getStreamData(streamID, len);
        if (len == 0) { return StreamData(); }
        stream = parse(data);
        return stream;
    #endif
    }

    inline SendStream StreamData::getSenderHandler(const STREAM_ID& streamID) {
        int state, ref;
        state = CorelinkDLL::getStreamState(streamID);
        ref = CorelinkDLL::getStreamRef(streamID);
        if (ref < 0 || (state & Corelink::Const::STREAM_STATE_SEND) == 0) { return SendStream(); }
        return SendStream(streamID, state, ref);
    }

    inline RecvStream StreamData::getReceiverHandler(const STREAM_ID& streamID) {
        int state, ref;
        state = CorelinkDLL::getStreamState(streamID);
        ref = CorelinkDLL::getStreamRef(streamID);
        if (ref < 0 || (state & Corelink::Const::STREAM_STATE_RECV) == 0) { return RecvStream(); }
        return RecvStream(streamID, state, ref);
    }

    inline std::vector<STREAM_ID> StreamData::listClientStreams()
    {
        char* data;
        int* dataCasted;
        std::vector<STREAM_ID> streamIDs;
        streamIDs = std::vector<STREAM_ID>();
    #ifdef WRAPPER_ALLOC
        int commID;
        CorelinkDLL::getClientStreamsPushed(commID);
        streamIDs.resize(CorelinkDLL::getCommDataLen(commID) / sizeof(int));
        data = new char[streamIDs.size() * sizeof(int)];
        dataCasted = (int*)data;
        CorelinkDLL::getCommDataStr(commID, data);
        for (std::size_t i = 0; i < streamIDs.size(); ++i) {
            streamIDs[i] = dataCasted[i];
        }
        delete[] data;
        dataCasted = nullptr;
        return streamIDs;
    #else
        data = CorelinkDLL::getClientStreams();
        dataCasted = (int*)data;
        streamIDs.resize(dataCasted[0]);
        for (std::size_t i = 0; i < streamIDs.size(); ++i) {
            streamIDs[i] = dataCasted[i + 1];
        }
        CorelinkDLL::freeData(data);
        dataCasted = nullptr;
        return streamIDs;
    #endif
    }

    inline std::vector<STREAM_ID> StreamData::listStreamSources(const STREAM_ID& streamID) {
        char* data;
        int* dataCasted;
        std::vector<STREAM_ID> streamIDs;
        streamIDs = std::vector<STREAM_ID>();
#ifdef WRAPPER_ALLOC
        int commID;
        CorelinkDLL::getClientStreamSourcesPushed(streamID, commID);
        streamIDs.resize(CorelinkDLL::getCommDataLen(commID) / sizeof(int));
        data = new char[streamIDs.size() * sizeof(int)];
        dataCasted = (int*)data;
        CorelinkDLL::getCommDataStr(commID, data);
        for (std::size_t i = 0; i < streamIDs.size(); ++i) {
            streamIDs[i] = dataCasted[i];
        }
        delete[] data;
        dataCasted = nullptr;
        return streamIDs;
#else
        data = CorelinkDLL::getClientStreamSources(streamID);
        dataCasted = (int*)data;
        streamIDs.resize(dataCasted[0]);
        for (std::size_t i = 0; i < streamIDs.size(); ++i) {
            streamIDs[i] = dataCasted[i + 1];
        }
        CorelinkDLL::freeData(data);
        dataCasted = nullptr;
        return streamIDs;
#endif
    }
}
#endif