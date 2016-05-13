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

	// ��ʼ��������������ͼ�Ĵ�Сȥ���������
	bool Scene::Init(uint32_t uSceneHeight, uint32_t uSceneWidth) {
		uint32_t x = 0;
		uint32_t y = 0;

		m_uHorizontalGridNum = uSceneWidth / GridSize;						// ˮƽ����ĸ�����
		m_uVerticalGridNum = uSceneHeight / GridSize;						// ��ֱ����ĸ�����
		m_pGrids = new Grid[m_uHorizontalGridNum * m_uVerticalGridNum];

		uint32_t index = 0;
		// �������
		for (uint32_t y = 0; y < uSceneHeight; y += GridSize) {
			for (uint32_t x = 0; x < uSceneWidth; x += GridSize) {
				Grid& grid = m_pGrids[index++];
				grid.x = x;													// ���½�������Ϊ���ӵ�����
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

		uint32_t uGridIndex = 0;														// ��ͷ���ڵĸ���

		// ģ�������ߵ��˶�
		map<uint32_t, Snake*>::iterator snakeIt = m_snakeMap.begin();
		for (; snakeIt != m_snakeMap.end();) {
			Snake* pSnake = snakeIt->second;
			if (!pSnake) {
				continue;
			}
			bool bRobot = pSnake->IsRobot();

			// ���������ط�ɾ�������򲻺ÿ���
			if (pSnake->GetStatus() == ObjectStatus::OBJ_DESTROY) {						// �������Ҫ����
				// �ͷ���
				gpFactory->ReleaseSnake(pSnake);
				m_snakeMap.erase(snakeIt++);
				continue;
			}

			// �������˶�
			pSnake->Move();
			uGridIndex = GetGridIndex(pSnake->GetSnakeHead()->GetPos());				// ��ȡ�˶�֮��ͷ���ڵĸ��ӣ�����ÿ���˶����ᳬ��һ����ͷ��ֱ��

			// �����֪ͨ��Χ�ĸ���ʳ�������ֻ֪ͨ��Ҫ�ģ�
			NotifyGrids(pSnake);						

			// �ж��߳Ե�ʳ������
			CheckEatFood(pSnake, uGridIndex);

			// �ж��ߵ���ײ���
			CheckCollide(pSnake, uGridIndex);

			// TODO ���ݲ�����������ʳ��
			//if (m_uFoodCount < 7000) {
			//	GenFoods(3000);
			//}

			snakeIt++;
		}

		DWORD dwEnd = ::GetTickCount();
		cout << "time cost=[" << dwEnd - dwStart << "], snake count=[" << m_snakeMap.size() << "]" << endl;
	}

	uint32_t Scene::GetGridIndex(const Vector2D& pos) {
		uint32_t uHorizontalGridNum = uint32_t(pos.x / GridSize);
		uint32_t uVerticalGridNum = uint32_t(pos.y / GridSize);

		uint32_t uIndex = uVerticalGridNum * m_uHorizontalGridNum + uHorizontalGridNum;
		if (uIndex >= m_uHorizontalGridNum * m_uVerticalGridNum) {
			// TODO 
			// ��Ҫ�����ж���Ե��Ŀǰֻ�Ǽ򵥵�����������
			uIndex = m_uHorizontalGridNum * m_uVerticalGridNum - 1;
		}
		return uIndex;
	}

	// ��ȡһ���������ڵ����и��ӣ���Ϊһ����������ڼ������ӵı�Ե�������������ڶ������
	ObjectGrids Scene::GetObjectGrids(Object* pObj) {
		const Vector2D& objPos = pObj->GetPos();
		Vector2D leftTopPos(objPos.x - pObj->GetRadius(), objPos.y + pObj->GetRadius());
		Vector2D rightTopPos(objPos.x + pObj->GetRadius(), objPos.y + pObj->GetRadius());
		Vector2D leftBottomPos(objPos.x - pObj->GetRadius(), objPos.y - pObj->GetRadius());
		Vector2D rightBottomPos(objPos.x + pObj->GetRadius(), objPos.y - pObj->GetRadius());

		ObjectGrids objectGrids;
		objectGrids.grids[0] = GetGridIndex(leftTopPos);
		objectGrids.grids[1] = GetGridIndex(rightTopPos);
		if (objectGrids.grids[1] == objectGrids.grids[0]) {				// �ж��Ƿ�ͬһ������
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

	// �����ߣ�����bRobot ���Ϊtrue��˵����ϵͳ���ɵĻ�����
	Snake* Scene::GenSnake(bool bRobot) {
		Vector2D pos;
		uint32_t uGridIndex = 0;

		pos.x = (float)cputil::GenFloatRandom(50.0f, 850.0f);
		pos.y = (float)cputil::GenFloatRandom(50.0f, 850.0f);

		Snake* pSnake = gpFactory->CreateSnake(this, m_uSnakeId++, pos, 30, bRobot);
		if (!pSnake) {
			return NULL;
		}
		m_snakeMap.insert(make_pair(pSnake->GetSnakeId(), pSnake));

		// �������߸������ּ���Grid��
		// ����ͷ���������
		ObjectGrids objectGrids = GetObjectGrids(pSnake->GetSnakeHead());
		for (uint32_t i = 0; i < MAX_GRID_IN; ++i) {
			if (objectGrids.grids[i] == -1) {
				continue;
			}
			m_pGrids[objectGrids.grids[i]].AddObj(pSnake->GetSnakeHead());
		}

		// ��������������
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

	// �������󣬽��߷ֽ��
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
			GenFoods((*bodyIt)->GetPos(), 1, 1);					// �������body�İ뾶�������ɵ�������ֵ
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
		pSnake->SetSpeedUp(bSpeedUp);			// �Ƿ����
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
				gridsVec.push_back(j + (i - uLeftBottomIndex));
			}
		}

		return gridsVec;
	}

	// ��ȡ��Ҫ֪ͨ���б�
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

				// ����Ұ��
				if (pTmpSnake->IsInView(pSnake)) {
					broadcastList.push_back(pTmpSnake);
				}
			}
		}

		broadcastList.push_back(pSnake);						// �����Լ�
	}
	
	void Scene::CheckEatFood(Snake* pSnake, uint16_t uGridIndex) {
		if (!pSnake) {
			return;
		}

		if (uGridIndex >= m_uHorizontalGridNum * m_uVerticalGridNum) {			// ���������ˣ�˵��������
			ERRORLOG("error grid index=[" << uGridIndex << "]");
			return;
		}

		slither::BroadcastMove moveNty;
		pSnake->SerializeToPB(*moveNty.mutable_snakeinfo());

		// �ж���ʳ��
		list<Object*>& foodObjList = m_pGrids[uGridIndex].GetFoodList();
		list<Object*>::iterator foodIt = foodObjList.begin();
		for (; foodIt != foodObjList.end();) {
			Food* pTmpFood = (Food*)*foodIt;
			if (!pTmpFood) {
				foodIt++;
				continue;
			}

			if (pSnake->GetSnakeHead()->IsContained(pTmpFood->GetPos())) {
				moveNty.add_eatenfoodlist(pTmpFood->GetFoodId());

				// ɾ��ʳ��
				foodObjList.erase(foodIt++);
				gpFactory->ReleaseFood(pTmpFood);

				continue;
			}
			foodIt++;
		}

		// broadcast
		list<Snake*> broadcastList;
		GetBroadcastList(pSnake, broadcastList);
		BroadcastMove(pSnake, broadcastList, moveNty);

		return;
	}

	void Scene::CheckCollide(Snake* pSnake, uint16_t uGridIndex) {
		// Ȼ���ж���ͷ�������ߵ���ײ
		list<Object*>& collidionObjList = m_pGrids[uGridIndex].GetSnakeBodyList();
		list<Object*>::iterator collideIt = collidionObjList.begin();
		list<Object*>::iterator collideItEnd = collidionObjList.end();
		for (; collideIt != collideItEnd; collideIt++) {
			SnakeBodyNode* pTmpObj = (SnakeBodyNode*)*collideIt;
			if (!pTmpObj) {
				continue;
			}
			if (pTmpObj->GetOwner() == pSnake) {									// �Լ����岿��
				continue;
			}
			// �����������ײ����ô���������ٵ���Ȼ������ֽ��ʳ��
			if (pSnake->GetSnakeHead()->IsCollide(pTmpObj)) {
				pSnake->SetStatus(ObjectStatus::OBJ_DESTROY);						// ��״̬����Ϊ�����١�

				// broadcast
				list<Snake*> broadcastList;
				GetBroadcastList(pSnake, broadcastList);

				// ����Χ�㲥��ײ
				BroadcastCollide(broadcastList, pSnake);

				// ���߷ֽ��
				BreakUpSnake(pSnake);
			}
		}
	}

	void Scene::BroadcastMove(Snake* pSnake, list<Snake*>& broadcastList, slither::BroadcastMove& snakeMove) {

		string strResponse;
		cputil::BuildResponseProto(snakeMove, strResponse, slither::ClientProtocol::BROADCAST_MOVE);
		uint32_t uMsgSize = strResponse.size();

		list<Snake*>::iterator snakeIt = broadcastList.begin();
		list<Snake*>::iterator snakeItEnd = broadcastList.end();
		for (; snakeIt != snakeItEnd; snakeIt++) {
			Snake* pTmpSnake = *snakeIt;
			if (!pTmpSnake) {
				continue;
			}

			// ����Ѿ��ڶԷ�����Ұ�����ˣ���ôֻ��Ҫ�򵥵�֪ͨ��ͷ����Ϣ����
			if (pSnake->HasInView(pTmpSnake)) {
				pTmpSnake->SendMsg(strResponse.c_str(), strResponse.size());
			}
			else {
				pTmpSnake->SendMsg(strResponse.c_str(), strResponse.size());
				pSnake->AddInView(pTmpSnake);
			}	
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
		cputil::BuildResponseProto(newFoods, strResponse, slither::ClientProtocol::BROADCAST_NEWFOODS);

		// �����㲥
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

	void Scene::BroadcastCollide(list<Snake*>& broadcastList, Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		slither::BroadcastCollide snakeCollide;
		pSnake->SerializeToPB(*snakeCollide.mutable_collidesnake());

		string strResponse;
		cputil::BuildResponseProto(snakeCollide, strResponse, slither::ClientProtocol::BROADCAST_COLLIDE);
		
		// �����㲥
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
		cputil::BuildResponseProto(gridsNtfy, strResponse, slither::ClientProtocol::NOTIFY_GRIDS);
		pSnake->SendMsg(strResponse.c_str(), strResponse.size());
	}

	void Scene::TestScene() {
		Vector2D pos;
		uint32_t uGridIndex = 0;

		// ����1w��ʳ��
		GenFoods(50000);

		// ����N����
		for (int i = 0; i < 1000; ++i) {
			GenSnake(true);
		}
	}
}