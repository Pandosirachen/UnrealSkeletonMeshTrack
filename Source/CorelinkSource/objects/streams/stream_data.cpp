#include "corelink/objects/streams/stream_data.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            stream_data::stream_data() :
                streamID(STREAM_DEF), state(STREAM_STATE_NONE), mtu(0), user(), workspace(), meta(), type(), sources()
            {
                stateIndex = -1;
                streamData = compact(streamID, state, mtu, user, workspace, meta, type);
            }

            stream_data::stream_data(const int& streamID, const int& state, const int& mtu, 
                    const std::string& user, const std::string& workspace, const std::string& meta, 
                    const std::vector<std::string>& type, const std::unordered_set<int>& sources) :
                streamID(streamID), state(state), mtu(mtu), user(user), workspace(workspace), meta(meta), type(type), sources(sources)
            {
                stateIndex = streamStateToBitIndex(state);
                streamData = compact(streamID, state, mtu, user, workspace, meta, type);
            }

            stream_data::stream_data(const stream_data& rhs) :
                streamID(rhs.streamID), state(rhs.state), stateIndex(rhs.stateIndex), mtu(rhs.mtu), user(rhs.user), workspace(rhs.workspace), meta(rhs.meta), type(rhs.type), sources(rhs.sources), streamData(rhs.streamData)
            {}

            stream_data::~stream_data() {}

            stream_data& stream_data::operator=(const stream_data& rhs) {
                this->streamID = rhs.streamID;
                this->state = rhs.state;
                this->stateIndex = rhs.stateIndex;
                this->mtu = rhs.mtu;
                this->user = rhs.user;
                this->workspace = rhs.workspace;
                this->meta = rhs.meta;
                this->type = rhs.type;
                this->sources = rhs.sources;
                this->streamData = rhs.streamData;
                return *this;
            }

            std::string stream_data::compact(const int& streamID, const int& state, const int& mtu, 
                    const std::string& user, const std::string& workspace, const std::string& meta,
                    const std::vector<std::string>& type) {
                std::string compactString;
                char* compactStringPtr;
                int* compactStringPtrCasted;
                int lenData, lenHeader;
                int lenTypes;

                lenTypes = (int)type.size();
                lenHeader = (lenTypes + 7) * sizeof(int);
                lenData = sizeof(int) * 3 + (int) (user.size() + workspace.size() + meta.size());
                for (const std::string& val : type) {
                    lenData += (int)val.size();
                }
                compactStringPtr = new char[lenHeader + lenData];
                compactStringPtrCasted = (int*) compactStringPtr;
                compactStringPtrCasted[0] = (int)type.size() + 6;
                compactStringPtrCasted[1] = compactStringPtrCasted[2] = compactStringPtrCasted[3] = sizeof(int);
                compactStringPtrCasted[4] = (int) user.size();
                compactStringPtrCasted[5] = (int) workspace.size();
                compactStringPtrCasted[6] = (int) meta.size();
                for (int i = 0; i < lenTypes; ++i) {
                    compactStringPtrCasted[7 + i] = type[i].size();
                }
                compactStringPtrCasted[7 + lenTypes] = streamID;
                compactStringPtrCasted[8 + lenTypes] = state;
                compactStringPtrCasted[9 + lenTypes] = mtu;
                lenData = sizeof(int) * 3;

                memcpy(compactStringPtr + lenData + lenHeader, user.c_str(), user.size());
                lenData += (int) user.size();
                memcpy(compactStringPtr + lenData + lenHeader, workspace.c_str(), workspace.size());
                lenData += (int) workspace.size();
                memcpy(compactStringPtr + lenData + lenHeader, meta.c_str(), meta.size());
                lenData += (int) meta.size();
                for (int i = 0; i < lenTypes; ++i) {
                    memcpy(compactStringPtr + lenData + lenHeader, type[i].c_str(), type[i].size());
                    lenData += (int) type[i].size();
                }

                compactStringPtrCasted = nullptr;
                compactString = std::string(compactStringPtr, lenHeader + lenData);
                delete[] compactStringPtr;
                return compactString;
            }
        }
    }
}