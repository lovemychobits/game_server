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
		OBJ_EXIST = 0,					// ����
		OBJ_DESTROY = 1,				// ����
	};

	enum ObjectType {
		Snake_Head_Type = 1,		// ����
		Snake_Body_Type = 2,		// ����
		Food_Type = 3,				// ʳ��
	};

	// �ߵ�ͳ�ƣ�ֻͳ�Ʊ��ֵģ�
	struct SnakeStatistics {
		uint32_t uRank;							// ����
		uint32_t uKillSnakeTimes;				// ��ɱ�ߵĴ���
		uint32_t uEatNum;						// ��������
		uint32_t uMaxLength;					// ��󳤶�
		uint32_t uDeadTimes;					// ��������
		uint32_t uSkinId;						// Ƥ��ID
		uint32_t uTeamId;						// ��������Id
		uint32_t uEnterRoomTime;				// ���뷿��ʱ��(����ڷ����ʱ��)
		uint32_t uLeaveRoomTime;				// �뿪����ʱ��(����ڷ����ʱ��)
		uint32_t uGameEndMass;					// ����ʱ����
		float fSpeedUpCost;						// ����������

		// ��������Ϣ�����漰ͳ��
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

	// ����Ұ��Χ��
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

		// ��ȡ��������ĵ�
		const Vector2D& GetPos() {
			return m_pos;
		}

		// �����������ĵ�
		void SetPos(Vector2D pos) {
			m_pos.x = pos.x;
			m_pos.y = pos.y;
		}

		// ��ȡ��������
		ObjectType GetType() {
			return m_objType;
		}

		// ���ð뾶
		void SetRadius(float fRadius) {
			m_fRadius = fRadius;
		}

		// ��ȡ�뾶
		float GetRadius() {
			return m_fRadius;
		}

		// ������ײ���
		bool IsCollide(Object* pObj) {
			if (!pObj) {
				return false;
			}

			// ����Բ����ײ��⣬ֻҪ�ж�Բ������Ƿ��������Բ�İ뾶����
			float fLen = sqrt(pow((pObj->GetPos().x - m_pos.x), 2) + pow((pObj->GetPos().y - m_pos.y), 2));
			if (fLen < ((pObj->GetRadius() + m_fRadius) * g_slitherConfig.m_fCollideProportion)) {
				return true;
			}

			return false;
		}

		// �ж�ĳ�����Ƿ񱻰��� 
		bool IsContained(const Vector2D& pos) {
			// ����㵽Բ�ĵľ���С�ڰ뾶�����Ǳ�������
			float fLen = sqrt(pow((pos.x - m_pos.x), 2) + pow((pos.y - m_pos.y), 2));
			if (fLen < m_fRadius + g_slitherConfig.m_fEatFoodRadius) {
				return true;
			}

			return false;
		}


	protected:
		Vector2D m_pos;							// �������ĵ�
		float m_fRadius;						// ����뾶
		ObjectType m_objType;					// ��������
	};

	class Snake;
	class Food;

	// ʹ����ѧֱ������ϵ��׼��ԭ�������½�
	struct Grid {
		uint32_t x;								// �����x
		uint32_t y;								// �����y
		uint32_t uHeight;						// ��
		uint32_t uWidth;						// ��

		map<Snake*, set<Object*> > m_snakeMap;	// �����е��ߣ�ֻҪ�����κ�һ���ڵ��ڴ˸����У�����Ϊ����������д��ߣ�
		list<Object*> foodList;					// ʳ���б�

		// ��ȡ���ӵ����ĵ�
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

		// ��ȡ������ʳ���б�
		list<Object*>& GetFoodList() {
			return foodList;
		}

		void DelObj(Object* pObj);							// �Ӹ������������

		void AddObj(Object* pObj);							// ���������������

		bool IsExist(Snake* pSnake);						// �Ƿ��Ѿ��д���
	};
	
	// һ�������������ͬʱ���ڼ�������
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
			// ��֮ǰ��ȫһ��������Ҫ���κ����ڸ��ӵĸ���
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
}

#endif