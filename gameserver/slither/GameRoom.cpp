#include "GameRoom.h"
#include "Scene.h"
#include "Snake.h"
#include "../config/SlitherConfig.h"
#include "Factory.h"
#include "PlayerMng.h"
#include "../../protocol/slither_server.pb.h"
#include "../../protocol/slither_battle.pb.h"

namespace slither {
	GameRoom::GameRoom() : m_pScene(NULL), m_uRoundTime(0), m_uMaxPlayer(0) {
		m_startTime = boost::posix_time::second_clock::local_time();
		Start();
	}

	GameRoom::GameRoom(uint32_t uRoundTime, uint32_t uMaxPlayer) : m_pScene(NULL), m_uRoundTime(uRoundTime), m_uMaxPlayer(uMaxPlayer) {
		m_startTime = boost::posix_time::microsec_clock::local_time();
		Start();
	}

	GameRoom::~GameRoom() {

	}

	void GameRoom::OnTimer() {
		boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff = end - m_startTime;
		//if (diff.total_seconds() > m_uRoundTime * 60) {				// 到了结束时间了
		//	End();
		//	return;
		//}
		m_pScene->OnTimer();
	}

	bool GameRoom::Start() {
		m_pScene = new Scene();
		if (!m_pScene) {
			return false;
		}

		m_pScene->Init(slither::gpSlitherConf->m_uMapLength, slither::gpSlitherConf->m_uMapLength, slither::gpSlitherConf->m_uGridSize);
		m_pScene->TestScene();
		return true;
	}

	void GameRoom::End() {

		const map<uint32_t, Snake*>&  snakeMap = m_pScene->GetSnakeMap();
		slither::GameRoomEndNty gameRoomEndNty;

		map<uint32_t, Snake*>::const_iterator snakeIt = snakeMap.begin();
		map<uint32_t, Snake*>::const_iterator snakeItEnd = snakeMap.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			const Snake* pSnake = snakeIt->second;
			slither::PBPlayerGameRound* pPBPlayer = gameRoomEndNty.add_gameroundinfo();
			pPBPlayer->set_rank(pSnake->GetStatistics().uRank);
			pPBPlayer->set_killsnaketimes(pSnake->GetStatistics().uKillSnakeTimes);
			pPBPlayer->set_totaleatnum(pSnake->GetStatistics().uEatNum);
			pPBPlayer->set_maxlength(pSnake->GetStatistics().uMaxLength);

			// 通知每个玩家
			slither::NotifyGameOverInfo gameOverNty;
			gameOverNty.mutable_gameroundinfo()->CopyFrom(*pPBPlayer);
			
			// 发送给客户端
			string strResponse;
			cputil::BuildResponseProto(gameOverNty, strResponse, slither::NOTIFY_GAMEOVER_INFO);
			(const_cast<Snake*>(pSnake))->SendMsg(strResponse.c_str(), strResponse.size());
		}

		// 通知lobbyserver
		string strResponse;
		cputil::BuildResponseProto(gameRoomEndNty, strResponse, slither::NTY_GAMEROOM_END);
		if (g_pLobbySession) {
			g_pLobbySession->SendMsg(strResponse.c_str(), strResponse.size());
		}

		m_pScene->Finish();
	}

	void GameRoom::Enter(Snake* pSnake) {
		if (!m_pScene) {
			return;
		}

		m_pScene->PlayerEnter(pSnake);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GameRoomMng::GameRoomMng() : m_uSnakeId(10000) {

	}

	GameRoomMng::~GameRoomMng() {

	}

	void GameRoomMng::OnTimer(const boost::system::error_code&) {
		list<GameRoom*>::iterator roomIt = m_gameRoomList.begin();
		list<GameRoom*>::iterator roomItEnd = m_gameRoomList.end();
		for (; roomIt != roomItEnd; roomIt++) {
			GameRoom* pGameRoom = *roomIt;
			if (!pGameRoom) {
				continue;
			}
			pGameRoom->OnTimer();
		}
	}

	void GameRoomMng::EnterGame(IConnection* pConn, uint64_t uUserId, const string& strNickName, uint32_t uSkinId) {
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
			pGameRoom = GetAvailabeRoom();
		}

		Snake* pSnake = pGameRoom->GetScene()->GetSnakeById(uSnakeId);
		if (!pSnake || pSnake->GetStatus() == OBJ_DESTROY) {								// 蛇不存在，或者已经死亡了
			Vector2D pos = pGameRoom->GetScene()->GenSnakeBornPos();
			pSnake = gpFactory->CreateSnake(pGameRoom->GetScene(), uSnakeId, pos);
			if (!pSnake) {
				ERRORLOG("create snake failed, snake id=[" << uSnakeId << "]");
				return;
			}

			pSnake->SetScene(pGameRoom->GetScene());
		}

		pSnake->SetConnection(pConn);
		gpPlayerMng->SetPlayerConn(pSnake, pConn);
		
		pSnake->SetNickName(strNickName);
		pSnake->SetSkinId(uSkinId);
		pGameRoom->Enter(pSnake);

		// 添加到列表中
		m_playerGameRoundMap.insert(make_pair(uSnakeId, pGameRoom));
		m_playerSnakeMap.insert(make_pair(uUserId, uSnakeId));
	}

	GameRoom* GameRoomMng::CreateGameRoom(uint32_t uGameTime, uint32_t uMaxPlayer) {
		GameRoom* pGameRoom = new GameRoom(uGameTime, uMaxPlayer);
		return pGameRoom;
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

	GameRoom* GameRoomMng::GetAvailabeRoom() {
		if (m_gameRoomList.empty()) {						// 如果一个游戏房间都没有
			GameRoom* pRoom = CreateGameRoom(gpSlitherConf->m_uGameRoundTime, 10000);
			m_gameRoomList.push_back(pRoom);
			return pRoom;
		}

		list<GameRoom*>::iterator roomIt = m_gameRoomList.begin();
		return *roomIt;
	}

	bool GameRoomMng::HasSnake(uint64_t uUserId) {
		map<uint64_t, uint32_t>::iterator snakeIt = m_playerSnakeMap.find(uUserId);
		if (snakeIt == m_playerSnakeMap.end()) {
			return false;
		}

		return true;
	}
}