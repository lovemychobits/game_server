#ifndef LOBBYSERVER_TEAMMNG_H
#define LOBBYSERVER_TEAMMNG_H

#include "../header.h"
#include <list>
#include <map>
#include "Team.h"
using namespace std;

class TeamMng {
public:
	~TeamMng();
	static TeamMng* GetInstance() {
		static TeamMng instance;
		return &instance;
	}

	// 玩家相关操作
	void CreateTeam(uint64_t uCreator);													// 创建队伍
	void LeaveTeam(uint64_t uPlayerId);													// 离开队伍
	void GetInviteList(uint64_t uInvitor);												// 获取邀请列表
	void InviteJoinTeam(uint64_t uInvitor, const list<uint64_t>& invteList);			// 邀请玩家加入队伍
	void AcceptJoinTeam(uint64_t uPlayerId, uint32_t uTeamId);							// 接受队伍邀请
	void RefuseJoinTeam(uint64_t uPlayerId, uint32_t uTeamId);							// 拒绝队伍邀请
	void KickTeamMember(uint64_t uKickerId, uint64_t uMemberId);						// 踢出队伍成员
	void StartTeamGame(uint64_t uPlayerId);												// 开始团队模式游戏

public:
	// 游戏内调用
	void PlayerDisconnect(uint64_t uPlayerId);											// 玩家断开连接

	// 测试使用，现在只要在线的玩家，除了自己就可以邀请
	void AddCanInvitePlayer(uint64_t uPlayerId);
	set<uint64_t> GetCanInvitePlayer();

private:
	TeamMng();

private:
	void AddPlayerIntoTeam(uint64_t uPlayerId, uint32_t uTeamId);						// 将玩家加入队伍map
	void DelPlayerFromTeam(uint64_t uPlayerId);											// 将玩家从队伍map中删除
	Team* GetTeamByPlayer(uint64_t uPlayerId);											// 获取玩家所在的队伍
	Team* GetTeam(uint32_t uTeamId);													// 根据TeamId获取Team
	void AddTeam(Team* pNewTeam);														// 添加team
	void DelTeam(Team* pTeam);															// 删除Team

private:
	uint32_t m_uTeamId;
	map<uint32_t, Team*> m_teamMap;														// 队伍map
	map<uint64_t, uint32_t> m_playerTeamMap;											// 玩家所在队伍map

	// 测试使用，可以邀请的玩家id
	set<uint64_t> m_canInviteSet;
};

#define gpTeamMng TeamMng::GetInstance()

#endif