#include "LobbyHandler.h"
#include "../config/ServerConfig.h"
#include "../header.h"
#include "../../utils/Utility.h"
#include "../../protocol/slither_server.pb.h"
#include "../slither/GameRoom.h"

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
	switch (ntohs(pMsgHeader->uMsgCmd))
	{
	case slither::REQ_REGISTER_GAMESERVER:
		{
			slither::RegisterGSAck registerGsAck;
			const char* pBuf = (const char*)pMsgHeader;
			registerGsAck.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

			if (registerGsAck.errorcode() != slither::SERVER_SUCCESS) {
				ERRORLOG("register gs id=[" << gpServerConfig->GetServerId() << "] failed");
				return;
			}

			// 注册房间
			RegGameRooms();
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
 
void LobbyHandler::RegGameRooms() 
{
	if (!g_pLobbySession) {
		return;
	}
	const map<uint32_t, slither::GameRoom*>& roomList = slither::gpGameRoomMng->GetRooms();
	slither::RegisterGameRoomReq regGameRoomReq;
	regGameRoomReq.set_gsid(gpServerConfig->GetServerId());

	map<uint32_t, slither::GameRoom*>::const_iterator roomIt = roomList.begin();
	map<uint32_t, slither::GameRoom*>::const_iterator roomItEnd = roomList.end();
	for (; roomIt != roomItEnd; roomIt++) {
		slither::GameRoom* pGameRoom = roomIt->second;
		if (!pGameRoom) {
			continue;
		}

		slither::PBGameRoom* pPBGameRoom = regGameRoomReq.add_roomlist();
		pPBGameRoom->set_roomid(pGameRoom->GetRoomId());
		pPBGameRoom->set_curplayernum(pGameRoom->GetCurPlayerNum());
		pPBGameRoom->set_lefttime(pGameRoom->GetLeftTime());
		pPBGameRoom->set_isstart(pGameRoom->HasStart());
		pPBGameRoom->set_totaltime(pGameRoom->GetTotalTime());
		pPBGameRoom->set_maxplayernum(pGameRoom->GetMaxPlayer());
		pPBGameRoom->set_refuseentertime(pGameRoom->GetRefuseEnterTime());
	}

	string strResponse;
	cputil::BuildResponseProto(regGameRoomReq, strResponse, slither::REQ_REGISTER_GAMEROOM);
	g_pLobbySession->SendMsg(strResponse.c_str(), strResponse.size());
}