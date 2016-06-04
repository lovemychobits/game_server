#include "LobbyHandler.h"
#include "../config/ServerConfig.h"
#include "../header.h"
#include "../../utils/Utility.h"
#include "../../protocol/slither_server.pb.h"

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
	slither::RegisterGSReq registerGsReq;
	slither::PBGameServer* pPBGameServer = registerGsReq.mutable_gsinfo();
	pPBGameServer->set_ip(gpServerConfig->GetRegisterIp());
	pPBGameServer->set_port(gpServerConfig->GetGsBindPort());
	pPBGameServer->set_serverid(gpServerConfig->GetServerId());

	string strResponse;
	cputil::BuildResponseProto(registerGsReq, strResponse, slither::REQ_REGISTER_GAMESERVER);
	pConnection->SendMsg(strResponse.c_str(), strResponse.size());
}
 