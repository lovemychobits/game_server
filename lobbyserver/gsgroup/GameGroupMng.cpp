#include "GameGroupMng.h"
#include "../../network/IConnection.h"
#include "../../tinyxml/tinyxml.h"

GameGroupMng::GameGroupMng()
{

}

GameGroupMng::~GameGroupMng()
{
}

void GameGroupMng::GameServerDisconnect(IConnection* pGsConnection)
{
	map<IConnection*, uint32_t>::iterator connIt = m_groupConnMap.find(pGsConnection);
	if (connIt == m_groupConnMap.end())
	{
		ERRORLOG("connection=[" << pGsConnection << "] cannot find in group connection map");
		return;
	}

	uint32_t uGroupId = connIt->second;
	m_groupConnMap.erase(connIt);								// 删除对应的连接

	map<uint32_t, GameServerInfo>::iterator gsIt = m_gsMap.find(uGroupId);
	if (gsIt == m_gsMap.end())
	{
		ERRORLOG("gsId=[" << uGroupId << "] cannot find in group map");
		return;
	}
	m_gsMap.erase(gsIt);
	return;
}

// gameserver注册
bool GameGroupMng::RegGameServer(IConnection* pGsConnection, uint32_t uGsId, const string& strIp, uint32_t uPort)
{
	if (!pGsConnection)
	{
		return false;
	}

	slither::RegisterGSAck registerGsAck;
	GameServerInfo gsInfo;

	map<uint32_t, GameServerInfo>::iterator groupIt = m_gsMap.find(uGsId);
	if (groupIt != m_gsMap.end())								// 已经存在
	{
		ERRORLOG("groupid=[" << uGsId << "] already exist");
		registerGsAck.set_errorcode(slither::GS_ID_EXIST);
		goto PROC_END;
	}

	gsInfo.uGSId = uGsId;
	gsInfo.strIp = strIp;
	gsInfo.uPort = uPort;

	m_gsMap.insert(make_pair(uGsId, gsInfo));
	m_groupConnMap[pGsConnection] = uGsId;						// 保存当前的区组ID对应的连接
	
PROC_END:
	string strResponse;
	cputil::BuildResponseProto(registerGsAck, strResponse, slither::REQ_REGISTER_GAMESERVER);
	pGsConnection->SendMsg(strResponse.c_str(), strResponse.size());

	return true;
}

// 注册房间
bool GameGroupMng::RegGameRoom(uint32_t uGsId, RoomInfo& roomInfo) {
	m_gsRoomMap.insert(make_pair(uGsId, roomInfo.uRoomId));

	if (roomInfo.bHasStart) {						// 如果已经开始了，那么开始计时
		roomInfo.Start();
	}

	m_roomMap.insert(make_pair(roomInfo.uRoomId, roomInfo));

	return true;
}

// 获取可用的房间
RoomInfo GameGroupMng::GetAvailableGameRoom(uint64_t uPlayerId) {
	RoomInfo roomInfo(0, 0, 0, 0, false, 0, 0, 0);

	map<uint64_t, uint32_t>::iterator playerRoomIt = m_playerRoomMap.find(uPlayerId);
	if (playerRoomIt != m_playerRoomMap.end()) {				// 如果之前已经存在
		map<uint32_t, RoomInfo>::iterator roomIt = m_roomMap.find(playerRoomIt->second);
		if (roomIt != m_roomMap.end()) {						// 房间也是存在的
			const RoomInfo& roomInfo = roomIt->second;
			return roomInfo;
		}
	}
	else {
		// 先从活跃的房间列表中获取
		int32_t uActiveRoom = GetAvailableActiveRoom();
		if (uActiveRoom > 0) {
			map<uint32_t, RoomInfo>::iterator activeIt = m_roomMap.find(uActiveRoom);
			if (activeIt != m_roomMap.end()) {
				const RoomInfo& roomInfo = activeIt->second;

				// 如果是一个合法的房间，那么就将玩家加入到这个房间，不等gameserver的返回消息，否则在并发量比较大的时候，可能导致同一个房间超出最大人数
				if (roomInfo.uRoomId != 0) {
					PlayerEnterRoom(uPlayerId, roomInfo.uRoomId);
				}

				return roomInfo;
			}
		}

		map<uint32_t, RoomInfo>::iterator roomIt = m_roomMap.begin();
		map<uint32_t, RoomInfo>::iterator roomItEnd = m_roomMap.end();
		for (; roomIt != roomItEnd; roomIt++) {
			const RoomInfo& roomInfo = roomIt->second;
			if (IsRoomOk(roomInfo))
			{
				AddActiveRoom(roomInfo.uRoomId);					// 添加一个活跃房间

				// 如果是一个合法的房间，那么就将玩家加入到这个房间，不等gameserver的返回消息，否则在并发量比较大的时候，可能导致同一个房间超出最大人数
				if (roomInfo.uRoomId != 0) {
					PlayerEnterRoom(uPlayerId, roomInfo.uRoomId);
				}

				return roomInfo;
			}
		}
	}


	return roomInfo;
}

int32_t GameGroupMng::GetAvailableActiveRoom() {
	set<uint32_t>::iterator activeIt = m_activeRoomSet.begin();
	set<uint32_t>::iterator activeItEnd = m_activeRoomSet.end();
	for (; activeIt != activeItEnd; ) {
		map<uint32_t, RoomInfo>::iterator roomIt = m_roomMap.find(*activeIt);
		if (roomIt != m_roomMap.end()) {
			const RoomInfo& roomInfo = roomIt->second;
			if (IsRoomOk(roomInfo)) {
				return (int32_t)(roomIt->first);
			}
			else {
				m_activeRoomSet.erase(activeIt++);
			}
		}
	}

	return -1;
}

void GameGroupMng::AddActiveRoom(uint32_t uRoomId) {
	m_activeRoomSet.insert(uRoomId);
}

void GameGroupMng::DelActiveRoom(uint32_t uRoomId) {
	m_activeRoomSet.erase(uRoomId);
}

// 房间是否可用
bool GameGroupMng::IsRoomOk(const RoomInfo& roomInfo) {
	// 如果房间还没开始，是可以进入的
	if (!roomInfo.bHasStart) {
		return true;
	}

	// 查看剩余时间
	if (!roomInfo.IsTimeEnough()) {
		return false;
	}

	// 查看人数限制
	if (!roomInfo.IsRoomFull()) {
		return false;
	}

	return true;
}

bool GameGroupMng::GetGameServer(uint32_t uGsId, GameServerInfo& gsInfo) {
	map<uint32_t, GameServerInfo>::iterator gsIt = m_gsMap.find(uGsId);
	if (gsIt != m_gsMap.end()) {
		gsInfo = gsIt->second;
		return true;
	}

	return false;
}

void GameGroupMng::PlayerEnterRoom(uint64_t uPlayerId, uint32_t uRoomId) {

	// 查看玩家是否已经存在
	bool bExist = false;
	map<uint64_t, uint32_t>::iterator playerIt = m_playerRoomMap.find(uPlayerId);
	if (playerIt != m_playerRoomMap.end()) {
		bExist = true;
	}
	else {
		m_playerRoomMap[uPlayerId] = uRoomId;
	}
	
	map<uint32_t, RoomInfo>::iterator roomIt = m_roomMap.find(uRoomId);
	if (roomIt == m_roomMap.end()) {
		return;
	}

	RoomInfo& roomInfo = roomIt->second;
	roomInfo.Start();
	if (!bExist) {
		roomInfo.IncPlayerNum();
	}
	
	return;
}

void GameGroupMng::GameRoomEnd(const slither::GameRoomEndNty& roomEndNty) {
	uint32_t uRoomId = roomEndNty.roomid();

	map<uint32_t, RoomInfo>::iterator roomIt = m_roomMap.find(uRoomId);
	if (roomIt == m_roomMap.end()) {
		return;
	}

	// 清除房间信息
	RoomInfo& roomInfo = roomIt->second;
	roomInfo.Clear();
	DelActiveRoom(uRoomId);									// 删除一个活跃房间

	// 玩家对应的房间信息
	uint32_t uPlayerNum = roomEndNty.gameroundinfo_size();
	for (uint32_t i = 0; i < uPlayerNum; ++i) {
		const slither::PBPlayerGameRound& pbPlayerRound = roomEndNty.gameroundinfo(i);
		DeletePlayerInRoom(pbPlayerRound.playerid());
	}

	return;
}

void GameGroupMng::DeletePlayerInRoom(uint64_t uPlayerId) {
	m_playerRoomMap.erase(uPlayerId);
}