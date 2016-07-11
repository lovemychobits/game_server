#ifndef LOBBYSERVER_SKINCONFIG_H
#define LOBBYSERVER_SKINCONFIG_H

#include "../header.h"
#include "../../tinyxml/tinyxml.h"
#include <boost/lexical_cast.hpp>

//<type ID = "1" Unlock_Type = "1,1" Skin_Type = "1" Node_Head = "0" Node_Body = "1" Node_Tail = "0" Name_Color = "1" Shit_Color = "1" Corpse_Color = "1" / >
struct SkinInfo
{
	int id;
	int isNeedRandom;
	map<int, int> unLockInfoMap;
	//string unLockInfo;
	//string Skin_Type;
	//string Node_Head;
	//string Node_Body;
	//string Node_Tail;
	//string Name_Color;
	//string Shit_Color;
	//string Corpse_Color;
};


class SkinConfig
{
public:
	SkinConfig();
	~SkinConfig();
	const int UNLOCK_TYPE_LEVEL= 1;
	const int IS_RAND = 0;
	

	static SkinConfig* GetInstance()
	{
		static SkinConfig instance;
		return &instance;
	}

	bool LoadSkinConf(const char* configPath);

	SkinInfo getSkinInfoById(const int skinId);
	bool isCanInitSkinId(const int skinId);
	vector<int> getAllCanInitSkinIds();
	vector<int> getAllSkinIds();
	int getRandCanInitSkinId();


private:
	TiXmlDocument m_xmlDoc;
	map<int, int> canInitSkinIdMap;
	vector<int> canInitSkinIdVector;
	vector<int> allSkinIdVector;

	map<int, SkinInfo> skinsMap;

};









#define skinConfig SkinConfig::GetInstance()


#endif