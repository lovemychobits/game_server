#ifndef GAMESERVER_BUFF_H
#define GAMESERVER_BUFF_H

#include "../header.h"

namespace slither {

	enum BuffType
	{
		INVALID_BUFF = 0x0,											// ��Ч��buff
		SNAKE_INVINCIBLE = 0x1,										// �޵�״̬��	�����ʣ�
		SNAKE_MULTI_POWER = 0x10,									// �౶������С���ǣ�
	};

	class Buff 
	{
	public:
		Buff();
		virtual ~Buff();

		void Refresh();												// ˢ��ʱ��
		bool IsExpire();											// �Ƿ����ʧЧ��
		BuffType GetBuffType() {
			return m_buffType;
		}

	protected:
		boost::posix_time::ptime m_startTime;						// buff��ʼʱ��
		BuffType m_buffType;									// buff����
	};

	// �ߵ�ǰ�ۺϵ�buff״̬
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
		uint64_t m_buffStat;										// ������ͬʱӵ��64��buff
		list<Buff*> m_buffList;										// buff�б�
	};

	// �޵�״̬buff
	class SnakeInvincibleBuff : public Buff {
	public:
		SnakeInvincibleBuff();
		virtual ~SnakeInvincibleBuff();
	};

	// �౶����buff
	class SnakeMultiPowerBuff : public Buff {
	public:
		SnakeMultiPowerBuff();
		virtual ~SnakeMultiPowerBuff();
	};
}

#endif