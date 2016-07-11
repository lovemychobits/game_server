#include "LvExpRankConfig.h"

bool LvExpRankConfig::LoadLvExpRankConf(const char* configPath)
{
	if (!m_xmlDoc.LoadFile(configPath))
	{
		cout << "not find LoadLvExpRankConf File" << endl;
		return false;
	}

	TiXmlElement* pRoot = m_xmlDoc.RootElement();
	if (!pRoot)
	{
		return false;
	}

	TiXmlElement* lvExpRankNodeInfo = pRoot->FirstChildElement("type");
	while (lvExpRankNodeInfo)
	{
		const char* idStr = lvExpRankNodeInfo->Attribute("ID");
		int id = boost::lexical_cast<int>(idStr);

		const char* valueStr = lvExpRankNodeInfo->Attribute("Value");
		int value = boost::lexical_cast<int>(valueStr);

		rankExpsMap.insert(pair<int, int>(id, value));

		lvExpRankNodeInfo = lvExpRankNodeInfo->NextSiblingElement();
	}


	//cout << "rankExpsMap  size= " << rankExpsMap.size() << endl;

	//map<int, int>::iterator it;
	//for (it = rankExpsMap.begin(); it != rankExpsMap.end(); ++it)
	//{
	//	cout << "key: " << it->first << " value: =Lv" << it->first << ",Exp:" << it->second <<"]" << endl;
	//}


	return true;
}

int LvExpRankConfig::getRankExp(const int rank)
{
	map<int, int>::iterator iter;
	iter = rankExpsMap.find(rank);

	if (iter != rankExpsMap.end())
	{
		int exp = iter->second;
		if (exp > 0)
		{
			return exp;
		}
	}
	return 0;
}