#include "SceneMng2.h"

int main() {
	plane_shooting::SceneMng::GetInstance()->TestQuadTree();
	plane_shooting::SceneMng::GetInstance()->OnTimer();
	return 0;
}