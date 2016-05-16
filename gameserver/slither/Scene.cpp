﻿#include "Scene.h"
#include "Snake.h"
#include "Food.h"
#include "../../utils/Utility.h"
#include "PlayerMng.h"
#include "../../protocol/slither_client.pb.h"
#include "Factory.h"
#include "SlitherMath.h"
#include <iostream>
using namespace std;

namespace slither {

#define GridSize 20
	Scene::Scene() : m_uSnakeId(1), m_uFoodId(1), m_uFoodCount(0) {
	}

	Scene::~Scene() {
		delete[] m_pGrids;
	}

	// 初始化，根据整个地图的大小去计算格子数
	bool Scene::Init(uint32_t uSceneHeight, uint32_t uSceneWidth) {
		uint32_t x = 0;
		uint32_t y = 0;

		m_uHorizontalGridNum = uSceneWidth / GridSize;						// 水平方向的格子数
		m_uVerticalGridNum = uSceneHeight / GridSize;						// 垂直方向的格子数
		m_pGrids = new Grid[m_uHorizontalGridNum * m_uVerticalGridNum];

		uint32_t index = 0;
		// 构造格子
		for (uint32_t y = 0; y < uSceneHeight; y += GridSize) {
			for (uint32_t x = 0; x < uSceneWidth; x += GridSize) {
				Grid& grid = m_pGrids[index++];
				grid.x = x;													// 左下角坐标作为格子的坐标
				grid.y = y;
				grid.uHeight = GridSize;
				grid.uWidth = GridSize;
			}
		}

		return true;
	}

	Snake* Scene::GetSnakeById(uint32_t uSnakeId) {
		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.find(uSnakeId);
		if (snakeIt == m_snakeMap.end()) {
			return NULL;
		}

		return snakeIt->second;
	}

	void Scene::OnTimer(const boost::system::error_code& errCode) {
		if (errCode) {
			ERRORLOG("scene ontimer error;");
			return;
		}

		//DWORD dwStart = ::GetTickCount();

		uint32_t uGridIndex = 0;														// 蛇头所在的格子

		// 模拟所有蛇的运动
		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.begin();
		for (; snakeIt != m_snakeMap.end();) {
			Snake* pSnake = snakeIt->second;
			if (!pSnake) {
				continue;
			}
			bool bRobot = pSnake->IsRobot();

			// 不在其他地方删除，否则不好控制
			if (pSnake->GetStatus() == OBJ_DESTROY) {						// 如果蛇需要销毁
				// 删除蛇与链接之间的关系
				gpPlayerMng->DeleteSnake(pSnake);

				// 释放蛇
				gpFactory->ReleaseSnake(pSnake);
				m_snakeMap.erase(snakeIt++);
				continue;
			}

			// 先让蛇运动
			pSnake->Move();
			uGridIndex = GetGridIndex(pSnake->GetSnakeHead()->GetPos());				// 获取运动之后头所在的格子，限制每次运动不会超过一个蛇头的直径

			// 向此蛇通知周围的格子食物情况（只通知必要的）
			NotifyGrids(pSnake);						

			// 获取需要通知的周围蛇列表
			list<Snake*> broadcastList;
			GetBroadcastList(pSnake, broadcastList);

			// 如果蛇是加速的，那么在蛇尾生成食物
			SnakeBodyNode* pTail = pSnake->GetSnakeTail();
			if (pSnake->GetSpeedUp()) {
				list<Food*> foodList;
				GenFoods(pSnake->GetSnakeTail()->GetPos(), 1, 1, foodList);
				// 通知生成新的食物了
				BroadcastNewFoods(broadcastList, foodList);
			}

			// 广播移动消息
			BroadcastMove(pSnake, broadcastList);

			// 判断蛇吃到食物的情况
			CheckEatFood(pSnake, uGridIndex, broadcastList);

			SnakeBodyNode* pNewTail = pSnake->GetSnakeTail();
			if (pTail != pNewTail) {				// 吃完食物之后尾部节点不同了，说明长出了新的节点
				BroadcastNewTail(pSnake, broadcastList);
			}

			// 判断蛇的碰撞情况
			CheckCollide(pSnake, uGridIndex, broadcastList);

			// TODO 根据参数定期生成食物
			//if (m_uFoodCount < 7000) {
			//	GenFoods(3000);
			//}

			snakeIt++;
		}

		//DWORD dwEnd = ::GetTickCount();
		//cout << "time cost=[" << dwEnd - dwStart << "], snake count=[" << m_snakeMap.size() << "]" << endl;
	}

	uint32_t Scene::GetGridIndex(const Vector2D& pos) {
		uint32_t uHorizontalGridNum = uint32_t(pos.x / GridSize);
		uint32_t uVerticalGridNum	= uint32_t(pos.y / GridSize);

		uint32_t uIndex = uVerticalGridNum * m_uHorizontalGridNum + uHorizontalGridNum;
		if (uIndex >= m_uHorizontalGridNum * m_uVerticalGridNum) {
			// TODO 
			// 需要根据判定边缘，目前只是简单的设置了上限
			uIndex = m_uHorizontalGridNum * m_uVerticalGridNum - 1;
		}
		return uIndex;
	}

	// 获取一个物体属于的所有格子，因为一个物体可能在几个格子的边缘，所以算是属于多个格子
	ObjectGrids Scene::GetObjectGrids(Object* pObj) {
		const Vector2D& objPos = pObj->GetPos();
		Vector2D leftTopPos		(objPos.x - pObj->GetRadius(), objPos.y + pObj->GetRadius());
		Vector2D rightTopPos	(objPos.x + pObj->GetRadius(), objPos.y + pObj->GetRadius());
		Vector2D leftBottomPos	(objPos.x - pObj->GetRadius(), objPos.y - pObj->GetRadius());
		Vector2D rightBottomPos	(objPos.x + pObj->GetRadius(), objPos.y - pObj->GetRadius());

		ObjectGrids objectGrids;
		objectGrids.grids[0] = GetGridIndex(leftTopPos);
		objectGrids.grids[1] = GetGridIndex(rightTopPos);
		if (objectGrids.grids[1] == objectGrids.grids[0]) {				// 判断是否同一个格子
			objectGrids.grids[1] = -1;
		}
		objectGrids.grids[2] = GetGridIndex(leftBottomPos);
		if (objectGrids.grids[2] == objectGrids.grids[0] || objectGrids.grids[2] == objectGrids.grids[1]) {
			objectGrids.grids[2] = -1;
		}
		objectGrids.grids[3] = GetGridIndex(rightBottomPos);
		if (objectGrids.grids[3] == objectGrids.grids[0] || objectGrids.grids[3] == objectGrids.grids[1] || objectGrids.grids[3] == objectGrids.grids[2]) {
			objectGrids.grids[3] = -1;
		}

		return objectGrids;
	}

	void Scene::GenFoods(uint32_t uNum) {
		Vector2D pos;
		uint32_t uGridIndex = 0;

		for (uint32_t i = 0; i < uNum; ++i) {
			pos.x = (float)cputil::GenFloatRandom(1.0f, (float)GridSize * m_uHorizontalGridNum);
			pos.y = (float)cputil::GenFloatRandom(1.0f, (float)GridSize * m_uVerticalGridNum);

			Food* pFood = gpFactory->CreateFood(m_uFoodId++, pos, 1);
			if (!pFood) {
				return;
			}
			uGridIndex = GetGridIndex(pos);
			m_pGrids[uGridIndex].AddObj(pFood);
			++m_uFoodCount;
		}
	}

	void Scene::GenFoods(const Vector2D& pos, uint32_t uNum, uint32_t uValue, list<Food*>& foodList) {
		for (uint32_t i = 0; i < uNum; ++i) {
			Food* pFood = gpFactory->CreateFood(m_uFoodId++, pos, uValue);
			if (!pFood) {
				return;
			}
			uint32_t uGridIndex = GetGridIndex(pos);
			m_pGrids[uGridIndex].AddObj(pFood);
			++m_uFoodCount;

			foodList.push_back(pFood);
		}
	}

	// 生成蛇，参数bRobot 如果为true，说明是系统生成的机器蛇
	Snake* Scene::GenSnake(bool bRobot) {
		Vector2D pos;
		uint32_t uGridIndex = 0;

		pos.x = (float)cputil::GenFloatRandom(80.0f, 800.0f);
		pos.y = (float)cputil::GenFloatRandom(80.0f, 800.0f);

		Snake* pSnake = gpFactory->CreateSnake(this, m_uSnakeId++, pos, 20, bRobot);
		if (!pSnake) {
			return NULL;
		}
		m_snakeMap.insert(make_pair(pSnake->GetSnakeId(), pSnake));

		// 将整条蛇各个部分加入Grid中
		// 将蛇头加入格子中
		ObjectGrids objectGrids = GetObjectGrids(pSnake->GetSnakeHead());
		for (uint32_t i = 0; i < MAX_GRID_IN; ++i) {
			if (objectGrids.grids[i] == -1) {
				continue;
			}
			m_pGrids[objectGrids.grids[i]].AddObj(pSnake->GetSnakeHead());
		}

		// 将蛇身加入格子中
		vector<SnakeBodyNode*>& snakeBodyVec = pSnake->GetSnakeBody();
		vector<SnakeBodyNode*>::iterator bodyIt = snakeBodyVec.begin();
		for (; bodyIt != snakeBodyVec.end(); bodyIt++) {
			SnakeBodyNode* pNode = *bodyIt;
			ObjectGrids objectGrids = GetObjectGrids(pNode);
			for (uint32_t i = 0; i < MAX_GRID_IN; ++i) {
				if (objectGrids.grids[i] == -1) {
					continue;
				}
				m_pGrids[objectGrids.grids[i]].AddObj(*bodyIt);
			}
		}

		return pSnake;
	}

	// 蛇死亡后，将蛇分解掉
	void Scene::BreakUpSnake(Snake* pSnake, list<Snake*>& broadcastList) {
		if (!pSnake) {
			return;
		}

		ObjectGrids objGrids = GetObjectGrids(pSnake->GetSnakeHead());
		for (int i = 0; i < MAX_GRID_IN; ++i) {
			if (objGrids.grids[i] == -1) {
				continue;
			}
			m_pGrids[objGrids.grids[i]].DelObj(pSnake->GetSnakeHead());
		}

		list<Food*> foodList;

		vector<SnakeBodyNode*>& snakeBodeVec = pSnake->GetSnakeBody();
		vector<SnakeBodyNode*>::iterator bodyIt = snakeBodeVec.begin();
		vector<SnakeBodyNode*>::iterator bodyItEnd = snakeBodeVec.end();
		for (; bodyIt != bodyItEnd; bodyIt++) {
			GenFoods((*bodyIt)->GetPos(), 1, 1, foodList);					// 后面根据body的半径决定生成的数量和值
			objGrids = GetObjectGrids(*bodyIt);
			for (int i = 0; i < MAX_GRID_IN; ++i) {
				if (objGrids.grids[i] == -1) {
					continue;
				}
				m_pGrids[objGrids.grids[i]].DelObj(*bodyIt);
			}
		}

		// 通知生成新的食物了
		BroadcastNewFoods(broadcastList, foodList);
		return;
	}

	void Scene::PlayerEnter(IConnection* pConn, uint32_t uUserId) {
		Snake* pSnake = GenSnake(false);
		if (!pSnake) {
			ERRORLOG("GenSnake failed.");
			return;
		}

		cout << "connection=[" << pConn << "] enter, snake=[" << pSnake << "]" << endl;

		pSnake->SetConnection(pConn);
		gpPlayerMng->SetPlayerConn(pSnake, pConn);

		slither::EnterGameAck enterGameAck;
		pSnake->SerializeToPB(*enterGameAck.mutable_snake());

		string strResponse;
		cputil::BuildResponseProto(enterGameAck, strResponse, REQ_ENTER_GAME);
		pSnake->SendMsg(strResponse.c_str(), strResponse.size());
	}

	void Scene::SnakeMove(Snake* pSnake, float fNewAngle, bool bSpeedUp) {
		if (!pSnake) {
			return;
		}

		pSnake->SetAngle(fNewAngle);
		pSnake->SetSpeedUp(bSpeedUp);			// 是否加速
	}

	vector<uint16_t> Scene::GetInViewGrids(Snake* pSnake) {
		vector<uint16_t> gridsVec;

		if (!pSnake) {
			return gridsVec;
		}

		const Vector2D& snakeHeadPos = pSnake->GetSnakeHead()->GetPos();
		uint32_t uViewRange = pSnake->GetViewRange();
		Vector2D leftTop(snakeHeadPos.x - uViewRange / 2, snakeHeadPos.y + uViewRange / 2);
		Vector2D rightTop(snakeHeadPos.x + uViewRange / 2, snakeHeadPos.y + uViewRange / 2);
		Vector2D leftBottom(snakeHeadPos.x - uViewRange / 2, snakeHeadPos.y - uViewRange / 2);
		Vector2D rightBottom(snakeHeadPos.x + uViewRange / 2, snakeHeadPos.y - uViewRange / 2);

		uint16_t uLeftTopIndex		= GetGridIndex(leftTop);
		uint16_t uRightTopIndex		= GetGridIndex(rightTop);
		uint16_t uLeftBottomIndex	= GetGridIndex(leftBottom);
		uint16_t uRightBottomIndex	= GetGridIndex(rightBottom);

		for (uint16_t i = uLeftBottomIndex; i <= uLeftTopIndex; i += m_uHorizontalGridNum) {
			for (uint16_t j = uLeftBottomIndex; j <= uRightBottomIndex; ++j) {
				uint16_t uViewIndex = j + (i - uLeftBottomIndex);
				if (uViewIndex >= m_uHorizontalGridNum * m_uVerticalGridNum) {
					continue;
				}
				gridsVec.push_back(uViewIndex);
			}
		}

		return gridsVec;
	}

	// 获取需要通知的列表
	void Scene::GetBroadcastList(Snake* pSnake, list<Snake*>& broadcastList) {
		uint16_t uGridIndex = GetGridIndex(pSnake->GetSnakeHead()->GetPos());
		vector<uint16_t> gridVec = GetInViewGrids(pSnake);

		for (uint16_t i = 0; i < gridVec.size(); ++i) {
			uint16_t uIndex = gridVec[i];
			list<Object*>& snakeHeadList = m_pGrids[uIndex].GetSnakeHeadList();

			list<Object*>::iterator snakeIt = snakeHeadList.begin();
			list<Object*>::iterator snakeItEnd = snakeHeadList.end();
			for (; snakeIt != snakeItEnd; snakeIt++) {

				SnakeBodyNode* pSnakeHead = (SnakeBodyNode*)*snakeIt;
				if (!pSnakeHead) {
					continue;
				}
				Snake* pTmpSnake = pSnakeHead->GetOwner();
				if (!pTmpSnake) {
					continue;
				}

				// 在视野内
				if (pTmpSnake->IsInView(pSnake)) {
					broadcastList.push_back(pTmpSnake);
				}
			}
		}

		broadcastList.push_back(pSnake);						// 加上自己
	}
	
	// 将移动通知消息和吃食物消息合并了，减少消息通知量
	void Scene::CheckEatFood(Snake* pSnake, uint16_t uGridIndex, list<Snake*>& broadcastList) {
		if (!pSnake) {
			return;
		}

		if (uGridIndex >= m_uHorizontalGridNum * m_uVerticalGridNum) {			// 超出上限了，说明出错了
			ERRORLOG("error grid index=[" << uGridIndex << "]");
			return;
		}

		slither::BroadcastEat eatNty;
		eatNty.set_snakeid(pSnake->GetSnakeId());

		// 判定吃食物
		list<Object*>& foodObjList = m_pGrids[uGridIndex].GetFoodList();
		list<Object*>::iterator foodIt = foodObjList.begin();
		for (; foodIt != foodObjList.end();) {
			Food* pTmpFood = (Food*)*foodIt;
			if (!pTmpFood) {
				foodIt++;
				continue;
			}

			if (pSnake->GetSnakeHead()->IsContained(pTmpFood->GetPos())) {
				eatNty.add_eatenfoodlist(pTmpFood->GetFoodId());

				// 蛇吃到了食物
				pSnake->EatFood(pTmpFood);

				// 删除食物
				foodObjList.erase(foodIt++);
				gpFactory->ReleaseFood(pTmpFood);

				continue;
			}
			foodIt++;
		}

		// 广播
		BroadcastEat(broadcastList, eatNty);

		return;
	}

	void Scene::CheckCollide(Snake* pSnake, uint16_t uGridIndex, list<Snake*>& broadcastList) {
		// 然后判断蛇头和其他蛇的碰撞
		list<Object*>& collidionObjList = m_pGrids[uGridIndex].GetSnakeBodyList();
		list<Object*>::iterator collideIt = collidionObjList.begin();
		list<Object*>::iterator collideItEnd = collidionObjList.end();
		for (; collideIt != collideItEnd; collideIt++) {
			SnakeBodyNode* pTmpObj = (SnakeBodyNode*)*collideIt;
			if (!pTmpObj) {
				continue;
			}
			if (pTmpObj->GetOwner() == pSnake) {									// 自己身体部分
				continue;
			}
			// 如果发生了碰撞，那么将此蛇销毁掉，然后身体分解成食物
			if (pSnake->GetSnakeHead()->IsCollide(pTmpObj)) {

				pSnake->SetStatus(OBJ_DESTROY);						// 将状态设置为“销毁”

				// 向周围广播碰撞
				BroadcastCollide(broadcastList, pSnake);

				// 将蛇分解掉
				BreakUpSnake(pSnake, broadcastList);
				cout << "snake=[" << pSnake << "] collide" << endl;
			}
		}
	}

	void Scene::BroadcastMove(Snake* pSnake, list<Snake*>& broadcastList) {

		// 正常的移动消息包
		slither::BroadcastMove snakeMove;								
		pSnake->SerializeToPB(*snakeMove.mutable_snakeinfo());
		string strResponse;
		cputil::BuildResponseProto(snakeMove, strResponse, BROADCAST_MOVE);

		// 只有蛇头的简要包
		slither::BroadcastMove briefSnakeMove;							
		briefSnakeMove.CopyFrom(snakeMove);
		briefSnakeMove.mutable_snakeinfo()->clear_snakebody();
		string strBriefResponse;
		cputil::BuildResponseProto(briefSnakeMove, strBriefResponse, BROADCAST_MOVE);

		list<Snake*>::iterator snakeIt = broadcastList.begin();
		list<Snake*>::iterator snakeItEnd = broadcastList.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pTmpSnake = *snakeIt;
			if (!pTmpSnake) {
				continue;
			}

			// 如果已经在对方的视野里面了，那么只需要简单的通知蛇头的信息即可
			if (pSnake->HasInView(pTmpSnake)) {
				//pTmpSnake->SendMsg(strBriefResponse.c_str(), strBriefResponse.size());
				pTmpSnake->SendMsg(strResponse.c_str(), strResponse.size());
			}
			else {
				pTmpSnake->SendMsg(strResponse.c_str(), strResponse.size());
				pSnake->AddInView(pTmpSnake);
			}	
		}

		return;
	}

	void Scene::BroadcastEat(list<Snake*>& broadcastList, slither::BroadcastEat& eatFoods) {
		string strResponse;
		cputil::BuildResponseProto(eatFoods, strResponse, BROADCAST_EAT);

		list<Snake*>::iterator snakeIt = broadcastList.begin();
		list<Snake*>::iterator snakeItEnd = broadcastList.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pTmpSnake = *snakeIt;
			if (!pTmpSnake) {
				continue;
			}

			pTmpSnake->SendMsg(strResponse.c_str(), strResponse.size());
		}
	}

	void Scene::BroadcastCollide(list<Snake*>& broadcastList, Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		slither::BroadcastCollide snakeCollide;
		pSnake->SerializeToPB(*snakeCollide.mutable_collidesnake());

		string strResponse;
		cputil::BuildResponseProto(snakeCollide, strResponse, BROADCAST_COLLIDE);
		
		// 挨个广播
		list<Snake*>::iterator snakeIt = broadcastList.begin();
		list<Snake*>::iterator snakeItEnd = broadcastList.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pSnake = *snakeIt;
			if (!pSnake) {
				continue;
			}

			pSnake->SendMsg(strResponse.c_str(), strResponse.size());
		}

		return;
	}

	void Scene::BroadcastNewFoods(list<Snake*>& broadcastList, const list<Food*>& newFoodList) {
		slither::BroadcastNewFood newFoods;
		list<Food*>::const_iterator foodIt = newFoodList.begin();
		list<Food*>::const_iterator foodItEnd = newFoodList.end();
		for (; foodIt != foodItEnd; foodIt++) {
			slither::PBFood* pPBFood = newFoods.add_foodlist();
			(*foodIt)->SerializeToPB(*pPBFood);
		}
		string strResponse;
		cputil::BuildResponseProto(newFoods, strResponse, BROADCAST_NEWFOODS);

		// 挨个广播
		list<Snake*>::iterator snakeIt = broadcastList.begin();
		list<Snake*>::iterator snakeItEnd = broadcastList.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pSnake = *snakeIt;
			if (!pSnake) {
				continue;
			}

			pSnake->SendMsg(strResponse.c_str(), strResponse.size());
		}
	}

	void Scene::BroadcastNewTail(Snake* pSnake, list<Snake*>& broadcastList) {
		if (!pSnake) {
			return;
		}

		slither::BroadcastNewTail newTails;
		SnakeBodyNode* pNewTail = pSnake->GetSnakeTail();
		newTails.set_snakeid(pSnake->GetSnakeId());
		slither::PBSnakeBody* pPBBody = newTails.add_newbodylist();
		pPBBody->set_bodyid(pNewTail->GetNodeId());
		pPBBody->mutable_pos()->set_x(pNewTail->GetPos().x);
		pPBBody->mutable_pos()->set_y(pNewTail->GetPos().y);

		string strResponse;
		cputil::BuildResponseProto(newTails, strResponse, BROADCAST_NEWTAIL);

		// 挨个广播
		list<Snake*>::iterator snakeIt = broadcastList.begin();
		list<Snake*>::iterator snakeItEnd = broadcastList.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pSnake = *snakeIt;
			if (!pSnake) {
				continue;
			}

			pSnake->SendMsg(strResponse.c_str(), strResponse.size());
		}
	}

	void Scene::NotifyGrids(Snake* pSnake) {

		slither::NotifyGrids gridsNtfy;

		uint16_t uGridIndex = GetGridIndex(pSnake->GetSnakeHead()->GetPos());
		vector<uint16_t> gridVec = GetInViewGrids(pSnake);

		uint32_t uFoodCount = 0;
		for (uint16_t i = 0; i < gridVec.size(); ++i) {
			uint16_t uIndex = gridVec[i];
			if (pSnake->HasInView(&m_pGrids[uIndex])) {
				continue;
			}
			else {
				pSnake->AddInView(&m_pGrids[uIndex]);
			}

			list<Object*>& foodList = m_pGrids[uIndex].GetFoodList();
			list<Object*>::iterator foodIt = foodList.begin();
			list<Object*>::iterator foodItEnd = foodList.end();
			for (; foodIt != foodItEnd; foodIt++) {
				slither::PBFood* pPBFood = gridsNtfy.add_foodlist();

				Food* pFood = (Food*)*foodIt;
				pFood->SerializeToPB(*pPBFood);
				++uFoodCount;
			}
		}

		string strResponse;
		cputil::BuildResponseProto(gridsNtfy, strResponse, NOTIFY_GRIDS);
		pSnake->SendMsg(strResponse.c_str(), strResponse.size());
	}

	void Scene::TestScene() {
		Vector2D pos;
		uint32_t uGridIndex = 0;

		// 创建1w个食物
		GenFoods(20000);

		// 创建N条蛇
		for (int i = 0; i < 100; ++i) {
			GenSnake(true);
		}
	}
}