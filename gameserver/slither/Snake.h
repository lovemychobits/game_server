#ifndef GAMESERVER_SLITHER_SNAKE_H
#define GAMESERVER_SLITHER_SNAKE_H

#include "Object.h"
#include "../../network/IConnection.h"
#include "../../protocol/slither_battle.pb.h"
#include "SlitherMath.h"
#include <vector>
#include <list>
#include <set>
using namespace std;
using namespace cpnet;

namespace slither {
	class Snake;
	class Food;
	struct Grid;

	// 蛇身节点
	class SnakeBodyNode : public Object {
	public:
		SnakeBodyNode(Snake* pOwner, uint16_t uNodeId, SnakeBodyNode* pPrevNode, const Vector2D& pos, float fRadius, float fSpeed, float fAngle);
		virtual ~SnakeBodyNode();

		SnakeBodyNode* GetPrevNode() {
			return m_pPrevNode;
		}

		void SetAngle(float fAngle) {
			m_angle = fAngle;
		}

		float GetAngle() {
			return m_angle;
		}

		float GetSpeed();

		void SetSpeed(float fSpeed) {
			m_fSpeed = fSpeed;
		}

		Snake* GetOwner() {
			return m_pOwner;
		}

		void SetObjectType(ObjectType type) {
			m_objType = type;
		}

		uint16_t GetNodeId() {
			return m_uNodeId;
		}

		void TracePreNode();

	private:
		float GetBodyNodeDist(float fRadius, float fRatio);

	protected:
		Snake* m_pOwner;						// 属于那条蛇
		uint16_t m_uNodeId;						// 节点
		SnakeBodyNode* m_pPrevNode;				// 前一个身体节点, 如果是NULL，前一个节点就是SnakeHead
		float m_fSpeed;							// 速度
		float m_angle;							// 移动角度
		Vector2D m_lastMove;
		bool m_bIsMove;							// 是否移动过
	};

	// 蛇的统计（只统计本局的）
	struct SnakeStatistics {
		uint32_t uRank;							// 排名
		uint32_t uKillSnakeTimes;				// 击杀蛇的次数
		uint32_t uEatNum;						// 吞噬数量
		uint32_t uMaxLength;					// 最大长度
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
		void EatFood(Food* pFood);
		void SpeedUpCost();							// 加速消耗
		void ClearInGrid();

	public:
		uint32_t GetSnakeId() {
			return m_uSnakeId;
		}

		void SetAngle(float fAngle) {
			m_pHead->SetAngle(fAngle);
		}
		
		// 设置是否加速
		void SetSpeedUp(bool bSpeedUp) {
			m_bSpeedUp = bSpeedUp;
		}

		bool GetSpeedUp() {
			return m_bSpeedUp;
		}

		void SetStopMove(bool bStopMove) {
			m_bStopMove = bStopMove;
		}

		SnakeBodyNode* GetSnakeHead() {
			return m_pHead;
		}

		SnakeBodyNode* GetSnakeTail() {
			return m_pTail;
		}

		void SetSnakeTail(SnakeBodyNode* pTail) {
			m_pTail = pTail;
		}

		list<SnakeBodyNode*>& GetSnakeBody() {
			return m_bodyList;
		}

		void SetConnection(IConnection* pConn) {
			m_pConn = pConn;
		}

		IConnection* GetConnection() {
			return m_pConn;
		}

		void SetStatus(ObjectStatus status) {
			m_status = status;
		}

		ObjectStatus GetStatus() {
			return m_status;
		}

		float GetViewRange() {
			//return m_uViewRange;						// 后面改成根据蛇的长度重量，来判断
			return GetSnakeView();
		}

		void ClearViews() {
			m_viewRangeSnakeSet.clear();
			m_viewRangeGridSet.clear();
		}

		bool IsRobot() {
			return m_bRobot;
		}

		uint32_t GetTotalMass() {
			return m_fTotalMass;
		}

		uint32_t GetTotalMass() const {
			return m_fTotalMass;
		}

		uint32_t GetMoveTick() {
			return m_uMoveTick;
		}

		void SetScene(Scene* pScene) {
			m_pScene = pScene;
		}

		Scene* GetScene() {
			return m_pScene;
		}

		void IncKillTimes() {
			++m_snakeStatistics.uKillSnakeTimes;
		}

		const SnakeStatistics& GetStatistics() const {
			return m_snakeStatistics;
		}

		void AddEatNum(uint32_t uEatNum) {
			m_snakeStatistics.uKillSnakeTimes += uEatNum;
		}

		void SetMaxLength(uint32_t uLength) {
			if (uLength > m_snakeStatistics.uMaxLength) {
				m_snakeStatistics.uMaxLength = uLength;
			}
		}

		void SetRank(uint32_t uRank) {
			m_snakeStatistics.uRank = uRank;
		}

		uint32_t GetMsgCount() {
			return m_uMsgCount;
		}

		uint32_t GetMsgSize() {
			return m_uMsgSize;
		}

		void SetNickName(const string& strNickName) {
			m_strNickName = strNickName;
		}

		const string& GetNickName() {
			return m_strNickName;
		}

		void SetSkinId(uint32_t uSkinId) {
			m_uSkinId = uSkinId;
		}

		uint32_t GetSkinId() {
			return m_uSkinId;
		}

		bool IsInView(Snake* pSnake);
		bool HasInView(Snake* pSnake);
		bool IsInView(Grid* pGrid);
		bool HasInView(uint32_t uGridId);
		void AddInView(Snake* pSnake);
		void AddInView(uint32_t uGridId);
		void DelInView(Snake* pSnake);
		void DelInView(uint32_t uGridId);

		void SendMsg(const char* pData, uint32_t uLen);

	private:
		void AddBodyNode(SnakeBodyNode* pNode) {
			m_bodyList.push_back(pNode);
		}

		SnakeBodyNode* IncTail();											// 添加尾部
		void DecTail();														// 减少尾部
		void IncRadius();													// 增加半径
		void DecRadius();													// 减少半径

		uint16_t GetSinAngle(uint16_t uAngle);								// 根据unity坐标系中的角度，计算出对于的直角坐标系中斜边sin对应的角度
		float GetXLen(float vectLen, uint16_t uSinAngle);					// 获取飞行一段距离后，x坐标的偏移量
		float GetYLen(float vectLen, uint16_t uSinAngle);					// 获取飞行一段距离后，y坐标的偏移量

		void GetSlitherParam(float& fCurRadius, uint32_t& uCurNode);
		float GetSnakeView();												// 获取当前视野

	private:
		Scene* m_pScene;													// 所属的场景
		uint32_t m_uSnakeId;												// 蛇ID
		bool m_bSpeedUp;													// 是否加速
		bool m_bStopMove;													// 是否停止运动（测试使用的）
		SnakeBodyNode* m_pHead;												// 蛇头
		SnakeBodyNode* m_pTail;												// 蛇尾
		list<SnakeBodyNode*> m_bodyList;									// 蛇的身体
		IConnection* m_pConn;												// 对应的网络连接
		ObjectStatus m_status;												// 蛇的状态
		bool m_bRobot;														// 是否为机器人
		uint16_t m_uViewRange;												// 视野范围
		uint16_t m_uMoveTick;												// 已经动了几次了
		float m_fTotalMass;													// 总的Mass
		uint16_t m_uNodeId;													// 节点Id

		set<uint32_t> m_viewRangeSnakeSet;									// 视野内的其他蛇
		set<uint32_t> m_viewRangeGridSet;									// 视野内的格子

		SnakeStatistics m_snakeStatistics;									// 蛇的统计
		uint32_t m_uMsgCount;												// 发送的消息次数（测试使用）
		uint32_t m_uMsgSize;												// 发送的消息量（测试使用）

		string m_strNickName;												// 赛局昵称
		uint32_t m_uSkinId;													// 皮肤ID
	};
}



#endif