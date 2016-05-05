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
		Object(uint16_t uObjId, const Rectangle& rect, ObjectType type) : m_uObjId(uObjId), m_rect(rect), m_uObjType(type) {
		}
		~Object() {
		}

	public:
		uint16_t GetObjId() {
			return m_uObjId;
		}

		Rectangle& GetRect() {
			return m_rect;
		}

		Vector2D GetPos() {
			Vector2D pos;
			pos.x = m_rect.x;
			pos.y = m_rect.y;
			return pos;
		}

		ObjectType GetType() {
			return (ObjectType)m_uObjType;
		}

		void SetPos(Vector2D pos) {
			m_rect.x = pos.x;
			m_rect.y = pos.y;
		}
	protected:
		uint16_t m_uObjId;						// 物体ID
		uint16_t m_uObjType;					// 物体类型
		Rectangle m_rect;						// 物体所在矩形
	};
}

#endif