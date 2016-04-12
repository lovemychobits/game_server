#include "GsConnectionMng.h"

void GsConnectionMng::SetGsConnection(IConnection* pGsConnection, uint32_t uGsIndex/* =0 */)
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	if (uGsIndex >= m_gsConnectionList.size())
	{
		m_gsConnectionList.push_back(pGsConnection);
		return;
	}
	m_gsConnectionList[uGsIndex] = pGsConnection;
}

void GsConnectionMng::SendMsgToGS(const char* pBuf, uint32_t uSize, uint32_t uGsIndex/* =0 */)
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	if (uGsIndex >= m_gsConnectionList.size())
	{
		ERRORLOG("cannot find gs connection for index=[" << uGsIndex << "]");
		return;
	}
	IConnection* pGsConnection = m_gsConnectionList.at(uGsIndex);
	if (!pGsConnection || !pGsConnection->IsConnected())
	{
		ERRORLOG("gs[" << uGsIndex << "] connection is unavailable");
		return;
	}
	pGsConnection->SendMsg(pBuf, uSize);
	return;
}