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
	bool CheckLogin(const char* pUserName, const char* pPasswd);				// ����ܷ��½�ɹ�

private:
	DBMng();

private:
	CMysqlConnection* m_pSqlConnection;
};

#endif