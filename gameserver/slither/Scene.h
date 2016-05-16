#ifndef GAMESERVER_SLITHER_SCENE_H
#define GAMESERVER_SLITHER_SCENE_H

#include "../../network/header.h"
#include "../../network/IConnection.h"
#include "Object.h"
#include "../../protocol/slither_client.pb.h"
#include <list>
#include <set>
#include <map>
using namespace std;
using namespace cpnet;

namespace slither {

	class Snake;
	class Food;

	// 使用数学直角坐标系标准，原点在左下角
	struct Grid {
		uint32_t x;					// 坐标点x
		uint32_t y;					// 坐标点u0
		uint32_t uHeight;			// 长
		uint32_t uWidth;			// 宽

		list<Object*> snakeHeadList;	// 蛇头列表
		list<Object*> snakeBodyList;	// 蛇身体列表
		list<Object*> foodList;			// 食物列表

		Vector2D GetCenterPos() {
			Vector2D pos(x + (float)uWidth / 2, y + (float)uHeight / 2);
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

	// 一个月物体最多能同时处于几个格子
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
			// 跟之前完全一个月，不需要做任何所在格子的更新
			if (IsSame(oldGrids)) {
				return;
			}

			// 如果不一样，那么从原来的那些格子中删除，然后加入新的格子中
			// 这里可以需要优化，可能某些格子还是一样的
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
		ObjectGrids GetObjectGrids(Object* pObj);					// 获取一个物体所在的格子
		Grid* GetSceneGrids() {
			return m_pGrids;
		}
		Snake* GetSnakeById(uint32_t uSnakeId);

	public:
		// 处理玩家信息
		void PlayerEnter(IConnection* pConn, uint32_t uUserId);
		void SnakeMove(Snake* pSnake, float fNewAngle, bool bSpeedUp);

	private:
		uint32_t GetGridIndex(const Vector2D& pos);												// 获取坐标点所在的格子
		void GenFoods(uint32_t uNum);															// 生成食物
		void GenFoods(const Vector2D& pos, uint32_t uNum, uint32_t uValue, list<Food*>& foodList);	// 在指定点生成食物
		Snake* GenSnake(bool bRobot);															// 生成一条蛇
		void BreakUpSnake(Snake* pSnake, list<Snake*>& broadcastList);							// 分解蛇
		vector<uint16_t> GetInViewGrids(Snake* pSnake);											// 获取格子列表，根据蛇本身的视野来决定

	private:
		void Notify(Snake* pSnake);																// 通知这条蛇周围的情况
		void GetBroadcastList(Snake* pSnake, list<Snake*>& broadcastList);						// 获取需要通知对象列表
		void CheckEatFood(Snake* pSnake, uint16_t uGridIndex, list<Snake*>& broadcastList);		// 在指定格子内判定吃食物
		void CheckCollide(Snake* pSnake, uint16_t uGridIndex, list<Snake*>& broadcastList);		// 在指定格子内判定碰撞
		void BroadcastMove(Snake* pSnake, list<Snake*>& broadcastList);							// 向周围广播这条蛇移动
		void BroadcastEat(list<Snake*>& broadcastList, slither::BroadcastEat& eatFoods);		// 广播吃食物
		void BroadcastCollide(list<Snake*>& broadcastList, Snake* pSnake);						// 向周围广播这条蛇发生了碰撞
		void BroadcastNewFoods(list<Snake*>& broadcastList, const list<Food*>& newFoodList);	// 向周围广播新生成了一些食物
		void BroadcastNewTail(Snake* pSnake, list<Snake*>& broadcastList);						// 向周围广播这条蛇长出了新的尾巴节点
		void NotifyGrids(Snake* pSnake);														// 向这条蛇通知新的格子信息

	private:
		Grid* m_pGrids;												// 将地图划分为N*N的格子
		uint32_t m_uHorizontalGridNum;								// 水平方向（x方向）格子数
		uint32_t m_uVerticalGridNum;								// 垂直方向（y方向）格子数
		uint32_t m_uSnakeId;										// 蛇的ID（递增）
		uint32_t m_uFoodId;											// 食物ID（递增）
		map<uint32_t, Snake*> m_snakeMap;							// 蛇的列表
		uint32_t m_TestCount;										// 测试使用
		uint32_t m_uFoodCount;
	};
}

#endif