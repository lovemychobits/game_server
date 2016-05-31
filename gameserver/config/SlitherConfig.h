#ifndef GAMESERVER_SLITHERCONFIG_H
#define GAMESERVER_SLITHERCONFIG_H

#include "../header.h"

namespace slither {
	class SlitherConfig
	{
	public:
		~SlitherConfig();
		static SlitherConfig* GetInstance() {
			static SlitherConfig instance;
			return &instance;
		}

		bool LoadSlitherConf(const char* pFilePath);
		bool LoadSlitherConf2(const char* pFilePath);

	private:
		SlitherConfig();

	public:
		float m_fInitRadius;										// �߳�ʼ�뾶
		float m_fBodyInterval;										// ���������
		float m_fSpeed;												// �ߵ��ٶ�
		float m_fSpeedUpRate;										// �����ٶ���������
		float m_fSpawnInterval;										// ����ʱ������ʳ��ļ��
		uint32_t m_uGrowUpValue;									// ��������ڵ���Ҫ��ֵ
		float m_fSpeedUpNeedValue;									// �����������ٵ�ֵ����ȥ�������ֵ��
		float m_fAttenuationValue;									// ����˥��ֵ
		float m_fAttenuationInterval;								// ����˥�����
		uint32_t m_uSpeedUpGenFoodValue;							// ��������ʳ���ֵ
		float m_fStopSpeedUpValue;									// ����ֵֹͣ
		uint32_t m_uInitSnakeBodySize;								// ��ʼ�߳�
		uint32_t m_uInitSnakeMass;									// ��ʼ�ߵ�����
		float m_fNodeSqt;											// �ڵ�ɳ�ϵ��
		uint32_t m_uMaxBodySize;									// ���ڵ���
		uint32_t m_uInitDeadFoodSize;								// ��ʼ�������ɵ�ʳ������
		uint32_t m_uDeadFoodIncValue;								// �������ӵ�ʳ���������
		float m_fIncRadiusValue;									// ����뾶����ֵ
		uint32_t m_uIncRadiusIntervalValue;							// ����������Ҫ��ֵ�ļ��
		float m_fRadiusSqt;											// �뾶�ɳ�ϵ��
		float m_fMaxRadius;											// ���뾶
		float m_fEatFoodRadius;										// ����뾶
		uint32_t m_uGenFoodMinValue;								// ����ʳ������ֵ
		uint32_t m_uGenFoodMaxValue;								// ����ʳ������ֵ
		float m_fRefreshGenFoodInterval;							// ˢ��ʳ��ʱ����
		float m_fRefreshFoodDensity;								// ˢ���ܶ�
		float m_fRefreshRankingListInterval;						// ˢ�����а�ʱ��
		uint32_t m_uGameRoundTime;									// һ��ʱ��
		uint32_t m_uMapLength;										// ��ͼ���
		uint32_t m_uGridSize;										// ÿ�����ӵĳ���
	};

#define gpSlitherConf SlitherConfig::GetInstance()
}

#endif