#include "../header.h"
#include "../../utils/Utility.h"
#include "../../utils/MyMd5.h"
#include "../config/ServerConfig.h"
#include "../jsonhelper/JsonHelper.h"
#include "../../utils/BoostHttpUtil.h"
#include "lzSDK.h"


#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


void sendPostToSDK(string ip, string port, string contenxt, string sendPostContent);
string strTolower(string str);
char *url_encode(char *str);
string urlencode(std::string &s);


string LzSDKMgr::getSDKAccountId(slither::VerifyTokenReq& reqInfo)
{
	string accountStr = "";
	SDKGetInfoReqInfo sdkReqInfo = SDKGetInfoReqInfo(reqInfo);
	map<string, string> queryParaMap = sdkReqInfo.getObjQueryMap();

	string appSecret = gpServerConfig->GetSDKInfo().sdkAppSecret;
	string queryParaStr = getQueryStrAndSign(queryParaMap, appSecret);
	
	
	string sdkIp = gpServerConfig->GetSDKInfo().sdkIp;
	string sdkPort = gpServerConfig->GetSDKInfo().sdkPort;
	string context = "/OAuth1/User/GetInfo";

	
	//sendPostToSDK(sdkIp, sdkPort, context, queryParaStr);
	TRACELOG("sdkURL=[" << sdkIp.c_str() << ":" << sdkPort.c_str() << context.c_str() << "]");
	TRACELOG("queryParaStr=[" << queryParaStr.c_str() << "]");

	string reponse_data = "";
	cputil::BoostHttpUtil boastHttpUtil;
	int ret = boastHttpUtil.post(sdkIp, sdkPort, context, queryParaStr, reponse_data);

	TRACELOG("error_code:" << ret);
	TRACELOG("SDK RESP[" << reponse_data.c_str() << "]");

	map<string, string> respJsonMap = jsonHelper::parseSDKGetInfoResp(reponse_data);
	if (!respJsonMap.empty() && respJsonMap.size() > 0)
	{
		string stateStr = respJsonMap["state"];
		if (stateStr != "")
		{
			int stateCode = boost::lexical_cast<int>(stateStr);
			if (stateCode == 0)
			{
				accountStr = respJsonMap["account"];
			}
		}
	}

	return accountStr;
}


void LzSDKMgr::postRoleCreateToSDK(const string account, const string ip, const slither::PBPlayerBase* playerInfo, const slither::VerifyTokenReq& reqInfo)
{
	PostRoleCreateData data = PostRoleCreateData(account, ip, playerInfo, reqInfo);
	map<string, string> queryParaMap = data.getObjQueryMap();

	string appSecret = gpServerConfig->GetSDKInfo().sdkAppSecret;
	string queryParaStr = getQueryStrAndSign(queryParaMap, appSecret);


	string toURL = gpServerConfig->GetSDKInfo().apiURL;
	string sdkPort = gpServerConfig->GetSDKInfo().sdkPort;
	string context = "/OAuth1/Game/PostRoleCreate";


	//sendPostToSDK(sdkIp, sdkPort, context, queryParaStr);
	TRACELOG("sdkURL=[" << toURL.c_str() << ":" << sdkPort.c_str() << context.c_str() << "]");
	TRACELOG("queryParaStr=[" << queryParaStr.c_str() << "]");

	string reponse_data = "";
	cputil::BoostHttpUtil boastHttpUtil;
	int ret = boastHttpUtil.post(toURL, sdkPort, context, queryParaStr, reponse_data);

	TRACELOG("error_code:" << ret);
	TRACELOG("SDK RESP[" << reponse_data.c_str() << "]");
}


void LzSDKMgr::postRoleLogonToSDK(const string account, const string ip, const slither::PBPlayerBase* playerInfo, const slither::VerifyTokenReq& reqInfo)
{
	PostRoleLogonData data = PostRoleLogonData(account, ip, playerInfo, reqInfo);
	map<string, string> queryParaMap = data.getObjQueryMap();

	string appSecret = gpServerConfig->GetSDKInfo().sdkAppSecret;
	string queryParaStr = getQueryStrAndSign(queryParaMap, appSecret);


	string toURL = gpServerConfig->GetSDKInfo().apiURL;
	string sdkPort = gpServerConfig->GetSDKInfo().sdkPort;
	string context = "/OAuth1/Game/PostRoleLogon";


	//sendPostToSDK(sdkIp, sdkPort, context, queryParaStr);
	TRACELOG("sdkURL=[" << toURL.c_str() << ":" << sdkPort.c_str() << context.c_str() << "]");
	TRACELOG("queryParaStr=[" << queryParaStr.c_str() << "]");

	string reponse_data = "";
	cputil::BoostHttpUtil boastHttpUtil;
	int ret = boastHttpUtil.post(toURL, sdkPort, context, queryParaStr, reponse_data);

	TRACELOG("error_code:" << ret);
	TRACELOG("SDK RESP[" << reponse_data.c_str() << "]");
}



/////////////////////////////////SDK UTIL/////////////////////////////////
LzSDKMgr::~LzSDKMgr(){

}

string LzSDKMgr::getQueryStrAndSign(map<string, string> queryParaMap, string appSecret)
{
	string queryStr = "";
	string strForSign = "";
	if (!queryParaMap.empty() && queryParaMap.size() > 0) 
	{
		
		for (map<string, string>::iterator i = queryParaMap.begin(); i != queryParaMap.end(); i++)
		{
			string key = i->first;
			string value = i->second;
			
			string enCodeValueStr = urlencode(value);
			queryStr += (key + "=" + enCodeValueStr + "&");

			strForSign += (value);
		}
	}

	if (!queryStr.empty() && queryStr.length() > 0 && !strForSign.empty() && strForSign.length())
	{
		if (boost::algorithm::ends_with(queryStr,"&"))
		{
			queryStr = queryStr.substr(0, queryStr.length() - 1);
		}

		strForSign += appSecret;
		//string utf8Str = jsonHelper::asiToUtf(strForSign);
		TRACELOG("BEFORE queryStr=[" << strForSign.c_str() << "]");

		string sign = cputil::md5(strForSign);
		boost::algorithm::to_lower(sign);

		TRACELOG("sign=[" << sign.c_str() << "]");
		queryStr += ("&sign=" + sign);
	}

	return queryStr;
}





/* Converts a hex character to its integer value */
char from_hex(char ch) {
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code) {
	static char hex[] = "0123456789abcdef";
	return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(char *str) {
	char *pstr = str, *buf = (char*)malloc(strlen(str) * 3 + 1), *pbuf = buf;
	while (*pstr) {
		if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
			*pbuf++ = *pstr;
		else if (*pstr == ' ')
			*pbuf++ = '+';
		else
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_decode(char *str) {
	char *pstr = str, *buf = (char*)malloc(strlen(str) + 1), *pbuf = buf;
	while (*pstr) {
		if (*pstr == '%') {
			if (pstr[1] && pstr[2]) {
				*pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
				pstr += 2;
			}
		}
		else if (*pstr == '+') {
			*pbuf++ = ' ';
		}
		else {
			*pbuf++ = *pstr;
		}
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}


string urlencode(std::string &s)
{
	static const char lookup[] = "0123456789abcdef";
	stringstream e;
	for (int i = 0, ix = s.length(); i<ix; i++)
	{
		const char& c = s[i];
		if ((48 <= c && c <= 57) ||//0-9
			(65 <= c && c <= 90) ||//abc...xyz
			(97 <= c && c <= 122) || //ABC...XYZ
			(c == '-' || c == '_' || c == '.' || c == '~')
			)
		{
			e << c;
		}
		else
		{
			e << '%';
			e << lookup[(c & 0xF0) >> 4];
			e << lookup[(c & 0x0F)];
		}
	}
	return e.str();
}




/////////////////////////////////////GET INFO/////////////////////////////////

SDKGetInfoReqInfo::SDKGetInfoReqInfo(slither::VerifyTokenReq& reqInfo)
{	
	appvers = reqInfo.appvers();
	token = reqInfo.token();
	device = reqInfo.device();
	deviceuuid = reqInfo.deviceuuid();
	os = reqInfo.os();
	osvers = reqInfo.osvers();

	appid = gpServerConfig->GetSDKInfo().sdkAppId;
	fun = "GetInfo";
	time = cputil::GetSysTime();
}

map<string, string> SDKGetInfoReqInfo::getObjQueryMap(){
	map<string, string> queryMap;
	queryMap.insert(pair<string, string>("appvers", appvers));
	queryMap.insert(pair<string, string>("token", token));
	queryMap.insert(pair<string, string>("device", device));
	queryMap.insert(pair<string, string>("deviceuuid", deviceuuid));
	queryMap.insert(pair<string, string>("os", os));
	queryMap.insert(pair<string, string>("osvers", osvers));

	queryMap.insert(pair<string, string>("appid", appid));
	queryMap.insert(pair<string, string>("fun", fun));
	queryMap.insert(pair<string, string>("time", boost::lexical_cast<string>(time)));

	return queryMap;
}


/////////////////////////
PostRoleCreateData::PostRoleCreateData(string accountStr, string ipStr, const slither::PBPlayerBase* playerInfo, const slither::VerifyTokenReq& reqInfo)
{
	account = accountStr;
	appid = gpServerConfig->GetSDKInfo().sdkAppId;
	area = gpServerConfig->GetClientServerId();
	cpscid = "0";
	device = reqInfo.device();
	deviceuuid = reqInfo.deviceuuid();
	from = "200002";
	fun = "PostRoleCreate";
	ip = ipStr;
	os = reqInfo.os();
	osvers = reqInfo.osvers();
	playerid = boost::lexical_cast<string>(playerInfo->playerid());
	playerlevel = boost::lexical_cast<string>(playerInfo->level());
	role = playerInfo->playername();
	time = cputil::GetSysTime();
}


//参数值 = md5(account + appid + area + cpscid + device + deviceUUID + from + fun + ip + os + osvers + playerid + playerlevel + role + +time + AppSecret).tolower();

map<string, string> PostRoleCreateData::getObjQueryMap(){
	map<string, string> queryMap;
	queryMap.insert(pair<string, string>("account", account));
	queryMap.insert(pair<string, string>("appid", appid));
	queryMap.insert(pair<string, string>("area", area));
	queryMap.insert(pair<string, string>("cpscid", cpscid));
	queryMap.insert(pair<string, string>("device", device));
	queryMap.insert(pair<string, string>("deviceuuid", deviceuuid));
	queryMap.insert(pair<string, string>("from", from));
	queryMap.insert(pair<string, string>("fun", fun));
	queryMap.insert(pair<string, string>("ip", ip));
	queryMap.insert(pair<string, string>("os", os));
	queryMap.insert(pair<string, string>("osvers", osvers));
	queryMap.insert(pair<string, string>("playerid", playerid));
	queryMap.insert(pair<string, string>("playerlevel", playerlevel));
	queryMap.insert(pair<string, string>("role", role));
	queryMap.insert(pair<string, string>("time", boost::lexical_cast<string>(time)));

	return queryMap;
}


PostRoleLogonData::PostRoleLogonData(string accountStr, string ipStr, const slither::PBPlayerBase* playerInfo, const slither::VerifyTokenReq& reqInfo)
{
	account = accountStr;
	appid = gpServerConfig->GetSDKInfo().sdkAppId;
	area = gpServerConfig->GetClientServerId();
	cpscid = "0";
	device = reqInfo.device();
	deviceuuid = reqInfo.deviceuuid();
	from = "200002";
	fun = "PostRoleLogon";
	ip = ipStr;
	os = reqInfo.os();
	osvers = reqInfo.osvers();
	playerid = boost::lexical_cast<string>(playerInfo->playerid());
	playerlevel = boost::lexical_cast<string>(playerInfo->level());
	role = playerInfo->playername();
	time = cputil::GetSysTime();
}


//参数值 = md5(account + appid + area + cpscid + device + deviceUUID + from + fun + ip + os + osvers + playerid + playerlevel + role + time + AppSecret).tolower();
//参数值 = md5(account + appid + area + cpscid + device + deviceUUID + from + fun + ip + os + osvers + playerid + playerlevel + role + time + AppSecret).tolower();

map<string, string> PostRoleLogonData::getObjQueryMap(){
	map<string, string> queryMap;
	queryMap.insert(pair<string, string>("account", account));
	queryMap.insert(pair<string, string>("appid", appid));
	queryMap.insert(pair<string, string>("area", area));
	queryMap.insert(pair<string, string>("cpscid", cpscid));
	queryMap.insert(pair<string, string>("device", device));
	queryMap.insert(pair<string, string>("deviceuuid", deviceuuid));
	queryMap.insert(pair<string, string>("from", from));
	queryMap.insert(pair<string, string>("fun", fun));
	queryMap.insert(pair<string, string>("ip", ip));
	queryMap.insert(pair<string, string>("os", os));
	queryMap.insert(pair<string, string>("osvers", osvers));
	queryMap.insert(pair<string, string>("playerid", playerid));
	queryMap.insert(pair<string, string>("playerlevel", playerlevel));
	queryMap.insert(pair<string, string>("role", role));
	queryMap.insert(pair<string, string>("time", boost::lexical_cast<string>(time)));

	return queryMap;
}











































