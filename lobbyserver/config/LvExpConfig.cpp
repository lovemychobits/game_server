#include "LvExpConfig.h"


LvExpConfig::LvExpConfig()
{
}

LvExpConfig::~LvExpConfig()
{
}


bool LvExpConfig::LoadLvExpConf(const char* configPath)
{
	if (!m_xmlDoc.LoadFile(configPath))
	{
		cout << "not find LoadLvExpConf File" << endl;
		return false;
	}

	TiXmlElement* pRoot = m_xmlDoc.RootElement();
	if (!pRoot)
	{
		return false;
	}


	////<type CurLvl = "1" Exp = "0" / >
	TiXmlElement* lvExpInfoNodeInfo = pRoot->FirstChildElement("type");
	int maxExpTemp = 0;
	while (lvExpInfoNodeInfo)
	{
		//LvExpInfo lvExpInfo;
		int level = 0;
		int exp = 0;
		const char* levelStr = lvExpInfoNodeInfo->Attribute("CurLvl");
		if (levelStr)
		{
			//lvExpInfo.level = boost::lexical_cast<int>(levelStr);
			level = boost::lexical_cast<int>(levelStr);
		}

		const char* expStr = lvExpInfoNodeInfo->Attribute("Exp");
		if (expStr)
		{
			exp = boost::lexical_cast<int>(expStr);
			//lvExpInfo.exp = boost::lexical_cast<int>(expStr);
		}

		if (level > 0)
		{
			lvExpsMapByLevel.insert(pair<int, int>(level, exp));
			if (exp > maxExpTemp)
			{
				maxExpTemp = exp;
			}
		}
		

		lvExpInfoNodeInfo = lvExpInfoNodeInfo->NextSiblingElement();
	}

	maxExp = maxExpTemp;
	//cout << "lvExpsMapByLevel  size= " << lvExpsMapByLevel.size() << endl;

	//map<int, int>::iterator it;
	//for (it = lvExpsMapByLevel.begin(); it != lvExpsMapByLevel.end(); ++it)
	//{
	//	cout << "key: " << it->first << " value: =Lv" << it->first << ",Exp:" << it->second <<"]" << endl;
	//}

	return true;
}

int LvExpConfig::getLvByExp(const int exp)
{
	int lv = 1;
	int needUpExp = 0;

	map<int, int>::iterator it;
	for (it = lvExpsMapByLevel.begin(); it != lvExpsMapByLevel.end(); ++it)
	{
		needUpExp = it->second;
		if (exp >= needUpExp){
			lv = it->first;
		}
		else
		{
			break;
		}
	}

	return lv;
}