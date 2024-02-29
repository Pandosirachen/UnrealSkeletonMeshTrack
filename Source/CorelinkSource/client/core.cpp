#include "corelink/client/core.h"
#include "corelink/objects/init.h"

namespace CorelinkDLL {
    CorelinkDLL::Object::client_main* client = nullptr;

    std::string safeString(const char* str, int len) {
        rapidjson::Document doc;
        len = (len < 0) ? strlen(str) : len;
        doc.SetObject();
        doc.AddMember("", rapidjson::Value(str, len), doc.GetAllocator());
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        std::size_t tmpHead = buffer.GetSize();
        return std::string(buffer.GetString() + 5, tmpHead - 7);
    }

    bool mainCommSend(char* msg, int len) {
        return client->mainComm->sendMsg(msg, len);
    }

    int mainCommGetCommID() {
        return client->mainComm->reserve();
    }

    rapidjson::Document mainCommSendRecv(char* msg, int len, const int& commID) {
        return client->mainComm->sendRecv(msg, len, commID);
    }

    bool getJsonResponse(const std::string& fileName, const std::string& funcName, const std::string& msg,
            rapidjson::Document& json, const int& commID, int& errorID, bool checkToken) {
        std::stringstream errorMsg;
        errorID = 0;
        if (checkToken && client->token.size() == 0) {
            errorMsg << fileName << " " << funcName << ": " << "no token set";
            errorID = addError(errorMsg.str(), ERROR_CODE_NO_TOKEN);
            return false;
        }
        json = mainCommSendRecv((char*) msg.c_str(),msg.size(), commID);
        if (json.MemberCount() == 0) {
            errorMsg << fileName << " " << funcName << ": " << "Invalid Server response";
            errorID = addError(errorMsg.str(), ERROR_CODE_COMM);
            return false;
        }
        if (json["statusCode"].GetInt() != 0) {
            errorMsg << fileName << " " << funcName << ": " << std::string(json["message"].GetString(), json["message"].GetStringLength());
            errorID = addError(errorMsg.str(), ERROR_CODE_COMM);
            return false;
        }
        return true;
    }

    void commConnect(int& errorID) {
        std::stringstream ss;
        rapidjson::Document json;
        rapidjson::Value jsonData;
        int commID;

        commID = mainCommGetCommID();
        ss << "{\"function\":\"auth\""
            << ",\"ID\":" << commID
            << ",\"username\":\"" << safeString(client->data.username.c_str(), client->data.username.size())
            << "\",\"password\":\"" << safeString(client->data.password.c_str(), client->data.password.size()) << "\"}";
        if (getJsonResponse("core.cpp", "commConnect", ss.str(), json, commID, errorID, false)) {
            jsonData = json["token"];
            client->token = std::string(jsonData.GetString(), jsonData.GetStringLength());
            jsonData = json["IP"];
            client->clientIP = std::string(jsonData.GetString(), jsonData.GetStringLength());
        }
        else {
            errorID = commID;
        }
        ss.clear();
    }

    void commConnectPlugin(const std::string& token, int& errorID) {
        std::stringstream ss;
        rapidjson::Document json;
        rapidjson::Value jsonData;
        int commID;

        commID = mainCommGetCommID();
        ss << "{\"function\":\"auth\""
            << ",\"ID\":" << commID
            << ",\"token\":\"" << token << "\"}";
        if (getJsonResponse("core.cpp", "commConnect", ss.str(), json, commID, errorID, false)) {
            jsonData = json["token"];
            client->token = std::string(jsonData.GetString(), jsonData.GetStringLength());
            jsonData = json["IP"];
            client->clientIP = std::string(jsonData.GetString(), jsonData.GetStringLength());
        }
        else {
            errorID = commID;
        }
        ss.clear();
    }

    /**
     * @private
     * Helper function to handle general functions called when connecting.
     */
    void corelinkConnectHelper(int& errorID) {
        errorID = 0;

        if (initData.clientInit) {
            errorID = addError("core.cpp corelinkConnect: client already initialized", ERROR_CODE_STATE);
            return;
        }

    #ifdef _WIN32
        WSADATA data;
        int wsOk;

        wsOk = WSAStartup(MAKEWORD(2, 0), &data);
        if (wsOk != 0) {
            errorID = addError("core.cpp corelinkConnect: winsock initialization error: " + std::to_string(wsOk), ERROR_CODE_SOCKET);
            return;
        }
    #endif
    }

    /**
     * @private
     * Helper function to handle cleaning up of client in event of corelinkCleanup or error in corelinkConnect.
     */
    void corelinkCleanupHelper() {
        initData.clientInit = false;

        if (client != nullptr) {
            client->data.initState = 0;
            client->data.onDropHandler = nullptr;
            client->data.onStaleHandler = nullptr;
            client->data.onSubscribeHandler = nullptr;
            client->data.onUpdateHandler = nullptr;

            std::vector<STREAM_ID> streamIDs;
            streamIDs = client->getStreams();

            for (const STREAM_ID& streamid : streamIDs) {
                commDisconnect(streamid);
            }

            delete client;
            client = nullptr;
        }

    #ifdef _WIN32
        WSACleanup();
    #endif
    }

    EXPORTED char* getToken(int& len) {
        char* buffer;
        len = (int) client->token.size();
        buffer = new char[len];
        memcpy(buffer, client->token.c_str(), len);
        return buffer;
    }

    EXPORTED int getTokenLen() {
        return (int) client->token.size();
    }

    EXPORTED void getTokenStr(char* buffer) {
        memcpy(buffer, client->token.c_str(), client->token.size());
    }

    EXPORTED void corelinkConnect(const char* serverIP, const int serverPort, int& errorID) {
        std::lock_guard<std::mutex> lck(initData.lock);
        corelinkConnectHelper(errorID);
        if (errorID > 0) {
            return;
        }

        std::string effectiveIP;

        initData.clientInit = true;
        client = new CorelinkDLL::Object::client_main(initData, errorID);
        if (errorID > 0) { 
            corelinkCleanupHelper();
            return;
        }
        effectiveIP = client->mainComm->connectServer(serverIP, serverPort, errorID);
        if (errorID > 0) { 
            corelinkCleanupHelper();
            return;
        }
        commConnect(errorID);
        if (errorID > 0) { 
            corelinkCleanupHelper();
            return;
        }
        client->initDataStreams(effectiveIP, errorID);
        if (errorID > 0) { 
            corelinkCleanupHelper();
            return;
        }
    }

    EXPORTED void corelinkConnectPlugin(int& errorID) {
        std::lock_guard<std::mutex> lck(initData.lock);
        corelinkConnectHelper(errorID);
        if (errorID > 0) {
            return;
        }

        std::string effectiveIP;
        std::string pluginToken;
        std::string serverIP;
        int serverPort;

        // TODO: replace with environment variables
        pluginToken = "!dfshdgs";
        serverIP = "127.0.0.1";
        serverPort = 20012;

        initData.clientInit = true;
        client = new CorelinkDLL::Object::client_main(initData, errorID);
        if (errorID > 0) { 
            corelinkCleanupHelper();
            return;
        }
        effectiveIP = client->mainComm->connectServer(serverIP, serverPort, errorID);
        if (errorID > 0) { 
            corelinkCleanupHelper();
            return;
        }
        commConnectPlugin(pluginToken, errorID);
        if (errorID > 0) { 
            corelinkCleanupHelper();
            return;
        }
        client->initDataStreams(effectiveIP, errorID);
        if (errorID > 0) { 
            corelinkCleanupHelper();
            return;
        }
    }

    EXPORTED void corelinkCleanup() {
        std::lock_guard<std::mutex> lck(initData.lock);
        if (!initData.clientInit) { return; }
        corelinkCleanupHelper();
    }

    EXPORTED bool commDisconnect(const STREAM_ID& streamID) {
        std::stringstream ss;
        rapidjson::Document json;
        bool success;
        int commID, errorID;
        commID = mainCommGetCommID();
        //stream not owned by client, do nothing.
        if (client->mapStreamData.find(streamID) == client->mapStreamData.end()) { return false; }

        ss << "{\"function\":\"disconnect\""
            << ",\"ID\":" << commID
            << ",\"workWorkspaces\":[],\"types\":[]"
            << ",\"streamIDs\":[" << streamID
            << "],\"token\":\"" << client->token << "\"}";
        
        success = getJsonResponse("core.cpp", "commDisconnect", ss.str(), json, commID, errorID);
        ss.clear();
        if (errorID != 0) {
            delete[] getError(errorID, commID);
            return false;
        }
        client->rmStream(streamID);
        return true;
    }
}