#include "corelink/client/mainStream.h"

namespace CorelinkDLL {
    void addCommData(const std::vector<std::string>& data, const int& commID) {
        std::string compactString;
        char* compactStringPtr;
        int* compactStringPtrCasted;
        int lenData, lenHeader;
        lenData = 0;
        for (const std::string& val : data) {
            lenData += (int) val.size();
        }
        lenHeader = sizeof(int) * ((int)data.size() + 1);
        compactStringPtr = new char[lenHeader + lenData];
        compactStringPtrCasted = (int*) compactStringPtr;
        
        lenData = 0;
        compactStringPtrCasted[0] = (int)data.size();
        for(int i = 0; i < data.size(); ++i) {
            compactStringPtrCasted[i + 1] = (int) data[i].size();
            memcpy(compactStringPtr + lenData + lenHeader, data[i].c_str(), compactStringPtrCasted[i + 1]);
            lenData += compactStringPtrCasted[i + 1];
        }
        compactStringPtrCasted = nullptr;
        compactString = std::string(compactStringPtr, lenHeader + lenData);
        delete[] compactStringPtr;
        client->data.msgHandler.add(compactString, commID);
    }

    void addCommData(const std::string& data, const int& commID) {
        client->data.msgHandler.add(data, commID);
    }

    int getStreamState(const char* proto, const char* dir) {
        int state;
        if (strcmp(proto, "udp") == 0) { state = STREAM_STATE_UDP; }
        else if (strcmp(proto, "tcp") == 0) { state = STREAM_STATE_TCP; }
        else if (strcmp(proto, "ws") == 0) { state = STREAM_STATE_WS; }
        else { return STREAM_STATE_NONE; }

        if (strcmp(dir, "source") == 0) { state &= STREAM_STATE_SEND; }
        else if (strcmp(dir, "target") == 0) { state &= STREAM_STATE_RECV; }
        else { return STREAM_STATE_NONE; }

        return state;
    }

    EXPORTED char* getCommData(const int& commID, int& len) {
        std::string data;
        char* buffer;
        data = client->data.msgHandler.get(commID);
        len = (int) data.size();
        buffer = new char[len];
        memcpy(buffer, data.c_str(), len);
        return buffer;
    }

    EXPORTED int getCommDataLen(const int& commID) {
        return client->data.msgHandler.getLen(commID);
    }

    EXPORTED bool getCommDataStr(const int& commID, char* buffer) {
        bool success = client->data.msgHandler.getStr(commID, buffer);
        client->data.msgHandler.remove(commID);
        return success;
    }

    EXPORTED void commListFunctions(int& commID, int& errorID) {
        rapidjson::Document json;
        std::stringstream ss;
        commID = mainCommGetCommID();

        ss << "{\"function\":\"listFunctions\""
            << ",\"ID\":" << commID
            << ",\"token\":\"" << client->token << "\"}";
        if (getJsonResponse("mainStream.cpp", "commListFunctions", ss.str(), json, commID, errorID)) {
            std::vector<std::string> data = std::vector<std::string>();
            rapidjson::Value::Array jsonData = json["functionList"].GetArray();
            data.resize(jsonData.Size());
            for (rapidjson::SizeType i = 0; i < jsonData.Size(); ++i) {
                data[i] = std::string(jsonData[i].GetString(), jsonData[i].GetStringLength());
            }
            addCommData(data, commID);
        }
        ss.clear();
    }

    EXPORTED void commGetFunctionInfo(const char* func, int& commID, int& errorID) {
        std::stringstream ss;
        rapidjson::Document json;
        commID = mainCommGetCommID();
        const static std::vector<std::string> funcInfoParams = {
            "name", "description", "version", "author", "email", "doc_href"
        };

        ss << "{\"function\":\"describeFunction\""
            << ",\"ID\":" << commID
            << ",\"functionName\":\"" << safeString(func)
            << "\",\"token\":\"" << client->token << "\"}";
        if (getJsonResponse("mainStream.cpp", "commGetFunctionInfo", ss.str(), json, commID, errorID)) {
            std::vector<std::string> data = std::vector<std::string>();
            rapidjson::Value jsonData;
            rapidjson::Value tmp;
            data.resize(funcInfoParams.size());
            jsonData = json["description"];
            for (int i = 0; i < funcInfoParams.size(); ++i) {
                tmp = jsonData[funcInfoParams[i].c_str()];
                data[i] = std::string(tmp.GetString(), tmp.GetStringLength());
            }
            addCommData(data, commID);
        }
        ss.clear();
    }

    EXPORTED void commListWorkspaces(int& commID, int& errorID) {
        std::stringstream ss;
        rapidjson::Document json;
        commID = mainCommGetCommID();
        ss << "{\"function\":\"listWorkspaces\""
            << ",\"ID\":" << commID
            << ",\"token\":\"" << client->token << "\"}";
        if (getJsonResponse("mainStream.cpp", "commListWorkspaces", ss.str(), json, commID, errorID)) {
            std::vector<std::string> data = std::vector<std::string>();
            rapidjson::Value::Array jsonData = json["workspaceList"].GetArray();
            data.resize(jsonData.Size());
            for (rapidjson::SizeType i = 0; i < jsonData.Size(); ++i) {
                data[i] = std::string(jsonData[i].GetString(), jsonData[i].GetStringLength());
            }
            addCommData(data, commID);
        }
        ss.clear();
    }

    EXPORTED bool commAddWorkspace(const char* workspace) {
        std::stringstream ss;
        rapidjson::Document json;
        bool success;
        int commID, errorID;
        commID = mainCommGetCommID();
        ss << "{\"function\":\"addWorkspace\""
            << ",\"ID\":" << commID
            << ",\"workspace\":\"" << safeString(workspace)
            << "\",\"token\":\"" << client->token << "\"}";
        success = getJsonResponse("mainStream.cpp", "commAddWorkspace", ss.str(), json, commID, errorID);
        if (errorID != 0) { delete[] getError(errorID, commID);}
        ss.clear();
        return success;
    }

    EXPORTED bool commRmWorkspace(const char* workspace) {
        std::stringstream ss;
        rapidjson::Document json;
        bool success;
        int commID, errorID;
        commID = mainCommGetCommID();
        ss << "{\"function\":\"rmWorkspace\""
            << ",\"ID\":" << commID
            << ",\"workspace\":\"" << safeString(workspace)
            << "\",\"token\":\"" << client->token << "\"}";
        success = getJsonResponse("mainStream.cpp", "commRmWorkspace", ss.str(), json, commID, errorID);
        if (errorID != 0) { delete[] getError(errorID, commID);}
        ss.clear();
        return success;
    }

    EXPORTED void commGeneric(const char* msg, const int& len, int& commID, int& errorID) {
        rapidjson::Document json;
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        json.SetObject();
        if (len == 0) {
            json.Parse(msg);
        }
        else {
            json.Parse(msg, len);
        }
        commID = mainCommGetCommID();
        json.AddMember("token", rapidjson::Value(client->token.c_str(), client->token.size()), json.GetAllocator());
        json.AddMember("ID", commID, json.GetAllocator());

        json.Accept(writer);

        // TODO: Find out how to reuse writer and buffer without getting abort.
        rapidjson::Document jsonRes;
        rapidjson::StringBuffer bufferRes;
        rapidjson::Writer<rapidjson::StringBuffer> writerRes(bufferRes);
        if (getJsonResponse("mainStream.cpp", "commGeneric", buffer.GetString(), jsonRes, commID, errorID)) {
            jsonRes.RemoveMember("ID");
            json.RemoveMember("statusCode");
            jsonRes.Accept(writerRes);
            
            std::vector<std::string> data;
            data.resize(1);
            data[0] = bufferRes.GetString();
            addCommData(data, commID);
        }
    }

    EXPORTED void commListStreams(const char** workspaces, int workspacesLen, const char** types,
            int typesLen, int& commID, int& errorID) {
        std::stringstream ss;
        rapidjson::Document json;
        commID = mainCommGetCommID();
        ss << "{\"function\":\"listStreams\""
            << ",\"ID\":" << commID
            << ",\"workspaces\":[";
        if (workspacesLen > 0) {
            ss << "\"" << safeString(workspaces[0]) << "\"";
            for (int i = 1; i < workspacesLen; ++i) {
                ss << ",\"" << safeString(workspaces[i]) << "\"";
            }
        }
        ss << "],\"types\":[";
        if (typesLen > 0) {
            ss << "\"" << safeString(types[0]) << "\"";
            for (int i = 1; i < typesLen; ++i) {
                ss << ",\"" << safeString(types[i]) << "\"";
            }
        }
        ss << "],\"token\":\"" << client->token << "\"}";
        if (getJsonResponse("mainStream.cpp", "commListStreams", ss.str(), json, commID, errorID)) {
            std::vector<std::string> data = std::vector<std::string>();
            int streamID;
            rapidjson::Value::Array jsonData = json["senderList"].GetArray();
            data.resize(jsonData.Size());

            for (rapidjson::SizeType i = 0; i < jsonData.Size(); ++i) {
                streamID = jsonData[i]["streamID"].GetInt();
                data[i] = std::string((const char*)&streamID, sizeof(int));
            }
            addCommData(data, commID);
        }
        ss.clear();
    }

    EXPORTED void commGetStreamInfo(const int& streamID, int& commID, int& errorID) {
        std::stringstream ss;
        rapidjson::Document json;
        commID = mainCommGetCommID();
        ss << "{\"function\":\"streamInfo\""
            << ",\"ID\":" << commID
            << ",\"streamID\":" << streamID
            << ",\"token\":\"" << client->token << "\"}";
        if (getJsonResponse("mainStream.cpp", "commGetStreamInfo", ss.str(), json, commID, errorID)) {
            std::string data;
            std::vector<std::string> type = std::vector<std::string>();
            rapidjson::Value jsonData;
            rapidjson::Value::MemberIterator itr;
            
            jsonData = json["info"];
            itr = jsonData.FindMember("type");
            if (itr->value.IsString()){
                type.push_back(std::string(itr->value.GetString(), itr->value.GetStringLength()));
            }
            else if (itr->value.IsArray()) {
                data.resize(7 + itr->value.Size());
                for (rapidjson::SizeType i = 0; i < itr->value.Size(); i++) {
                    type.push_back(std::string(itr->value[i].GetString(), itr->value[i].GetStringLength()));
                }
            }

            data = CorelinkDLL::Object::Stream::stream_data::compact(
                streamID, getStreamState(jsonData["proto"].GetString(), jsonData["direction"].GetString()), jsonData["MTU"].GetInt(),
                (itr = jsonData.FindMember("user")) == jsonData.MemberEnd() ? "" : std::string(itr->value.GetString(), itr->value.GetStringLength()),
                (itr = jsonData.FindMember("workspace")) == jsonData.MemberEnd() ? "" : std::string(itr->value.GetString(), itr->value.GetStringLength()),
                (itr = jsonData.FindMember("meta")) == jsonData.MemberEnd() ? "" : std::string(itr->value.GetString(), itr->value.GetStringLength()),
                type
            );

            addCommData(data, commID);
        }
        ss.clear();
    }

    EXPORTED void commAddSender(const char* workspace, const char* type, const char* meta,
            bool echo, bool alert, int protocol, int& commID, int& errorID) {
        std::stringstream ss;
        rapidjson::Document json;
        protocol = protocol & STREAM_STATE_SEND & client->data.initState;
        if (!isPow2(protocol)) {
            errorID = addError("mainStream.cpp commAddSender: Invalid protocol value", ERROR_CODE_VALUE);
            return;
        }
        commID = mainCommGetCommID();

        ss << "{\"function\":\"sender\""
            << ",\"ID\":" << commID
            << ",\"workspace\":\"" << safeString(workspace)
            << "\",\"IP\":\"" << client->clientIP
            << "\",\"port\":0,\"type\":\"" << safeString(type)
            << "\",\"meta\":\"" << safeString(meta)
            << "\",\"echo\":" << (echo ? "true" : "false")
            << ",\"alert\":" << (alert ? "true" : "false")
            << ",\"token\":\"" << client->token
            << "\",\"proto\":\"" << streamStateName(protocol)
            << "\"}";

        if (getJsonResponse("mainStream.cpp", "commAddSender", ss.str(), json, commID, errorID)) {
            int tmpVal;
            CorelinkDLL::Object::Stream::stream_data stream = CorelinkDLL::Object::Stream::stream_data(
                json["streamID"].GetInt(), protocol, json["MTU"].GetInt(),
                client->data.username, workspace, meta,
                {type}, {}
            );
            
            tmpVal = json["port"].GetInt();
            client->addStream(stream, protocol, tmpVal);
            addCommData(stream.streamData, commID);
        }
        ss.clear();
    }

    EXPORTED void commAddReceiver(const char* workspace, const char** types, int typeLen, const char* meta,
            bool echo, bool alert, int protocol, int& commID, int& errorID) {
        std::stringstream ss;
        rapidjson::Document json;
        std::vector<std::string> typesVec;
        protocol = protocol & STREAM_STATE_RECV & client->data.initState;
        if (!isPow2(protocol)) {
            errorID = addError("mainStream.cpp commAddReceiver: Invalid protocol value", ERROR_CODE_VALUE);
            return;
        }
        commID = mainCommGetCommID();
        typesVec = std::vector<std::string>();
        typesVec.resize(typeLen);
        for (int i = 0; i < typeLen; ++i) {
            typesVec[i] = std::string(types[i]);
        }

        ss << "{\"function\":\"receiver\""
            << ",\"ID\":" << commID
            << ",\"workspace\":\"" << safeString(workspace)
            << "\",\"IP\":\"" << client->clientIP
            << "\",\"port\":0,\"type\":[";
        if (typeLen > 0) {
            ss << "\"" << safeString(typesVec[0].c_str(), typesVec[0].size()) << "\"";
        }
        for (int i = 1; i < typeLen; ++i) {
            ss << ",\"" << safeString(typesVec[i].c_str(), typesVec[i].size()) << "\"";
        }
        ss << "],\"meta\":\"" << safeString(meta)
            << "\",\"echo\":" << (echo ? "true" : "false")
            << ",\"alert\":" << (alert ? "true" : "false")
            << ",\"token\":\"" << client->token
            << "\",\"proto\":\"" << streamStateName(protocol)
            << "\"}";
        if (getJsonResponse("mainStream.cpp", "commAddReceiver", ss.str(), json, commID, errorID)) {
            std::unordered_set<int> sources = std::unordered_set<int>();
            rapidjson::SizeType jsonSize;
            int port;
            rapidjson::Value::MemberIterator itr = json.FindMember("streamList");
            
            jsonSize = itr->value.Size();
            for (rapidjson::SizeType i = 0; i < jsonSize; i++) {
                sources.insert(itr->value[i]["streamID"].GetInt());
            }

            CorelinkDLL::Object::Stream::stream_data stream = CorelinkDLL::Object::Stream::stream_data(
                json["streamID"].GetInt(), protocol, json["MTU"].GetInt(),
                client->data.username, workspace, meta,
                typesVec, sources
            );

            port = json["port"].GetInt();
            client->addStream(stream, protocol, port);
            addCommData(stream.streamData, commID);
        }
        ss.clear();
    }

    EXPORTED bool commSubscribe(const STREAM_ID& receiverID, const STREAM_ID& senderID) {
        std::stringstream ss;
        rapidjson::Document json;
        bool success;
        int commID, errorID;
        commID = mainCommGetCommID();
        ss << "{\"function\":\"subscribe\""
            << ",\"ID\":" << commID
            << ",\"receiverID\":\"" << receiverID
            << "\",\"streamIDs\":[\"" << senderID
            << "\"],\"token\":\"" << client->token << "\"}";
        success = getJsonResponse("mainStream.cpp", "commSubscribe", ss.str(), json, commID, errorID);
        if (errorID != 0) { delete[] getError(errorID, commID);}
        ss.clear();
        client->addSource(receiverID, senderID);
        return success;
    }

    EXPORTED bool commUnsubscribe(const STREAM_ID& receiverID, const STREAM_ID& senderID) {
        std::stringstream ss;
        rapidjson::Document json;
        bool success;
        int commID, errorID;
        commID = mainCommGetCommID();
        ss << "{\"function\":\"unsubscribe\""
            << ",\"ID\":" << commID
            << ",\"receiverID\":\"" << receiverID
            << "\",\"streamIDs\":[\"" << senderID
            << "\"],\"token\":\"" << client->token << "\"}";
        success = getJsonResponse("mainStream.cpp", "commSubscribe", ss.str(), json, commID, errorID);
        if (errorID != 0) { delete[] getError(errorID, commID);}
        ss.clear();
        client->rmSource(receiverID, senderID);
        return success;
    }
}