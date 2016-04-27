#ifndef GAMESERVER_PLANESHOOTING_PLANE_H
#define GAMESERVER_PLANESHOOTING_PLANE_H

#include <stdint.h>
#include "Object.h"
#include "../../network/IConnection.h"
using namespace cpnet;

namespace plane_shooting {
	

	enum PlaneStatus {
		ALIVE = 0,					// ����
		DEAD = 1,					// ����
	};

#define MOVE_STEP 1

	// �ɻ���Ұ��Χ��Ҳ��������������ɻ��������ӵ����������Χ�ˣ�����Ҫ֪ͨ�ɻ�
	struct PlaneViewRange
	{
		uint16_t uWidth;			// ����
		uint16_t uHeight;			// ���
	};

	class Plane : public Object
	{
	public:
		Plane();
		Plane(uint16_t uPlaneId, Vector2D& pos, int8_t orientation, IConnection* pConn);
		~Plane();

	public:
		// ��Ϊ������
		void Left(uint32_t uLeftTimes = 1);					// ����
		void Right(uint32_t uRightTimes = 1);				// ����
		void Up(uint32_t uUpTimes = 1);						// ����
		void Down(uint32_t uDownTimes = 1);					// ����
		void TurnAround();									// ��ͷ

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

		bool NeedNotify(Object* pObj);					// �Ƿ���Ҫ֪ͨ�Լ�

	private:
		bool IsInViewRange(Vector2D& pos);				// �Ƿ�����Ұ��

	private:
		IConnection* m_pConn;
		int8_t m_orientation;							// ����1Ϊ���ϣ�-1Ϊ����
		uint16_t m_curSpeed;							// ��ǰ�ٶ�
		PlaneStatus m_status;							// �ɻ���ǰ״̬
		PlaneViewRange m_viewRange;						// ��Ұ��Χ
	};
}

#endif