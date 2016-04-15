#ifndef LOBBYSERVER_GAMEGROUPMNG_H
#define LOBBYSERVER_GAMEGROUPMNG_H

#include "../header.h"
using namespace cpnet;

// gameserver状态
enum EGSState
{
	NEW_SERVER     = 0,						// 新区
	ACTIVE_SERVER,							// 活跃
	HOT_SERVER,								// 火热
	FULL_HOUSE,								// 爆满
	MAINTAIN,								// 维护
};

struct GroupInfo
{
	uint32_t uGroupId;			// 区组ID
	string strGroupName;		// 区组名
	string strIp;				// 区组IP
	uint32_t uPort;				// 区组端口
	bool bHasRegister;			// 已经注册过了
	uint32_t uState;			// 区组当前状态
	GroupInfo() : uGroupId(0), uPort(0), bHasRegister(false), uState(MAINTAIN)
	{

	}
};

class GameGroupMng
{
public:
	GameGroupMng();
	~GameGroupMng();

	bool LoadGameGroupConf();
	void GameServerDisconnect(IConnection* pGsConnection);
	bool RegGameServer(IConnection* pGsConnection, uint32_t uGroupId, const string& strIp, uint32_t uPort, uint32_t uState);
	void GetGameGroups(vector<GroupInfo>& groupVec);

private:
	map<uint32_t, GroupInfo> m_groupMap;						// 区组信息map
	map<IConnection*, uint32_t> m_groupConnMap;					// 区组连接对应ID信息
};

#endif