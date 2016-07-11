#ifndef GAMESERVER_BUFF_H
#define GAMESERVER_BUFF_H

#include "../header.h"

namespace slither {

	enum BuffType
	{
		INVALID_BUFF = 0x0,											// 无效的buff
		SNAKE_INVINCIBLE = 0x1,										// 无敌状态（	暗物质）
		SNAKE_MULTI_POWER = 0x10,									// 多倍能量（小恒星）
	};

	class Buff 
	{
	public:
		Buff();
		virtual ~Buff();

		void Refresh();												// 刷新时间
		bool IsExpire();											// 是否过期失效了
		BuffType GetBuffType() {
			return m_buffType;
		}

	protected:
		boost::posix_time::ptime m_startTime;						// buff开始时间
		BuffType m_buffType;									// buff类型
	};

	// 蛇当前综合的buff状态
	class SnakeBuffStat {
	public:
		SnakeBuffStat();
		~SnakeBuffStat();

		void AddBuff(BuffType buffType);
		void DelBuff(BuffType buffType);
		bool HasBuff(BuffType buffType);
		void CheckBuffs();
		void ClearBuffs();

	private:
		uint64_t m_buffStat;										// 最多可以同时拥有64个buff
		list<Buff*> m_buffList;										// buff列表
	};

	// 无敌状态buff
	class SnakeInvincibleBuff : public Buff {
	public:
		SnakeInvincibleBuff();
		virtual ~SnakeInvincibleBuff();
	};

	// 多倍能量buff
	class SnakeMultiPowerBuff : public Buff {
	public:
		SnakeMultiPowerBuff();
		virtual ~SnakeMultiPowerBuff();
	};
}

#endif