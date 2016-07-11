#ifndef LOBBYSERVER_LVEXPRANK_H
#define LOBBYSERVER_LVEXPRANK_H

#include "../header.h"
#include "../../tinyxml/tinyxml.h"
#include <boost/lexical_cast.hpp>




class LvExpRankConfig{
public:
	LvExpRankConfig(){

	}
	~LvExpRankConfig(){

	}

	static LvExpRankConfig* GetInstance()
	{
		static LvExpRankConfig instance;
		return &instance;
	}

	bool LoadLvExpRankConf(const char* configPath);

	int getRankExp(const int rank);
	

	//<type ID="1" Value="30"/>
private:
	TiXmlDocument m_xmlDoc;

	map<int, int> rankExpsMap;
};





#define lvExpRankConfig LvExpRankConfig::GetInstance()


#endif