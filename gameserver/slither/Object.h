#ifndef GAMESERVER_SLITHER_OBJECT_H
#define GAMESERVER_SLITHER_OBJECT_H

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../config/SlitherConfig.h"

namespace slither {
	struct Vector2D {
		float x;
		float y;

		Vector2D() : x(0), y(0) {
		}

		Vector2D(float _x, float _y) : x(_x), y(_y) {
		}

		float Magnitude() {
			return ::sqrt(x * x + y * y);
		}
	};

	enum ObjectStatus {
		OBJ_EXIST = 0,					// 存在
		OBJ_DESTROY = 1,				// 销毁
	};

	enum ObjectType {
		Snake_Head_Type = 1,		// 蛇身
		Snake_Body_Type = 2,		// 蛇身
		Food_Type = 3,				// 食物
	};

	// 蛇的统计（只统计本局的）
	struct SnakeStatistics {
		uint32_t uRank;							// 排名
		uint32_t uKillSnakeTimes;				// 击杀蛇的次数
		uint32_t uEatNum;						// 吞噬数量
		uint32_t uMaxLength;					// 最大长度
		uint32_t uDeadTimes;					// 死亡次数
		uint32_t uSkinId;						// 皮肤ID
		uint32_t uTeamId;						// 所属队伍Id
		uint32_t uEnterRoomTime;				// 进入房间时间(相对于房间的时间)
		uint32_t uLeaveRoomTime;				// 离开房间时间(相对于房间的时间)
		uint32_t uGameEndMass;					// 结算时长度
		float fSpeedUpCost;						// 加速消耗量

		// 玩家相关信息，不涉及统计
		bool gender;
		string strPlayerName;

		SnakeStatistics(bool _gender, string _strPlayerName, uint32_t uEnterTime) : 
			uEnterRoomTime(uEnterTime), gender(_gender), strPlayerName(_strPlayerName) {
			uRank			= 0;
			uKillSnakeTimes = 0;
			uEatNum			= 0;
			uMaxLength		= 0;
			uDeadTimes		= 0;
			uSkinId			= 0;
			uTeamId			= 0;
			uGameEndMass	= 0;
			fSpeedUpCost	= 0.0f;
			uLeaveRoomTime	= 0;
			
			return;
		}
	};

	// 蛇视野范围点
	struct SnakeViewPoint {
		uint32_t uLeftTopIndex;
		uint32_t uLeftBottomIndex; 
		uint32_t uRightBottomIndex;

		SnakeViewPoint() : uLeftTopIndex(0), uLeftBottomIndex(0), uRightBottomIndex(0) {

		}

		bool IsSame(const SnakeViewPoint& viewPoint) {
			if (uLeftTopIndex != viewPoint.uLeftTopIndex) {
				return false;
			}

			if (uLeftBottomIndex != viewPoint.uLeftBottomIndex) {
				return false;
			}

			if (uRightBottomIndex != viewPoint.uRightBottomIndex) {
				return false;
			}

			return true;
		}
	};

	class Object
	{
	public:
		Object() {
		}
		Object(const Vector2D& pos, float fRadius, ObjectType type) :m_pos(pos), m_fRadius(fRadius), m_objType(type) {
		}
		virtual ~Object() {
		}

	public:
		void Clear() {
			memset(this, 0, sizeof(*this));
		}

		// 获取物体的中心点
		const Vector2D& GetPos() {
			return m_pos;
		}

		// 设置物体中心点
		void SetPos(Vector2D pos) {
			m_pos.x = pos.x;
			m_pos.y = pos.y;
		}

		// 获取物体类型
		ObjectType GetType() {
			return m_objType;
		}

		// 设置半径
		void SetRadius(float fRadius) {
			m_fRadius = fRadius;
		}

		// 获取半径
		float GetRadius() {
			return m_fRadius;
		}

		// 进行碰撞检测
		bool IsCollide(Object* pObj) {
			if (!pObj) {
				return false;
			}

			// 两个圆做碰撞检测，只要判定圆点距离是否大于两个圆的半径即可
			float fLen = sqrt(pow((pObj->GetPos().x - m_pos.x), 2) + pow((pObj->GetPos().y - m_pos.y), 2));
			if (fLen < ((pObj->GetRadius() + m_fRadius) * g_slitherConfig.m_fCollideProportion)) {
				return true;
			}

			return false;
		}

		// 判断某个点是否被包含 
		bool IsContained(const Vector2D& pos) {
			// 如果点到圆心的距离小于半径，则是被包含了
			float fLen = sqrt(pow((pos.x - m_pos.x), 2) + pow((pos.y - m_pos.y), 2));
			if (fLen < m_fRadius + g_slitherConfig.m_fEatFoodRadius) {
				return true;
			}

			return false;
		}


	protected:
		Vector2D m_pos;							// 物体中心点
		float m_fRadius;						// 物体半径
		ObjectType m_objType;					// 物体类型
	};

	class Snake;
	class Food;

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

		bool HasSnake() {
			if (m_snakeMap.empty()) {
				return false;
			}

			return true;
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
}

#endif