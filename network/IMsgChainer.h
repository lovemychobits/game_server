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
		virtual boost::shared_ptr<MsgBuffer> Decode(IConnection* pConn, const void* pMsg, uint32_t uLen) = 0;					// ������
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

		virtual boost::shared_ptr<MsgBuffer> Encode(IConnection* pConn, const void* pMsg, uint32_t uLen) = 0;						// д����
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

	// ��Ϣ�������ӿ�
	class IMsgChainer
	{
	public:
		virtual void AddDecodeLast(IMsgDecoder* pMsgDecoder) = 0;												// �����Ϣ��������
		virtual void AddEncodeLast(IMsgEncoder* pMsgEncoder) = 0;												// �����Ϣ��װ����

		virtual boost::shared_ptr<MsgBuffer> Decode(IConnection* pConn, const char* pData, uint32_t ulen) = 0;			// ������Ϣ		
		virtual boost::shared_ptr<MsgBuffer> Encode(IConnection* pConn, const char* pData, uint32_t uLen) = 0;			// ��װ��Ϣ
	};

}

#endif