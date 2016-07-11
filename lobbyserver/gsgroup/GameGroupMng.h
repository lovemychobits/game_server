#ifndef LOBBYSERVER_GAMEGROUPMNG_H
#define LOBBYSERVER_GAMEGROUPMNG_H

#include "../header.h"
#include "../../protocol/slither_server.pb.h"
#include "../../protocol/slither_player.pb.h"
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
	string strIp;				// 区组IP
	uint32_t uPort;				// 区组端口
	bool bHasRegister;			// 已经注册过了
	GroupInfo() : uGroupId(0), uPort(0), bHasRegister(false)
	{

	}
};

struct GameServerInfo
{
	uint32_t uGSId;				// gs ID
	string strIp;				// gs的IP
	uint32_t uPort;				// gs的端口
	GameServerInfo() : uGSId(0), uPort(0) {

	}
};

struct RoomInfo
{
	uint32_t uGSId;				// 所在的gs
	uint32_t uRoomId;			// 房间ID
	uint32_t uCurPlayerNum;		// 当前玩家数
	uint32_t uLeftTime;			// 剩余时间
	bool bHasStart;				// 房间是否已经开始
	uint32_t uTotalTime;		// 总的时间
	uint32_t uMaxPlayerNum;		// 房间最高人数限制
	uint32_t uRefuseEnterTime;	// 禁止进入时间
	boost::posix_time::ptime startTime;			// 房间开始时间
	bool bStartTime;

	RoomInfo(uint32_t _uGsId, uint32_t _uRoomId, uint32_t _uCurPlayerNum, uint32_t _uLeftTime, bool _bHasStart, uint32_t _uTotalTime, uint32_t _uMaxPlayerNum, uint32_t _uRefuseEnterTime) :
		uGSId(_uGsId), uRoomId(_uRoomId), uCurPlayerNum(_uCurPlayerNum), uLeftTime(_uLeftTime), bHasStart(_bHasStart), uTotalTime(_uTotalTime), uMaxPlayerNum(_uMaxPlayerNum), uRefuseEnterTime(_uRefuseEnterTime) {
		bStartTime = false;
	}

	// 房间开始
	void Start() {
		if (!bStartTime) {
			startTime = boost::posix_time::second_clock::local_time();
			bStartTime = true;
		}
		
		bHasStart = true;
	}

	// 时间是否够
	bool IsTimeEnough() const{
		boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff = end - startTime;
		if (uLeftTime > diff.total_seconds() + uRefuseEnterTime) {
			return true;
		}

		return false;
	}

	void IncPlayerNum() {
		++uCurPlayerNum;
	}

	// 人数是否已经满了
	bool IsRoomFull() const{
		if (uCurPlayerNum < uMaxPlayerNum) {
			return true;
		}

		return false;
	}

	void Clear() {
		uCurPlayerNum = 0;
		uLeftTime = uTotalTime;
		bHasStart = false;
		bStartTime = false;
	}
};

class GameGroupMng
{
public:
	~GameGroupMng();
	static GameGroupMng* GetInstance() {
		static GameGroupMng instance;
		return &instance;
	}

	void GameServerDisconnect(IConnection* pGsConnection);
	bool RegGameServer(IConnection* pGsConnection, uint32_t uGsId, const string& strIp, uint32_t uPort);
	bool RegGameRoom(uint32_t uGsId, RoomInfo& roomInfo);
	RoomInfo GetAvailableGameRoom(uint64_t uPlayerId);					// 获取一个当前可用的游戏房间
	bool GetGameServer(uint32_t uGsId, GameServerInfo& gsInfo);
	void PlayerEnterRoom(uint64_t uPlayerId, uint32_t uRoomId);			// 玩家进入房间
	void GameRoomEnd(const slither::GameRoomEndNty& roomEndNty);		// 游戏房间结束了

private:
	GameGroupMng();

private:
	bool IsRoomOk(const RoomInfo& roomInfo);
	void DeletePlayerInRoom(uint64_t uPlayerId);
	int32_t GetAvailableActiveRoom();							// 获取一个可用的活跃房间
	void AddActiveRoom(uint32_t uRoomId);						// 增加一个活跃房间
	void DelActiveRoom(uint32_t uRoomId);						// 删除一个活跃房间

private:
	map<uint32_t, GameServerInfo> m_gsMap;						// gameserver信息map
	map<IConnection*, uint32_t> m_groupConnMap;					// gameserver连接对应ID信息
	map<uint32_t, uint32_t> m_gsRoomMap;						// gs对应的room
	map<uint32_t, RoomInfo> m_roomMap;							// 房间ID对应的房间信息
	map<uint64_t, uint32_t> m_playerRoomMap;					// 玩家对应的房间
	set<uint32_t> m_activeRoomSet;								// 当前活动的房间集合
};

#define gpGameGroupMng GameGroupMng::GetInstance()
#endif