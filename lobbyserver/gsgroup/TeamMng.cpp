#include "TeamMng.h"
#include "../../protocol/slither_lobby.pb.h"
#include "ClientConnMgr.h"

TeamMng::TeamMng() : m_uTeamId(1) {

}

TeamMng::~TeamMng() {

}

/*
	��������������
	������ 
	1��uCreator ���������Id
*/
void TeamMng::CreateTeam(uint64_t uCreator) {
	Team* pTeam = new Team(m_uTeamId++);
	pTeam->AddMember(uCreator);

	AddTeam(pTeam);
	AddPlayerIntoTeam(uCreator, pTeam->GetTeamId());

	TRACELOG("player id=[" << uCreator << "] create room id=[" << pTeam->GetTeamId() << "]");

	// ���ؿͻ��˴���������
	slither::CreateTeamAck createTeamAck;
	IConnection* pClientConn = clientConnMgr->getConnByPlayerId(uCreator);
	if (!pClientConn || !pClientConn->IsConnected()) {
		return;
	}

	string strResponse;
	cputil::BuildResponseProto(createTeamAck, strResponse, slither::REQ_CREATE_TEAM);
	pClientConn->SendMsg(strResponse.c_str(), strResponse.size());

	return;
}

/*
	����������뿪����
	������
	1��uPlayerId �뿪��������Id
*/
void TeamMng::LeaveTeam(uint64_t uPlayerId) {

	slither::LeaveTeamAck leaveTeamAck;
	string strResponse;

	IConnection* pPlayerConn = clientConnMgr->getConnByPlayerId(uPlayerId);
	if (!pPlayerConn || !pPlayerConn->IsConnected()) {
		return;
	}

	Team* pTeam = GetTeamByPlayer(uPlayerId);
	if (!pTeam) {
		DEBUGLOG("cannot find player id=[" << uPlayerId << "] team");
		leaveTeamAck.set_errorcode(slither::TEAM_NOT_EXIST);

		cputil::BuildResponseProto(leaveTeamAck, strResponse, slither::REQ_LEAVE_TEAM);
		pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	pTeam->KickMember(uPlayerId);
	TRACELOG("player id=[" << uPlayerId << "] leave team id=[" << pTeam->GetTeamId() << "]");

	// ��������Ѿ����ˣ���ôɾ������
	if (pTeam->IsEmpty()) {
		DelTeam(pTeam);
	}

	// ����������滹���ˣ�֪ͨ�����ˣ������뿪�˶���
	slither::MemberLeaveTeamNty memberLeaveNty;
	memberLeaveNty.set_playerid(uPlayerId);
	string strNotify;
	cputil::BuildResponseProto(memberLeaveNty, strNotify, slither::NTY_MEMBER_LEAVE_TEAM);

	const list<uint64_t>& memberList = pTeam->GetMemberList();
	list<uint64_t>::const_iterator memberIt = memberList.begin();
	list<uint64_t>::const_iterator memberItEnd = memberList.end();
	for (; memberIt != memberItEnd; memberIt++) {
		IConnection* pMemberConn = clientConnMgr->getConnByPlayerId(*memberIt);
		if (!pMemberConn || !pMemberConn->IsConnected()) {
			continue;
		}

		pMemberConn->SendMsg(strNotify.c_str(), strNotify.size());
	}

	// ֪ͨ�뿪��������
	cputil::BuildResponseProto(leaveTeamAck, strResponse, slither::REQ_LEAVE_TEAM);
	pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());

	return;
}

/*
	��������ȡ��������б�
	������
	1��uInvitor ������ID
*/
void TeamMng::GetInviteList(uint64_t uInvitor) {

	///////// ����ʹ��
	set<uint64_t> inviteSet = GetCanInvitePlayer();
	inviteSet.erase(uInvitor);
	/////////

	IConnection* pInvitorConn = clientConnMgr->getConnByPlayerId(uInvitor);
	if (!pInvitorConn) {
		return;
	}

	slither::GetInviteListAck getInviteListAck;
	set<uint64_t>::iterator inviteIt = inviteSet.begin();
	set<uint64_t>::iterator inviteItEnd = inviteSet.end();
	for (; inviteIt != inviteItEnd; inviteIt++) {
		slither::TeamMember* pMember = getInviteListAck.add_invitelist();
		pMember->set_playerid(*inviteIt);
	}

	string strResponse;
	cputil::BuildResponseProto(getInviteListAck, strResponse, slither::REQ_GET_INVITE_LIST);
	pInvitorConn->SendMsg(strResponse.c_str(), strResponse.size());
	return;
}

/*
	����������������
	������
	1��uInvitor ������ID
	2��invteList ���������б�
*/
void TeamMng::InviteJoinTeam(uint64_t uInvitor, const list<uint64_t>& invteList) {

	slither::InviteJoinTeamAck inviteJoinAck;
	string strResponse;

	IConnection* pPlayerConn = clientConnMgr->getConnByPlayerId(uInvitor);
	if (!pPlayerConn || !pPlayerConn->IsConnected()) {
		return;
	}

	Team* pTeam = GetTeamByPlayer(uInvitor);
	if (!pTeam) {
		DEBUGLOG("cannot find player id=[" << uInvitor << "] team");
		inviteJoinAck.set_errorcode(slither::TEAM_NOT_EXIST);

		cputil::BuildResponseProto(inviteJoinAck, strResponse, slither::REQ_INVITE_JOIN_TEAM);
		pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	// ֪ͨ�������ɹ�
	cputil::BuildResponseProto(inviteJoinAck, strResponse, slither::REQ_INVITE_JOIN_TEAM);
	pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());


	// ֪ͨ������������
	slither::JoinTeamInviteNty inviteNty;
	inviteNty.set_playerid(uInvitor);
	inviteNty.set_teamid(pTeam->GetTeamId());

	strResponse.clear();
	cputil::BuildResponseProto(inviteNty, strResponse, slither::NTY_JOIN_TEAM_INVITE);

	list<uint64_t>::const_iterator inviteIt = invteList.begin();
	list<uint64_t>::const_iterator inviteItEnd = invteList.end();
	for (; inviteIt != inviteItEnd; inviteIt++) {
		IConnection* pInvitorConn = clientConnMgr->getConnByPlayerId(*inviteIt);
		if (!pInvitorConn) {
			continue;
		}

		pInvitorConn->SendMsg(strResponse.c_str(), strResponse.size());
	}

	return;
}

/*
	��������������
	������
	1��uPlayerId ������������Id
	2��uTeamId ��������Ķ���ID
*/
void TeamMng::AcceptJoinTeam(uint64_t uPlayerId, uint32_t uTeamId) {
	slither::AcceptJoinTeamAck acceptJoinAck;
	string strResponse;

	IConnection* pPlayerConn = clientConnMgr->getConnByPlayerId(uPlayerId);
	if (!pPlayerConn || !pPlayerConn->IsConnected()) {
		return;
	}

	Team* pTeam = GetTeam(uTeamId);
	if (!pTeam) {
		DEBUGLOG("cannot find team id=[" << uTeamId << "]");

		acceptJoinAck.set_errorcode(slither::TEAM_NOT_EXIST);

		cputil::BuildResponseProto(acceptJoinAck, strResponse, slither::REQ_ACCEPT_JOIN_TEAM);
		pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	// �жϷ����Ƿ�����
	if (pTeam->IsFull()) {
		TRACELOG("player id=[" << uPlayerId << "] cannot join team id=[" << uTeamId << "] is full");

		acceptJoinAck.set_errorcode(slither::TEAM_IS_FULL);

		cputil::BuildResponseProto(acceptJoinAck, strResponse, slither::REQ_ACCEPT_JOIN_TEAM);
		pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	// ����Ҽ������
	pTeam->AddMember(uPlayerId);

	// ֪ͨ������ң�����Ҽ��������
	slither::MemberJoinTeamNty memberJoinNty;
	memberJoinNty.mutable_member()->set_playerid(uPlayerId);
	cputil::BuildResponseProto(memberJoinNty, strResponse, slither::NTY_MEMBER_JOIN_TEAM);

	const list<uint64_t>& memberList = pTeam->GetMemberList();
	list<uint64_t>::const_iterator memberIt = memberList.begin();
	list<uint64_t>::const_iterator memberItEnd = memberList.end();
	for (; memberIt != memberItEnd; memberIt++) {
		if (*memberIt == uPlayerId) {
			continue;
		}

		IConnection* pMemberConn = clientConnMgr->getConnByPlayerId(*memberIt);
		if (!pMemberConn || !pMemberConn->IsConnected()) {
			continue;
		}

		strResponse.clear();
		pMemberConn->SendMsg(strResponse.c_str(), strResponse.size());
	}

	// ֪ͨ�������Ҷ�����Ϣ
	pTeam->SerializeToPB(*acceptJoinAck.mutable_teaminfo());

	strResponse.clear();
	cputil::BuildResponseProto(memberJoinNty, strResponse, slither::REQ_ACCEPT_JOIN_TEAM);
	pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());

	return;
}

/*
	�������ܾ�����
	������
	1��uPlayerId �ܾ���������Id
	2��uTeamId	 ���ܾ�����Ķ���ID
*/
void TeamMng::RefuseJoinTeam(uint64_t uPlayerId, uint32_t uTeamId) {

	slither::RefuseJoinTeamAck refuseJoinAck;
	string strResponse;

	IConnection* pPlayerConn = clientConnMgr->getConnByPlayerId(uPlayerId);
	if (!pPlayerConn || !pPlayerConn->IsConnected()) {
		return;
	}

	Team* pTeam = GetTeam(uTeamId);
	if (!pTeam) {
		DEBUGLOG("cannot find team id=[" << uTeamId << "]");

		refuseJoinAck.set_errorcode(slither::TEAM_NOT_EXIST);

		cputil::BuildResponseProto(refuseJoinAck, strResponse, slither::REQ_REFUSE_JOIN_TEAM);
		pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	// ֪ͨ��Ҿܾ��ɹ�
	cputil::BuildResponseProto(refuseJoinAck, strResponse, slither::REQ_REFUSE_JOIN_TEAM);
	pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());

	return;
}

/*
	�������߳������Ա
	������
	1��uKickerId �����߳������ID
	2��uMemberId ���߳������ID
*/
void TeamMng::KickTeamMember(uint64_t uKickerId, uint64_t uMemberId) {

	slither::KickTeamMemberAck kickMemberAck;
	string strResponse;

	IConnection* pPlayerConn = clientConnMgr->getConnByPlayerId(uKickerId);
	if (!pPlayerConn || !pPlayerConn->IsConnected()) {
		return;
	}

	Team* pKickerTeam = GetTeamByPlayer(uKickerId);
	Team* pMemberTeam = GetTeamByPlayer(uMemberId);
	if (!pKickerTeam || !pMemberTeam) {
		DEBUGLOG("cannot find kicker id=[" << uKickerId << "] team, or member id=[" << uMemberId << "] team");
		kickMemberAck.set_errorcode(slither::NOT_IN_TEAM);

		cputil::BuildResponseProto(kickMemberAck, strResponse, slither::REQ_KICK_TEAM_MEMBER);
		pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	// ��������˲���һ�����飬��ôû��Ȩ���߳�
	if (pKickerTeam != pMemberTeam) {
		DEBUGLOG("kicker id=[" << uKickerId << "] team is diffrent from member id=[" << uMemberId << "] team");

		kickMemberAck.set_errorcode(slither::NOT_TEAM_CAPTAIN);

		cputil::BuildResponseProto(kickMemberAck, strResponse, slither::REQ_KICK_TEAM_MEMBER);
		pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	// �ж����˵��ǲ��Ƕӳ������Ƕӳ�ҲûȨ��
	if (uKickerId != pKickerTeam->GetCaptain()) {
		DEBUGLOG("kicker id=[" << uKickerId << "] is not team id=[" << pKickerTeam->GetTeamId() << "] captain");

		kickMemberAck.set_errorcode(slither::NOT_TEAM_CAPTAIN);

		cputil::BuildResponseProto(kickMemberAck, strResponse, slither::REQ_KICK_TEAM_MEMBER);
		pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	// ��Ҳ����߳��Լ�
	if (uKickerId == uMemberId) {
		DEBUGLOG("cannot kick out self id=[" << uKickerId << "]");

		kickMemberAck.set_errorcode(slither::CAN_NOT_KICK_SELF);

		cputil::BuildResponseProto(kickMemberAck, strResponse, slither::REQ_KICK_TEAM_MEMBER);
		pPlayerConn->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	// ֪ͨ�����г�Ա���ж�Ա���߳��ˣ��������߳��Ķ�Ա�Լ�
	slither::KickTeamMemberNty kickMemberNty;
	kickMemberNty.set_playerid(uMemberId);
	cputil::BuildResponseProto(kickMemberNty, strResponse, slither::NTY_KICK_TEAM_MEMBER);

	const list<uint64_t>& memberList = pKickerTeam->GetMemberList();
	list<uint64_t>::const_iterator memberIt = memberList.begin();
	list<uint64_t>::const_iterator memberItEnd = memberList.end();
	for (; memberIt != memberItEnd; memberIt++) {
		IConnection* pMemberConn = clientConnMgr->getConnByPlayerId(*memberIt);
		if (!pMemberConn || !pMemberConn->IsConnected()) {
			continue;
		}

		strResponse.clear();
		pMemberConn->SendMsg(strResponse.c_str(), strResponse.size());
	}

	// ����ҴӶ������Ƴ�
	pKickerTeam->KickMember(uMemberId);

	return;
}

/*
	��������ʼ�Ŷ���Ϸ
	������
	1��uPlayerId ��ʼ�Ŷ���Ϸ�����ID
*/
void TeamMng::StartTeamGame(uint64_t uPlayerId) {

}

void TeamMng::AddPlayerIntoTeam(uint64_t uPlayerId, uint32_t uTeamId) {
	m_playerTeamMap[uPlayerId] = uTeamId;
}

void TeamMng::DelPlayerFromTeam(uint64_t uPlayerId) {
	m_playerTeamMap.erase(uPlayerId);
}

Team* TeamMng::GetTeamByPlayer(uint64_t uPlayerId) {
	map<uint64_t, uint32_t>::iterator teamIdIt = m_playerTeamMap.find(uPlayerId);
	if (teamIdIt == m_playerTeamMap.end()) {
		return NULL;
	}

	map<uint32_t, Team*>::iterator teamIt = m_teamMap.find(teamIdIt->second);
	if (teamIt == m_teamMap.end()) {
		return NULL;
	}

	return teamIt->second;
}

Team* TeamMng::GetTeam(uint32_t uTeamId) {
	map<uint32_t, Team*>::iterator teamIt = m_teamMap.find(uTeamId);
	if (teamIt == m_teamMap.end()) {
		return NULL;
	}

	return teamIt->second;
}

void TeamMng::AddTeam(Team* pNewTeam) {
	if (!pNewTeam) {
		return;
	}
	m_teamMap.insert(make_pair(pNewTeam->GetTeamId(), pNewTeam));
	return;
}

void TeamMng::DelTeam(Team* pTeam) {
	if (!pTeam) {
		return;
	}

	m_teamMap.erase(pTeam->GetTeamId());
	return;
}

void TeamMng::AddCanInvitePlayer(uint64_t uPlayerId) {
	m_canInviteSet.insert(uPlayerId);
}

set<uint64_t> TeamMng::GetCanInvitePlayer() {
	return m_canInviteSet;
}