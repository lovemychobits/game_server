#include "SlitherConfig.h"
#include "../../tinyxml/tinyxml.h"

namespace slither {
	slither::SlitherConfig g_slitherConfig;

	SlitherConfig::SlitherConfig() {

	}

	SlitherConfig::~SlitherConfig() {

	}

	bool SlitherConfig::LoadSnakeConf(const char* pFilePath) {
		TiXmlDocument xmlDoc;
		if (!xmlDoc.LoadFile(pFilePath)) {
			return false;
		}

		TiXmlElement* pRoot = xmlDoc.RootElement();
		if (!pRoot) {
			return false;
		}

		TiXmlElement* pConf = pRoot->FirstChildElement("type");
		if (!pConf) {
			ERRORLOG("cannot find type tag");
			return false;
		}

		m_fInitRadius				= (float)atof(pConf->Attribute("Radius"));
		m_fBodyInterval				= (float)atof(pConf->Attribute("Interval"));
		m_fSpeed					= (float)atof(pConf->Attribute("Speed_Normal"));
		m_fSpeedUpRate				= (float)atof(pConf->Attribute("Level_Of_Accelation"));
		m_fSpawnInterval			= (float)atof(pConf->Attribute("Spawn_Interval"));
		m_fGrowUpValue				= (float)atof(pConf->Attribute("Grow_Up"));
		m_fAttenuationValue			= (float)atof(pConf->Attribute("Attenuation"));
		m_fAttenuationInterval		= (float)atof(pConf->Attribute("Attenuation_Interval"));
		m_uSpeedUpGenFoodValue		= atoi(pConf->Attribute("Single_Value"));
		m_fStopSpeedUpValue			= (float)atof(pConf->Attribute("Stop_Accelation"));
		m_uInitSnakeBodySize		= atoi(pConf->Attribute("Body_Num"));
		m_uInitSnakeMass			= atoi(pConf->Attribute("Begin_Length"));
		m_fSpeedUpNeedValue			= (float)atof(pConf->Attribute("Speed_Accelation")) + m_uInitSnakeMass;
		m_fNodeSqt					= (float)atof(pConf->Attribute("Body_Index"));
		m_uMaxBodySize				= atoi(pConf->Attribute("Max_Body"));
		m_uInitDeadFoodSize			= atoi(pConf->Attribute("Begin_Death_Ball_Num"));
		m_uDeadFoodIncValue			= atoi(pConf->Attribute("Death_Ball_Range"));
		m_uDeadFoodValue			= atoi(pConf->Attribute("Death_Ball_Value"));
		m_fIncRadiusValue			= (float)atof(pConf->Attribute("Bulky"));
		m_uIncRadiusIntervalValue	= atoi(pConf->Attribute("Bulky_Value"));
		m_fRadiusSqt				= (float)atof(pConf->Attribute("Bulky_Index"));
		m_fMaxRadius				= (float)atof(pConf->Attribute("Max_Bulky"));
		m_fEatFoodRadius			= (float)atof(pConf->Attribute("Eat_Ball_Range"));
		m_fCollideProportion		= (float)atof(pConf->Attribute("Touch_Proportion"));
		return true;
	}

	bool SlitherConfig::LoadMapConf(const char* pFilePath) {
		TiXmlDocument xmlDoc;
		if (!xmlDoc.LoadFile(pFilePath)) {
			return false;
		}

		TiXmlElement* pRoot = xmlDoc.RootElement();
		if (!pRoot) {
			return false;
		}

		TiXmlElement* pConf = pRoot->FirstChildElement("type");
		if (!pConf) {
			ERRORLOG("cannot find type tag");
			return false;
		}

		m_uGenFoodMinValue				= atoi(pConf->Attribute("B_Ball"));
		m_uGenFoodMaxValue				= atoi(pConf->Attribute("S_Ball"));
		m_fRefreshGenFoodInterval		= (float)atof(pConf->Attribute("Refresh_Interval"));
		m_fRefreshFoodDensity			= (float)atof(pConf->Attribute("Refresh_Density"));
		m_fRefreshRankingListInterval	= (float)atof(pConf->Attribute("Score_Refresh_Interval"));
		m_uGameRoundTime				= atoi(pConf->Attribute("Game_Time"));
		m_uMapLength					= atoi(pConf->Attribute("Map_Length"));
		m_uGridSize						= atoi(pConf->Attribute("Grid_Size"));
		m_uSimInterval					= atoi(pConf->Attribute("Sim_Interval"));
		m_uInitFoodNum					= atoi(pConf->Attribute("Initial_Ball_Num"));
		m_uRefreshFoodThreshold			= atoi(pConf->Attribute("Less_Than_Num"));
		m_uRefreshFoodNum				= atoi(pConf->Attribute("Again_Ball_Num"));

		m_fInsideScale					= (float)atof(pConf->Attribute("Inside_Scale"));
		m_fMiddleScale					= (float)atof(pConf->Attribute("Middle_Scale"));
		m_fInsideGenScale				= (float)atof(pConf->Attribute("Inside_Produce_Scale"));
		m_fMiddleGenScale				= (float)atof(pConf->Attribute("Middle_Produce_Scale"));

		m_uInitViewRange				= atoi(pConf->Attribute("Init_View_Range"));
		m_fViewAttenuation				= (float)atof(pConf->Attribute("Camera_Attenuation_Index"));
		m_uSnakeSimTimes				= atoi(pConf->Attribute("Snake_Sim_Times"));

		return true;
	}

	bool SlitherConfig::LoadRoomConf(const char* pFilePath) {
		TiXmlDocument xmlDoc;
		if (!xmlDoc.LoadFile(pFilePath)) {
			return false;
		}

		TiXmlElement* pRoot = xmlDoc.RootElement();
		if (!pRoot) {
			return false;
		}

		TiXmlElement* pConf = pRoot->FirstChildElement("type");
		if (!pConf) {
			ERRORLOG("cannot find type tag");
			return false;
		}

		m_uRoomNum						= atoi(pConf->Attribute("RoomNum"));
		m_uRoomSnakeLimit				= atoi(pConf->Attribute("RoomSnakeLimit"));
		m_uRefuseEnterTime				= atoi(pConf->Attribute("RefuseEnterTime"));
		m_uInvalidNum					= atoi(pConf->Attribute("InvalidNum"));
		m_uRobotNum						= atoi(pConf->Attribute("RobotNum"));
		m_uRobotMass					= atoi(pConf->Attribute("RobotMass"));

		return true;
	}
}