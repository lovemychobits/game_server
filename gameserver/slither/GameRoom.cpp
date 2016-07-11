#include "GameRoom.h"
#include "Scene.h"
#include "TeamScene.h"
#include "Snake.h"
#include "../config/SlitherConfig.h"
#include "../config/ServerConfig.h"
#include "Factory.h"
#include "PlayerMng.h"
#include "../../protocol/slither_server.pb.h"
#include "../../protocol/slither_battle.pb.h"

namespace slither {
	GameRoom::GameRoom() : m_pScene(NULL), m_uRoundTime(0), m_uMaxPlayer(0), m_bClosed(false), m_bStart(false), m_uRefuseEnterTime(0) {
	}

	GameRoom::GameRoom(uint32_t uRoundTime, uint32_t uMaxPlayer, uint32_t uRoomId, uint32_t uRefuseEnterTime) : 
		m_pScene(NULL), m_uRoundTime(uRoundTime), m_uMaxPlayer(uMaxPlayer), m_uRoomId(uRoomId), m_bClosed(false), m_bStart(false), m_uRefuseEnterTime(uRefuseEnterTime) {
		m_startTime = boost::posix_time::microsec_clock::local_time();
	}

	GameRoom::~GameRoom() {

	}

	void GameRoom::OnTimer() {
		boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff = end - m_startTime;
		if (diff.total_seconds() >= (int)m_uRoundTime * 60) {				// 到了结束时间了
			End();
			Clear();
			m_bClosed = true;
			m_bStart = false;
			cout << "game room close" << endl;
			TRACELOG("game room Id=[0x" << hex << m_uRoomId << "] close");
			return;
		}
		m_pScene->OnTimer();
	}

	uint32_t GameRoom::GetLeftTime() {
		if (m_bStart == false) {									// 如果房间还没开始
			return m_uRoundTime * 60;
		}
		boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff = end - m_startTime;

		if (m_uRoundTime * 60 > (uint32_t)diff.total_seconds()) {
			return m_uRoundTime * 60 - (uint32_t)diff.total_seconds();
		}
		 
		return 0;
	}

	uint32_t GameRoom::GetPassTime() {
		if (m_bStart == false) {
			return 0;
		}

		boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff = end - m_startTime;

		return diff.total_seconds();
	}

	void GameRoom::BoundSnakeStatistics(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		uint64_t uPlayerId = pSnake->GetPlayerId();
		if (uPlayerId == 0) {
			ERRORLOG("playerid = 0 is error");
			return;
		}

		map<uint64_t, SnakeStatistics>::iterator statIt = m_snakeStatisticsMap.find(uPlayerId);
		if (statIt == m_snakeStatisticsMap.end()) {
			SnakeStatistics statistics(pSnake->GetGender(), pSnake->GetPlayerName(), GetPassTime());
			statistics.uSkinId = pSnake->GetSkinId();

			m_snakeStatisticsMap.insert(make_pair(uPlayerId, statistics));
			statIt = m_snakeStatisticsMap.find(uPlayerId);
			if (statIt == m_snakeStatisticsMap.end()) {
				ERRORLOG("statIt is invalid");
				return;
			}
		}
		pSnake->SetStatistics(&(statIt->second));
		return;
	}

	void GameRoom::ClearSnakes() {
		set<uint32_t>::iterator snakeIt = m_snakeSet.begin();
		set<uint32_t>::iterator snakeItEnd = m_snakeSet.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			// 将玩家的房间关系删除
			gpGameRoomMng->DeleteSnakeGameRoom(*snakeIt);
		}
	}

	void GameRoom::PlayerLeaveRoom(uint64_t uPlayerId) {
		map<uint64_t, SnakeStatistics>::iterator statIt = m_snakeStatisticsMap.find(uPlayerId);
		if (statIt == m_snakeStatisticsMap.end()) {
			return;
		}

		SnakeStatistics& snakeStatistics = statIt->second;
		snakeStatistics.uLeaveRoomTime = GetPassTime();
		TRACELOG("player id=[" << uPlayerId << "] leave roomId=[0x" << hex << m_uRoomId << "]");
	}

	void GameRoom::Clear() {
		m_playerSkinMap.clear();
		m_snakeStatisticsMap.clear();
		ClearSnakes();
		if (m_pScene)
		{
			delete m_pScene;
		}
	}

	bool GameRoom::Start() {
		m_startTime = boost::posix_time::second_clock::local_time();
		
		// 开始时间
		m_gameRoundStat.startTime = boost::posix_time::to_iso_extended_string(m_startTime);
		m_gameRoundStat.startTime[10] = ' ';

		m_pScene = new Scene(this);
		if (!m_pScene) {
			ERRORLOG("create scene failed");
			return false;
		}

		m_pScene->Init(slither::g_slitherConfig.m_uMapLength, slither::g_slitherConfig.m_uMapLength, slither::g_slitherConfig.m_uGridSize);
		m_pScene->InitSceneFoods();
		m_bStart = true;
		m_bClosed = false;
		return true;
	}

	void GameRoom::End() {
		TRACELOG("game roomId=[0x" << hex << m_uRoomId << "] is end");

		// 场景结束
		m_pScene->Finish();

		uint32_t uValidNum = 0;
		slither::GameRoomEndNty gameRoomEndNty;

		map<uint64_t, SnakeStatistics>::const_iterator statIt = m_snakeStatisticsMap.begin();
		map<uint64_t, SnakeStatistics>::const_iterator statItEnd = m_snakeStatisticsMap.end();
		for (; statIt != statItEnd; statIt++) {
			const SnakeStatistics& statistics = statIt->second;
			slither::PBPlayerGameRound* pPBPlayer = gameRoomEndNty.add_gameroundinfo();
			pPBPlayer->set_playerid(statIt->first);
			pPBPlayer->set_playername(statistics.strPlayerName);
			pPBPlayer->set_gender(statistics.gender);

			Snake* pSnake = m_pScene->GetSnakeByPlayerId(statIt->first);
			if (pSnake) {									// 没有找到蛇，说明是离线的
				pPBPlayer->set_gameendstatus(1);			// 在线设置为1，不在线默认为0
				pPBPlayer->set_rank(statistics.uRank);
				if (statistics.uRank == 0) {				// 如果排名是0，也按最后一名算
					pPBPlayer->set_rank(m_pScene->GetGameEndLastRank() + 1);
				}
				pPBPlayer->set_gameendmass((uint32_t)pSnake->GetTotalMass());

				++uValidNum;
			}	
			else {
				pPBPlayer->set_rank(m_pScene->GetGameEndLastRank() + 1);					// 如果玩家已经不在线了，那么它的排名是最后排名+1
			}
			
			pPBPlayer->set_killsnaketimes(statistics.uKillSnakeTimes);
			pPBPlayer->set_totaleatnum(statistics.uEatNum);
			pPBPlayer->set_maxmass(statistics.uMaxLength);
			pPBPlayer->set_deadtimes(statistics.uDeadTimes);
			pPBPlayer->set_speedupcost((uint32_t)statistics.fSpeedUpCost);
			pPBPlayer->set_skinid(statistics.uSkinId);
			pPBPlayer->set_enterroomtime(statistics.uEnterRoomTime);
			pPBPlayer->set_leavetime(statistics.uLeaveRoomTime);

			// 通知每个玩家
			slither::NotifyGameOverInfo gameOverNty;
			gameOverNty.mutable_gameroundinfo()->CopyFrom(*pPBPlayer);
			
			// 发送给客户端
			if (pSnake) {
				string strResponse;
				cputil::BuildResponseProto(gameOverNty, strResponse, slither::NOTIFY_GAMEOVER_INFO);
				(pSnake)->SendMsg(strResponse.c_str(), strResponse.size());
			}
			else {
				TRACELOG("snake not found playerId = [" << statIt->first << "]");
			}
		}

		// 设置房间信息
		gameRoomEndNty.mutable_gameround()->set_gameroomid(m_uRoomId);
		gameRoomEndNty.mutable_gameround()->set_starttime(m_gameRoundStat.startTime);
		gameRoomEndNty.mutable_gameround()->set_roomfulltime(m_gameRoundStat.uRoomFullTime);
		gameRoomEndNty.mutable_gameround()->set_maxenternum(m_snakeStatisticsMap.size());
		gameRoomEndNty.mutable_gameround()->set_roundendnum(uValidNum);

		// 通知lobbyserver
		gameRoomEndNty.set_roomid(m_uRoomId);
		if (m_snakeStatisticsMap.size() < g_slitherConfig.m_uInvalidNum) {
			gameRoomEndNty.set_isvalid(false);
		}
		else {
			gameRoomEndNty.set_isvalid(true);
		}
		string strResponse;
		cputil::BuildResponseProto(gameRoomEndNty, strResponse, slither::NTY_GAMEROOM_END);
		if (g_pLobbySession) {
			g_pLobbySession->SendMsg(strResponse.c_str(), strResponse.size());
		}

		return;
	}

	void GameRoom::Enter(Snake* pSnake) {
		if (!m_pScene) {
			return;
		}

		m_snakeSet.insert(pSnake->GetSnakeId());
		m_pScene->PlayerEnter(pSnake);

		if (m_snakeStatisticsMap.size() >= g_slitherConfig.m_uRoomSnakeLimit)
		{
			m_gameRoundStat.uRoomFullTime = GetPassTime();								// 记录满员时间
		}
	}

	int32_t GameRoom::GetSkinId(uint64_t uPlayerId) {
		map<uint64_t, int32_t>::iterator skinIt = m_playerSkinMap.find(uPlayerId);
		if (skinIt == m_playerSkinMap.end()) {
			return -1;
		}

		return skinIt->second;
	}

	void GameRoom::SetSkinId(uint64_t uPlayerId, uint32_t uSkinId) {
		m_playerSkinMap[uPlayerId] = uSkinId;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TeamRoom::TeamRoom() {

	}

	TeamRoom::TeamRoom(uint32_t uRoundTime, uint32_t uMaxPlayer, uint32_t uRoomId, uint32_t uRefuseEnterTime) :
		GameRoom(uRoundTime, uMaxPlayer, uRoomId, uRefuseEnterTime) {
		m_startTime = boost::posix_time::microsec_clock::local_time();
	}

	TeamRoom::~TeamRoom() {
	}

	bool TeamRoom::Start() {
		m_startTime = boost::posix_time::second_clock::local_time();

		// 开始时间
		m_gameRoundStat.startTime = boost::posix_time::to_iso_extended_string(m_startTime);
		m_gameRoundStat.startTime[10] = ' ';

		m_pScene = new TeamScene(this);
		if (!m_pScene) {
			ERRORLOG("create scene failed");
			return false;
		}

		m_pScene->Init(slither::g_slitherConfig.m_uMapLength, slither::g_slitherConfig.m_uMapLength, slither::g_slitherConfig.m_uGridSize);
		m_pScene->InitSceneFoods();
		m_bStart = true;
		m_bClosed = false;
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GameRoomMng::GameRoomMng() : m_uSnakeId(10000), m_uRoomId(1) {

	}

	GameRoomMng::~GameRoomMng() {

	}

	void GameRoomMng::OnTimer(const boost::system::error_code&) {
		map<uint32_t, GameRoom*>::iterator roomIt = m_gameRoomList.begin();
		map<uint32_t, GameRoom*>::iterator roomItEnd = m_gameRoomList.end();
		for (; roomIt != roomItEnd; ) {
			GameRoom* pGameRoom = roomIt->second;
			if (!pGameRoom) {
				continue;
			}
			if (pGameRoom->HasStart() == false) {				// 房间还没开始
				roomIt++;
				continue;
			}
			pGameRoom->OnTimer();
			roomIt++;
		}
	}

	bool GameRoomMng::InitRooms(uint32_t uRoomNum) {
		for (uint32_t i = 0; i < uRoomNum; ++i) {
			uint32_t uRoomId = CalcRoomId(gpServerConfig->GetServerId(), m_uRoomId++);
			GameRoom* pRoom = CreateTeamRoom(g_slitherConfig.m_uGameRoundTime, g_slitherConfig.m_uRoomSnakeLimit, uRoomId, g_slitherConfig.m_uRefuseEnterTime);
			m_gameRoomList.insert(make_pair(uRoomId, pRoom));

			// for test 
			// pRoom->Start();
		}

		return true;
	}

	void GameRoomMng::EnterGame(IConnection* pConn, uint64_t uUserId, const string& strNickName, uint32_t uSkinId, bool gender, const string& strPlayerName, uint32_t uRoomId) {
		if (!pConn) {
			return;
		}
		
		uint32_t uSnakeId = 0;
		map<uint64_t, uint32_t>::iterator snakeIt = m_playerSnakeMap.find(uUserId);
		if (snakeIt != m_playerSnakeMap.end()) {
			uSnakeId = snakeIt->second;
		}
		else {
			uSnakeId = m_uSnakeId++;
		}

		GameRoom* pGameRoom = GetGameRoom(uSnakeId);
		if (pGameRoom == NULL) {					// 如果目前没有在任何一个房间内
			pGameRoom = GetRoom(uRoomId);
		}

		if (!pGameRoom) {
			ERRORLOG("cannot find roomid=[0x" << hex << uRoomId << "]");
			return;
		}

		if (!pGameRoom->HasStart()) {
			pGameRoom->Start();
			TRACELOG("room id=[0x" << hex << pGameRoom->GetRoomId() << "] start");
		}

		Snake* pSnake = pGameRoom->GetScene()->GetSnakeById(uSnakeId);
		if (!pSnake ) {																				// 蛇不存在，或者已经死亡了
			Vector2D pos = pGameRoom->GetScene()->GenSnakeBornPos();
			pSnake = gpFactory->CreateSnake(pGameRoom->GetScene(), uSnakeId, pos);
			if (!pSnake) {
				ERRORLOG("create snake failed, snake id=[" << uSnakeId << "]");
				return;
			}

			pSnake->SetScene(pGameRoom->GetScene());
		}

		if (pSnake->GetStatus() == OBJ_DESTROY) {
			uSnakeId = m_uSnakeId++;

			Vector2D pos = pGameRoom->GetScene()->GenSnakeBornPos();
			pSnake = gpFactory->CreateSnake(pGameRoom->GetScene(), uSnakeId, pos);
			if (!pSnake) {
				ERRORLOG("create snake failed, snake id=[" << uSnakeId << "]");
				return;
			}

			pSnake->SetScene(pGameRoom->GetScene());
		}

		int32_t nRoomSkinId = pGameRoom->GetSkinId(uUserId);
		if (nRoomSkinId != -1) {
			pSnake->SetSkinId(nRoomSkinId);
		}
		else {
			pGameRoom->SetSkinId(uUserId, uSkinId);
			pSnake->SetSkinId(uSkinId);
		}

		// 测试使用
		pSnake->SetTeamId(1);
		pSnake->SetSkinId(1);
		////////////

		pSnake->SetNickName(strNickName);
		pSnake->SetPlayerId(uUserId);
		pSnake->SetGender(gender);
		pSnake->SetPlayerName(strPlayerName);
		pSnake->SetConnection(pConn);
		gpPlayerMng->SetPlayerConn(pSnake, pConn);
		DEBUGLOG("snake=[" << pSnake << "] enter room id=[0x" << hex << pGameRoom->GetRoomId() << dec << "], left time=[" << pGameRoom->GetLeftTime() << "] set connection=[" << pConn << "], snake id=[" << uSnakeId << "], user id=[" << uUserId << "], nickname=[" << strNickName.c_str() << "]");
		
		pGameRoom->Enter(pSnake);

		// 添加到列表中
		m_playerGameRoundMap.insert(make_pair(uSnakeId, pGameRoom));
		m_playerSnakeMap[uUserId] = uSnakeId;

		// 不再通知lobbyserver玩家进入的房间信息了，lobbyserver那边自己统计了
		return;
	}

	GameRoom* GameRoomMng::CreateGameRoom(uint32_t uGameTime, uint32_t uMaxPlayer, uint32_t uRoomId, uint32_t uRefuseEnterTime) {
		GameRoom* pGameRoom = new GameRoom(uGameTime, uMaxPlayer, uRoomId, uRefuseEnterTime);
		return pGameRoom;
	}

	GameRoom* GameRoomMng::CreateTeamRoom(uint32_t uGameTime, uint32_t uMaxPlayer, uint32_t uRoomId, uint32_t uRefuseEnterTime) {
		GameRoom* pGameRoom = new TeamRoom(uGameTime, uMaxPlayer, uRoomId, uRefuseEnterTime);
		return pGameRoom;
	}

	void GameRoomMng::ReleaseGameRoom(GameRoom* pGameRoom) {
		if (!pGameRoom) {
			return;
		}

		delete pGameRoom;
	}

	// 是否已经在游戏中了
	bool GameRoomMng::HasInGame(uint32_t uSnakeId) {
		map<uint32_t, GameRoom*>::iterator gameIt = m_playerGameRoundMap.find(uSnakeId);
		if (gameIt == m_playerGameRoundMap.end()) {
			return false;
		}

		return true;
	}

	GameRoom* GameRoomMng::GetGameRoom(uint32_t uSnakeId) {
		map<uint32_t, GameRoom*>::iterator gameIt = m_playerGameRoundMap.find(uSnakeId);
		if (gameIt == m_playerGameRoundMap.end()) {
			return NULL;
		}

		return gameIt->second;
	}

	void GameRoomMng::DeleteSnakeGameRoom(uint32_t uSnakeId) {
		m_playerGameRoundMap.erase(uSnakeId);
	}

	void GameRoomMng::PlayerLeaveRoom(uint64_t uPlayerId) {
		map<uint64_t, uint32_t>::iterator snakeIt = m_playerSnakeMap.find(uPlayerId);
		if (snakeIt == m_playerSnakeMap.end()) {
			return;
		}

		map<uint32_t, GameRoom*>::iterator roomIt = m_playerGameRoundMap.find(snakeIt->second);
		if (roomIt == m_playerGameRoundMap.end()) {
			return;
		}

		GameRoom* pGameRoom = roomIt->second;
		if (pGameRoom) {
			pGameRoom->PlayerLeaveRoom(uPlayerId);
		}
	}

	const map<uint32_t, GameRoom*>& GameRoomMng::GetRooms() {
		return m_gameRoomList;
	}

	GameRoom* GameRoomMng::GetRoom(uint32_t uRoomId) {
		map<uint32_t, GameRoom*>::iterator gameIt = m_gameRoomList.find(uRoomId);
		if (gameIt == m_gameRoomList.end()) {
			return NULL;
		}

		return gameIt->second;
	}

	bool GameRoomMng::HasSnake(uint64_t uUserId) {
		map<uint64_t, uint32_t>::iterator snakeIt = m_playerSnakeMap.find(uUserId);
		if (snakeIt == m_playerSnakeMap.end()) {
			return false;
		}

		return true;
	}

	uint32_t GameRoomMng::CalcRoomId(uint32_t uGsId, uint32_t uRoomId) {
		uint32_t uGsRoomId = (uGsId << 16) + uRoomId;
		return uGsRoomId;
	}
}