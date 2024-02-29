#include "corelink/objects/initialization_data.h"

namespace CorelinkDLL {
    namespace Object {
        initialization_data::initialization_data() {
            clientInit = false;
            initState = STREAM_STATE_ALL;
            certClientFileName = "ca-crt.pem";
            certServerFileName = "ca-crt-default.pem";
            username = "";
            password = "";
            onDropHandler = nullptr;
            onStaleHandler = nullptr;
            onSubscribeHandler = nullptr;
            onUpdateHandler = nullptr;
        }

        initialization_data::~initialization_data() {}

        initialization_data::initialization_data(const initialization_data& rhs) :
            clientInit(rhs.clientInit), initState(rhs.initState), certClientFileName(rhs.certClientFileName),
            certServerFileName(rhs.certServerFileName), username(rhs.username), password(rhs.password),
            onDropHandler(rhs.onDropHandler), onStaleHandler(rhs.onStaleHandler),
            onSubscribeHandler(rhs.onSubscribeHandler), onUpdateHandler(rhs.onUpdateHandler)
        {}

        initialization_data& initialization_data::operator=(const initialization_data& rhs) {
            clientInit = rhs.clientInit;
            initState = rhs.initState;
            certClientFileName = rhs.certClientFileName;
            certServerFileName = rhs.certServerFileName;
            username = rhs.username;
            password = rhs.password;
            onDropHandler = rhs.onDropHandler;
            onStaleHandler = rhs.onStaleHandler;
            onSubscribeHandler = rhs.onSubscribeHandler;
            onUpdateHandler = rhs.onUpdateHandler;
            return *this;
        }
    }
}