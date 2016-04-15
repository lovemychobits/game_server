#ifndef LOGINSERVER_DBOPERATION_H
#define LOGINSERVER_DBOPERATION_H

#include "../../utils/Mysql.h"

class DBMng
{
public:
	~DBMng();
	static DBMng* getInstance() {
		static DBMng instance;
		return &instance;
	}

	bool init();

public:
	bool CheckLogin(const char* pUserName, const char* pPasswd);				// 检查能否登陆成功
	bool Register(const char* pUserName, const char* pPasswd);					// 用户注册
	
private:
	bool IsUserExist(const char* pUserName);									// 是否账号已经存在

private:
	DBMng();

private:
	CMysqlConnection* m_pSqlConnection;
};

#endif