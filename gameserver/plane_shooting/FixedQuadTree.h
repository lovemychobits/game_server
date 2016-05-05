#pragma once

#include <list>
#include <map>
#include <stdint.h>
#include "Object.h"
using namespace plane_shooting;
using namespace std;

/*
四叉树，管理场景中的飞机和障碍物，方便进行碰撞检测，以及对玩家的飞机的广播
*/
class FixedQuadTree
{
#define NODE_SIZE 4
#define NODE_MAX_BOJS 5

public:
	FixedQuadTree(plane_shooting::Rectangle bounds, uint32_t uMaxLevel, int32_t uLevel=1);
	~FixedQuadTree();

public:
	void insert(Object* pObj);
	void remove(Object* pObj);
	void retrieve(const plane_shooting::Rectangle& rect, list<Object*>& objList);
	int16_t GetIndex(const plane_shooting::Rectangle& rect);
	bool IsLeafNode();

private:
	void Clear(FixedQuadTree* pQuadTree);

private:
	list<Object*> m_objects;
	FixedQuadTree* nodes[NODE_SIZE];
	plane_shooting::Rectangle m_bounds;
};

class ObjectInNodeMng
{
public:
	static ObjectInNodeMng* GetInstance() {
		static ObjectInNodeMng instance;
		return &instance;
	}

	void SetHeadNode(FixedQuadTree* pQuadTree);
	FixedQuadTree* GetQuadTree(Object* pObject);
	void Insert(Object* pObject, FixedQuadTree* pQuadTree);
	void Remove(Object* pObject);
	void CheckUpdate(Object* pObject, const Vector2D& oldPos);

private:
	map<Object*, FixedQuadTree*> m_objToTreeMap;
	FixedQuadTree* m_pHeadNode;						// 四叉树的首节点
};