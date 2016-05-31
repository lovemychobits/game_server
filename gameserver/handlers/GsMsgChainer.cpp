#include "GsMsgChainer.h"
#include "../../network/IConnection.h"
#include "../../network/MsgBuffer.h"

ClientCryptDecoder::~ClientCryptDecoder()
{
}

boost::shared_ptr<MsgBuffer> ClientCryptDecoder::Decode(IConnection* pConn, const void* pMsg, uint32_t uLen)
{
	if (!pConn)
	{
		ERRORLOG("Connection is NULL");
		boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer());
		return msgBuffer;
	}
	boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer((const char*)pMsg, uLen));
	return msgBuffer;
}

ClientMsgDecoder::~ClientMsgDecoder()
{

}

boost::shared_ptr<MsgBuffer> ClientMsgDecoder::Decode(IConnection* pConn, const void* pMsg, uint32_t uLen)
{
	if (!pConn)
	{
		ERRORLOG("Connection is NULL");
		boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer());
		return msgBuffer;
	}

	uint32_t* pMsgSize = (uint32_t*)pMsg;
	if (*pMsgSize < 4)
	{
		boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer());
		return msgBuffer;
	}

	boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer((const char*)pMsg, uLen));
	return msgBuffer;
}

ClientMsgEncoder::~ClientMsgEncoder()
{

}

boost::shared_ptr<MsgBuffer> ClientMsgEncoder::Encode(IConnection* pConn, const void* pMsg, uint32_t uLen)
{
	boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer((const char*)pMsg, uLen));
	if (!pConn)
	{
		return msgBuffer;
	}
	return msgBuffer;
}