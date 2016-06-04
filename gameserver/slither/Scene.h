#ifndef GAMESERVER_SLITHER_SCENE_H
#define GAMESERVER_SLITHER_SCENE_H

#include "../../network/header.h"
#include "../../network/IConnection.h"
#include "Object.h"
#include "../../protocol/slither_battle.pb.h"
#include <list>
#include <set>
#include <map>
using namespace std;
using namespace cpnet;

namespace slither {

	class Snake;
	class Food;
	class GameRoom;
	typedef set<Snake*> SnakeSet;

	// 使用数学直角坐标系标准，原点在左下角
	struct Grid {
		uint32_t x;								// 坐标点x
		uint32_t y;								// 坐标点y
		uint32_t uHeight;						// 长
		uint32_t uWidth;						// 宽

		map<Snake*, set<Object*> > m_snakeMap;	// 格子中的蛇（只要蛇有任何一个节点在此格子中，就认为这个格子中有此蛇）
		list<Object*> foodList;					// 食物列表

		// 获取格子的中心点
		Vector2D GetCenterPos() {
			Vector2D pos(x + (float)uWidth / 2, y + (float)uHeight / 2);
			return pos;
		}

		map<Snake*, set<Object*> >& GetSnakeList() {
			return m_snakeMap;
		}

		// 获取格子中食物列表
		list<Object*>& GetFoodList() {
			return foodList;
		}

		void DelObj(Object* pObj);							// 从格子中添加物体

		void AddObj(Object* pObj);							// 往格子中添加物体

		bool IsExist(Snake* pSnake);						// 是否已经有此蛇
	};

	// 一个月物体最多能同时处于几个格子
	#define MAX_GRID_IN 4
	struct ObjectGrids {
		int32_t grids[MAX_GRID_IN];
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

		void DeleteInGrids(Object* pObj, Grid* pGrids) {
			for (int i = 0; i < MAX_GRID_IN; ++i) {
				if (grids[i] == -1) {
					continue;
				}
				pGrids[grids[i]].DelObj(pObj);
			}
		}

		void UpdateGrids(Object* pObj, Grid* pGrids, const ObjectGrids& oldGrids) {
			// 跟之前完全一样，不需要做任何所在格子的更新
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
		Scene(GameRoom* pGameRoom);
		~Scene();

		bool Init(uint32_t uSceneHeight, uint32_t uSceneWidth, uint32_t uGridSize);
		void OnTimer();
		void TestScene();
		void Finish();																// 结束场景

	public:
		ObjectGrids GetObjectGrids(Object* pObj);									// 获取一个物体所在的格子
		ObjectGrids GetObjectGrids(const Vector2D& pos, float fRadius);				// 获取一个物体所在的格子
		Grid* GetSceneGrids() {
			return m_pGrids;
		}
		Snake* GetSnakeById(uint32_t uSnakeId);
		Vector2D GenSnakeBornPos();																	// 生成蛇的初始坐标
		const map<uint32_t, Snake*>& GetSnakeMap() {
			return m_snakeMap;
		}

		set<uint32_t> GetInViewGrids(Snake* pSnake);												// 获取格子列表，根据蛇本身的视野来决定
		void GetInViewGrids(Snake* pSnake, set<uint32_t>& gridSet);

	public:
		// 处理玩家信息
		void PlayerEnter(IConnection* pConn);
		void PlayerEnter(Snake* pSnake);
		void SnakeMove(Snake* pSnake, float fNewAngle, bool bSpeedUp=false, bool bStopMove=false);

	private:
		uint32_t GetGridIndex(const Vector2D& pos);													// 获取坐标点所在的格子
		void GenFoods(uint32_t uNum);																// 生成食物
		void GenFoods(const Vector2D& pos, uint32_t uNum, uint32_t uValue, list<Food*>& foodList);	// 在指定点生成食物
		void GenFoods(Grid& grid, uint32_t uNum, list<Food*>& foodList);							// 在指定格子中刷新食物
		void RefreshFoods();																		// 定时刷新食物
		Snake* GenSnake(bool bRobot);																// 生成一条蛇
		void AddSnakeInScene(Snake* pSnake);														// 将蛇加入场景
		void BreakUpSnake(Snake* pSnake, bool bGenFood = true);										// 分解蛇
		uint32_t GetBreakUpFoodSize(Snake* pSnake);													// 获取分解可以得到的食物数量
		
		void DeleteSnakeViewGrids(Snake* pSnake);													// 删除蛇视野中的格子
		void UpdateSnakeViewGrids(Snake* pSnake);													// 更新蛇视野中的格子
		SnakeSet& GetSnakeSet(uint32_t uGridId);													// 获取拥有此格子视野的蛇列表

	private:
		void NotifyAround(Snake* pSnake);															// 通知这条蛇周围的情况
		void CheckSpeedUp(Snake* pSnake);															// 检查加速情况
		void CheckEatFood(Snake* pSnake, uint16_t uGridIndex);										// 在指定格子内判定吃食物
		void CheckCollide(Snake* pSnake, uint16_t uGridIndex);										// 在指定格子内判定碰撞
		void CheckCollideWithEgde(Snake* pSnake, uint16_t uGridIndex);								// 判断和边界的碰撞
		void BroadcastEat(set<Snake*>& broadcastList, slither::BroadcastEat& eatFoods);				// 广播吃食物
		void BroadcastCollide(set<Snake*>& broadcastList, Snake* pSnake);							// 向周围广播这条蛇发生了碰撞
		void BroadcastNewFoods(const list<Food*>& newFoodList);										// 向周围广播新生成了一些食物
		void BroadcastNewFoods(uint32_t uGridId, const list<Food*>& newFoodList);					// 广播生成了新的食物
		void BroadcastRankingList();																// 广播排行榜信息

	private:
		Grid* m_pGrids;												// 将地图划分为N*N的格子
		float m_fSceneLength;										// 场景长度
		uint32_t m_uHorizontalGridNum;								// 水平方向（x方向）格子数
		uint32_t m_uVerticalGridNum;								// 垂直方向（y方向）格子数
		uint32_t m_uGridSize;										// 每个小格子的长度
		uint32_t m_uSnakeId;										// 蛇的ID（递增）
		uint32_t m_uFoodId;											// 食物ID（递增）
		map<uint32_t, Snake*> m_snakeMap;							// 蛇的列表
		uint32_t m_TestCount;										// 测试使用
		uint32_t m_uFoodCount;										// 食物数量

		map<uint32_t, SnakeSet>	m_gridInSnakeViewMap;				// 能看见格子的蛇列表
		GameRoom* m_pGameRoom;
		boost::posix_time::ptime m_startTime;						// 场景开始时间
	};
}

#endif