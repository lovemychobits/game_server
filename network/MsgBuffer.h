#ifndef NETWORK_MSGBUFFER_H
#define NETWORK_MSGBUFFER_H


#include "header.h"

namespace cpnet
{
	/*
		������װ��Ϣ�ڶ����Ϣ������֮��Ĵ��ݣ����Լ���ʹ������string���µĶ���ڴ濽������
		PS������ֻ�����ڴ��ָ�룬��������ڴ濪�٣����������Ҫ�����������ͷ��ڴ棬�����ڹ�������ǽ��б�ʶ
	*/
	class MsgBuffer
	{
	public:
		MsgBuffer() : m_bAutoDel(false), m_pBuf(NULL), m_uLen(0)
		{

		}
		MsgBuffer(const char* pBuf, uint32_t uLen, bool bAutoDel = false)
		{
			m_pBuf = pBuf;
			m_uLen = uLen;
			m_bAutoDel = bAutoDel;
		}

		~MsgBuffer()
		{
			if (m_pBuf && m_bAutoDel)
			{
				delete m_pBuf;
			}
		}

		inline const char* GetBuf()
		{
			return m_pBuf;
		}

		inline uint32_t GetLen()
		{
			return m_uLen;
		}

	private:
		bool m_bAutoDel;				// �Ƿ���MsgBuffer����ʱɾ���ڴ�ռ�
		const char* m_pBuf;				// ָ��buffer���ڴ�ռ�
		uint32_t m_uLen;				// buffer����
	};
}

#endif