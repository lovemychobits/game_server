#ifndef NETWORK_ICONNECTION_H
#define NETWORK_ICONNECTION_H

#include "header.h"
#include "IMsgParser.h"
#include "IMsgHandler.h"
#include "IMsgChainer.h"

namespace cpnet
{
	class IConnection
	{
	public:
		virtual const char* GetRemoteIp() = 0;										// 获取连接对方的IP
		virtual uint32_t GetRemotePort() = 0;										// 获取连接对方的Port
		virtual bool IsConnected() = 0;												// 是否已经建立连接
		virtual void Shutdown() = 0;												// 关闭连接
		
		virtual void SetMsgChainer(IMsgChainer* pMsgChainer) = 0;					// 设置连接的消息处理链
		virtual void SetMsgHandler(IMsgHandler* pMsgHandler) = 0;					// 设置消息的处理对象
		virtual void SendMsg(const void* pData, uint32_t nLen) = 0;					// 发送消息
	};
}

#endif