#ifndef	DATASEVER_SQLTABLES_H
#define DATASEVER_SQLTABLES_H

#include "header.h"
#include "Mysql.h"
#include <iostream>
using namespace std;

class SqlTables
{
public:
	SqlTables(CMysqlConnection& sqlConn) : m_pSqlConn(&sqlConn)
	{

	}
	bool CreatePlayerInfo()
	{
		const char* pPlayerInfoTable = " \
								 CREATE TABLE IF NOT EXISTS playerinfo( \
								 id			int(16)		NOT NULL primary key auto_increment, \
								 userid		int(16)		NOT NULL default 0, \
								 areaid		int(4)		NOT NULL default 0, \
								 ptname		varchar(48) NOT NULL default '', \
								 rolename	varchar(48) NOT NULL default '', \
								 dataversion int(16)	NOT NULL default 0, \
								 level		int(16)		NOT NULL default 0, \
								 exp		int(16)		NOT NULL default 0, \
								 gold		int(16)		NOT NULL default 0,	\
								 diamond	int(16)		NOT NULL default 0,	\
								 ap			int(16)		NOT NULL default 0, \
								 playerbase   blob,			\
								 herolist	  blob,			\
								 gatelist     blob,			\
								 playerbag	  blob,			\
								 playerlineup blob,			\
								 extrainfo    blob,			\
								 actorlist	  blob,			\
								 INDEX rolename_index (rolename), \
								 INDEX ptname_index (ptname), \
								 INDEX userid_index(userid) \
								 )";

		if (!m_pSqlConn->Execute(pPlayerInfoTable))
		{
			ERRORLOG("create table playerinfo failed. error msg:" << m_pSqlConn->GetErrInfo());
			return false; 
		}
		return true;
	}

	bool CreateMailTable()
	{
		const char* pMailTable = "\
								  CREATE TABLE IF NOT EXISTS mail( \
								  id		int(16) NOT NULL primary key auto_increment, \
								  owner		int(16)	NOT NULL default 0, \
								  mailtext	varchar(256) NOT NULL default '', \
								  sign		varchar(32)	 NOT NULL default '', \
								  mailtype	int(8)	NOT NULL default 0, \
								  state		int(8)	NOT NULL default 0, \
								  mailtime	varchar(32) default '', \
								  deadtime	varchar(32) default '', \
								  gold		int(8) default 0, \
								  diamond	int(8) default 0, \
								  ap		int(8) default 0, \
								  prizes	varchar(256) NOT NULL default '', \
								  INDEX owner_index (owner) \
								  )";


		if (!m_pSqlConn->Execute(pMailTable))
		{
			ERRORLOG("create table mail failed. error msg:" << m_pSqlConn->GetErrInfo());
			return false; 
		}
		return true;						  
	}

	bool CreatePlayerSnapshot()
	{
		const char* pSnapshotTable = " \
									   CREATE TABLE IF NOT EXISTS snapshot( \
									   id			int(16)		NOT NULL primary key auto_increment, \
									   areaid		int(4)		NOT NULL default 0, \
									   ptname		varchar(48) NOT NULL default '', \
									   rolename		varchar(48) NOT NULL default '', \
									   snapshot		blob,			\
									   INDEX rolename_index (rolename), \
									   INDEX ptname_index (ptname) \
									   )";

		if (!m_pSqlConn->Execute(pSnapshotTable))
		{
			ERRORLOG("create table snapshot failed. error msg:" << m_pSqlConn->GetErrInfo());
			return false; 
		}
		return true;
	}

	bool CreateArenaInfo() 
	{
		const char* pArenaInfoTable = "CREATE TABLE IF NOT EXISTS arenainfo( \
									   id				int(16)		NOT NULL primary key auto_increment, \
									   arenaRankList	blob			\
									   )";

		if (!m_pSqlConn->Execute(pArenaInfoTable))
		{
			ERRORLOG("create table arenainfo failed. error msg:" << m_pSqlConn->GetErrInfo());
			return false; 
		}
		return true;
	}

	bool CreateRankListInfo() 
	{
		const char* pRankListInfoTable = "CREATE TABLE IF NOT EXISTS rankinfo( \
									  id				int(16)		NOT NULL primary key auto_increment, \
									  fightRankList		tinyblob,			\
									  teamRankList		tinyblob,			\
									  richRankList		tinyblob			\
									  )";

		if (!m_pSqlConn->Execute(pRankListInfoTable))
		{
			ERRORLOG("create table rankinfo failed. error msg:" << m_pSqlConn->GetErrInfo());
			return false; 
		}
		return true;
	}

	// 积分竞技场积分段信息
	bool CreateScoreArenaInfo() 
	{
		const char* pScoreArenaInfoTable = "CREATE TABLE IF NOT EXISTS scorearenainfo( \
									  id				int(16)		NOT NULL primary key auto_increment, \
									  scoreRankList		blob,			\
									  scoreRobotList	blob			\
									  )";

		if (!m_pSqlConn->Execute(pScoreArenaInfoTable))
		{
			ERRORLOG("create table scorearenainfo failed. error msg:" << m_pSqlConn->GetErrInfo());
			return false; 
		}
		return true;
	}

	// 创建聊天数据表
	bool CreateChatMessage()
	{
		const char* pChatMessage = "CREATE TABLE IF NOT EXISTS chatmessage( \
										   id				int(4)		NOT NULL primary key auto_increment, \
										   msgtype			int(4),			\
										   chatinfo			blob			\
										   )";

		if (!m_pSqlConn->Execute(pChatMessage))
		{
			ERRORLOG("create table chatmessage failed. error msg:" << m_pSqlConn->GetErrInfo());
			return false; 
		}
		return true;
	}

	//创建公会表
	bool CreateSociatyMessage()
	{
		const char* pSociatyInfoTable = " \
									   CREATE TABLE IF NOT EXISTS sociatyinfo( \
									   sociatyid			int(16)		NOT NULL primary key auto_increment, \
									   sociatyname			varchar(48) NOT NULL default '', \
									   sociatyidlevel		int(16)		NOT NULL default 0, \
									   sociatyallinfo		blob			\
									   )";

		if (!m_pSqlConn->Execute(pSociatyInfoTable))
		{
			ERRORLOG("create table sociatyinfo failed. error msg:" << m_pSqlConn->GetErrInfo());
			return false; 
		}
		return true;
	}

	// 创建事件表
	bool CreateEventRecord()
	{
		const char* pEventRecordTable = "\
										CREATE TABLE IF NOT EXISTS eventRecord(\
										eventid			int(16)			NOT NULL primary key auto_increment, \
										userid			int(16)			NOT NULL, \
										eventtype		int(16)			NOT NULL, \
										eventvalue		int(16)			NOT NULL default 0, \
										eventnum		int(16)			NOT NULL default 0, \
										msg				varchar(256)	NOT NULL default '', \
										time			varchar(36)		NOT NULL default '', \
										INDEX userid_index (userid) \
										)";
		if (!m_pSqlConn->Execute(pEventRecordTable))
		{
			ERRORLOG("create table eventRecord failed. error msg:" << m_pSqlConn->GetErrInfo());
			return false; 
		}
		return true;
	}

private:
	CMysqlConnection* m_pSqlConn;
};

#endif