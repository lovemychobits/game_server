#include "ClientHandler.h"
#include "../header.h"
#include "../LoginServer.h"

ClientHandler::ClientHandler(LoginServer* pLoginServer) : m_pLoginServer(pLoginServer)
{

}

void ClientHandler::HandleConnect(IConnection* pConn)
{
	TRACELOG("client connect:" << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort());
}

void ClientHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{
	TRACELOG("client " << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort() << " disconnect");
}

void ClientHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		ERRORLOG("error:" << boost::system::system_error(error).what());
	}
}

void ClientHandler::HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen)
{
	if (!pBuf || !uLen)
	{
		return;
	}

	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	switch (pMsgHeader->uMsgCmd)
	{
		// TODO
	case 0:
		break;
	default:
		break;
	}
}
