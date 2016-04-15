#include "ClientMsgHandler.h"
#include "../gsgroup/GameGroupMng.h"

ClientMsgHandler::ClientMsgHandler() 
{

}

void ClientMsgHandler::SetGroupMng(GameGroupMng* pGroupMng)
{
	m_pGameGroupMng = pGroupMng;
}


void ClientMsgHandler::HandleConnect(IConnection* pConn)
{
	TRACELOG("client connect:" << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort());
}

void ClientMsgHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{
	TRACELOG("client " << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort() << " disconnect, error=[" << error.message().c_str() << "]");
}

void ClientMsgHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred) 
{
	if (error)
	{
		ERRORLOG("error:" << boost::system::system_error(error).what());
	}
	TRACELOG("bytes_transferred=[" << bytes_transferred << "]");
}

void ClientMsgHandler::HandleRecv(IConnection* pConn, const char* pBuf, uint32_t nLen)
{
	if (!pBuf || !nLen)
	{
		return;
	}

	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	switch (pMsgHeader->uMsgCmd)
	{
	case 0:				// 请求游戏区组
		{
		}
		break;
	default:
		break;
	}
}