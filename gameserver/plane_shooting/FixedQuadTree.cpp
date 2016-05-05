#include "FixedQuadTree.h"

FixedQuadTree::FixedQuadTree(plane_shooting::Rectangle bounds, uint32_t uMaxLevel, int32_t uLevel) {
	m_bounds = bounds;

	// ��ʼ���ӽڵ�
	for (int i = 0; i < NODE_SIZE; ++i) {
		nodes[i] = NULL;
	}

	if (uLevel == uMaxLevel) {				// �Ѿ�������
		return;
	}

	// �����ӽڵ�
	float fSubWidth = m_bounds.fWidth / 2;
	float fSubHeight = m_bounds.fHeight / 2;
	float x = m_bounds.x;
	float y = m_bounds.y;

	/*
	width
	-----------------
	|	0	|	1	|
	|-----(x,y)-----| height
	|	2  	|	3	|
	-----------------
	*/

	nodes[0] = new FixedQuadTree(plane_shooting::Rectangle(x - fSubWidth / 2, y + fSubHeight / 2, fSubWidth, fSubHeight), uMaxLevel, uLevel + 1);
	nodes[1] = new FixedQuadTree(plane_shooting::Rectangle(x + fSubWidth / 2, y + fSubHeight / 2, fSubWidth, fSubHeight), uMaxLevel, uLevel + 1);
	nodes[2] = new FixedQuadTree(plane_shooting::Rectangle(x - fSubWidth / 2, y - fSubHeight / 2, fSubWidth, fSubHeight), uMaxLevel, uLevel + 1);
	nodes[3] = new FixedQuadTree(plane_shooting::Rectangle(x + fSubWidth / 2, y - fSubHeight / 2, fSubWidth, fSubHeight), uMaxLevel, uLevel + 1);

	return;
}

FixedQuadTree::~FixedQuadTree() {
	Clear(this);
}

// ��ȡһ���������ڵ��Ĳ���
int16_t FixedQuadTree::GetIndex(const Rectangle& rect) {
	int16_t index = -1;

	bool topQuadrant = (rect.y + rect.fHeight / 2 < m_bounds.y + m_bounds.fHeight / 2) && (rect.y - rect.fHeight / 2 > m_bounds.y);
	bool bottomQuadrant = (rect.y + rect.fHeight / 2 < m_bounds.y) && (rect.y - rect.fHeight / 2 > m_bounds.y - m_bounds.fHeight / 2);

	// left quadrant
	if ((rect.x - rect.fWidth / 2 > m_bounds.x - m_bounds.fWidth / 2) && (rect.x + rect.fWidth / 2 < m_bounds.x)) {
		if (topQuadrant) {
			index = 0;
		}
		if (bottomQuadrant) {
			index = 2;
		}
	}

	// right quadrant
	if ((rect.x - rect.fWidth / 2 > m_bounds.x) && (rect.x + rect.fWidth / 2 < m_bounds.x + m_bounds.fWidth / 2)) {
		if (topQuadrant) {
			index = 1;
		}
		if (bottomQuadrant) {
			index = 3;
		}
	}

	return index;
}

// �Ƿ�ΪҶ�ӽڵ�
bool FixedQuadTree::IsLeafNode()
{
	if (nodes[0] != NULL) {
		return false;
	}

	return true;
}

void FixedQuadTree::insert(Object* pObj) {
	if (!pObj) {
		return;
	}
	const plane_shooting::Rectangle& rect = pObj->GetRect();
	int16_t index = GetIndex(rect);
	if (index != -1 && nodes[0] != NULL) {
		nodes[index]->insert(pObj);
		return;
	}

	// ��������Ӷ�Ӧ������
	m_objects.push_back(pObj);
	ObjectInNodeMng::GetInstance()->Insert(pObj, this);
}

// ����������ɾ������
void FixedQuadTree::remove(Object* pObj) {
	if (!pObj) {
		return;
	}

	const plane_shooting::Rectangle& rect = pObj->GetRect();
	int16_t index = GetIndex(rect);
	if (index != -1 && nodes[0] != NULL) {					// ȥ��������
		nodes[index]->remove(pObj);
		return;
	}

	// ɾ������
	list<Object*>::iterator objIt = m_objects.begin();
	list<Object*>::iterator objItEnd = m_objects.end();
	for (; objIt != objItEnd; objIt++) {
		if (pObj == *objIt) {
			m_objects.erase(objIt);
			ObjectInNodeMng::GetInstance()->Remove(pObj);
			return;
		}
	}

	return;
}


void FixedQuadTree::retrieve(const plane_shooting::Rectangle& rect, list<Object*>& objList) {
	int16_t index = GetIndex(rect);
	if (index != -1 && nodes[0] != NULL) {
		nodes[index]->retrieve(rect, objList);
		return;
	}

	list<Object*>::iterator objIt = m_objects.begin();
	list<Object*>::iterator objItEnd = m_objects.end();
	for (; objIt != objItEnd; objIt++) {
		if ((*objIt)->GetType() == Plane_Type) {
			objList.push_back(*objIt);
		}
	}

	return;
}

void FixedQuadTree::Clear(FixedQuadTree* pQuadTree) 
{
	for (int i = 0; i < NODE_SIZE; ++i)
	{
		if (nodes[i] == NULL) {
			continue;
		}

		nodes[i]->Clear(nodes[i]);
	}

	m_objects.clear();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ObjectInNodeMng::SetHeadNode(FixedQuadTree* pQuadTree)
{
	m_pHeadNode = pQuadTree;
}

FixedQuadTree* ObjectInNodeMng::GetQuadTree(Object* pObject) 
{
	map<Object*, FixedQuadTree*>::iterator objIt = m_objToTreeMap.find(pObject);
	if (objIt == m_objToTreeMap.end()) {
		return NULL;
	}

	return objIt->second;
}

void ObjectInNodeMng::Insert(Object* pObject, FixedQuadTree* pQuadTree) 
{
	m_objToTreeMap.insert(make_pair(pObject, pQuadTree));
}

void ObjectInNodeMng::Remove(Object* pObject)
{
	m_objToTreeMap.erase(pObject);
}

void ObjectInNodeMng::CheckUpdate(Object* pObject, const Vector2D& oldPos)
{
	FixedQuadTree* pQuadTree = GetQuadTree(pObject);
	if (!pQuadTree) {
		return;
	}

	int16_t index = pQuadTree->GetIndex(pObject->GetRect());
	if (index != -1 && pQuadTree->IsLeafNode()) {									// �ڵ�ǰ����, ��ô˵������Ҫ�޸��������ڵ�����
		return;
	}
	
	Vector2D newPos = pObject->GetPos();

	// �ȴ�ԭ��������ɾ��
	pObject->SetPos(oldPos);
	pQuadTree->remove(pObject);

	// Ȼ���ٲ���
	pObject->SetPos(newPos);
	m_pHeadNode->insert(pObject);

	return;
}