#ifndef GAMESERVER_SLITHER_OBJECT_H
#define GAMESERVER_SLITHER_OBJECT_H

#include <stdint.h>
#include <string.h>
#include <math.h>

namespace slither {
#define PI 3.14159f

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
		Vector2D& GetPos() {
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
			if (fLen < pObj->GetRadius() + m_fRadius) {
				return true;
			}

			return false;
		}

		// 判断某个点是否被包含 
		bool IsContained(const Vector2D& pos) {
			// 如果点到圆心的距离小于半径，则是被包含了
			float fLen = sqrt(pow((pos.x - m_pos.x), 2) + pow((pos.y - m_pos.y), 2));
			if (fLen < m_fRadius) {
				return true;
			}

			return false;
		}

	protected:
		Vector2D m_pos;							// 物体中心点
		float m_fRadius;						// 物体半径
		ObjectType m_objType;					// 物体类型
	};
}

#endif