#ifndef GAMESERVER_PLANESHOOTING_OBJECT_H
#define GAMESERVER_PLANESHOOTING_OBJECT_H

#include <stdint.h>
#include <string.h>

namespace plane_shooting {
	struct Vector2D {
		float x;
		float y;

		Vector2D() : x(0), y(0) {
		}

		Vector2D(float _x, float _y) : x(_x), y(_y) {
		}
	};

	enum ObjectStatus {
		ALIVE = 0,					// 存活的
		DEAD = 1,					// 死亡
	};

	/*
	width
	x1y1-----------x2y2
	|				|
	|	 center		|	height
	|	  (x,y)		|
	x3y3-----------x4y4
	*/
	struct Rectangle {
		float x;			// 矩形中心点x
		float y;			// 矩形中心点y
		float fHeight;		// 矩形长度
		float fWidth;		// 矩形宽度

		Rectangle() {
			init();
		}

		Rectangle(float _x, float _y, float _uWidth, float _uHeight) :
			x(_x), y(_y), fWidth(_uWidth), fHeight(_uHeight){
		}

		void init() {
			memset(this, 0, sizeof(*this));
		}
	};

	enum ObjectType {
		Plane_Type = 1,
		Bullet_Type = 2,
	};

	class Object
	{
	public:
		Object() {
		}
		Object(uint16_t uObjId, const Rectangle& rect, ObjectType type) : m_uObjId(uObjId), m_rect(rect), m_objType(type) {
		}
		~Object() {
		}

	public:
		// 获取物体ID
		uint16_t GetObjId() {
			return m_uObjId;
		}

		// 获取物体的碰撞矩形
		Rectangle& GetRect() {
			return m_rect;
		}

		// 获取物体的中心点
		Vector2D GetPos() {
			Vector2D pos(m_rect.x, m_rect.y);
			return pos;
		}

		// 设置物体中心点
		void SetPos(Vector2D pos) {
			m_rect.x = pos.x;
			m_rect.y = pos.y;
		}

		// 获取物体类型
		ObjectType GetType() {
			return m_objType;
		}

	protected:
		uint16_t m_uObjId;						// 物体ID
		ObjectType m_objType;					// 物体类型
		Rectangle m_rect;						// 物体碰撞矩形
	};
}

#endif