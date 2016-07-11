#include "PlayerMng.h" 
#include "Snake.h"
#include "GameRoom.h"

namespace slither {
	PlayerMng::PlayerMng() {

	}

	PlayerMng::~PlayerMng() {

	}

	void PlayerMng::SetPlayerConn(Snake* pPlayerSnake, IConnection* pConn) {
		if (!pPlayerSnake) {
			return;
		}
		m_snakeConnMap[pConn] = pPlayerSnake;
		m_playerConnMap[pConn] = pPlayerSnake->GetPlayerId();
	}

	void PlayerMng::PlayerDisconnect(IConnection* pConn) {
		DEBUGLOG("connection=[" << pConn << "] disconnect");
		map<IConnection*, Snake*>::iterator snakeIt = m_snakeConnMap.find(pConn);
		if (snakeIt == m_snakeConnMap.end()) {
			return;
		}

		DEBUGLOG("connection=[" << pConn << "] disconnect, snake id=[" << snakeIt->second->GetSnakeId() << "] snake addr=[" << snakeIt->second << "]");

		Snake* pSnake = snakeIt->second;
		if (pSnake) {
			pSnake->SetConnection(NULL);			
		}

		map<IConnection*, uint64_t>::iterator playerIt = m_playerConnMap.find(pConn);
		if (playerIt != m_playerConnMap.end()) {
			gpGameRoomMng->PlayerLeaveRoom(playerIt->second);
		}
		
		m_playerConnMap.erase(pConn);
		m_snakeConnMap.erase(snakeIt);
	}

	Snake* PlayerMng::GetPlayerSnake(IConnection* pConn) {
		map<IConnection*, Snake*>::iterator snakeIt = m_snakeConnMap.find(pConn);
		if (snakeIt == m_snakeConnMap.end()) {
			return NULL;
		}

		return snakeIt->second;
	}

	void PlayerMng::DeleteSnake(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		map<IConnection*, Snake*>::iterator snakeIt = m_snakeConnMap.find(pSnake->GetConnection());
		if (snakeIt == m_snakeConnMap.end()) {
			return;
		}

		m_playerConnMap.erase(snakeIt->first);
		m_snakeConnMap.erase(snakeIt);
	}
}