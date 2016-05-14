#ifndef GAMESERVER_PLANESHOOTING_OBJECT_H
#define GAMESERVER_PLANESHOOTING_OBJECT_H

#include <stdint.h>
#include <string.h>

namespace plane_shooting {
	struct Vector2D {
		uint16_t x;
		uint16_t y;

		Vector2D() : x(0), y(0) {
		}
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
		uint16_t x;				// 矩形中心点x
		uint16_t y;				// 矩形中心点y
		uint16_t uHeight;		// 矩形长度
		uint16_t uWidth;		// 矩形宽度

		Rectangle() {
			init();
		}

		Rectangle(uint16_t _x, uint16_t _y, uint16_t _uWidth, uint16_t _uHeight) :
			x(_x), y(_y), uWidth(_uWidth), uHeight(_uHeight){
		}

		void init() {
			memset(this, 0, sizeof(*this));
		}
	};


	class Object
	{
	public:
		Object() {
		}
		Object(uint16_t uObjId, const Rectangle& rect) : m_uObjId(uObjId), m_rect(rect) {
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

	protected:
		uint16_t m_uObjId;						// 物体ID
		Rectangle m_rect;						// 物体所在矩形
	};
}

#endif