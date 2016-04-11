#ifndef NETWORK_MSGBUFFER_H
#define NETWORK_MSGBUFFER_H


#include "header.h"

namespace cpnet
{
	/*
		用来封装消息在多个消息处理链之间的传递，可以减少使用类似string导致的多次内存拷贝消耗
		PS：此类只保存内存的指针，不会进行内存开辟，不过如果需要由它来主动释放内存，可以在构造对象是进行标识
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
		bool m_bAutoDel;				// 是否在MsgBuffer析构时删除内存空间
		const char* m_pBuf;				// 指向buffer的内存空间
		uint32_t m_uLen;				// buffer长度
	};
}

#endif