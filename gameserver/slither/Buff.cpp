#include "Buff.h"
#include "Factory.h"

namespace slither {
	Buff::Buff() : m_buffType(INVALID_BUFF) {
		m_startTime = boost::posix_time::second_clock::local_time();
	}

	Buff::~Buff() {

	}

	// 刷新buff时间
	void Buff::Refresh() {
		m_startTime = boost::posix_time::second_clock::local_time();
	}

	// buff是否过期
	bool Buff::IsExpire() {
		boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff = end - m_startTime;

		if (10 > (uint32_t)diff.total_seconds()) {
			return true;
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SnakeBuffStat::SnakeBuffStat() : m_buffStat(0) {

	}

	SnakeBuffStat::~SnakeBuffStat() {

	}

	void SnakeBuffStat::AddBuff(BuffType buffType) {
		// 如果之前没有这个buff，那么新增一个buff状态，否则刷新之前的buff状态时间
		if (!HasBuff(buffType)) {
			Buff* pNewBuff = gpFactory->CreateBuff(buffType);
			if (!pNewBuff) {
				return;
			}
			m_buffList.push_back(pNewBuff);
			m_buffStat |= buffType;
			return;
		}

		list<Buff*>::iterator buffIt = m_buffList.begin();
		list<Buff*>::iterator buffItEnd = m_buffList.end();
		for (; buffIt != buffItEnd; buffIt++) {
			Buff* pBuff = *buffIt;
			if (pBuff->GetBuffType() != buffType) {
				continue;
			}
			pBuff->Refresh();
		}

		return;
	}

	void SnakeBuffStat::DelBuff(BuffType buffType) {
		list<Buff*>::iterator buffIt = m_buffList.begin();
		list<Buff*>::iterator buffItEnd = m_buffList.end();
		for (; buffIt != buffItEnd; buffIt++) {
			Buff* pBuff = *buffIt;
			if (pBuff->GetBuffType() != buffType) {
				continue;
			}

			m_buffList.erase(buffIt);
			gpFactory->ReleaseBuff(pBuff);

			// 将标志位清空
			m_buffStat &= ~buffType;
			return;
		}
	}

	bool SnakeBuffStat::HasBuff(BuffType buffType) {
		if ((m_buffStat & buffType) != 0) {
			return true;
		}
		return false;
	}

	void SnakeBuffStat::CheckBuffs() {
		// 删除Buff指针
		list<Buff*>::iterator buffIt = m_buffList.begin();
		list<Buff*>::iterator buffItEnd = m_buffList.end();
		for (; buffIt != buffItEnd;) {
			Buff* pBuff = *buffIt;

			// 如果过期了就删除buff
			if (!pBuff || !pBuff->IsExpire()) {
				buffIt++;
			}

			// 将标志位清空
			BuffType buffType = pBuff->GetBuffType();
			m_buffStat &= ~buffType;

			m_buffList.erase(buffIt++);
			gpFactory->ReleaseBuff(pBuff);
		}
	}

	void SnakeBuffStat::ClearBuffs() {
		m_buffStat = 0;

		// 删除Buff指针
		list<Buff*>::iterator buffIt = m_buffList.begin();
		list<Buff*>::iterator buffItEnd = m_buffList.end();
		for (; buffIt != buffItEnd; ) {
			Buff* pBuff = *buffIt;
			
			m_buffList.erase(buffIt++);
			gpFactory->ReleaseBuff(pBuff);
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SnakeInvincibleBuff::SnakeInvincibleBuff() {
		m_buffType = SNAKE_INVINCIBLE;
	}

	SnakeInvincibleBuff::~SnakeInvincibleBuff() {

	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SnakeMultiPowerBuff::SnakeMultiPowerBuff() {
		m_buffType = SNAKE_MULTI_POWER;
	}

	SnakeMultiPowerBuff::~SnakeMultiPowerBuff() {

	}
}