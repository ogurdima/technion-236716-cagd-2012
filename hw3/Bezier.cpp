
#include "Bezier.h"

Bezier::Bezier()
{
}

Bezier::~Bezier()
{
}

UINT Bezier::DrawCtrlPolygon()
{
	Calculate();
	//return cagdAddPolyline(&m_ctrlPts[0], m_ctrlPts.size(), CAGD_SEGMENT_CTLPLYGN);
	return 0;
}

UINT Bezier::DrawCurve()
{
	Calculate();
	return cagdAddPolyline(&m_dataPts[0], m_dataPts.size(), CAGD_SEGMENT_POLYLINE);
}

void Bezier::Calculate()
{
	m_dataPts.clear();
	for (double t = 0; t <= 1; t += 0.001)
	{
		CCagdPoint curvePt = CCagdPoint(0,0,0);
		for (int i; i < m_ctrlPts.size(); i++)
		{
			//curvePt += m_ctrlPts[i].m_pt
		}
	}
}

void Bezier::SetPoly(const vector<CCagdPoint>& pts)
{
	m_ctrlPts.clear();
	for(int i=0; i<pts.size(); ++i)
	{
		m_ctrlPts.push_back(BezierPt(pts[i], 1.0));
	}
}

void Bezier::SetPoly(const vector<BezierPt>& pts)
{
	m_ctrlPts = pts;	
}

bool Bezier::SetWeight(int idx, double weight)
{
	if((idx < 0) || (idx >= m_ctrlPts.size()) || (weight < 0))
	{ return false; }

	m_ctrlPts[idx].m_weight = weight;
}

double Bezier::GetWeight(int idx)
{
	if((idx < 0) || (idx >= m_ctrlPts.size()))
	{ return -1.0; }

	return m_ctrlPts[idx].m_weight;
}

