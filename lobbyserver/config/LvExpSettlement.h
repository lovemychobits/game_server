#ifndef LOBBYSERVER_LVEXPSETT_H
#define LOBBYSERVER_LVEXPSETT_H

#include "../header.h"
#include "../../tinyxml/tinyxml.h"
#include <boost/lexical_cast.hpp>


class LvExpSettConfig{
public:
	LvExpSettConfig(){

	}
	~LvExpSettConfig(){

	}

	static LvExpSettConfig* GetInstance()
	{
		static LvExpSettConfig instance;
		return &instance;
	}

	bool LoadLvExpSettConf(const char* configPath);

	int getBaseExp()
	{
		return baseExp;
	}

	int getBaseMultiple()
	{
		return baseMultiple;
	}

	int getTotalMultiple()
	{
		return totalMultiple;
	}

	//<type ID = "1" Base_Exp = "50" Base_Multiple = "1" Total_Multiple = "1" / >
private:
	TiXmlDocument m_xmlDoc;

	int baseExp;
	int baseMultiple;
	int totalMultiple;
};







#define lvExpSettConfig LvExpSettConfig::GetInstance()


#endif