#include "MsgChainer.h"
#include "MsgBuffer.h"
#include "IConnection.h"

namespace cpnet
{
	MsgChainer::MsgChainer() : m_pMsgDecodeHeader(NULL), m_pMsgEncodeHeader(NULL)
	{

	}
	MsgChainer::~MsgChainer()
	{

	}

	// 增加消息解析链
	void MsgChainer::AddDecodeLast(IMsgDecoder* pMsgDecoder)
	{
		if (!m_pMsgDecodeHeader)
		{
			m_pMsgDecodeHeader = pMsgDecoder;
			return;
		}

		IMsgDecoder* pDecoder = m_pMsgDecodeHeader;
		while (pDecoder->GetNext())
		{
			pDecoder = pMsgDecoder->GetNext();
		}
		pDecoder->SetNext(pMsgDecoder);
		return;
	}

	// 增加消息封装链
	void MsgChainer::AddEncodeLast(IMsgEncoder* pMsgEncoder)
	{
		if (!m_pMsgEncodeHeader)
		{
			m_pMsgEncodeHeader = pMsgEncoder;
			return;
		}

		IMsgEncoder* pEncoder = m_pMsgEncodeHeader;
		while (pEncoder->GetNext())
		{
			pEncoder = pEncoder->GetNext();
		}
		pEncoder->SetNext(pMsgEncoder);
		return;
	}

	boost::shared_ptr<MsgBuffer> MsgChainer::Decode(IConnection* pConn, const char* pData, uint32_t uLen)
	{
		boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer(pData, uLen));
		if (!m_pMsgDecodeHeader)
		{
			//ERROR_NET("decode chainer is NULL");
			return msgBuffer;
		}
		
		IMsgDecoder* pMsgDecoder = m_pMsgDecodeHeader;
		while (pMsgDecoder)
		{
			msgBuffer = pMsgDecoder->Decode(pConn, msgBuffer->GetBuf(), msgBuffer->GetLen());
			pMsgDecoder = pMsgDecoder->GetNext();
		}

		return msgBuffer;
	}

	boost::shared_ptr<MsgBuffer> MsgChainer::Encode(IConnection* pConn, const char* pData, uint32_t uLen)
	{
		boost::shared_ptr<MsgBuffer> msgBuffer(new MsgBuffer(pData, uLen));
		if (!m_pMsgEncodeHeader)
		{
			//ERROR_NET("encode chainer is NULL");
			return msgBuffer;
		}

		IMsgEncoder* pMsgEncoder = m_pMsgEncodeHeader;
		while (pMsgEncoder)
		{
			msgBuffer = pMsgEncoder->Encode(pConn, msgBuffer->GetBuf(), msgBuffer->GetLen());
			pMsgEncoder = pMsgEncoder->GetNext();
		}

		return msgBuffer;
	}
}
