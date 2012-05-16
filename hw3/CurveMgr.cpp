
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

bool CurveMgr::ToggleShowBezierPolygon(int curveIdx)
{
	if((curveIdx < 0) || (curveIdx >= m_beziers.size()))
	{ return false; }
	m_beziers[curveIdx].m_showCtrPoly = !m_beziers[curveIdx].m_showCtrPoly;
	RedrawCurve(curveIdx);
}

bool CurveMgr::RedrawCurve(int curveIdx)
{
	if((curveIdx < 0) || (curveIdx >= m_beziers.size()))
	{ return false; }
	BezierWrp& bw = m_beziers[curveIdx];

	// clear existing curve & control polygon
	cagdFreeSegment(bw.m_curveId);
	cagdFreeSegment(bw.m_ctrPolyId);
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
	return true;
}

bool CurveMgr::AddLastBezierCtrlPt(const CCagdPoint& pt, double weight, int curveIdx)
{
	if((curveIdx < 0) || (curveIdx >= m_beziers.size()))
	{ return false; }
	return AddBezierCtrlPt(pt, weight, curveIdx, m_beziers[curveIdx].m_curve.polygonSize());
}

bool CurveMgr::AddBezierCtrlPt(const CCagdPoint& pt, double weight, int curveIdx, int polyPointIdx)
{
	if (-1 == curveIdx)
	{
		curveIdx = NewBezierCurve();
	}
	if((curveIdx < 0) || (curveIdx >= m_beziers.size()))
	{ return false; }

	BezierWrp& bw = m_beziers[curveIdx];

	if (-1 == polyPointIdx)
	{
		polyPointIdx = bw.m_curve.GetInsertionIndex(pt);
	}

	// add the point
	bw.m_curve.InsertPt(pt, weight, polyPointIdx);

	RedrawCurve(curveIdx);
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
