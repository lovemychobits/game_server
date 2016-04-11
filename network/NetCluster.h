#ifndef NETWORK_NETCLUSTER_H
#define NETWORK_NETCLUSTER_H

#include "INetCluster.h"
#include "header.h"
#include "MsgChainer.h"

namespace cpnet
{
	void ReleaseCluster(INetCluster*& pNetCluster)
	{
		if (pNetCluster)
		{
			delete pNetCluster;
			pNetCluster = NULL;
		}
	}

	class NetCluster : public INetCluster
	{
	public:
		static NetCluster* GetInstance()
		{
			if (m_pInstance == NULL)
			{
				m_pInstance = new NetCluster();
			}
			return m_pInstance;
		}


	private:
		NetCluster() : m_strand(m_ioService), m_uThreadNum(0)
		{
		}

	public:
		virtual void SetNetThreadNum(uint32_t uThreadNum);
		virtual IClientSession* CreateClientSession();
		virtual IServerSession* CreateServerSession();
		virtual ITimerTrigger* CreateTimerTrigger();
		virtual IMsgChainer* CreateMsgChainer();
		virtual bool Init();
		virtual void Run();
		virtual void Stop();

	private:
		static NetCluster* m_pInstance;
		BoostIoService m_ioService;
		BoostStrand m_strand;
		vector<boost::shared_ptr<boost::thread> > m_threadGroup;

		uint32_t m_uThreadNum;
		string m_strLogProp;
	};

	NetCluster* NetCluster::m_pInstance = NULL;
}
#endif