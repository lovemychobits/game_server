#ifndef	DATASEVER_SQLTABLES_H
#define DATASEVER_SQLTABLES_H

#include "../header.h"
#include "../../utils/Mysql.h"
#include "../../utils/SqlParser.h"
#include <iostream>
using namespace std;

class SqlTables
{
public:
	SqlTables(CMysqlConnection& sqlConn) : m_pSqlConn(&sqlConn), m_sqlParser(m_pSqlConn)
	{
	}
	
public:
	bool CreateTables(const char* pPath) {
		return m_sqlParser.ExecuteSqlFile(pPath);
	}

private:
	CMysqlConnection* m_pSqlConn;
	SqlParser m_sqlParser;
};

#endif