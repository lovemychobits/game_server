#include "DsMsgChainer.h"
#include "../../network/IConnection.h"
#include "../../network/MsgBuffer.h"

ClientMsgDecoder::~ClientMsgDecoder()
{

}

boost::shared_ptr<MsgBuffer> ClientMsgDecoder::Decode(IConnection* pConn, const void* pMsg, uint32_t uLen)
{
	uint32_t* pMsgSize = (uint32_t*)pMsg;
	if (*pMsgSize < 12)
	{
		boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer());
		return msgBuffer;
	}
	uint32_t* pMagicCode = (uint32_t*)((char*)pMsg + 8);
	if (*pMagicCode != 0xA1B2C3D4)
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