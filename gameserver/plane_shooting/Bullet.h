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
		Bullet(uint16_t uBulletId, Plane* pOwner, const Rectangle& rect, uint16_t uAngle, float fSpeed);
		~Bullet();

	public:
		void fly();								// �ӵ����У���ȫ�ɷ�������ģ��
		bool CanDestroy() {
			return m_bDestroy;
		}

		void SetStatus(ObjectStatus status) {
			m_status = status;
			if (status == DEAD)
			{
				m_bDestroy = true;
			}
		}

		ObjectStatus GetStatus() {
			return m_status;
		}

	private:
		uint16_t GetSinAngle(uint16_t uAngle);	// ����unity����ϵ�еĽǶȣ���������ڵ�ֱ������ϵ��б��sin��Ӧ�ĽǶ�
		float GetXLen(float vectLen, uint16_t uSinAngle);
		float GetYLen(float vectLen, uint16_t uSinAngle);

	private:
		Plane* m_pOwner;						// �����ĸ��ɻ�
		uint16_t m_uAngle;						// �Ƕȣ�0~360������unity����ϵ�У�����ʱ�뷽���ת�ǣ�
		float m_fSpeed;							// �ٶ�
		int8_t m_orientation;					// ����
		float m_fDistance;						// �Ѿ����еľ���
		bool m_bDestroy;						// ����
		ObjectStatus m_status;					// ״̬
	};

}

#endif