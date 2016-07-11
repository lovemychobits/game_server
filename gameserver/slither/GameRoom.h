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

	// ��Ϸÿ����Ϣ
	struct GameRoundStat {
		string startTime;								// ���俪ʼʱ��
		uint32_t uRoomFullTime;							// ������Աʱ��
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
		void End();										// �������
		bool IsClosed() {								// �Ƿ�ر��� 
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

		uint32_t GetLeftTime();							// ��ȡʣ��ʱ��
		uint32_t GetPassTime();							// ��ȡ�Ѿ����ŵ�ʱ��
		void Clear();									// ��������������
		void ClearSnakes();								// �������е���
		void PlayerLeaveRoom(uint64_t uPlayerId);		// ����뿪����

		void Enter(Snake* pSnake);

	public:
		Scene* GetScene() {
			return m_pScene;
		}

		int32_t GetSkinId(uint64_t uPlayerId);								// ��ȡ��Ҷ�Ӧ��ID
		void SetSkinId(uint64_t uPlayerId, uint32_t uSkinId);				// ����SkinId

	protected:
		Scene* m_pScene;											// ��Ӧ�ĳ���
		uint32_t m_uRoundTime;										// һ��ʱ��
		uint32_t m_uMaxPlayer;										// ÿ���������
		uint32_t m_uRoomId;											// ����ID
		boost::posix_time::ptime m_startTime;						// ���俪ʼʱ��
		bool m_bClosed;												// �ر���
		bool m_bStart;												// �Ƿ�ʼ��
		uint32_t m_uRefuseEnterTime;								// ʣ��೤ʱ���ֹ����
		set<uint32_t> m_snakeSet;									// �ߵ��б�
		map<uint64_t, int32_t> m_playerSkinMap;						// ���Skin Map
		map<uint64_t, SnakeStatistics> m_snakeStatisticsMap;		// ���ID��Ӧ���ߵ�ͳ����Ϣmap
		GameRoundStat m_gameRoundStat;								// ÿ��ͳ����Ϣ
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

		bool InitRooms(uint32_t uRoomNum);																								// ��ʼ��������Ϣ
		void EnterGame(IConnection* pConn, uint64_t uUserId, const string& strNickName, uint32_t uSkinId, bool gender, const string& strPlayerName, uint32_t uRoomId=1);			// ��ҽ�����Ϸ

	public:
		void DeleteSnakeGameRoom(uint32_t uSnakeId);						// ɾ���ߵķ����ϵ
		void PlayerLeaveRoom(uint64_t uPlayerId);							// ����뿪����
		const map<uint32_t, GameRoom*>& GetRooms();							// ��ȡ�����б�

	private:
		GameRoom* CreateGameRoom(uint32_t uGameTime, uint32_t uMaxPlayer, uint32_t uRoomId, uint32_t uRefuseEnterTime);			// ������Ϸ����
		GameRoom* CreateTeamRoom(uint32_t uGameTime, uint32_t uMaxPlayer, uint32_t uRoomId, uint32_t uRefuseEnterTime);			// �����Ŷӷ���
		void ReleaseGameRoom(GameRoom* pGameRoom);							// �ͷ���Ϸ����
		bool HasInGame(uint32_t uSnakeId);									// �Ƿ��Ѿ�����Ϸ����
		GameRoom* GetGameRoom(uint32_t uSnakeId);							// ��ȡ��Ϸ����
		GameRoom* GetRoom(uint32_t uRoomId);								// ���ݷ���ID��ȡ��Ϸ����
		bool HasSnake(uint64_t uUserId);									// ������Ƿ�����
		uint32_t CalcRoomId(uint32_t uGsId, uint32_t uRoomId);				// ���㷿��ID

	private:
		map<uint32_t, GameRoom*> m_playerGameRoundMap;						// ������ڵ���Ϸ�����б�
		map<uint32_t, GameRoom*> m_gameRoomList;							// ��Ϸ�����б�
		uint32_t m_uSnakeId;												// �ߵ�ID
		map<uint64_t, uint32_t> m_playerSnakeMap;							// ���ID��Ӧ��ID�ı�
		uint32_t m_uRoomId;													// ����ID
	};

#define gpGameRoomMng GameRoomMng::GetInstance()
}


#endif