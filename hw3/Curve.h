#pragma once

#include "cagd.h"
#include <vector>
using std::vector;



struct WeightedPt
{
	WeightedPt() : m_weight(1.0) {}
	WeightedPt(const CCagdPoint& pt, double weight) 
		: m_pt(pt), m_weight(weight)
	{}
	CCagdPoint m_pt;
	double m_weight;
};

class Curve
{
public:
	/******************************************************************
	*	Public members
	*******************************************************************/
	UINT DrawCtrlPolygon();
	UINT DrawCurve();

	/******************************************************************
	*	Public methods
	*******************************************************************/
	Curve(void);
	virtual ~Curve(void);
	bool InsertPt(const CCagdPoint& pt, double weight, int ptIdxAt = -1);
	void SetPoly(const vector<CCagdPoint>& pts);
	void SetPoly(const vector<WeightedPt>& pts);
	bool SetWeight(int idx, double weight);
	double GetWeight(int idx);
	int GetInsertionIndex(const CCagdPoint& p);
	int polygonSize();

protected:
	virtual void Calculate() = 0;
	// the control polygon
	vector<WeightedPt> m_ctrlPts;
	// curve data
	vector<CCagdPoint> m_dataPts;
};

