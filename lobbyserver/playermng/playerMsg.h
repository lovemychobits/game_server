#ifndef LOBBYSERVER_PLAYERMSGDEAL_H
#define LOBBYSERVER_PLAYERMSGDEAL_H

#include "../header.h"
#include "../mysql/DataMng.h"
#include "../../protocol/slither_lobby.pb.h"

class PlayerMsgDeal
{
public:
	~PlayerMsgDeal();
	static PlayerMsgDeal* GetInstance() {
		static PlayerMsgDeal instance;
		return &instance;
	}
	bool Init();
	void GetPlayerData(IConnection* pConn, slither:: GetPlayerInfoReq& reqInfo);
	void dealVerifySDKToken(IConnection* pConn, slither::VerifyTokenReq& reqInfo);
	void playerSetUseSkin(IConnection* pConn, slither::SetUseSkinReq& reqInfo);
	void playerUnLockSkin(uint64_t playerId, int unLockSkinId);
	void modifyPlayerInfo(IConnection* pConn, slither::ModifyPlayerInfoReq& reqInfo);
	void attention(IConnection* pConn, slither::PayAttentionReq& reqInfo);
	void getAttentivedList(IConnection* pConn, slither::GetAttentionListReq& getAttentivedListReq);
	void getFansList(IConnection* pConn, slither::GetFansListReq& getFansListReq);






private:
	DataMng m_dataMng;
	//我关注的
	map<uint64_t, map<uint64_t, uint64_t>> allPlayerAttentivedMap;
	//关注我的
	map<uint64_t, map<uint64_t, uint64_t>> allPlayerFansMap;

	void initLoadLayerAttentions(uint64_t playerId);
	//actionId 是否关注了 destId
	bool atcionIdIsAttentDestId(uint64_t actionId, uint64_t destId);
	//actionId 是否是 destId的粉丝
	bool atcionIdIsDestIdFans(uint64_t actionId, uint64_t destId);

	void delAttentPalyer(uint64_t actionId, uint64_t delPlayerId);
	void addAttentPlayer(uint64_t actionId, uint64_t addPlayerId);


	vector<uint64_t> getPlayerAttentived(uint64_t playerId, int startIndex, int endIndex);
	vector<uint64_t> getPlayerFans(uint64_t playerId, int startIndex, int endIndex);

	int getPlayerAttentivedNum(uint64_t playerId);
	int getPlayerFansNum(uint64_t playerId);

	//void putAttentionForPlayer(PlayerAttentions attentions);
		
};

#define  playerMsgDeal PlayerMsgDeal::GetInstance()

#endif