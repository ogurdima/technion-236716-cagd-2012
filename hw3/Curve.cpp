
#include "Curve.h"
#include "BezierMath.h"
#pragma warning (disable : 4800)
#pragma warning (disable : 4018)

Curve::Curve()
{
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
	return cagdAddPolyline(&polyPoints[0], polyPoints.size(), CAGD_SEGMENT_CTLPLYGN);
}

UINT Curve::DrawCurve()
{
	Calculate();
	return cagdAddPolyline(&m_dataPts[0], m_dataPts.size(), CAGD_SEGMENT_POLYLINE);
}

bool Curve::InsertPt(const CCagdPoint& pt, double weight, int ptIdxAt)
{
	int idxToUse = ptIdxAt;
	if(-1 == idxToUse)
	{
		idxToUse = m_ctrlPts.size();
	}
	if((idxToUse < 0) || (idxToUse >= (m_ctrlPts.size()+1)))
	{ return false; }
	
	vector<WeightedPt>::iterator pos = (m_ctrlPts.begin() + idxToUse);
	m_ctrlPts.insert(pos, WeightedPt(pt, weight));


	return true;
}

int Curve::polygonSize()
{
	return m_ctrlPts.size();
}

void Curve::SetPoly(const vector<CCagdPoint>& pts)
{
	m_ctrlPts.clear();
	for(int i=0; i<pts.size(); ++i)
	{
		m_ctrlPts.push_back(WeightedPt(pts[i], 1.0));
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

