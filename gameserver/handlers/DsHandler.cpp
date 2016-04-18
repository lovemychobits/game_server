#include "../header.h"
#include "DsHandler.h"
#include "../../utils/Utility.h"


void DsHandler::HandleConnect(IConnection* pConnection)
{
	cout << "connect datasever " << pConnection->GetRemoteIp() << " success" << endl;
	TRACELOG("connect datasever " << pConnection->GetRemoteIp() << " success");

	return;
}

void DsHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{
	cout << "dataserver disconnect" << endl;
	TRACELOG("dataserver " << pConn->GetRemoteIp() << " disconnect");
	if (error)
	{
		ERRORLOG("handle disconnect error, error=[" << boost::system::system_error(error).what() << "]");
	}
}

void DsHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred) 
{
	if (error)
	{
		TRACELOG("error:" << boost::system::system_error(error).what() << ", bytes_transferred=[" << bytes_transferred << "]");
	}
}

void DsHandler::HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen)
{
	if (!pBuf || !uLen)
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
		break;
	default:   
		break;
	}
}
