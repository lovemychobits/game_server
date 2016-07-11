#include "LvExpLenConfig.h"

bool LvExpLenConfig::LoadLvExpLenConf(const char* configPath)
{
	if (!m_xmlDoc.LoadFile(configPath))
	{
		cout << "not find LoadLvExpLenConf File" << endl;
		return false;
	}

	TiXmlElement* pRoot = m_xmlDoc.RootElement();
	if (!pRoot)
	{
		return false;
	}


	//<type Subsection="5000" Value="5"/>
	TiXmlElement* lvExpLenNodeInfo = pRoot->FirstChildElement("type");
	if (lvExpLenNodeInfo)
	{
		const char* subsectionStr = lvExpLenNodeInfo->Attribute("Subsection");
		subsection = boost::lexical_cast<int>(subsectionStr);

		const char* valueStr = lvExpLenNodeInfo->Attribute("Value");
		value = boost::lexical_cast<int>(valueStr);
	}

	//cout << "Subsection:" << subsection << ",Value:" << value << endl;
	return true;
}