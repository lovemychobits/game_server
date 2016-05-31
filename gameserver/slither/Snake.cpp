#include "Snake.h"
#include "Scene.h"
#include "Food.h"
#include "../../utils/Utility.h"
#include "SlitherMath.h"
#include "../config/SlitherConfig.h"
#include <math.h>

namespace slither {

	SnakeBodyNode::SnakeBodyNode(Snake* pOwner, uint16_t uNodeId, SnakeBodyNode* pPrevNode, const Vector2D& pos, float fRadius, float fSpeed, float fAngle) :
		Object(pos, fRadius, Snake_Body_Type), m_pOwner(pOwner), m_uNodeId(uNodeId), m_pPrevNode(pPrevNode), m_fSpeed(fSpeed), m_angle(fAngle), m_bIsMove(false) {
	}

	SnakeBodyNode::~SnakeBodyNode() {

	}

	float SnakeBodyNode::GetSpeed() {
		if (!m_pOwner->GetSpeedUp()) {
			return m_fSpeed;
		}
		else {
			return m_fSpeed * gpSlitherConf->m_fSpeedUpRate;
		}
	}

	void SnakeBodyNode::TracePreNode() {
		if (!m_pPrevNode) {
			return;
		}

		Vector2D distVect(m_pPrevNode->GetPos().x - m_pos.x, m_pPrevNode->GetPos().y - m_pos.y);
		float fSnakeRadius = m_pOwner->GetSnakeHead()->GetRadius();
		float fMoveDist = distVect.Magnitude() - GetBodyNodeDist(fSnakeRadius, gpSlitherConf->m_fBodyInterval);

		bool bIsMove = false;
		if (fMoveDist > 0.0f && !bIsMove) {
			bIsMove = true;
		}

		if (bIsMove) {
			if (fMoveDist > 0.0f) {
				m_lastMove = SlitherMath::MoveTo(distVect, fMoveDist);
				m_pos.x += m_lastMove.x;
				m_pos.y += m_lastMove.y;
			}
			else {
				/*m_pos.x += m_lastMove.x;
				m_pos.y += m_lastMove.y;*/
			}
		}
	}

	float SnakeBodyNode::GetBodyNodeDist(float fRadius, float fRatio) {
		return 2 * fRadius * (1 - fRatio);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Snake::Snake(Scene* pScene, uint32_t uSnakeId, float fRadius, const Vector2D& initPos, uint32_t uBodySize, bool bRobot) : 
		m_pScene(pScene), m_uSnakeId(uSnakeId), m_bSpeedUp(false), m_bStopMove(false), m_pConn(NULL), m_status(OBJ_EXIST), m_bRobot(bRobot),
		m_uViewRange(20), m_uMoveTick(0), m_fTotalMass(gpSlitherConf->m_uInitSnakeMass), m_uNodeId(1), m_uMsgCount(0), m_uMsgSize(0)
	{
		m_pHead = new SnakeBodyNode(this, m_uNodeId++, NULL, initPos, fRadius, gpSlitherConf->m_fSpeed / 10.0f, 0);
		m_pHead->SetObjectType(Snake_Head_Type);

		Vector2D nodePos = initPos;

		SnakeBodyNode* pPrevNode = m_pHead;
		for (uint32_t i = 0; i < uBodySize; ++i) {
			nodePos.x -= (1 - gpSlitherConf->m_fBodyInterval) * fRadius;
			SnakeBodyNode* pSnakeNode = new SnakeBodyNode(this, m_uNodeId++, pPrevNode, nodePos, fRadius, gpSlitherConf->m_fSpeed / 10, 0);
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

		// 序列化蛇头
		pbSnake.mutable_snakehead()->mutable_pos()->set_x(m_pHead->GetPos().x);
		pbSnake.mutable_snakehead()->mutable_pos()->set_y(m_pHead->GetPos().y);
		pbSnake.mutable_snakehead()->set_angle(m_pHead->GetAngle());
		pbSnake.mutable_snakehead()->set_radius(m_pHead->GetRadius());
		pbSnake.mutable_snakehead()->set_speed(m_pHead->GetSpeed() * 10);
		pbSnake.set_totalmass(m_fTotalMass);

		if (bJustHead) {					// 如果只需要序列化头
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
		//if (m_bRobot) {
		//	return;
		//}

		if (m_bRobot && m_uMoveTick % 15 == 0) {
			float fRandAngle = cputil::GenFloatRandom(0.1f, 3.0f);
			this->SetAngle(fRandAngle);
			m_pHead->SetSpeed(0.0f);
		}
		
		// 蛇头运动
		ObjectGrids oldGrids = m_pScene->GetObjectGrids(m_pHead);
		SlitherMath::MoveToAngle(m_pHead->GetPos(), m_pHead->GetAngle(), m_pHead->GetSpeed());
		ObjectGrids newGrids = m_pScene->GetObjectGrids(m_pHead);
		newGrids.UpdateGrids(m_pHead, m_pScene->GetSceneGrids(), oldGrids);
		//cout << "new head pos=[" << m_pHead->GetPos().x << ", " << m_pHead->GetPos().y << "]" << endl;

		// 蛇身每节的运动
		list<SnakeBodyNode*>::iterator bodyIt = m_bodyList.begin();
		list<SnakeBodyNode*>::iterator bodyItEnd = m_bodyList.end();
		for (; bodyIt != bodyItEnd; bodyIt++) {
			SnakeBodyNode* pBodyNode = *bodyIt;
			if (!pBodyNode) {														// 发生了错误
				return;
			}

			ObjectGrids oldGrids = m_pScene->GetObjectGrids(pBodyNode);
			SnakeBodyNode* pPrevNode = pBodyNode->GetPrevNode();
			
			pBodyNode->TracePreNode();												// 跟随前一个点
			
			ObjectGrids newGrids = m_pScene->GetObjectGrids(pBodyNode);
			newGrids.UpdateGrids(pBodyNode, m_pScene->GetSceneGrids(), oldGrids);
		}

		// 每移动30次，将一些不在视野内的物体，或者格子删除
		//if (m_uMoveTick++ >= 30) {
		if (true) {
			// 判断蛇
			set<uint32_t>::iterator snakeIt = m_viewRangeSnakeSet.begin();
			set<uint32_t>::iterator snakeItEnd = m_viewRangeSnakeSet.end();
			for (; snakeIt != snakeItEnd; ) {
				Snake* pSnake = m_pScene->GetSnakeById(*snakeIt);
				if (!pSnake) {
					m_viewRangeSnakeSet.erase(snakeIt++);
					continue;
				}

				if (!IsInView(pSnake) || pSnake->GetStatus() == OBJ_DESTROY) {			// 不在视野范围内了, 或者已经死亡了
					m_viewRangeSnakeSet.erase(snakeIt++);
					continue;
				}
				snakeIt++;
			}

			set<uint32_t> gridSet = m_pScene->GetInViewGrids(this);

			// 判断格子
			set<uint32_t>::iterator gridIt = m_viewRangeGridSet.begin();
			set<uint32_t>::iterator gridItEnd = m_viewRangeGridSet.end();
			for (; gridIt != gridItEnd;) {
				set<uint32_t>::iterator findIt = gridSet.find(*gridIt);
				if (findIt == gridSet.end()) {					// 不在视野范围内了
					m_viewRangeGridSet.erase(gridIt++);
					continue;
				}

				gridIt++;
			}
		}
		m_uMoveTick++;
	}

	// 吃食物
	void Snake::EatFood(Food* pFood) {
 		if (!pFood) {
			return;
		}

		AddEatNum(pFood->GetMass());
		m_fTotalMass += pFood->GetMass();

		SetMaxLength(m_fTotalMass);

		float fNewRadius = 0.0f;
		uint32_t uNewNodeNum = 0;
		GetSlitherParam(fNewRadius, uNewNodeNum);

		if (uNewNodeNum > m_bodyList.size()) {					// 增加蛇身节点
			IncTail();
		}

		if (fNewRadius > m_pHead->GetRadius() + gpSlitherConf->m_fIncRadiusValue - 0.001f) {
			IncRadius();
		}	

		return;
	}

	void Snake::SpeedUpCost() {

		if (!m_bSpeedUp) {
			return;
		}

		// 减少重量
		m_fTotalMass -= (0.1 / gpSlitherConf->m_fAttenuationInterval * gpSlitherConf->m_fAttenuationValue);

		float fNewRadius = 0.0f;
		uint32_t uNewNodeNum = 0;
		GetSlitherParam(fNewRadius, uNewNodeNum);

		if (uNewNodeNum < m_bodyList.size()) {					// 减少蛇身节点
			DecTail();
		}

		if (fNewRadius < m_pHead->GetRadius() - gpSlitherConf->m_fIncRadiusValue + 0.001f) {
			DecRadius();
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
		tailPos.x -= (1 - gpSlitherConf->m_fBodyInterval) * m_pTail->GetRadius();

		SnakeBodyNode* pNewTail = new SnakeBodyNode(m_pHead->GetOwner(), m_uNodeId++, m_pTail, tailPos, m_pTail->GetRadius(), m_pTail->GetSpeed(), m_pTail->GetAngle());
		m_pTail = pNewTail;
		m_bodyList.push_back(pNewTail);

		ObjectGrids objectGrids = m_pScene->GetObjectGrids(pNewTail);
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
		ERRORLOG("snake=[" << this << "] inc tail");

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
		ObjectGrids objectGrids = m_pScene->GetObjectGrids(pOldTail);
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

	void Snake::IncRadius() {
		float fNewRadius = m_pHead->GetRadius() + gpSlitherConf->m_fIncRadiusValue;

		// 需要先从格子中删除，否则半径发生变化之后，会导致在之前的格子中有残留，引发指针异常
		ObjectGrids oldGrids = m_pScene->GetObjectGrids(m_pHead);
		m_pHead->SetRadius(fNewRadius);											// 设置新的半径
		ObjectGrids newGrids = m_pScene->GetObjectGrids(m_pHead);
		newGrids.UpdateGrids(m_pHead, m_pScene->GetSceneGrids(), oldGrids);

		list<SnakeBodyNode*>::iterator bodyIt = m_bodyList.begin();
		list<SnakeBodyNode*>::iterator bodyItEnd = m_bodyList.end();

		//float fBodyInterval = gpSlitherConf->m_fBodyInterval;

		for (; bodyIt != bodyItEnd; bodyIt++) {
			SnakeBodyNode* pBodyNode = *bodyIt;
			if (!pBodyNode) {
				return;
			}

			oldGrids = m_pScene->GetObjectGrids(pBodyNode);
			pBodyNode->SetRadius(fNewRadius);												// 更新身体节点的半径
			newGrids = m_pScene->GetObjectGrids(pBodyNode);
			newGrids.UpdateGrids(pBodyNode, m_pScene->GetSceneGrids(), oldGrids);
		}

		ERRORLOG("snake=[" << this << "] inc radius");
	}

	void Snake::DecRadius() {
		float fNewRadius = m_pHead->GetRadius() - gpSlitherConf->m_fIncRadiusValue;			// 减去对应的半径
		
		ObjectGrids oldGrids = m_pScene->GetObjectGrids(m_pHead);
		m_pHead->SetRadius(fNewRadius);														// 设置新的半径
		ObjectGrids newGrids = m_pScene->GetObjectGrids(m_pHead);
		newGrids.UpdateGrids(m_pHead, m_pScene->GetSceneGrids(), oldGrids);


		list<SnakeBodyNode*>::iterator bodyIt = m_bodyList.begin();
		list<SnakeBodyNode*>::iterator bodyItEnd = m_bodyList.end();
		for (; bodyIt != bodyItEnd; bodyIt++) {
			SnakeBodyNode* pBodyNode = *bodyIt;
			if (!pBodyNode) {
				return;
			}

			oldGrids = m_pScene->GetObjectGrids(pBodyNode);
			pBodyNode->SetRadius(fNewRadius);												// 更新身体节点的半径
			newGrids = m_pScene->GetObjectGrids(pBodyNode);
			newGrids.UpdateGrids(pBodyNode, m_pScene->GetSceneGrids(), oldGrids);
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

	// 是否在对方视野中
	bool Snake::IsInView(Snake* pSnake) {
		if (!pSnake) {
			return false;
		}

		SnakeBodyNode* pHead = pSnake->GetSnakeHead();

		float fLen = sqrt(pow((pHead->GetPos().x - m_pHead->GetPos().x), 2) + pow((pHead->GetPos().y - m_pHead->GetPos().y), 2));
		if (fLen < pSnake->GetViewRange()) {
			return true;
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
		if (fLen < (float)GetSnakeView()) {
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

	void Snake::DelInView(Snake* pSnake) {
		if (!pSnake) {
			return;
		}
		m_viewRangeSnakeSet.erase(pSnake->GetSnakeId());
	}

	void Snake::DelInView(uint32_t uGridId) {
		m_viewRangeGridSet.erase(uGridId);
	}

	void Snake::SendMsg(const char* pData, uint32_t uLen) {
		if (!pData || uLen == 0) {
			return;
		}

		if (m_pConn && m_pConn->IsConnected()) {
			m_pConn->SendMsg(pData, uLen);
			++m_uMsgCount;
			m_uMsgSize += uLen;
		}

		return;
	}

	void Snake::GetSlitherParam(float& fCurRadius, uint32_t& uCurNode) {
		const uint32_t nInitialPower = gpSlitherConf->m_uInitSnakeMass;				//蛇初始能量值
		const float nInitialRadius = gpSlitherConf->m_fInitRadius;					//蛇初始半径值
		const uint32_t nInitialNode = gpSlitherConf->m_uInitSnakeBodySize;			//蛇初始节点值
	
		const float nGrowRadius = gpSlitherConf->m_fIncRadiusValue;					//每次增长的半径
		const uint32_t nRadiusRate = gpSlitherConf->m_uIncRadiusIntervalValue;		//半径成长系数
		const float fRadiusSqt = gpSlitherConf->m_fRadiusSqt;						//半径成长指数
		const float fMaxRadius = gpSlitherConf->m_fMaxRadius;						//蛇身最大半径

		const uint32_t nGrowNode = 1;
		const float fNodeRate = (float)gpSlitherConf->m_uGrowUpValue;				//节点成长系数
		const float nNodeSqt = gpSlitherConf->m_fNodeSqt;							//节点成长指数
		const uint32_t nMaxNode = gpSlitherConf->m_uMaxBodySize;					//蛇身最大节点

		//根据当前能量，计算半径实际成长的次数
		uint32_t nRadiusTime = (uint32_t)floor(pow((m_fTotalMass) / nRadiusRate, fRadiusSqt));

		//根据已成长的次数，得出当前的实际半径
		fCurRadius = min(fMaxRadius, nInitialRadius + nRadiusTime * nGrowRadius);
		fCurRadius = fCurRadius;

		//根据当前能量，计算节点实际增加的次数
		uint32_t nNodeTime = (uint32_t)floor(pow((m_fTotalMass - nInitialPower) / fNodeRate, nNodeSqt));

		//根据已成长的次数，得出当前的实际半径
		uCurNode = min(nMaxNode, nInitialNode + nNodeTime*nGrowNode);
	}

	float Snake::GetSnakeView() {
		const float fRadiusSqt = gpSlitherConf->m_fRadiusSqt;						//半径成长指数
		const float fGrowRadius = gpSlitherConf->m_fIncRadiusValue;					//每次增长的半径
		const float fInitialRadius = gpSlitherConf->m_fInitRadius;					//蛇初始半径值

		float fTmp = floor(pow((m_fTotalMass / 100), fRadiusSqt));
		float fCurView = (fTmp * fGrowRadius * 100 + fInitialRadius * 100) / (fInitialRadius * 100) * 30;

		return fCurView;
	}
}