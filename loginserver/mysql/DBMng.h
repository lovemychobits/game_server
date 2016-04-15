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
	bool Register(const char* pUserName, const char* pPasswd);					// �û�ע��
	
private:
	bool IsUserExist(const char* pUserName);									// �Ƿ��˺��Ѿ�����

private:
	DBMng();

private:
	CMysqlConnection* m_pSqlConnection;
};

#endif