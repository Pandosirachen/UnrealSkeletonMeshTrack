#include "corelink/objects/streams/comm_data_send_tcp.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            comm_data_send_tcp::comm_data_send_tcp() {
                this->running = true;
                this->sendThread = std::thread(&comm_data_send_tcp::sendFunc, this);
            }

            comm_data_send_tcp::~comm_data_send_tcp() {
                this->running = false;
                this->sendQueue.clear();
                this->sendQueue.enqueue(std::pair<SOCKET, std::string>(INVALID_SOCKET, ""));
                if (this->sendThread.joinable()) {
                    this->sendThread.join();
                }
                this->sendQueue.clear();
            }

            void comm_data_send_tcp::addStream(const STREAM_ID& streamID, const std::string& ip, int port) {
                this->streamMap.addObject(streamID, DataSenderTCP(ip, port));
            }

            int comm_data_send_tcp::getStreamRef(const STREAM_ID& streamID) {
                return this->streamMap.getStreamRedirect(streamID);
            }

            void comm_data_send_tcp::rmStream(const STREAM_ID& streamID) {
                int ref = this->streamMap.getStreamRedirect(streamID);
                closesocket(this->streamMap.at(ref).sock);
                this->streamMap.rmObjectIndex(ref);
            }
            
            bool comm_data_send_tcp::sendMsg(int ref, const STREAM_ID& streamID, const int& federationID, const std::string& msg) {
                if (this->streamMap.getStream(ref) == streamID) {
                    this->sendQueue.enqueue(std::pair<int, std::string>(this->streamMap.at(ref).sock,
                        packageSend(streamID, federationID, msg)));
                    return true;
                }
                return false;
            }

            bool comm_data_send_tcp::sendMsg(int ref, const STREAM_ID& streamID, const int& federationID, const std::string& msg, const std::string& json, bool serverCheck) {
                if (this->streamMap.getStream(ref) == streamID) {
                    this->sendQueue.enqueue(std::pair<int, std::string>(this->streamMap.at(ref).sock,
                        packageSend(streamID, federationID, msg, json, serverCheck)));
                    return true;
                }
                return false;
            }

            void comm_data_send_tcp::sendFunc() {
                std::pair<SOCKET, std::string> message;
                int sendOk;
                while (this->running) {
                    message = this->sendQueue.dequeue();
                    int curr = 0;
                    if (message.first == INVALID_SOCKET) { continue; }
                    while (curr < message.second.size()) {
                        sendOk = send(message.first, message.second.c_str() + curr, message.second.size() - curr, 0);
                        if (sendOk == SOCKET_ERROR) { break; }
                        curr += sendOk;
                    }
                }
            }
        }
    }
}