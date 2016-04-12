#include "GsHandler.h"
#include "PlayerDataMng.h"
#include "header.h"
#include "DataServer.h"
#include "DataThread.h"
#include "GsConnectionMng.h"


GsHandler::GsHandler(DataServer* pDataServer) : m_pDataServer(pDataServer)
{

}

void GsHandler::HandleConnect(IConnection* pConn)
{
	cout << "gs connect:" << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort() << endl;
	TRACELOG("gs connect:" << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort());
	gpGsConnMng->SetGsConnection(pConn);
}

void GsHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{
	cout << "gs " << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort() << " disconnect" << endl;
	TRACELOG("gs " << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort() << " disconnect");
	gpGsConnMng->SetGsConnection(NULL);
}

void GsHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred) 
{
	if (error)
	{
		cout << "error:" << boost::system::system_error(error).what() << endl;
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
