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
	// 蛇身节点
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
		Snake* m_pOwner;						// 属于那条蛇
		SnakeBodyNode* m_pPrevNode;				// 前一个身体节点, 如果是NULL，前一个节点就是SnakeHead
		float m_fSpeed;							// 速度
		uint16_t m_angle;						// 移动角度
	};

	class Scene;
	// 蛇
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
		
		// 设置是否加速
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
			return m_uViewRange;						// 后面改成根据蛇的长度重量，来判断
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

		uint16_t GetSinAngle(uint16_t uAngle);								// 根据unity坐标系中的角度，计算出对于的直角坐标系中斜边sin对应的角度
		float GetXLen(float vectLen, uint16_t uSinAngle);					// 获取飞行一段距离后，x坐标的偏移量
		float GetYLen(float vectLen, uint16_t uSinAngle);					// 获取飞行一段距离后，y坐标的偏移量
		void CalcNextPos(uint16_t uAngle, float fSpeed, Vector2D& pos);		// 根据角度和速度，算出下一个坐标点位置

	private:
		Scene* m_pScene;													// 所属的场景
		uint32_t m_uSnakeId;												// 蛇ID
		bool m_bSpeedUp;													// 是否加速
		SnakeBodyNode* m_pHead;												// 蛇头
		vector<SnakeBodyNode*> m_bodyVec;									// 蛇的身体
		IConnection* m_pConn;												// 对应的网络连接
		ObjectStatus m_status;												// 蛇的状态
		bool m_bRobot;														// 是否为机器人
		uint16_t m_uViewRange;												// 视野范围
		uint16_t m_uMoveTick;												// 已经动了几次了

		set<uint32_t> m_viewRangeSnakeSet;									// 视野内的其他蛇
		set<Grid*> m_viewRangeGridSet;										// 视野内的格子

	};
}



#endif