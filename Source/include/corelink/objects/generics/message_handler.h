/**
 * @file message_handler.h
 * @brief Class that handles communication of messages throughout the DLL and to the client interface. Uses a unique key to enforce thread safety.
 */
#ifndef CORELINK_OBJECTS_GENERICS_MESSAGEHANDLER_H
#define CORELINK_OBJECTS_GENERICS_MESSAGEHANDLER_H
#include "corelink/headers/header.h"
#include "corelink/objects/counter.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Generic {
            template<class T>
            class message_handler {
            protected:
                T defVal;

                // Stores messages in a map structure
                std::map<unsigned int, T> messages;
                
                // Gets unique identifiers using the counter.
                CorelinkDLL::Object::counter counter;

                // Number of gets currently trying to retrieve data. Will be set to a negative value when clearing.
                int accessors;
                bool running;
                mutable std::mutex lock;
                std::condition_variable c;

            public:
                message_handler(const T& defVal);
                ~message_handler();
                
                /**
                 * THREADSAFE
                 * Reserves a unique index to use later.
                 */
                unsigned int reserve();

                /**
                 * Adds message to the handler.
                 * @param msg Message to store.
                 * @param index Optional index if reserve() was called earlier
                 * @return Index used to store and retrieve the message. 0 if stopped.
                 */
                unsigned int add(const T& msg, unsigned int index = 0);

                /**
                 * Gets the data at the index and removes it.
                 * @param index Key to retrieve data from.
                 * @param wait Should it retrieve value immediately or poll for data.
                 * @return Data stored at the index.
                 */
                T get(unsigned int index, bool wait = true);

                /**
                 * THREADSAFE
                 * Stops all retrievals and empties the handler.
                 */
                void clear();

                /**
                 * Resumes usage of the message handler.
                 */
                void resume();

            };
        }
    }
}
namespace CorelinkDLL {
    namespace Object {
        namespace Generic {
            /**
             * Specialization for strings.
             */
            template<>
            class message_handler<std::string> {
            private:
                // Stores messages in a map structure
                std::map<unsigned int, std::string> messages;
                
                // Gets unique identifiers using the counter.
                CorelinkDLL::Object::counter counter;

                // Number of gets currently trying to retrieve data. Will be set to a negative value when clearing.
                int accessors;
                bool running;
                mutable std::mutex lock;
                std::condition_variable c;
            public:
                message_handler();
                ~message_handler();

                /**
                 * THREADSAFE
                 * Reserves a unique index to use later.
                 */
                unsigned int reserve();

                /**
                 * Adds message to the handler.
                 * @param msg Message to store.
                 * @param index Optional index if reserve() was called earlier
                 * @return Index used to store and retrieve the message. 0 if stopped.
                 */
                unsigned int add(const std::string& msg, unsigned int index = 0);

                /**
                 * Adds message to the handler.
                 * @param msg Message to store.
                 * @param msgLen Length of data to store.
                 * @param index Optional index if reserve() was called earlier
                 * @return Index used to store and retrieve the message. 0 if stopped.
                 */
                unsigned int add(char* msg, int msgLen, unsigned int index = 0);

                /**
                 * Gets the string at the index.
                 * @param index Key to retrieve string from.
                 * @param wait Should it retrieve value immediately or poll for data.
                 * @return message stored for the index.
                 */
                std::string get(unsigned int index, bool wait = true);

                /**
                 * Gets the length of string at the index.
                 * @param index Key to retrieve string from.
                 * @param wait Should it retrieve value immediately or poll for data.
                 * @return Length of message stored for the index. -1 if the data cannot be retrieved.
                 */
                int getLen(unsigned int index, bool wait = true);

                /**
                 * Gets the string at the index. Assumes that getLen() was successful.
                 * @param index Key to retrieve string from.
                 * @param buffer Array to store the data in.
                 * @return Whether data was succcessfully retrieved.
                 */
                bool getStr(unsigned int index, char* buffer);

                /**
                 * Removes key value pair from the map.
                 * @param index Key to remove.
                 */
                void remove(unsigned int index);

                /**
                 * THREADSAFE
                 * Stops all retrievals and empties the handler.
                 */
                void clear();

                /**
                 * Resumes usage of the message handler.
                 */
                void resume();

            private:
            };
        }
    }
}

namespace CorelinkDLL {
    namespace Object {
        namespace Generic {
            template<class T>
            message_handler<T>::message_handler(const T& defVal) : defVal(defVal), accessors(0), running(true) {}
            
            template<class T>
            message_handler<T>::~message_handler() {
                clear();
            }

            template<class T>
            inline unsigned int message_handler<T>::reserve() {
                return counter.get();
            }

            template<class T>
            inline unsigned int message_handler<T>::add(const T& msg, unsigned int index) {
                if (index == 0) { index = counter.get(); }
                {
                    std::lock_guard<std::mutex> lck(lock);
                    if (!running) { return 0; }
                    messages.insert(std::pair<unsigned int, T>(index, msg));
                }
                c.notify_all();
                return index;
            }

            template<class T>
            inline T message_handler<T>::get(unsigned int index, bool wait) {
                T ret = defVal;
                typename std::map<unsigned int, T>::iterator it = messages.end();
                if (!wait) {
                    it = messages.find(index);
                }
                else {
                    std::unique_lock<std::mutex> lck(lock);
                    if (messages.empty()) { c.wait(lck); }
                    while (running && (it = messages.find(index)) == messages.end()) {
                        c.wait(lck);
                    }
                }
                {
                    std::lock_guard<std::mutex> lck(lock);
                    if (it != messages.end()) {
                        ret = it->second;
                        messages.erase(it);
                    }
                }
                return ret;
            }

            template<class T>
            inline void message_handler<T>::clear() {
                {
                    std::lock_guard<std::mutex> lck(lock);
                    messages.clear();
                    running = false;
                }
                c.notify_all();
            }

            template<class T>
            inline void message_handler<T>::resume() {
                std::lock_guard<std::mutex> lck(lock);
                messages.clear();
                running = true;
            }
        }
    }
}

namespace CorelinkDLL {
    namespace Object {
        namespace Generic {
            inline message_handler<std::string>::message_handler() : accessors(0), running(true) {}

            inline message_handler<std::string>::~message_handler() {
                clear();
            }

            inline unsigned int message_handler<std::string>::reserve() {
                return counter.get();
            }

            inline unsigned int message_handler<std::string>::add(const std::string& msg, unsigned int index) {
                if (index == 0) { index = counter.get(); }
                {
                    std::lock_guard<std::mutex> lck(lock);
                    if (!running) { return 0; }
                    messages.insert(std::pair<unsigned int, std::string>(index, msg));
                }
                c.notify_all();
                return index;
            }

            inline unsigned int message_handler<std::string>::add(char* msg, int msgLen, unsigned int index) {
                return add(std::string(msg, msgLen), index);
            }

            inline std::string message_handler<std::string>::get(unsigned int index, bool wait) {
                std::string ret = "";
                typename std::map<unsigned int, std::string>::iterator it = messages.end();
                if (!wait) {
                    it = messages.find(index);
                }
                else {
                    std::unique_lock<std::mutex> lck(lock);
                    if (messages.empty()) { c.wait(lck); }
                    while (running && (it = messages.find(index)) == messages.end()) {
                        c.wait(lck);
                    }
                }
                {
                    std::lock_guard<std::mutex> lck(lock);
                    if (it != messages.end()) {
                        ret = it->second;
                        messages.erase(it);
                    }
                }
                return ret;
            }

            inline int message_handler<std::string>::getLen(unsigned int index, bool wait) {
                int ret = -1;
                typename std::map<unsigned int, std::string>::iterator it = messages.end();
                if (!wait) {
                    it = messages.find(index);
                }
                else {
                    std::unique_lock<std::mutex> lck(lock);
                    if (messages.empty()) { c.wait(lck); }
                    while (running && (it = messages.find(index)) == messages.end()) {
                        c.wait(lck);
                    }
                }
                {
                    std::lock_guard<std::mutex> lck(lock);
                    if (it != messages.end()) {
                        ret = it->second.size();
                    }
                }
                return ret;
            }

            inline bool message_handler<std::string>::getStr(unsigned int index, char* buffer) {
                std::map<unsigned int, std::string>::iterator it;
                std::lock_guard<std::mutex> lck(lock);
                it = messages.find(index);
                if (it == messages.end()) { return false; }
                memcpy(buffer, it->second.c_str(), it->second.size());
                return true;
            }

            inline void message_handler<std::string>::remove(unsigned int index) {
                std::lock_guard<std::mutex> lck(lock);
                messages.erase(index);
            }

            inline void message_handler<std::string>::clear() {
                {
                    std::lock_guard<std::mutex> lck(lock);
                    messages.clear();
                    running = false;
                }
                c.notify_all();
            }

            inline void message_handler<std::string>::resume() {
                std::lock_guard<std::mutex> lck(lock);
                messages.clear();
                running = true;
            }
        }
    }
}

#endif
