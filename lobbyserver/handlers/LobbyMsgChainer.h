#ifndef LOBBYSERVER_LOBBYMSGCHAINER_H
#define LOBBYSERVER_LOBBYMSGCHAINER_H

#include "../header.h"
#include "../../network/IMsgChainer.h"
using namespace cpnet;

class ClientMsgDecoder : public IMsgDecoder
{
public:
	virtual boost::shared_ptr<MsgBuffer> Decode(IConnection* pConn, const void* pMsg, uint32_t uLen);					// 读数据
};

class ClientMsgEncoder : public IMsgEncoder
{
public:
	virtual boost::shared_ptr<MsgBuffer> Encode(IConnection* pConn, const void* pMsg, uint32_t uLen);					// 写数据
};


#endif