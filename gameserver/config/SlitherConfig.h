#ifndef GAMESERVER_SLITHERCONFIG_H
#define GAMESERVER_SLITHERCONFIG_H

#include "../header.h"

namespace slither {
	class SlitherConfig
	{
	public:
		SlitherConfig();
		~SlitherConfig();

		bool LoadSnakeConf(const char* pFilePath);
		bool LoadMapConf(const char* pFilePath);
		bool LoadRoomConf(const char* pFilePath);

	public:
		float m_fInitRadius;										// �߳�ʼ�뾶
		float m_fBodyInterval;										// ���������
		float m_fSpeed;												// �ߵ��ٶ�
		float m_fSpeedUpRate;										// �����ٶ���������
		float m_fSpawnInterval;										// ����ʱ������ʳ��ļ��
		float m_fGrowUpValue;										// ��������ڵ���Ҫ��ֵ
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
		uint32_t m_uDeadFoodValue;									// ����ʱ���ɵ�ʳ������ֵ
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
		uint32_t m_uSimInterval;									// ������ģ��������λ�����룩
		float m_fCollideProportion;									// ��ײ����
		uint32_t m_uInitFoodNum;									// ��ʼʳ������
		uint32_t m_uRefreshFoodThreshold;							// ˢ��ʳ�����ֵ
		uint32_t m_uRefreshFoodNum;									// ˢ��ʳ�������
		uint32_t m_uInitViewRange;									// ��ʼ��Ұ��Χ
		float m_fViewAttenuation;									// ��Ұ˥��ֵ
		uint32_t m_uSnakeSimTimes;									// ��ÿ��ģ�����

		// �����������
		uint32_t m_uRoomNum;										// ��������
		uint32_t m_uRoomSnakeLimit;									// �������ߵ���������
		uint32_t m_uRefuseEnterTime;								// �ܾ�����ʱ��
		uint32_t m_uInvalidNum;										// �����������Ϸ�������
		uint32_t m_uRobotNum;										// ����������
		uint32_t m_uRobotMass;										// ���������������

		// ʳ�����ɲ���
		float m_fInsideScale;										// �ڲ㷶Χ
		float m_fMiddleScale;										// �в㷶Χ
		float m_fInsideGenScale;									// �ڲ�����ʳ�����
		float m_fMiddleGenScale;									// �в�����ʳ�����
	};

	extern SlitherConfig g_slitherConfig;
//#define gpSlitherConf SlitherConfig::GetInstance()
}

#endif