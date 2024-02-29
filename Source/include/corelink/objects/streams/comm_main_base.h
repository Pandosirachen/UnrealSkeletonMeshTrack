/**
 * @file comm_main_base.h
 * @brief Base class for main communication with corelink server.
 */
#ifndef CORELINK_OBJECTS_STREAMS_COMMMAINBASE_H
#define CORELINK_OBJECTS_STREAMS_COMMMAINBASE_H

#include "corelink/headers/header.h"
#include "corelink/objects/client_main.h"


namespace CorelinkDLL {
    namespace Object {
        class client_main;
    }
}
namespace CorelinkDLL {
    namespace Object {
        namespace Stream {
            class comm_main_base {
            protected:
                /// Reference to client.
                client_main* clientRef;

                /// Stores the server's response to send.
                CorelinkDLL::Object::Generic::message_handler<std::shared_ptr<rapidjson::Document>> responseHandler;

                /// Queue to pass data to the callback thread.
                CorelinkDLL::Object::Generic::safe_queue<std::shared_ptr<rapidjson::Document>> callbackQueue;
                /// Thread for server callback.
                std::thread threadCallback;

                comm_main_base() = delete;
                comm_main_base(client_main* clientRef);

            public:
                virtual ~comm_main_base();

                /**
                 * Sends message to server.
                 * @param msg Message to send to server.
                 * @param len Length of message.
                 * @return Success of sending message.
                 */
                virtual bool sendMsg(char* msg, int len) = 0;

                /**
                 * Connects the client to the server using ip and port.
                 * @param username Username credential.
                 * @param password Password credential.
                 * @return Effective server ip (IPv4 needs to be updated for websockets) to use if successful.
                 */
                virtual std::string connectServer(const std::string& ip, const int& port, int& errorID) = 0;

                /**
                 * Reserves a unique commID to use.
                 * @return unique value.
                 */
                int reserve();

                /**
                 * Sends message with unique id and recieves response.
                 * @param msg Json message to send to server.
                 * @param len Length of message sent to server.
                 * @param commID Unique identifier of message in event of error.
                 * @return Json response.
                 */
                rapidjson::Document sendRecv(char* msg, int len, const int& commID);

                //TODO: Add more data to SUBSCRIBE and UPDATE functionality if necessary. Additional data currently commented.
                /**
                 * Callback thread for server to client messages.
                 */
                void callbackThread();
            };
        }
    }
}
#endif