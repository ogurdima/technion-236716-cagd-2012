#pragma once
#include <string>
#include <vector>

class FrenetParser
{
public:
	FrenetParser(void);
	FrenetParser(const std::string& fileString);
	~FrenetParser(void);
	bool ParseFile(const std::string& fileName);
	bool ParseString(const std::string& fileString);


	std::vector<std::string> m_equations;
	double m_paramA;
	double m_paramB;
	std::string m_fileString;
	int m_parseLine;
};


