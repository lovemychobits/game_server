#include "Team.h"

Team::Team(uint32_t uTeamId) : m_uTeamId(uTeamId), m_uCaptainId(0) {

}

Team::~Team() {
	m_memberList.clear();
}

void Team::SerializeToPB(slither::TeamInfo& pbTeamInfo) {
	return;
}

bool Team::AddMember(uint64_t uMemberId) {
	// �жϷ����Ƿ��Ѿ�����
	if (IsFull()) {
		return false;
	}
	
	if (IsExist(uMemberId)) {						// ����Ѿ����ڣ�����ʧ��
		ERRORLOG("add memberId=[" << uMemberId << "] into teamId=[" << m_uTeamId << "] failed, already exist");
		return false;
	}

	// ����Ҽ������
	m_memberList.push_back(uMemberId);

	// ����ǵ�һ����ң���ô��������Ϊ�ӳ�
	if (m_memberList.size() == 1) {
		SetCaptain(uMemberId);
	}

	return true;
}

bool Team::KickMember(uint64_t uMemberId) {
	
	list<uint64_t>::iterator memberIt = m_memberList.begin();
	list<uint64_t>::iterator memberItEnd = m_memberList.end();
	for (; memberIt != memberItEnd; memberIt++)
	{
		if (*memberIt == uMemberId) {
			m_memberList.erase(memberIt);
			break;
		}
	}

	// ����Ƕӳ��뿪��Ϸ����ô���õڶ�˳λΪ�ӳ�����������ڣ���ô�����ɢ
	if (uMemberId == m_uCaptainId) {
		if (IsEmpty()) {
			return true;
		}

		uint64_t uSecondMemberId = m_memberList.front();
		SetCaptain(uSecondMemberId);
	}

	return true;
}

bool Team::IsFull() {
	if (m_memberList.size() == TEAM_MAX_MEMBER) {
		return true;
	}

	return false;
}

bool Team::IsEmpty() {
	return m_memberList.empty();
}

uint64_t Team::GetCaptain() {
	return m_uCaptainId;
}

void Team::SetCaptain(uint64_t uNewCaptainId) {
	m_uCaptainId = uNewCaptainId;
}

bool Team::IsExist(uint64_t uMemberId) {

	list<uint64_t>::iterator memberIt = m_memberList.begin();
	list<uint64_t>::iterator memberItEnd = m_memberList.end();

	for (; memberIt != memberItEnd; memberIt++) {
		if (*memberIt == uMemberId) {
			return true;
		}
	}

	return false;
}