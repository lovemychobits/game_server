#ifndef UTILS_UTILITY_HPP
#define UTILS_UTILITY_HPP

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/random.hpp>
#include <string>
#include <set>
using namespace std;

namespace cputil
{
	// range in [uMin, uMax], not [uMin, uMax)
	uint32_t GenRandom(uint32_t uMin, uint32_t uMax);
	float GenFloatRandom(float fMin, float fMax);

	// 生成指定个数的不重复随机数
	void GenUniRandVec(uint32_t uStart, uint32_t uEnd, uint32_t uNum, vector<uint32_t>& randVec);

	// 获取当前系统时间(秒)
	uint64_t GetSysTime();

	// 判断两个时间是否为同一天
	bool IsSameDay(time_t uTime1, time_t uTimes);

	// 获取两个时间之间的天数间隔，如果是同一天为0
	uint32_t GetIntervalDays(time_t uTime1, time_t uTime2);

	// 获取时间结构体
	struct tm GetTimeInfo(time_t uTime);
	
	// 获取日期 年月日(20140828)
	string GetLocalDate();

	// 获取日期 年月日时分秒(20140828T101010)
	string GetLocalTime();

	// 把表示时间字的字符串转换成uint32
	uint32_t StrToTime(const string& strTime);

	// 将时间转换成字符串
	string TimeToStr(uint64_t uTime);
	
	// 把一个字符串中的某个字符串(第一个)替换成另一个字符串
	void StrReplace(string &str, const string& strOld, const string& strNew);
}



#endif