#include "GameServerHandler.h"
#include "../header.h"
#include "../LobbyServer.h"
#include "../gsgroup/GameGroupMng.h"
#include "../../utils/Utility.h"
#include "../../protocol/slither_server.pb.h"
#include "../playermng/PlayerStatisticsMgr.h"

#include <google/protobuf/text_format.h>
#include "../playermng/ServerService.h"


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
	switch (htons(pMsgHeader->uMsgCmd))
	{
	case slither::REQ_REGISTER_GAMESERVER:				// 请求注册gameserver
	{
		registerGameServer(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_REGISTER_GAMEROOM:
	{
		registerGameRoom(pConn, pBuf, nLen);
	}
	break;
	case slither::NTY_GAMEROOM_END:						// 游戏房间结束接受GameServer信息
	{
		savePlayerStatisticsData(pBuf, nLen);
	}
	break;
	case slither::BULLETIN_FROM_HTTP_SERVER:			// 
	{
		receiveHttpBulletin(pConn,pBuf, nLen);
	}
		break;
	case slither::DEL_BULLETIN_FROM_HTTP_SERVER:			// 
	{
		delHttpBulletin(pConn, pBuf, nLen);
	}
		break;
	case slither::NTY_PLAYER_ENTER_GAME:
	{
		notifyPlayerEnterRoom(pBuf, nLen);
	}
		break;
	default:
		break;
	}
}

void GameServerHandler::savePlayerStatisticsData(const char* pBuf, uint32_t nLen) {
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::GameRoomEndNty gameRoomEndNty;
	gameRoomEndNty.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	string strPrint;
	google::protobuf::TextFormat::PrintToString(gameRoomEndNty, &strPrint);

	cout << "savePlayerStatisticsData REP proto:" << endl << strPrint << endl;
	TRACELOG("savePlayerStatisticsData proto:[\n" << strPrint.c_str() << "\n");


	playerStatisticsMgr->savePlayerStatisticsData(gameRoomEndNty);

	// 通知房间管理器，房间结束了
	gpGameGroupMng->GameRoomEnd(gameRoomEndNty);

	return;
}

void GameServerHandler::receiveHttpBulletin(IConnection* pGsConn, const char* pBuf, uint32_t uLen) {
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::BulletinInfo bulletinInfo;
	bulletinInfo.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	serverService->receiveHttpServerBulletin(pGsConn, bulletinInfo);
	return;
}


void GameServerHandler::delHttpBulletin(IConnection* pGsConn, const char* pBuf, uint32_t uLen) {
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::DelBulletinReq delInfo;
	delInfo.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	serverService->delHttpServerBulletin(pGsConn, delInfo);
	return;
}



void GameServerHandler::registerGameServer(IConnection* pGsConn, const char* pBuf, uint32_t uLen) {
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::RegisterGSReq registerGsReq;
	registerGsReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	const slither::PBGameServer& pbGameInfo = registerGsReq.gsinfo();
	gpGameGroupMng->RegGameServer(pGsConn, pbGameInfo.serverid(), pbGameInfo.ip(), pbGameInfo.port());
	return;
}

void GameServerHandler::registerGameRoom(IConnection* pGsConn, const char* pBuf, uint32_t uLen) 
{
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::RegisterGameRoomReq registerRoomReq;
	registerRoomReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	uint32_t uRoomNum = registerRoomReq.roomlist_size();
	for (uint32_t i = 0; i < uRoomNum; ++i)
	{
		const slither::PBGameRoom& pbGameRoom = registerRoomReq.roomlist(i);
		RoomInfo roomInfo(registerRoomReq.gsid(), pbGameRoom.roomid(), pbGameRoom.curplayernum(), pbGameRoom.lefttime(), pbGameRoom.isstart(), pbGameRoom.totaltime(), pbGameRoom.maxplayernum(), pbGameRoom.refuseentertime());
		gpGameGroupMng->RegGameRoom(registerRoomReq.gsid(), roomInfo);
	}
	
	return;
}

void GameServerHandler::notifyPlayerEnterRoom(const char* pBuf, uint32_t uLen) 
{
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::PlayerEnterRoomNty enterRoomNty;
	enterRoomNty.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	gpGameGroupMng->PlayerEnterRoom(enterRoomNty.playerid(), enterRoomNty.roomid());

	return;
}