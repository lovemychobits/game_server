//#pragma once
//
//#include "../header.h"
//#include "Map.h"
//#include "../../recastnavigation-master/Recast/Include/Recast.h"
//#include "../../recastnavigation-master/Detour/Include/DetourNavMesh.h"
//#include "../../recastnavigation-master/Detour/Include/DetourNavMeshQuery.h"
//#include "../../recastnavigation-master/Detour/Include/DetourCommon.h"
//
//namespace scene_alpha {
//	struct NavMeshSetHeader
//	{
//		int magic;
//		int version;
//		int numTiles;
//		dtNavMeshParams params;
//	};
//
//	struct NavMeshSetHeader_CAI
//	{
//		int version;
//		int tileCount;
//		dtNavMeshParams params;
//	};
//
//	struct NavMeshTileHeader
//	{
//		dtTileRef tileRef;
//		int dataSize;
//	};
//
//	class NavMeshLoader {
//	public:
//		NavMeshLoader();
//		~NavMeshLoader();
//		bool load(const char* path);
//		bool load_cai(const char* path);
//
//		dtNavMesh* getNavMesh() {
//			return m_navMesh;
//		}
//
//		dtNavMeshQuery* getNavMeshQuery() {
//			return m_navQuery;
//		}
//
//	private:
//		dtNavMesh* m_navMesh;
//		dtNavMeshQuery* m_navQuery;
//	};
//
//	struct Vector3D {
//		int x, y, z;
//		void operator=(Vector3D& _vector) {
//			this->x = _vector.x;
//			this->y = _vector.y;
//			this->z = _vector.z;
//		}
//	};
//
//	class Object {
//	public:
//		Object();
//		~Object();
//
//		void set_pos(Vector3D& _pos);
//	private:
//		Vector3D pos;
//	};
//
//	class SceneMng 
//	{
//	public:
//		SceneMng();
//		~SceneMng();
//
//		static SceneMng* getInstance() {
//			static SceneMng instance;
//			return &instance;
//		}
//
//		bool Init();
//
//	public:
//		void enter(Object* object);
//		void move(Object* object, Vector3D& newPos);
//		int findPath(float* startPos, float* endPos);
//		float* getPath() {
//			return m_smoothPath;
//		}
//
//	private:
//		Map m_map;
//		NavMeshLoader m_navMeshLoader;
//
//		static const int MAX_SMOOTH = 2048;
//		float m_smoothPath[MAX_SMOOTH * 3];
//	};
//}