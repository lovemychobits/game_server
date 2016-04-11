#ifndef NETWORK_IMSGCHAINER_H
#define NETWORK_IMSGCHAINER_H

#include "header.h"

namespace cpnet
{
	class MsgBuffer;
	class IMsgDecoder
	{
	public:
		IMsgDecoder() : m_pNextDecoder(NULL)
		{

		}
		virtual ~IMsgDecoder()
		{

		}
		virtual boost::shared_ptr<MsgBuffer> Decode(IConnection* pConn, const void* pMsg, uint32_t uLen) = 0;					// 读数据
		virtual IMsgDecoder* GetNext()
		{
			return m_pNextDecoder;
		}
		virtual void SetNext(IMsgDecoder* pMsgDecoder)
		{
			m_pNextDecoder = pMsgDecoder;
		}

	private:
		IMsgDecoder* m_pNextDecoder;
	};

	class IMsgEncoder
	{
	public:
		IMsgEncoder() : m_pNextEncoder(NULL)
		{

		}
		virtual ~IMsgEncoder()
		{

		}

		virtual boost::shared_ptr<MsgBuffer> Encode(IConnection* pConn, const void* pMsg, uint32_t uLen) = 0;						// 写数据
		virtual IMsgEncoder* GetNext()
		{
			return m_pNextEncoder;
		}
		virtual void SetNext(IMsgEncoder* pMsgEncoder)
		{
			m_pNextEncoder = pMsgEncoder;
		}

	private:
		IMsgEncoder* m_pNextEncoder;
	};

	// 消息处理链接口
	class IMsgChainer
	{
	public:
		virtual void AddDecodeLast(IMsgDecoder* pMsgDecoder) = 0;												// 添加消息解析对象
		virtual void AddEncodeLast(IMsgEncoder* pMsgEncoder) = 0;												// 添加消息封装对象

		virtual boost::shared_ptr<MsgBuffer> Decode(IConnection* pConn, const char* pData, uint32_t ulen) = 0;			// 解析消息		
		virtual boost::shared_ptr<MsgBuffer> Encode(IConnection* pConn, const char* pData, uint32_t uLen) = 0;			// 封装消息
	};

}

#endif