#include "corelink/objects/client_main.h"

#include "corelink/objects/streams/comm_main_tcp.h"

#include "corelink/objects/streams/comm_data_send_base.h"
#include "corelink/objects/streams/comm_data_send_tcp.h"
#include "corelink/objects/streams/comm_data_send_udp.h"

#include "corelink/objects/streams/comm_data_recv_base.h"
#include "corelink/objects/streams/comm_data_recv_tcp.h"
#include "corelink/objects/streams/comm_data_recv_udp.h"

namespace CorelinkDLL {
    namespace Object {
        client_main::client_main(const initialization_data& data, int& errorID) : data(data) {
            this->serverIP = "0.0.0.0";
            this->serverIPEffective = "";
            this->serverPort = 0;
            this->token = "";
            this->clientIP = "";

            this->mainComm = nullptr;

            this->mainComm = new CorelinkDLL::Object::Stream::comm_main_tcp(this);
            // this->mainComm = new CorelinkDLL::Object::Stream::comm_main_ws(this);

            this->dataStreams = new CorelinkDLL::Object::Stream::comm_data_base*[(int) StreamStateBitIndex::LAST];
            for (int i = 0; i < (int) StreamStateBitIndex::LAST; ++i) {
                dataStreams[i] = nullptr;
            }
        }

        client_main::~client_main() {
            cleanDataStreams();
            if (this->mainComm != nullptr){
                delete this->mainComm;
                this->mainComm = nullptr;
            }
        }

        void client_main::initDataStreams(const std::string& effectiveIP, int& errorID) {
            this->serverIPEffective = effectiveIP;
            // dont actually need the != 0, leaving it there for clarity.
            if ((this->data.initState & STREAM_STATE_SEND_UDP) != 0) {
                this->dataStreams[streamStateToBitIndex(STREAM_STATE_SEND_UDP)] = new CorelinkDLL::Object::Stream::comm_data_send_udp(this->serverIPEffective, errorID);
                if (errorID > 0) { return; }
            }
            if ((this->data.initState & STREAM_STATE_SEND_TCP) != 0) {
                this->dataStreams[streamStateToBitIndex(STREAM_STATE_SEND_TCP)] = new CorelinkDLL::Object::Stream::comm_data_send_tcp();
                if (errorID > 0) { return; }
            }
            if ((this->data.initState & STREAM_STATE_SEND_WS) != 0) {
                // this->dataStreams[streamStateToBitIndex(STREAM_STATE_SEND_WS)] = new CorelinkDLL::Object::Stream::comm_data_send_ws(this->wsHandler);
                this->dataStreams[streamStateToBitIndex(STREAM_STATE_SEND_WS)] = nullptr;
                if (errorID > 0) { return; }
            }
            if ((this->data.initState & STREAM_STATE_RECV_UDP) != 0) {
                this->dataStreams[streamStateToBitIndex(STREAM_STATE_RECV_UDP)] = new CorelinkDLL::Object::Stream::comm_data_recv_udp();
                if (errorID > 0) { return; }
            }
            if ((this->data.initState & STREAM_STATE_RECV_TCP) != 0) {
                this->dataStreams[streamStateToBitIndex(STREAM_STATE_RECV_TCP)] = new CorelinkDLL::Object::Stream::comm_data_recv_tcp();
                if (errorID > 0) { return; }
            }
            if ((this->data.initState & STREAM_STATE_RECV_WS) != 0) {
                //this->dataStreams[streamStateToBitIndex(STREAM_STATE_RECV_WS)] = new CorelinkDLL::Object::Stream::comm_data_recv_ws(this->wsHandler);
                this->dataStreams[streamStateToBitIndex(STREAM_STATE_RECV_WS)] = nullptr;
                if (errorID > 0) { return; }
            }
        }

        void client_main::cleanDataStreams() {
            for (int i = 0; i < (int) StreamStateBitIndex::LAST; ++i) {
                if (this->dataStreams[i] != nullptr) {
                    delete this->dataStreams[i];
                    this->dataStreams[i] = nullptr;
                }
            }
        }

        void client_main::addStream(const CorelinkDLL::Object::Stream::stream_data& streamData, int protocol, int port) {
            if (this->dataStreams[streamData.stateIndex] != nullptr) {
                std::lock_guard<std::mutex> lck(this->streamLock);
                this->mapStreamData[streamData.streamID] = streamData;
                this->dataStreams[streamData.stateIndex]->addStream(streamData.streamID, this->serverIPEffective, port);
            }
        }

        void client_main::rmStream(const STREAM_ID& streamID) {
            std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>::iterator iter;
            std::lock_guard<std::mutex> lck(this->streamLock);
            if ((iter = this->mapStreamData.find(streamID)) == this->mapStreamData.end()) { return; }
            this->dataStreams[iter->second.stateIndex]->rmStream(streamID);
            this->mapStreamData.erase(iter);
        }

        int client_main::getStreamRef(const STREAM_ID& streamID) {
            std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>::iterator iter;
            std::lock_guard<std::mutex> lck(this->streamLock);
            if ((iter = this->mapStreamData.find(streamID)) == this->mapStreamData.end()) { return -1; }
            return this->dataStreams[iter->second.stateIndex]->getStreamRef(streamID);
        }

        int client_main::getStreamState(const STREAM_ID& streamID) {
            std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>::iterator iter;
            std::lock_guard<std::mutex> lck(this->streamLock);
            if ((iter = this->mapStreamData.find(streamID)) == this->mapStreamData.end()) { return STREAM_STATE_NONE; }
            return iter->second.state;
        }

        std::string client_main::getStreamData(const STREAM_ID& streamID) {
            std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>::iterator iter;
            std::lock_guard<std::mutex> lck(this->streamLock);
            if ((iter = this->mapStreamData.find(streamID)) == this->mapStreamData.end()) { return ""; }
            return iter->second.streamData;
        }

        bool client_main::streamIsType(const STREAM_ID& streamID, int streamState) {
            std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>::iterator iter;
            std::lock_guard<std::mutex> lck(this->streamLock);
            if ((iter = this->mapStreamData.find(streamID)) == this->mapStreamData.end()) { return false; }
            return (iter->second.state & streamState) > 0;
        }
        
        std::vector<int> client_main::getStreams() {
            std::vector<int> streams = std::vector<int>();
            std::lock_guard<std::mutex> lck(this->streamLock);
            streams.reserve(this->mapStreamData.size());
            for (const std::pair<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>& streamData : this->mapStreamData) {
                streams.push_back(streamData.second.streamID);
            }
            return streams;
        }

        std::vector<int> client_main::getStreamSources(const STREAM_ID& streamID) {
            std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>::iterator iter;
            std::lock_guard<std::mutex> lck(this->streamLock);
            if ((iter = this->mapStreamData.find(streamID)) == this->mapStreamData.end()) { return {}; }
            return std::vector<int>(iter->second.sources.begin(), iter->second.sources.end());
        }

        void client_main::addSource(const STREAM_ID& targetID, const STREAM_ID& sourceID) {
            std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>::iterator iter;
            std::lock_guard<std::mutex> lck(this->streamLock);
            if ((iter = this->mapStreamData.find(targetID)) == this->mapStreamData.end()) { return; }
            iter->second.sources.insert(sourceID);
        }

        void client_main::rmSource(const STREAM_ID& sourceID) {
            std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>::iterator iter;
            std::lock_guard<std::mutex> lck(this->streamLock);
            iter = this->mapStreamData.begin();
            while (iter != this->mapStreamData.end()) {
                iter->second.sources.erase(sourceID);
                ++iter;
            }
        }

        void client_main::rmSource(const STREAM_ID& targetID, const STREAM_ID& sourceID) {
            std::unordered_map<STREAM_ID, CorelinkDLL::Object::Stream::stream_data>::iterator iter;
            std::lock_guard<std::mutex> lck(this->streamLock);
            if ((iter = this->mapStreamData.find(targetID)) == this->mapStreamData.end()) { return; }
            iter->second.sources.erase(sourceID);
        }
    }
}