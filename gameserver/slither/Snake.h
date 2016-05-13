#ifndef GAMESERVER_SLITHER_SNAKE_H
#define GAMESERVER_SLITHER_SNAKE_H

#include "Object.h"
#include "../../network/IConnection.h"
#include "../../protocol/slither_client.pb.h"
#include <vector>
#include <list>
#include <set>
using namespace std;
using namespace cpnet;

namespace slither {
	class Snake;
	struct Grid;
	// ����ڵ�
	class SnakeBodyNode : public Object {
	public:
		SnakeBodyNode(Snake* pOwner, SnakeBodyNode* pPrevNode, const Vector2D& pos, float fRadius, float fSpeed, uint16_t uAngle);
		virtual ~SnakeBodyNode();

		SnakeBodyNode* GetPrevNode() {
			return m_pPrevNode;
		}

		void SetAngle(uint16_t uAngle) {
			m_angle = uAngle;
		}

		uint16_t GetAngle() {
			return m_angle;
		}

		float GetSpeed() {
			return m_fSpeed;
		}

		void SetSpeed(float fSpeed) {
			m_fSpeed = fSpeed;
		}

		Snake* GetOwner() {
			return m_pOwner;
		}

		void SetObjectType(ObjectType type) {
			m_objType = type;
		}

	protected:
		Snake* m_pOwner;						// ����������
		SnakeBodyNode* m_pPrevNode;				// ǰһ������ڵ�, �����NULL��ǰһ���ڵ����SnakeHead
		float m_fSpeed;							// �ٶ�
		uint16_t m_angle;						// �ƶ��Ƕ�
	};

	class Scene;
	// ��
	class Snake {
	public:
		Snake(Scene* pScene, uint32_t uSnakeId, float fRadius, const Vector2D& initPos, uint32_t uBodySize, bool bRobot);
		~Snake();
		void SerializeToPB(slither::PBSnake& pbSnake, bool bJustHead = false);

	public:
		void Move();
		

	public:
		uint32_t GetSnakeId() {
			return m_uSnakeId;
		}

		void SetAngle(uint16_t uAngle) {
			m_pHead->SetAngle(uAngle);
		}
		
		// �����Ƿ����
		void SetSpeedUp(bool bSpeedUp) {
			m_bSpeedUp = bSpeedUp;
		}

		SnakeBodyNode* GetSnakeHead() {
			return m_pHead;
		}

		vector<SnakeBodyNode*>& GetSnakeBody() {
			return m_bodyVec;
		}

		void SetConnection(IConnection* pConn) {
			m_pConn = pConn;
		}

		void SetStatus(ObjectStatus status) {
			m_status = status;
		}

		ObjectStatus GetStatus() {
			return m_status;
		}

		uint32_t GetViewRange() {
			return m_uViewRange;						// ����ĳɸ����ߵĳ������������ж�
		}

		bool IsRobot() {
			return m_bRobot;
		}

		bool IsInView(Snake* pSnake);
		bool HasInView(Snake* pSnake);
		bool IsInView(Grid* pGrid);
		bool HasInView(Grid* pGrid);
		void AddInView(Snake* pSnake);
		void AddInView(Grid* pGrid);
		void DelInView(Snake* pSnake);
		void DelInView(Grid* pGrid);

		void SendMsg(const char* pData, uint32_t uLen);

	private:
		void AddBodyNode(SnakeBodyNode* pNode) {
			m_bodyVec.push_back(pNode);
		}

		uint16_t GetSinAngle(uint16_t uAngle);								// ����unity����ϵ�еĽǶȣ���������ڵ�ֱ������ϵ��б��sin��Ӧ�ĽǶ�
		float GetXLen(float vectLen, uint16_t uSinAngle);					// ��ȡ����һ�ξ����x�����ƫ����
		float GetYLen(float vectLen, uint16_t uSinAngle);					// ��ȡ����һ�ξ����y�����ƫ����
		void CalcNextPos(uint16_t uAngle, float fSpeed, Vector2D& pos);		// ���ݽǶȺ��ٶȣ������һ�������λ��

	private:
		Scene* m_pScene;													// �����ĳ���
		uint32_t m_uSnakeId;												// ��ID
		bool m_bSpeedUp;													// �Ƿ����
		SnakeBodyNode* m_pHead;												// ��ͷ
		vector<SnakeBodyNode*> m_bodyVec;									// �ߵ�����
		IConnection* m_pConn;												// ��Ӧ����������
		ObjectStatus m_status;												// �ߵ�״̬
		bool m_bRobot;														// �Ƿ�Ϊ������
		uint16_t m_uViewRange;												// ��Ұ��Χ
		uint16_t m_uMoveTick;												// �Ѿ����˼�����

		set<uint32_t> m_viewRangeSnakeSet;									// ��Ұ�ڵ�������
		set<Grid*> m_viewRangeGridSet;										// ��Ұ�ڵĸ���

	};
}



#endif