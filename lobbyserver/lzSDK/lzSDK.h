#ifndef LOBBYSERVER_SDK_H
#define LOBBYSERVER_SDK_H

#include "../header.h"
#include "../../protocol/slither_lobby.pb.h"


class LzSDKMgr
{
public:
	static LzSDKMgr* GetInstance()
	{
		static LzSDKMgr instance;
		return &instance;
	}
	~LzSDKMgr();
	string getSDKAccountId(slither::VerifyTokenReq& reqInfo);
	string getQueryStrAndSign(map<string, string> queryParaMap, string appSecret);
	void postRoleCreateToSDK(const string account, const string ip, const slither::PBPlayerBase* playerInfo, const slither::VerifyTokenReq& reqInfo);
	void postRoleLogonToSDK(const string account, const string ip, const slither::PBPlayerBase* playerInfo, const slither::VerifyTokenReq& reqInfo);
private:
	
};



class SDKGetInfoReqInfo
{
public:
	SDKGetInfoReqInfo(slither::VerifyTokenReq& reqInfo);
	map<string, string> getObjQueryMap();
private:
	string appid;				//	应用编号	1 - 20字符，需申请
	string appvers;				//	应用版本	1 - 32字符
	string token;				//	令牌	String 32
	string device;				//	设备	1 - 128字符
	string deviceuuid;			//	设备唯一号	1 - 64字符 ios: 7及以上idfa, 7以下mac安卓 : 存在advertisingid, 不存在androidid
	string fun;					//	接口方法名	“GetInfo”，严格匹配大小写
	string os;					//	系统	1 - 64字符
	string osvers;				//	系统版本	1 - 32字符
	uint64_t time;					//	时间戳	Long型，当前请求时间戳
//	string sign;				//	签名	md5（参数值）参数值 = md5(参数顺序排序.值 + AppSecret).tolower();参数不包含sign
};


class PostRoleCreateData
{
public:
	PostRoleCreateData();
	PostRoleCreateData(string account, string ip, const slither::PBPlayerBase* playerInfo, const slither::VerifyTokenReq& reqInfo);
	map<string, string> getObjQueryMap();
private:
	string account;					//		渠道账号	Varchar(100)
	string appid;					//		应用编号	1 - 20字符 参考AppID
	string area;					//		区服编号	Long
	string cpscid;					//		媒体编号	1 - 32字符，CPS分包媒体编号 判断 / META - INF / 下是否存在 unichannel_xxx文件，xxx = cpscid（选填）
	string device;					//		设备	1 - 128字符
	string deviceuuid;				//		设备唯一号	1 - 64字符
	string from;					//		渠道编号	Varchar(64)
	string fun;						//		接口方法名	“PostRoleCreate”，严格匹配大小写
	string ip;						//		Ip地址
	string os;						//		系统	1 - 64字符
	string osvers;					//		系统版本	1 - 32字符
	string playerid;				//		角色编号	varchar(64)
	string playerlevel;				//		角色等级	Int
	string role;					//		角色名	varchar(64)
	uint64_t time;					//		时间戳	Long型，当前请求时间戳

};



class PostRoleLogonData
{
public:
	PostRoleLogonData();
	PostRoleLogonData(string account, string ip, const slither::PBPlayerBase* playerInfo, const slither::VerifyTokenReq& reqInfo);
	map<string, string> getObjQueryMap();
private:
	string account;					//		渠道账号	Varchar(100)
	string appid;					//		应用编号	1 - 20字符 参考AppID
	string area;					//		区服编号	Long
	string cpscid;					//		媒体编号	1 - 32字符，CPS分包媒体编号 判断 / META - INF / 下是否存在 unichannel_xxx文件，xxx = cpscid（选填）
	string device;					//		设备	1 - 128字符
	string deviceuuid;				//		设备唯一号	1 - 64字符
	string from;					//		渠道编号	Varchar(64)
	string fun;						//		接口方法名	“PostRoleCreate”，严格匹配大小写
	string ip;						//		Ip地址
	string os;						//		系统	1 - 64字符
	string osvers;					//		系统版本	1 - 32字符
	string playerid;				//		角色编号	varchar(64)
	string playerlevel;				//		角色等级	Int
	string role;					//		角色名	varchar(64)
	uint64_t time;					//		时间戳	Long型，当前请求时间戳

};



#define  lzSDKMgr LzSDKMgr::GetInstance()

#endif