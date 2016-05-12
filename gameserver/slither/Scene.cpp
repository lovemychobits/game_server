#include "Scene.h"
#include "Snake.h"
#include "Food.h"
#include "../../utils/Utility.h"
#include "PlayerMng.h"
#include "../../protocol/slither_client.pb.h"
#include "Factory.h"
#include <iostream>
using namespace std;

namespace slither {

#define GridSize 20
	Scene::Scene() : m_uSnakeId(1), m_uFoodId(1), m_uFoodCount(0) {
	}

	Scene::~Scene() {

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

		DWORD dwStart = ::GetTickCount();

		uint32_t uGridIndex = 0;														// 蛇头所在的格子

		// 模拟所有蛇的运动
		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.begin();
		for (; snakeIt != m_snakeMap.end();) {
			Snake* pSnake = snakeIt->second;
			if (!pSnake) {
				continue;
			}

			if (pSnake->GetStatus() == ObjectStatus::OBJ_DESTROY) {						// 如果蛇需要销毁
				// 释放蛇
				gpFactory->ReleaseSnake(pSnake);
				m_snakeMap.erase(snakeIt++);
				continue;
			}

			// 先让蛇运动
			pSnake->Move();
			uGridIndex = GetGridIndex(pSnake->GetSnakeHead()->GetPos());				// 获取运动之后头所在的格子，限制每次运动不会超过一个蛇头的直径

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
					pSnake->SetStatus(ObjectStatus::OBJ_DESTROY);						// 将状态设置为“销毁”
					BreakUpSnake(pSnake);												// 将蛇分解掉
				}
			}

			// 判定吃食物
			list<Object*>& foodObjList = m_pGrids[uGridIndex].GetFoodList();
			list<Object*>::iterator foodIt = foodObjList.begin();
			for (; foodIt != foodObjList.end(); ) {
				Food* pTmpFood = (Food*)*foodIt;
				if (!pTmpFood) {
					continue;
				}
				
				if (pTmpFood->GetStatus() == ObjectStatus::OBJ_DESTROY) {
					// 删除食物
					foodObjList.erase(foodIt++);
					gpFactory->ReleaseFood(pTmpFood);
					continue;
				}

				if (pSnake->GetSnakeHead()->IsContained(pTmpFood->GetPos())) {
					// 如果食物被吃，也和蛇销毁一样的逻辑，在此时钟周期将其状态设置为销毁，在下一个时钟周期才真正的销毁
					pTmpFood->SetStatus(ObjectStatus::OBJ_DESTROY);
					pTmpFood->SetEatenSnakeId(pSnake->GetSnakeId());
				}
				foodIt++;
			}

			if (m_uFoodCount < 7000) {
				GenFoods(3000);
			}

			// 通知这条蛇周围的情况
			Notify(pSnake);

			snakeIt++;
		}

		DWORD dwEnd = ::GetTickCount();
		cout << "time cost=[" << dwEnd - dwStart << "]" << endl;
	}

	uint32_t Scene::GetGridIndex(const Vector2D& pos) {
		uint32_t uHorizontalGridNum = uint32_t(pos.x / GridSize);
		uint32_t uVerticalGridNum = uint32_t(pos.y / GridSize);

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
		Vector2D leftTopPos(objPos.x - pObj->GetRadius(), objPos.y + pObj->GetRadius());
		Vector2D rightTopPos(objPos.x + pObj->GetRadius(), objPos.y + pObj->GetRadius());
		Vector2D leftBottomPos(objPos.x - pObj->GetRadius(), objPos.y - pObj->GetRadius());
		Vector2D rightBottomPos(objPos.x + pObj->GetRadius(), objPos.y - pObj->GetRadius());

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

	void Scene::GenFoods(const Vector2D& pos, uint32_t uNum, uint32_t uValue) {
		for (uint32_t i = 0; i < uNum; ++i) {
			Food* pFood = gpFactory->CreateFood(m_uFoodId++, pos, uValue);
			if (!pFood) {
				return;
			}
			uint32_t uGridIndex = GetGridIndex(pos);
			m_pGrids[uGridIndex].AddObj(pFood);
			++m_uFoodCount;
		}
	}

	// 生成蛇，参数bRobot 如果为true，说明是系统生成的机器蛇
	Snake* Scene::GenSnake(bool bRobot) {
		Vector2D pos;
		uint32_t uGridIndex = 0;

		pos.x = (float)cputil::GenFloatRandom(100.0f, 250.0f);
		pos.y = (float)cputil::GenFloatRandom(100.0f, 250.0f);

		Snake* pSnake = gpFactory->CreateSnake(this, m_uSnakeId++, pos, 30, bRobot);
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

		vector<SnakeBodyNode*>& snakeBodeVec = pSnake->GetSnakeBody();
		vector<SnakeBodyNode*>::iterator bodyIt = snakeBodeVec.begin();
		vector<SnakeBodyNode*>::iterator bodyItEnd = snakeBodeVec.end();
		for (; bodyIt != bodyItEnd; bodyIt++) {
			GenFoods((*bodyIt)->GetPos(), 1, 1);					// 后面根据body的半径决定生成的数量和值
			objGrids = GetObjectGrids(*bodyIt);
			for (int i = 0; i < MAX_GRID_IN; ++i) {
				if (objGrids.grids[i] == -1) {
					continue;
				}
				m_pGrids[objGrids.grids[i]].DelObj(*bodyIt);
			}
		}
	}

	void Scene::PlayerEnter(IConnection* pConn, uint32_t uUserId) {
		Snake* pSnake = GenSnake(false);
		if (!pSnake) {
			ERRORLOG("GenSnake failed.");
			return;
		}

		pSnake->SetConnection(pConn);
		PlayerMng::GetInstance()->SetPlayerConn(pSnake, pConn);

		slither::EnterGameAck enterGameAck;
		pSnake->SerializeToPB(*enterGameAck.mutable_snake());

		string strResponse;
		cputil::BuildResponseProto(enterGameAck, strResponse, slither::ClientProtocol::REQ_ENTER_GAME);
		pSnake->SendMsg(strResponse.c_str(), strResponse.size());
	}

	void Scene::SnakeMove(Snake* pSnake, uint16_t uNewAngle, bool bSpeedUp) {
		if (!pSnake) {
			return;
		}

		pSnake->SetAngle(uNewAngle);
		pSnake->SetSpeedUp(bSpeedUp);			// 是否加速
	}

	vector<uint16_t> Scene::GetInViewGrids(uint16_t uGridIndex, uint16_t uSnakeViewRange) {
		vector<uint16_t> gridsVec;

		for (int16_t i = 0; i < 3; ++i) {		// 横向3排
			for (int16_t j = 0; j < 3; ++j) {	// 纵向3列
				int16_t uTmpIndex = (int16_t)uGridIndex + (int16_t)m_uHorizontalGridNum * (i - 1) + (j - 1);
				if (uTmpIndex < 0 || uTmpIndex >= m_uHorizontalGridNum * m_uVerticalGridNum) {
					continue;
				}
				gridsVec.push_back(uTmpIndex);
			}
		}

		return gridsVec;
	}

	// 向这条蛇通知周围的情况
	void Scene::Notify(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		uint16_t uGridIndex = GetGridIndex(pSnake->GetSnakeHead()->GetPos());
		vector<uint16_t> gridVec = GetInViewGrids(uGridIndex, pSnake->GetViewRange());

		string strResponse;
		slither::NotifyObjs objsNty;

		for (uint16_t i = 0; i < gridVec.size(); ++i) {
			uint16_t uIndex = gridVec[i];
			list<Object*>& snakeHeadList = m_pGrids[uIndex].GetSnakeHeadList();

			// 通知这条蛇，周围其他蛇目前的情况 
			list<Object*>::iterator snakeIt = snakeHeadList.begin();
			list<Object*>::iterator snakeItEnd = snakeHeadList.end();
			for (; snakeIt != snakeItEnd; snakeIt++) {
				slither::PBSnake* pPBSnake = objsNty.add_snakelist();

				SnakeBodyNode* pSnakeHead = (SnakeBodyNode*)*snakeIt;
				if (!pSnakeHead) {
					continue;
				}
				Snake* pTmpSnake = pSnakeHead->GetOwner();
				if (!pTmpSnake) {
					continue;
				}

				// 如果已经在视野中了，那么只需要知道蛇头的信息就可以了
				if (pSnake->IsInView(pTmpSnake)) {
					// 序列化蛇头
					pTmpSnake->SerializeToPB(*pPBSnake, true);
				}
				else {
					pTmpSnake->SerializeToPB(*pPBSnake);
					pSnake->AddInView(pTmpSnake);
				}
			}

			// 通知这条蛇，周围食物的情况
			if (pSnake->IsInView(&m_pGrids[uIndex])) {
				continue;
			}
			else {
				list<Object*>& foodList = m_pGrids[uIndex].GetFoodList();
				list<Object*>::iterator foodIt = foodList.begin();
				list<Object*>::iterator foodItEnd = foodList.end();
				for (; foodIt != foodItEnd; foodIt++) {
					slither::PBFood* pPBFood = objsNty.add_foodlist();

					Food* pFood = (Food*)*foodIt;
					pFood->SerializeToPB(*pPBFood);
				}
				pSnake->AddInView(&m_pGrids[uIndex]);
			}
		}

		// 发送给客户端
		cputil::BuildResponseProto(objsNty, strResponse, slither::ClientProtocol::NTY_OBJS);
		pSnake->SendMsg(strResponse.c_str(), strResponse.size());
	}
	
	void Scene::TestScene() {
		Vector2D pos;
		uint32_t uGridIndex = 0;

		// 创建1w个食物
		GenFoods(50000);

		// 创建N条蛇
		for (int i = 0; i < 300; ++i) {
			GenSnake(true);
		}
	}
}