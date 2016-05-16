#ifndef GAMESERVER_SLITHER_PLAYERMNG_H
#define GAMESERVER_SLITHER_PLAYERMNG_H

#include "../header.h"
#include "../../network/IConnection.h"
#include <map>
using namespace std;

namespace slither {
	class Snake;

	class PlayerMng {
	public:
		PlayerMng();
		~PlayerMng();
		static PlayerMng* GetInstance() {
			static PlayerMng instance;
			return &instance;
		}

		void SetPlayerConn(Snake* pPlayerSnake, IConnection* pConn);
		void PlayerDisconnect(IConnection* pConn);
		Snake* GetPlayerSnake(IConnection* pConn);
		void DeleteSnake(Snake* pSnake);

	private:
		map<IConnection*, Snake*> m_playerConnMap;
	};

#define gpPlayerMng PlayerMng::GetInstance()
}

#endif