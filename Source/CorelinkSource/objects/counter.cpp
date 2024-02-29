#include "corelink/objects/counter.h"

namespace CorelinkDLL {
    namespace Object {
        counter::counter() : val(START) {}
        counter::~counter() {}

        unsigned int counter::get() {
            unsigned int tmp;
            std::lock_guard<std::mutex> lck(lock);
            tmp = val;
            val = val == MAX ? START : val + 1;
            return tmp;
        }
    }
}