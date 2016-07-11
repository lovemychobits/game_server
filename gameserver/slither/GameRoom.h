#ifndef GAMESERVER_GAMEROUND_H
#define GAMESERVER_GAMEROUND_H

#include "../header.h"
#include "Object.h"
#include <map>
#include <list>
using namespace std;

namespace slither {

	class Snake;
	class Scene;

	// 游戏每局信息
	struct GameRoundStat {
		string startTime;								// 房间开始时间
		uint32_t uRoomFullTime;							// 房间满员时间
		GameRoundStat() : uRoomFullTime(0) {
		}
	};

	class GameRoom {
	public:
		GameRoom();
		GameRoom(uint32_t uRoundTime, uint32_t uMaxPlayer, uint32_t uRoomId, uint32_t uRefuseEnterTime);
		virtual ~GameRoom();

		void OnTimer();
		
		virtual bool Start();
		void End();										// 房间结束
		bool IsClosed() {								// 是否关闭了 
			return m_bClosed;
		}

		bool HasStart() {
			return m_bStart;
		}

		uint32_t GetRoomId() {
			return m_uRoomId;
		}

		uint32_t GetCurPlayerNum() {
			return m_snakeSet.size();
		}

		uint32_t GetTotalTime() {
			return m_uRoundTime * 60;
		}

		uint32_t GetMaxPlayer() {
			return m_uMaxPlayer;
		}

		uint32_t GetRefuseEnterTime() {
			return m_uRefuseEnterTime * 60;
		}

		void BoundSnakeStatistics(Snake* pSnake);

		uint32_t GetLeftTime();							// 获取剩余时间
		uint32_t GetPassTime();							// 获取已经流逝的时间
		void Clear();									// 房间结束后的清理
		void ClearSnakes();								// 清理房间中的蛇
		void PlayerLeaveRoom(uint64_t uPlayerId);		// 玩家离开房间

		void Enter(Snake* pSnake);

	public:
		Scene* GetScene() {
			return m_pScene;
		}

		int32_t GetSkinId(uint64_t uPlayerId);								// 获取玩家对应的ID
		void SetSkinId(uint64_t uPlayerId, uint32_t uSkinId);				// 设置SkinId

	protected:
		Scene* m_pScene;											// 对应的场景
		uint32_t m_uRoundTime;										// 一局时间
		uint32_t m_uMaxPlayer;										// 每局最多人数
		uint32_t m_uRoomId;											// 房间ID
		boost::posix_time::ptime m_startTime;						// 房间开始时间
		bool m_bClosed;												// 关闭了
		bool m_bStart;												// 是否开始了
		uint32_t m_uRefuseEnterTime;								// 剩余多长时间禁止进入
		set<uint32_t> m_snakeSet;									// 蛇的列表
		map<uint64_t, int32_t> m_playerSkinMap;						// 玩家Skin Map
		map<uint64_t, SnakeStatistics> m_snakeStatisticsMap;		// 玩家ID对应的蛇的统计信息map
		GameRoundStat m_gameRoundStat;								// 每局统计信息
	};

	class TeamRoom : public GameRoom {
	public:
		TeamRoom();
		TeamRoom(uint32_t uRoundTime, uint32_t uMaxPlayer, uint32_t uRoomId, uint32_t uRefuseEnterTime);
		virtual ~TeamRoom();

		virtual bool Start();

	private:
		// todo
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

		bool InitRooms(uint32_t uRoomNum);																								// 初始化房间信息
		void EnterGame(IConnection* pConn, uint64_t uUserId, const string& strNickName, uint32_t uSkinId, bool gender, const string& strPlayerName, uint32_t uRoomId=1);			// 玩家进入游戏

	public:
		void DeleteSnakeGameRoom(uint32_t uSnakeId);						// 删除蛇的房间关系
		void PlayerLeaveRoom(uint64_t uPlayerId);							// 玩家离开房间
		const map<uint32_t, GameRoom*>& GetRooms();							// 获取房间列表

	private:
		GameRoom* CreateGameRoom(uint32_t uGameTime, uint32_t uMaxPlayer, uint32_t uRoomId, uint32_t uRefuseEnterTime);			// 创建游戏房间
		GameRoom* CreateTeamRoom(uint32_t uGameTime, uint32_t uMaxPlayer, uint32_t uRoomId, uint32_t uRefuseEnterTime);			// 创建团队房间
		void ReleaseGameRoom(GameRoom* pGameRoom);							// 释放游戏房间
		bool HasInGame(uint32_t uSnakeId);									// 是否已经在游戏中了
		GameRoom* GetGameRoom(uint32_t uSnakeId);							// 获取游戏房间
		GameRoom* GetRoom(uint32_t uRoomId);								// 根据房间ID获取游戏房间
		bool HasSnake(uint64_t uUserId);									// 此玩家是否有蛇
		uint32_t CalcRoomId(uint32_t uGsId, uint32_t uRoomId);				// 计算房间ID

	private:
		map<uint32_t, GameRoom*> m_playerGameRoundMap;						// 玩家所在的游戏房间列表
		map<uint32_t, GameRoom*> m_gameRoomList;							// 游戏房间列表
		uint32_t m_uSnakeId;												// 蛇的ID
		map<uint64_t, uint32_t> m_playerSnakeMap;							// 玩家ID对应蛇ID的表
		uint32_t m_uRoomId;													// 房间ID
	};

#define gpGameRoomMng GameRoomMng::GetInstance()
}


#endif