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
#include "GameRoom.h"
#include "SlitherMath.h"
using namespace std;

namespace slither {

	Scene::Scene(GameRoom* pGameRoom) : m_uSnakeId(1), m_uFoodId(1), m_uFoodCount(0), m_pGameRoom(pGameRoom), m_fastIndexArray(NULL) {
		m_startTime = boost::posix_time::second_clock::local_time();
		m_lastBroadcastRankTime = boost::posix_time::microsec_clock::local_time();
	}

	Scene::~Scene() {

		// 删除蛇
		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.begin();
		map<uint32_t, Snake*>::iterator snakeItEnd = m_snakeMap.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pSnake = snakeIt->second;
			if (!pSnake) {
				return;
			}

			gpPlayerMng->DeleteSnake(pSnake);							// 删除对应连接
			gpFactory->ReleaseSnake(pSnake);
		}

		// 删除食物
		for (uint32_t i = 0; i < m_uHorizontalGridNum * m_uVerticalGridNum - 1; ++i)
		{
			Grid& grid = m_pGrids[i];
			list<Object*>& objList = grid.GetFoodList();
			list<Object*>::iterator objIt = objList.begin();
			list<Object*>::iterator objItEnd = objList.end();
			for (; objIt != objItEnd; objIt++) {
				Object* pObj = *objIt;
				gpFactory->ReleaseFood((Food*)pObj);
			}
		}
		
		// 删除快速查找数组
		if (m_fastIndexArray != NULL) {
			for (uint32_t i = 0; i < (uint32_t)m_fSceneLength; ++i) {
				delete[] m_fastIndexArray[i];
			}
		}

		delete[] m_pGrids;
	}

	// 初始化，根据整个地图的大小去计算格子数
	bool Scene::Init(uint32_t uSceneHeight, uint32_t uSceneWidth, uint32_t uGridSize) {

		m_fSceneLength = (float)uSceneWidth;

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

		// 构造快速查找数组
		uint32_t h_index = 0;			// 水平索引
		uint32_t v_index = 0;			// 垂直索引
		m_fastIndexArray = new uint32_t*[uSceneWidth];
		for (uint32_t i = 0; i < uSceneWidth; ++i) {
			m_fastIndexArray[i] = new uint32_t[uSceneHeight];
			
			for (uint32_t j = 0; j < uSceneHeight; ++j) {
				m_fastIndexArray[i][j] = (i / uGridSize) * m_uHorizontalGridNum + (j / uGridSize);
				++h_index;
			}
		}

		return true;
	}

	Snake* Scene::GetSnakeById(uint32_t uSnakeId) {
		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.find(uSnakeId);
		if (snakeIt == m_snakeMap.end()) {
			return NULL;
		}

		Snake* pSnake = snakeIt->second;
		return pSnake;
	}

	Snake* Scene::GetSnakeByPlayerId(uint64_t uPlayerId) {
		map<uint64_t, Snake*>::iterator snakeIt = m_playerToSnakeMap.find(uPlayerId);
		if (snakeIt == m_playerToSnakeMap.end()) {
			return NULL;
		}

		Snake* pSnake = snakeIt->second;
		return pSnake;
	}

	void Scene::OnTimer() {
		boost::posix_time::ptime start	= boost::posix_time::microsec_clock::local_time();
		boost::posix_time::ptime end	= boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff;											// 统计每个tick的耗时

		// 模拟所有蛇的行为
		SimulateSnakes();

		end = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration rankDiff = end - m_lastBroadcastRankTime;

		// 更新排行榜
		if (rankDiff.total_milliseconds() > 500) {					//  500ms更新一次
			BroadcastRankingList();
			m_lastBroadcastRankTime = boost::posix_time::microsec_clock::local_time();

			diff = end - start;
			DEBUGLOG("roomId=[0x" << hex << m_pGameRoom->GetRoomId() << dec << "] time cost=[" << diff.total_milliseconds() << "], snake count=[" << m_snakeMap.size() << "]");
		}

		// 如果有机器人，那么如果数量不足就重新刷
		if (m_snakeMap.size() < 40 && g_slitherConfig.m_uRobotNum > 0) {
			for (int i = 0; i < 10; ++i)
			{
				GenSnake(true);
			}
		}
	}

	void Scene::SimulateSnakes() {
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
			if (pSnake->GetStatus() == OBJ_DESTROY && m_pGameRoom->GetLeftTime() > 10) {					// 如果蛇需要销毁, 并且剩余时间大于10秒

				if (pSnake->GetHasDeadTime() < 3) {															// 死后延迟

					// 向此蛇通知周围的格子情况
					NotifyAround(pSnake);

					snakeIt++;
					continue;
				}

				// 先删除当前能看见的格子列表
				DeleteSnakeViewGrids(pSnake);

				// 如果已经死亡超过3秒了，然后就清理蛇
				// 删除蛇与链接之间的关系
				gpPlayerMng->DeleteSnake(pSnake);
				if (pSnake == GetSnakeByPlayerId(pSnake->GetPlayerId())) {
					// 先判断对应的是不是同一条蛇，因为蛇死亡之后，删除之前，玩家可能已经又进入游戏了，已经修改了对应的蛇关系
					m_playerToSnakeMap.erase(pSnake->GetPlayerId());
				}

				TRACELOG("m_playerToSnakeMap erase playerId=[" << pSnake->GetPlayerId() << "]");

				// 释放蛇
				DEBUGLOG("delete snake id=[" << pSnake->GetSnakeId() << "], addr=[" << pSnake << "], leftTime=[" << m_pGameRoom->GetLeftTime() << "]");
				gpFactory->ReleaseSnake(pSnake);
				m_snakeMap.erase(snakeIt++);
				continue;
			}

			if (pSnake->GetStatus() == OBJ_DESTROY) {									// 死亡的蛇不再做处理
				snakeIt++;
				continue;
			}

			// 先check蛇的状态
			pSnake->GetSnakeBuffStat().CheckBuffs();

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
			CheckCollideWithEgde(pSnake, uGridIndex);

			// TODO 根据参数定期生成食物
			if (m_uFoodCount < g_slitherConfig.m_uRefreshFoodThreshold) {
				RefreshFoods();
			}

			snakeIt++;
		}
	}

	// 获取一个物体属于的所有格子，因为一个物体可能在几个格子的边缘，所以算是属于多个格子
	ObjectGrids Scene::GetObjectGrids(Object* pObj) {
		if (!pObj) {
			ObjectGrids objectGrids;
			return objectGrids;
		}

		return GetObjectGrids(pObj->GetPos(), pObj->GetRadius());
	}

	ObjectGrids Scene::GetObjectGrids(const Vector2D& pos, float fRadius) {
		static Vector2D leftTopPos;
		static Vector2D rightTopPos;
		static Vector2D leftBottomPos;
		static Vector2D rightBottomPos;

		leftTopPos.x = pos.x - fRadius;
		leftTopPos.y = pos.y + fRadius;

		rightTopPos.x = pos.x + fRadius;
		rightTopPos.y = pos.y + fRadius;

		leftBottomPos.x = pos.x - fRadius;
		leftBottomPos.y = pos.y - fRadius;

		rightBottomPos.x = pos.x + fRadius;
		rightBottomPos.y = pos.y - fRadius;

		static ObjectGrids objectGrids;
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

		// 生成内圈食物
		GenFoods(g_slitherConfig.m_fInsideScale, (uint32_t)(g_slitherConfig.m_fInsideGenScale * uNum));

		// 生成中圈食物
		GenFoods(g_slitherConfig.m_fMiddleScale, (uint32_t)(g_slitherConfig.m_fMiddleGenScale * uNum));

		// 生成外圈食物
		GenFoods(1.0f, (uint32_t)((1.0f - g_slitherConfig.m_fInsideGenScale - g_slitherConfig.m_fMiddleGenScale) * uNum));
	}

	// 在指定范围内刷指定食物
	void Scene::GenFoods(float fRangeScale, uint32_t uNum) {
		Vector2D center(m_fSceneLength / 2.0f, m_fSceneLength / 2.0f);
		float fScaleLen = fRangeScale * m_fSceneLength / 2.0f;

		Vector2D pos;
		uint32_t uGridIndex = 0;
		for (uint32_t i = 0; i < uNum; ++i) {
			pos.x = (float)cputil::GenFloatRandom(max(center.x - fScaleLen, 0.0f), min(center.x + fScaleLen, m_fSceneLength));
			pos.y = (float)cputil::GenFloatRandom(max(center.y - fScaleLen, 0.0f), min(center.y + fScaleLen, m_fSceneLength));

			uint32_t uFoodMass = 1;
			uint32_t uMassRand = cputil::GenRandom(1, 10);
			if (uMassRand < 9) {
				uFoodMass = cputil::GenRandom(g_slitherConfig.m_uGenFoodMinValue, g_slitherConfig.m_uGenFoodMinValue + 2);
			}
			else {
				uFoodMass = cputil::GenRandom(std::max(g_slitherConfig.m_uGenFoodMaxValue - 1, 1u), g_slitherConfig.m_uGenFoodMaxValue);
			}
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

	void Scene::GenFoods(Grid& grid, uint32_t uNum, list<Food*>& foodList) {
		Vector2D pos;

		for (uint32_t i = 0; i < uNum; ++i) {
			pos.x = (float)cputil::GenFloatRandom((float)grid.x, (float)(grid.x + m_uGridSize));
			pos.y = (float)cputil::GenFloatRandom((float)grid.y, (float)(grid.y + m_uGridSize));

			uint32_t uFoodMass = cputil::GenRandom(g_slitherConfig.m_uGenFoodMinValue, g_slitherConfig.m_uGenFoodMaxValue);
			Food* pFood = gpFactory->CreateFood(m_uFoodId++, pos, uFoodMass);
			if (!pFood) {
				return;
			}

			foodList.push_back(pFood);
			grid.AddObj(pFood);
			++m_uFoodCount;
		}
	}

	// 生成蛇，参数bRobot 如果为true，说明是系统生成的机器蛇
	Snake* Scene::GenSnake(bool bRobot) {
		Vector2D pos = GenSnakeBornPos();

		float fCurRadius = 0.0f;
		uint32_t uCurNode = 0;
		Snake::GetSlitherParam((float)g_slitherConfig.m_uRobotMass, fCurRadius, uCurNode);

		Snake* pSnake = gpFactory->CreateSnake(this, m_uSnakeId++, pos, uCurNode, fCurRadius, bRobot);
		if (!pSnake) {
			return NULL;
		}
		
		pSnake->SetSkinId(cputil::GenRandom(1, 20));
		
		pSnake->SetTotalMass((float)g_slitherConfig.m_uRobotMass);
		AddSnakeInScene(pSnake);

		return pSnake;
	}

	void Scene::RefreshFoods() {
		GenFoods(g_slitherConfig.m_uRefreshFoodNum);
	}

	void Scene::AddSnakeInScene(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		m_playerToSnakeMap[pSnake->GetPlayerId()] = pSnake;
		TRACELOG("add snake=[" << pSnake << "] into m_playerToSnakeMap playerId=[" << pSnake->GetPlayerId() << "]");

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
	// 参数 bGenFood : 是否需要生成食物
	void Scene::BreakUpSnake(Snake* pSnake, bool bGenFood) {
		if (!pSnake) {
			return;
		}

		list<SnakeBodyNode*>& snakeBodyList = pSnake->GetSnakeBody();
		if (snakeBodyList.size() == 0) {
			return;
		}

		// 分解后生成的食物数量
		int32_t uBreakUpSize = GetBreakUpFoodSize(pSnake);						

		int32_t uEachNodeBreakUp = uBreakUpSize / (snakeBodyList.size() / 2);		// 每个节点生成的食物数量
		int32_t uExtraBreakUp = uBreakUpSize % (snakeBodyList.size() / 2);			// 额外的生成的食物数量

		int32_t uGenFoodsStep = 0;
		if (uExtraBreakUp != 0) {
			uGenFoodsStep = (snakeBodyList.size() / 2) / uExtraBreakUp;
		}

		// 最后生成的食物列表
		list<Food*> foodList;

		// 将蛇头从格子中删除
		ObjectGrids objGrids = GetObjectGrids(pSnake->GetSnakeHead());
		for (int i = 0; i < MAX_GRID_IN; ++i) {
			if (objGrids.grids[i] == -1) {
				continue;
			}
			m_pGrids[objGrids.grids[i]].DelObj(pSnake->GetSnakeHead());
		}
		
		int32_t uGenStep = 2;										// 至少每2个节点生成一次食物
		int32_t uStep = 1;
		list<SnakeBodyNode*>::iterator bodyIt = snakeBodyList.begin();
		list<SnakeBodyNode*>::iterator bodyItEnd = snakeBodyList.end();
		for (; bodyIt != bodyItEnd; bodyIt++) {
			if (bGenFood) {											// 是否可以生成食物

				if (uGenStep-- == 2) {
					if (uEachNodeBreakUp > 0) {							// 每个节点生成的食物
						Vector2D foodPos = (*bodyIt)->GetPos();
						foodPos.x += cputil::GenFloatRandom(-(*bodyIt)->GetRadius(), (*bodyIt)->GetRadius());
						foodPos.y += cputil::GenFloatRandom(-(*bodyIt)->GetRadius(), (*bodyIt)->GetRadius());
						GenFoods(foodPos, uEachNodeBreakUp, g_slitherConfig.m_uDeadFoodValue, foodList);
					}

					if (uExtraBreakUp-- > 0 && uStep++ <= uGenFoodsStep) {
						Vector2D foodPos = (*bodyIt)->GetPos();
						foodPos.x += cputil::GenFloatRandom(-(*bodyIt)->GetRadius(), (*bodyIt)->GetRadius());
						foodPos.y += cputil::GenFloatRandom(-(*bodyIt)->GetRadius(), (*bodyIt)->GetRadius());
						GenFoods(foodPos, 1, g_slitherConfig.m_uDeadFoodValue, foodList);
					}

				}
				else {
					uGenStep = 2;
				}
			}

			if (uStep >= uGenFoodsStep) {
				uStep = 1;
			}
			
			// 从格子中删除掉
			objGrids = GetObjectGrids(*bodyIt);
			for (int i = 0; i < MAX_GRID_IN; ++i) {
				if (objGrids.grids[i] == -1) {
					continue;
				}
				m_pGrids[objGrids.grids[i]].DelObj(*bodyIt);
			}
		}

		if (bGenFood) {
			// 通知生成新的食物了
			BroadcastNewFoods(foodList);
		}
		return;
	}

	int32_t Scene::GetBreakUpFoodSize(Snake* pSnake) {
		if (!pSnake) {
			return 0;
		}

		int32_t uSize = (int32_t)((pSnake->GetTotalMass() - g_slitherConfig.m_uInitSnakeMass) / g_slitherConfig.m_uDeadFoodIncValue + g_slitherConfig.m_uInitDeadFoodSize);
		return uSize;
	}

	void Scene::PlayerEnter(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		// 可能玩家是断线重新进入游戏的，所以需要先清除玩家蛇的视野
		pSnake->ClearViews();

		// 将蛇加入场景
		AddSnakeInScene(pSnake);				

		// 绑定蛇对应的统计信息
		m_pGameRoom->BoundSnakeStatistics(pSnake);

		// 返回给客户端
		slither::EnterGameAck enterGameAck;
		pSnake->SerializeToPB(*enterGameAck.mutable_snake());

		enterGameAck.set_scenewidth(m_uGridSize * m_uHorizontalGridNum);
		enterGameAck.set_gridwidth(m_uGridSize);
		enterGameAck.set_lefttime(m_pGameRoom->GetLeftTime());

		// 计算当前游戏局已经经过了多长时间
		boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff = end - m_startTime;
		enterGameAck.set_servertime((uint32_t)diff.total_milliseconds());

		string strResponse;
		cputil::BuildResponseProto(enterGameAck, strResponse, REQ_ENTER_GAME);
		pSnake->SendMsg(strResponse.c_str(), strResponse.size());
	}

	void Scene::SnakeMove(Snake* pSnake, float fNewAngle, bool bSpeedUp, bool bStopMove) {

		if (!pSnake) {
			return;
		}

		pSnake->SetAngle(fNewAngle);
		if (pSnake->GetTotalMass() > g_slitherConfig.m_fSpeedUpNeedValue) {				// 必须满足加速的最小体重
			pSnake->SetSpeedUp(bSpeedUp);			// 是否加速
		}
		else {
			pSnake->SetSpeedUp(false);
		}

		if (bStopMove)
			pSnake->SetStopMove(bStopMove);
	}

	set<uint32_t> Scene::GetInViewGrids(Snake* pSnake, SnakeViewPoint& viewPoint) {
		set<uint32_t> gridsVec;					// 需要返回的列表
		if (!pSnake) {
			return gridsVec;
		}

		const Vector2D& snakeHeadPos = pSnake->GetSnakeHead()->GetPos();
		float fViewRange = pSnake->GetViewRange();

		static Vector2D leftTop;
		leftTop.x = max(snakeHeadPos.x - fViewRange / 2, 0.0f);
		leftTop.y = min(snakeHeadPos.y + fViewRange / 2, m_fSceneLength);

		//Vector2D rightTop	(min(snakeHeadPos.x + fViewRange / 2, m_fSceneLength),	min(snakeHeadPos.y + fViewRange / 2, m_fSceneLength));

		static Vector2D leftBottom;
		leftBottom.x = max(snakeHeadPos.x - fViewRange / 2, 0.0f);
		leftBottom.y = max(snakeHeadPos.y - fViewRange / 2, 0.0f);

		static Vector2D rightBottom;
		rightBottom.x = min(snakeHeadPos.x + fViewRange / 2, m_fSceneLength);
		rightBottom.y = max(snakeHeadPos.y - fViewRange / 2, 0.0f);

		uint32_t uLeftTopIndex		= GetGridIndex(leftTop);
		//uint32_t uRightTopIndex	= GetGridIndex(rightTop);								// 用不上的参数
		uint32_t uLeftBottomIndex	= GetGridIndex(leftBottom);
		uint32_t uRightBottomIndex	= GetGridIndex(rightBottom);

		viewPoint.uLeftTopIndex = uLeftTopIndex;
		viewPoint.uLeftBottomIndex = uLeftBottomIndex;
		viewPoint.uRightBottomIndex = uRightBottomIndex;

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

		static Vector2D leftTop;
		leftTop.x = max(snakeHeadPos.x - fViewRange / 2, 0.0f);
		leftTop.y = min(snakeHeadPos.y + fViewRange / 2, m_fSceneLength);

		//Vector2D rightTop	(min(snakeHeadPos.x + fViewRange / 2, m_fSceneLength),	min(snakeHeadPos.y + fViewRange / 2, m_fSceneLength));

		static Vector2D leftBottom;	
		leftBottom.x = max(snakeHeadPos.x - fViewRange / 2, 0.0f);
		leftBottom.y = max(snakeHeadPos.y - fViewRange / 2, 0.0f);

		static Vector2D rightBottom;
		rightBottom.x = min(snakeHeadPos.x + fViewRange / 2, m_fSceneLength);
		rightBottom.y = max(snakeHeadPos.y - fViewRange / 2, 0.0f);

		uint32_t uLeftTopIndex		= GetGridIndex(leftTop);
		//uint32_t uRightTopIndex	= GetGridIndex(rightTop);								// 用不上的参数
		uint32_t uLeftBottomIndex	= GetGridIndex(leftBottom);
		uint32_t uRightBottomIndex	= GetGridIndex(rightBottom);

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

		const set<uint32_t>& gridVec = pSnake->GetGridsInView();			// 获取蛇所能看见的格子列表							

		set<uint32_t>::const_iterator gridIt = gridVec.begin();
		set<uint32_t>::const_iterator gridItEnd = gridVec.end();
		for (; gridIt != gridItEnd; gridIt++) {
			SnakeSet& snakeSet = GetSnakeSet(*gridIt);
			snakeSet.erase(pSnake);											// 从格子列表中删除
		}
	}

	void Scene::UpdateSnakeViewGrids(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		// 判断视野范围内格子的leftTop, leftBottom，rightBottom，如果一直，说明根本没有发生过变化
		SnakeViewPoint newViewPoint;
		set<uint32_t> gridVec = GetInViewGrids(pSnake, newViewPoint);		// 获取蛇所能看见的格子列表

		if (pSnake->GetSnakeViewPoint().IsSame(newViewPoint)) {				// 如果视野跟之前完全一样，那么就不做处理
			return;
		}

		// 如果需要更新，则先删除
		DeleteSnakeViewGrids(pSnake);

		// 再更新
		pSnake->SetGridsInView(gridVec);
		pSnake->SetSnakeViewPoint(newViewPoint);

		set<uint32_t>::const_iterator gridIt = gridVec.begin();
		set<uint32_t>::const_iterator gridItEnd = gridVec.end();
		for (; gridIt != gridItEnd; gridIt++) {
			SnakeSet& snakeSet = GetSnakeSet(*gridIt);
			snakeSet.insert(pSnake);										// 在格子的列表中添加
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
		int32_t uCount = 10;
		// 检测一下是否这个格子内有其他蛇，如果有就再随机一下，最多随机10次
		while (uCount-- > 0) {
			pos.x = (float)cputil::GenFloatRandom(10.0f, m_fSceneLength * 0.8f);
			pos.y = (float)cputil::GenFloatRandom(10.0f, m_fSceneLength * 0.8f);
			
			uint32_t uGridIndex = GetGridIndex(pos);
			if (!m_pGrids[uGridIndex].HasSnake()) {				// 如果格子中没有蛇， 否则继续随机
				break;
			}
		}

		return pos;
	}

	void Scene::CheckSpeedUp(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		if (pSnake->GetSpeedUp()) {
			// 计算加速消耗
			pSnake->SpeedUpCost();
			if (pSnake->GetTotalMass() <= g_slitherConfig.m_fStopSpeedUpValue) {			// 低于能加速的值了，停止加速
				pSnake->SetSpeedUp(false);
			}

			// 生成加速的食物
			if (pSnake->GetMoveTick() % (uint32_t)(g_slitherConfig.m_fSpawnInterval * (1000.0 / g_slitherConfig.m_uSimInterval)) == 0) {
				list<Food*> foodList;
				GenFoods(pSnake->GetSnakeTail()->GetPos(), 1, g_slitherConfig.m_uSpeedUpGenFoodValue, foodList);
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
		bool bHasEat = false;													// 是否吃到了食物，如果没有吃到就不发送协议了

		ObjectGrids objGrids = GetObjectGrids(pSnake->GetSnakeHead()->GetPos(), g_slitherConfig.m_fEatFoodRadius);
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

				if (pSnake->GetSnakeHead()->IsContainedInSector(pTmpFood->GetPos())) {
					eatNty.add_eatenfoodlist(pTmpFood->GetFoodId());

					// 蛇吃到了食物
					pSnake->EatFood(pTmpFood);
					bHasEat = true;

					// 删除食物
					foodObjList.erase(foodIt++);
					gpFactory->ReleaseFood(pTmpFood);
					m_uFoodCount--;
					continue;
				}
				foodIt++;
			}
		}

		if (!bHasEat) {
			return;
		}
		
		eatNty.set_newmass((uint32_t)pSnake->GetTotalMass());

		// 广播
		SnakeSet& snakeSet = GetSnakeSet(uGridIndex);
		BroadcastEat(snakeSet, eatNty);

		return;
	}

	void Scene::CheckCollide(Snake* pSnake, uint16_t uGridIndex) {
		if (!pSnake || pSnake->GetStatus() == OBJ_DESTROY) {
			return;
		}

		// 判断和其他蛇的碰撞
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
						pSnake->IncDeadTimes();

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

	void Scene::CheckCollideWithEgde(Snake* pSnake, uint16_t uGridIndex) {
		if (!pSnake) {
			return;
		}

		if (pSnake->GetStatus() == OBJ_DESTROY) {
			return;
		}

		bool bCollide = false;
		const Vector2D& headPos = pSnake->GetSnakeHead()->GetPos();
		float fCollideLen = g_slitherConfig.m_fCollideProportion * pSnake->GetSnakeHead()->GetRadius();
		if (headPos.x + fCollideLen > m_fSceneLength || headPos.y + fCollideLen > m_fSceneLength) {
			bCollide = true;
		}
		if (headPos.x - fCollideLen < 0 || headPos.y - fCollideLen < 0) {
			bCollide = true;
		}

		if (bCollide) {
			pSnake->SetStatus(OBJ_DESTROY);													// 将状态设置为“销毁”
			pSnake->IncDeadTimes();

			// 判断和其他蛇的碰撞
			SnakeSet& snakeSet = GetSnakeSet(uGridIndex);									// 能看见此格子的蛇的列表

			// 向周围广播碰撞
			BroadcastCollide(snakeSet, pSnake);

			// 将蛇分解掉, 但不生成食物
			BreakUpSnake(pSnake, false);
			DEBUGLOG("snake id=" << pSnake->GetSnakeId() << " addr=" << pSnake << " collide with edge");
		}

		return;
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
		snakeCollide.mutable_collidesnake()->set_status(slither::DESTROY);

		string strResponse;
		cputil::BuildResponseProto(snakeCollide, strResponse, BROADCAST_COLLIDE);
		
		// 挨个广播
		set<Snake*>::iterator snakeIt = broadcastList.begin();
		set<Snake*>::iterator snakeItEnd = broadcastList.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pTmpSnake = *snakeIt;
			if (!pTmpSnake) {
				continue;
			}

			pTmpSnake->SendMsg(strResponse.c_str(), strResponse.size());
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

	// 广播某个格子生成的食物信息
	void Scene::BroadcastNewFoods(uint32_t uGridId, const list<Food*>& newFoodList) {
		slither::BroadcastNewFood newFoods;

		list<Food*>::const_iterator foodIt = newFoodList.begin();
		list<Food*>::const_iterator foodItEnd = newFoodList.end();
		for (; foodIt != foodItEnd; foodIt++) {
			slither::PBFood* pPBFood = newFoods.add_foodlist();
			(*foodIt)->SerializeToPB(*pPBFood);
		}
		string strResponse;
		cputil::BuildResponseProto(newFoods, strResponse, BROADCAST_NEWFOODS);


		SnakeSet& snakeSet = GetSnakeSet(uGridId);
		if (snakeSet.empty()) {
			return;
		}

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

	bool Greater(const Snake* pSnake1, const Snake* pSnake2) {
		if (pSnake1->GetTotalMass() > pSnake2->GetTotalMass()) {
			return true;
		}

		return false;
	}

	void Scene::BroadcastRankingList(bool bGameEnd) {

		vector<Snake*> snakeVec;

		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.begin();
		map<uint32_t, Snake*>::iterator snakeItEnd = m_snakeMap.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pSnake = snakeIt->second; 
			if (!pSnake || pSnake->GetStatus() == OBJ_DESTROY || pSnake->IsRobot()) {
				continue;
			}

			snakeVec.push_back(pSnake);
		}

		std::sort(snakeVec.begin(), snakeVec.end(), Greater);
		
		// 通知给每个玩家
		uint32_t uRank = 1;										// 玩家排名
		slither::BroadcastRankingList rankingList;
		uint32_t uRankCount = std::min((size_t)10, snakeVec.size());
		for (uint32_t i = 0; i < snakeVec.size(); ++i) {
			Snake* pSnake = snakeVec[i];
			if (!pSnake || pSnake->IsRobot()) {
				continue;
			}

			pSnake->SetRank(uRank++);
			if (i >= uRankCount) {								// 超出了需要的排名，就不再PB序列化了
				continue;
			}

			slither::PBPlayerRank* pPBPlayerRank = rankingList.add_rankinglist();
			pPBPlayerRank->set_nickname(pSnake->GetNickName());
			pPBPlayerRank->set_mass((uint32_t)pSnake->GetTotalMass());
			pPBPlayerRank->set_skinid(pSnake->GetSkinId());
			pPBPlayerRank->set_killnum(pSnake->GetStatistics().uKillSnakeTimes);
			if (bGameEnd) {
				pPBPlayerRank->set_gender(pSnake->GetStatistics().gender);
				pPBPlayerRank->set_playername(pSnake->GetStatistics().strPlayerName);
			}
		}

		m_uGameEndLastRank = uRank;

		vector<Snake*>::iterator rankIt = snakeVec.begin();
		vector<Snake*>::iterator rankItEnd = snakeVec.end();
		for (; rankIt != rankItEnd; rankIt++) {
			Snake* pSnake = *rankIt;
			if (!pSnake) {
				continue;
			}

			if (pSnake->GetRank() == 0) {						// 如果排名是0，按最后一名算
				pSnake->SetRank(m_uGameEndLastRank + 1);
			}
			rankingList.set_selfrank(pSnake->GetRank());

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

		set<uint32_t> gridVec;
		GetInViewGrids(pSnake, gridVec);

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
				if (pSnake->HasInView(pTmpSnake) || pTmpSnake == pSnake) {
					// 序列化蛇头
					pTmpSnake->SerializeToPB(*pPBSnake, true);
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

		// 计算当前游戏局已经经过了多长时间
		boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff = end - m_startTime;
		aroundNty.set_servertime((uint32_t)diff.total_milliseconds());

		// 发送给客户端
		cputil::BuildResponseProto(aroundNty, strResponse, slither::NOTIFY_AROUND);
		pSnake->SendMsg(strResponse.c_str(), strResponse.size());
	}

	void Scene::InitSceneFoods() {

		// 创建1w个食物
		GenFoods(g_slitherConfig.m_uInitFoodNum);

		// 创建N条蛇
		for (uint32_t i = 0; i < g_slitherConfig.m_uRobotNum; ++i) {
			GenSnake(true);
		}
	}

	void Scene::Finish() {
		// 广播排名
		BroadcastRankingList(true);
	}
}
