#include "Bullet.h"

namespace plane_shooting {
	Bullet::Bullet() {

	}

	Bullet::Bullet(uint16_t uBulletId, Plane* pOwner, Vector2D& pos, uint16_t uAngle, uint16_t uSpeed) :
		Object(uBulletId, pos), m_pOwner(pOwner), m_uAngle(uAngle), m_uSpeed(uSpeed){

		m_orientation = pOwner->GetOrientation();
	}

	Bullet::~Bullet() {

	}

	void Bullet::fly() {
		if (m_uDistance >= MAX_DISTANCE) {
			return;
		}
		// ��ֻ����ֱ�ߣ������Ǵ��Ƕȵķ���
		if (m_orientation == 1) {			// ��y��������
			m_pos.y += m_uSpeed;
		}
		else {								// ��y�ᷴ����
			m_pos.y -= m_uSpeed;
		}

		m_uDistance += m_uSpeed;
		return;
	}
}