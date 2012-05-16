
#include "Bezier.h"
#include "BezierMath.h"

Bezier::Bezier()
{
}

Bezier::~Bezier()
{
}

UINT Bezier::DrawCtrlPolygon()
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

UINT Bezier::DrawCurve()
{
	Calculate();
	return cagdAddPolyline(&m_dataPts[0], m_dataPts.size(), CAGD_SEGMENT_POLYLINE);
}

void Bezier::Calculate()
{
	m_dataPts.clear();
	int order = m_ctrlPts.size() - 1;
	//Calculating coefficients for
	vector<double> BCoeff;
	for (int i = 0; i <= order; i++)
	{
		BCoeff.push_back(U::choose(order, i));
	}

	for (double t = 0; t <= 1; t += 0.001)
	{
		CCagdPoint curvePt = CCagdPoint(0,0,0);
		double divFactor = 0;
		for (int i = 0; i <= order; i++)
		{
			double theta = (BCoeff[i] * pow((1-t),order-i) * pow(t,i));
			curvePt = curvePt + m_ctrlPts[i].m_pt * m_ctrlPts[i].m_weight * theta;
			divFactor += (m_ctrlPts[i].m_weight * theta);
		}
		m_dataPts.push_back(curvePt/divFactor);
	}
}

bool Bezier::InsertPt(const CCagdPoint& pt, double weight, int ptIdxAt)
{
	int idxToUse = ptIdxAt;
	if(-1 == idxToUse)
	{
		idxToUse = m_ctrlPts.size();
	}
	if((idxToUse < 0) || (idxToUse >= (m_ctrlPts.size()+1)))
	{ return false; }
	

	vector<BezierPt>::iterator pos = (m_ctrlPts.begin() + idxToUse);
	m_ctrlPts.insert(pos, BezierPt(pt, weight));


	return true;
}

int Bezier::polygonSize()
{
	return m_ctrlPts.size();
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

int Bezier::GetInsertionIndex(const CCagdPoint& p)
{
	return U::ptOnLineSegmentAfter(p, m_ctrlPts);
}

