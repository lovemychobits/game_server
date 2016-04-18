#ifndef GAMESERVER_MSGCHAINER_H
#define GAMESERVER_MSGCHAINER_H

#include "../header.h"
#include "../../network/IMsgChainer.h"
using namespace cpnet;

class ClientCryptDecoder : public IMsgDecoder
{
public:
	virtual ~ClientCryptDecoder();
	virtual boost::shared_ptr<MsgBuffer> Decode(IConnection* pConn, const void* pMsg, uint32_t uLen);					// 读数据
};

class ClientMsgDecoder : public IMsgDecoder
{
public:
	virtual ~ClientMsgDecoder();
	virtual boost::shared_ptr<MsgBuffer> Decode(IConnection* pConn, const void* pMsg, uint32_t uLen);					// 读数据
};

class ClientMsgEncoder : public IMsgEncoder
{
public:
	virtual ~ClientMsgEncoder();
	virtual boost::shared_ptr<MsgBuffer> Encode(IConnection* pConn, const void* pMsg, uint32_t uLen);					// 写数据
};

#endif