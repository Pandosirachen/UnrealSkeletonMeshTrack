#include "corelink/objects/streams/comm_data_recv_base.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            recv_stream_data_base::recv_stream_data_base(){
                funcPointer = nullptr;
                funcExtra = nullptr;
            }

            recv_stream_data_base::recv_stream_data_base(const recv_stream_data_base& rhs) {
                this->funcPointer = rhs.funcPointer;
                this->funcExtra = rhs.funcExtra;
            }

            recv_stream_data_base::~recv_stream_data_base() {
                funcPointer = nullptr;
                funcExtra = nullptr;
            }

            recv_stream_data_base& recv_stream_data_base::operator=(const recv_stream_data_base& rhs) {
                std::lock_guard<std::mutex> lck(this->funcLock);
                this->funcPointer = rhs.funcPointer;
                this->funcExtra = rhs.funcExtra;
                return *this;
            }

            recv_stream_data_base& recv_stream_data_base::operator=(recv_stream_data_base&& rhs) {
                std::lock_guard<std::mutex> lck(this->funcLock);
                std::lock_guard<std::mutex> lckRhs(rhs.funcLock);
                
                this->funcPointer = std::move(rhs.funcPointer);
                this->funcExtra = std::move(rhs.funcExtra);
                rhs.funcPointer = nullptr;
                rhs.funcExtra = nullptr;
                return *this;
            }

            void* recv_stream_data_base::changeFunc(Callback _funcPointer, void* _funcExtra){
                void* data;
                std::lock_guard<std::mutex> lck(this->funcLock);
                data = this->funcExtra;
                this->funcPointer = _funcPointer;
                this->funcExtra = _funcExtra;
                return data;
            }

            void recv_stream_data_base::callFunc(const STREAM_ID& recvID, const STREAM_ID& sendID, const char* data, const int& jsonLen, const int& msgLen) {
                std::lock_guard<std::mutex> lck(this->funcLock);
                if (this->funcPointer != nullptr){
                    this->funcPointer(recvID, sendID, data, jsonLen, msgLen, this->funcExtra);
                }
            }

            comm_data_recv_base::comm_data_recv_base() : streamMap(nullptr) {}

            void* comm_data_recv_base::setRecvCallback(int ref, const STREAM_ID& streamID, Callback recvCallback, void* callbackData) {
                if (this->streamMap.getStream(ref) != streamID) { return nullptr; }
                return this->streamMap.at(ref)->changeFunc(recvCallback, callbackData);
            }
        }
    }
}