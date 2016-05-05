#ifndef NETWORK_TIMERTRIGGER
#define NETWORK_TIMERTRIGGER

#include "header.h"
#include "ITimerTrigger.h"

namespace cpnet
{
	class TimerTrigger : public ITimerTrigger
	{
	public:
		typedef TimerTrigger* pointer;

		TimerTrigger(BoostIoService& ioService, BoostStrand& strand)
		{
			m_pIoService = &ioService;
			m_pStrand = &strand;
		}

		virtual BoostTimer* AddCircleTimer(TriggerCallback triggerCallback , size_t nMicroSeconds)
		{
			BoostTimer* timer = new BoostTimer(*m_pIoService, boost::posix_time::millisec(nMicroSeconds));
			m_timerList.push_back(timer);
			m_triggerCallbackMap.insert(make_pair(timer, triggerCallback));
			m_intervalTimeMap.insert(make_pair(timer, nMicroSeconds));
			timer->async_wait(m_pStrand->wrap(boost::bind(&TimerTrigger::HandleCircleTimer, this, boost::asio::placeholders::error, timer)));
			return timer;
		}

		virtual BoostTimer* AddOnceTimer(TriggerCallback triggerOnceCallback, size_t nIntervalTime)
		{
			BoostTimer* timer = new BoostTimer(*m_pIoService, boost::posix_time::seconds(nIntervalTime));
			m_timerList.push_back(timer);
			m_triggerCallbackMap.insert(make_pair(timer, triggerOnceCallback));
			m_intervalTimeMap.insert(make_pair(timer, nIntervalTime));
			timer->async_wait(m_pStrand->wrap(boost::bind(&TimerTrigger::HandleOnceTimer, this, boost::asio::placeholders::error, timer)));
			return timer;
		}

		virtual BoostTimer* ResetOnceTimer(BoostTimer* pTimer, TriggerCallback triggerOnceCallback, size_t nIntervalTime)
		{
			map<BoostTimer*, TriggerCallback>::iterator callbackIt = m_triggerCallbackMap.find(pTimer);
			if (callbackIt != m_triggerCallbackMap.end())
			{
				m_triggerCallbackMap.erase(callbackIt);
			}
			map<BoostTimer*, size_t>::iterator timeIt = m_intervalTimeMap.find(pTimer);
			if (timeIt != m_intervalTimeMap.end())
			{
				m_intervalTimeMap.erase(timeIt);
			}

			if (pTimer)
			{
				delete pTimer;
			}
			
			return AddOnceTimer(triggerOnceCallback, nIntervalTime);
		}

		virtual void RemoveTimer(BoostTimer* pTimer)
		{
			map<BoostTimer*, TriggerCallback>::iterator callbackIt = m_triggerCallbackMap.find(pTimer);
			if (callbackIt != m_triggerCallbackMap.end())
			{
				m_triggerCallbackMap.erase(callbackIt);
			}

			map<BoostTimer*, size_t>::iterator timeIt = m_intervalTimeMap.find(pTimer);
			if (timeIt != m_intervalTimeMap.end())
			{
				m_intervalTimeMap.erase(timeIt);
			}

			vector<BoostTimer*>::iterator timerIt = find(m_timerList.begin(), m_timerList.end(), pTimer);
			if (timerIt != m_timerList.end())
			{
				m_timerList.erase(timerIt);
			}

			if (pTimer)
			{
				delete pTimer;
			}
			return;
		}

		virtual ~TimerTrigger()
		{
			vector<BoostTimer*>::iterator itBegin = m_timerList.begin();
			vector<BoostTimer*>::iterator itEnd = m_timerList.end();
			for (; itBegin != itEnd; ++itBegin)
			{
				if (*itBegin)
				{
					delete *itBegin;
				}
			}
		}

	private:
		void HandleCircleTimer(const boost::system::error_code& errCode, BoostTimer* pTimer)
		{
			// 是否有定时器被取消
			if (errCode)
			{
				return;
			}

			map<BoostTimer*, TriggerCallback>::iterator callbackIt = m_triggerCallbackMap.find(pTimer);
			if (callbackIt == m_triggerCallbackMap.end())
			{
				return;
			}
			map<BoostTimer*, size_t>::iterator timeIt = m_intervalTimeMap.find(pTimer);
			if (timeIt == m_intervalTimeMap.end())
			{
				return;
			}

			TriggerCallback& triggerCallback = callbackIt->second;
			triggerCallback(errCode);
			pTimer->expires_at(pTimer->expires_at() + boost::posix_time::millisec(timeIt->second));
			pTimer->async_wait(m_pStrand->wrap(boost::bind(&TimerTrigger::HandleCircleTimer, this, boost::asio::placeholders::error, pTimer)));
		}

		void HandleOnceTimer(const boost::system::error_code& errCode, BoostTimer* pTimer)
		{
			if (errCode)
			{
				return;
			}

			map<BoostTimer*, TriggerCallback>::iterator callbackIt = m_triggerCallbackMap.find(pTimer);
			if (callbackIt == m_triggerCallbackMap.end())
			{
				return;
			}
			map<BoostTimer*, size_t>::iterator timeIt = m_intervalTimeMap.find(pTimer);
			if (timeIt == m_intervalTimeMap.end())
			{
				return;
			}

			TriggerCallback& triggerCallback = callbackIt->second;
			triggerCallback(errCode);
			if (pTimer)
			{
				delete pTimer;
			}
			m_triggerCallbackMap.erase(callbackIt);
		}

	private:
		BoostIoService* m_pIoService;
		BoostStrand* m_pStrand;

		vector<BoostTimer*> m_timerList;
		map<BoostTimer*, TriggerCallback> m_triggerCallbackMap;
		map<BoostTimer*, size_t> m_intervalTimeMap;
	};
}

#endif