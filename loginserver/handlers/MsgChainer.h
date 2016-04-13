#ifndef LOGINSERVER_DSMSGCHAINER_H
#define LOGINSERVER_DSMSGCHAINER_H

#include "../header.h"
#include "../../network/IMsgChainer.h"

class ClientMsgDecoder : public IMsgDecoder
{
public:
	virtual ~ClientMsgDecoder();
	virtual boost::shared_ptr<MsgBuffer> Decode(IConnection* pConn, const void* pMsg, uint32_t uLen);					// ������
};

class ClientMsgEncoder : public IMsgEncoder
{
public:
	virtual ~ClientMsgEncoder();
	virtual boost::shared_ptr<MsgBuffer> Encode(IConnection* pConn, const void* pMsg, uint32_t uLen);					// д����
};


#endif