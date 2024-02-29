/**
 * @file counter.h
 * @brief Class that returns a positive unique unsigned int when called.
 */
#ifndef CORELINK_OBJECTS_COUNTER_H
#define CORELINK_OBJECTS_COUNTER_H
#include "corelink/headers/header.h"

namespace CorelinkDLL {
    namespace Object {
        class counter {
        private:
            unsigned int val;
            const static unsigned int START = 1U;
            const static unsigned int MAX = ((unsigned int)-1) / 4;
            mutable std::mutex lock;
        public:
            counter();
            ~counter();

            /**
             * THREADSAFE
             * Gets value and increments.
             * @return unique unsigned int.
             */
            unsigned int get();
        };
    }
}
#endif