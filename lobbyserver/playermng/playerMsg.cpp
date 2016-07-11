#include "playerMsg.h"
#include "ServerService.h"
#include "../header.h"
#include "../lzSDK/lzSDK.h"
#include "../mysql/DataMng.h"
#include "../config/ServerConfig.h"
#include "../config/SkinConfig.h"
#include "../gsgroup/ClientConnMgr.h"
#include "../mysql/SqlTables.h"
#include "../../protocol/slither_lobby.pb.h"
#include "../../utils/ProtoTemplate.hpp"

#include <google/protobuf/text_format.h>




PlayerMsgDeal :: ~PlayerMsgDeal()
{

}



bool PlayerMsgDeal::Init() {
	cputil::CMysqlConnection sqlConn;
	if (!sqlConn.Init(gpServerConfig->GetMysqlIp().c_str(), gpServerConfig->GetMysqlPort(), gpServerConfig->GetMysqlUser().c_str(), gpServerConfig->GetMysqlPwd().c_str(), gpServerConfig->GetMysqlDB().c_str())) {
		ERRORLOG("init mysql connection failed, mysql ip=[" << gpServerConfig->GetMysqlIp().c_str() << "], port=[" << gpServerConfig->GetMysqlPort() << "], user=[" << gpServerConfig->GetMysqlUser().c_str() << "], pwd=[" << gpServerConfig->GetMysqlPwd().c_str() << "], db=[" << gpServerConfig->GetMysqlDB().c_str() << "]");
		return false;
	}

	SqlTables sqlTables(sqlConn);
	bool bRet = sqlTables.CreateTables("./sqls/create_tables.sql");
	if (!bRet) {
		return false;
	}

	bRet = m_dataMng.InitMysql(gpServerConfig->GetMysqlIp().c_str(), gpServerConfig->GetMysqlPort(), gpServerConfig->GetMysqlUser().c_str(), gpServerConfig->GetMysqlPwd().c_str(), gpServerConfig->GetMysqlDB().c_str());
	if (!bRet) {
		return false;
	}
	return bRet;
}

// 获取玩家数据
void PlayerMsgDeal::GetPlayerData(IConnection* pConn, slither::GetPlayerInfoReq& reqInfo) {
	string strPrint1;
	google::protobuf::TextFormat::PrintToString(reqInfo, &strPrint1);

	cout << "GetPlayerData REQ proto:" << endl << strPrint1 << endl;
	TRACELOG("GetPlayerData RESP proto:[\n" << strPrint1.c_str() << "\n");



	slither::GetPlayerInfoAck getPlayerInfoAck;
	slither::PBPlayerInfo* pPBPlayerInfo = getPlayerInfoAck.mutable_playerinfo();

	bool bRet = false;
	if (reqInfo.has_playerid() && reqInfo.playerid() > 0)
	{
		bRet = m_dataMng.GetPlayerDataByPlayerId(reqInfo.playerid(), *pPBPlayerInfo);
	}
	else if (reqInfo.has_playername() && !(reqInfo.playername().empty())){
		//TODO FIND BY NAME LIKE
		bRet = m_dataMng.GetPlayerDataByName(reqInfo.playername(), *pPBPlayerInfo, false);
	}


	if (!bRet) {
		getPlayerInfoAck.set_errorcode(slither::PLAYER_NOT_FOUND);
	}
	else{
		slither::PBPlayerSkins* pPBPlayerSkins = getPlayerInfoAck.mutable_playerskins();
		m_dataMng.FindPlayerSkinByPlayerId(pPBPlayerInfo->playerbase().playerid(), pPBPlayerSkins);
	}



	string strPrint;
	google::protobuf::TextFormat::PrintToString(getPlayerInfoAck, &strPrint);

	cout << "GetPlayerData RESP proto:" << endl << strPrint << endl;
	TRACELOG("GetPlayerData RESP proto:[\n" << strPrint.c_str() << "\n");

	if (pConn) {
		string strResponse;
		cputil::BuildResponseProto(getPlayerInfoAck, strResponse, slither::REQ_GET_PLAYER_INFO);
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}
}


void PlayerMsgDeal::dealVerifySDKToken(IConnection* pConn, slither::VerifyTokenReq& reqInfo)
{	
	string strPrint;
	google::protobuf::TextFormat::PrintToString(reqInfo, &strPrint);

	cout << "verify token REQ proto:" << endl << strPrint << endl;
	TRACELOG( "verify token REQ proto:[\n" << strPrint.c_str() <<"\n");

	string accountId = "";
	if (reqInfo.token() == "-1")
	{
		accountId = reqInfo.deviceuuid();
	}
	else{
		accountId =  lzSDKMgr->getSDKAccountId(reqInfo);
	}

	 
	slither::VerifyTokenAck verifyTokenAck;
	if (!accountId.empty())
	{
		string ipStr = "127.0.0.1";
		if (pConn){
			const char* ip = pConn->GetRemoteIp();
			string ipStrTemp(ip);
			ipStr = ipStrTemp;
		}


		slither::PBPlayerInfo* pPBPlayerInfo = verifyTokenAck.mutable_playerinfo();
		bool bRet = m_dataMng.GetPlayerDataByAccount(accountId, ipStr, *pPBPlayerInfo, reqInfo);

		if (!bRet) {
			verifyTokenAck.set_errorcode(slither::PLAYER_NOT_FOUND);
		}
		else{
			vector<slither::BulletinInfo> bulletins = serverService->getNowAllBulletin();
			int bulletinNum = bulletins.size();
			for (int i = 0; i < bulletinNum; i++)
			{
				slither::BulletinInfo* bullTemp = verifyTokenAck.add_bulletins();
				bullTemp->CopyFrom(bulletins.at(i));
			}
			
			//lzSDKMgr->postRoleCreateToSDK(accountId, ipStr, pPBPlayerInfo->mutable_playerbase(), reqInfo);
			string nowTimeStr = cputil::GetLocalTime();
			uint64_t playerId = pPBPlayerInfo->mutable_playerbase()->playerid();

			m_dataMng.UpdatePlayerLastLoginTime(playerId,nowTimeStr);
			lzSDKMgr->postRoleLogonToSDK(accountId, ipStr, pPBPlayerInfo->mutable_playerbase(), reqInfo);
			if (pConn){
				clientConnMgr->addPlayerIdAndConn(playerId, pConn);
			}

			vector<int> allSkinIds = skinConfig->getAllSkinIds();
			int len = allSkinIds.size();
			for (int i = 0; i < len; i++)
			{
				int skinId = allSkinIds.at(i);
				playerUnLockSkin(playerId, skinId);
			}

			initLoadLayerAttentions(playerId);
		}
	}
	else{
		verifyTokenAck.set_errorcode(slither::SDK_VERIFY_SDK_FAIL);
	}


	

	string strPrint2;
	google::protobuf::TextFormat::PrintToString(verifyTokenAck, &strPrint2);

	cout << "proto RESP:" << endl << strPrint2 << endl;
	TRACELOG("verify token RESP proto:[\n" << strPrint2.c_str() << "\n");

	string strResponse;
	cputil::BuildResponseProto(verifyTokenAck, strResponse, slither::REQ_VERIFY_TOKEN);

	if (pConn)
	{
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}
}
////////////////
bool isHaveTheSkinId(const int skinId, const slither::PBPlayerSkins playerSkins){
	int skinIdNum = playerSkins.skinlist().size();
	bool isHaveTheSkin = false;
	for (int i = 0; i < skinIdNum; i++)
	{
		int haveSkinId = playerSkins.skinlist().Get(i);
		if (haveSkinId == skinId){
			isHaveTheSkin = true;
			break;
		}
	}
	return isHaveTheSkin;
}

void PlayerMsgDeal::playerSetUseSkin(IConnection* pConn, slither::SetUseSkinReq& reqInfo)
{
	string strPrint;
	google::protobuf::TextFormat::PrintToString(reqInfo, &strPrint);

	cout << "playerSetUseSkin REQ proto:" << endl << strPrint << endl;
	TRACELOG("playerSetUseSkin REQ proto:[\n" << strPrint.c_str() << "\n");

	int setUseSkinId = reqInfo.skinid();
	if (setUseSkinId == -1){
		setUseSkinId = skinConfig->getRandCanInitSkinId();
	}

	int resultCode = slither::SKIN_NOT_UNLOCK;

	if (setUseSkinId > 0){
		uint64_t playerId = reqInfo.playerid();

		slither::PBPlayerSkins dbSkinInfo;
		int dbFindResult = m_dataMng.FindPlayerSkinByPlayerId(playerId, &dbSkinInfo);
		if (dbFindResult == -2){		//如果没有皮肤数据则需要去创建一个
			bool isCreateResult = m_dataMng.CreateNewPlayerSkins(playerId);
			if (isCreateResult)
			{
				m_dataMng.FindPlayerSkinByPlayerId(playerId, &dbSkinInfo);
			}
		}

		if (dbSkinInfo.useskinid() > 0){		//判断皮胖数据是否已经有了，DB默认值useSkinId为1，而其它皮肤ID也大0，所以只要大于0，则可认为DB已有对应数据
			if ((int)dbSkinInfo.useskinid() == setUseSkinId)
			{
				resultCode = slither::LOBBY_SUCCESS;
			}
			else{
				if (skinConfig->isCanInitSkinId(setUseSkinId))
				{
					dbSkinInfo.set_useskinid(setUseSkinId);
					bool isUpdateSuss = m_dataMng.UpdatePlayerSkin(playerId, dbSkinInfo);
					if (isUpdateSuss)
					{
						resultCode = slither::LOBBY_SUCCESS;
					}
				}
				else{
					bool isHaveTheSkin = isHaveTheSkinId(setUseSkinId, dbSkinInfo);
					if (isHaveTheSkin){
						dbSkinInfo.set_useskinid(setUseSkinId);
						bool isUpdateSuss = m_dataMng.UpdatePlayerSkin(playerId, dbSkinInfo);
						if (isUpdateSuss)
						{

							resultCode = slither::LOBBY_SUCCESS;
						}
					}
				}
			}
		}
	}
	else
	{
		resultCode = slither::SKIN_NOT_EXIST;
	}

	slither::SetUseSkinAck setUseSkinAck;
	setUseSkinAck.set_errorcode(resultCode);
	if (resultCode == slither::LOBBY_SUCCESS){
		setUseSkinAck.set_skinid(setUseSkinId);
	}

	google::protobuf::TextFormat::PrintToString(setUseSkinAck, &strPrint);

	cout << "playerSetUseSkin RESP proto:" << endl << strPrint << endl;
	TRACELOG("playerSetUseSkin RESP proto:[\n" << strPrint.c_str() << "\n");


	string strResponse;
	cputil::BuildResponseProto(setUseSkinAck, strResponse, slither::REQ_SET_SKIN);

	if (pConn)
	{
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}
}


const int UNLOCK_TYPE_LV = 1;
const int UNLOCK_TYPE_GRADE = 2;

bool isCanUnLock(const slither::PBPlayerBase& playerBaseInfo,map<int,int> unLockInfo)
{
	map<int, int>::iterator it;
	for (it = unLockInfo.begin(); it != unLockInfo.end(); ++it)
	{
		int unLockType = it->first;
		int unLockValue = it->second;
		
		switch (unLockType)
		{
		case UNLOCK_TYPE_LV:
			if (playerBaseInfo.level() < (uint32_t)unLockValue){
				return false;
			}
			break;
		case UNLOCK_TYPE_GRADE:
			if (playerBaseInfo.grade() < (uint32_t)unLockValue){
				return false;
			}
			break;
		default:
			return false;
			break;
		}
	}
	return true;
}


void PlayerMsgDeal::playerUnLockSkin(uint64_t playerId, int unLockSkinId)
{
	int resultCode = slither::SKIN_NOT_EXIST;

	if (unLockSkinId >= 0){
		if (skinConfig->isCanInitSkinId(unLockSkinId))
		{
			resultCode = slither::LOBBY_SUCCESS;
		}
		else{
			slither::PBPlayerSkins dbSkinInfo;
			int dbFindResult = m_dataMng.FindPlayerSkinByPlayerId(playerId, &dbSkinInfo);
			bool isNeedUnLock = false;
			if (dbFindResult == 0)
			{
				bool isHaveTheSkin = isHaveTheSkinId(unLockSkinId, dbSkinInfo);
				if (isHaveTheSkin){
					resultCode = slither::SKIN_IS_UNLOCK;
				}
				else{
					isNeedUnLock = true;
				}
			}

			if (isNeedUnLock){
				SkinInfo skinInfo = skinConfig->getSkinInfoById(unLockSkinId);
				if (!skinInfo.unLockInfoMap.empty() && skinInfo.unLockInfoMap.size() > 0)
				{
					slither::PBPlayerBase playerBaseInfo;
					int findPlayerResult = m_dataMng.FindPlayerBaseInfoByPlayerId(playerId, &playerBaseInfo);
					if (findPlayerResult == 0){
						if (isCanUnLock(playerBaseInfo, skinInfo.unLockInfoMap))
						{
							dbSkinInfo.add_skinlist(unLockSkinId);
							bool isUpdateSuss = m_dataMng.UpdatePlayerSkin(playerId, dbSkinInfo);
							if (isUpdateSuss)
							{
								resultCode = slither::LOBBY_SUCCESS;
							}
						}
						else{
							resultCode = slither::SKIN_NO_CAN_UNLOCK;
						}
					}
					else{
						resultCode = slither::PLAYER_NOT_FOUND;
					}
				}
			}

		}
	}
}

///
void PlayerMsgDeal::modifyPlayerInfo(IConnection* pConn, slither::ModifyPlayerInfoReq& reqInfo)
{
	string strPrint;
	google::protobuf::TextFormat::PrintToString(reqInfo, &strPrint);

	cout << "modifyPlayerInfo REQ proto:" << endl << strPrint << endl;
	TRACELOG("modifyPlayerInfo REQ proto:[\n" << strPrint.c_str() << "\n");

	uint64_t playerId = reqInfo.playerid();

	slither::ModifyPlayerInfoAck modifyPlayerInfoAck;
	int resultCode = slither::PLAYER_NOT_FOUND;
	
	if (playerId > 0)
	{
		slither::PBPlayerBase playerBaseInfo;
		int findPlayerResult = m_dataMng.FindPlayerBaseInfoByPlayerId(playerId, &playerBaseInfo);
		if (findPlayerResult == 0 && playerBaseInfo.playerid() > 0)
		{
			bool isNeedUpdate = false;
			bool isNewName = false;
			if (reqInfo.has_newname() && reqInfo.newname() != "")
			{
				if (reqInfo.newname() != playerBaseInfo.playername())
				{
					isNewName = true;
					if (playerBaseInfo.namemodifytimes() >= 1)
					{
						resultCode = slither::MODIFY_TIMES_MAX;
					}
					else
					{
						slither::PBPlayerBase dbPlayerTemp;
						int namePlayerNum = m_dataMng.FindPlayerBaseInfoByPlayerName(reqInfo.newname(), &dbPlayerTemp,true);
						if (namePlayerNum == -2 && dbPlayerTemp.playerid() == 0){
							playerBaseInfo.set_playername(reqInfo.newname());
							playerBaseInfo.set_namemodifytimes(playerBaseInfo.namemodifytimes() + 1);
							isNeedUpdate = true;
						}
						else{
							resultCode = slither::USER_NAME_IS_EXIST;
						}
					}
				}
				else{
					resultCode = slither::LOBBY_SUCCESS;
				}
			}

			if (reqInfo.has_newgender() && !isNewName)
			{
				if (reqInfo.newgender() != playerBaseInfo.gender())
				{
					playerBaseInfo.set_gender(reqInfo.newgender());
					playerBaseInfo.set_gendermodifytimes(playerBaseInfo.gendermodifytimes() + 1);
					isNeedUpdate = true;
				}
				else
				{
					resultCode = slither::LOBBY_SUCCESS;
				}
			}


			if (isNeedUpdate)
			{
				bool updateResult = m_dataMng.UpdatePlayerBaseInfo(playerBaseInfo);
				if (updateResult)
				{
					resultCode = slither::LOBBY_SUCCESS;
				}
				else
				{
					resultCode = slither::SERVER_DB_ERROR;
				}
			}

			if (resultCode == slither::LOBBY_SUCCESS)
			{
				modifyPlayerInfoAck.set_namemodifytimes(playerBaseInfo.namemodifytimes());
				modifyPlayerInfoAck.set_gendermodifytimes(playerBaseInfo.gendermodifytimes());
			}
		}
	}


	modifyPlayerInfoAck.set_errorcode(resultCode);

	google::protobuf::TextFormat::PrintToString(modifyPlayerInfoAck, &strPrint);

	cout << "modifyPlayerInfo RESP proto:" << endl << strPrint << endl;
	TRACELOG("modifyPlayerInfo RESP proto:[\n" << strPrint.c_str() << "\n");

	if (pConn) {
		string strResponse;
		cputil::BuildResponseProto(modifyPlayerInfoAck, strResponse, slither::REQ_MODIFY_PLAYER_INFO);
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}
}




void PlayerMsgDeal::attention(IConnection* pConn, slither::PayAttentionReq& reqInfo)
{
	string strPrint;
	google::protobuf::TextFormat::PrintToString(reqInfo, &strPrint);

	cout << "attention REQ proto:" << endl << strPrint << endl;
	TRACELOG("attention REQ proto:[\n" << strPrint.c_str() << "\n");

	slither::PayAttentionAck attentionAck;

	uint64_t actionId = reqInfo.actionplayerid();
	uint64_t targetPlayerId = reqInfo.targetplayerid();
	uint32_t actionType = reqInfo.actiontype();

	int resultCode = slither::LOBBY_SUCCESS;


	bool isAtted = atcionIdIsAttentDestId(actionId, targetPlayerId);

	if (actionType == slither::DEL)
	{
		if (!isAtted)
		{
			resultCode = slither::NOT_ATTENTED; 
		}
	}

	if (actionType == slither::ADD)
	{
		if (isAtted)
		{
			resultCode = slither::IS_ATTENTED;
		}
	}


	if (resultCode == slither::LOBBY_SUCCESS)
	{
		if (actionType == slither::ADD)
		{
			bool isInsertSuss = m_dataMng.InsertPlayerAttentionInfo(targetPlayerId, actionId);
			if (isInsertSuss)
			{
				addAttentPlayer(actionId, targetPlayerId);
			}
			else{
				resultCode = slither::SERVER_DB_ERROR;
			}
			
		}
		if (actionType == slither::DEL)
		{
			//TODO status--->to const
			bool isDelSuss = m_dataMng.UpdatePlayerAttentionInfoStatus(targetPlayerId, actionId, 1);
			if (isDelSuss)
			{
				delAttentPalyer(actionId, targetPlayerId);
			}
			else{
				resultCode = slither::SERVER_DB_ERROR;
			}
		}
	}


	attentionAck.set_errorcode(resultCode);






	google::protobuf::TextFormat::PrintToString(attentionAck, &strPrint);

	cout << "attention RESP proto:" << endl << strPrint << endl;
	TRACELOG("attention RESP proto:[\n" << strPrint.c_str() << "\n");

	if (pConn) {
		string strResponse;
		cputil::BuildResponseProto(attentionAck, strResponse, slither::REQ_PAY_ATTENTION);
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}
}



void PlayerMsgDeal::getAttentivedList(IConnection* pConn, slither::GetAttentionListReq& reqInfo)
{
	string strPrint;
	google::protobuf::TextFormat::PrintToString(reqInfo, &strPrint);

	cout << "getAttentivedList REQ proto:" << endl << strPrint << endl;
	TRACELOG("getAttentivedList REQ proto:[\n" << strPrint.c_str() << "\n");

	slither::GetAttentionListAck getAttentionListAck;
	int resultCode = slither::LOBBY_SUCCESS;

	uint64_t playerId = reqInfo.playerid();
	uint32_t page = reqInfo.page();			//客户端从0开始传
	page += 1;

	int pageRecordNum = 20;
	int startIndex = (page - 1)*pageRecordNum;
	int endIndex = page*pageRecordNum-1;
	

	vector<uint64_t> palyerAttivedVector = getPlayerAttentived(playerId, startIndex, endIndex);
	if (palyerAttivedVector.size() > 0){
		vector<uint64_t> ::iterator palyerAttivedIterator;
		for (palyerAttivedIterator = palyerAttivedVector.begin(); palyerAttivedIterator != palyerAttivedVector.end(); palyerAttivedIterator++)
		{
			uint64_t attentivedPlayerId = *palyerAttivedIterator;
			bool playerIsMeFans = atcionIdIsDestIdFans(playerId, attentivedPlayerId);
			
			slither::AttentionInfo* attentInfo = getAttentionListAck.add_attentionlist();
			slither::PBPlayerInfo* playerInfo = attentInfo->mutable_playerinfo();

			m_dataMng.GetPlayerDataByPlayerId(attentivedPlayerId, *playerInfo);

			if (playerInfo->playerbase().playerid() > 0){
				slither::PBPlayerBase playerBaseInfo = playerInfo->playerbase();
				attentInfo->set_playerid(attentivedPlayerId);
				attentInfo->set_level(playerBaseInfo.level());

				attentInfo->set_attentivednum(getPlayerAttentivedNum(attentivedPlayerId));
				attentInfo->set_fansnum(getPlayerFansNum(attentivedPlayerId));

				bool isOnline = false;
				if (clientConnMgr->getConnByPlayerId(attentivedPlayerId))
				{
					isOnline = true;
				}

				attentInfo->set_isonline(isOnline ? 1 : 0);
				attentInfo->set_playerismefans(playerIsMeFans ? 1 : 0);
			}
			else{
				attentInfo = NULL;
				playerInfo = NULL;
			}
		}
	}








	getAttentionListAck.set_errorcode(resultCode);


	google::protobuf::TextFormat::PrintToString(getAttentionListAck, &strPrint);

	cout << "getAttentivedList RESP proto:" << endl << strPrint << endl;
	TRACELOG("getAttentivedList RESP proto:[\n" << strPrint.c_str() << "\n");

	if (pConn) {
		string strResponse;
		cputil::BuildResponseProto(getAttentionListAck, strResponse, slither::REQ_PAY_ATTENTION);
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}
}



void PlayerMsgDeal::getFansList(IConnection* pConn, slither::GetFansListReq& reqInfo)
{
	string strPrint;
	google::protobuf::TextFormat::PrintToString(reqInfo, &strPrint);

	cout << "getFansList REQ proto:" << endl << strPrint << endl;
	TRACELOG("getFansList REQ proto:[\n" << strPrint.c_str() << "\n");

	slither::GetFansListAck getFansListAck;

	int resultCode = slither::LOBBY_SUCCESS;


	uint64_t playerId = reqInfo.playerid();
	uint32_t page = reqInfo.page();			//客户端从0开始传
	page += 1;

	int pageRecordNum = 20;
	int startIndex = (page - 1)*pageRecordNum;
	int endIndex = page*pageRecordNum - 1;


	vector<uint64_t> palyerFansVector = getPlayerFans(playerId, startIndex, endIndex);
	if (palyerFansVector.size() > 0){
		vector<uint64_t> ::iterator palyerFansIterator;
		for (palyerFansIterator = palyerFansVector.begin(); palyerFansIterator != palyerFansVector.end(); palyerFansIterator++)
		{
			uint64_t fansPlayerId = *palyerFansIterator;
			bool meIsPlayerFans = atcionIdIsDestIdFans(fansPlayerId, playerId);

			slither::FansInfo* fansInfo = getFansListAck.add_fanslist();
			slither::PBPlayerInfo* playerInfo = fansInfo->mutable_playerinfo();

			m_dataMng.GetPlayerDataByPlayerId(fansPlayerId, *playerInfo);

			if (playerInfo->playerbase().playerid() > 0){
				slither::PBPlayerBase playerBaseInfo = playerInfo->playerbase();
				fansInfo->set_playerid(fansPlayerId);
				fansInfo->set_level(playerBaseInfo.level());

				fansInfo->set_attentivednum(getPlayerAttentivedNum(fansPlayerId));
				fansInfo->set_fansnum(getPlayerFansNum(fansPlayerId));

				bool isOnline = false;
				if (clientConnMgr->getConnByPlayerId(fansPlayerId))
				{
					isOnline = true;
				}

				fansInfo->set_isonline(isOnline ? 1 : 0);
				fansInfo->set_meisplayerfans(meIsPlayerFans ? 1 : 0);
			}
			else{
				fansInfo = NULL;
				playerInfo = NULL;
			}

		}
	}




	getFansListAck.set_errorcode(resultCode);


	google::protobuf::TextFormat::PrintToString(getFansListAck, &strPrint);

	cout << "getFansList RESP proto:" << endl << strPrint << endl;
	TRACELOG("getFansList RESP proto:[\n" << strPrint.c_str() << "\n");

	if (pConn) {
		string strResponse;
		cputil::BuildResponseProto(getFansListAck, strResponse, slither::REQ_PAY_ATTENTION);
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}
}









































//////////////////
void PlayerMsgDeal::initLoadLayerAttentions(uint64_t playerId){
	vector<PlayerAttentions> playerAttentions = m_dataMng.FindPlayerAttentivedAndFans(playerId);
	if (playerAttentions.size() > 0)
	{
		vector<PlayerAttentions> :: iterator it;
		for (it = playerAttentions.begin(); it != playerAttentions.end(); ++it)
		{
			PlayerAttentions attention = *it;
			if (attention.getId() > 0)
			{
				if (attention.getFansId() == playerId)				//FansId =于自己的是，自己的关注的 列表
				{
					uint64_t attentivedId = attention.getPlayerId();			//被关注者的ID
					map<uint64_t, uint64_t> playerAttentivedMap;

					map<uint64_t, map<uint64_t, uint64_t>> ::iterator allAttentivedMapIterator = allPlayerAttentivedMap.find(playerId);
					if (allAttentivedMapIterator != allPlayerAttentivedMap.end())
					{
						playerAttentivedMap = allAttentivedMapIterator->second;
					}
					
					playerAttentivedMap[attentivedId] = attentivedId;
					allPlayerAttentivedMap[playerId] = playerAttentivedMap;
				}


				if (attention.getPlayerId() == playerId)				//PlayerId =于自己的是，关注我的 列表（即粉丝列表）
				{
					uint64_t fansId = attention.getFansId();			//关注者的ID[粉丝ID]
					map<uint64_t, uint64_t> playerFansMap;

					map<uint64_t, map<uint64_t, uint64_t>> ::iterator allFansMapIterator = allPlayerFansMap.find(playerId);
					if (allFansMapIterator != allPlayerFansMap.end())
					{
						playerFansMap = allFansMapIterator->second;
					}

					playerFansMap[fansId] = fansId;
					allPlayerFansMap[playerId] = playerFansMap;
				}
			}
		}
	}
}


// actionId 是否关注了 destId
bool PlayerMsgDeal :: atcionIdIsAttentDestId(uint64_t actionId, uint64_t destId)
{
	bool isAttentived = false;
	
	map<uint64_t, map<uint64_t, uint64_t>> ::iterator allAttentivedMapIterator = allPlayerAttentivedMap.find(actionId);
	if (allAttentivedMapIterator != allPlayerAttentivedMap.end())
	{
		map<uint64_t, uint64_t> playerAttentivedMap = allAttentivedMapIterator->second;
		map<uint64_t, uint64_t> ::iterator attentivedIterator = playerAttentivedMap.find(destId);
		if (attentivedIterator != playerAttentivedMap.end())
		{
			isAttentived = true;
		}
	}

	return isAttentived;
}


// actionId 是否 destId 的Fans
bool PlayerMsgDeal::atcionIdIsDestIdFans(uint64_t actionId, uint64_t destId)
{
	bool isFans = false;

	map<uint64_t, map<uint64_t, uint64_t>> ::iterator allFansMapIterator = allPlayerFansMap.find(actionId);
	if (allFansMapIterator != allPlayerFansMap.end())
	{
		map<uint64_t, uint64_t> playerFansMap = allFansMapIterator->second;
		map<uint64_t, uint64_t> ::iterator fanssIterator = playerFansMap.find(destId);
		if (fanssIterator != playerFansMap.end())
		{
			isFans = true;
		}
	}

	return isFans;
}



void PlayerMsgDeal::delAttentPalyer(uint64_t actionId, uint64_t delPlayerId)
{
	map<uint64_t, map<uint64_t, uint64_t>> ::iterator allAttentivedMapIterator = allPlayerAttentivedMap.find(actionId);
	if (allAttentivedMapIterator != allPlayerAttentivedMap.end())
	{
		map<uint64_t, uint64_t> playerAttentivedMap = allAttentivedMapIterator->second;
		map<uint64_t, uint64_t> ::iterator attentivedIterator = playerAttentivedMap.find(delPlayerId);
		if (attentivedIterator != playerAttentivedMap.end())
		{
			playerAttentivedMap.erase(delPlayerId);
		}
	}


	map<uint64_t, map<uint64_t, uint64_t>> ::iterator allFansMapIterator = allPlayerFansMap.find(delPlayerId);
	if (allFansMapIterator != allPlayerFansMap.end())
	{
		map<uint64_t, uint64_t> playerFansMap = allFansMapIterator->second;
		map<uint64_t, uint64_t> ::iterator fanssIterator = playerFansMap.find(actionId);
		if (fanssIterator != playerFansMap.end())
		{
			playerFansMap.erase(actionId);
		}
	}

}


void PlayerMsgDeal::addAttentPlayer(uint64_t actionId, uint64_t addPlayerId)
{
	map<uint64_t, uint64_t> playerAttentivedMap;
	map<uint64_t, map<uint64_t, uint64_t>> ::iterator allAttentivedMapIterator = allPlayerAttentivedMap.find(actionId);
	if (allAttentivedMapIterator != allPlayerAttentivedMap.end())
	{
		playerAttentivedMap = allAttentivedMapIterator->second;
	}
	playerAttentivedMap[addPlayerId] = addPlayerId;
	allPlayerAttentivedMap[actionId] = playerAttentivedMap;



	map<uint64_t, uint64_t> playerFansMap;
	map<uint64_t, map<uint64_t, uint64_t>> ::iterator allFansMapIterator = allPlayerFansMap.find(addPlayerId);
	if (allFansMapIterator != allPlayerFansMap.end())
	{
		playerFansMap = allFansMapIterator->second;
	}
	playerFansMap[actionId] = actionId;
	allPlayerFansMap[addPlayerId] = playerFansMap;
}



vector<uint64_t> PlayerMsgDeal::getPlayerAttentived(uint64_t playerId,int startIndex,int endIndex)
{
	vector<uint64_t> palyerAttentivedVector;

	map<uint64_t, uint64_t> playerAttentivedMap;
	map<uint64_t, map<uint64_t, uint64_t>> ::iterator allAttentivedMapIterator = allPlayerAttentivedMap.find(playerId);
	if (allAttentivedMapIterator != allPlayerAttentivedMap.end())
	{
		playerAttentivedMap = allAttentivedMapIterator->second;
	}

	if (playerAttentivedMap.size() > 0)
	{
		map<uint64_t, uint64_t> ::iterator  playerAttentivedIterator;

		for (int i = 0; i <= endIndex; i++)
		{
			if (i == 0){
				playerAttentivedIterator = playerAttentivedMap.begin();
			}

			if (playerAttentivedIterator != playerAttentivedMap.end()){
				if (i >= startIndex && i <= endIndex){
					palyerAttentivedVector.push_back(playerAttentivedIterator->first);
				}
			}

			playerAttentivedIterator++;
		}
	}

	return palyerAttentivedVector;
}


vector<uint64_t> PlayerMsgDeal::getPlayerFans(uint64_t playerId, int startIndex, int endIndex)
{
	vector<uint64_t> palyerFansVector;

	map<uint64_t, uint64_t> playerFansMap;
	map<uint64_t, map<uint64_t, uint64_t>> ::iterator allFansMapIterator = allPlayerFansMap.find(playerId);
	if (allFansMapIterator != allPlayerFansMap.end())
	{
		playerFansMap = allFansMapIterator->second;
	}

	if (playerFansMap.size() > 0)
	{
		map<uint64_t, uint64_t> ::iterator  playerFansIterator;

		for (int i = 0; i <= endIndex; i++)
		{
			if (i == 0){
				playerFansIterator = playerFansMap.begin();
			}

			if (playerFansIterator != playerFansMap.end()){
				if (i >= startIndex && i <= endIndex){
					palyerFansVector.push_back(playerFansIterator->first);
				}
			}

			playerFansIterator++;
		}
	}

	return palyerFansVector;
}



///
int PlayerMsgDeal::getPlayerAttentivedNum(uint64_t playerId)
{
	int attentivedNum = 0;

	map<uint64_t, uint64_t> playerAttentivedMap;
	map<uint64_t, map<uint64_t, uint64_t>> ::iterator allAttentivedMapIterator = allPlayerAttentivedMap.find(playerId);
	if (allAttentivedMapIterator != allPlayerAttentivedMap.end())
	{
		playerAttentivedMap = allAttentivedMapIterator->second;
		attentivedNum = playerAttentivedMap.size();
	}


	return attentivedNum;
}


int PlayerMsgDeal::getPlayerFansNum(uint64_t playerId)
{
	int fansNum = 0;

	map<uint64_t, uint64_t> playerFansMap;
	map<uint64_t, map<uint64_t, uint64_t>> ::iterator allFansMapIterator = allPlayerFansMap.find(playerId);
	if (allFansMapIterator != allPlayerFansMap.end())
	{
		playerFansMap = allFansMapIterator->second;
		fansNum = playerFansMap.size();
	}

	return fansNum;
}

