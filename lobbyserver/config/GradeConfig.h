#ifndef LOBBYSERVER_GRADECONFIG_H
#define LOBBYSERVER_GRADECONFIG_H

#include "../header.h"
#include "../../tinyxml/tinyxml.h"
#include <boost/lexical_cast.hpp>

//<type Level = "1" Now_Stars = "3" Total_Stars = "3" Name = "ÇàÍ­" Image = "µØÖ·" R1 = "1" R2 = "1" R3 = "1" R4_6 = "1" R7_10 = "1" R11_15 = "1" R16_20 = "1" R21_30 = "1" R31_50 = "1" / >
struct GradeInfo
{
	int levelId;
	int nowStars;
	int totalStars;
	string name;
	string image;
	int r1;
	int r2;
	int r3;
	int r4_6;
	int r7_10;
	int r11_15;
	int r16_20;
	int r21_30;
	int r31_50;
};


class GradeConfig
{
public:
	GradeConfig();
	~GradeConfig();

	static GradeConfig* GetInstance()
	{
		static GradeConfig instance;
		return &instance;
	}

	bool LoadGradeConf(const char* pFilePath);
	int getGradeByTotalStar(const int totalStar);
	int getGradeMaxStarByGrade(const int grade);
	GradeInfo getGradeInfoByGrade(const int grade);
	int addStarByGradeAndRank(const int grade,const int rank);

	int getMaxTotalStar(){
		return maxTotalStar;
	}




private:
	TiXmlDocument m_xmlDoc;

	vector<GradeInfo> grades;
	map<int, GradeInfo> gradesMapByLevelId;
	int getAddNumFromInfoByRank(const GradeInfo& grade, const int rank);
	int maxTotalStar;
};









#define gradeConfig GradeConfig::GetInstance()


#endif