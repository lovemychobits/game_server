#ifndef LOBBYSERVER_LVEXP_H
#define LOBBYSERVER_LVEXP_H

#include "../header.h"
#include "../../tinyxml/tinyxml.h"
#include <boost/lexical_cast.hpp>

//<type CurLvl = "1" Exp = "0" / >
struct LvExpInfo
{
	int level;
	int exp;
};



class LvExpConfig{
public:
	LvExpConfig();
	~LvExpConfig();

	static LvExpConfig* GetInstance()
	{
		static LvExpConfig instance;
		return &instance;
	}

	bool LoadLvExpConf(const char* configPath);
	int getLvByExp(const int exp);

	int getMaxExp()
	{
		return maxExp;
	}
private:
	TiXmlDocument m_xmlDoc;

	map<int, int> lvExpsMapByLevel;
	int maxExp;
};


#define lvExpConfig LvExpConfig::GetInstance()


#endif