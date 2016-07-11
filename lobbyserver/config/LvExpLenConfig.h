#ifndef LOBBYSERVER_LVEXPLEN_H
#define LOBBYSERVER_LVEXPLEN_H

#include "../header.h"
#include "../../tinyxml/tinyxml.h"
#include <boost/lexical_cast.hpp>


class LvExpLenConfig{
public:
	LvExpLenConfig(){

	}
	~LvExpLenConfig(){

	}

	static LvExpLenConfig* GetInstance()
	{
		static LvExpLenConfig instance;
		return &instance;
	}

	bool LoadLvExpLenConf(const char* configPath);

	int getSubsection()
	{
		return subsection;
	}

	int getValue()
	{
		return value;
	}

	//<type Subsection="5000" Value="5"/>
private:
	TiXmlDocument m_xmlDoc;

	int subsection;
	int value;
};







#define lvExpLenConfig LvExpLenConfig::GetInstance()


#endif