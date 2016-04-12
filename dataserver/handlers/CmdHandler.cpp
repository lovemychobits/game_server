#include "CmdHandler.h"
#include "../../utils/Utility.h"
#include "../DataServer.h"


void CmdHandler::HandleConnect(IConnection* pConn)
{
	TRACELOG("command client " << pConn->GetRemoteIp() << " connect ");
}

void CmdHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{   
	TRACELOG("command client " << pConn->GetRemoteIp() << " disconnect ");
}

void CmdHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred) 
{
	if (error)
	{
		ERRORLOG("handle write error, error=[" << boost::system::system_error(error).what() << "]");
	}
}

void CmdHandler::HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen)
{
	if (!pBuf || uLen == 0)
	{
		ERRORLOG("message error");
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
