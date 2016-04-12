#include "PlayerDataMng.h"
#include "header.h"
#include "GsConnectionMng.h"
#include "ServerConfig.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

void PlayerDataMng::SetMysqlInfo(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb)
{
	m_strHostIp = szHost;
	m_uPort = nPort;
	m_strUser= szUser;
	m_strPasswd = szPasswd;
	m_strDB = szDb;
}

bool PlayerDataMng::Init()
{
	if (!m_sqlConn.Init(m_strHostIp.c_str(), m_uPort, m_strUser.c_str(), m_strPasswd.c_str(), m_strDB.c_str()))
	{
		ERRORLOG("init mysql connection failed, mysql ip=[" << m_strHostIp.c_str() << "], port=[" << m_uPort << "], user=[" << m_strUser.c_str() << "], pwd=[" << m_strPasswd.c_str() << "], db=[" << m_strDB.c_str() << "]");
		return false;
	}

	if (!InitTables())
	{
		ERRORLOG("init create tables failed.");
		return false;
	}
	m_strAreaId = boost::lexical_cast<string>(gpServerConfig->GetAreaId());
	return true;
}

bool PlayerDataMng::InitTables()
{
	if (!m_sqlTables.CreatePlayerInfo())
	{
		ERRORLOG("create table playerinfo failed.");
		return false;
	}
	if (!m_sqlTables.CreateMailTable())
	{
		ERRORLOG("create table mail failed.");
		return false;
	}
	if (!m_sqlTables.CreatePlayerSnapshot())
	{
		ERRORLOG("create table snapshot failed.");
		return false;
	}
	if (!m_sqlTables.CreateArenaInfo())
	{
		ERRORLOG("create table arenainfo failed.");
		return false;
	}
	if (!m_sqlTables.CreateRankListInfo())
	{
		ERRORLOG("create table rankinfo failed.");
		return false;
	}
	if (!m_sqlTables.CreateScoreArenaInfo())
	{
		ERRORLOG("create table scorearenainfo failed.");
		return false;
	}
	if (!m_sqlTables.CreateChatMessage())
	{
		ERRORLOG("create table chatmessage failed.");
		return false;
	}
	if (!m_sqlTables.CreateSociatyMessage())
	{
		ERRORLOG("create table chatmessage failed.");
		return false;
	}
	if (!m_sqlTables.CreateEventRecord())
	{
		ERRORLOG("create table event record failed.");
		return false;
	}
	return true;
}
