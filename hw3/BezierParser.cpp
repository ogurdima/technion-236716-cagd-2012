#include "stdafx.h"
#include "BezierParser.h"
#include <iosfwd>
#include <iostream>
#include <fstream>
#include <sstream>
#include "BezierMath.h"

#pragma warning (disable : 4800)
#pragma warning (disable : 4018)

BezierParser::BezierParser(void)
	: m_state(ParseStateNone)
	, m_expectedPtCount(0)
	, m_expectedKnotCount(0)
	, m_type(SplineTypeUnknown)
{
}


BezierParser::~BezierParser(void)
{
}

SplineType BezierParser::Type() const
{
	return m_type;
}

void BezierParser::Clear()
{
	m_pts.clear();
	m_knots.clear();
	m_expectedPtCount = 0;
	m_expectedKnotCount = 0;
	m_state = ParseStateNone;
	m_type = SplineTypeUnknown;
}

bool BezierParser::ParseFile(const std::string& filename)
{
	Clear();

	std::ifstream fs(filename.c_str());
	if(!fs) 
	{
		return false;
	}

	std::string fileLine;
	char file_line[1000];
	while(!fs.eof()) 
	{
		fs.getline(file_line, 1000, '\n');
		fileLine = std::string(file_line);
		if(!ParseLine(fileLine))
		{
			return false;
		}
		if(ParseStateDone == m_state)
		{ break; }
	}


	return (ParseStateDone == m_state);
}


bool BezierParser::ParseLine(const std::string& line)
{
	// if line starts with #, return true
	// if line is empty, skip
	if(line.empty()) 
	{
		::OutputDebugString("Empty\n");
		return true;
	}
	if(line[0]=='#') 
	{
		::OutputDebugString("#\n");
		return true;
	}
	int firstNonWhitespace = line.find_first_not_of(" \t\n\r");
	int lastNonWhitespace = line.find_last_not_of(" \t\n\r");
	if(-1 == firstNonWhitespace) 
	{
		::OutputDebugString("Whitespace\n");
		return true;
	}

	// prepare a trimmed line
	std::string line_trim = line.substr(firstNonWhitespace, (lastNonWhitespace+1) - firstNonWhitespace);

	if(ParseStateNone == m_state)
	{
		// look for number 
		int point_count = -1;
		std::stringstream sstrm(line_trim);

		// try to parse the number
		bool success = sstrm >> point_count;
		if(!success)
		{ return false; }
		
		// record the count
		m_expectedPtCount = point_count;
		
		// advance the state
		m_state = ParseStateFoundNumber;
		
		// the number should be on its own line, so quit once we've found it; ignore everything else.
		return true;
	}

	// if state is found_number, we're looking for either knots or a float
	if(ParseStateFoundNumber == m_state)
	{
		// should either be 'knots[' or a float value
		std::string knots_str("knots[");
		std::string line_begin = line_trim.substr(0, knots_str.size());
		int cmp_res = _stricmp(knots_str.c_str(), line_begin.c_str());
		if(0 == cmp_res)
		{
			// find the second bracket
			int bracket_close_pos = line_trim.find_first_of(']');
			if(bracket_close_pos < knots_str.size())
			{
				// appears before [
				::OutputDebugString(std::string(std::string("Misplaced bracket: ") + line_trim).c_str());
				return false;
			}

			// get the string between the brackets
			std::string knot_count_str = line_trim.substr(knots_str.size(), bracket_close_pos-knots_str.size());
			if(!U::IsInteger(knot_count_str))
			{
				::OutputDebugString(std::string(std::string("Value in brackets not an integer: ") + line_trim).c_str());
				return false;
			}

			// it should be an integer, so parse it
			int knot_count = -1;
			std::stringstream kcstrm(knot_count_str);
			kcstrm >> knot_count;
			m_expectedKnotCount = knot_count;

			// success. look for '='
			int equals_pos = line_trim.find_first_of('=');
			if(equals_pos < bracket_close_pos)
			{
				::OutputDebugString(std::string(std::string("Error parsing '=': ") + line_trim).c_str());
				return false;
			}
			
			// upgrade the state
			m_state = ParseStateKnots;
			m_type = SplineTypeBspline;

			// upgrade the line for below
			int last_non_whitespace = line_trim.find_last_not_of(' \t\n\r');
			line_trim = line_trim.substr(equals_pos+1, last_non_whitespace);
			int first_non_whitespace = line_trim.find_first_not_of(' \t\n\r');
			line_trim = line_trim.substr(first_non_whitespace);
		}
		else 
		{
			// make sure it starts with a float value
			std::stringstream sstrm(line_trim);
			double first_val = -1.0;
			bool is_float = sstrm >> first_val;
			if(!is_float)
			{
				return false;
			}

			// advance state to points
			m_state = ParseStatePoints;
			m_type = SplineTypeBezier;
		}
	}

	// if state is knots
	if(ParseStateKnots == m_state)
	{
		std::stringstream sstrm(line_trim);
		do
		{
			double knot_val;
			bool success = sstrm >> knot_val;
			if(!success)
			{
				::OutputDebugString(std::string(std::string("Parse error at ") + sstrm.str()).c_str());
				return false;
			}

			m_knots.push_back(knot_val);
		}
		while((!sstrm.eof()) && (m_knots.size() < m_expectedKnotCount));

		if(m_knots.size() == m_expectedKnotCount)
		{
			// if we've reached the expected knots, continue
			m_state = ParseStatePoints;
		}
		return true;
	}

	// if state is points
	if(ParseStatePoints == m_state)
	{
		double x, y, z;
		bool success = true;
		std::stringstream sstrm(line_trim);
		success = sstrm >> x;
		if((!success) || (sstrm.eof()))
		{ return false; }
		
		success = sstrm >> y;
		if((!success) || (sstrm.eof()))
		{ return false; }

		success = sstrm >> z;
		if(!success)
		{ return false; }

		double tmp_val;
		bool more_data = sstrm >> tmp_val;
		if(more_data)
		{
			::OutputDebugString(std::string(std::string("Invalid line (too much data): ") + sstrm.str()).c_str());
			return false;
		}

		CCagdPoint newPt(x, y, z);
		m_pts.push_back(newPt);

		if(m_pts.size() == m_expectedPtCount)
		{
			m_state = ParseStateDone;
		}

	}

	return true;
}
