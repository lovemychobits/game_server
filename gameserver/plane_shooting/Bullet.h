#ifndef GAMESERVER_PLANESHOOTING_BULLET_H
#define GAMESERVER_PLANESHOOTING_BULLET_H

#include "Plane.h"
#include "Object.h"

namespace plane_shooting {

// 子弹最大飞行距离
#define MAX_DISTANCE 30	

	class Bullet : public Object
	{
	public:
		Bullet();
		Bullet(uint16_t uBulletId, Plane* pOwner, Vector2D& pos, uint16_t uAngle, uint16_t uSpeed);
		~Bullet();

	public:
		void fly();								// 子弹飞行，完全由服务器来模拟

	private:
		Plane* m_pOwner;						// 属于哪个飞机
		uint16_t m_uAngle;						// 角度（0~360）
		uint16_t m_uSpeed;						// 速度
		int8_t m_orientation;					// 朝向
		uint16_t m_uDistance;					// 已经飞行的距离
	};

}

#endif