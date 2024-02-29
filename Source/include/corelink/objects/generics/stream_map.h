/**
 * @file stream_map.h
 * @brief Data structure to store and map streams as well as check if it exists.
 * Emphasis is on quick checking/retrieval of a stream by index.
 * In exchange, requires a longer setup and extra memory.
 * Type T is the data stored in the stream map.
 */
#ifndef CORELINK_OBJECTS_GENERICS_STREAMMAP_H
#define CORELINK_OBJECTS_GENERICS_STREAMMAP_H
#include "corelink/headers/header.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Generic {
            template <class T>
            class stream_map
            {
            private:
                // dynamic array of stream ids of size cap
                STREAM_ID* streams;

                // dynamic array of stream data of size cap
                T* data;
                
                // maps from STREAM_ID to the index of data
                std::unordered_map<STREAM_ID, int> redirect;

                // stores the next available index to store new stream in.
                std::priority_queue<int, std::vector<int>, std::greater<int>> slots;
                
                //capacity
                int cap;
                mutable std::mutex m;
            public:
                const T defaultVal;

                /**
                 * @param defaultVal Default value stored in map. Can be used for error checking.
                 * @param size Initial number of indicies to allocate.
                 */
                stream_map(const T& defaultVal = T(), const int& size = 2);
                
                ~stream_map();

                /**
                 * THREADSAFE
                 * Insert an element in the map and data.
                 * @param stream Stream used as unique key.
                 * @param t Value to store.
                 * @return -1 if stream already exists. Otherwise, index value is stored at.
                 */
                int addObject(const STREAM_ID& stream, const T& t);

                /**
                 * THREADSAFE
                 * Remove element at an index. Does nothing if index is out of bounds or nothing is stored at index.
                 * @param index Index of element to remove.
                 */
                void rmObjectIndex(int index);

                /**
                 * THREADSAFE
                 * Remove element using stream as key. Does nothing if key does not exist.
                 * @param stream Key of element to remove.
                 */
                void rmObjectID(const STREAM_ID& stream);

                /**
                 * Gets string at index. Returns empty if index is out of bounds or nothing is stored at index.
                 * @param index Index to get string.
                 * @return Empty string if index is out of bounds or nothing is stored at index.
                 * @return Streamid at index.
                 */ 
                STREAM_ID getStream(int index) const;

                /**
                 * THREADSAFE
                 * Gets index to reference stream data quickly.
                 * @param stream Stream used as unique key.
                 * @return -1 if stream does not exist.
                 * @return Index to get stream data.
                 */
                int getStreamRedirect(const STREAM_ID& stream) const;

                /**
                 * THREADSAFE
                 * Gets all streams in the map at the moment it is called.
                 * @return Vector of strings, each one being a unique key in the map.
                 */
                std::vector<STREAM_ID> listStreams() const;

                /**
                 * Gets value at index.
                 * @param index index to get element.
                 * @return default value if index out of bounds or invalid.
                 * @return Element at index.
                 */
                const T& operator[](int index);

                /**
                 * Gets value at index. No safety checking and allow modification.
                 * @param index index to get element.
                 * @return Element at index.
                 */
                T& at(int index);

                /**
                 * Gets the reference number of the next available slot. No safety checking.
                 * @return Next available reference value.
                 */
                int nextSlot() const;
            };

        }
    }
}

// Implementation
namespace CorelinkDLL {
    namespace Object {
        namespace Generic {
            template<class T>
            stream_map<T>::stream_map(const T& defaultVal, const int& size):
                    defaultVal(defaultVal) {
                cap = size;
                if (cap < 1) { cap = 1; }
                data = new T[cap];
                streams = new STREAM_ID[cap];
                slots = std::priority_queue<int, std::vector<int>, std::greater<int>>();
                redirect = std::unordered_map<STREAM_ID, int>();

                for (int i = 0; i < cap; ++i) {
                    data[i] = defaultVal;
                    streams[i] = STREAM_DEF;
                    slots.emplace(i);
                }
            }

            template<class T>
            stream_map<T>::~stream_map(){
                delete[] streams;
                streams = nullptr;
                delete[] data;
                data = nullptr;
            }

            template<class T>
            int stream_map<T>::addObject(const STREAM_ID& stream, const T& t){
                std::lock_guard<std::mutex> lock(m);
                //don't accept duplicate streams
                if (redirect.find(stream) != redirect.end()) { return -1; }
                //resize
                if (slots.empty()) {
                    T* newData = new T[cap * 2];
                    STREAM_ID* newStreams = new STREAM_ID[cap * 2];
                    for (int i = 0; i < cap; ++i) {
                        newData[i] = std::move(data[i]);
                        newData[i + cap] = defaultVal;
                        newStreams[i] = std::move(streams[i]);
                        newStreams[i + cap] = STREAM_DEF;
                        slots.emplace(i + cap);
                    }
                    delete[] data;
                    delete[] streams;
                    data = newData;
                    streams = newStreams;
                    newData = nullptr;
                    newStreams = nullptr;

                    cap *= 2;
                }
                int slot = slots.top();
                slots.pop();
                data[slot] = t;
                streams[slot] = stream;
                redirect[stream] = slot;
                return slot;
            }

            template<class T>
            inline void stream_map<T>::rmObjectIndex(int index){
                std::lock_guard<std::mutex> lock(m);
                if (index < 0 || index >= cap || streams[index] == STREAM_DEF) { return; }
                slots.emplace(index);
                redirect.erase(streams[index]);
                data[index] = defaultVal;
                streams[index] = STREAM_DEF;
            }

            template<class T>
            inline void stream_map<T>::rmObjectID(const STREAM_ID & stream) {
                std::lock_guard<std::mutex> lock(m);
                std::unordered_map<STREAM_ID, int>::const_iterator iter;
                if ((iter = redirect.find(stream)) == redirect.end()) { return; }
                slots.emplace(iter->second);
                streams[iter->second] = STREAM_DEF;
                data[iter->second] = defaultVal;
                redirect.erase(iter);
            }

            template<class T>
            inline STREAM_ID stream_map<T>::getStream(int index) const {
                if (index < 0 || index >= cap) {return STREAM_DEF;}
                return streams[index];
            }

            template<class T>
            inline int stream_map<T>::getStreamRedirect(const STREAM_ID& stream) const{
                std::lock_guard<std::mutex> lock(m);
                std::unordered_map<STREAM_ID, int>::const_iterator iter;
                return (iter = redirect.find(stream)) == redirect.end() ? -1 : iter->second;
            }

            template<class T>
            inline std::vector<STREAM_ID> stream_map<T>::listStreams() const{
                std::lock_guard<std::mutex> lock(m);
                std::vector<STREAM_ID> output;
                std::unordered_map<STREAM_ID, int>::const_iterator iter;
                output = std::vector<STREAM_ID>();
                output.reserve(redirect.size());
                for (iter = redirect.begin(); iter != redirect.end(); ++iter) { output.push_back(iter->first); }
                return output;
            }

            template<class T>
            inline const T& stream_map<T>::operator[](int index) {
                if (index < 0 || index >= cap) { return defaultVal; }
                return data[index];
            }

            template<class T>
            inline T& stream_map<T>::at(int index) {
                return data[index];
            }

            template<class T>
            inline int stream_map<T>::nextSlot() const {
                return slots.empty() ? cap : slots.top();
            }
        }
    }
}
#endif