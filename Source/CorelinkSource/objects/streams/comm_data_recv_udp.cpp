#include "corelink/objects/streams/comm_data_recv_udp.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            recv_stream_data_udp::recv_stream_data_udp(int port) : recv_stream_data_base() {
                this->nsPort = INVALID_PORT;
                if (port > 0 && port <= 65535) {
                    this->nsPort = htons(port);
                }
                this->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                // TODO: do some error handling later.
                if (this->sock == INVALID_SOCKET) {}
                CorelinkDLL::setRecvSocketOpts(sock);
            }

            recv_stream_data_udp::recv_stream_data_udp(const recv_stream_data_udp& rhs) : recv_stream_data_base(rhs) {
                this->nsPort = rhs.nsPort;
                this->sock = rhs.sock;
            }

            recv_stream_data_udp::~recv_stream_data_udp() {}

            recv_stream_data_udp& recv_stream_data_udp::operator=(const recv_stream_data_udp& rhs) {
                recv_stream_data_base::operator=(rhs);
                this->nsPort = rhs.nsPort;
                this->sock = rhs.sock;
                return *this;
            }

            recv_stream_data_udp& recv_stream_data_udp::operator=(recv_stream_data_udp&& rhs) {
                recv_stream_data_base::operator=(std::move(rhs));
                this->nsPort = rhs.nsPort;
                this->sock = std::move(rhs.sock);
                this->listener = std::move(rhs.listener);
                return *this;
            }

            comm_data_recv_udp::comm_data_recv_udp() : comm_data_recv_base() {}

            comm_data_recv_udp::~comm_data_recv_udp() {}

            void comm_data_recv_udp::addStream(const STREAM_ID& streamID, const std::string& ip, int port) {
                this->streamMap.addObject(streamID, new recv_stream_data_udp(port));
                int ref = this->streamMap.getStreamRedirect(streamID);
                ((recv_stream_data_udp*)this->streamMap.at(ref))->listener = std::thread(&comm_data_recv_udp::recvFunc, this, ref, ip);
            }

            int comm_data_recv_udp::getStreamRef(const STREAM_ID& streamID) {
                return this->streamMap.getStreamRedirect(streamID);
            }

            void comm_data_recv_udp::rmStream(const STREAM_ID& streamID) {
                int ref = this->streamMap.getStreamRedirect(streamID);
                recv_stream_data_udp* streamData = (recv_stream_data_udp*)this->streamMap.at(ref);
                SOCKET sock = streamData->sock;
                streamData->sock = INVALID_SOCKET;
                streamData->nsPort = INVALID_PORT;
                shutdown(sock, SD_BOTH);
                closesocket(sock);
                if (streamData->listener.joinable()) {
                    streamData->listener.join();
                }
                this->streamMap.rmObjectIndex(ref);
                delete streamData;
            }

            void comm_data_recv_udp::recvFunc(int ref, const std::string& ip) {
                recv_stream_data_udp* streamData = (recv_stream_data_udp*)this->streamMap.at(ref);
                STREAM_ID streamID = this->streamMap.getStream(ref);

                sockaddr_in hint;
                hint.sin_family = AF_INET;
                inet_pton(AF_INET, ip.c_str(), &hint.sin_addr);
                hint.sin_port = streamData->nsPort;

                socklen_t addrlen = sizeof(hint);
                std::string package = packageSend(streamID, 0, "");
                sendto(streamData->sock, package.c_str(), package.size(), 0, (sockaddr*)&hint, addrlen);

                int bytesRecieved;
                char* buffer;
                unsigned char* bufferCasted;
                int source;
                SOCKET sock;
                int hdrLen, msgLen;

                buffer = new char[SOCKET_RECV_BUFFER_SIZE];
                bufferCasted = (unsigned char*) buffer;

                /*
                * Stream data:
                * -2 bytes header length.
                * -2 bytes message length.
                * -4 bytes sender id.
                * -header.
                * -message.
                */

                while ((sock = streamData->sock) != INVALID_SOCKET) {
                    bytesRecieved = recvfrom(sock, buffer, SOCKET_RECV_BUFFER_SIZE, 0, (sockaddr*)&hint, &addrlen);
                    if (bytesRecieved <= 0) { continue; }
                    hdrLen = bufferCasted[0] + (bufferCasted[1] << 8);
                    msgLen = bufferCasted[2] + (bufferCasted[3] << 8);

                    if (hdrLen + msgLen + 8 != bytesRecieved) { continue; }
                    source = bufferCasted[4] + (bufferCasted[5] << 8) + (bufferCasted[6] << 16) + (bufferCasted[7] << 24);
                    // take off high bit
                    streamData->callFunc(streamID, source, buffer + 8, hdrLen & 32767, msgLen);
                }
                delete[] buffer;
                buffer = nullptr;
                bufferCasted = nullptr;
            }

        }
    }
}