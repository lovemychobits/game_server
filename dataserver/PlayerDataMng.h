#ifndef	DATASERVER_PLAYERDATAMNG_H
#define DATASERVER_PLAYERDATAMNG_H

#include "../utils/Utility.h"
#include <string>
#include "SqlTables.h"
#include "Mysql.h"
#include "../network/IConnection.h"
#include "../utils/ProtoTemplate.hpp"
using namespace std;

#define MAX_BLOB_SIZE 255*1024
#define MAX_SOCIATYID_START 10000		//公会ID开始

enum EMailEvent
{
	ADD_MAIL = 1,					// 添加邮件
	DEL_MAIL = 2,					// 删除邮件
};

class PlayerDataMng
{
public:
	PlayerDataMng() : m_sqlTables(m_sqlConn)
	{

	}

	static PlayerDataMng* GetInstance()
	{
		static PlayerDataMng instance;
		return &instance;
	}

	void SetMysqlInfo(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb);
	bool Init();

private:
	bool InitTables(); 

private:
	CMysqlConnection m_sqlConn;			// mysql连接对象
	SqlTables m_sqlTables;				// 创建表

	string m_strHostIp;					// Mysql IP
	uint32_t m_uPort;					// Mysql端口
	string m_strUser;					// 用户名
	string m_strPasswd;					// 密码
	string m_strDB;						// 指定的DB
	string m_strAreaId;					// DataServer所在区组的ID

	char m_escapeBuf[MAX_BLOB_SIZE];
};

#define gpPlayerDataMng PlayerDataMng::GetInstance()

#endif