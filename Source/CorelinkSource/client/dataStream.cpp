#include "corelink/client/dataStream.h"
#include "corelink/objects/streams/comm_data_send_base.h"
#include "corelink/objects/streams/comm_data_recv_base.h"

namespace CorelinkDLL {
    EXPORTED int getStreamRef(const STREAM_ID& streamID) {
        return client->getStreamRef(streamID);
    }

    EXPORTED int getStreamState(const STREAM_ID& streamID) {
        return client->getStreamState(streamID);
    }

    EXPORTED const char* getStreamData(const STREAM_ID& streamID, int& len) {
        std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>::iterator iter;
        if ((iter = client->mapStreamData.find(streamID)) == client->mapStreamData.end()) { 
            len = 0;
            return nullptr;
        }
        len = (int) iter->second.streamData.size();
        return iter->second.streamData.c_str();
    }

    EXPORTED int getStreamDataLen(const STREAM_ID& streamID) {
        return client->getStreamData(streamID).size();
    }

    EXPORTED void getStreamDataStr(const STREAM_ID& streamID, char* buffer) {
        std::string streamData = client->getStreamData(streamID);
        memcpy(buffer, streamData.c_str(), streamData.size());
    }

    EXPORTED bool sendMsg(int protocol, int ref, const STREAM_ID& streamID, int federationID, const char* msg, int msgLen) {
        return ((CorelinkDLL::Object::Stream::comm_data_send_base*) client->dataStreams[streamStateToBitIndex(protocol & STREAM_STATE_SEND)])
            ->sendMsg(ref, streamID, federationID, std::string(msg, msgLen));
    }

    EXPORTED bool sendMsgJson(int protocol, int ref, const STREAM_ID& streamID, int federationID, const char* msg, int msgLen, const char* json, int jsonLen, bool serverCheck) {
        return ((CorelinkDLL::Object::Stream::comm_data_send_base*) client->dataStreams[streamStateToBitIndex(protocol & STREAM_STATE_SEND)])
            ->sendMsg(ref, streamID, federationID, std::string(msg, msgLen), std::string(json, jsonLen), serverCheck);
    }

    EXPORTED void* setOnRecv(int protocol, int ref, const STREAM_ID& streamID, CorelinkDLL::Object::Stream::Callback func, void* funcData) {
        if (!client->streamIsType(streamID, STREAM_STATE_RECV)) { return nullptr; }
        return ((CorelinkDLL::Object::Stream::comm_data_recv_base*) client->dataStreams[streamStateToBitIndex(protocol & STREAM_STATE_RECV)])->setRecvCallback(ref, streamID, func, funcData);
    }
    
    EXPORTED bool isSendStream(int streamID) {
        return client->streamIsType(streamID, STREAM_STATE_SEND);
    }

    EXPORTED bool isRecvStream(int streamID) {
        return client->streamIsType(streamID, STREAM_STATE_RECV);
    }

    EXPORTED char* getClientStreams() {
        std::vector<int> streams;
        char* data;
        int* dataCasted;
        streams = client->getStreams();
        data = new char[(streams.size() + 1) * sizeof(int)];
        dataCasted = (int*) data;
        dataCasted[0] = streams.size();
        for (int i = 0; i < streams.size(); ++i) {
            dataCasted[i + 1] = streams[i];
        }
        dataCasted = nullptr;
        return data;
    }

    EXPORTED void getClientStreamsPushed(int& commID) {
        std::vector<int> streams;
        char* data;
        int* dataCasted;
        int len;
        streams = client->getStreams();
        len = streams.size() * sizeof(int);
        data = new char[len];
        dataCasted = (int*) data;
        for (int i = 0; i < streams.size(); ++i) {
            dataCasted[i] = streams[i];
        }
        dataCasted = nullptr;
        commID = mainCommGetCommID();
        client->data.msgHandler.add(std::string(data, len), commID);
        delete[] data;
    }

    EXPORTED char* getClientStreamSources(int streamID) {
        std::vector<int> streams;
        char* data;
        int* dataCasted;
        streams = client->getStreamSources(streamID);
        data = new char[(streams.size() + 1) * sizeof(int)];
        dataCasted = (int*) data;
        dataCasted[0] = streams.size();
        for (int i = 0; i < streams.size(); ++i) {
            dataCasted[i + 1] = streams[i];
        }
        dataCasted = nullptr;
        return data;
    }

    EXPORTED void getClientStreamSourcesPushed(int streamID, int& commID) {
        std::vector<int> streams;
        char* data;
        int* dataCasted;
        int len;
        streams = client->getStreamSources(streamID);
        len = streams.size() * sizeof(int);
        data = new char[len];
        dataCasted = (int*) data;
        for (int i = 0; i < streams.size(); ++i) {
            dataCasted[i] = streams[i];
        }
        dataCasted = nullptr;
        commID = mainCommGetCommID();
        client->data.msgHandler.add(std::string(data, len), commID);
        delete[] data;
    }
}