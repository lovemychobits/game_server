#include "GsHandler.h"
#include "../mysql/PlayerDataMng.h"
#include "../header.h"
#include "../DataServer.h"
#include "../mysql/DataThread.h"
#include "../gsmng/GsConnectionMng.h"


GsHandler::GsHandler(DataServer* pDataServer) : m_pDataServer(pDataServer)
{

}

void GsHandler::HandleConnect(IConnection* pConn)
{
	TRACELOG("gs connect:" << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort());
	gpGsConnMng->SetGsConnection(pConn);
}

void GsHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{
	TRACELOG("gs " << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort() << " disconnect");
	gpGsConnMng->SetGsConnection(NULL);
}

void GsHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred) 
{
	if (error)
	{
		ERRORLOG("error:" << boost::system::system_error(error).what());
	}
}

void GsHandler::HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen)
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
