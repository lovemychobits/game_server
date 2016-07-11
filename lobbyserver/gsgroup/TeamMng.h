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

	// �����ز���
	void CreateTeam(uint64_t uCreator);													// ��������
	void LeaveTeam(uint64_t uPlayerId);													// �뿪����
	void GetInviteList(uint64_t uInvitor);												// ��ȡ�����б�
	void InviteJoinTeam(uint64_t uInvitor, const list<uint64_t>& invteList);			// ������Ҽ������
	void AcceptJoinTeam(uint64_t uPlayerId, uint32_t uTeamId);							// ���ܶ�������
	void RefuseJoinTeam(uint64_t uPlayerId, uint32_t uTeamId);							// �ܾ���������
	void KickTeamMember(uint64_t uKickerId, uint64_t uMemberId);						// �߳������Ա
	void StartTeamGame(uint64_t uPlayerId);												// ��ʼ�Ŷ�ģʽ��Ϸ

public:
	// ��Ϸ�ڵ���
	void PlayerDisconnect(uint64_t uPlayerId);											// ��ҶϿ�����

	// ����ʹ�ã�����ֻҪ���ߵ���ң������Լ��Ϳ�������
	void AddCanInvitePlayer(uint64_t uPlayerId);
	set<uint64_t> GetCanInvitePlayer();

private:
	TeamMng();

private:
	void AddPlayerIntoTeam(uint64_t uPlayerId, uint32_t uTeamId);						// ����Ҽ������map
	void DelPlayerFromTeam(uint64_t uPlayerId);											// ����ҴӶ���map��ɾ��
	Team* GetTeamByPlayer(uint64_t uPlayerId);											// ��ȡ������ڵĶ���
	Team* GetTeam(uint32_t uTeamId);													// ����TeamId��ȡTeam
	void AddTeam(Team* pNewTeam);														// ���team
	void DelTeam(Team* pTeam);															// ɾ��Team

private:
	uint32_t m_uTeamId;
	map<uint32_t, Team*> m_teamMap;														// ����map
	map<uint64_t, uint32_t> m_playerTeamMap;											// ������ڶ���map

	// ����ʹ�ã�������������id
	set<uint64_t> m_canInviteSet;
};

#define gpTeamMng TeamMng::GetInstance()

#endif