#include "corelink/objects/streams/comm_data_send_udp.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            comm_data_send_udp::comm_data_send_udp(const std::string& ip, int& errorID) {
                this->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (this->sock == INVALID_SOCKET) {
                    errorID = addError("comm_data_send_udp.cpp comm_data_send_udp: Socket error " + std::to_string(SOCKET_ERROR_CODE), ERROR_CODE_SOCKET);
                    return;
                }
                this->sendThread = std::thread(&comm_data_send_udp::sendFunc, this, ip);
            }

            comm_data_send_udp::~comm_data_send_udp() {
                if (this->sock != INVALID_SOCKET) {
                    SOCKET _sock = this->sock;
                    this->sock = INVALID_SOCKET;
                    closesocket(_sock);
                }
                this->sendQueue.clear();
                this->sendQueue.enqueue(std::pair<int, std::string>(INVALID_PORT, ""));
                if (this->sendThread.joinable()) {
                    this->sendThread.join();
                }
                this->sendQueue.clear();
            }

            void comm_data_send_udp::addStream(const STREAM_ID& streamID, const std::string&, int port) {
                this->streamMap.addObject(streamID, DataSenderUDP(port));
            }

            int comm_data_send_udp::getStreamRef(const STREAM_ID& streamID) {
                return this->streamMap.getStreamRedirect(streamID);
            }

            void comm_data_send_udp::rmStream(const STREAM_ID& streamID) {
                this->streamMap.rmObjectIndex(streamID);
            }

            bool comm_data_send_udp::sendMsg(int ref, const STREAM_ID& streamID, const int& federationID, const std::string& msg) {
                if (this->streamMap.getStream(ref) == streamID) {
                    this->sendQueue.enqueue(std::pair<int, std::string>(this->streamMap.at(ref).nsPort,
                        packageSend(streamID, federationID, msg)));
                    return true;
                }
                return false;
            }

            bool comm_data_send_udp::sendMsg(int ref, const STREAM_ID& streamID, const int& federationID, const std::string& msg, const std::string& json, bool serverCheck) {
                if (this->streamMap.getStream(ref) == streamID) {
                    this->sendQueue.enqueue(std::pair<int, std::string>(this->streamMap.at(ref).nsPort,
                        packageSend(streamID, federationID, msg, json, serverCheck)));
                    return true;
                }
                return false;
            }

            void comm_data_send_udp::sendFunc(const std::string& serverIP) {
                sockaddr_in hint;
                int hintLen;
                std::pair<int, std::string> message;
                int sendOk;

                hint.sin_family = AF_INET;
                inet_pton(AF_INET, serverIP.c_str(), &hint.sin_addr);
                hintLen = sizeof(hint);

                while (this->sock != INVALID_SOCKET) {
                    message = sendQueue.dequeue();
                    if (message.first == INVALID_PORT) {
                        continue;
                    }
                    hint.sin_port = message.first;
                    sendOk = sendto(sock, message.second.c_str(), message.second.size(), 0, (sockaddr*)&hint, hintLen);
                    if (sendOk == SOCKET_ERROR) {
                        // TODO: Do error handling
                        continue;
                    }
                }
            }

        }
    }
}