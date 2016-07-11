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

	bool AddMember(uint64_t uMemberId);				// ��ӳ�Ա
	bool KickMember(uint64_t uMemberId);			// �߳���Ա
	bool IsFull();									// �����Ƿ��Ѿ�����
	bool IsEmpty();									// �����Ƿ��Ѿ�û������
	uint64_t GetCaptain();							// ��ȡ�ӳ�

public:
	uint32_t GetTeamId() {
		return m_uTeamId;
	}

	const list<uint64_t>& GetMemberList() {
		return m_memberList;
	}

private:
	void SetCaptain(uint64_t uNewCaptainId);		// ���öӳ�
	bool IsExist(uint64_t uMemberId);				// ��Ա�Ƿ����

private:
	uint32_t m_uTeamId;								// ����id
	list<uint64_t> m_memberList;					// ��Ա�б�
	uint64_t m_uCaptainId;							// �ӳ�ID
};

#endif