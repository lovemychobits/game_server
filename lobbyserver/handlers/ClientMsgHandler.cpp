#include "ClientMsgHandler.h"
#include "../gsgroup/GameGroupMng.h"
#include "../gsgroup/ClientConnMgr.h"
#include "../../protocol/slither_lobby.pb.h"
#include "../playermng/PlayerMng.h"
#include "../playermng/playerMsg.h"
#include "../playermng/ServerService.h"
#include "../gsgroup/TeamMng.h"

ClientMsgHandler::ClientMsgHandler() 
{

}

void ClientMsgHandler::SetGroupMng(GameGroupMng* pGroupMng)
{
	m_pGameGroupMng = pGroupMng;
}


void ClientMsgHandler::HandleConnect(IConnection* pConn)
{
	TRACELOG("client connect:" << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort());
}

void ClientMsgHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{
	clientConnMgr->removeConn(pConn);
	TRACELOG("client " << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort() << " disconnect, error=[" << error.message().c_str() << "]");
}

void ClientMsgHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred) 
{
	if (error)
	{
		ERRORLOG("error:" << boost::system::system_error(error).what());
	}
	TRACELOG("bytes_transferred=[" << bytes_transferred << "]");
}

void ClientMsgHandler::HandleRecv(IConnection* pConn, const char* pBuf, uint32_t nLen)
{
	if (!pConn || !pBuf || !nLen)
	{
		return;
	}

	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	//TODO
	cout << "pMsgHeaderCMD  =" << htons(pMsgHeader->uMsgCmd) << endl;
	switch (htons(pMsgHeader->uMsgCmd)) {
	case slither::REQ_VERIFY_TOKEN:
	{
		verifySDKTokenAndLogin(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_GET_PLAYER_INFO:
	{
		reqGetPlayerInfo(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_SET_SKIN:
	{
		setUseSkin(pConn, pBuf, nLen);
	}
		break;
	//case slither::REQ_UNLOCK_SKIN:
	//{
	//	unlockSkin(pConn, pBuf, nLen);
	//}
	//	break;
	case slither::REQ_MODIFY_PLAYER_INFO:
	{
		modifyUserInfo(pConn, pBuf, nLen);
	}
		break;
	case slither::REQ_VERIFY_NETSPEED:
	{
		netEcho(pConn, pBuf, nLen);
	}
		break;
	case slither::REQ_GET_BULLETIN:
	{
		getBulletins(pConn);
	}
		break;
	case slither::REQ_GET_ENTER_ROOM_ID:
	{
		getEnterRoomId(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_PAY_ATTENTION:
	{
		palyerAttention(pConn, pBuf, nLen);
	}
		break;
	case slither::REQ_CREATE_TEAM:
	{
		createTeam(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_LEAVE_TEAM:
	{
		leaveTeam(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_GET_INVITE_LIST:
	{
		getInviteList(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_INVITE_JOIN_TEAM:
	{
		inviteJoinTeam(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_ACCEPT_JOIN_TEAM:
	{
		acceptJoinTeam(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_REFUSE_JOIN_TEAM:
	{
		refuseJoinTeam(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_KICK_TEAM_MEMBER:
	{
		kickTeamMember(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_START_TEAM_GAME:
	{
		startTeamGame(pConn, pBuf, nLen);
	}
	break;
	case slither::REQ_GET_ATTENTION_LIST:
	{
		getInviteList(pConn, pBuf, nLen);
	}
		break;
	case slither::REQ_GET_FANS_LIST:
	{
		getInviteList(pConn, pBuf, nLen);
	}
		break;

	default:
		break;
	}
}

void ClientMsgHandler::reqGetPlayerInfo(IConnection* pConn, const char* pBuf, uint32_t nLen) {
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::GetPlayerInfoReq getPlayerInfoReq;
	getPlayerInfoReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	playerMsgDeal->GetPlayerData(pConn, getPlayerInfoReq);
	return;
}


void ClientMsgHandler::verifySDKTokenAndLogin(IConnection* pConn, const char* pBuf, uint32_t nLen) {
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::VerifyTokenReq verifyTokenReq;
	verifyTokenReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	playerMsgDeal->dealVerifySDKToken(pConn, verifyTokenReq);
	return;
}


void ClientMsgHandler::setUseSkin(IConnection* pConn, const char* pBuf, uint32_t nLen) {
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::SetUseSkinReq setUseSkinReq;
	setUseSkinReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	playerMsgDeal->playerSetUseSkin(pConn, setUseSkinReq);
	return;
}


//void ClientMsgHandler::unlockSkin(IConnection* pConn, const char* pBuf, uint32_t nLen) {
//	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
//
//	slither::UnLockSkinReq unLockSkinReq;
//	unLockSkinReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));
//
//	playerMsgDeal->playerUnLockSkin(pConn, unLockSkinReq);
//	return;
//}

//////////
void ClientMsgHandler::modifyUserInfo(IConnection* pConn, const char* pBuf, uint32_t nLen) {
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::ModifyPlayerInfoReq modifyPlayerInfoReq;
	modifyPlayerInfoReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	playerMsgDeal->modifyPlayerInfo(pConn, modifyPlayerInfoReq);
	return;
}

void ClientMsgHandler::netEcho(IConnection* pConn, const char* pBuf, uint32_t nLen) {
	//MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	//slither::UnLockSkinReq unLockSkinReq;
	//unLockSkinReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	
	slither::VerifyNetSpeedAck netEchoAck;
	netEchoAck.set_errorcode(slither::LOBBY_SUCCESS);

	if (pConn) {
		string strResponse;
		cputil::BuildResponseProto(netEchoAck, strResponse, slither::REQ_VERIFY_NETSPEED);
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}


	return;
}

void ClientMsgHandler::getBulletins(IConnection* pConn) 
{
	slither::GetBulletinAck getBulletinAck;

	vector<slither::BulletinInfo> bulletins = serverService->getNowAllBulletin();
	int bulletinNum = bulletins.size();
	for (int i = 0; i < bulletinNum; i++)
	{
		slither::BulletinInfo* bullTemp = getBulletinAck.add_bulletins();
		bullTemp->CopyFrom(bulletins.at(i));
	}

	if (pConn) {
		string strResponse;
		cputil::BuildResponseProto(getBulletinAck, strResponse, slither::REQ_GET_BULLETIN);
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}

	return;
}

void ClientMsgHandler::getEnterRoomId(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	if (!pConn || uLen == 0) {
		return;
	}

	slither::GetEnterRoomIdReq getRoomIdReq;
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	getRoomIdReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	slither::GetEnterRoomIdAck getRoomIdAck;
	const RoomInfo& roomInfo = gpGameGroupMng->GetAvailableGameRoom(getRoomIdReq.playerid());

	if (roomInfo.uRoomId == 0) {
		getRoomIdAck.set_errorcode(slither::GAME_SERVER_NOT_FOUND);
	}
	else {
		GameServerInfo gsInfo;
		bool bRet = gpGameGroupMng->GetGameServer(roomInfo.uGSId, gsInfo);
		if (!bRet) {						// 没找到对应的gameserver信息
			getRoomIdAck.set_errorcode(slither::GAME_SERVER_NOT_FOUND);
		}
		else {
			getRoomIdAck.mutable_roominfo()->set_ip(gsInfo.strIp);
			getRoomIdAck.mutable_roominfo()->set_port(gsInfo.uPort);
			getRoomIdAck.mutable_roominfo()->set_roomid(roomInfo.uRoomId);
			getRoomIdAck.mutable_roominfo()->set_lefttime(roomInfo.uLeftTime);
		}
	}

	string strResponse;
	cputil::BuildResponseProto(getRoomIdAck, strResponse, slither::REQ_GET_ENTER_ROOM_ID);
	pConn->SendMsg(strResponse.c_str(), strResponse.size());
	return;
}




void ClientMsgHandler::palyerAttention(IConnection* pConn, const char* pBuf, uint32_t nLen) {
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	slither::PayAttentionReq attentionReq;
	attentionReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	playerMsgDeal->attention(pConn, attentionReq);
	return;
}


void ClientMsgHandler::getAttentivedList(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	slither::GetAttentionListReq getAttentionListReq;

	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	getAttentionListReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	gpTeamMng->GetInviteList(getAttentionListReq.playerid());
}

void ClientMsgHandler::getFansList(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	slither::GetFansListReq getFansListReq;

	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	getFansListReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	gpTeamMng->GetInviteList(getFansListReq.playerid());
}




void ClientMsgHandler::createTeam(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	if (!pConn || uLen == 0) {
		return;
	}

	slither::CreateTeamReq createTeamReq;
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	createTeamReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	gpTeamMng->CreateTeam(createTeamReq.playerid());
}

void ClientMsgHandler::leaveTeam(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	if (!pConn || uLen == 0) {
		return;
	}

	slither::LeaveTeamReq leaveTeamReq;
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	leaveTeamReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	gpTeamMng->LeaveTeam(leaveTeamReq.playerid());
}

void ClientMsgHandler::getInviteList(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	if (!pConn || uLen == 0) {
		return;
	}

	slither::GetInviteListReq getInviteListReq;
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	getInviteListReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	gpTeamMng->GetInviteList(getInviteListReq.playerid());
}

void ClientMsgHandler::inviteJoinTeam(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	if (!pConn || uLen == 0) {
		return;
	}

	slither::InviteJoinTeamReq inviteJoinReq;
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	inviteJoinReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	uint32_t uInviteNum = inviteJoinReq.invitelist_size();
	list<uint64_t> inviteList;
	for (uint32_t i = 0; i < uInviteNum; ++i) {
		inviteList.push_back(inviteJoinReq.invitelist(i));
	}

	gpTeamMng->InviteJoinTeam(inviteJoinReq.playerid(), inviteList);
	return;
}

void ClientMsgHandler::acceptJoinTeam(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	if (!pConn || uLen == 0) {
		return;
	}

	slither::AcceptJoinTeamReq acceptJoinTeamReq;
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	acceptJoinTeamReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	gpTeamMng->AcceptJoinTeam(acceptJoinTeamReq.playerid(), acceptJoinTeamReq.teamid());
	return;
}

void ClientMsgHandler::refuseJoinTeam(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	if (!pConn || uLen == 0) {
		return;
	}

	slither::RefuseJoinTeamReq refuseJoinTeamReq;
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	refuseJoinTeamReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	gpTeamMng->RefuseJoinTeam(refuseJoinTeamReq.playerid(), refuseJoinTeamReq.teamid());
	return;
}

void ClientMsgHandler::kickTeamMember(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	if (!pConn || uLen == 0) {
		return;
	}

	slither::KickTeamMemberReq kickMemberReq;
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	kickMemberReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	gpTeamMng->KickTeamMember(kickMemberReq.playerid(), kickMemberReq.memberid());
	return;
}

void ClientMsgHandler::startTeamGame(IConnection* pConn, const char* pBuf, uint32_t uLen) {
	if (!pConn || uLen == 0) {
		return;
	}

	slither::StartTeamGameReq startTeamReq;
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;
	startTeamReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	gpTeamMng->StartTeamGame(startTeamReq.playerid());
	return;
}