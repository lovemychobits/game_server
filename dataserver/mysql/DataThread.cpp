#include "../header.h"
#include "DataThread.h"
#include "../config/ServerConfig.h"

DataThread::DataThread() : m_bRun(true)
{
	
}

DataThread::~DataThread()
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	return;
}

bool DataThread::Init()
{
	m_playerDataMng.SetMysqlInfo(gpServerConfig->GetMysqlIp().c_str(), gpServerConfig->GetMysqlPort(), gpServerConfig->GetMysqlUser().c_str(), gpServerConfig->GetMysqlPwd().c_str(), gpServerConfig->GetMysqlDB().c_str());
	if (!m_playerDataMng.Init())
	{
		return false;
	}
	return true;
}

void DataThread::Push(const char* pBuf, int nLen)
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_msgQueue.push_back(string(pBuf, nLen));
	m_cond.notify_one();
}

void DataThread::Stop()
{
	m_bRun = false;
	Push("stop", strlen("stop"));
}

void DataThread::Run()
{
	while (m_bRun)
	{
		string strMsg;
		{
			boost::lock_guard<boost::mutex> lock(m_mutex);
			while (m_msgQueue.empty())
			{
				m_cond.wait(m_mutex);
			}

			strMsg = m_msgQueue.front();
			m_msgQueue.pop_front();
		}

		uint32_t uMsgSize = strMsg.size();
		if (uMsgSize == 0 || !m_bRun)
		{
			ERRORLOG("error message info!!");
			break;
		}

		MessageHeader* pMsgHeader = (MessageHeader*)strMsg.c_str();
		switch (pMsgHeader->uMsgCmd)
		{
			// TODO
		case 0:
			break;
		default:
			break;
		}
	}
}