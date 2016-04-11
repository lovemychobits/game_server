#ifndef NETWORK_ITIMERTRIGGER_H
#define NETWORK_ITIMERTRIGGER_H

#include "header.h"

namespace cpnet
{
	class ITimerTrigger
	{
	public:
		virtual ~ITimerTrigger(){};
		virtual BoostTimer* AddCircleTimer(TriggerCallback triggerCallback, size_t nIntervalTime) = 0;
		virtual BoostTimer* AddOnceTimer(TriggerCallback triggerOnceCallback, size_t nIntervalTime) = 0;
		virtual BoostTimer* ResetOnceTimer(BoostTimer* pTimer, TriggerCallback triggerOnceCallback, size_t nIntervalTime) = 0;
		virtual void RemoveTimer(BoostTimer* pTimer) = 0;
	};
}

#endif