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
		void End();										// �������
		bool IsClosed() {								// �Ƿ�ر��� 
			return m_bClosed;
		}

		uint32_t GetLeftTime();							// ��ȡʣ��ʱ��
		void ClearSnakes();								// �������е���

		void Enter(Snake* pSnake);

	public:
		Scene* GetScene() {
			return m_pScene;
		}

	private:
		Scene* m_pScene;								// ��Ӧ�ĳ���
		uint32_t m_uRoundTime;							// һ��ʱ��
		uint32_t m_uMaxPlayer;							// ÿ���������
		boost::posix_time::ptime m_startTime;
		bool m_bClosed;									// �ر���
		set<uint32_t> m_snakeSet;						// �ߵ��б�
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

		void EnterGame(IConnection* pConn, uint64_t uUserId, const string& strNickName, uint32_t uSkinId);				// ��ҽ�����Ϸ

	public:
		void DeleteSnakeGameRoom(uint32_t uSnakeId);						// ɾ���ߵķ����ϵ

	private:
		GameRoom* CreateGameRoom(uint32_t uGameTime, uint32_t uMaxPlayer);	// ������Ϸ����
		void ReleaseGameRoom(GameRoom* pGameRoom);							// �ͷ���Ϸ����
		bool HasInGame(uint32_t uSnakeId);									// �Ƿ��Ѿ�����Ϸ����
		GameRoom* GetGameRoom(uint32_t uSnakeId);							// ��ȡ��Ϸ����
		GameRoom* GetAvailabeRoom();										// ��ȡ���Լ������Ϸ����
		bool HasSnake(uint64_t uUserId);									// ������Ƿ�����

	private:
		map<uint32_t, GameRoom*> m_playerGameRoundMap;						// ������ڵ���Ϸ�����б�
		list<GameRoom*> m_gameRoomList;										// ��Ϸ�����б�
		uint32_t m_uSnakeId;												// �ߵ�ID
		map<uint64_t, uint32_t> m_playerSnakeMap;							// ���ID��Ӧ��ID�ı�
	};

#define gpGameRoomMng GameRoomMng::GetInstance()
}


#endif