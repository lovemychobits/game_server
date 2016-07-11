#include <iostream>
#include "header.h"
#include "LobbyServer.h"
#include "../utils/MyMd5.h"
#include "../utils/BoostHttpUtil.h"
#include "jsonhelper/JsonHelper.h"
#include <boost/locale.hpp>
#include "lzSDK/lzSDK.h"
#include <boost/algorithm/string.hpp>
#include "../protocol/slither_server.pb.h"
#include "../protocol/slither_player.pb.h"
#include "playermng/PlayerStatisticsMgr.h"
#include "playermng/playerMsg.h"




using namespace std;
IConnection* g_pGsConnection = NULL;
Logger g_lobbyLog = Logger::getInstance(LOG4CPLUS_TEXT("ALL_MSGS"));
Logger g_clientLog = Logger::getInstance(LOG4CPLUS_TEXT("client"));


int testLogin();
void testBoostSplit();
void testNTYGAMEROOMEND();
void testSplitUnLock();
void testSetUseSkin(int playerId, int skinId);
void testUnLockSkin(int playerId, int skinId);
void testModifyPlayer(int playerId, bool gender, string name);
void testRand(int beginNum, int endXNum);


int main(int argc, char* argv[])
{
	LobbyServer lobbyServer;
	const char* serverConfigPath = "./lobby.xml";
	
	if (argc >= 2)
	{
		serverConfigPath = argv[1];		// 读取配置
	}

	if (!lobbyServer.Init(serverConfigPath))
	{
		ERRORLOG("lobbyServer init failed.");
		system("pause");
		return 1;
	}

	TRACELOG("dataserver start.");
	cout << "server start success" << endl;





	//testModifyPlayer(66, false, "ABCDD");
	//system("pause");
	//testLogin();
	//testModifyPlayer(39, false, "AAAAABCDAAAAA");
	//testModifyPlayer(66, false, "ABCDD");
	//testLogin();
	
	//testBoostSplit();
	//testNTYGAMEROOMEND();
	//testLogin();
	//testSplitUnLock();
	//testUnLockSkin(7, 1);
	//testSetUseSkin(7, 3);
	//testUnLockSkin(7, 12);
	//testSetUseSkin(7,4);
	//testSetUseSkin(7, 12);
	//testSetUseSkin(7, 12);
	//testRand(0, 11);

	lobbyServer.Start();
	return 0;
}


int testLogin()
{
	//string str = "123";
	//string md5Str = cputil::md5(str);
	//cout << "md5Str=[" << md5Str << "]" << endl;

	cputil::BoostHttpUtil boastHttpUtil;

	//http://42.62.67.224:8013/OAuth1/User/GetInfo

	//string host = "sdk.lezhuogame.com";
	//string port = "80";
	////string page = "/OAuth1/User/GetInfo";
	//string page = "/OAuth1/User/GetInfo";


	//map<string, string> queryParaMap;
	//queryParaMap["appid"] = "6f3da2dbfc37";
	//queryParaMap["appvers"] = "aaaaaa";
	//queryParaMap["device"] = "iphone5";
	//queryParaMap["deviceuuid"] = "F9CBB739-6C7C-42FB-A9EB-6F6EB7F265AD";
	//queryParaMap["fun"] = "GetInfo";
	//queryParaMap["os"] = "iphoneOS";
	//queryParaMap["osvers"] = "7.1.2";
	//queryParaMap["time"] = "1464343734";
	//queryParaMap["token"] = "";


	//string data = lzSDKMgr->getQueryStrAndSign(queryParaMap,"444e8ac01dd14c78a26b9dac");

	////string data = "appid=6f3da2dbfc37&appvers=%e7%89%88%e6%9c%ac%e5%a4%aa%e9%ab%98&device=iphone5%e7%99%bd%e8%89%b2&deviceuuid=F9CBB739-6C7C-42FB-A9EB-6F6EB7F265AD&fun=GetInfo&os=iphoneOS&osvers=7.1.2&time=1464343734&token=bc370e93babd4a0ca3f0ca680a2be3b&sign=41c5d09d4c57fcc81bf6464ae2885b18";
	//string reponse_data = "";

	//	//const string& host, const string& port, const string& page, const string& data, string& reponse_data
	//int ret = boastHttpUtil.post(host, port, page, data, reponse_data);

	//cout << "error_code:" << ret << std::endl;
	//cout << "RESP: " << reponse_data << endl;

	//TRACELOG("SDK RESP[" << reponse_data.c_str() << "]");

	//map<string, string> respJsonMap = jsonHelper::parseSDKGetInfoResp(reponse_data);
	//if (!respJsonMap.empty() && respJsonMap.size() > 0)
	//{
	//	string stateStr = respJsonMap["state"];
	//	cout << "statStr:" << stateStr << endl;
	//	if (stateStr != "")
	//	{
	//		int stateCode = boost::lexical_cast<int>(stateStr);
	//	}
	//}


	//system("pause");

	//getSDKAccountId

	slither::VerifyTokenReq reqInfo;
	reqInfo.set_token("-1");
	//reqInfo.set_appvers(gpServerConfig->GetSDKInfo().ver);
	reqInfo.set_device("IPHONE");
	//reqInfo.set_deviceuuid("6a45e558eb4258d7619086e1ec9d17f7fdda10a8");
	reqInfo.set_deviceuuid("abcdAKKKKAAAefd");
	//reqInfo.set_deviceuuid("uuid");
	reqInfo.set_os("iphoneOS");
	reqInfo.set_osvers("7.1.2");


	
	playerMsgDeal->dealVerifySDKToken(NULL,reqInfo);

	//slither::GetPlayerInfoReq getInfoReq;
	//getInfoReq.set_playerid(39);
	//playerMsgDeal->GetPlayerDataByPlayerId(NULL,getInfoReq.playerid());


	//string s = "6f3da2dbfc37aaaaaa高版本iphone5白色uuidGetInfoiphoneOS7.1.214643658708c39bb67f125449289c65d1d6326adcb444e8ac01dd14c78a26b9dac";

	//int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, 0);


	//vector<wchar_t> unicode(len);
	//MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, &unicode[0], len);

	//wstring sw = &unicode[0];

	//int len2 = WideCharToMultiByte(CP_UTF8, 0, sw.c_str(), -1, NULL, 0, NULL, NULL);

	//vector<char> utf8(len2);
	//WideCharToMultiByte(CP_UTF8, 0, sw.c_str(), -1, &utf8[0], len2, NULL, NULL);

	//string ssss = &utf8[0];
	//cout << ssss << endl;


	//string sign = cputil::md5(s);
	//if (sign == "1ddf6e1ff9e0894c6d7cf70b31d6eb4d") cout << "md5 right" << endl;
	//else cout << "md5 error" << endl;
	//cout << "sign:" << sign << endl;




	return 0;
}

void testBoostSplit(){
	string str = "";

	vector<string> splitIds;
	boost::split(splitIds, str, boost::is_any_of(","));

	int len = splitIds.size();
	cout << "LEN:" << len << endl;

	for (int i = 0; i<len; i++)
	{
		string str = splitIds[i];
		if (str.empty() || str == "" || str.size() == 0){
			cout << i << "---------NULLNULLL--------->" << splitIds[i] << endl;
		}
		else{
			cout << i << "--->" << splitIds[i] << endl;
		}
	}

	string str1 = "ddddd,";
	uint32_t id = 456;
	str1 += ((boost::lexical_cast<string>(id)) + ",");
	cout << str1 << endl;
}

void testNTYGAMEROOMEND()
{
	slither::GameRoomEndNty notifyInfo;

	slither::PBPlayerGameRound* roundInfo;

	roundInfo = notifyInfo.add_gameroundinfo();
	roundInfo->set_playerid(65);
	roundInfo->set_rank(1);
	roundInfo->set_killsnaketimes(33);
	roundInfo->set_totaleatnum(270);
	roundInfo->set_maxmass(5200);

	notifyInfo.set_isvalid(true);
	notifyInfo.set_roomid(1001);



	//roundInfo = notifyInfo.add_gameroundinfo();
	//roundInfo->set_playerid(7);
	//roundInfo->set_rank(16);
	//roundInfo->set_killsnaketimes(51);
	//roundInfo->set_totaleatnum(208);
	//roundInfo->set_maxmass(1800);

	playerStatisticsMgr->savePlayerStatisticsData(notifyInfo);
}


void testSetUseSkin(int playerId,int skinId)
{
	slither::SetUseSkinReq reqInfo;
	reqInfo.set_playerid(playerId);
	reqInfo.set_skinid(skinId);

	playerMsgDeal->playerSetUseSkin(NULL, reqInfo);
}


//void testUnLockSkin(int playerId, int skinId)
//{
//	slither::UnLockSkinReq reqInfo;
//	reqInfo.set_playerid(playerId);
//	reqInfo.set_skinid(skinId);
//
//	playerMsgDeal->playerUnLockSkin(NULL, reqInfo);
//}


void testModifyPlayer(int playerId,bool gender,string name)
{
	slither::ModifyPlayerInfoReq reqInfo;
	reqInfo.set_playerid(playerId);
	reqInfo.set_newgender(gender);
	reqInfo.set_newname(name);


	playerMsgDeal->modifyPlayerInfo(NULL, reqInfo);
}








void testWriteUnLockInfo(string unLockStr, map<int, int>* unLockInfoMap)
{
	if (!unLockStr.empty() && unLockStr.length() > 0)
	{
		vector<string> unLockInfoArr;
		boost::split(unLockInfoArr, unLockStr, boost::is_any_of(";"));

		if (!unLockInfoArr.empty() && unLockInfoArr.size() > 0)
		{
			size_t size = unLockInfoArr.size();
			for (size_t i = 0; i<size; i++)
			{
				string singleUnLockInfoStr = unLockInfoArr[i];
				vector<string> singleUnLockInfoArr;

				boost::split(singleUnLockInfoArr, singleUnLockInfoStr, boost::is_any_of(","));
				if (!singleUnLockInfoArr.empty() && singleUnLockInfoArr.size() == 2)
				{
					int unLockType = boost::lexical_cast<int>(singleUnLockInfoArr[0]);
					int unLockValue = boost::lexical_cast<int>(singleUnLockInfoArr[1]);

					unLockInfoMap->insert(pair<int, int>(unLockType, unLockValue));
				}
			}
		}
	}
}


void testSplitUnLock()
{
	map<int, int> unlockMap;
	string strUnlocType = "1,1;2,1";

	testWriteUnLockInfo(strUnlocType, &unlockMap);


	cout << "unlockMap  size= " << unlockMap.size() << endl;

	map<int, int>::iterator it;
	for (it = unlockMap.begin(); it != unlockMap.end(); ++it)
	{
		cout << "["  << " UNLOCK_TYPE:" << it->first << ",UNLOCK_VALUE:" << it->second << "]" << endl;
	}
}


void testRand(int beginNum, int endNum)
{
	for (size_t i = 0; i < 1000; i++)
	{
		int x = beginNum + rand() % endNum;
		cout << x << endl;

		if (x <beginNum || x >= endNum)
		{
			cout << "------------------------------------------------------------------" << endl;
		}
	}
}










