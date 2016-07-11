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
	string appid;				//	Ӧ�ñ��	1 - 20�ַ���������
	string appvers;				//	Ӧ�ð汾	1 - 32�ַ�
	string token;				//	����	String 32
	string device;				//	�豸	1 - 128�ַ�
	string deviceuuid;			//	�豸Ψһ��	1 - 64�ַ� ios: 7������idfa, 7����mac��׿ : ����advertisingid, ������androidid
	string fun;					//	�ӿڷ�����	��GetInfo�����ϸ�ƥ���Сд
	string os;					//	ϵͳ	1 - 64�ַ�
	string osvers;				//	ϵͳ�汾	1 - 32�ַ�
	uint64_t time;					//	ʱ���	Long�ͣ���ǰ����ʱ���
//	string sign;				//	ǩ��	md5������ֵ������ֵ = md5(����˳������.ֵ + AppSecret).tolower();����������sign
};


class PostRoleCreateData
{
public:
	PostRoleCreateData();
	PostRoleCreateData(string account, string ip, const slither::PBPlayerBase* playerInfo, const slither::VerifyTokenReq& reqInfo);
	map<string, string> getObjQueryMap();
private:
	string account;					//		�����˺�	Varchar(100)
	string appid;					//		Ӧ�ñ��	1 - 20�ַ� �ο�AppID
	string area;					//		�������	Long
	string cpscid;					//		ý����	1 - 32�ַ���CPS�ְ�ý���� �ж� / META - INF / ���Ƿ���� unichannel_xxx�ļ���xxx = cpscid��ѡ�
	string device;					//		�豸	1 - 128�ַ�
	string deviceuuid;				//		�豸Ψһ��	1 - 64�ַ�
	string from;					//		�������	Varchar(64)
	string fun;						//		�ӿڷ�����	��PostRoleCreate�����ϸ�ƥ���Сд
	string ip;						//		Ip��ַ
	string os;						//		ϵͳ	1 - 64�ַ�
	string osvers;					//		ϵͳ�汾	1 - 32�ַ�
	string playerid;				//		��ɫ���	varchar(64)
	string playerlevel;				//		��ɫ�ȼ�	Int
	string role;					//		��ɫ��	varchar(64)
	uint64_t time;					//		ʱ���	Long�ͣ���ǰ����ʱ���

};



class PostRoleLogonData
{
public:
	PostRoleLogonData();
	PostRoleLogonData(string account, string ip, const slither::PBPlayerBase* playerInfo, const slither::VerifyTokenReq& reqInfo);
	map<string, string> getObjQueryMap();
private:
	string account;					//		�����˺�	Varchar(100)
	string appid;					//		Ӧ�ñ��	1 - 20�ַ� �ο�AppID
	string area;					//		�������	Long
	string cpscid;					//		ý����	1 - 32�ַ���CPS�ְ�ý���� �ж� / META - INF / ���Ƿ���� unichannel_xxx�ļ���xxx = cpscid��ѡ�
	string device;					//		�豸	1 - 128�ַ�
	string deviceuuid;				//		�豸Ψһ��	1 - 64�ַ�
	string from;					//		�������	Varchar(64)
	string fun;						//		�ӿڷ�����	��PostRoleCreate�����ϸ�ƥ���Сд
	string ip;						//		Ip��ַ
	string os;						//		ϵͳ	1 - 64�ַ�
	string osvers;					//		ϵͳ�汾	1 - 32�ַ�
	string playerid;				//		��ɫ���	varchar(64)
	string playerlevel;				//		��ɫ�ȼ�	Int
	string role;					//		��ɫ��	varchar(64)
	uint64_t time;					//		ʱ���	Long�ͣ���ǰ����ʱ���

};



#define  lzSDKMgr LzSDKMgr::GetInstance()

#endif