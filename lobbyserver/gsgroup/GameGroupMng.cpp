#include "GameGroupMng.h"
#include "../../network/IConnection.h"
#include "../../tinyxml/tinyxml.h"

GameGroupMng::GameGroupMng()
{

}

GameGroupMng::~GameGroupMng()
{
	m_groupMap.clear();
}

bool GameGroupMng::LoadGameGroupConf()
{
	TiXmlDocument xmlDoc;
	if (!xmlDoc.LoadFile("./xml/groupList.xml"))
	{
		return false;
	}

	TiXmlElement* pRoot = xmlDoc.RootElement();
	if (!pRoot)
	{
		return false;
	}

	TiXmlElement* pGroupList = pRoot->FirstChildElement("Group");
	if (!pGroupList)
	{
		return false;
	}

	TiXmlElement* pGroup = pGroupList->FirstChildElement("group");
	while (pGroup)
	{
		GroupInfo groupInfo;
		groupInfo.uGroupId		= boost::lexical_cast<uint32_t>(pGroup->Attribute("Id"));
		groupInfo.strGroupName  = pGroup->Attribute("Name");
		
		m_groupMap.insert(make_pair(groupInfo.uGroupId, groupInfo));
		pGroup = pGroup->NextSiblingElement();
	}

	return true;
}

void GameGroupMng::GameServerDisconnect(IConnection* pGsConnection)
{
	map<IConnection*, uint32_t>::iterator connIt = m_groupConnMap.find(pGsConnection);
	if (connIt == m_groupConnMap.end())
	{
		ERRORLOG("connection=[" << pGsConnection << "] cannot find in group connection map");
		return;
	}

	uint32_t uGroupId = connIt->second;
	m_groupConnMap.erase(connIt);

	map<uint32_t, GroupInfo>::iterator groupIt = m_groupMap.find(uGroupId);
	if (groupIt == m_groupMap.end())
	{
		ERRORLOG("groupId=[" << uGroupId << "] cannot find in group map");
		return;
	}
	GroupInfo& groupInfo = groupIt->second;
	groupInfo.bHasRegister = false;
	groupInfo.uState = MAINTAIN;
	return;
}

// gameserver注册
bool GameGroupMng::RegGameServer(IConnection* pGsConnection, uint32_t uGroupId, const string& strIp, uint32_t uPort, uint32_t uState)
{
	if (!pGsConnection)
	{
		return false;
	}
	map<uint32_t, GroupInfo>::iterator groupIt = m_groupMap.find(uGroupId);
	if (groupIt == m_groupMap.end())			// 已经存在
	{
		ERRORLOG("groupid=[" << uGroupId << "] not exist");
		return false;
	}

	GroupInfo& groupInfo = groupIt->second;	
	groupInfo.strIp = strIp;
	groupInfo.uPort = uPort;
	groupInfo.uState = uState;
	groupInfo.bHasRegister = true;

	m_groupConnMap[pGsConnection] = uGroupId;	// 保存当前的区组ID对应的连接
	
	return true;
}

// 获取gameserver区组列表
void GameGroupMng::GetGameGroups(vector<GroupInfo>& groupVec)
{
	map<uint32_t, GroupInfo>::iterator groupIt = m_groupMap.begin();
	map<uint32_t, GroupInfo>::iterator groupItEnd = m_groupMap.end();

	for (; groupIt!=groupItEnd; groupIt++)
	{
		groupVec.push_back(groupIt->second);
	}

	return;
}