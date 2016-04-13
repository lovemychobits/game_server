#include <iostream>
#include "header.h"
#include "LoginServer.h"

using namespace std;

Logger g_lsLog = Logger::getInstance(LOG4CPLUS_TEXT("ALL_MSGS"));


int main(int argc, char* argv[])
{
	LoginServer loginServer;
	const char* pInitConfPath = "./ls.xml";
	if (argc >= 2)
	{
		pInitConfPath = argv[1];		// ∂¡»°≈‰÷√
	}

	if (!loginServer.Init(pInitConfPath))
	{
		ERRORLOG("loginserver init failed.");
		return 1;
	}
	TRACELOG("loginserver start.");
	loginServer.Start();
	TRACELOG("loginserver end.");

	return 0;
}