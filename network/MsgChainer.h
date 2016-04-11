#ifndef NETWORK_MSGHANDLERCHAIN_H
#define NETWORK_MSGHANDLERCHAIN_H

#include "header.h"
#include "IMsgChainer.h"
#include <list>
using namespace std;

namespace cpnet
{
	/*
		消息处理链，用来处理某条消息从接收到处理过程中，消息的处理流程
	*/
	class MsgChainer : public IMsgChainer
	{
	public:
		MsgChainer();
		~MsgChainer();

		virtual void AddDecodeLast(IMsgDecoder* pMsgDecoder);			// 添加一个消息解析处理链
		virtual void AddEncodeLast(IMsgEncoder* pMsgEncoder);			// 添加一个消息封装处理链

		virtual boost::shared_ptr<MsgBuffer> Decode(IConnection* pConn, const char* pData, uint32_t ulen);
		virtual boost::shared_ptr<MsgBuffer> Encode(IConnection* pConn, const char* pData, uint32_t uLen);

	private:
		IMsgDecoder* m_pMsgDecodeHeader;							// 消息解析处理链
		IMsgEncoder* m_pMsgEncodeHeader;							// 消息封装处理链	
	};
}


#endif