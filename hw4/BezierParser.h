#pragma once
#include <vector>
#include <string>
#include "cagd.h"
#include "CurveMgr.h"
using std::vector;



struct ParsedCurve
{


	ParsedCurve() : m_order(0), m_type(SplineTypeUnknown){} 
	void Clear() 
	{
		m_pts.clear();
		m_knots.clear();
		m_type = SplineTypeUnknown;
	}
	vector<CCagdPoint> m_pts;
	vector<double> m_knots;
	int m_order;
	SplineType m_type;
};


class BezierParser
{
	enum ParseState
	{
		ParseStateNone,
		ParseStateFoundNumber,
		ParseStateKnots,
		ParseStatePoints,
		ParseStateDone
	};
public:
	BezierParser(void);
	~BezierParser(void);
	void Clear();
	
	bool ParseFile(const std::string& filename);
	bool ParseLine(const std::string& line);

	vector<ParsedCurve> m_curves;

	int m_intval;
	int m_expectedPtCount;
	int m_expectedKnotCount;
	int m_crvIdx;
	bool m_valid;

	ParseState m_state;
};

