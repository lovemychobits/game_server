#ifndef GAMESERVER_PLANESHOOTING_PLANE_H
#define GAMESERVER_PLANESHOOTING_PLANE_H

#include <stdint.h>
#include "Object.h"
#include "../../network/IConnection.h"
using namespace cpnet;

namespace plane_shooting {

#define MOVE_STEP 1

	// �ɻ���Ұ��Χ��Ҳ��������������ɻ��������ӵ����������Χ�ˣ�����Ҫ֪ͨ�ɻ�
	struct PlaneViewRange
	{
		float fWidth;			// ����
		float fHeight;			// ���
	};

	class Plane : public Object
	{
	public:
		Plane();
		Plane(uint16_t uPlaneId, const Rectangle& rect, int8_t orientation, IConnection* pConn);
		~Plane();

	public:
		// ������Ϣ
		void SendMsg(const char* pData, uint32_t uLen);

	public:
		// ��������
		void SetSpeed(float fSpeed) {
			m_curSpeed = fSpeed;
		}

		float GetSpeed() {
			return m_curSpeed;
		}

		int8_t GetOrientation() {
			return m_orientation;
		}

		void SetFireAngle(uint16_t uAngle) {
			m_uFireAngle = uAngle;
		}

		uint16_t GetFireAngle() {
			return m_uFireAngle;
		}

		void SetFlyAngle(uint16_t uAngle) {
			m_uFlyAngle = uAngle;
		}

		uint16_t GetFlyAngle() {
			return m_uFlyAngle;
		}

		void SetStatus(ObjectStatus status) {
			m_status = status;
		}

		ObjectStatus GetStatus() {
			return m_status;
		}

		Rectangle GetViewRect() {
			Rectangle viewRect(m_rect.x, m_rect.y, m_viewRange.fWidth, m_viewRange.fHeight);
			return viewRect;
		}

		bool NeedNotify(Object* pObj);					// �Ƿ���Ҫ֪ͨ

	private:
		bool IsInViewRange(const Rectangle& rect);		// �Ƿ�����Ұ��

	private:
		IConnection* m_pConn;
		int8_t m_orientation;							// ����1Ϊ���ϣ�-1Ϊ����
		float m_curSpeed;								// ��ǰ�ٶ�
		uint16_t m_uFireAngle;							// ����Ƕ�
		uint16_t m_uFlyAngle;							// ���нǶ�
		ObjectStatus m_status;							// �ɻ���ǰ״̬
		PlaneViewRange m_viewRange;						// ��Ұ��Χ
	};
}

#endif