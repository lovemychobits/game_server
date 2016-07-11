#ifndef GAMESERVER_SLITHER_SNAKE_H
#define GAMESERVER_SLITHER_SNAKE_H

#include "Object.h"
#include "../../network/IConnection.h"
#include "../../protocol/slither_battle.pb.h"
#include "SlitherMath.h"
#include "Buff.h"
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

		const ObjectGrids& GetObjectGrids() {
			return m_objGrids;
		}

		void SetObjectGrids(ObjectGrids& grids) {
			m_objGrids = grids;
		}

		// 扇形包含判断
		bool IsContainedInSector(const Vector2D& pos) {
			// 先判断是否被圆包含
			if (!IsContained(pos)) {
				return false;
			}

			Vector2D thetaPos(pos.x - m_pos.x, pos.y - m_pos.y);
			float theta = SlitherMath::PosToAnglePI(thetaPos);
			
			float theta1 = m_angle + 1.0f;
			if (theta1 > 2 * PI) {
				theta1 = theta1 - 2 * PI;
			}

			float theta2 = m_angle - 1.0f;
			if (theta2 < 0.0f) {
				theta2 = 2 * PI + theta2;
			}

			if (theta < 0.5f * PI) {									// 第一象限内
				if (theta <= theta1) {
					return true;
				}
			}
			if (theta < 1.5f * PI) {									// 第二，第三象限
				if (theta <= theta1 && theta >= theta2) {				
					return true;
				}
			}
			else {														// 第四象限
				if (theta >= theta2) {
					return true;
				}
			}
			
			return false;
		}


		void TracePreNode(Vector2D& preNodePos, Vector2D& headDeltaVec);

	private:
		inline float GetBodyNodeDist(float fRadius, float fRatio);

	protected:
		Snake* m_pOwner;						// 属于那条蛇
		uint16_t m_uNodeId;						// 节点
		SnakeBodyNode* m_pPrevNode;				// 前一个身体节点, 如果是NULL，前一个节点就是SnakeHead
		float m_fSpeed;							// 速度
		float m_angle;							// 移动角度
		Vector2D m_lastMove;					// 最后一次移动的向量
		bool m_bIsMove;							// 是否移动过
		ObjectGrids m_objGrids;					// 此节点当前所占的格子
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

		void SetPlayerId(uint64_t uPlayerId) {
			m_uPlayerId = uPlayerId;
		}

		uint64_t GetPlayerId() const{
			return m_uPlayerId;
		}

		void SetGender(bool gender) {
			m_gender = gender;
		}

		bool GetGender() {
			return m_gender;
		}

		void SetPlayerName(const string& strPlayerName) {
			m_strPlayerName = strPlayerName;
		}

		const string& GetPlayerName() {
			return m_strPlayerName;
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
			if (m_pConn && m_pConn->IsConnected()) {
				m_pConn->Shutdown();					// 如果之前连接还存在，则先关闭
			}
			m_pConn = pConn;
		}

		IConnection* GetConnection() {
			return m_pConn;
		}

		void SetStatus(ObjectStatus status) {
			m_status = status;
			if (m_status == OBJ_DESTROY) {
				m_deadTime = boost::posix_time::second_clock::local_time();
			}
		}

		// 获取已经死亡了的时间
		uint32_t GetHasDeadTime() {
			boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
			boost::posix_time::time_duration diff = end - m_deadTime;
			return diff.total_seconds();
		}

		ObjectStatus GetStatus() {
			return m_status;
		}

		float GetViewRange() {
			return GetSnakeView();
		}

		void ClearViews() {
			m_viewRangeSnakeSet.clear();
			m_viewRangeGridSet.clear();
		}

		bool IsRobot() {
			return m_bRobot;
		}

		float GetTotalMass() {
			return m_fTotalMass;
		}

		void SetTotalMass(float fMass) {
			m_fTotalMass = fMass;
		}

		float GetTotalMass() const {
			return m_fTotalMass;
		}

		uint32_t GetMoveTick() {
			return m_uMoveTick;
		}

		void ResetMoveTick() {
			m_uMoveTick = 0;
		}

		void SetScene(Scene* pScene) {
			m_pScene = pScene;
		}

		Scene* GetScene() {
			return m_pScene;
		}

		void SetStatistics(SnakeStatistics* pStatistics) {
			m_pSnakeStatistics = pStatistics;
			m_pSnakeStatistics->uLeaveRoomTime = 0;						// 将离开时间清空
		}

		const SnakeStatistics& GetStatistics() const {
			return *m_pSnakeStatistics;
		}

		void IncKillTimes() {
			if (!m_pSnakeStatistics) {
				return;
			}
			++m_pSnakeStatistics->uKillSnakeTimes;
		}

		void IncDeadTimes() {
			if (!m_pSnakeStatistics) {
				return;
			}
			++m_pSnakeStatistics->uDeadTimes;
		}

		void AddEatNum(uint32_t uEatNum) {
			if (!m_pSnakeStatistics) {
				return;
			}
			m_pSnakeStatistics->uEatNum += uEatNum;
		}

		void SetMaxLength(uint32_t uLength) {
			if (!m_pSnakeStatistics) {
				return;
			}
			if (uLength > m_pSnakeStatistics->uMaxLength) {
				m_pSnakeStatistics->uMaxLength = uLength;
			}
		}

		void SetRank(uint32_t uRank) {
			if (!m_pSnakeStatistics) {
				return;
			}
			m_pSnakeStatistics->uRank = uRank;
		}

		uint32_t GetRank() {
			if (!m_pSnakeStatistics) {
				return 0;
			}
			return m_pSnakeStatistics->uRank;
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

		const set<uint32_t>& GetGridsInView() {
			return m_gridsInView;
		}

		void SetGridsInView(set<uint32_t>& gridsInView) {
			m_gridsInView = gridsInView;
		}

		SnakeViewPoint& GetSnakeViewPoint() {
			return m_snakeViewPoint;
		}

		void SetSnakeViewPoint(SnakeViewPoint& viewPoint) {
			m_snakeViewPoint = viewPoint;
		}

		uint32_t GetTeamId() {
			return m_uTeamId;
		}

		void SetTeamId(uint32_t uTeamId) {
			m_uTeamId = uTeamId;
		}

		SnakeBuffStat& GetSnakeBuffStat() {
			return m_buffStat;
		}

		void SetLeaveTime();

		bool IsInView(Snake* pSnake, const set<uint32_t>& gridSet);
		bool HasInView(Snake* pSnake);
		bool IsInView(Grid* pGrid);
		bool HasInView(uint32_t uGridId);
		void AddInView(Snake* pSnake);
		void AddInView(uint32_t uGridId);
		static void GetSlitherParam(float fTotalMass, float& fCurRadius, uint32_t& uCurNode);

		void SendMsg(const char* pData, uint32_t uLen);

	private:
		void AddBodyNode(SnakeBodyNode* pNode) {
			m_bodyList.push_back(pNode);
		}

		SnakeBodyNode* IncTail();											// 添加尾部
		void DecTail();														// 减少尾部
		void IncRadius(float fNewRadius);									// 增加半径
		void DecRadius(float fNewRadius);									// 减少半径

		uint16_t GetSinAngle(uint16_t uAngle);								// 根据unity坐标系中的角度，计算出对于的直角坐标系中斜边sin对应的角度
		float GetXLen(float vectLen, uint16_t uSinAngle);					// 获取飞行一段距离后，x坐标的偏移量
		float GetYLen(float vectLen, uint16_t uSinAngle);					// 获取飞行一段距离后，y坐标的偏移量
		void GetSlitherParam(float& fCurRadius, uint32_t& uCurNode);
		
		float GetSnakeView();												// 获取当前视野

	private:
		Scene* m_pScene;													// 所属的场景
		uint64_t m_uPlayerId;												// 玩家Id
		bool m_gender;														// 性别
		string m_strPlayerName;												// 玩家名
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

		SnakeStatistics* m_pSnakeStatistics;								// 蛇的统计

		struct MsgInfo {
			uint32_t uCount;
			uint32_t uSize;
			MsgInfo() : uCount(0), uSize(0) {

			}
		};

		string m_strNickName;												// 赛局昵称
		uint32_t m_uSkinId;													// 皮肤ID
		uint32_t m_uTeamId;													// 蛇所在的队伍
		set<uint32_t> m_gridsInView;										// 视野内的格子
		SnakeViewPoint m_snakeViewPoint;									// 蛇视野范围点

		boost::posix_time::ptime m_deadTime;								// 蛇死亡时间

		// 蛇的buff
		SnakeBuffStat m_buffStat;
	};
}



#endif