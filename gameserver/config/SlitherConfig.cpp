#include "SlitherConfig.h"
#include "../../tinyxml/tinyxml.h"

namespace slither {
	SlitherConfig::SlitherConfig() {

	}

	SlitherConfig::~SlitherConfig() {

	}

	bool SlitherConfig::LoadSlitherConf(const char* pFilePath) {
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
		m_uGrowUpValue				= atoi(pConf->Attribute("Grow_Up"));
		m_fSpeedUpNeedValue			= (float)atof(pConf->Attribute("Speed_Accelation")) + m_fInitRadius;
		m_fAttenuationValue			= (float)atof(pConf->Attribute("Attenuation"));
		m_fAttenuationInterval		= (float)atof(pConf->Attribute("Attenuation_Interval"));
		m_uSpeedUpGenFoodValue		= atoi(pConf->Attribute("Single_Value"));
		m_fStopSpeedUpValue			= (float)atof(pConf->Attribute("Stop_Accelation"));
		m_uInitSnakeBodySize		= atoi(pConf->Attribute("Body_Num"));
		m_uInitSnakeMass			= atoi(pConf->Attribute("Begin_Length"));
		m_fNodeSqt					= (float)atof(pConf->Attribute("Body_Index"));
		m_uMaxBodySize				= atoi(pConf->Attribute("Max_Body"));
		m_uInitDeadFoodSize			= atoi(pConf->Attribute("Begin_Death_Ball_Num"));
		m_uDeadFoodIncValue			= atoi(pConf->Attribute("Death_Ball_Range"));
		m_fIncRadiusValue			= (float)atof(pConf->Attribute("Bulky"));
		m_uIncRadiusIntervalValue	= atoi(pConf->Attribute("Bulky_Value"));
		m_fRadiusSqt				= (float)atof(pConf->Attribute("Bulky_Index"));
		m_fMaxRadius				= (float)atof(pConf->Attribute("Max_Bulky"));
		m_fEatFoodRadius			= (float)atof(pConf->Attribute("Eat_Ball_Range"));
		return true;
	}

	bool SlitherConfig::LoadSlitherConf2(const char* pFilePath) {
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
		return true;
	}
}