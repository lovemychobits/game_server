#ifndef GAMESERVER_PLANESHOOTING_BULLET_H
#define GAMESERVER_PLANESHOOTING_BULLET_H

#include "Plane.h"
#include "Object.h"

namespace plane_shooting {

// �ӵ������о���
#define MAX_DISTANCE 30	

	class Bullet : public Object
	{
	public:
		Bullet();
		Bullet(uint16_t uBulletId, Plane* pOwner, Vector2D& pos, uint16_t uAngle, uint16_t uSpeed);
		~Bullet();

	public:
		void fly();								// �ӵ����У���ȫ�ɷ�������ģ��

	private:
		Plane* m_pOwner;						// �����ĸ��ɻ�
		uint16_t m_uAngle;						// �Ƕȣ�0~360��
		uint16_t m_uSpeed;						// �ٶ�
		int8_t m_orientation;					// ����
		uint16_t m_uDistance;					// �Ѿ����еľ���
	};

}

#endif