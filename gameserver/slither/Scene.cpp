#include "Scene.h"
#include "Snake.h"
#include "Food.h"
#include "../../utils/Utility.h"
#include "PlayerMng.h"
#include "../../protocol/slither_battle.pb.h"
#include "Factory.h"
#include "SlitherMath.h"
#include "../config/SlitherConfig.h"
#include <iostream>
#include <algorithm>
using namespace std;

namespace slither {

	// 往格子中添加物体
	void Grid::AddObj(Object* pObj) {
		if (!pObj) {
			return;
		}

		if (pObj->GetType() == Food_Type) {
			foodList.push_back(pObj);
		}
		else if (pObj->GetType() == Snake_Head_Type || pObj->GetType() == Snake_Body_Type) {

			SnakeBodyNode* pNode = (SnakeBodyNode*)pObj;
			Snake* pSnake = pNode->GetOwner();
			if (!pSnake) {
				return;
			}

			map<Snake*, set<Object*> >::iterator snakeIt = m_snakeMap.find(pSnake);
			if (snakeIt == m_snakeMap.end()) {						// 此蛇还不存在
				set<Object*> nodeList;
				nodeList.insert(pObj);
				m_snakeMap.insert(make_pair(pSnake, nodeList));

				ERRORLOG("grid =[" << x << "," << y << "] insert snake=[" << pSnake << "], node id=[" << pNode->GetNodeId() << "], node=[" << pNode << "] total nodes=[" << pSnake->GetSnakeBody().size() << "]");
			}
			else {
				(snakeIt->second).insert(pObj);
				ERRORLOG("grid =[" << x << "," << y << "] insert snake=[" << pSnake << "], node id=[" << pNode->GetNodeId() << "], node=[" << pNode << "] total nodes=[" << pSnake->GetSnakeBody().size() << "]");
			}
		}
	}

	// 从格子中删除物体
	void Grid::DelObj(Object* pObj) {
		if (!pObj) {
			return;
		}

		if (pObj->GetType() == Food_Type) {															// 食物
			list<Object*>::iterator objIt = foodList.begin();
			for (; objIt != foodList.end();) {
				if (*objIt == pObj) {
					foodList.erase(objIt++);
					break;
				}
				objIt++;
			}
		}
		else if (pObj->GetType() == Snake_Head_Type || pObj->GetType() == Snake_Body_Type) {		// 蛇
			SnakeBodyNode* pNode = (SnakeBodyNode*)pObj;
			if (!pNode) {
				return;
			}
			Snake* pSnake = pNode->GetOwner();
			if (!pSnake) {
				return;
			}

			map<Snake*, set<Object*> >::iterator snakeIt = m_snakeMap.find(pSnake);
			if (snakeIt == m_snakeMap.end()) {														// 此蛇还不存在, 理论上不会有这种情况
				ERRORLOG("grid =[" << x << "," << y << "] snake id=[" << pSnake->GetSnakeId() << "], just body in grid.");
				return;
			}

			ERRORLOG("grid =[" << x << "," << y << "] delete snake=[" << pSnake << "], node id=[" << pNode->GetNodeId() << "], node=[" << pNode << "] total nodes=[" << pSnake->GetSnakeBody().size() << "]");

			(snakeIt->second).erase(pObj);
			if ((snakeIt->second).empty()) {						// 如果蛇的节点都不在了，那么把蛇就从这个格子中删除
				m_snakeMap.erase(pSnake);
				ERRORLOG("grid =[" << x << "," << y << "] delete snake=[" << pSnake << "]");
			}
		}
	}

	bool Grid::IsExist(Snake* pSnake) {
		map<Snake*, set<Object*> >::iterator snakeIt = m_snakeMap.find(pSnake);
		if (snakeIt != m_snakeMap.end()) {
			return true;
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Scene::Scene() : m_uSnakeId(1), m_uFoodId(1), m_uFoodCount(0) {
	}

	Scene::~Scene() {
		delete[] m_pGrids;
	}

	// 初始化，根据整个地图的大小去计算格子数
	bool Scene::Init(uint32_t uSceneHeight, uint32_t uSceneWidth, uint32_t uGridSize) {
		uint32_t x = 0;
		uint32_t y = 0;

		m_fSceneLength = uSceneWidth;

		m_uGridSize = uGridSize;
		m_uHorizontalGridNum = uSceneWidth / uGridSize;						// 水平方向的格子数
		m_uVerticalGridNum = uSceneHeight / uGridSize;						// 垂直方向的格子数
		m_pGrids = new Grid[m_uHorizontalGridNum * m_uVerticalGridNum];

		uint32_t index = 0;
		// 构造格子
		for (uint32_t y = 0; y < uSceneHeight; y += uGridSize) {
			for (uint32_t x = 0; x < uSceneWidth; x += uGridSize) {
				Grid& grid = m_pGrids[index++];
				grid.x = x;													// 左下角坐标作为格子的坐标
				grid.y = y;
				grid.uHeight = uGridSize;
				grid.uWidth = uGridSize;
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

	void Scene::OnTimer() {
		boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration diff;

		uint32_t uGridIndex = 0;														// 蛇头所在的格子
		uint32_t uCount = 1;

		// 模拟所有蛇的运动
		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.begin();
		for (; snakeIt != m_snakeMap.end();) {
			Snake* pSnake = snakeIt->second;
			if (!pSnake) {
				continue;
			}

			// 测试使用
			//bool bRobot = pSnake->IsRobot();
			//if (!bRobot) {
			//	cout << "player msg count=" << pSnake->GetMsgCount() << ", total size=" << pSnake->GetMsgSize() / 1024.0 << "k" << endl;
			//}

			// 蛇移动前，先删除当前能看见的格子列表
			DeleteSnakeViewGrids(pSnake);

			// 不在其他地方删除，否则不好控制
			if (pSnake->GetStatus() == OBJ_DESTROY) {									// 如果蛇需要销毁
				// 删除蛇与链接之间的关系
				gpPlayerMng->DeleteSnake(pSnake);

				// 释放蛇
				DEBUGLOG("delete snake id=[" << pSnake->GetSnakeId() << "], addr=[" << pSnake << "]");
				gpFactory->ReleaseSnake(pSnake);
				m_snakeMap.erase(snakeIt++);
				continue;
			}
			
			// 让蛇运动
			pSnake->Move();

			// 更新蛇当前能看见的格子列表
			UpdateSnakeViewGrids(pSnake);

			// 向此蛇通知周围的格子情况
			NotifyAround(pSnake);

			// 检查蛇的加速情况
			CheckSpeedUp(pSnake);

			// 判断蛇吃到食物的情况
			uGridIndex = GetGridIndex(pSnake->GetSnakeHead()->GetPos());				// 获取运动之后头所在的格子，限制每次运动不会超过一个蛇头的直径
			CheckEatFood(pSnake, uGridIndex);

			// 判断蛇的碰撞情况(碰撞一点要放到最后判断)
			CheckCollide(pSnake, uGridIndex);

			// TODO 根据参数定期生成食物
			if (m_uFoodCount < 4000) {
				GenFoods(2000);
			}

			//cout << "snake id=[" << pSnake->GetSnakeId() << "] pos=[" << pSnake->GetSnakeHead()->GetPos().x << ", " << pSnake->GetSnakeHead()->GetPos().y << "]" << endl;

			snakeIt++;
			//cout << endl << endl;
		}

		BroadcastRankingList();

		end = boost::posix_time::microsec_clock::local_time();
		diff = end - start;
		cout << "time cost=[" << diff.total_milliseconds() << "], snake count=[" << m_snakeMap.size() << "]" << endl;
	}

	uint32_t Scene::GetGridIndex(const Vector2D& pos) {
		uint32_t uHorizontalGridNum = uint32_t(pos.x / (gpSlitherConf->m_uGridSize));
		uint32_t uVerticalGridNum	= uint32_t(pos.y / (gpSlitherConf->m_uGridSize));

		uint32_t uIndex = uVerticalGridNum * m_uHorizontalGridNum + uHorizontalGridNum;
		if (uIndex >= m_uHorizontalGridNum * m_uVerticalGridNum) {
			// 需要根据判定边缘，目前只是简单的设置了上限
			uIndex = m_uHorizontalGridNum * m_uVerticalGridNum - 1;
		}
		return uIndex;
	}

	// 获取一个物体属于的所有格子，因为一个物体可能在几个格子的边缘，所以算是属于多个格子
	ObjectGrids Scene::GetObjectGrids(Object* pObj) {
		ObjectGrids objectGrids;
		if (!pObj) {
			return objectGrids;
		}

		return GetObjectGrids(pObj->GetPos(), pObj->GetRadius());
	}

	ObjectGrids Scene::GetObjectGrids(Vector2D& pos, float fRadius) {
		Vector2D leftTopPos(pos.x - fRadius, pos.y + fRadius);
		Vector2D rightTopPos(pos.x + fRadius, pos.y + fRadius);
		Vector2D leftBottomPos(pos.x - fRadius, pos.y - fRadius);
		Vector2D rightBottomPos(pos.x + fRadius, pos.y - fRadius);

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
			pos.x = (float)cputil::GenFloatRandom(1.0f, (float)gpSlitherConf->m_uGridSize * m_uHorizontalGridNum);
			pos.y = (float)cputil::GenFloatRandom(1.0f, (float)gpSlitherConf->m_uGridSize * m_uVerticalGridNum);

			uint32_t uFoodMass = cputil::GenRandom(gpSlitherConf->m_uGenFoodMinValue, gpSlitherConf->m_uGenFoodMaxValue);
			Food* pFood = gpFactory->CreateFood(m_uFoodId++, pos, uFoodMass);
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
		Vector2D pos = GenSnakeBornPos();
		Snake* pSnake = gpFactory->CreateSnake(this, m_uSnakeId++, pos, gpSlitherConf->m_uInitSnakeBodySize, bRobot);
		if (!pSnake) {
			return NULL;
		}
		
		AddSnakeInScene(pSnake);

		return pSnake;
	}

	void Scene::RefreshFoods() {

	}

	void Scene::AddSnakeInScene(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.find(pSnake->GetSnakeId());
		if (snakeIt != m_snakeMap.end()) {					// 如果已经存在了，那就不再添加
			return;
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
		list<SnakeBodyNode*>& snakeBodyList = pSnake->GetSnakeBody();
		list<SnakeBodyNode*>::iterator bodyIt = snakeBodyList.begin();
		for (; bodyIt != snakeBodyList.end(); bodyIt++) {
			SnakeBodyNode* pNode = *bodyIt;
			ObjectGrids objectGrids = GetObjectGrids(pNode);
			for (uint32_t i = 0; i < MAX_GRID_IN; ++i) {
				if (objectGrids.grids[i] == -1) {
					continue;
				}
				m_pGrids[objectGrids.grids[i]].AddObj(*bodyIt);
			}
		}
	}

	// 蛇死亡后，将蛇分解掉
	void Scene::BreakUpSnake(Snake* pSnake) {
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

		list<SnakeBodyNode*>& snakeBodyList = pSnake->GetSnakeBody();
		list<SnakeBodyNode*>::iterator bodyIt = snakeBodyList.begin();
		list<SnakeBodyNode*>::iterator bodyItEnd = snakeBodyList.end();
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
		BroadcastNewFoods(foodList);
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

		enterGameAck.set_scenewidth(m_uGridSize * m_uHorizontalGridNum);
		enterGameAck.set_gridwidth(m_uGridSize);

		string strResponse;
		cputil::BuildResponseProto(enterGameAck, strResponse, REQ_ENTER_GAME);
		pSnake->SendMsg(strResponse.c_str(), strResponse.size());
	}

	void Scene::PlayerEnter(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		// 可能玩家是断线重新进入游戏的，所以需要先清除玩家蛇的视野
		pSnake->ClearViews();

		AddSnakeInScene(pSnake);

		// 返回给客户端
		slither::EnterGameAck enterGameAck;
		pSnake->SerializeToPB(*enterGameAck.mutable_snake());

		enterGameAck.set_scenewidth(m_uGridSize * m_uHorizontalGridNum);
		enterGameAck.set_gridwidth(m_uGridSize);

		string strResponse;
		cputil::BuildResponseProto(enterGameAck, strResponse, REQ_ENTER_GAME);
		pSnake->SendMsg(strResponse.c_str(), strResponse.size());
	}

	void Scene::SnakeMove(Snake* pSnake, float fNewAngle, bool bSpeedUp, bool bStopMove) {
		if (!pSnake) {
			return;
		}

		pSnake->SetAngle(fNewAngle);
		if (pSnake->GetTotalMass() > gpSlitherConf->m_fSpeedUpNeedValue) {				// 必须满足加速的最小体重
			pSnake->SetSpeedUp(bSpeedUp);			// 是否加速
		}
		else {
			pSnake->SetSpeedUp(false);
		}

		if (bStopMove)
			pSnake->SetStopMove(bStopMove);
	}

	set<uint32_t> Scene::GetInViewGrids(Snake* pSnake) {
		set<uint32_t> gridsVec;					// 需要返回的列表
		if (!pSnake) {
			return gridsVec;
		}

		const Vector2D& snakeHeadPos = pSnake->GetSnakeHead()->GetPos();
		float fViewRange = pSnake->GetViewRange();
		Vector2D leftTop(max(snakeHeadPos.x - fViewRange / 2, 0.0f), min(snakeHeadPos.y + fViewRange / 2, m_fSceneLength));
		Vector2D rightTop(min(snakeHeadPos.x + fViewRange / 2, m_fSceneLength), min(snakeHeadPos.y + fViewRange / 2, m_fSceneLength));
		Vector2D leftBottom(max(snakeHeadPos.x - fViewRange / 2, 0.0f), max(snakeHeadPos.y - fViewRange / 2, 0.0f));
		Vector2D rightBottom(min(snakeHeadPos.x + fViewRange / 2, m_fSceneLength), max(snakeHeadPos.y - fViewRange / 2, 0.0f));

		uint32_t uLeftTopIndex = GetGridIndex(leftTop);
		uint32_t uRightTopIndex = GetGridIndex(rightTop);
		uint32_t uLeftBottomIndex = GetGridIndex(leftBottom);
		uint32_t uRightBottomIndex = GetGridIndex(rightBottom);

		for (uint32_t i = uLeftBottomIndex; i <= uLeftTopIndex; i += m_uHorizontalGridNum) {
			for (uint32_t j = uLeftBottomIndex; j <= uRightBottomIndex; ++j) {
				uint32_t uViewIndex = j + (i - uLeftBottomIndex);
				if (uViewIndex >= m_uHorizontalGridNum * m_uVerticalGridNum) {
					continue;
				}
				gridsVec.insert(uViewIndex);
			}
		}

		return gridsVec;
	}

	void Scene::GetInViewGrids(Snake* pSnake, set<uint32_t>& gridSet) {
		if (!pSnake) {
			return;
		}

		const Vector2D& snakeHeadPos = pSnake->GetSnakeHead()->GetPos();
		float fViewRange = pSnake->GetViewRange();
		Vector2D leftTop(max(snakeHeadPos.x - fViewRange / 2, 0.0f), min(snakeHeadPos.y + fViewRange / 2, m_fSceneLength));
		Vector2D rightTop(min(snakeHeadPos.x + fViewRange / 2, m_fSceneLength), min(snakeHeadPos.y + fViewRange / 2, m_fSceneLength));
		Vector2D leftBottom(max(snakeHeadPos.x - fViewRange / 2, 0.0f), max(snakeHeadPos.y - fViewRange / 2, 0.0f));
		Vector2D rightBottom(min(snakeHeadPos.x + fViewRange / 2, m_fSceneLength), max(snakeHeadPos.y - fViewRange / 2, 0.0f));

		uint32_t uLeftTopIndex = GetGridIndex(leftTop);
		uint32_t uRightTopIndex = GetGridIndex(rightTop);
		uint32_t uLeftBottomIndex = GetGridIndex(leftBottom);
		uint32_t uRightBottomIndex = GetGridIndex(rightBottom);

		for (uint32_t i = uLeftBottomIndex; i <= uLeftTopIndex; i += m_uHorizontalGridNum) {
			for (uint32_t j = uLeftBottomIndex; j <= uRightBottomIndex; ++j) {
				uint32_t uViewIndex = j + (i - uLeftBottomIndex);
				if (uViewIndex >= m_uHorizontalGridNum * m_uVerticalGridNum) {
					continue;
				}
				gridSet.insert(uViewIndex);
			}
		}

		return;
	}

	void Scene::DeleteSnakeViewGrids(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		set<uint32_t> gridVec;
		GetInViewGrids(pSnake, gridVec);									// 获取蛇所能看见的格子列表

		set<uint32_t>::iterator gridIt = gridVec.begin();
		set<uint32_t>::iterator gridItEnd = gridVec.end();
		for (; gridIt != gridItEnd; gridIt++) {
			SnakeSet& snakeSet = GetSnakeSet(*gridIt);
			snakeSet.erase(pSnake);											// 从格子列表中删除
		}
	}

	void Scene::UpdateSnakeViewGrids(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		set<uint32_t> gridVec = GetInViewGrids(pSnake);					// 获取蛇所能看见的格子列表

		set<uint32_t>::iterator gridIt = gridVec.begin();
		set<uint32_t>::iterator gridItEnd = gridVec.end();
		for (; gridIt != gridItEnd; gridIt++) {
			SnakeSet& snakeSet = GetSnakeSet(*gridIt);
			snakeSet.insert(pSnake);									// 在格子的列表中添加
		}
	}

	SnakeSet& Scene::GetSnakeSet(uint32_t uGridId) {
		map<uint32_t, SnakeSet>::iterator setIt = m_gridInSnakeViewMap.find(uGridId);
		if (setIt != m_gridInSnakeViewMap.end()) {
			return setIt->second;
		}

		SnakeSet snakeSet;
		m_gridInSnakeViewMap.insert(make_pair(uGridId, snakeSet));
		return m_gridInSnakeViewMap[uGridId];
	}

	Vector2D Scene::GenSnakeBornPos() {
		Vector2D pos;
		pos.x = (float)cputil::GenFloatRandom(19.0f, 190.0f);
		pos.y = (float)cputil::GenFloatRandom(19.0f, 190.0f);
		return pos;
	}

	void Scene::CheckSpeedUp(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		if (pSnake->GetSpeedUp()) {
			// 计算加速消耗
			pSnake->SpeedUpCost();
			if (pSnake->GetTotalMass() <= gpSlitherConf->m_fStopSpeedUpValue) {			// 低于能加速的值了，停止加速
				pSnake->SetSpeedUp(false);
			}

			// 生成加速的食物
			if (pSnake->GetMoveTick() % (uint32_t)(gpSlitherConf->m_fSpawnInterval * 10) == 0) {
				list<Food*> foodList;
				GenFoods(pSnake->GetSnakeTail()->GetPos(), 1, gpSlitherConf->m_uSpeedUpGenFoodValue, foodList);
				// 通知生成新的食物了
				BroadcastNewFoods(foodList);											// 这个函数有待优化
			}
		}
	}
	
	// 将移动通知消息和吃食物消息合并了，减少消息通知量
	void Scene::CheckEatFood(Snake* pSnake, uint16_t uGridIndex) {
		if (!pSnake) {
			return;
		}

		if (uGridIndex >= m_uHorizontalGridNum * m_uVerticalGridNum) {			// 超出上限了，说明出错了
			ERRORLOG("error grid index=[" << uGridIndex << "]");
			return;
		}

		slither::BroadcastEat eatNty;
		eatNty.set_snakeid(pSnake->GetSnakeId());

		ObjectGrids objGrids = GetObjectGrids(pSnake->GetSnakeHead()->GetPos(), gpSlitherConf->m_fEatFoodRadius);
		for (int i = 0; i < MAX_GRID_IN; ++i) {
			int32_t uIndex = objGrids.grids[i];
			if (uIndex < 0) {
				continue;
			}

			// 判定吃食物
			list<Object*>& foodObjList = m_pGrids[uIndex].GetFoodList();
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
		}

		
		eatNty.set_newmass(pSnake->GetTotalMass());

		// 广播
		SnakeSet& snakeSet = GetSnakeSet(uGridIndex);
		BroadcastEat(snakeSet, eatNty);

		return;
	}

	void Scene::CheckCollide(Snake* pSnake, uint16_t uGridIndex) {

		SnakeSet& snakeSet = GetSnakeSet(uGridIndex);									// 能看见此格子的蛇的列表

		ObjectGrids objGrids = GetObjectGrids(pSnake->GetSnakeHead());
		for (int i = 0; i < MAX_GRID_IN; ++i) {
			int32_t uIndex = objGrids.grids[i];
			if (uIndex < 0) {
				continue;
			}

			// 然后判断蛇和其他蛇的碰撞
			const map<Snake*, set<Object*> >& gridSnakeMap = m_pGrids[uIndex].GetSnakeList();					// 获取当前格子的其他蛇

			map<Snake*, set<Object*> >::const_iterator collideIt = gridSnakeMap.begin();
			map<Snake*, set<Object*> >::const_iterator collideItEnd = gridSnakeMap.end();
			for (; collideIt != collideItEnd; collideIt++) {
				Snake* pTmpSnake = collideIt->first;
				if (!pTmpSnake || pTmpSnake == pSnake || pTmpSnake->GetStatus() == OBJ_DESTROY) {				// 如果是自己, 或者死亡了就不做判断了
					continue;
				}

				set<Object*>::iterator objIt = (collideIt->second).begin();
				set<Object*>::iterator objItEnd = (collideIt->second).end();
				for (; objIt != objItEnd; objIt++) {
					SnakeBodyNode* pTmpObj = (SnakeBodyNode*)*objIt;
					if (!pTmpObj) {
						continue;
					}
					
					// 如果发生了碰撞，那么将此蛇销毁掉，然后身体分解成食物
					if (pSnake->GetSnakeHead()->IsCollide(pTmpObj)) {

						pTmpObj->GetOwner()->IncKillTimes();								// 增加被碰撞的蛇的击杀次数
						pSnake->SetStatus(OBJ_DESTROY);										// 将状态设置为“销毁”

						// 向周围广播碰撞
						BroadcastCollide(snakeSet, pSnake);

						// 将蛇分解掉
						BreakUpSnake(pSnake);
						DEBUGLOG("snake id=" << pSnake->GetSnakeId() << " addr=" << pSnake << " collide in grid=" << uIndex);
						return;																// 不用再做其他碰撞测试了
					}
				}
			}
		}
	}

	void Scene::BroadcastEat(set<Snake*>& broadcastList, slither::BroadcastEat& eatFoods) {
		string strResponse;
		cputil::BuildResponseProto(eatFoods, strResponse, BROADCAST_EAT);

		set<Snake*>::iterator snakeIt = broadcastList.begin();
		set<Snake*>::iterator snakeItEnd = broadcastList.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pTmpSnake = *snakeIt;
			if (!pTmpSnake) {
				continue;
			}
			pTmpSnake->SendMsg(strResponse.c_str(), strResponse.size());
		}
	}

	void Scene::BroadcastCollide(set<Snake*>& broadcastList, Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		slither::BroadcastCollide snakeCollide;
		pSnake->SerializeToPB(*snakeCollide.mutable_collidesnake());

		string strResponse;
		cputil::BuildResponseProto(snakeCollide, strResponse, BROADCAST_COLLIDE);
		
		// 挨个广播
		set<Snake*>::iterator snakeIt = broadcastList.begin();
		set<Snake*>::iterator snakeItEnd = broadcastList.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pSnake = *snakeIt;
			if (!pSnake) {
				continue;
			}

			pSnake->SendMsg(strResponse.c_str(), strResponse.size());
		}

		return;
	}

	void Scene::BroadcastNewFoods(const list<Food*>& newFoodList) {
		slither::BroadcastNewFood newFoods;

		set<uint32_t> gridSet;

		list<Food*>::const_iterator foodIt = newFoodList.begin();
		list<Food*>::const_iterator foodItEnd = newFoodList.end();
		for (; foodIt != foodItEnd; foodIt++) {
			slither::PBFood* pPBFood = newFoods.add_foodlist();
			(*foodIt)->SerializeToPB(*pPBFood);

			uint32_t uGridIndex = GetGridIndex((*foodIt)->GetPos());
			gridSet.insert(uGridIndex);
		}
		string strResponse;
		cputil::BuildResponseProto(newFoods, strResponse, BROADCAST_NEWFOODS);

		// 向能看见的蛇广播
		set<uint32_t>::iterator gridIt = gridSet.begin();
		set<uint32_t>::iterator gridItEnd = gridSet.end();
		for (; gridIt != gridItEnd; gridIt++) {
			SnakeSet& snakeSet = GetSnakeSet(*gridIt);

			SnakeSet::iterator snakeIt = snakeSet.begin();
			SnakeSet::iterator snakeItEnd = snakeSet.end();
			for (; snakeIt != snakeItEnd; snakeIt++) {
				Snake* pSnake = *snakeIt;
				if (!pSnake) {
					continue;
				}

				pSnake->SendMsg(strResponse.c_str(), strResponse.size());
			}
		}
	}

	bool Greater(const Snake* pSnake1, const Snake* pSnake2) {
		if (pSnake1->GetTotalMass() > pSnake2->GetTotalMass()) {
			return true;
		}

		return false;
	}

	void Scene::BroadcastRankingList() {

		vector<Snake*> snakeVec;

		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.begin();
		map<uint32_t, Snake*>::iterator snakeItEnd = m_snakeMap.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pSnake = snakeIt->second; 
			if (!pSnake || pSnake->GetStatus() == OBJ_DESTROY) {
				continue;
			}

			snakeVec.push_back(pSnake);
		}

		std::sort(snakeVec.begin(), snakeVec.end(), Greater);
		
		// 通知给每个玩家
		slither::BroadcastRankingList rankingList;
		uint32_t uRankCount = std::min((size_t)10, snakeVec.size());
		for (uint32_t i = 0; i < uRankCount; ++i) {
			slither::PBPlayerRank* pPBPlayerRank = rankingList.add_rankinglist();
			//pPBPlayerRank->set_playername()
			pPBPlayerRank->set_mass(snakeVec[i]->GetTotalMass());
		}

		vector<Snake*>::iterator rankIt = snakeVec.begin();
		vector<Snake*>::iterator rankItEnd = snakeVec.end();
		uint32_t uRank = 1;
		for (; rankIt != rankItEnd; rankIt++) {
			Snake* pSnake = *rankIt;
			rankingList.set_selfrank(uRank++);

			string strResponse;
			cputil::BuildResponseProto(rankingList, strResponse, slither::BROADCAST_RANKING_LIST);
			pSnake->SendMsg(strResponse.c_str(), strResponse.size());
		}

		return;
	}

	// 向这条蛇通知周围的情况
	void Scene::NotifyAround(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		set<uint32_t> gridVec = GetInViewGrids(pSnake);

		string strResponse;
		slither::NotifyAround aroundNty;

		set<Snake*> hasNotifySnakeSet;								// 已经通知了的蛇的列表

		set<uint32_t>::iterator gridIt = gridVec.begin();
		set<uint32_t>::iterator gridItEnd = gridVec.end();
		for (; gridIt != gridItEnd; gridIt++) {
			uint16_t uIndex = *gridIt;

			aroundNty.add_inviewgrids(uIndex);

			// 通知这条蛇，周围其他蛇目前的情况 
			const map<Snake*, set<Object*> >& gridSnakeMap = m_pGrids[uIndex].GetSnakeList();
			map<Snake*, set<Object*> >::const_iterator snakeIt = gridSnakeMap.begin();
			map<Snake*, set<Object*> >::const_iterator snakeItEnd = gridSnakeMap.end();
			for (; snakeIt != snakeItEnd; snakeIt++) {

				Snake* pTmpSnake = snakeIt->first;
				if (!pTmpSnake) {
					continue;
				}

				set<Snake*>::iterator notifyIt = hasNotifySnakeSet.find(pTmpSnake);
				if (notifyIt != hasNotifySnakeSet.end()) {				// 如果这次已经通知过了，就不在通知了
					continue;
				}

				slither::PBSnake* pPBSnake = aroundNty.add_snakelist();
				hasNotifySnakeSet.insert(pTmpSnake);

				// 如果已经在视野中了，那么只需要知道蛇头的信息就可以了
				if (pSnake->HasInView(pTmpSnake)) {
					// 序列化蛇头
					pTmpSnake->SerializeToPB(*pPBSnake, true);
					//pTmpSnake->SerializeToPB(*pPBSnake);
				}
				else {
					pTmpSnake->SerializeToPB(*pPBSnake);
					pSnake->AddInView(pTmpSnake);
				}
			}

			// 通知这条蛇，周围食物的情况
			if (pSnake->HasInView(uIndex)) {
				continue;
			}
			else {
				list<Object*>& foodList = m_pGrids[uIndex].GetFoodList();
				list<Object*>::iterator foodIt = foodList.begin();
				list<Object*>::iterator foodItEnd = foodList.end();
				for (; foodIt != foodItEnd; foodIt++) {
					slither::PBFood* pPBFood = aroundNty.add_foodlist();

					Food* pFood = (Food*)*foodIt;
					pFood->SerializeToPB(*pPBFood);
				}
				pSnake->AddInView(uIndex);
			}
		}

		// 发送给客户端
		cputil::BuildResponseProto(aroundNty, strResponse, slither::NOTIFY_AROUND);
		pSnake->SendMsg(strResponse.c_str(), strResponse.size());
	}

	void Scene::TestScene() {
		Vector2D pos;
		uint32_t uGridIndex = 0;

		// 创建1w个食物
		GenFoods(6000);

		// 创建N条蛇
		for (int i = 0; i < 0; ++i) {
			GenSnake(true);
		}
	}

	void Scene::Finish() {
		// 先计算所有玩家排名 
		vector<Snake*> snakeVec;

		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.begin();
		map<uint32_t, Snake*>::iterator snakeItEnd = m_snakeMap.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pSnake = snakeIt->second;
			if (!pSnake || pSnake->GetStatus() == OBJ_DESTROY) {
				continue;
			}

			snakeVec.push_back(pSnake);
		}

		std::sort(snakeVec.begin(), snakeVec.end(), Greater);

		// 设置所有玩家排名
		uint32_t uRank = 1;
		vector<Snake*>::iterator rankIt = snakeVec.begin();
		vector<Snake*>::iterator rankItEnd = snakeVec.end();
		for (; rankIt != rankItEnd; rankIt++) {
			Snake* pTmpSnake = *rankIt;
			if (!pTmpSnake) {
				continue;
			}

			pTmpSnake->SetRank(uRank++);
		}
	}
}