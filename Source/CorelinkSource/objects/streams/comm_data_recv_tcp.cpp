#include "corelink/objects/streams/comm_data_recv_tcp.h"
#include "corelink/objects/streams/tcp_recv_handler.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            recv_stream_data_tcp::recv_stream_data_tcp(const std::string& serverIP, int port) : recv_stream_data_base() {
                sock = INVALID_SOCKET;
                hint.sin_family = AF_INET;
                inet_pton(AF_INET, serverIP.c_str(), &hint.sin_addr);
                hint.sin_port = htons(port);
                if (port > 0 && port < 65535) {
                    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                    if (sock != INVALID_SOCKET) {
                        connect(sock, (sockaddr*)&hint, sizeof(hint));
                    }
                }
            }

            recv_stream_data_tcp::recv_stream_data_tcp(const recv_stream_data_tcp& rhs) : recv_stream_data_base(rhs) {
                this->hint = rhs.hint;
                this->sock = rhs.sock;
            }

            recv_stream_data_tcp::~recv_stream_data_tcp() {}

            recv_stream_data_tcp& recv_stream_data_tcp::operator=(const recv_stream_data_tcp& rhs) {
                recv_stream_data_base::operator=(rhs);
                this->hint = rhs.hint;
                this->sock = rhs.sock;
                return *this;
            }

            recv_stream_data_tcp& recv_stream_data_tcp::operator=(recv_stream_data_tcp&& rhs) {
                recv_stream_data_base::operator=(std::move(rhs));
                this->hint = rhs.hint;
                this->sock = std::move(rhs.sock);
                this->listener = std::move(rhs.listener);
                return *this;
            }

            comm_data_recv_tcp::comm_data_recv_tcp() : comm_data_recv_base() {}
            
            comm_data_recv_tcp::~comm_data_recv_tcp() {}

            void comm_data_recv_tcp::addStream(const STREAM_ID& streamID, const std::string& ip, int port) {
                this->streamMap.addObject(streamID, new recv_stream_data_tcp(ip, port));
                int ref = this->streamMap.getStreamRedirect(streamID);
                CorelinkDLL::setRecvSocketOpts(((recv_stream_data_tcp*)this->streamMap.at(ref))->sock);
                ((recv_stream_data_tcp*)this->streamMap.at(ref))->listener = std::thread(&comm_data_recv_tcp::recvFunc, this, ref);
            }

            int comm_data_recv_tcp::getStreamRef(const STREAM_ID& streamID) {
                return this->streamMap.getStreamRedirect(streamID);
            }

            void comm_data_recv_tcp::rmStream(const STREAM_ID& streamID) {
                int ref = this->streamMap.getStreamRedirect(streamID);
                recv_stream_data_tcp* streamData = (recv_stream_data_tcp*)this->streamMap.at(ref);
                SOCKET sock = streamData->sock;
                streamData->sock = INVALID_SOCKET;
                shutdown(sock, SD_BOTH);
                closesocket(sock);
                if (streamData->listener.joinable()) {
                    streamData->listener.join();
                }
                this->streamMap.rmObjectIndex(ref);
                delete streamData;
            }

            void comm_data_recv_tcp::recvFunc(int ref) {
                recv_stream_data_tcp* streamData = (recv_stream_data_tcp*)this->streamMap.at(ref);
                tcp_recv_handler recvHandler = tcp_recv_handler(streamData->sock);
                STREAM_ID streamID = this->streamMap.getStream(ref);

                int bytesRecieved;
                char* tmpArr;
                unsigned char* dataArr;
                int source;
                int hdrLen;
                int msgLen;
                int totLen;

                std::string package = packageSend(streamID, 0, "");
                msgLen = 0;
                while (streamData->sock != INVALID_SOCKET && msgLen < package.size()) {
                    totLen = send(streamData->sock, package.c_str() + msgLen, package.size() - msgLen, 0);
                    if (totLen == -1) {
                        //something went wrong. silent failure
                        return;
                    }
                    msgLen += totLen;
                }
                
                totLen = -1;
                rapidjson::Document json;
                json.SetObject();

                while (streamData->sock != INVALID_SOCKET) {
                    if (totLen == -1 && recvHandler.size() > 4) {
                        dataArr = (unsigned char*)recvHandler.getData(4);
                        hdrLen = dataArr[0] + (dataArr[1] << 8);
                        msgLen = dataArr[2] + (dataArr[3] << 8);
                        totLen = hdrLen + msgLen + 4;
                        delete[] dataArr;
                    }

                    if (totLen > 0 && totLen <= recvHandler.size()) {
                        dataArr = (unsigned char*)recvHandler.getData(4, 0);
                        source = dataArr[0] + (dataArr[1] << 8) + (dataArr[2] << 16) + (dataArr[3] << 24);
                        tmpArr = recvHandler.getData(hdrLen + msgLen, 0);
                        // take off high bit 
                        streamData->callFunc(streamID, source, tmpArr, hdrLen & 32767, msgLen);
                        delete[] dataArr;
                        delete[] tmpArr;
                        totLen = -1;
                        continue;
                    }
                    recvHandler.recvData(bytesRecieved);
                }
            }
        }
    }
}