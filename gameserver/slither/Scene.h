#ifndef GAMESERVER_SLITHER_SCENE_H
#define GAMESERVER_SLITHER_SCENE_H

#include "../../network/header.h"
#include "../../network/IConnection.h"
#include "Object.h"
#include <list>
#include <set>
#include <map>
using namespace std;
using namespace cpnet;

namespace slither {

	class Snake;

	// ʹ����ѧֱ������ϵ��׼��ԭ�������½�
	struct Grid {
		uint32_t x;					// �����x
		uint32_t y;					// �����u0
		uint32_t uHeight;			// ��
		uint32_t uWidth;			// ��

		list<Object*> snakeHeadList;	// ��ͷ�б�
		list<Object*> snakeBodyList;	// �������б�
		list<Object*> foodList;			// ʳ���б�

		Vector2D GetCenterPos() {
			Vector2D pos(x + uWidth / 2, y + uHeight / 2);
			return pos;
		}

		void DelObj(Object* pObj) {
			if (!pObj) {
				return;
			}

			if (pObj->GetType() == Food_Type) {
				list<Object*>::iterator objIt = foodList.begin();
				for (; objIt != foodList.end(); ) {
					if (*objIt == pObj) {
						foodList.erase(objIt++);
						break;
					}
					objIt++;
				}
			}
			else if (pObj->GetType() == Snake_Head_Type) {
				list<Object*>::iterator objIt = snakeHeadList.begin();
				for (; objIt != snakeHeadList.end();) {
					if (*objIt == pObj) {
						snakeHeadList.erase(objIt++);
						break;
					}
					objIt++;
				}
			}
			else if (pObj->GetType() == Snake_Body_Type) {
				list<Object*>::iterator objIt = snakeBodyList.begin();
				for (; objIt != snakeBodyList.end();) {
					if (*objIt == pObj) {
						snakeBodyList.erase(objIt++);
						break;
					}
					objIt++;
				}
			}
		}

		void AddObj(Object* pObj) {
			if (!pObj) {
				return;
			}

			if (pObj->GetType() == Food_Type) {
				foodList.push_back(pObj);
			}
			else if (pObj->GetType() == Snake_Head_Type) {
				snakeHeadList.push_back(pObj);
			}
			else if (pObj->GetType() == Snake_Body_Type) {
				snakeBodyList.push_back(pObj);
			}
		}

		list<Object*>& GetSnakeBodyList() {
			return snakeBodyList;
		}

		list<Object*>& GetSnakeHeadList() {
			return snakeHeadList;
		}

		list<Object*>& GetFoodList() {
			return foodList;
		}
	};

	// һ�������������ͬʱ���ڼ�������
	#define MAX_GRID_IN 4
	struct ObjectGrids {
		int16_t grids[MAX_GRID_IN];
		ObjectGrids() {
			for (int i = 0; i < MAX_GRID_IN; ++i) {
				grids[i] = -1;
			}
		}

		bool IsSame(const ObjectGrids& otherGrids) {
			for (int i = 0; i < MAX_GRID_IN; ++i) {
				if (otherGrids.grids[i] != grids[i]) {
					return false;
				}
			}

			return true;
		}

		void UpdateGrids(Object* pObj, Grid* pGrids, const ObjectGrids& oldGrids) {
			// ��֮ǰ��ȫһ���£�����Ҫ���κ����ڸ��ӵĸ���
			if (IsSame(oldGrids)) {
				return;
			}

			// �����һ������ô��ԭ������Щ������ɾ����Ȼ������µĸ�����
			// ���������Ҫ�Ż�������ĳЩ���ӻ���һ����
			for (int i = 0; i < MAX_GRID_IN; ++i) {
				if (oldGrids.grids[i] == -1) {
					continue;
				}
				pGrids[oldGrids.grids[i]].DelObj(pObj);
			}
			for (int i = 0; i < MAX_GRID_IN; ++i) {
				if (grids[i] == -1) {
					continue;
				}
				pGrids[grids[i]].AddObj(pObj);
			}
		}
	};

	class Scene {
	public:
		Scene();
		~Scene();
		static Scene* GetInstance() {
			static Scene instance;
			return &instance;
		}

		bool Init(uint32_t uSceneHeight, uint32_t uSceneWidth);
		void OnTimer(const boost::system::error_code&);
		void TestScene();

	public:
		ObjectGrids GetObjectGrids(Object* pObj);					// ��ȡһ���������ڵĸ���
		Grid* GetSceneGrids() {
			return m_pGrids;
		}
		Snake* GetSnakeById(uint32_t uSnakeId);

	public:
		// ���������Ϣ
		void PlayerEnter(IConnection* pConn, uint32_t uUserId);
		void SnakeMove(Snake* pSnake, uint16_t uNewAngle, bool bSpeedUp);

	private:
		uint32_t GetGridIndex(const Vector2D& pos);												// ��ȡ��������ڵĸ���
		void GenFoods(uint32_t uNum);															// ����ʳ��
		void GenFoods(const Vector2D& pos, uint32_t uNum, uint32_t uValue);						// ��ָ��������ʳ��
		Snake* GenSnake(bool bRobot);															// ����һ����
		void BreakUpSnake(Snake* pSnake);														// �ֽ���
		vector<uint16_t> GetInViewGrids(uint16_t uGridIndex, uint16_t uSnakeViewRange);			// ��ȡ�����б������߱������Ұ������
		void Notify(Snake* pSnake);																// ����Χ�㲥������

	private:
		Grid* m_pGrids;												// ����ͼ����ΪN*N�ĸ���
		uint32_t m_uHorizontalGridNum;								// ˮƽ����x���򣩸�����
		uint32_t m_uVerticalGridNum;								// ��ֱ����y���򣩸�����
		uint32_t m_uSnakeId;										// �ߵ�ID��������
		uint32_t m_uFoodId;											// ʳ��ID��������
		map<uint32_t, Snake*> m_snakeMap;							// �ߵ��б�
		uint32_t m_TestCount;										// ����ʹ��
		uint32_t m_uFoodCount;
	};
}

#endif