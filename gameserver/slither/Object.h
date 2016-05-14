#ifndef GAMESERVER_SLITHER_OBJECT_H
#define GAMESERVER_SLITHER_OBJECT_H

#include <stdint.h>
#include <string.h>
#include <math.h>

namespace slither {
#define PI 3.14159f

	struct Vector2D {
		float x;
		float y;

		Vector2D() : x(0), y(0) {
		}

		Vector2D(float _x, float _y) : x(_x), y(_y) {
		}

		float Magnitude() {
			return ::sqrt(x * x + y * y);
		}
	};

	enum ObjectStatus {
		OBJ_EXIST = 0,					// ����
		OBJ_DESTROY = 1,				// ����
	};

	enum ObjectType {
		Snake_Head_Type = 1,		// ����
		Snake_Body_Type = 2,		// ����
		Food_Type = 3,				// ʳ��
	};

	class Object
	{
	public:
		Object() {
		}
		Object(const Vector2D& pos, float fRadius, ObjectType type) :m_pos(pos), m_fRadius(fRadius), m_objType(type) {
		}
		virtual ~Object() {
		}

	public:
		void Clear() {
			memset(this, 0, sizeof(*this));
		}

		// ��ȡ��������ĵ�
		Vector2D& GetPos() {
			return m_pos;
		}

		// �����������ĵ�
		void SetPos(Vector2D pos) {
			m_pos.x = pos.x;
			m_pos.y = pos.y;
		}

		// ��ȡ��������
		ObjectType GetType() {
			return m_objType;
		}

		// ���ð뾶
		void SetRadius(float fRadius) {
			m_fRadius = fRadius;
		}

		// ��ȡ�뾶
		float GetRadius() {
			return m_fRadius;
		}

		// ������ײ���
		bool IsCollide(Object* pObj) {
			if (!pObj) {
				return false;
			}

			// ����Բ����ײ��⣬ֻҪ�ж�Բ������Ƿ��������Բ�İ뾶����
			float fLen = sqrt(pow((pObj->GetPos().x - m_pos.x), 2) + pow((pObj->GetPos().y - m_pos.y), 2));
			if (fLen < pObj->GetRadius() + m_fRadius) {
				return true;
			}

			return false;
		}

		// �ж�ĳ�����Ƿ񱻰��� 
		bool IsContained(const Vector2D& pos) {
			// ����㵽Բ�ĵľ���С�ڰ뾶�����Ǳ�������
			float fLen = sqrt(pow((pos.x - m_pos.x), 2) + pow((pos.y - m_pos.y), 2));
			if (fLen < m_fRadius) {
				return true;
			}

			return false;
		}

	protected:
		Vector2D m_pos;							// �������ĵ�
		float m_fRadius;						// ����뾶
		ObjectType m_objType;					// ��������
	};
}

#endif