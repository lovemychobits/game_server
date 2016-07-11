#include "SkinConfig.h"
#include <boost/algorithm/string.hpp>


void getUnLockMapFromStr(string unLockStr, map<int, int>* unLockInfoMap);
int getRandNum(int beginNum,int endNum);

SkinConfig::SkinConfig()
{
}

SkinConfig::~SkinConfig()
{
}


bool SkinConfig::LoadSkinConf(const char* pFilePath)
{
	if (!m_xmlDoc.LoadFile(pFilePath))
	{
		cout << "not find LoadSkinConf File" << endl;
		return false;
	}

	TiXmlElement* pRoot = m_xmlDoc.RootElement();
	if (!pRoot)
	{
		return false;
	}


	//<type ID="1" Unlock_Type="1,1" Skin_Type="1" Node_Head="0" Node_Body="1" Node_Tail="0" Name_Color="1" Shit_Color="1" Corpse_Color="1"/>
	TiXmlElement* skinInfoNodeInfo = pRoot->FirstChildElement("type");
	while (skinInfoNodeInfo)
	{
		const char* idStr = skinInfoNodeInfo->Attribute("ID");
		if (idStr)
		{
			int skinId = boost::lexical_cast<int>(idStr);
			const char* unlockTypeStr = skinInfoNodeInfo->Attribute("Unlock_Type");
			if (unlockTypeStr)
			{
				map<int, int> unLockInfoMap;
				getUnLockMapFromStr(unlockTypeStr, &unLockInfoMap);

				if (!unLockInfoMap.empty() && unLockInfoMap.size() > 0)
				{
					SkinInfo skinInfo;
					skinInfo.id = skinId;
					skinInfo.unLockInfoMap = unLockInfoMap;

					skinsMap.insert(pair<int, SkinInfo>(skinInfo.id, skinInfo));
					allSkinIdVector.push_back(skinInfo.id);

					const char* isNeedRandChar = skinInfoNodeInfo->Attribute("Random_Or_Display");
					string isNeedRandStr(isNeedRandChar);
					int isNeedRand = boost::lexical_cast<int>(isNeedRandStr);
					if (isNeedRand == IS_RAND)
					{
						canInitSkinIdVector.push_back(skinId);
						canInitSkinIdMap.insert(pair<int, int>(skinId, skinId));
					}
				}
			}
		}

		skinInfoNodeInfo = skinInfoNodeInfo->NextSiblingElement();
	}
	return true;
}

vector<int> SkinConfig::getAllCanInitSkinIds()
{
	return canInitSkinIdVector;
}


vector<int> SkinConfig::getAllSkinIds()
{
	return allSkinIdVector;
}


bool SkinConfig::isCanInitSkinId(const int skinId)
{
	map<int, int>::iterator iter;
	iter = canInitSkinIdMap.find(skinId);

	if (iter != canInitSkinIdMap.end())
	{
		return true;
	}
	return false;
}

int SkinConfig::getRandCanInitSkinId()
{
	int id = 1;

	vector<int> allCanInitSkinIds = getAllCanInitSkinIds();
	if (allCanInitSkinIds.size() > 0){
		int randIndex = getRandNum(0, allCanInitSkinIds.size());
		cout << "rand NUM===  " << randIndex << endl;
		id = allCanInitSkinIds.at(randIndex);
	}

	return id;
}



SkinInfo SkinConfig::getSkinInfoById(const int skinId)
{
	SkinInfo skinInfo;

	map<int, SkinInfo>::iterator iter;
	iter = skinsMap.find(skinId);

	if (iter != skinsMap.end())
	{
		skinInfo = iter->second;
	}

	return skinInfo;
}










void getUnLockMapFromStr(string unLockStr, map<int, int>* unLockInfoMap)
{
	if (!unLockStr.empty() && unLockStr.length() > 0)
	{
		vector<string> unLockInfoArr;
		boost::split(unLockInfoArr, unLockStr, boost::is_any_of(";"));

		if (!unLockInfoArr.empty() && unLockInfoArr.size() > 0)
		{
			size_t size = unLockInfoArr.size();
			for (size_t i = 0; i<size; i++)
			{
				string singleUnLockInfoStr = unLockInfoArr[i];
				vector<string> singleUnLockInfoArr;

				boost::split(singleUnLockInfoArr, singleUnLockInfoStr, boost::is_any_of(","));
				if (!singleUnLockInfoArr.empty() && singleUnLockInfoArr.size() == 2)
				{
					int unLockType = boost::lexical_cast<int>(singleUnLockInfoArr[0]);
					int unLockValue = boost::lexical_cast<int>(singleUnLockInfoArr[1]);

					unLockInfoMap->insert(pair<int, int>(unLockType, unLockValue));
				}
			}
		}
	}
}



int getRandNum(int beginNum, int endNum)
{
	srand((unsigned)time(NULL)); //初始化随机数种子
	return  beginNum + (rand()%endNum);
}
