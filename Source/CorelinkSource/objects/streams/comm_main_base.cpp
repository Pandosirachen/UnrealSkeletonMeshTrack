#include "corelink/objects/streams/comm_main_base.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            comm_main_base::comm_main_base(client_main* clientRef) :
                clientRef(clientRef), responseHandler(std::shared_ptr<rapidjson::Document>(nullptr))
            {
                this->threadCallback = std::thread(&comm_main_base::callbackThread, this);
            }

            comm_main_base::~comm_main_base() {
                this->clientRef = nullptr;
                this->callbackQueue.clear();
                this->callbackQueue.enqueue(std::shared_ptr<rapidjson::Document>(nullptr));
                if (this->threadCallback.joinable()) {
                    this->threadCallback.join();
                }
                this->callbackQueue.clear();
            }

            int comm_main_base::reserve() {
                return (int) responseHandler.reserve();
            }

            rapidjson::Document comm_main_base::sendRecv(char* msg, int len, const int& commID) {
                rapidjson::Document recvJson;
                std::shared_ptr<rapidjson::Document> response;
                recvJson.SetObject();

                if (!sendMsg(msg, len)) { return recvJson; }
                response = responseHandler.get(commID);
                if (response) {
                    recvJson.CopyFrom(*response, recvJson.GetAllocator());
                }
                return recvJson;
            }

            void comm_main_base::callbackThread() {
                client_main* _clientRef;
                void (*callbackRef1)(const int&);
                void (*callbackRef2)(const int&, const int&);
                std::shared_ptr<rapidjson::Document> jsonPtr;
                rapidjson::Document recvJson;
                rapidjson::Value jsonData;
                // int tmpVal;
                // STREAM_ID streamID1;
                // STREAM_ID streamID2;
                ServerCallback callback;

                while ((_clientRef = this->clientRef) != nullptr) {
                    jsonPtr = this->callbackQueue.dequeue();
                    if (!jsonPtr) { continue; }
                    recvJson.CopyFrom(*jsonPtr, recvJson.GetAllocator());
                    jsonPtr.reset();
                    
                    callback = strToCallback(recvJson["function"].GetString());
                    if (callback == ServerCallback::LAST) { continue; }

                    switch (callback) {
                    case ServerCallback::DROPPED://update sender when reciever listening is destroyed
                        if ((callbackRef1 = _clientRef->data.onDropHandler) != nullptr) {
                            callbackRef1(recvJson["streamID"].GetInt());
                        }
                        break;
                    case ServerCallback::STALE://reciever identify when a send stream is dead
                        if ((callbackRef1 = _clientRef->data.onStaleHandler) != nullptr) {
                            callbackRef1(recvJson["streamID"].GetInt());
                        }
                        _clientRef->rmSource(recvJson["streamID"].GetInt());
                        break;
                    case ServerCallback::SUBSCRIBE://update a sender when reciever subscribes
                        if ((callbackRef2 = _clientRef->data.onSubscribeHandler) != nullptr) {
                            callbackRef2(recvJson["senderID"].GetInt(), recvJson["receiverID"].GetInt());
                        }
                        /*
                        jsonData = recvJson["type"];
                        if (jsonData.IsString()) {
                            tmpVal = 1;
                            data = new const char* [6];
                            data[5] = jsonData.GetString();
                        }
                        else {
                            tmpVal = jsonData.GetArray().Size();
                            data = new const char* [5 + tmpVal];
                            for (int i = 0; i < tmpVal; ++i) {
                                data[5 + i] = jsonData.GetArray()[i].GetString();
                            }
                        }
                        // ****FIX THIS LATER TO HANDLE MULTIPLE****
                        streamID1 = recvJson["senderID"].GetInt();
                        data[0] = (const char*)&streamID1;
                        streamID2 = recvJson["receiverID"].GetInt();
                        data[1] = (const char*)&streamID2;
                        data[2] = recvJson["user"].GetString();
                        data[3] = recvJson["meta"].GetString();
                        data[4] = (const char*)&tmpVal;
                        callbackRef(data, CALLBACK_SUBSCRIBE);*/
                        break;
                    case ServerCallback::UPDATE:
                        if ((callbackRef2 = _clientRef->data.onUpdateHandler) != nullptr) {
                            callbackRef2(recvJson["receiverID"].GetInt(), recvJson["streamID"].GetInt());
                        }
                        /*
                        data = new const char*[5];
                        streamID1 = recvJson["receiverID"].GetInt();
                        data[0] = (const char*)&streamID1;
                        streamID2 = recvJson["streamID"].GetInt();
                        data[1] = (const char*)&streamID2;
                        data[2] = recvJson["user"].GetString();
                        data[3] = recvJson["meta"].GetString();
                        data[4] = recvJson["type"].GetString();
                        callbackRef(data, CALLBACK_UPDATE);*/
                        break;
                    }
                }
            }
        }
    }
}