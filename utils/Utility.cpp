#include "Utility.h"
#include <time.h>
#include <stdio.h>

namespace cputil
{
	uint32_t GenRandom(uint32_t uMin, uint32_t uMax)
	{
		static boost::random::mt19937 rng((uint32_t)std::time(0)); 
		boost::random::uniform_int_distribution<> randGen(uMin, uMax);
		return randGen(rng);
	}

	void GenUniRandVec(uint32_t uStart, uint32_t uEnd, uint32_t uNum, vector<uint32_t>& randVec)
	{
		uint32_t uOrignSize = uEnd - uStart + 1;
		if (uNum > uOrignSize + 1 || uNum == 0)					// 超过能随机的范围, 或者不需要随机
		{
			return;
		}

		/*
			如果需要不重复的随机数和数据源的比列比较接近，那么采用数组下标置换算法
			如果需要不重复的随机数占数据源的比列比较少，那么采用随机碰撞算法
			如果数据源本身数据在100以内，并且随机比列低于0.2，那么直接只用数组下标置换算法
		*/

		// 随机碰撞方式
		if ((uOrignSize > 100 && uOrignSize * 0.3 >= uNum) || (uOrignSize <= 100 && uOrignSize * 0.2 >= uNum))
		{
			do 
			{
				uint32_t uRandIndex = GenRandom(uStart, uEnd);

				// 查找是否已经存在了
				bool bFind = false;
				vector<uint32_t>::iterator randIt = randVec.begin();
				vector<uint32_t>::iterator randItEnd = randVec.end();
				for (; randIt != randItEnd; randIt++)
				{
					if (uRandIndex == *randIt)
					{
						bFind = true;
					}
				}

				if (!bFind)
				{
					randVec.push_back(uRandIndex);
				}

			} while (randVec.size() < uNum);
			
			return;
		}

		// 采用数组下标置换算法，需要先复制一次数据
		vector<uint32_t> tmpVec;
		for (uint32_t uRank=uStart; uRank<=uEnd; uRank++)
		{
			tmpVec.push_back(uRank);
		}
		/*
			采用数组下标置换的方式，传入的orignVec需要是指定随机范围内数字排序好之后的数组
			如果获得了指定数量的随机值后，算法就终止
		*/
		uint32_t uRandCount = 0;
		for (uint32_t i=0; i<uOrignSize; i++)
		{
			if (uRandCount == uNum)
			{
				break;
			}

			// 在min和max之间随机一个数字
			uint32_t uRandIndex = GenRandom(i, uOrignSize-1);

			// 置换元素下标
			std::swap(tmpVec[i], tmpVec[uRandIndex]);
			randVec.push_back(tmpVec[i]);
			//if (tmpVec[i] == 0)
			//{
			//	continue;
			//}
			++uRandCount;
		}

		return;
	}

	uint64_t GetSysTime()
	{
		time_t t = time(0);
		return t;
	}

	bool IsSameDay(time_t uTime1, time_t uTime2)
	{
#ifndef _MSC_VER
		struct tm tm1 = *localtime(&uTime1);
		struct tm tm2 = *localtime(&uTime2);
#else
		struct tm tm1; 
		localtime_s(&tm1, &uTime1);					// windows下localtime的安全版本，否则会有warning
		struct tm tm2;
		localtime_s(&tm2, &uTime2);
#endif
		if (tm1.tm_mday == tm2.tm_mday 
			&& tm1.tm_mon == tm2.tm_mon 
			&& tm1.tm_year == tm2.tm_year)
		{
			return true;
		}

		return false;
	}

	uint32_t GetIntervalDays(time_t uTime1, time_t uTime2)
	{
		uint32_t uDuration = (uint32_t)std::abs(uTime1 - uTime2);
		uint32_t uIntervalDays = uDuration / (24 * 60 * 60);
		return uIntervalDays;
	}

	struct tm GetTimeInfo(time_t uTime)
	{
#ifndef _MSC_VER
		struct tm t = *localtime(&uTime);
#else
		struct tm t; 
		localtime_s(&t, &uTime);					// windows下localtime的安全版本，否则会有warning
#endif
		return t;
	}

	string GetLocalDate()
	{
		string strTime = boost::gregorian::to_iso_string(boost::gregorian::day_clock::local_day());
		return strTime;
	}

	// 获取日期 年月日时分秒(20140828T101010)
	string GetLocalTime()
	{
		return boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
	}

	uint32_t StrToTime(const string& strTime)
	{
		struct tm tb;
		boost::posix_time::ptime pt = boost::posix_time::from_iso_string(strTime);
		tb = boost::posix_time::to_tm(pt);
		return static_cast<uint32_t>(mktime(&tb));
	}

	string TimeToStr(uint64_t uTime)
	{
		boost::posix_time::ptime pt = boost::posix_time::from_time_t(uTime);
		return boost::posix_time::to_iso_string(pt);
	}

	void StrReplace(string &str, const string& strOld, const string& strNew)
	{
		uint32_t uPos = 0;
		uint32_t uStartIndex = 0;
		uint32_t uOldLen = strOld.size();
		uint32_t uNewLen = strNew.size();
		while((uPos = str.find(strOld, uStartIndex)) != string::npos)
		{
			str.replace(uPos, uOldLen, strNew);
			uStartIndex = uPos + uNewLen;
		}
		return;
	}
}
