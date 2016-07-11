#ifndef LOBBYSERVER_CONFIGPATH_H
#define LOBBYSERVER_CONFIGPATH_H

#include "header.h"

class ConfigPath{

public:
	const char* gradeConfigPath = "./xml/Grade_Table.xml";
	const char* skinConfigPath = "./xml/Skin.xml";
	const char* lvExpConfigPath = "./xml/Exp.xml";
	const char* lvExpSettConfigPath = "./xml/D-ExpSettlement.xml";
	const char* lvExpLenConfigPath = "./xml/D-Length_MakeUp.xml";
	const char* lvExpRankConfigPath = "./xml/D-Ranking_MakeUp.xml";


	static ConfigPath* GetInstance()
	{
		static ConfigPath instance;
		return &instance;
	}

private:

};

#define configPath ConfigPath::GetInstance()

#endif