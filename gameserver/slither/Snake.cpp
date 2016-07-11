#include "Snake.h"
#include "Scene.h"
#include "Food.h"
#include "../../utils/Utility.h"
#include "SlitherMath.h"
#include "../config/SlitherConfig.h"
#include "../../protocol/slither_battle.pb.h"
#include "GameRoom.h"
#include <math.h>

namespace slither {

	SnakeBodyNode::SnakeBodyNode(Snake* pOwner, uint16_t uNodeId, SnakeBodyNode* pPrevNode, const Vector2D& pos, float fRadius, float fSpeed, float fAngle) :
		Object(pos, fRadius, Snake_Body_Type), m_pOwner(pOwner), m_uNodeId(uNodeId), m_pPrevNode(pPrevNode), m_fSpeed(fSpeed), m_angle(fAngle), m_bIsMove(false) {
	}

	SnakeBodyNode::~SnakeBodyNode() {

	}

	float SnakeBodyNode::GetSpeed() {
		float fSpeed = 0.0f;
		if (!m_pOwner->GetSpeedUp()) {
			fSpeed = m_fSpeed;
		}
		else {
			fSpeed = m_fSpeed * g_slitherConfig.m_fSpeedUpRate;
		}

		uint32_t uSimTimes = g_slitherConfig.m_uSnakeSimTimes / (1000 / g_slitherConfig.m_uSimInterval);					// 每个tick的模拟次数
		return (fSpeed / uSimTimes);
	}

	void SnakeBodyNode::TracePreNode(Vector2D& preNodePos, Vector2D& headDeltaVec) {
		if (!m_pPrevNode) {
			return;
		}

		static Vector2D distVect;
		distVect.x = preNodePos.x - m_pos.x;
		distVect.y = preNodePos.y - m_pos.y;

		float fSnakeRadius = m_pOwner->GetSnakeHead()->GetRadius();
		float fDist = GetBodyNodeDist(fSnakeRadius, g_slitherConfig.m_fBodyInterval);
		float fMoveDist = distVect.Magnitude() - fDist;
		float fMoveSpeed = headDeltaVec.Magnitude();
		fMoveDist = fMoveDist > fMoveSpeed ? fMoveSpeed : fMoveDist;

		if (distVect.Magnitude() >= fDist) {
			m_lastMove = SlitherMath::MoveTo(distVect, fMoveDist);
			if (m_lastMove.Magnitude() > 1) {
				return;
			}
			m_pos.x += m_lastMove.x;
			m_pos.y += m_lastMove.y;
			preNodePos = m_pos;
		}
	}

	inline float SnakeBodyNode::GetBodyNodeDist(float fRadius, float fRatio) {
		return 2 * fRadius * (1 - fRatio);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Snake::Snake(Scene* pScene, uint32_t uSnakeId, float fRadius, const Vector2D& initPos, uint32_t uBodySize, bool bRobot) : 
		m_pScene(pScene), m_uPlayerId(0), m_uSnakeId(uSnakeId), m_bSpeedUp(false), m_bStopMove(false), m_pConn(NULL), m_status(OBJ_EXIST), m_bRobot(bRobot),
		m_uViewRange(20), m_uMoveTick(0), m_fTotalMass((float)g_slitherConfig.m_uInitSnakeMass), m_uNodeId(1), m_pSnakeStatistics(NULL), m_uTeamId(0)
	{
		m_pHead = new SnakeBodyNode(this, m_uNodeId++, NULL, initPos, fRadius, g_slitherConfig.m_fSpeed / (1000.0f / g_slitherConfig.m_uSimInterval), 0);
		m_pHead->SetObjectType(Snake_Head_Type);
		ObjectGrids newGrids = m_pScene->GetObjectGrids(m_pHead);
		m_pHead->SetObjectGrids(newGrids);

		Vector2D nodePos = initPos;

		SnakeBodyNode* pPrevNode = m_pHead;
		for (uint32_t i = 0; i < uBodySize; ++i) {
			nodePos.y -= (1 - g_slitherConfig.m_fBodyInterval) * fRadius;
			SnakeBodyNode* pSnakeNode = new SnakeBodyNode(this, m_uNodeId++, pPrevNode, nodePos, fRadius, g_slitherConfig.m_fSpeed / (1000.0f / g_slitherConfig.m_uSimInterval), 0);
			newGrids = m_pScene->GetObjectGrids(pSnakeNode);
			pSnakeNode->SetObjectGrids(newGrids);

			pPrevNode = pSnakeNode;

			m_bodyList.push_back(pSnakeNode);
		}

		// 设置蛇尾
		SetSnakeTail(pPrevNode);
	}

	Snake::~Snake() {
		if (m_pHead) {
			delete m_pHead;
		}
		list<SnakeBodyNode*>::iterator bodyIt = m_bodyList.begin();
		list<SnakeBodyNode*>::iterator bodyItEnd = m_bodyList.end();
		for (; bodyIt != bodyItEnd; bodyIt++) {
			if (*bodyIt) {
				delete *bodyIt;
			}
		}
	}

	void Snake::SerializeToPB(slither::PBSnake& pbSnake, bool bJustHead) {
		if (!m_pHead) {
			return;
		}

		pbSnake.set_snakeid(m_uSnakeId);
		pbSnake.set_speedup(m_bSpeedUp);

		// 序列化蛇头
		pbSnake.mutable_snakehead()->mutable_pos()->set_x(m_pHead->GetPos().x);
		pbSnake.mutable_snakehead()->mutable_pos()->set_y(m_pHead->GetPos().y);
		pbSnake.mutable_snakehead()->set_angle(m_pHead->GetAngle());
		pbSnake.mutable_snakehead()->set_radius(m_pHead->GetRadius());
		pbSnake.mutable_snakehead()->set_speed(m_pHead->GetSpeed() * (1000.0f / g_slitherConfig.m_uSimInterval));
		pbSnake.set_totalmass((uint32_t)m_fTotalMass);

		if (bJustHead) {					// 如果只需要序列化头
			pbSnake.set_status(slither::UPDATE);
			return;
		}

		pbSnake.set_nickname(m_strNickName);
		pbSnake.set_skinid(m_uSkinId);

		// 序列化蛇身
		list<SnakeBodyNode*>::iterator bodyIt = m_bodyList.begin();
		list<SnakeBodyNode*>::iterator bodyItEnd = m_bodyList.end();
		for (; bodyIt != bodyItEnd; bodyIt++) {
			SnakeBodyNode* pBodyNode = *bodyIt;
			slither::PBSnakeBody* pPBSnakeBody = pbSnake.add_snakebody();
			pPBSnakeBody->mutable_pos()->set_x(pBodyNode->GetPos().x);
			pPBSnakeBody->mutable_pos()->set_y(pBodyNode->GetPos().y);
		}
	}

	// 模拟蛇运动
	void Snake::Move() {
		if (!m_pHead) {
			return;
		}

		if (m_bStopMove) {
			return;
		}

		if (m_bRobot && m_uMoveTick % 15 == 0) {
			float fRandAngle = cputil::GenFloatRandom(0.1f, 3.0f);
			this->SetAngle(fRandAngle);
			//m_pHead->SetSpeed(0.0f);
		}
		
		uint32_t uSimTimes = g_slitherConfig.m_uSnakeSimTimes / (1000 / g_slitherConfig.m_uSimInterval);			// 每个tick的模拟次数
		for (uint32_t i = 0; i < uSimTimes; ++i) {

			// 蛇头运动
			const ObjectGrids& oldGrids = m_pHead->GetObjectGrids();
			Vector2D oldPos = m_pHead->GetPos();
			Vector2D newPos = SlitherMath::MoveToAngle(m_pHead->GetPos(), m_pHead->GetAngle(), m_pHead->GetSpeed());
			m_pHead->SetPos(newPos);

			ObjectGrids newGrids = m_pScene->GetObjectGrids(m_pHead);
			newGrids.UpdateGrids(m_pHead, m_pScene->GetSceneGrids(), oldGrids);
			m_pHead->SetObjectGrids(newGrids);

			Vector2D headDeltaVec(newPos.x - oldPos.x, newPos.y - oldPos.y);
			Vector2D preNodePos = m_pHead->GetPos();

			// 蛇身每节的运动
			list<SnakeBodyNode*>::iterator bodyIt = m_bodyList.begin();
			list<SnakeBodyNode*>::iterator bodyItEnd = m_bodyList.end();
			for (; bodyIt != bodyItEnd; bodyIt++) {
				SnakeBodyNode* pBodyNode = *bodyIt;
				if (!pBodyNode) {														// 发生了错误
					return;
				}

				const ObjectGrids& oldGrids = pBodyNode->GetObjectGrids();

				pBodyNode->TracePreNode(preNodePos, headDeltaVec);									// 跟随前一个点

				ObjectGrids newGrids = m_pScene->GetObjectGrids(pBodyNode);
				newGrids.UpdateGrids(pBodyNode, m_pScene->GetSceneGrids(), oldGrids);
				pBodyNode->SetObjectGrids(newGrids);
			}
		}

		// 每移动20次，将一些不在视野内的物体，或者格子删除
		bool bNeedClearSnake = false;
		slither::NotifyClearSnakeInView clearSnakeNty;
		if (m_uMoveTick++ % 20 == 0) {
			set<uint32_t> gridSet;
			m_pScene->GetInViewGrids(this, gridSet);

			// 判断蛇
			set<uint32_t>::iterator snakeIt = m_viewRangeSnakeSet.begin();
			set<uint32_t>::iterator snakeItEnd = m_viewRangeSnakeSet.end();
			for (; snakeIt != snakeItEnd; ) {
				Snake* pSnake = m_pScene->GetSnakeById(*snakeIt);
				if (!pSnake) {
					m_viewRangeSnakeSet.erase(snakeIt++);
					continue;
				}

				if (!IsInView(pSnake, gridSet) || pSnake->GetStatus() == OBJ_DESTROY) {			// 不在自己视野范围内了, 或者已经死亡了
					clearSnakeNty.add_snakelist(pSnake->GetSnakeId());
					bNeedClearSnake = true;
					m_viewRangeSnakeSet.erase(snakeIt++);
					continue;
				}
				snakeIt++;
			}

			// 判断格子
			set<uint32_t>::iterator gridIt = m_viewRangeGridSet.begin();
			set<uint32_t>::iterator gridItEnd = m_viewRangeGridSet.end();
			for (; gridIt != gridItEnd;) {
				set<uint32_t>::iterator findIt = gridSet.find(*gridIt);
				if (findIt == gridSet.end()) {													// 不在视野范围内了
					m_viewRangeGridSet.erase(gridIt++);
					continue;
				}

				gridIt++;
			}
		}
		m_uMoveTick++;

		// 通知客户端需要删除的蛇
		if (bNeedClearSnake) {
			string strResponse;
			cputil::BuildResponseProto(clearSnakeNty, strResponse, slither::NOTIFY_CLEAR_IN_VIEW);
			this->SendMsg(strResponse.c_str(), strResponse.size());
		}
	}

	// 吃食物
	void Snake::EatFood(Food* pFood) {
 		if (!pFood) {
			return;
		}

		AddEatNum(pFood->GetMass());

		// 判断buff相关
		if (m_buffStat.HasBuff(SNAKE_MULTI_POWER)) {
			m_fTotalMass += pFood->GetMass() * 3;
		}
		else {
			m_fTotalMass += pFood->GetMass();
		}

		// 如果此food有特殊buff，给蛇加上
		if (pFood->GetBuffType() != INVALID_BUFF) {
			m_buffStat.AddBuff(pFood->GetBuffType());
		}

		SetMaxLength((uint32_t)m_fTotalMass);

		float fNewRadius = 0.0f;
		uint32_t uNewNodeNum = 0;
		GetSlitherParam(fNewRadius, uNewNodeNum);

		while (uNewNodeNum > m_bodyList.size()) {					// 增加蛇身节点
			IncTail();
		}

		while (fNewRadius > m_pHead->GetRadius() + 0.001f) {		// 增加蛇身半径
			IncRadius(fNewRadius);
		}	

		return;
	}

	void Snake::SpeedUpCost() {

		if (!m_bSpeedUp) {
			return;
		}

		// 减少重量
		float fEachTickCost = 1.0f / (1000.0f / (float)g_slitherConfig.m_uSimInterval);
		float fSpeedUpCost = (fEachTickCost / g_slitherConfig.m_fAttenuationInterval * g_slitherConfig.m_fAttenuationValue);					// 加速消耗值
		m_pSnakeStatistics->fSpeedUpCost += fSpeedUpCost;

		m_fTotalMass -= fSpeedUpCost;
		if (m_fTotalMass < (float)g_slitherConfig.m_uInitSnakeMass) {				// 不能低于蛇的初始重量
			m_fTotalMass = (float)g_slitherConfig.m_uInitSnakeMass;
		}

		float fNewRadius = 0.0f;
		uint32_t uNewNodeNum = 0;
		GetSlitherParam(fNewRadius, uNewNodeNum);

		if (uNewNodeNum < m_bodyList.size()) {										// 减少蛇身节点
			DecTail();
		}

		if (fNewRadius < m_pHead->GetRadius() - 0.001f) {
			DecRadius(fNewRadius);
		}
	}

	// 从格子中清空
	void Snake::ClearInGrid() {
		Grid* pGrids = m_pScene->GetSceneGrids();
		ObjectGrids oldGrids = m_pScene->GetObjectGrids(m_pHead);
		oldGrids.DeleteInGrids(m_pHead, pGrids);

		// 蛇身每节的运动
		list<SnakeBodyNode*>::iterator bodyIt = m_bodyList.begin();
		list<SnakeBodyNode*>::iterator bodyItEnd = m_bodyList.end();
		for (; bodyIt != bodyItEnd; bodyIt++) {
			SnakeBodyNode*& pBodyNode = *bodyIt;
			if (!pBodyNode) {				// ERROR
				return;
			}

			ObjectGrids oldGrids = m_pScene->GetObjectGrids(pBodyNode);
			oldGrids.DeleteInGrids(pBodyNode, pGrids);
		}
	}

	// 在尾部添加节点
	SnakeBodyNode* Snake::IncTail() {
		Vector2D tailPos = m_pTail->GetPos();

		SnakeBodyNode* pNewTail = new SnakeBodyNode(m_pHead->GetOwner(), m_uNodeId++, m_pTail, tailPos, m_pTail->GetRadius(), m_pTail->GetSpeed(), m_pTail->GetAngle());
		m_pTail = pNewTail;
		m_bodyList.push_back(pNewTail);

		ObjectGrids objectGrids = m_pScene->GetObjectGrids(pNewTail);
		pNewTail->SetObjectGrids(objectGrids);

		for (uint32_t i = 0; i < MAX_GRID_IN; ++i) {
			if (objectGrids.grids[i] == -1) {
				continue;
			}
			Grid* pGrids = m_pScene->GetSceneGrids();
			if (!pGrids) {
				return NULL;
			}
			pGrids[objectGrids.grids[i]].AddObj(pNewTail);
		}

		return pNewTail;
	}

	void Snake::DecTail() {
		if (m_bodyList.size() == 0) {
			return;
		}

		SnakeBodyNode* pOldTail = m_bodyList.back();
		if (!pOldTail) {
			ERRORLOG("snake id=[" << m_uSnakeId << " tail is null");
			return;
		}

		// 从格子中删除
		const ObjectGrids& objectGrids = pOldTail->GetObjectGrids();
		for (uint32_t i = 0; i < MAX_GRID_IN; ++i) {
			if (objectGrids.grids[i] == -1) {
				continue;
			}
			Grid* pGrids = m_pScene->GetSceneGrids();
			if (!pGrids) {
				return;
			}
			pGrids[objectGrids.grids[i]].DelObj(pOldTail);
		}

		if (pOldTail) {
			delete pOldTail;
		}

		m_bodyList.pop_back();
		m_pTail = m_bodyList.back();											// 设置新的尾巴
	}

	void Snake::IncRadius(float fNewRadius) {

		// 需要先从格子中删除，否则半径发生变化之后，会导致在之前的格子中有残留，引发指针异常
		const ObjectGrids& oldGrids = m_pHead->GetObjectGrids();
		m_pHead->SetRadius(fNewRadius);											// 设置新的半径
		ObjectGrids newGrids = m_pScene->GetObjectGrids(m_pHead);
		newGrids.UpdateGrids(m_pHead, m_pScene->GetSceneGrids(), oldGrids);
		m_pHead->SetObjectGrids(newGrids);

		list<SnakeBodyNode*>::iterator bodyIt = m_bodyList.begin();
		list<SnakeBodyNode*>::iterator bodyItEnd = m_bodyList.end();

		for (; bodyIt != bodyItEnd; bodyIt++) {
			SnakeBodyNode* pBodyNode = *bodyIt;
			if (!pBodyNode) {
				return;
			}

			const ObjectGrids& oldGrids = pBodyNode->GetObjectGrids();
			pBodyNode->SetRadius(fNewRadius);												// 更新身体节点的半径
			newGrids = m_pScene->GetObjectGrids(pBodyNode);
			newGrids.UpdateGrids(pBodyNode, m_pScene->GetSceneGrids(), oldGrids);
			pBodyNode->SetObjectGrids(newGrids);
		}
	}

	void Snake::DecRadius(float fNewRadius) {

		const ObjectGrids& oldGrids = m_pHead->GetObjectGrids();
		m_pHead->SetRadius(fNewRadius);														// 设置新的半径
		ObjectGrids newGrids = m_pScene->GetObjectGrids(m_pHead);
		newGrids.UpdateGrids(m_pHead, m_pScene->GetSceneGrids(), oldGrids);
		m_pHead->SetObjectGrids(newGrids);

		list<SnakeBodyNode*>::iterator bodyIt = m_bodyList.begin();
		list<SnakeBodyNode*>::iterator bodyItEnd = m_bodyList.end();
		for (; bodyIt != bodyItEnd; bodyIt++) {
			SnakeBodyNode* pBodyNode = *bodyIt;
			if (!pBodyNode) {
				return;
			}

			const ObjectGrids& oldGrids = pBodyNode->GetObjectGrids();
			pBodyNode->SetRadius(fNewRadius);												// 更新身体节点的半径
			newGrids = m_pScene->GetObjectGrids(pBodyNode);
			newGrids.UpdateGrids(pBodyNode, m_pScene->GetSceneGrids(), oldGrids);
			pBodyNode->SetObjectGrids(newGrids);
		}
	}

	uint16_t Snake::GetSinAngle(uint16_t uAngle) {
		// 为了算出所在象限中飞行射线与Y轴的夹角
		if (uAngle <= 90) {
			return uAngle;
		}
		else if (uAngle <= 180) {
			return 180 - uAngle;
		}
		else if (uAngle <= 270) {
			return uAngle - 180;
		}
		else if (uAngle <= 360) {
			return 360 - uAngle;
		}

		// 其他情况理论上都是错误的
		return 0;
	}

	float Snake::GetXLen(float vectLen, uint16_t uSinAngle) {
		float fAngle = float(uSinAngle * PI / 180);
		float sin_value = (float)::sin(fAngle);
		return vectLen * sin_value;
	}

	float Snake::GetYLen(float vectLen, uint16_t uSinAngle) {
		float fAngle = float(uSinAngle * PI / 180);
		float cos_value = (float)::cos(fAngle);
		return vectLen * cos_value;
	}

	void Snake::SetLeaveTime() {
		if (!m_pScene) {
			return;
		}
		if (m_pScene->GetGameRoom()) {
			m_pSnakeStatistics->uLeaveRoomTime = m_pScene->GetGameRoom()->GetPassTime();
		}
	}


	// 是否自己视野中
	// 参数 pSnake 判断是否在自己视野中的其他蛇， 自己的视野内的格子gridSet
	bool Snake::IsInView(Snake* pSnake, const set<uint32_t>& gridSet) {
		if (!pSnake) {
			return false;
		}

		// 判断一条蛇是否在自己的视野内，从蛇头一直到整个蛇身，如果某个节点在格子内，格子能被看见，蛇就能被看见
		SnakeBodyNode* pHead = pSnake->GetSnakeHead();
		ObjectGrids bodyGrids = m_pScene->GetObjectGrids(pHead);
		for (int i = 0; i < MAX_GRID_IN; ++i) {
			int32_t uGridIndex = bodyGrids.grids[i];
			if (uGridIndex == -1) {
				continue;
			}
			
			set<uint32_t>::const_iterator gridIt = gridSet.find(uGridIndex);
			if (gridIt != gridSet.end()) {
				return true;
			}
		}

		const list<SnakeBodyNode*>& otherBodyList = pSnake->GetSnakeBody();
		list<SnakeBodyNode*>::const_iterator otherBodyIt = otherBodyList.begin();
		list<SnakeBodyNode*>::const_iterator otherBodyItEnd = otherBodyList.end();
		for (; otherBodyIt != otherBodyItEnd; otherBodyIt++) {
			SnakeBodyNode* pOtherBody = *otherBodyIt;
			bodyGrids = m_pScene->GetObjectGrids(pOtherBody);
			for (int i = 0; i < MAX_GRID_IN; ++i) {
				int32_t uGridIndex = bodyGrids.grids[i];
				if (uGridIndex == -1) {
					continue;
				}

				set<uint32_t>::iterator gridIt = gridSet.find(uGridIndex);
				if (gridIt != gridSet.end()) {
					return true;
				}
			}
		}

		return false;
	}

	// 已经在对方的视野中了
	bool Snake::HasInView(Snake* pSnake) {
		if (!pSnake) {
			return false;
		}

		set<uint32_t>::iterator snakeIt = m_viewRangeSnakeSet.find(pSnake->GetSnakeId());
		if (snakeIt == m_viewRangeSnakeSet.end()) {
			return false;
		}

		return true;
	}

	bool Snake::IsInView(Grid* pGrid) {
		if (!pGrid) {
			return false;
		}
		
		float fLen = sqrt(pow((pGrid->GetCenterPos().x - m_pHead->GetPos().x), 2) + pow((pGrid->GetCenterPos().y - m_pHead->GetPos().y), 2));
		if (fLen < (float)GetSnakeView() / 2.0f) {
			return true;
		}

		return false;
	}

	bool Snake::HasInView(uint32_t uGridId) {

		set<uint32_t>::iterator gridIt = m_viewRangeGridSet.find(uGridId);
		if (gridIt == m_viewRangeGridSet.end()) {
			return false;
		}

		return true;
	}

	void Snake::AddInView(Snake* pSnake) {
		if (!pSnake) {
			return;
		}
		m_viewRangeSnakeSet.insert(pSnake->GetSnakeId());
	}

	void Snake::AddInView(uint32_t uGridId) {
		m_viewRangeGridSet.insert(uGridId);
	}

	void Snake::SendMsg(const char* pData, uint32_t uLen) {
		if (!pData || uLen == 0) {
			return;
		}

		if (m_pConn && m_pConn->IsConnected()) {
			m_pConn->SendMsg(pData, uLen);
		}
		return;
	}

	void Snake::GetSlitherParam(float& fCurRadius, uint32_t& uCurNode) {
		const uint32_t nInitialPower = g_slitherConfig.m_uInitSnakeMass;				//蛇初始能量值
		const float nInitialRadius = g_slitherConfig.m_fInitRadius;					//蛇初始半径值
		const uint32_t nInitialNode = g_slitherConfig.m_uInitSnakeBodySize;			//蛇初始节点值
	
		const float nGrowRadius = g_slitherConfig.m_fIncRadiusValue;					//每次增长的半径
		const uint32_t nRadiusRate = g_slitherConfig.m_uIncRadiusIntervalValue;		//半径成长系数
		const float fRadiusSqt = g_slitherConfig.m_fRadiusSqt;						//半径成长指数
		const float fMaxRadius = g_slitherConfig.m_fMaxRadius;						//蛇身最大半径

		const uint32_t nGrowNode = 1;
		const float fNodeRate = (float)g_slitherConfig.m_fGrowUpValue;				//节点成长系数
		const float nNodeSqt = g_slitherConfig.m_fNodeSqt;							//节点成长指数
		const uint32_t nMaxNode = g_slitherConfig.m_uMaxBodySize;					//蛇身最大节点

		//根据当前能量，计算半径实际成长的次数
		uint32_t nRadiusTime = (uint32_t)floor(pow((m_fTotalMass) / nRadiusRate * 100, fRadiusSqt));

		//根据已成长的次数，得出当前的实际半径
		fCurRadius = min(fMaxRadius, nInitialRadius + nRadiusTime * nGrowRadius * (float)pow(0.01f, fRadiusSqt));
		fCurRadius = fCurRadius;

		//根据当前能量，计算节点实际增加的次数
		uint32_t nNodeTime = (uint32_t)floor(pow((m_fTotalMass - nInitialPower) / fNodeRate, nNodeSqt));

		//根据已成长的次数，得出当前的实际半径
		uCurNode = min(nMaxNode, nInitialNode + nNodeTime * nGrowNode);
	}

	void Snake::GetSlitherParam(float fTotalMass, float& fCurRadius, uint32_t& uCurNode) {
		const uint32_t nInitialPower = g_slitherConfig.m_uInitSnakeMass;				//蛇初始能量值
		const float nInitialRadius = g_slitherConfig.m_fInitRadius;					//蛇初始半径值
		const uint32_t nInitialNode = g_slitherConfig.m_uInitSnakeBodySize;			//蛇初始节点值

		const float nGrowRadius = g_slitherConfig.m_fIncRadiusValue;					//每次增长的半径
		const uint32_t nRadiusRate = g_slitherConfig.m_uIncRadiusIntervalValue;		//半径成长系数
		const float fRadiusSqt = g_slitherConfig.m_fRadiusSqt;						//半径成长指数
		const float fMaxRadius = g_slitherConfig.m_fMaxRadius;						//蛇身最大半径

		const uint32_t nGrowNode = 1;
		const float fNodeRate = (float)g_slitherConfig.m_fGrowUpValue;				//节点成长系数
		const float nNodeSqt = g_slitherConfig.m_fNodeSqt;							//节点成长指数
		const uint32_t nMaxNode = g_slitherConfig.m_uMaxBodySize;					//蛇身最大节点

		//根据当前能量，计算半径实际成长的次数
		uint32_t nRadiusTime = (uint32_t)floor(pow((fTotalMass) / nRadiusRate * 100, fRadiusSqt));

		//根据已成长的次数，得出当前的实际半径
		fCurRadius = min(fMaxRadius, nInitialRadius + nRadiusTime * nGrowRadius * (float)pow(0.01f, fRadiusSqt));
		fCurRadius = fCurRadius;

		//根据当前能量，计算节点实际增加的次数
		uint32_t nNodeTime = (uint32_t)floor(pow((fTotalMass - nInitialPower) / fNodeRate, nNodeSqt));

		//根据已成长的次数，得出当前的实际半径
		uCurNode = min(nMaxNode, nInitialNode + nNodeTime * nGrowNode);
	}

	float Snake::GetSnakeView() {
		const float fRadiusSqt = g_slitherConfig.m_fRadiusSqt;						//半径成长指数
		const float fGrowRadius = g_slitherConfig.m_fIncRadiusValue;					//每次增长的半径
		const float fInitialRadius = g_slitherConfig.m_fInitRadius;					//蛇初始半径值
		const uint32_t nRadiusRate = g_slitherConfig.m_uIncRadiusIntervalValue;		//半径成长系数

		float fTmp = floor(pow((m_fTotalMass) / nRadiusRate * 100, fRadiusSqt));
		float fCurView = (float)pow((fTmp * fGrowRadius * (float)pow(0.01f, fRadiusSqt) + fInitialRadius) / fInitialRadius, g_slitherConfig.m_fViewAttenuation) * g_slitherConfig.m_uInitViewRange;

		return fCurView;
	}
}