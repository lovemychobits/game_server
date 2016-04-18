#ifndef GAMESERVER_CLIENTHANDLER_H
#define GAMESERVER_CLIENTHANDLER_H

#include "../../network/header.h"
#include "../../network/IMsgParser.h"
#include "../../network/IConnection.h"
#include "../../network/IMsgHandler.h"
using namespace cpnet;
/*
	处理从客户端(玩家)发送过来的请求
*/
class ClientHandler : public IMsgHandler
{
public:
	ClientHandler()
	{
		_RegisterAllCmd();								// 注册所有命令
	}
	virtual ~ClientHandler()
	{
		m_cmdFuncMap.clear();
	}

	void HandleConnect(IConnection* pConn);

	void HandleDisconnect(IConnection* pConn, const BoostErrCode& error);

	void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);

	void HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen);

private:
	typedef void (ClientHandler::*pCmdFunc)(IConnection*, MessageHeader*);
	void _RegisterCmd(uint32_t uCmdId, pCmdFunc cmdFunc);
	void _RegisterAllCmd();

private:
	// 网络基本功能测试	
	void _RequestTestPingPong(IConnection* pConn, MessageHeader* pMsgHeader);
	
	// 玩家数据相关操作
	void _RequestPlayerMngProc(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestCreateActor(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestSelectActor(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestNewNotify(IConnection* pConn, MessageHeader* pMsgHeader);

	// GM指令相关
	void _RequestGMCommand(IConnection* pConn, MessageHeader* pMsgHeader);

	// 充值相关
	void _RequestPayOrder(IConnection* pConn, MessageHeader* pMsgHeader);

	// 关卡相关操作
	void _RequestEnterGate(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestFinishGate(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestSweepGate(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestMapAllStarPrize(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestOpenGateCard(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestResetNormalGate(IConnection* pConn, MessageHeader* pMsgHeader);

	// 英雄相关操作
	void _RequestHeroDressRune(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestHeroUpgradeRank(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestHeroUpgradeStar(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestHeroUpgradeSkillLevel(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestHeroUseExp(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestCompoundRune(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestSetHeroLineup(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestCallHero(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestGetHeroList(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestBuySkillPoint(IConnection* pConn, MessageHeader* pMsgHeader);
		
	//  抽奖相关
	void _RequestDrawPrize(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestDrawInfo(IConnection* pConn, MessageHeader* pMsgHeader);
	
	// 体力相关
	void _RequestUpdateAp(IConnection* pConn, MessageHeader* pMsgHeader);

	// 购买体力、金币、钻石相关
	void _RequestBuyCurrency(IConnection* pConn, MessageHeader* pMsgHeader);

	// 商店相关
	void _RequestStoreInfo(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestBuyGoods(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestSaleGoods(IConnection* pConn, MessageHeader* pMsgHeader);

	// 邮件相关
	void _RequestReadMail(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestNewMail(IConnection* pConn, MessageHeader* pMsgHeader);

	// 秘术相关
	void _RequestActiveArcane(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestUpgradeArcane(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestUpgradeArcaneRune(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestUpgradeArcaneSkill(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestActiveArcaneSkill(IConnection* pConn, MessageHeader* pMsgHeader);

	// 竞技场相关
	void _RequestRankArenaInfo(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestArenaRival(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestFightArenaRival(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestArenaFightEnd(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestGetArenaPoint(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestArenaRevenge(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestRevengeFightInfo(IConnection* pConn, MessageHeader* pMsgHeader);

	// 积分竞技场
	void _RequestScoreArenaInfo(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestScoreArenaRival(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestFightScoreArenaRival(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestScoreArenaFightEnd(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestScoreArenaTopic(IConnection* pConn, MessageHeader* pMsgHeader);

	// 排行榜相关
	void _RequestRankList(IConnection* pConn, MessageHeader* pMsgHeader);

	// 签到相关
	void _RequestSignInInfo(IConnection* pConn, MessageHeader* pMsgHeader);				// 请求签到的信息
	void _RequestSignIn(IConnection* pConn, MessageHeader* pMsgHeader);					// 请求签到

	// 战报分享
	void _RequestShareFightResult(IConnection* pConn, MessageHeader* pMsgHeader);

	// 聊天相关
	void _RequestTalk(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestChatMessage(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestBlackListAvatar(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestAddChatBlackList(IConnection* pCOnn, MessageHeader* pMsgHeader);

	//任务相关
	void _RequestDailyTaskInfo(IConnection* pConn, MessageHeader* pMsgHeader);		// 请求每日任务信息
	void _RequestAchievementInfo(IConnection* pConn, MessageHeader* pMsgHeader);	// 请求成就信息
	void _RequestTaskGetReward(IConnection* pConn, MessageHeader* pMsgHeader);			// 请求 成就任务/总成就奖励/每日任务  领取奖励

	// 新手引导相关
	void _RequestGuideFinish(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestSetBeginnerState(IConnection* pConn, MessageHeader* pMsgHeader);

	//水晶迷宫
	void _RequestCrystalMazeInfo(IConnection* pConn, MessageHeader* pMsgHeader);		//请求水晶迷宫的信息
	void _RequestSweepCrystalMaze(IConnection* pConn, MessageHeader* pMsgHeader);		//请求扫荡水晶迷宫
	void _RequestEnterCrystalMaze(IConnection* pConn, MessageHeader* pMsgHeader);		//请求进入水晶迷宫楼层
	void _RequestFinishCrystalMaze(IConnection* pConn, MessageHeader* pMsgHeader);		//请求完成水晶迷宫楼层
	void _RequestResetCrystalMaze(IConnection* pConn, MessageHeader* pMsgHeader);		//请求重置水晶迷宫
	void _RequestDrawCrystalMaze(IConnection* pConn, MessageHeader* pMsgHeader);		//请求水晶迷宫抽奖

	//公会相关
	void _RequestSociatyInfo(IConnection* pConn, MessageHeader* pMsgHeader);			//请求公会信息 有公会就给自己公会信息
	void _RequestSociaty(IConnection* pConn, MessageHeader* pMsgHeader);				//没有公会请求20个公会的信息
	void _RequestCreatSociaty(IConnection* pConn, MessageHeader* pMsgHeader);			//请求创建公会
	void _RequestDissolutionSociaty(IConnection* pConn, MessageHeader* pMsgHeader);		//请求解散公会	
	void _RequestQuerySociaty(IConnection* pConn, MessageHeader* pMsgHeader);			//请求查询公会
	void _RequestApplyAddSociatyInfo(IConnection* pConn, MessageHeader* pMsgHeader);	//请求申请加入公会
	void _RequestOutSociatyInfo(IConnection* pConn, MessageHeader* pMsgHeader);			//请求退出公会
	void _RequestApprovalAddSociatyInfo(IConnection* pConn, MessageHeader* pMsgHeader);	//批准加入公会  1.批准一个 2.全部批准 3.全部忽略
	void _RequestChangePosition(IConnection* pConn, MessageHeader* pMsgHeader);			//更改公会职务
	void _RequestModifyValues(IConnection* pConn, MessageHeader* pMsgHeader);			//更改公会 1.公告 2.加入等级 3.公会加入方式 4.修改公会图标
	void _RequestPray(IConnection* pConn, MessageHeader* pMsgHeader);					//公会祈祷 1.第一个祈祷 2.第二个祈祷 3.第三个祈祷	 	
	void _RequestSociatyGateInfo(IConnection* pConn, MessageHeader* pMsgHeader);	    //请求公会副本信息
	void _RequestSociatyGateRankIng(IConnection* pConn, MessageHeader* pMsgHeader);		//请求公会副本的排名信息

	//好友相关
	void _RequestFriendInfo(IConnection* pConn, MessageHeader* pMsgHeader);				//请求好友信息
	void _RequestApplyAddFriend(IConnection* pConn, MessageHeader* pMsgHeader);			//玩家申请加好友
	void _RequestDelFriendInfo(IConnection* pConn, MessageHeader* pMsgHeader);			//玩家删除好友
	void _RequestDelFriendApplyInfo(IConnection* pConn, MessageHeader* pMsgHeader);		//玩家删除申请列表好友
	void _RequestAddFriend(IConnection* pConn, MessageHeader* pMsgHeader);				//玩家添加好友
	void _RequestGiveFriendAp(IConnection* pConn, MessageHeader* pMsgHeader);			//给好友送体力

	// 查看玩家信息
	void _RequestViewPlayerInfo(IConnection* pConn, MessageHeader* pMsgHeader);

	// wifi PVP对战相关
	void _RequestRegClientWifi(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestSameWifiPlayers(IConnection* pConn, MessageHeader* pMsgHeader);

	// 英雄装备相关操作
	void _RequestStrengthenEquip(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestUpgradeEquip(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestMountGemToEquip(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestUnmountGemFromEquip(IConnection* pConn, MessageHeader* pMsgHeader);

	// 宝石合成相关
	void _RequestCompoundGem(IConnection* pConn, MessageHeader* pMsgHeader);			// 请求合成宝石（五合一）
	void _RequestRandomCompoundGem(IConnection* pConn, MessageHeader* pMsgHeader);		// 请求随机合成宝石（二合一）
	void _RequestAutoCompoundGem(IConnection* pConn, MessageHeader* pMsgHeader);		// 请求自动合成宝石


	//新试炼相关
	void _RequestTrialGateInfo(IConnection* pConn, MessageHeader* pMsgHeader);			// 请求新试炼信息
	void _RequestEnterTrialGate(IConnection* pConn, MessageHeader* pMsgHeader);			// 玩家请求开启某个关卡
	void _RequestEnterTrialWave(IConnection* pConn, MessageHeader* pMsgHeader);			// 玩家请求进入某波试炼
	void _RequestFinishTrialWave(IConnection* pConn, MessageHeader* pMsgHeader);		// 玩家请求完成某波试炼
	void _RequestSweepTrialGate(IConnection* pConn, MessageHeader* pMsgHeader);			// 玩家请求扫荡新试炼关卡
	
	// 时光之穴相关
	void _RequestCaveDeadInfo(IConnection* pConn, MessageHeader* pMsgHeader);			//请求时光之穴信息
	void _RequestEnterCaveDeadGate(IConnection* pConn, MessageHeader* pMsgHeader);		//请求进入时光之穴关卡
	void _RequestFinishCaveDeadGate(IConnection* pConn, MessageHeader* pMsgHeader);		//请求结束时光之穴关卡

private:
	template<typename T>
	void _SendPlayerNotFound(IConnection* pConn, T& response, uint32_t uMsgCmd)
	{
		response.set_errcode(ERROR_PLAYER_NOT_FOUND);
		string strMessage;
		BuildResponseProto(response, strMessage, uMsgCmd);

		pConn->SendMsg(strMessage.c_str(), strMessage.size());
		return;
	}

	template<typename T>
	void _SendPlayerNoAccess(IConnection* pConn, T& response, uint32_t uErrcode, uint32_t uMsgCmd)
	{
		response.set_errcode(uErrcode);
		string strMessage;
		BuildResponseProto(response, strMessage, uMsgCmd);

		pConn->SendMsg(strMessage.c_str(), strMessage.size());
		return;
	}

private:
	map<uint32_t, pCmdFunc> m_cmdFuncMap;
};

#endif