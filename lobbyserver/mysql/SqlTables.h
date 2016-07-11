#ifndef LOBBYSERVER_SQLTABLES_H
#define LOBBYSERVER_SQLTABLES_H

#include "../header.h"
#include "../../utils/Mysql.h"
#include "../../utils/SqlParser.h"
#include <iostream>
using namespace std;

class SqlTables
{
public:
	SqlTables(cputil::CMysqlConnection& sqlConn) : m_pSqlConn(&sqlConn), m_sqlParser(m_pSqlConn) {
	}

public:
	bool CreateTables(const char* pPath) {
		return m_sqlParser.ExecuteSqlFile(pPath);
	}

private:
	cputil::CMysqlConnection* m_pSqlConn;
	cputil::SqlParser m_sqlParser;
};

#endif