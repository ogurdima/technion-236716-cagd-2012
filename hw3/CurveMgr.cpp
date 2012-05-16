
#include "CurveMgr.h"

CurveMgr::CurveMgr()
{
}

CurveMgr::~CurveMgr()
{
}


int CurveMgr::NewBezierCurve()
{
	m_beziers.push_back(BezierWrp());
	return m_beziers.size()-1;
}


bool CurveMgr::AddBezierCtrlPt(int idx, const CCagdPoint& pt, double weight)
{
	if((idx < 0) || (idx >= m_beziers.size()))
	{ return false; }

	BezierWrp& bw = m_beziers[idx];

	// clear existing curve & control polygon
	cagdFreeSegment(bw.m_curveId);
	cagdFreeSegment(bw.m_ctrPolyId);

	// add the point
	bw.m_curve.InsertPt(pt, weight);

	// recalculate
	bw.m_curveId = bw.m_curve.DrawCurve();
	bw.m_ctrPolyId = bw.m_curve.DrawCtrlPolygon();

	if(!bw.m_showCtrPoly)
	{
		cagdHideSegment(bw.m_ctrPolyId);
	}
	if(!bw.m_showCurve)
	{
		cagdHideSegment(bw.m_curveId);
	}

}


void CurveMgr::ClearAll()
{
	for(int i=0; i<m_beziers.size(); ++i)
	{
		BezierWrp& bw = m_beziers[i];
		cagdFreeSegment(bw.m_ctrPolyId);
		bw.m_ctrPolyId = 0;
		cagdFreeSegment(bw.m_curveId);
		bw.m_curveId = 0;
	}
	m_beziers.clear();
}

PtContext CurveMgr::getPtContext(const CCagdPoint& p)
{
	for (int i = 0; i < m_beziers.size(); i++)
	{
		int idx = m_beziers[i].m_curve.GetInsertionIndex(p);
		if (-1 != idx)
		{
			return ContextBezierPoly;
		}
	}


	return ContextEmpty;
}

int CurveMgr::getCurveIndexByPointOnPolygon(const CCagdPoint& p)
{
	for (int i = 0; i < m_beziers.size(); i++)
	{
		int idx = m_beziers[i].m_curve.GetInsertionIndex(p);
		if (-1 != idx)
		{
			return i;
		}
	}	
	return -1;
}

void CurveMgr::InsertBezierCtrlPt(const CCagdPoint& p)
{
	for (int i = 0; i < m_beziers.size(); i++)
	{
		int idx = m_beziers[i].m_curve.GetInsertionIndex(p);
		if (-1 != idx)
		{
			BezierWrp bw = m_beziers[i];
			

			// clear existing curve & control polygon
			cagdFreeSegment(bw.m_curveId);
			cagdFreeSegment(bw.m_ctrPolyId);

			bw.m_curve.InsertPt(p, 1, idx);

			// recalculate
			bw.m_curveId = bw.m_curve.DrawCurve();
			bw.m_ctrPolyId = bw.m_curve.DrawCtrlPolygon();

			if(!bw.m_showCtrPoly)
			{
				cagdHideSegment(bw.m_ctrPolyId);
			}
			if(!bw.m_showCurve)
			{
				cagdHideSegment(bw.m_curveId);
			}
		}
	}
}