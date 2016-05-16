﻿#include "Snake.h"
#include "Scene.h"
#include "Food.h"
#include "../../utils/Utility.h"
#include "SlitherMath.h"

namespace slither {

	SnakeBodyNode::SnakeBodyNode(Snake* pOwner, uint16_t uNodeId, SnakeBodyNode* pPrevNode, const Vector2D& pos, float fRadius, float fSpeed, float fAngle) :
		Object(pos, fRadius, Snake_Body_Type), m_pOwner(pOwner), m_uNodeId(uNodeId), m_pPrevNode(pPrevNode), m_fSpeed(fSpeed), m_angle(fAngle) {
	}

	SnakeBodyNode::~SnakeBodyNode() {

	}

	float SnakeBodyNode::GetSpeed() {
		if (!m_pOwner->GetSpeedUp()) {
			return m_fSpeed;
		}
		else {
			return m_fSpeed * 3;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Snake::Snake(Scene* pScene, uint32_t uSnakeId, float fRadius, const Vector2D& initPos, uint32_t uBodySize, bool bRobot) : 
		m_pScene(pScene), m_uSnakeId(uSnakeId), m_bSpeedUp(false), m_pConn(NULL), m_status(OBJ_EXIST), m_bRobot(bRobot),
		m_uViewRange(40), m_uMoveTick(0), m_uTotalMass(0), m_uNodeId(1)
	{
		m_pHead = new SnakeBodyNode(this, m_uNodeId++, NULL, initPos, fRadius, 0.25f, 0);
		m_pHead->SetObjectType(Snake_Head_Type);

		Vector2D nodePos = initPos;

		SnakeBodyNode* pPrevNode = m_pHead;
		for (uint32_t i = 0; i < uBodySize; ++i) {
			//nodePos.x -= fRadius;
			nodePos.x -= 0.25f;
			SnakeBodyNode* pSnakeNode = new SnakeBodyNode(this, m_uNodeId++, pPrevNode, nodePos, fRadius, 0.1f, 0);
			pPrevNode = pSnakeNode;

			m_bodyVec.push_back(pSnakeNode);
		}

		// 设置蛇尾
		SetSnakeTail(pPrevNode);
	}

	Snake::~Snake() {
		if (m_pHead) {
			delete m_pHead;
		}
		vector<SnakeBodyNode*>::iterator bodyIt = m_bodyVec.begin();
		vector<SnakeBodyNode*>::iterator bodyItEnd = m_bodyVec.end();
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
		pbSnake.mutable_snakehead()->set_speed(m_pHead->GetSpeed());

		if (bJustHead) {					// 如果只需要序列化头
			return;
		}

		// 序列化蛇身
		for (uint32_t i = 0; i < m_bodyVec.size(); ++i) {
			slither::PBSnakeBody* pPBSnakeBody = pbSnake.add_snakebody();
			pPBSnakeBody->set_bodyid(m_bodyVec[i]->GetNodeId());
			pPBSnakeBody->mutable_pos()->set_x(m_bodyVec[i]->GetPos().x);
			pPBSnakeBody->mutable_pos()->set_y(m_bodyVec[i]->GetPos().y);
		}
	}

	// 模拟蛇运动
	void Snake::Move() {
		if (!m_pHead) {
			return;
		}

		// 测试用的，随机运动方向
		if (m_bRobot) 
		{
			//uint32_t uAngle = cputil::GenRandom(1, 359);
			//m_pHead->SetAngle(uAngle);
			m_pHead->SetSpeed(0.0f);
		}
		
		//float fSpeed = cputil::GenFloatRandom(0.1, 1.0);
		//m_pHead->SetSpeed(fSpeed);

		// 蛇头运动
		ObjectGrids oldGrids = m_pScene->GetObjectGrids(m_pHead);
		//CalcNextPos(m_pHead->GetAngle(), m_pHead->GetSpeed(), m_pHead->GetPos());				// 计算下一个点
		SlitherMath::MoveToAngle(m_pHead->GetPos(), m_pHead->GetAngle(), m_pHead->GetSpeed());
		ObjectGrids newGrids = m_pScene->GetObjectGrids(m_pHead);
		newGrids.UpdateGrids(m_pHead, m_pScene->GetSceneGrids(), oldGrids);

		// 蛇身每节的运动
		for (uint32_t i = 0; i < m_bodyVec.size(); ++i) {
			SnakeBodyNode*& pBodyNode = m_bodyVec[i];
			if (!pBodyNode) {				// ERROR
				return;
			}

			ObjectGrids oldGrids = m_pScene->GetObjectGrids(pBodyNode);
			SnakeBodyNode* pPrevNode = pBodyNode->GetPrevNode();
			
			//CalcNextPos(pPrevNode->GetAngle(), m_pHead->GetSpeed(), pBodyNode->GetPos());
			pBodyNode->TracePreNode();
			

			ObjectGrids newGrids = m_pScene->GetObjectGrids(pBodyNode);
			newGrids.UpdateGrids(pBodyNode, m_pScene->GetSceneGrids(), oldGrids);
		}

		// 每移动1次，更新一次视野内的所有物体
		if (m_uMoveTick++ >= 1) {
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

			// 判断格子
			set<Grid*>::iterator gridIt = m_viewRangeGridSet.begin();
			set<Grid*>::iterator gridItEnd = m_viewRangeGridSet.end();
			for (; gridIt != gridItEnd;) {
				if (!IsInView(*gridIt)) {						// 不在视野范围内了
					m_viewRangeGridSet.erase(gridIt++);
					continue;
				}
				gridIt++;
			}
		}
	}

	// 吃食物
	void Snake::EatFood(Food* pFood) {
		if (!pFood) {
			return;
		}

		m_uTotalMass += pFood->GetMass();
		if (m_uTotalMass % 5 == 0) {			// 先暂时测试使用，后面替换成真实的数值
			AddTail();
		}

		return;
	}

	// 在尾部添加节点
	SnakeBodyNode* Snake::AddTail() {
		Vector2D tailPos = m_pTail->GetPos();
		tailPos.x -= 0.25f;
		SnakeBodyNode* pNewTail = new SnakeBodyNode(m_pHead->GetOwner(), m_uNodeId++, m_pTail, tailPos, m_pTail->GetRadius(), m_pTail->GetSpeed(), m_pTail->GetAngle());
		m_pTail = pNewTail;
		m_bodyVec.push_back(pNewTail);

		return pNewTail;
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

	void Snake::CalcNextPos(uint16_t uAngle, float fSpeed, Vector2D& pos) {

		uint16_t uSinAngle = GetSinAngle(uAngle);
		float x_len = GetXLen(fSpeed, uSinAngle);
		float y_len = GetYLen(fSpeed, uSinAngle);

		// 根据飞行方向，重新计算子弹坐标点，处理四个象限的不同情况，第一象限不需要特殊处理
		if (uAngle <= 90) {					// 第二象限
			x_len = -x_len;
		}
		else if (uAngle <= 180) {			// 第三象限
			x_len = -x_len;
			y_len = -y_len;
		}
		else if (uAngle <= 270) {			// 第四象限
			y_len = -y_len;
		}

		pos.x += x_len;
		if (pos.x < 0) {
			pos.x = 0.1f;
		}
		else if (pos.x >= 1000) {
			pos.x = 999.0f;
		}

		pos.y += y_len;
		if (pos.y < 0) {
			pos.y = 0.1f;
		}
		else if (pos.y >= 1000) {
			pos.y = 999.0f;
		}
		return;
	}

	// 是否在对方视野中
	bool Snake::IsInView(Snake* pSnake) {
		if (!pSnake) {
			return false;
		}
		

		SnakeBodyNode* pHead = pSnake->GetSnakeHead();

		float fLen = sqrt(pow((pHead->GetPos().x - m_pHead->GetPos().x), 2) + pow((pHead->GetPos().y - m_pHead->GetPos().y), 2));
		if (fLen < (float)pSnake->GetViewRange()) {
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
		if (fLen < (float)m_uViewRange) {
			return true;
		}

		return false;
	}

	bool Snake::HasInView(Grid* pGrid) {
		if (!pGrid) {
			return false;
		}

		set<Grid*>::iterator gridIt = m_viewRangeGridSet.find(pGrid);
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

	void Snake::AddInView(Grid* pGrid) {
		m_viewRangeGridSet.insert(pGrid);
	}

	void Snake::DelInView(Snake* pSnake) {
		if (!pSnake) {
			return;
		}
		m_viewRangeSnakeSet.erase(pSnake->GetSnakeId());
	}

	void Snake::DelInView(Grid* pGrid) {
		m_viewRangeGridSet.erase(pGrid);
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
}