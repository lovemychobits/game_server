#include "LobbyHandler.h"
#include "../config/ServerConfig.h"
#include "../header.h"
#include "../../utils/Utility.h"

void LobbyHandler::HandleConnect(IConnection* pConnection)
{
	cout << "connect LobbyServer " << pConnection->GetRemoteIp() << " success" << endl;
	TRACELOG("connect LobbyServer success");
	RegGameServer(pConnection);
}

void LobbyHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{
	cout << "LobbyServer " << pConn->GetRemoteIp() << " disconnect " << endl;
	TRACELOG("LobbyServer disconnect ");
	if (error)
	{
		ERRORLOG("handle disconnect error, error=[" << boost::system::system_error(error).what() << "]");
	}
}

void LobbyHandler::HandleWrite(const boost::system::error_code& error, size_t uBytesTransferred) 
{
	if (error)
	{
		ERRORLOG("error:" << boost::system::system_error(error).what() << ", uBytesTransferred=[" << uBytesTransferred << "]");
	}
}

void LobbyHandler::HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen)
{
	if (!pConn || !pBuf || !uLen)
	{
		return;
	}
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	if (!pMsgHeader)
	{
		return;
	}
	switch (pMsgHeader->uMsgCmd)
	{
	case 0:
		{
		}
		break;
	default:
		break;
	}
}

void LobbyHandler::RegGameServer(IConnection* pConnection)
{
}
 