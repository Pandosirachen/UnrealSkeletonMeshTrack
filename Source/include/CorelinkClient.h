/**
 * @file CorelinkClient.h
 * Contains client to server commands.
 */
#ifndef CORELINKCLIENT_H
#define CORELINKCLIENT_H
#pragma once

#include "CorelinkClasses.h"
#include "CorelinkException.h"
#include "CorelinkConst.h"

namespace Corelink {
    inline bool isActive() {
        return CorelinkDLL::getClientState();
    }

    inline std::vector<std::string> Client::getCommResponseDataHelper(char* data) {
        std::vector<std::string> dataParsed;
        int* dataCast;
        int lenHeader;
        int lenData;
        dataCast = (int*) data;
        dataParsed = std::vector<std::string>();
        dataParsed.resize(dataCast[0]);
        lenHeader = (dataCast[0] + 1) * sizeof(int);
        lenData = 0;
        for (int i = 0; i < dataCast[0]; ++i) {
            dataParsed[i] = std::string(data + lenHeader + lenData, dataCast[i + 1]);
            lenData += dataCast[i + 1];
        }
        dataCast = nullptr;
        return dataParsed;
    }

    inline std::vector<std::string> Client::getCommResponseData(const int& commID) {
        int responseLen;
        char* responseMsg;
        std::vector<std::string> data;
    #ifdef WRAPPER_ALLOC
        responseLen = CorelinkDLL::getCommDataLen(commID);
        responseMsg = new char[responseLen];
        CorelinkDLL::getCommDataStr(commID, responseMsg);
        data = getCommResponseDataHelper(responseMsg);
        delete[] responseMsg;
        return data;
    #else
        responseMsg = CorelinkDLL::getCommData(commID, responseLen);
        data = getCommResponseDataHelper(responseMsg);
        CorelinkDLL::freeData(responseMsg);
        return data;
    #endif
    }

    inline const char** Client::vecStringToCharArray(const std::vector<std::string>& vec) {
        const char** data = new const char*[vec.size()];
        for (int i = 0; i < vec.size();++i) {
            data[i] = vec[i].c_str();
        }
        return data;
    }

    // TODO: replace some logic with StreamData.parse
    // Look at StreamData.getStreamData for reference
    inline StreamData Client::getDataStreamInfo(int& commID) {
        std::vector<std::string> dataOut;
        dataOut = getCommResponseData(commID);

        int streamID = *(STREAM_ID*)dataOut[0].c_str();
        int streamRef = CorelinkDLL::getStreamRef(streamID);

        return StreamData(streamID, *((int*)dataOut[1].c_str()), *((int*)dataOut[2].c_str()), dataOut[3], dataOut[4], dataOut[5], streamRef, std::vector<std::string>(dataOut.begin() + 6, dataOut.end()));
    }

    inline void Client::connect(const std::string& serverIP, int port) {
        int errorID;
        CorelinkDLL::corelinkConnect(serverIP.c_str(), port, errorID);
        CorelinkException::GetDLLException(errorID);
    }

    inline void Client::connectPlugin() {
        int errorID;
        CorelinkDLL::corelinkConnectPlugin(errorID);
        CorelinkException::GetDLLException(errorID);
    }

    inline void Client::cleanup() {
        if (!CorelinkDLL::getClientState()) { return; }
        std::vector<int> streams = StreamData::listClientStreams();
        // clear out on recv callbacks to avoid memory leaks.
        for (std::size_t i = 0; i < streams.size(); ++i) {
            if (StreamData::isReceiver(streams[i])) {
                StreamData::getReceiverHandler(streams[i]).setOnReceive();
            }
        }

        CorelinkDLL::corelinkCleanup();
    }

    /**
     * Gets the token assigned to the current session.
     * @return Token string.
     */
    inline std::string Client::getToken() {
        char* data;
        std::string token;
        int len;
    #ifdef WRAPPER_ALLOC
        len = CorelinkDLL::getTokenLen();
        if (len == 0) { return ""; }
        data = new char[len];
        CorelinkDLL::getTokenStr(data);
        token = std::string(data, len);
        delete[] data;
        return token;
    #else
        data = CorelinkDLL::getToken(len);
        token = std::string(data, len);
        CorelinkDLL::freeData(data);
        return token;
    #endif
    }

    inline std::vector<std::string> Client::listServerFunctions() {
        int commID, errorID;
        CorelinkDLL::commListFunctions(commID, errorID);
        CorelinkException::GetDLLException(errorID);
        return getCommResponseData(commID);
    }

    inline std::vector<std::string> Client::describeServerFunction(const std::string& func) {
        int commID, errorID;
        std::vector<std::string> data;
        CorelinkDLL::commGetFunctionInfo(func.c_str(), commID, errorID);
        CorelinkException::GetDLLException(errorID);
        data = getCommResponseData(commID);
        return data;
    }

    inline std::vector<std::string> Client::listWorkspaces() {
        int commID, errorID;
        CorelinkDLL::commListWorkspaces(commID, errorID);
        CorelinkException::GetDLLException(errorID);
        return getCommResponseData(commID);
    }

    inline bool Client::addWorkspace(const std::string& workspace) {
        return CorelinkDLL::commAddWorkspace(workspace.c_str());
    }

    inline bool Client::rmWorkspace(const std::string& workspace) {
        return CorelinkDLL::commRmWorkspace(workspace.c_str());
    }

    inline rapidjson::Document Client::genericComm(const rapidjson::Document& json) {
        rapidjson::Document res = rapidjson::Document();
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        int commID, errorID;
        std::string jsonStr;

        res.SetObject();
        if (!json.IsObject()) {
            return res;
        }

        json.Accept(writer);
        jsonStr = buffer.GetString();
        CorelinkDLL::commGeneric(jsonStr.c_str(), (int) jsonStr.size(), commID, errorID);
        CorelinkException::GetDLLException(errorID);
        jsonStr = getCommResponseData(commID)[0];
        res.Parse(jsonStr.c_str(), jsonStr.size());
        return res;
    }

    inline rapidjson::Document Client::genericComm(const std::string& json) {
        rapidjson::Document res = rapidjson::Document();
        int commID, errorID;
        std::string jsonStr;
        CorelinkDLL::commGeneric(json.c_str(), (int) json.size(), commID, errorID);
        CorelinkException::GetDLLException(errorID);
        jsonStr = getCommResponseData(commID)[0];
        
        res.SetObject();
        res.Parse(jsonStr.c_str(), jsonStr.size());
        return res;
    }

    inline rapidjson::Document Client::genericComm(const char* json, const int& len) {
        rapidjson::Document res = rapidjson::Document();
        int commID, errorID;
        std::string jsonStr;
        CorelinkDLL::commGeneric(json, len, commID, errorID);
        CorelinkException::GetDLLException(errorID);
        jsonStr = getCommResponseData(commID)[0];
        
        res.SetObject();
        res.Parse(jsonStr.c_str(), jsonStr.size());
        return res;
    }

    inline std::vector<int> Client::listStreams(const std::vector<std::string>& workspaces, const std::vector<std::string>& types) {
        int commID, errorID;
        std::vector<std::string> response;
        std::vector<int> data;
        const char** dataWorkspaces = vecStringToCharArray(workspaces);
        const char** dataTypes = vecStringToCharArray(types);
        CorelinkDLL::commListStreams(dataWorkspaces, (int) workspaces.size(), dataTypes, (int) types.size(), commID, errorID);
        delete[] dataWorkspaces;
        delete[] dataTypes;
        CorelinkException::GetDLLException(errorID);
        response = getCommResponseData(commID);
        data.resize(response.size());
        for (int i = 0; i < response.size(); ++i) {
            data[i] = *((int*)response[i].c_str());
        }
        return data;
    }

    inline StreamData Client::streamInfo(const int& streamID) {
        int commID, errorID;
        CorelinkDLL::commGetStreamInfo(streamID, commID, errorID);
        CorelinkException::GetDLLException(errorID);
        return getDataStreamInfo(commID);
    }

    inline SendStream Client::createSender(const std::string& workspace, const std::string& type, const std::string& meta, bool echo, bool alert, int protocol) {
        int commID, errorID;
        CorelinkDLL::commAddSender(workspace.c_str(), type.c_str(), meta.c_str(), echo, alert, protocol, commID, errorID);
        CorelinkException::GetDLLException(errorID);
        return SendStream(getDataStreamInfo(commID));
    }

    inline RecvStream Client::createReceiver(const std::string& workspace, const std::vector<std::string>& types, const std::string& meta, bool echo, bool alert, int protocol) {
        int commID, errorID;
        const char** typeData = vecStringToCharArray(types);
        CorelinkDLL::commAddReceiver(workspace.c_str(), typeData, (int) types.size(), meta.c_str(), echo, alert, protocol, commID, errorID);
        CorelinkException::GetDLLException(errorID);
        return RecvStream(getDataStreamInfo(commID));
    }

    inline bool Client::subscribe(const STREAM_ID& receiverID, const STREAM_ID& senderID) {
        bool output = CorelinkDLL::commSubscribe(receiverID, senderID);
        return output;
    }

    inline bool Client::unsubscribe(const STREAM_ID& receiverID, const STREAM_ID& senderID) {
        bool output = CorelinkDLL::commUnsubscribe(receiverID, senderID);
        return output;
    }

    inline bool Client::rmStream(const STREAM_ID& streamID) {
        bool output = CorelinkDLL::commDisconnect(streamID);
        return output;
    }
}

#endif