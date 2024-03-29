#include "stdafx.h"
#include "FrenetParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
FrenetParser::FrenetParser(void)
: m_paramA(0.0)
, m_paramB(0.0)
, m_parseLine(0)
{
}

FrenetParser::FrenetParser(const std::string& fileString) 
: m_paramA(0.0)
, m_paramB(0.0)
, m_parseLine(0)
{
	m_fileString = fileString;
}

FrenetParser::~FrenetParser(void)
{
}

bool FrenetParser::ParseFile(const std::string& fileName)
{
	std::ifstream fs(fileName.c_str());
	if(!fs) 
	{
		return false;
	}
	m_paramA = m_paramB = 0.0;
	m_parseLine = 0;
	m_equations.clear();

	std::string fileLine;
	char file_line[1000];
	while(!fs.eof()) 
	{
		fs.getline(file_line, 1000, '\n');
		fileLine = std::string(file_line);
		/*std::string msg = */
		std::stringstream sstrm;
		sstrm << fileLine << std::endl;
		std::string theLine = sstrm.str();
		::OutputDebugString((LPCSTR)theLine.c_str());
		ParseString(theLine);
	}

	return true;

}

bool FrenetParser::ParseString(const std::string& str)
{
	if(str.empty()) 
	{
		::OutputDebugString("Empty\n");
		return true;
	}
	if(str[0]=='#') 
	{
		::OutputDebugString("#\n");
		return true;
	}
	int firstNonWhitespace = str.find_first_not_of(" \t\n\r");
	if(-1 == firstNonWhitespace) 
	{
		::OutputDebugString("Whitespace\n");
		return true;
	}
	
	switch(m_parseLine)
	{
	case 0:
		m_equations.push_back(str);
		break;
	case 1:
		m_equations.push_back(str);
		break;
	case 2:
		m_equations.push_back(str);
		break;
	case 3:
		{
			// here we're at the parameter line. break it up
			double aVal;
			double bVal;
			int commaPos = str.find(",");
			if (-1 == commaPos) 
			{
				commaPos = str.find_first_of(" \t");
			}
			std::string beforeComma = str.substr(0, commaPos);
			std::string afterComma = str.substr(commaPos +1, str.size());
			std::stringstream sstrmBefore(beforeComma);
			std::stringstream sstrmAfter(afterComma);
			sstrmBefore >> aVal;
			sstrmAfter >> bVal;
			m_paramA = aVal;
			m_paramB = bVal;
		}
		break;
	}

	++m_parseLine;
	return true;	
}