/**
 * @file CorelinkClasses.h
 * @brief Defines the classes in the wrapper and their relations.
 */
#ifndef CORELINKCLASSES_H
#define CORELINKCLASSES_H
#pragma once

#include "CorelinkConst.h"
#include <stdexcept>

namespace Corelink {
    class CorelinkException;
    class DLLInit;
    class Client;
    class StreamData;
    class SendStream;
    class RecvStream;
    class Callback;
    class RecvData;
}

namespace Corelink {
    class CorelinkException : public std::runtime_error {
        friend class DLLInit;
        friend class Client;
        friend class StreamData;
        friend class SendStream;
        friend class RecvStream;
    private:
        CorelinkException(const std::string& msg, const int& code);

        /**
         * Gets the error message for the corresponding error ID.
         * @param errorID id returned by call to the dll.
         * @exception Throws a CorelinkException for the errorID if errorID != 0.
         */
        static void GetDLLException(const unsigned int errorID);
    public:
        ~CorelinkException();
        CorelinkException(const CorelinkException& rhs);
        CorelinkException& operator=(const CorelinkException& rhs) = delete;

        /// Message string from dll.
        const std::string msg;
        /// Error code from dll. Use with Corelink::Const::ErrorCodeString to make into a readable string.
        const int code;
    };
}

namespace Corelink {
    class DLLInit {
    private:
        DLLInit() = delete;
        ~DLLInit() = delete;
    public:
        /**
         * Initializes DLL.
         */
        static void Init();

        /**
         * Gets the state for components the client will initialize with.
         * @return Currently assigned state value.
         */
        static int getInitState();

        /**
         * Sets the state for components the client will initialize with.
         * @param state State determining which components to use.
         * @exception ERROR_CODE_VALUE if state value is invalid.
         */
        static void setInitState(int state);

        /**
         * Gets the certificate path for the local server.
         * @return Local certificate path.
         */
        static std::string getLocalCertPath();

        /**
         * Sets the local server certificate path.
         * @param path New path value.
         * @param len Length of the path to copy.
         */
        static void setLocalCertPath(const char* path, int len);

        /**
         * Sets the local server certificate path.
         * @param path New path value.
         */
        static void setLocalCertPath(const std::string& path);

        /**
         * Gets the certificate path for the server.
         * @return Server certificate path.
         */
        static std::string getServerCertPath();

        /**
         * Sets the server certificate path.
         * @param path New path value.
         * @param len Length of the path to copy.
         */
        static void setServerCertPath(const char* path, int len);

        /**
         * Sets the server certificate path.
         * @param path New path value.
         */
        static void setServerCertPath(const std::string& path);

        /**
         * Sets the Corelink server login credentials.
         * @param username Corelink username credentials.
         * @param password Corelink password credentials.
         */
        static void setServerCredentials(const std::string& username, const std::string& password);

        /**
         * Sets the callback for the drop function (subscriber listening to client stream destroyed).
         * Assume that the receiver is already discarded from the client and server when called.
         * May be called multiple times consecutively.\n
         * DataOrder:\n
         * -const int& sender streamid.\n
         */
        static void setOnDrop(void(*func)(const STREAM_ID&));

        /**
         * Sets the callback for the stale function.
         * Assume that the stream is already discarded from the client and server when called.
         * May be called multiple times consecutively.\n
         * DataOrder:\n
         * -const int& receiver streamid.\n
         */
        static void setOnStale(void(*func)(const STREAM_ID&));

        /**
         * Sets the callback for the subscribe function (new subscriber listening to client sender).
         * May be called multiple times consecutively if client has multiple senders.\n
         * DataOrder:\n
         * -const int& senderID.\n
         * -const int& receiver stream. May be replaced with StreamData in future update.\n
         */
        static void setOnSubscribe(void(*func)(const STREAM_ID&, const STREAM_ID&));

        /**
         * Sets the callback for the update function (new sender stream that client can listen to).
         * May be called multiple times consecutively if client has multiple receiver.\n
         * DataOrder:\n
         * -const int& receiverID.\n
         * -const int& sender stream. May be replaced with StreamData in future update.\n
         */
        static void setOnUpdate(void(*func)(const STREAM_ID&, const STREAM_ID&));
    };
}

namespace Corelink {
    class Client {
    private:
        Client() = delete;
        ~Client() = delete;

        /**
         * Gets whether client has been initialized or not
         */
        static bool isActive();

        /**
         * Helper function to parse the data from the dll.
         * @param data Array to parse to array of strings.
         * @return Parsed message.
         */
        static std::vector<std::string> getCommResponseDataHelper(char* data);
        
        /**
         * Retrieves the response message for the client and parses it.
         * @param commID ID to retrieve.
         * @return Parsed response message.
         */
        static std::vector<std::string> getCommResponseData(const int& commID);

        /**
         * Helper function to convert vector of strings to const char**.
         * @param vec Vector to convert.
         * @return Resulting const char**.
         */
        static const char** vecStringToCharArray(const std::vector<std::string>& vec);

        /**
         * Parse the retrieved the data from dll into the client's stream format
         */
        static StreamData getDataStreamInfo(int& commID);
    public:
        /**
         * Tries to connect to the server using the provided address.
         * @param serverIP Server IP.
         * @param port Server port.
         * @exception ERROR_CODE_STATE if client is already initialized.
         * @exception ERROR_CODE_SOCKET if socket connection issues occur.
         * @exception ERROR_CODE_COMM if credentials are incorrect.
         */
        static void connect(const std::string& serverIP, int port);

        /**
         * Tries to connect to the server using environment variables provided by kubernetes.
         * @param serverIP Server IP.
         * @param port Server port.
         * @exception ERROR_CODE_STATE if client is already initialized.
         * @exception ERROR_CODE_SOCKET if socket connection issues occur.
         * @exception ERROR_CODE_COMM if credentials are incorrect.
         * @exception ERROR_CODE_VALUE if environment variables not found.
         */
        static void connectPlugin();

        /**
         * Disconnects and cleans up client.
         */
        static void cleanup();

        /**
         * Gets the current token string. (No actual use on client)
         * @return Current token value.
         */
        static std::string getToken();

        /**
         * Gets list of functions available on the server.
         * @return List of functions on server.
         */
        static std::vector<std::string> listServerFunctions();

        /**
         * TODO: Create a wrapper class for the function data.
         * Gets information about the function specified.
         * @param func Function to get information on.
         * @return List of properties of the function.
         */
        static std::vector<std::string> describeServerFunction(const std::string& func);

        /**
         * Gets list of workspaces available on the server.
         * @return List of workspaces on server.
         */
        static std::vector<std::string> listWorkspaces();

        /**
         * Tries to add a workspace to the server.
         * @param workspace Name of workspace to add to server.
         * @return Success of adding workspace.
         */
        static bool addWorkspace(const std::string& workspace);

        /**
         * Tries to remove a workspace from the server.
         * @param workspace Name of workspace to remove from server.
         * @return Success of removing workspace.
         */
        static bool rmWorkspace(const std::string& workspace);

        /**
         * Sends a generic message to the server. (Best approach to use)
         * @param json Rapidjson object to send.
         */
        static rapidjson::Document genericComm(const rapidjson::Document& json);

        /**
         * Sends a generic message to the server.
         * @param json Json string to send to the server.
         */
        static rapidjson::Document genericComm(const std::string& json);

        /**
         * Sends a generic message to the server.
         * @param json Json string to send to the server.
         * @param len Length of message. Default to 0 if string ends with a null terminator.
         */
        static rapidjson::Document genericComm(const char* json, const int& len = 0);

        /**
         * Gets the list of sender streams available on the server.
         * @param workspaces List of workspaces to include in data. Leave empty for all.
         * @param types List of types to include in data. Leave empty for all.
         * @return List of sender streamIDs.
         */
        static std::vector<int> listStreams(const std::vector<std::string>& workspaces = {}, const std::vector<std::string>& types = {});

        /**
         * Gets information on a specific stream in the server.
         * @param streamID ID to obtain information about.
         * @return Streamdata containing server side information on the stream (does not include sources, reference and some other members).
         */
        static StreamData streamInfo(const int& streamID);

        /**
         * Attempts to create a sender stream.
         * @param workspace Workspace being used for stream.
         * @param type Type of information stream is sending.
         * @param meta Metadata attached to stream.
         * @param echo Can client listen to itself.
         * @param alert Recieve server updates for new streams.
         * @param protocol Protocol used by stream.
         * @return Handler to access send functions.
         */
        static SendStream createSender(const std::string& workspace, const std::string& type, const std::string& meta, bool echo, bool alert, int protocol);

        /**
         * Attempts to create a receiver stream.
         * @param workspace workspace being used for stream.
         * @param types types of information stream is listening to.
         * @param typeLen amount of types provided
         * @param meta Metadata attached to stream.
         * @param echo Can client listen to itself.
         * @param alert Recieve server updates for new streams.
         * @param protocol Protocol used by stream.
         */
        static RecvStream createReceiver(const std::string& workspace, const std::vector<std::string>& types, const std::string& meta, bool echo, bool alert, int protocol);

        /**
         * Subcribes a reciever to a new sender.
         * 
         * @param receiver Reciever stream to listen to new sender.
         * @param sender Sender stream to listen to.
         * @return Success or failure upon subscribing to stream.
         */
        static bool subscribe(const STREAM_ID& receiverID, const STREAM_ID& senderID);

        /**
         * Unsubcribes a reciever from a sender.
         * 
         * @param receiver Reciever stream to stop listening to sender.
         * @param sender Sender stream to stop listening to.
         * @return Success or failure upon unsubscribing from stream.
         */
        static bool unsubscribe(const STREAM_ID& receiverID, const STREAM_ID& senderID);

        /**
         * Destroys a receiver associated with this session and cleans it from the server.
         * @param streamID ID of stream to remove from both server and client.
         */
        static bool rmStream(const STREAM_ID& streamID);
    };
}

namespace Corelink {
    class StreamData {
        friend class Client;
        friend class DLLInit;
        friend class Client;
        friend class StreamData;
        friend class SendStream;
        friend class RecvStream;
    private:
        /// Dll internal reference to the stream.
        int streamRef;

        /**
         * Function type that needs to be defined for reciever functions.
         * Takes parameters (receiver, sender, message, messsage length).
         */
        typedef void(*RecvFunc)(const STREAM_ID&, const STREAM_ID&, const char*, const int&);

    public:
        // TODO: move the data to private instead of public and implement getters. 
        // Might not be necessary since this is readonly data from the dll.

        /// ID of the stream seen by the server.
        int streamID;
        /// Stores the type of the stream (UDP, TCP, WS & SEND, RECV).
        int state;
        /// Maximum transmission unit. Max size of packets transferable at a time. Not actually used as of now.
        int mtu;
        /// Username of the creator of the stream.
        std::string user;
        /// Workspace stream belongs to in the server.
        std::string workspace;
        /// Additional data attached to the stream
        std::string meta;
        /// Type of the stream. Sender may only have a single type while Receiver has arbritary amount.
        std::vector<std::string> type;
    private:
        StreamData(const int& streamID, const int& state, const int& mtu, const std::string& user, const std::string& workspace, const std::string& meta, int streamRef, const std::vector<std::string>& type);

    public:
        StreamData();
        ~StreamData();
        StreamData(const StreamData& rhs);
        StreamData& operator=(const StreamData& rhs);
        friend std::ostream& operator<<(std::ostream& os, const StreamData& rhs);

        static StreamData parse(const char* data);

        /**
         * Checks whether this stream is a sender on the client.
         * @returns True if it is a sender and false if it isn't.
         */
        bool isSender() const;

        /**
         * Checks whether a stream is a sender on the client.
         * This does not check for streams on the server.
         * @param streamID Sender stream to check if exists.
         * @returns True if sender exists and false if it doesn't.
         */
        static bool isSender(const STREAM_ID& streamID);

        /**
         * Checks whether this receiver is a sender on the client.
         * @returns True if it is a receiver and false if it isn't.
         */
        bool isReceiver() const;

        /**
         * Gets whether a stream is a receiver on the client.
         * This does not check for streams on the server.
         * @param streamID Receiver stream to check if exists.
         * @returns True if receiver exists and false if it doesn't.
         */
        static bool isReceiver(const STREAM_ID& streamID);

        /**
         * Gets a copy of the data for a stream.
         * @param streamID Client stream to retrieve data from.
         * @return StreamData for the corresponding streamID if it is owned by client.
         * Otherwise it returns a default constructed StreamData (state = STATE_NONE, streamRef = -1).
         */
        static StreamData getStreamData(const STREAM_ID& streamID);

        /**
         * Creates a sender handler to interact with the network.
         * @param streamID Sender stream to create a handler for.
         * @return Handler to use the sender stream if it is owned by client.
         * Otherwise it returns a default constructed SendStream (state = STATE_NONE, streamRef = -1).
         */
        static SendStream getSenderHandler(const STREAM_ID& streamID);

        /**
         * Creates a receiver handler to interact with the network.
         * @param streamID Receiver stream to create a handler for.
         * @return Handler to use the receiver stream if it is owned by client.
         * Otherwise it returns a default constructed RecvStream (state = STATE_NONE, streamRef = -1).
         */
        static RecvStream getReceiverHandler(const STREAM_ID& streamID);

        /**
         * Gets all streams on client.
         * @return Vector of stream ids.
         */
        static std::vector<STREAM_ID> listClientStreams();

        /**
         * Gets all sources for stream on client.
         * @return Vector of stream ids.
         */
        static std::vector<STREAM_ID> listStreamSources(const STREAM_ID& streamID);

    };
}

// TODO: determine which members should be const
namespace Corelink {
    class SendStream {
        friend class StreamData;
    private:
        /// ID of the stream seen by the server.
        STREAM_ID streamID;
        /// Dll internal reference to the stream. May be removed/made invalid on another thread without notifying handler.
        int streamRef;
        /// Stream state.
        int state;
    public:
    private:
        SendStream(const STREAM_ID& streamID = STREAM_DEF, int state = Const::STREAM_STATE_NONE, int streamRef = -1);
    public:
        SendStream(const StreamData& streamData);
        SendStream(const SendStream& rhs);
        SendStream(SendStream&& rhs);
        SendStream& operator=(const SendStream& rhs);

        operator STREAM_ID() const;
    private:
    public:
        /**
         * Use to send data on the stream.
         * Target functionity not implemented though it should be able to specify the reciever.
         * Thread safe.
         * @param msg Data to send to server.
         * @param federationID Server specified to recieve data.
         * @return If stream exists on client.
         */
        bool send(const std::string& msg, const int& federationID = 0);

        /**
         * Use to send data on the stream.
         * Target functionity not implemented though it should be able to specify the reciever.
         * Thread safe.
         * @param msg Data to send to server.
         * @param json Json to attach with the message.
         * @param federationID Server specified to recieve data.
         * @param serverCheck Should server check the json.
         * @return If stream exists on client.
         */
        bool send(const std::string& msg, const rapidjson::Document& json, bool serverCheck = false, const int& federationID = 0);

        /**
         * Use to send data on the stream.
         * Target functionity not implemented though it should be able to specify the reciever.
         * Thread safe.
         * @param msg Data to send to server.
         * @param msgLen Length of data to send.
         * @param federationID Server specified to recieve data.
         * @return If stream exists on client.
         */
        bool send(const char* msg, int msgLen, const int& federationID = 0);

        /**
         * Use to send data on the stream.
         * Target functionity not implemented though it should be able to specify the reciever.
         * Thread safe.
         * @param msg Data to send to server.
         * @param msgLen Length of data to send.
         * @param json Json to attach with the message.
         * @param federationID Server specified to recieve data.
         * @param serverCheck Should server check the json.
         * @return If stream exists on client.
         */
        bool send(const char* msg, int msgLen, const rapidjson::Document& json, bool serverCheck = false, const int& federationID = 0);
    };
}

namespace Corelink {
    class RecvStream {
        friend class StreamData;
    private:
        /// ID of the stream seen by the server.
        STREAM_ID streamID;
        /// Dll internal reference to the stream. May be removed/made invalid on another thread without notifying handler.
        int streamRef;
        /// Stream state.
        int state;
    public:
    private:
        RecvStream(const STREAM_ID& streamID = STREAM_DEF, int state = Const::STREAM_STATE_NONE, int streamRef = -1);
    public:
        RecvStream(const StreamData& streamData);
        RecvStream(const RecvStream& rhs);
        RecvStream(RecvStream&& rhs);
        RecvStream& operator=(const RecvStream& rhs);

        operator STREAM_ID() const;
    private:
    public:
        /**
         * Clears the receive callback.
         */
        void setOnReceive();

        /**
         * Callback with the format:
         * sendID
         * recvID
         * data
         * data length
         */
        void setOnReceive(void(*func)(const STREAM_ID&, const STREAM_ID&, const char*, const int&));

        /**
         * Callback with the format:
         * obj passed by user
         * sendID
         * recvID
         * data
         * data length
         */
        void setOnReceive(void(*func)(void*, const STREAM_ID&, const STREAM_ID&, const char*, const int&), void* obj);

        /**
         * Callback with the format:
         * sendID
         * recvID
         * data
         * data length
         * json object
         */
        void setOnReceive(void(*func)(const STREAM_ID&, const STREAM_ID&, const char*, const int&, const rapidjson::Document&));

        /**
         * Callback with the format:
         * obj passed by user
         * sendID
         * recvID
         * data
         * data length
         * json object
         */
        void setOnReceive(void(*func)(void*, const STREAM_ID&, const STREAM_ID&, const char*, const int&, const rapidjson::Document&), void* obj);

        std::vector<STREAM_ID> listSources();
    };
}

namespace Corelink {
    class Callback {
    public:
        virtual ~Callback() = default;

        virtual void Func(const RecvData& recvData) = 0;

        static void RecvCallback(STREAM_ID recvID, STREAM_ID sendID, const char* msg, int jsonLen, int msgLen, void* callback);
    };

    class CallbackData : public Callback {
    public:
        /**
         * Data order:
         * sendID
         * recvID
         * data
         * data length
         */
        void(*func)(const STREAM_ID&, const STREAM_ID&, const char*, const int&);

        CallbackData(void(*func)(const STREAM_ID&, const STREAM_ID&, const char*, const int&));
        
        ~CallbackData();

        void Func(const RecvData& recvData) override;

        static void RecvCallback(STREAM_ID recvID, STREAM_ID sendID, const char* msg, int jsonLen, int msgLen, void* callback);
    };

    class CallbackDataVoid : public Callback {
    public:
        /**
         * Data order:
         * obj passed by user
         * sendID
         * recvID
         * data
         * data length
         */
        void(*func)(void*, const STREAM_ID&, const STREAM_ID&, const char*, const int&);

        void* obj;

        CallbackDataVoid(void(*func)(void*, const STREAM_ID&, const STREAM_ID&, const char*, const int&), void* obj);
        
        ~CallbackDataVoid();

        void Func(const RecvData& recvData) override;

        static void RecvCallback(STREAM_ID recvID, STREAM_ID sendID, const char* msg, int jsonLen, int msgLen, void* callback);
    };
    
    class CallbackDataJson : public Callback {
    public:
        /**
         * Data order:
         * sendID
         * recvID
         * data
         * data length
         * json object
         */
        void(*func)(const STREAM_ID&, const STREAM_ID&, const char*, const int&, const rapidjson::Document&);

        CallbackDataJson(void(*func)(const STREAM_ID&, const STREAM_ID&, const char*, const int&, const rapidjson::Document&));

        ~CallbackDataJson();

        void Func(const RecvData& recvData) override;

        static void RecvCallback(STREAM_ID recvID, STREAM_ID sendID, const char* msg, int jsonLen, int msgLen, void* callback);
    };

    class CallbackDataJsonVoid : public Callback {
    public:
        /**
         * Data order:
         * obj passed by user
         * sendID
         * recvID
         * data
         * data length
         * json object
         */
        void(*func)(void*, const STREAM_ID&, const STREAM_ID&, const char*, const int&, const rapidjson::Document&);

        void* obj;

        CallbackDataJsonVoid(void(*func)(void*, const STREAM_ID&, const STREAM_ID&, const char*, const int&, const rapidjson::Document&), void* obj);
        
        ~CallbackDataJsonVoid();

        void Func(const RecvData& recvData) override;

        static void RecvCallback(STREAM_ID recvID, STREAM_ID sendID, const char* msg, int jsonLen, int msgLen, void* callback);
    };
}

namespace Corelink {
    class RecvData {
    public:
        RecvData(STREAM_ID recvID, STREAM_ID sendID, const char* data, int hdrLen, int msgLen);
        RecvData(const RecvData& rhs);
        ~RecvData();
        RecvData& operator=(const RecvData& rhs);

        STREAM_ID recvID;
        STREAM_ID sendID;
        const char* data;
        int hdrLen;
        int msgLen;
    };
}
#endif