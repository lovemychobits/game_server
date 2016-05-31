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
		float m_fInitRadius;										// 蛇初始半径
		float m_fBodyInterval;										// 蛇身间距比例
		float m_fSpeed;												// 蛇的速度
		float m_fSpeedUpRate;										// 加速速度提升比例
		float m_fSpawnInterval;										// 加速时候生成食物的间隔
		uint32_t m_uGrowUpValue;									// 增加身体节点需要的值
		float m_fSpeedUpNeedValue;									// 可以启动加速的值（除去蛇身基本值）
		float m_fAttenuationValue;									// 加速衰减值
		float m_fAttenuationInterval;								// 加速衰减间隔
		uint32_t m_uSpeedUpGenFoodValue;							// 加速生成食物的值
		float m_fStopSpeedUpValue;									// 加速停止值
		uint32_t m_uInitSnakeBodySize;								// 初始蛇长
		uint32_t m_uInitSnakeMass;									// 初始蛇的重量
		float m_fNodeSqt;											// 节点成长系数
		uint32_t m_uMaxBodySize;									// 最多节点数
		uint32_t m_uInitDeadFoodSize;								// 初始死亡生成的食物数量
		uint32_t m_uDeadFoodIncValue;								// 死亡增加的食物数量间隔
		float m_fIncRadiusValue;									// 蛇身半径增粗值
		uint32_t m_uIncRadiusIntervalValue;							// 蛇身增粗需要的值的间隔
		float m_fRadiusSqt;											// 半径成长系数
		float m_fMaxRadius;											// 最大半径
		float m_fEatFoodRadius;										// 吃球半径
		uint32_t m_uGenFoodMinValue;								// 生成食物的最低值
		uint32_t m_uGenFoodMaxValue;								// 生成食物的最高值
		float m_fRefreshGenFoodInterval;							// 刷新食物时间间隔
		float m_fRefreshFoodDensity;								// 刷新密度
		float m_fRefreshRankingListInterval;						// 刷新排行榜时间
		uint32_t m_uGameRoundTime;									// 一局时间
		uint32_t m_uMapLength;										// 地图宽度
		uint32_t m_uGridSize;										// 每个格子的长度
	};

#define gpSlitherConf SlitherConfig::GetInstance()
}

#endif