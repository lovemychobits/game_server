#include "PlayerMng.h" 
#include "Snake.h"

namespace slither {
	PlayerMng::PlayerMng() {

	}

	PlayerMng::~PlayerMng() {

	}

	void PlayerMng::SetPlayerConn(Snake* pPlayerSnake, IConnection* pConn) {
		m_playerConnMap[pConn] = pPlayerSnake;
	}

	void PlayerMng::PlayerDisconnect(IConnection* pConn) {
		map<IConnection*, Snake*>::iterator snakeIt = m_playerConnMap.find(pConn);
		if (snakeIt == m_playerConnMap.end()) {
			return;
		}
		snakeIt->second->SetConnection(NULL);
		m_playerConnMap.erase(snakeIt);
	}

	Snake* PlayerMng::GetPlayerSnake(IConnection* pConn) {
		map<IConnection*, Snake*>::iterator snakeIt = m_playerConnMap.find(pConn);
		if (snakeIt == m_playerConnMap.end()) {
			return NULL;
		}

		return snakeIt->second;
	}

	void PlayerMng::DeleteSnake(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		map<IConnection*, Snake*>::iterator snakeIt = m_playerConnMap.find(pSnake->GetConnection());
		if (snakeIt == m_playerConnMap.end()) {
			return;
		}

		m_playerConnMap.erase(snakeIt);
	}
}