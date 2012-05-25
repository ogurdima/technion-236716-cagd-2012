
#include "CurveMgr.h"

#pragma warning (disable : 4800)
#pragma warning (disable : 4018)

CurveMgr::CurveMgr()
{
}

CurveMgr::~CurveMgr()
{
}


int CurveMgr::NewBezierCurve()
{
	m_curves.push_back(CurveWrp(SplineTypeBezier));
	return m_curves.size()-1;
}

int CurveMgr::NewBsplineCurve()
{
	//m_curves.push_back(CurveWrp(SplineTypeBspline));
	//return m_curves.size()-1;
	return -1;
}

bool CurveMgr::ToggleShowPolygon(int curveIdx)
{
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return false; }
	m_curves[curveIdx].m_showCtrPoly = !m_curves[curveIdx].m_showCtrPoly;
	return RedrawCurve(curveIdx);
}

bool CurveMgr::RedrawCurve(int curveIdx)
{
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return false; }
	CurveWrp& cw = m_curves[curveIdx];

	// clear existing curve & control polygon
	cagdFreeSegment(cw.m_curveId);
	cagdFreeSegment(cw.m_ctrPolyId);
	// recalculate
	cw.m_curveId = cw.m_curve->DrawCurve();
	cw.m_ctrPolyId = cw.m_curve->DrawCtrlPolygon();

	if(!cw.m_showCtrPoly)
	{
		cagdHideSegment(cw.m_ctrPolyId);
	}
	if(!cw.m_showCurve)
	{
		cagdHideSegment(cw.m_curveId);
	}
	return true;
}

bool CurveMgr::AddLastCtrlPt(const CCagdPoint& pt, double weight, int curveIdx)
{
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return false; }
	return AddCtrlPt(pt, weight, curveIdx, m_curves[curveIdx].m_curve->polygonSize());
}

bool CurveMgr::AddCtrlPt(const CCagdPoint& pt, double weight, int curveIdx, int polyPointIdx)
{
	if (-1 == curveIdx)
	{
		curveIdx = NewBezierCurve();
	}
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return false; }

	CurveWrp& cw = m_curves[curveIdx];

	if (-1 == polyPointIdx)
	{
		polyPointIdx = cw.m_curve->GetInsertionIndex(pt);
	}

	// add the point
	cw.m_curve->InsertPt(pt, weight, polyPointIdx);

	RedrawCurve(curveIdx);
}


void CurveMgr::ClearAll()
{
	for(int i=0; i<m_curves.size(); ++i)
	{
		CurveWrp& cw = m_curves[i];
		cagdFreeSegment(cw.m_ctrPolyId);
		cw.m_ctrPolyId = 0;
		cagdFreeSegment(cw.m_curveId);
		cw.m_curveId = 0;
	}
	m_curves.clear();
}

PtContext CurveMgr::getPtContext(const CCagdPoint& p)
{
	for (int i = 0; i < m_curves.size(); i++)
	{
		int idx = m_curves[i].m_curve->GetInsertionIndex(p);
		if (-1 != idx)
		{
			return (m_curves[i].m_type == SplineTypeBezier) ? ContextBezierPoly : ContextBsplinePoly;
		}
	}
	return ContextEmpty;
}

int CurveMgr::getCurveIndexByPointOnPolygon(const CCagdPoint& p)
{
	for (int i = 0; i < m_curves.size(); i++)
	{
		int idx = m_curves[i].m_curve->GetInsertionIndex(p);
		if (-1 != idx)
		{
			return i;
		}
	}	
	return -1;
}
