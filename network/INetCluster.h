#ifndef NETWORK_INETCLUSTER_H
#define NETWORK_INETCLUSTER_H

/*
	用来代表网络中的客户端，主动对服务器发起连接
*/

namespace cpnet
{
	class ITimerTrigger;
	class IMsgParser;
	class IMsgHandler;
	class IMsgChainer;
	class IClientSession
	{
	public:
		virtual ~IClientSession(){};
		virtual void Connect(const char* pServerIp, uint32_t uPort) = 0;
		virtual void SetMsgChainer(IMsgChainer* pMsgChainer) = 0;
		virtual void SetMsgHandler(IMsgHandler* pMsgHandler) = 0;
		virtual void SetHeadLen(uint32_t uHeadLen) = 0;
		virtual void SendMsg(const char* pBuf, uint32_t uLen) = 0;
		virtual void Stop() = 0;
		virtual bool IsConnected() = 0;
	};

	/*
		用来代表网络中的服务器，监听端口，接受连接
	*/
	class IServerSession
	{
	public:
		virtual ~IServerSession(){};
		virtual void Listen(const char* pHostIp, uint32_t nPort) = 0;
		virtual void SetMsgChainer(IMsgChainer* pMsgChainer) = 0;
		virtual void SetMsgHandler(IMsgHandler* pMsgHandler) = 0;
		virtual void SetHeadLen(uint32_t uHeadLen) = 0;
		virtual void Stop() = 0;
	};

	class INetCluster
	{
	public:
		virtual ~INetCluster(){};
		virtual void SetNetThreadNum(uint32_t uThreadNum) = 0;
		virtual IClientSession* CreateClientSession() = 0;
		virtual IServerSession* CreateServerSession() = 0;
		virtual ITimerTrigger* CreateTimerTrigger() = 0;
		virtual IMsgChainer* CreateMsgChainer() = 0;
		virtual bool Init() = 0;
		virtual void Run() = 0;
		virtual void Stop() = 0;
	};

	INetCluster* CreateNetCluster();
	void ReleaseCluster(INetCluster*& pNetCluster);
}
#endif