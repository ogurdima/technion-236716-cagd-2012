#include "StdAfx.h"
#include "Curve.h"
#include "BezierMath.h"
#pragma warning (disable : 4800)
#pragma warning (disable : 4018)

Curve::Curve()
{
	m_ctrlPts.clear();
}

Curve::~Curve()
{
}

UINT Curve::DrawCtrlPolygon()
{
	Calculate();
	vector<CCagdPoint> polyPoints;
	for (int i = 0; i < m_ctrlPts.size(); i++)
	{
		polyPoints.push_back(m_ctrlPts[i].m_pt);
	}
	if (1 == polyPoints.size())
	{
		return cagdAddPoint(&polyPoints[0]);
	}
	UINT uid = cagdAddPolyline(&polyPoints[0], polyPoints.size(), CAGD_SEGMENT_CTLPLYGN);
	cagdSetSegmentColor(uid, 255, 0, 0);
	return uid;
}

UINT Curve::DrawCurve()
{
	Calculate();
	if(0 >= m_dataPts.size())
	{
		return 0;
	}
	return cagdAddPolyline(&m_dataPts[0], m_dataPts.size(), CAGD_SEGMENT_POLYLINE);
}

bool Curve::InsertPt(const CCagdPoint& pt, double weight, int ptIdxAt)
{
	int idxToUse = ptIdxAt;
	if((-1 == idxToUse) || (0 == m_ctrlPts.size()))
	{
		WeightedPt p(pt, weight);
		p.m_pt.z = 0;
		m_ctrlPts.push_back(p);
		return true;
	}
	if((idxToUse < 0) || (idxToUse >= (m_ctrlPts.size()+1)))
	{ return false; }
	
	vector<WeightedPt>::iterator pos = (m_ctrlPts.begin() + idxToUse);
	WeightedPt p(pt, weight);
	p.m_pt.z = 0;
	m_ctrlPts.insert(pos, p);

	return true;
}

bool Curve::UpdatePtPos(const CCagdPoint& pt, int ptIdxAt)
{
	if((ptIdxAt < 0) || (ptIdxAt >= (m_ctrlPts.size()+1)))
	{ return false; }

	m_ctrlPts[ptIdxAt].m_pt = pt;
	return true;
}

bool Curve::UpdatePtWeight(double weight, int ptIdxAt)
{
	if((ptIdxAt < 0) || (ptIdxAt >= (m_ctrlPts.size()+1)))
	{ return false; }

	m_ctrlPts[ptIdxAt].m_weight = weight;
	return true;
}

int Curve::polygonSize() const
{
	return m_ctrlPts.size();
}

void Curve::SetPoly(const vector<CCagdPoint>& pts)
{
	m_ctrlPts.clear();
	for(int i=0; i<pts.size(); ++i)
	{
		WeightedPt p(pts[i], 1.0);
		//p.m_pt.z = 0;
		m_ctrlPts.push_back(p);
	}
}

void Curve::SetPoly(const vector<WeightedPt>& pts)
{
	m_ctrlPts = pts;	
}

bool Curve::SetWeight(int idx, double weight)
{
	if((idx < 0) || (idx >= m_ctrlPts.size()) || (weight < 0))
	{ return false; }

	m_ctrlPts[idx].m_weight = weight;
	return true;
}

double Curve::GetWeight(int idx)
{
	if((idx < 0) || (idx >= m_ctrlPts.size()))
	{ return -1.0; }

	return m_ctrlPts[idx].m_weight;
}

int Curve::GetInsertionIndex(const CCagdPoint& p)
{
	return U::ptOnLineSegmentAfter(p, m_ctrlPts);
}


int Curve::PickPoint(int x, int y) const
{
	for(int i=0; i<m_ctrlPts.size(); ++i)
	{
		int winX, winY;
		if(!cagdToWindow(const_cast<CCagdPoint*>(&m_ctrlPts[i].m_pt), &winX, &winY))
		{ continue; }

		CCagdPoint diff(double(winX-x), double(winY-y));
		double dist = length(diff);
		if(dist < 15.0)
		{
			return i;	
		}
	}
	return -1;
}

Curve& Curve::operator=(const Curve& rhs)
{
	m_ctrlPts = rhs.m_ctrlPts;
	m_dataPts = rhs.m_dataPts;
	return *this;
}

vector<WeightedPt> Curve::ControlPoints()
{
	return m_ctrlPts; // i think it copies the data
}

void Curve::RemoveCtrlPoint(int idx)
{
	if((idx < 0) || (idx >= m_ctrlPts.size()))
	{ return; }

	vector<WeightedPt> tmp = m_ctrlPts;
	m_ctrlPts.clear();

	for (int i = 0; i < tmp.size(); i++)
	{
		if (i != idx)
			m_ctrlPts.push_back(tmp[i]);
	}
}


