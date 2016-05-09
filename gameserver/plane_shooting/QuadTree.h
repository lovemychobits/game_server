#pragma once

#include <list>
#include <stdint.h>
#include "Object.h"
using namespace plane_shooting;
using namespace std;

/*
	四叉树，管理场景中的飞机和障碍物，方便进行碰撞检测，以及对玩家的飞机的广播
*/
class QuadTree
{
#define NODE_SIZE 4
#define NODE_MAX_BOJS 5

public:
	QuadTree(plane_shooting::Rectangle bounds);
	~QuadTree();

public:
	void insert(Object* pObj);
	void remove(Object* pObj);
	void retrieve(const plane_shooting::Rectangle& rect, list<Object*>& objList);

private:
	void Split();
	int16_t GetIndex(const plane_shooting::Rectangle& rect);
	

private:
	list<Object*> m_objects;
	QuadTree* nodes[NODE_SIZE];
	plane_shooting::Rectangle m_bounds;
};