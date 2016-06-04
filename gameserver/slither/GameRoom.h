#ifndef GAMESERVER_GAMEROUND_H
#define GAMESERVER_GAMEROUND_H

#include "../header.h"
#include <map>
#include <list>
using namespace std;

namespace slither {

	class Snake;
	class Scene;

	class GameRoom {
	public:
		GameRoom();
		GameRoom(uint32_t uRoundTime, uint32_t uMaxPlayer);
		~GameRoom();

		void OnTimer();
		
		bool Start();
		void End();										// 房间结束
		bool IsClosed() {								// 是否关闭了 
			return m_bClosed;
		}

		uint32_t GetLeftTime();							// 获取剩余时间
		void ClearSnakes();								// 清理房间中的蛇

		void Enter(Snake* pSnake);

	public:
		Scene* GetScene() {
			return m_pScene;
		}

	private:
		Scene* m_pScene;								// 对应的场景
		uint32_t m_uRoundTime;							// 一局时间
		uint32_t m_uMaxPlayer;							// 每局最多人数
		boost::posix_time::ptime m_startTime;
		bool m_bClosed;									// 关闭了
		set<uint32_t> m_snakeSet;						// 蛇的列表
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class GameRoomMng {
	public:
		GameRoomMng();
		~GameRoomMng();

		static GameRoomMng* GetInstance() {
			static GameRoomMng instance;
			return &instance;
		}

		void OnTimer(const boost::system::error_code&);

		void EnterGame(IConnection* pConn, uint64_t uUserId, const string& strNickName, uint32_t uSkinId);				// 玩家进入游戏

	public:
		void DeleteSnakeGameRoom(uint32_t uSnakeId);						// 删除蛇的房间关系

	private:
		GameRoom* CreateGameRoom(uint32_t uGameTime, uint32_t uMaxPlayer);	// 创建游戏房间
		void ReleaseGameRoom(GameRoom* pGameRoom);							// 释放游戏房间
		bool HasInGame(uint32_t uSnakeId);									// 是否已经在游戏中了
		GameRoom* GetGameRoom(uint32_t uSnakeId);							// 获取游戏房间
		GameRoom* GetAvailabeRoom();										// 获取可以加入的游戏房间
		bool HasSnake(uint64_t uUserId);									// 此玩家是否有蛇

	private:
		map<uint32_t, GameRoom*> m_playerGameRoundMap;						// 玩家所在的游戏房间列表
		list<GameRoom*> m_gameRoomList;										// 游戏房间列表
		uint32_t m_uSnakeId;												// 蛇的ID
		map<uint64_t, uint32_t> m_playerSnakeMap;							// 玩家ID对应蛇ID的表
	};

#define gpGameRoomMng GameRoomMng::GetInstance()
}


#endif