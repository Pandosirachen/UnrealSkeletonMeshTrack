/**
 * @file initialization_data.h
 * @brief Data stored for handling startup of the dll.
 */
#ifndef CORELINK_OBJECTS_INITIALIZATIONDATA_H
#define CORELINK_OBJECTS_INITIALIZATIONDATA_H

#include "corelink/headers/header.h"
#include "corelink/objects/generics/message_handler.h"

namespace CorelinkDLL {
    namespace Object {
        class initialization_data {
        public:
            /// Client initialization state.
            bool clientInit;

            CorelinkDLL::Object::Generic::message_handler<std::string> msgHandler;

            /// State the client initialize in.
            int initState;

            /// Absolute path to the file for localhost certification.
            std::string certClientFileName;

            /// Absolute path to the file for server certification.
            std::string certServerFileName;

            std::string username;
            std::string password;
            
            /// Handler for when subscriber is dropped or unsubscribed from sender.
            void (*onDropHandler)(const int&);

            // TODO: Ask to add the receiver id in the OnStale call.
            /// Handler for when sender is dropped or receiver unsubscribed from sender.
            void (*onStaleHandler)(const int&);

            /// Handler for when a sender(param1) is subscribed by a new receiver(param2).
            void (*onSubscribeHandler)(const int&, const int&);

            /// Handler for when a receiver(param1) has a new sender(param2) available.
            void (*onUpdateHandler)(const int&, const int&);

            mutable std::mutex lock;
        public:
            initialization_data();
            ~initialization_data();

            initialization_data(const initialization_data& rhs);

            initialization_data& operator=(const initialization_data& rhs);
        };
    }
}
#endif