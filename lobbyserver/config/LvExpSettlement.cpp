#include "LvExpSettlement.h"

bool LvExpSettConfig::LoadLvExpSettConf(const char* configPath)
{
	if (!m_xmlDoc.LoadFile(configPath))
	{
		cout << "not find LoadLvExpSettConf File" << endl;
		return false;
	}

	TiXmlElement* pRoot = m_xmlDoc.RootElement();
	if (!pRoot)
	{
		return false;
	}


	//<type ID = "1" Base_Exp = "50" Base_Multiple = "1" Total_Multiple = "1" / >
	TiXmlElement* lvExpSettNodeInfo = pRoot->FirstChildElement("type");
	if (lvExpSettNodeInfo)
	{
		const char* baseExpStr = lvExpSettNodeInfo->Attribute("Base_Exp");
		baseExp = boost::lexical_cast<int>(baseExpStr);

		const char* baseMultipleStr = lvExpSettNodeInfo->Attribute("Base_Multiple");
		baseMultiple = boost::lexical_cast<int>(baseMultipleStr);

		const char* totalMultipleStr = lvExpSettNodeInfo->Attribute("Total_Multiple");
		totalMultiple = boost::lexical_cast<int>(totalMultipleStr);
	}

	//cout << "baseExp:" << baseExp << ",baseMultiple:" << baseMultiple << ",totalMultiple:" << totalMultiple << endl;
	return true;
}