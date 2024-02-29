#include "corelink/objects/streams/comm_main_tcp.h"
#include "corelink/objects/streams/tcp_recv_handler.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            comm_main_tcp::comm_main_tcp(client_main* clientRef) : comm_main_base(clientRef) {
                this->sock = INVALID_SOCKET;
                this->serverHint = sockaddr_in();
            }

            comm_main_tcp::~comm_main_tcp() {
                if (this->sock != INVALID_SOCKET) {
                    SOCKET tmp = this->sock;
                    this->sock = INVALID_SOCKET;
                    shutdown(tmp, SD_BOTH);
                    closesocket(tmp);
                }

                if (this->recvThread.joinable()) {
                    this->recvThread.join();
                }
            }

            bool comm_main_tcp::sendMsg(char* msg, int len) {
                std::lock_guard<std::mutex> lck(this->sendLock);
                if (this->sock == INVALID_SOCKET) { return false; }
                int sendVal;
                int left;

                left = len;
                while (left > 0) {
                    sendVal = send(this->sock, msg + len - left, left, 0);
                    if (sendVal == SOCKET_ERROR) {
                        return false;
                    }
                    left -= sendVal;
                }
                return true;
            }

            std::string comm_main_tcp::connectServer(const std::string& ip, const int& port, int& errorID) {
                if (this->sock != INVALID_SOCKET) {
                    errorID = addError("comm_main_tcp.cpp connectServer: Already Connected.", ERROR_CODE_STATE);
                    return "";
                }
                this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (this->sock == INVALID_SOCKET) {
                    errorID = addError("comm_main_tcp.cpp connectServer: could not initialize socket. Error code: " + std::to_string(SOCKET_ERROR_CODE), ERROR_CODE_SOCKET);
                    return "";
                }
                CorelinkDLL::setRecvSocketOpts(this->sock);
                this->serverHint.sin_port = htons((u_short)port);
                this->serverHint.sin_family = AF_INET;
                inet_pton(AF_INET, ip.c_str(), &this->serverHint.sin_addr);

                if (connect(this->sock, (sockaddr*)&this->serverHint, sizeof(this->serverHint)) == SOCKET_ERROR) {
                    errorID = addError("comm_main_tcp.cpp connectServer: could not connect to server. Error code: " + std::to_string(SOCKET_ERROR_CODE), ERROR_CODE_SOCKET);
                    closesocket(this->sock);
                    this->sock = INVALID_SOCKET;
                    return "";
                }
                this->recvThread = std::thread(&comm_main_tcp::RecvThread, this);

                return ip;
            }

            void comm_main_tcp::RecvThread() {
                tcp_recv_handler recvHandler = tcp_recv_handler(this->sock);
                int counter;
                int msgLen;
                int bytesRecv;
                char* msgRecv;
                char* str;
                std::shared_ptr<rapidjson::Document> json;

                counter = 0;
                msgLen = 0;
                while (this->sock != INVALID_SOCKET) {
                    if ((msgRecv = recvHandler.recvData(bytesRecv)) == nullptr) {
                        continue;
                    }
                    for (int i = 0; i < bytesRecv; ++i) {
                        if (msgRecv[i] == '{') {
                            ++counter;
                        }
                        else if (msgRecv[i] == '}') {
                            if (--counter == 0) {
                                msgLen = msgLen + i + 1;
                                str = recvHandler.getData(msgLen, 1);
                                json = std::make_shared<rapidjson::Document>();
                                json->SetObject();
                                // assumes only using valid json string
                                json->Parse(str, msgLen);
                                msgLen = - i - 1;
                                // push to receiver
                                if (json->FindMember("ID") != json->MemberEnd()) {
                                    responseHandler.add(json, (*json)["ID"].GetInt());
                                    json.reset();
                                }
                                // push to handler
                                else {
                                    // this->recvCallback(json.get());
                                    json.reset();
                                }
                                delete[] str;
                                str = nullptr;
                            }
                        }
                    }
                    msgLen += bytesRecv;
                }
            }

        }
    }
}