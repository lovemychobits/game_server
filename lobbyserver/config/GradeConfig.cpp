#include "GradeConfig.h"


GradeConfig::GradeConfig()
{
}

GradeConfig::~GradeConfig()
{
}


bool GradeConfig::LoadGradeConf(const char* pFilePath)
{
	if (!m_xmlDoc.LoadFile(pFilePath))
	{
		cout << "not find LoadGradeConf File" << endl;
		return false;
	}


	TiXmlElement* pRoot = m_xmlDoc.RootElement();
	if (!pRoot)
	{
		return false;
	}


	//<type Level = "1" Now_Stars = "3" Total_Stars = "3" Name = "ÇàÍ­" Image = "µØÖ·" R1 = "1" R2 = "1" R3 = "1" R4_6 = "1" R7_10 = "1" R11_15 = "1" R16_20 = "1" R21_30 = "1" R31_50 = "1" / >
	TiXmlElement* gradeInfoNodeInfo = pRoot->FirstChildElement("type");
	int maxTotalStarTemp = 0;
	while (gradeInfoNodeInfo)
	{
		GradeInfo gradeInfo;
		const char* levelIdChars = gradeInfoNodeInfo->Attribute("Level");
		if (levelIdChars)
		{
			gradeInfo.levelId = boost::lexical_cast<int>(levelIdChars);
		}

		const char* nowStarsChars = gradeInfoNodeInfo->Attribute("Now_Stars");
		if (nowStarsChars)
		{
			gradeInfo.nowStars = boost::lexical_cast<int>(nowStarsChars);
		}
		const char* totalStarsChars = gradeInfoNodeInfo->Attribute("Total_Stars");
		if (totalStarsChars)
		{
			gradeInfo.totalStars = boost::lexical_cast<int>(totalStarsChars);
		}

		if (gradeInfo.totalStars > maxTotalStarTemp)
		{
			maxTotalStarTemp = gradeInfo.totalStars;
		}

		const char* nameChars = gradeInfoNodeInfo->Attribute("Name");
		if (nameChars)
		{
			gradeInfo.name = nameChars;
		}

		const char* imageChars = gradeInfoNodeInfo->Attribute("Image");
		if (imageChars)
		{
			gradeInfo.image = imageChars;
		}

		const char* r1Chars = gradeInfoNodeInfo->Attribute("R1");
		if (r1Chars)
		{
			gradeInfo.r1 = boost::lexical_cast<int>(r1Chars);
		}

		const char* r2Chars = gradeInfoNodeInfo->Attribute("R2");
		if (r2Chars)
		{
			gradeInfo.r2 = boost::lexical_cast<int>(r2Chars);
		}

		const char* r3Chars = gradeInfoNodeInfo->Attribute("R3");
		if (r3Chars)
		{
			gradeInfo.r3 = boost::lexical_cast<int>(r3Chars);
		}

		const char* r4_6Chars = gradeInfoNodeInfo->Attribute("R4_6");
		if (r4_6Chars)
		{
			gradeInfo.r4_6 = boost::lexical_cast<int>(r4_6Chars);
		}

		const char* r7_10Chars = gradeInfoNodeInfo->Attribute("R7_10");
		if (r7_10Chars)
		{
			gradeInfo.r7_10 = boost::lexical_cast<int>(r7_10Chars);
		}

		const char* r11_15Chars = gradeInfoNodeInfo->Attribute("R11_15");
		if (r11_15Chars)
		{
			gradeInfo.r11_15 = boost::lexical_cast<int>(r11_15Chars);
		}

		const char* r16_20Chars = gradeInfoNodeInfo->Attribute("R16_20");
		if (r16_20Chars)
		{
			gradeInfo.r16_20 = boost::lexical_cast<int>(r16_20Chars);
		}

		const char* r31_50Chars = gradeInfoNodeInfo->Attribute("R31_50");
		if (r31_50Chars)
		{
			gradeInfo.r31_50 = boost::lexical_cast<int>(r31_50Chars);
		}

		grades.push_back(gradeInfo);
		gradesMapByLevelId.insert(pair<int, GradeInfo>(gradeInfo.levelId, gradeInfo));
		 
		gradeInfoNodeInfo = gradeInfoNodeInfo->NextSiblingElement();
	}

	maxTotalStar = maxTotalStarTemp;


	//cout << "levels vector size= " << levels.size() << endl;
	//size_t len = levels.size();

	//for (size_t i = 0; i < len; i++) 
	//{
	//	LevelInfo config = levels[i];
	//	cout << config.levelId << "," << config.totalStars << endl;

	//}

	return true;
}


//TODO
int GradeConfig::getGradeByTotalStar(const int totalStar)
{
	int grade = 1;
	int maxStar = 0;

	if (totalStar >= maxStar)
	{
		size_t len = grades.size();
		if (len > 0)
		{
			for (size_t i = 0; i < len; i++)
			{
				GradeInfo config = grades[i];
				if (config.levelId > 0)
				{
					maxStar = config.totalStars;
					if (totalStar > maxStar){
						grade = config.levelId+1;
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	return grade;
}


int GradeConfig::getGradeMaxStarByGrade(const int grade)
{
	map<int, GradeInfo>::iterator iter;
	iter = gradesMapByLevelId.find(grade);

	if (iter != gradesMapByLevelId.end())
	{
		GradeInfo gradeInfo = iter->second;
		if (gradeInfo.levelId > 0)
		{
			return gradeInfo.totalStars;
		}
	}
	return 0;
}

GradeInfo GradeConfig::getGradeInfoByGrade(const int grade)
{
	GradeInfo gradeInfo;
	map<int, GradeInfo>::iterator iter;
	iter = gradesMapByLevelId.find(grade);

	if (iter != gradesMapByLevelId.end())
	{
		GradeInfo gradeTemp = iter->second;
		if (gradeTemp.levelId > 0)
		{
			gradeInfo = gradeTemp;
		}
	}
	return gradeInfo;
}


int GradeConfig::addStarByGradeAndRank(int grade,int rank)
{
	GradeInfo gradeInfo = getGradeInfoByGrade(grade);
	if (gradeInfo.levelId > 0)
	{
		return getAddNumFromInfoByRank(gradeInfo, rank);
	}
	return 0;
}


int GradeConfig::getAddNumFromInfoByRank(const GradeInfo& grade, const int rank)
{
	if (grade.levelId > 0 && rank >= 1 && rank <= 50){
		//R1 = "1" R2 = "1" R3 = "1" R4_6 = "1" R7_10 = "1" R11_15 = "1" R16_20 = "1" R21_30 = "1" R31_50 = "1"
		if (rank == 1){
			return grade.r1;
		}
		else if (rank == 2){
			return grade.r2;
		}
		else if (rank == 3){
			return grade.r3;
		}
		else if (rank >= 4 && rank <= 6){
			return grade.r4_6;
		}
		else if (rank >= 7 && rank <= 10){
			return grade.r7_10;
		}
		else if (rank >= 11 && rank <= 15){
			return grade.r11_15;
		}
		else if (rank >= 16 && rank <= 20){
			return grade.r16_20;
		}
		else if (rank >= 21 && rank <= 30){
			return grade.r21_30;
		}
		else if (rank >= 31 && rank <= 50){
			return grade.r31_50;
		}
	}
	return 0;
}