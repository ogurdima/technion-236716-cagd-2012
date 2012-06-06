#pragma once

#include "cagd.h"
#include <vector>
#include <string>
#include <sstream>
using std::vector;
using std::string;



struct WeightedPt
{
	WeightedPt() : m_weight(1.0) {}
	WeightedPt(const CCagdPoint& pt, double weight) 
		: m_pt(pt), m_weight(weight)
	{}
	WeightedPt& operator=(const WeightedPt& rhs)
	{
		m_pt = rhs.m_pt;
		m_weight = rhs.m_weight;
		return *this;
	}
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
	virtual UINT DrawCurve();

	/******************************************************************
	*	Public methods
	*******************************************************************/
	Curve(void);
	virtual ~Curve(void);
	virtual bool InsertPt(const CCagdPoint& pt, double weight, int ptIdxAt = -1);
	bool UpdatePtPos(const CCagdPoint& pt, int ptIdxAt);
	bool UpdatePtWeight(double weight, int ptIdxAt);
	void SetPoly(const vector<CCagdPoint>& pts);
	void SetPoly(const vector<WeightedPt>& pts);
	bool SetWeight(int idx, double weight);
	double GetWeight(int idx);
	int GetInsertionIndex(const CCagdPoint& p);
	int polygonSize() const;
	int PickPoint(int x, int y) const;
	Curve& operator=(const Curve& rhs);
	virtual string toIrit(int id) = 0;
	virtual string toDat(int id = 0) = 0;
	vector<WeightedPt> ControlPoints();

	virtual void RemoveCtrlPoint(int idx);

protected:
	virtual void Calculate() = 0;
	// the control polygon
	vector<WeightedPt> m_ctrlPts;
	// curve data
	vector<CCagdPoint> m_dataPts;
};

