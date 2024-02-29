#include "corelink/objects/streams/comm_data_base.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            // TODO: validate if the target parameter is necessary/how is it used. (alternative would be passing json string for server data)
            std::string comm_data_base::packageSend(const STREAM_ID& stream, const int& federationID, const std::string& msg, const std::string& json, bool serverCheck) {
                char* package;
                int len;
                int lenHead;
                int lenData;
                
                lenHead = (int) json.size();
                lenData = (int) msg.size();

                len = 8 + lenHead + lenData;
                package = new char[len];

                // header size + set high bit if necessary
                package[0] = (char)(lenHead >> 0);
                package[1] = (char)((lenHead >> 8) | (serverCheck ? 128 : 0));
                // data size
                package[2] = (char)(lenData >> 0);
                package[3] = (char)(lenData >> 8);
                // stream id
                package[4] = (char)(stream >> 0);
                package[5] = (char)(stream >> 8);
                package[6] = (char)(federationID >> 0);
                package[7] = (char)(federationID >> 8);
                // header
                memcpy(package + 8, json.c_str(), lenHead);
                // data
                memcpy(package + 8 + lenHead, msg.c_str(), lenData);
                
                std::string ret(package, len);
                delete[] package;
                return ret;
            }
        }
    }
}

/* TODO: (When necessary) extend functionality to allow appending json data to stream messages.
rapidjson::Document doc;
doc.SetObject();
rapidjson::StringBuffer buffer;
rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
doc.Accept(writer);
tmpHead = buffer.GetSize();
if (tmpHead <= 2) {
    tmpHead = 0;
}
*/