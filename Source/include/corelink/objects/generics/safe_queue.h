/**
 * @file safe_queue.h
 * @brief A threadsafe-queue copied and modified from stack overflow for threadsafe transfer of data.
 */
#ifndef CORELINK_OBJECTS_GENERICS_SAFEQUEUE_H
#define CORELINK_OBJECTS_GENERICS_SAFEQUEUE_H

#include "corelink/headers/header.h"

namespace CorelinkDLL {
    namespace Object {
        namespace Generic {
            template <class T>
            class safe_queue {
            public:
                safe_queue(): q(), m(), c() {}
                ~safe_queue() {}

                /**
                 * THREADSAFE
                 * Add an element to the queue.
                 * @param t Element of type T to insert into queue.
                 */
                void enqueue(T t) {
                    std::lock_guard<std::mutex> lock(m);
                    q.push(t);
                    c.notify_one();
                }

                /**
                 * THREADSAFE
                 * Gets the front element. If the queue is empty, wait till a element is avaiable.
                 * @return Element of type T from the front of the queue.
                 */
                T dequeue() {
                    std::unique_lock<std::mutex> lock(m);
                    T val;

                    while (q.empty()) {
                        c.wait(lock);
                    }
                    val = q.front();
                    q.pop();
                    return val;
                }

                /**
                 * Gets the front element.
                 * @return Element of type T from the front of the queue.
                 */
                T dequeue_unsafe() {
                    T val;
                    val = q.front();
                    q.pop();
                    return val;
                }

                /**
                 * THREADSAFE
                 * Gets the front element without calling pop.
                 * If the queue is empty, returns def value.
                 * @param val Value the front element will be stored in.
                 * @param def Value returned if queue is empty.
                 * @return Element of type T from the front of the queue or def if queue is empty.
                 */
                void front(T& val, const T& def) {
                    std::lock_guard<std::mutex> lock(m);
                    val = q.empty() ? def : q.front();
                }

                /**
                 * Gets the front element without calling pop.
                 * @param val Value the front element will be stored in.
                 * @return Element of type T from the front of the queue. Undefined behavior if queue is empty.
                 */
                void front_unsafe(T& val) const {
                    val = q.front();
                }

                /**
                 * THREADSAFE
                 * Removes all elements and clears queue.
                 */
                void clear() {
                    size_t size;
                    std::lock_guard<std::mutex> lock(m);

                    size = q.size();
                    for (std::size_t i = 0; i < size; ++i) { q.pop(); }
                }

                /**
                 * THREADSAFE
                 * Removes all elements and clears queue.
                 * Calls delete to prevent memory leaks from pointer types.
                 */
                void clearDelete() {
                    size_t size;
                    std::lock_guard<std::mutex> lock(m);
                    
                    size = q.size();
                    for (int i = 0; i < size; ++i) {
                        delete q.front();
                        q.pop();
                    }
                }

                /**
                 * THREADSAFE
                 * Gets the number of elements in the queue when called.
                 * @return Size of queue.
                 */
                size_t size() {
                    std::lock_guard<std::mutex> lock(m);
                    return q.size();
                }

                /**
                 * Gets the number of elements in the queue when called.
                 * @return Size of queue.
                 */
                size_t size_unsafe() const {
                    return q.size();
                }
            private:
                std::queue<T> q;
                mutable std::mutex m;
                std::condition_variable c;
            };
        }
    }
}

#endif