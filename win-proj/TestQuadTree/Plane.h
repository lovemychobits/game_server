#ifndef GAMESERVER_PLANESHOOTING_PLANE_H
#define GAMESERVER_PLANESHOOTING_PLANE_H

#include <stdint.h>
#include "Object.h"

namespace plane_shooting {

	enum ObjectStatus {
		ALIVE = 0,					// 存活的
		DEAD = 1,					// 死亡
	};

#define MOVE_STEP 1

	// 飞机视野范围，也就是如果有其他飞机，或者子弹进入这个范围了，就需要通知飞机
	struct PlaneViewRange
	{
		uint16_t uWidth;			// 长度
		uint16_t uHeight;			// 宽度
	};

	class Plane : public Object
	{
	public:
		Plane();
		Plane(uint16_t uPlaneId, const Rectangle& rect, int8_t orientation);
		~Plane();

	public:
		// 行为，操作
		void Left(uint32_t uLeftTimes = 1);					// 左移
		void Right(uint32_t uRightTimes = 1);				// 右移
		void Up(uint32_t uUpTimes = 1);						// 上移
		void Down(uint32_t uDownTimes = 1);					// 下移
		void TurnAround();									// 掉头

	public:
		void SetSpeed(uint16_t uSpeed) {
			m_curSpeed = uSpeed;
		}

		PlaneStatus GetStatus() {
			return m_status;
		}

		int8_t GetOrientation() {
			return m_orientation;
		}

		bool NeedNotify(Object* pObj);					// 是否需要通知

	private:
		bool IsInViewRange(const Rectangle& rect);		// 是否在视野内

	private:
		int8_t m_orientation;							// 朝向，1为朝上，-1为朝下
		uint16_t m_curSpeed;							// 当前速度
		PlaneStatus m_status;							// 飞机当前状态
		PlaneViewRange m_viewRange;						// 视野范围
	};
}

#endif