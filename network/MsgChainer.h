#ifndef NETWORK_MSGHANDLERCHAIN_H
#define NETWORK_MSGHANDLERCHAIN_H

#include "header.h"
#include "IMsgChainer.h"
#include <list>
using namespace std;

namespace cpnet
{
	/*
		��Ϣ����������������ĳ����Ϣ�ӽ��յ���������У���Ϣ�Ĵ�������
	*/
	class MsgChainer : public IMsgChainer
	{
	public:
		MsgChainer();
		~MsgChainer();

		virtual void AddDecodeLast(IMsgDecoder* pMsgDecoder);			// ���һ����Ϣ����������
		virtual void AddEncodeLast(IMsgEncoder* pMsgEncoder);			// ���һ����Ϣ��װ������

		virtual boost::shared_ptr<MsgBuffer> Decode(IConnection* pConn, const char* pData, uint32_t ulen);
		virtual boost::shared_ptr<MsgBuffer> Encode(IConnection* pConn, const char* pData, uint32_t uLen);

	private:
		IMsgDecoder* m_pMsgDecodeHeader;							// ��Ϣ����������
		IMsgEncoder* m_pMsgEncodeHeader;							// ��Ϣ��װ������	
	};
}


#endif