/**
 * @file CorelinkRecvData.h
 * A container for data from dll callbacks.
 */
#ifndef CORELINKRECVDATA_H
#define CORELINKRECVDATA_H

#include "CorelinkClasses.h"

namespace Corelink {
    inline RecvData::RecvData(STREAM_ID recvID, STREAM_ID sendID, const char* data, int hdrLen, int msgLen) :
        recvID(recvID), sendID(sendID), data(data), hdrLen(hdrLen), msgLen(msgLen)
    {}

    inline RecvData::RecvData(const RecvData& rhs) :
        recvID(rhs.recvID), sendID(rhs.sendID), data(rhs.data), hdrLen(rhs.hdrLen), msgLen(rhs.msgLen)
    {}

    inline RecvData::~RecvData() {
        data = nullptr;
    }

    inline RecvData& RecvData::operator=(const RecvData& rhs) {
        this->recvID = rhs.recvID;
        this->sendID = rhs.sendID;
        this->data = rhs.data;
        this->hdrLen = rhs.hdrLen;
        this->msgLen = rhs.msgLen;
        return *this;
    }
}

#endif