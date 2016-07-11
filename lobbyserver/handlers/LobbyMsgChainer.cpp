#include "LobbyMsgChainer.h"
#include "../../network/IConnection.h"
#include "../../network/MsgBuffer.h"

boost::shared_ptr<MsgBuffer> ClientMsgDecoder::Decode(IConnection* pConn, const void* pMsg, uint32_t uLen)
{
	uint32_t* pMsgSize = (uint32_t*)pMsg;
	if (*pMsgSize < 4)
	{
		ERRORLOG("client msg size=[" << *pMsgSize << "]");
		boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer());
		return msgBuffer;
	}
	
	boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer((const char*)pMsg, uLen));
	return msgBuffer;
}

boost::shared_ptr<MsgBuffer> ClientMsgEncoder::Encode(IConnection* pConn, const void* pMsg, uint32_t uLen)
{
	boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer((const char*)pMsg, uLen));
	return msgBuffer;
}