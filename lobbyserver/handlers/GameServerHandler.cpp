#include "GameServerHandler.h"
#include "../header.h"
#include "../LobbyServer.h"
#include "../gsgroup/GameGroupMng.h"
#include "../../utils/Utility.h"

GameServerHandler::GameServerHandler(LobbyServer* pLobbyServer) : m_pLobbyServer(pLobbyServer)
{

}

GameServerHandler::~GameServerHandler()
{

}

void GameServerHandler::SetGroupMng(GameGroupMng* pGroupMng)
{
	m_pGameGroupMng = pGroupMng;
}

void GameServerHandler::HandleConnect(IConnection* pConn)
{
	cout << "gs connect:" << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort() << endl;
}

void GameServerHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{
	cout << "gs " << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort() << " disconnect" << endl;
	m_pGameGroupMng->GameServerDisconnect(pConn);
}

void GameServerHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred) 
{
	if (error)
	{
		cout << "error" << endl;
		cout << "error:" << boost::system::system_error(error).what() << endl;
	}
}

void GameServerHandler::HandleRecv(IConnection* pConn, const char* pBuf, uint32_t nLen)
{
	if (!pBuf || !nLen)
	{
		return;
	}

	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	switch (pMsgHeader->uMsgCmd)
	{
	case 0:						// 服务器请求注册信息
		{
		}
		break;
	default:
		break;
	}
}
