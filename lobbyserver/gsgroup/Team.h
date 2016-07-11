#ifndef LOBBYSERVER_TEAM_H
#define LOBBYSERVER_TEAM_H

#include "../header.h"
#include "../../protocol/slither_lobby.pb.h"
#include <list>
using namespace std;

#define TEAM_MAX_MEMBER 5
class Team 
{
public:
	Team(uint32_t uTeamId);
	~Team();

	void SerializeToPB(slither::TeamInfo& pbTeamInfo);

	bool AddMember(uint64_t uMemberId);				// 添加成员
	bool KickMember(uint64_t uMemberId);			// 踢出成员
	bool IsFull();									// 房间是否已经满了
	bool IsEmpty();									// 队伍是否已经没有人了
	uint64_t GetCaptain();							// 获取队长

public:
	uint32_t GetTeamId() {
		return m_uTeamId;
	}

	const list<uint64_t>& GetMemberList() {
		return m_memberList;
	}

private:
	void SetCaptain(uint64_t uNewCaptainId);		// 设置队长
	bool IsExist(uint64_t uMemberId);				// 成员是否存在

private:
	uint32_t m_uTeamId;								// 队伍id
	list<uint64_t> m_memberList;					// 队员列表
	uint64_t m_uCaptainId;							// 队长ID
};

#endif