#include "StdAfx.h"
#include "CurveMgr.h"
#include <vector>
#include "BezierMath.h"
using std::vector;
#pragma warning (disable : 4800)
#pragma warning (disable : 4018)

CurveMgr::CurveMgr() :
m_showGrid(false)
{
}

CurveMgr::~CurveMgr()
{
}

//=============================================================================
// Curve insertion and manipulation
//=============================================================================

int CurveMgr::NewBezierCurve()
{
	m_curves.push_back(CurveWrp(SplineTypeBezier));
	return m_curves.size()-1;
}

int CurveMgr::NewBsplineCurve(unsigned int degree)
{
	m_curves.push_back(CurveWrp(SplineTypeBspline));
	int last_idx = m_curves.size()-1;
	BSpline* bsp = static_cast<BSpline*>(m_curves[last_idx].m_curve);
	bsp->SetDegree(degree);
	return m_curves.size()-1;
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

	// need to remove curve wrapper
	if (cw.m_curve->polygonSize() == 0)
	{
		vector<CurveWrp> tmp = m_curves;
		m_curves.clear();
		for (int i = 0; i < tmp.size(); i++)
		{
			if (i != curveIdx)
				m_curves.push_back(tmp[i]);
		}
		return false;
	}


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

bool CurveMgr::AddLastCtrlPt(const CCagdPoint& pt, double weight, int curveIdx, bool noRecalc)
{
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return false; }
	return AddCtrlPt(pt, weight, curveIdx, m_curves[curveIdx].m_curve->polygonSize(), noRecalc);
}

bool CurveMgr::AddCtrlPt(const CCagdPoint& pt, double weight, int curveIdx, int polyPointIdx, bool noRecalc)
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
	vector<WeightControl>::iterator pos = (cw.m_wc.begin() + polyPointIdx);
	cw.m_wc.insert(pos, WeightControl(pt, weight));

  if(!noRecalc)
  {
	  RedrawCurve(curveIdx);
  }

	::OutputDebugString((LPCSTR)(m_curves[curveIdx].m_curve->toIrit(curveIdx).c_str()));

	return true;
}

bool CurveMgr::RemoveCtrlPt(const CCagdPoint& pt)
{
	int windX, windY;
	cagdToWindow(const_cast<CCagdPoint*>(&pt), &windX, &windY);
	ControlPointInfo pi = PickControlPoint(windX, windY);
	if (! pi.IsValid())
		return false;
	m_curves[pi.m_curveIdx].m_curve->RemoveCtrlPoint(pi.m_pointIdx);
	vector<WeightControl> tmp = m_curves[pi.m_curveIdx].m_wc;
	m_curves[pi.m_curveIdx].m_wc.clear();
	for (int i = 0; i < tmp.size(); i++)
	{
		if (i != pi.m_pointIdx)
			m_curves[pi.m_curveIdx].m_wc.push_back(tmp[i]);
	}

	RedrawCurve(pi.m_curveIdx);
	return true;
}

bool CurveMgr::UpdateCtrlPtPos(const ControlPointInfo& ptInfo, const CCagdPoint& pt)
{
	if((0 > ptInfo.m_curveIdx) || (m_curves.size() <= ptInfo.m_curveIdx))
	{ return false; }
	bool success = m_curves[ptInfo.m_curveIdx].m_curve->UpdatePtPos(pt, ptInfo.m_pointIdx);
	if (success)
	{
		m_curves[ptInfo.m_curveIdx].m_wc[ptInfo.m_pointIdx].SetCenter(pt);
	}
	RedrawCurve(ptInfo.m_curveIdx);
	return success;
}

// Not used
bool CurveMgr::UpdateCtrlPtWeight(const ControlPointInfo& ptInfo, double weight)
{
	if((0 > ptInfo.m_curveIdx) || (m_curves.size() <= ptInfo.m_curveIdx))
	{ return false; }

	bool success = m_curves[ptInfo.m_curveIdx].m_curve->SetWeight(weight, ptInfo.m_pointIdx);
	if (success)
	{
		m_curves[ptInfo.m_curveIdx].m_wc[ptInfo.m_pointIdx].SetRadius(weight);
	}
	RedrawCurve(ptInfo.m_curveIdx);
	return success;
}

//=============================================================================
// Context, index
//=============================================================================

PtContext CurveMgr::getPtContext(const CCagdPoint& p)
{
	int windX, windY;
	cagdToWindow(const_cast<CCagdPoint*>(&p), &windX, &windY);
	ControlPointInfo pi = PickControlPoint(windX, windY);
	if (pi.IsValid())
	{
		if (SplineTypeBezier == m_curves[pi.m_curveIdx].m_type)
			return ContextBezierPt;
		if (SplineTypeBspline == m_curves[pi.m_curveIdx].m_type)
			return ContextBsplinePt;
	}

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

ControlPointInfo CurveMgr::PickControlPoint(int x, int y) const
{
	for(int i=0; i<m_curves.size(); ++i)
	{
		const CurveWrp& cw = m_curves[i];
		int idx = cw.m_curve->PickPoint(x, y);
		if((idx < 0) || (idx >= cw.m_curve->polygonSize()))
		{ continue; }

		// if we got here, we have a valid idx
		ControlPointInfo cpInfo(i, idx);
		return cpInfo;
	}

	return ControlPointInfo(); //Invalid control point
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

//=============================================================================
// Weight manipulation
//=============================================================================

bool CurveMgr::ToggleWeightConrol(const CCagdPoint& p)
{
	int windX, windY;
	cagdToWindow(const_cast<CCagdPoint*>(&p), &windX, &windY);
	ControlPointInfo pi = PickControlPoint(windX, windY);
	if (! pi.IsValid())
		return false;
	return m_curves[pi.m_curveIdx].m_wc[pi.m_pointIdx].ToggleShow();
}

void CurveMgr::ChangeWeight(int curveIdx, int ptIdx, int x, int y)
{
	CCagdPoint ctr = m_curves[curveIdx].m_wc[ptIdx].Center();
	CCagdPoint pt[2];
	cagdToObject(x, y, pt);
	pt[0].z = 0;
	double w = length(ctr - pt[0]);
	m_curves[curveIdx].m_curve->SetWeight(ptIdx, w);
	m_curves[curveIdx].m_wc[ptIdx].SetRadius(w);
	RedrawCurve(curveIdx);
}

ControlPointInfo CurveMgr::AttemptWeightAnchor(int x, int y)
{
	for (int i = 0; i < m_curves.size(); i++)
	{
		CurveWrp& cw = m_curves[i];
		for (int j = 0; j < cw.m_wc.size(); j++)
		{
			WeightControl& wc = cw.m_wc[j];
			if (wc.IsUnderCursor(x,y))
			{
				return ControlPointInfo(i,j);
			}
		}
	}
	return ControlPointInfo();
}

//=============================================================================
// Knot vector stuff
//=============================================================================

bool CurveMgr::SetKnotVector(int curveIdx, const vector<double> & kv)
{
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return false; }

	CurveWrp& cw = m_curves[curveIdx];
	if(SplineTypeBspline != cw.m_type)
	{ return false; }

	bool success = static_cast<BSpline*>(cw.m_curve)->SetKnotVector(kv);
	RedrawCurve(curveIdx);
	return success;
}

vector<double> CurveMgr::GetKnotVector(int curveIdx)
{
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return vector<double>(); }

	CurveWrp& cw = m_curves[curveIdx];
	if(SplineTypeBspline != cw.m_type)
	{ return vector<double>(); }

	return dynamic_cast<BSpline*>(cw.m_curve)->GetKnotVector();

}

bool CurveMgr::InsertKnot(int curveIdx, double knotVal)
{
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return false; }

	CurveWrp& cw = m_curves[curveIdx];
	if(SplineTypeBspline != cw.m_type)
	{ return false; }

	bool success = dynamic_cast<BSpline*>(cw.m_curve)->InsertKnotBoehm(knotVal);
	if(success)
	{
		vector<WeightedPt> newCtrl = cw.m_curve->ControlPoints();
		m_curves[curveIdx].m_wc.clear();
		for (int i = 0; i < newCtrl.size(); i++)
		{
			WeightedPt p = newCtrl[i];
			m_curves[curveIdx].m_wc.push_back(WeightControl(p.m_pt, p.m_weight));
		}
		RedrawCurve(curveIdx);
	}
	return success;
}

//=============================================================================
// Printing
//=============================================================================

string CurveMgr::toDat() const
{
	std::ostringstream buf = std::ostringstream();
	for (int i = 0; i < m_curves.size(); i++)
	{
		buf << m_curves[i].m_curve->toDat() << std::endl;
	}
	return buf.str();
}

string CurveMgr::toIrit() const
{
	std::ostringstream buf = std::ostringstream();
	for (int i = 0; i < m_curves.size(); i++)
	{
		buf << m_curves[i].m_curve->toIrit(i) << std::endl;
	}
	return buf.str();
}

//=============================================================================
// Connecting Curves
//=============================================================================

void CurveMgr::connectG0(int it, int to)
{
	for (int i = 0; i < 10; i++)
		connectCustom(it, to, false, false);
}

void CurveMgr::connectG1(int it, int to)
{
	for (int i = 0; i < 10; i++)
		connectCustom(it, to, false, true);
}

void CurveMgr::connectC1(int it, int to)
{
	for (int i = 0; i < 10; i++)
		connectCustom(it, to, true, true);
}

void CurveMgr::connectCustom(int it, int to, bool doScale, bool doRotate)
{
	if((it < 0) || (it >= m_curves.size()) || (to < 0) || (to >= m_curves.size()) || it == to)
	{ return; }

	if (m_curves[to].m_curve->polygonSize() < 2 || m_curves[it].m_curve->polygonSize() < 2)
		return;

	std::ostringstream buff;
	buff << "Connecting " << it << " to " << to << " doScale = " << doScale << " doRotate = " << doRotate << std::endl;
	::OutputDebugString((LPCSTR)(buff.str().c_str()));

	vector<WeightedPt> first = m_curves[it].m_curve->ControlPoints();

	CCagdPoint dest = m_curves[to].m_curve->ControlPoints()[0].m_pt;

	vector<WeightedPt> itCtr = m_curves[it].m_curve->ControlPoints();
	vector<WeightedPt> toCtr = m_curves[to].m_curve->ControlPoints();

	int itSize = m_curves[it].m_curve->polygonSize();

	CCagdPoint dir = toCtr[1].m_pt	- toCtr[0].m_pt;
	CCagdPoint origDir = itCtr[itSize - 1].m_pt - itCtr[itSize - 2].m_pt;

	double reqDer = (m_curves[to].m_curve->polygonSize() - 1) * length(dir);
	double curDer = (m_curves[it].m_curve->polygonSize() - 1) * length(origDir);
	double scaleFactor = abs(reqDer / curDer);
	

	if (doScale)
		first = U::scalePoly(first, scaleFactor);
	if (doRotate)
		first = U::rotatePolyRoundLastPt(first, dir);

	first = U::translateLastPointTo(first, dest);
	m_curves[it].m_curve->SetPoly(first);
	RedrawCurve(it);
}

//=============================================================================
// Misc
//=============================================================================

bool CurveMgr::showGrid(double density)
{
	m_showGrid = !m_showGrid;
	for (int i = 0; i < m_grid.size(); i++)
	{
		cagdFreeSegment(m_grid[i]);
	}
	m_grid.clear();
	for (double i = -10; i <= 10; i += density)
	{
		CCagdPoint vert[2];
		CCagdPoint hor[2];
		hor[0] = CCagdPoint(10, i, 0);
		hor[1] = CCagdPoint(-10, i, 0);
		vert[0] = CCagdPoint(i, 10, 0);
		vert[1] = CCagdPoint(i, -10, 0);

		UINT id = cagdAddPolyline(hor, 2, CAGD_SEGMENT_POLYLINE);
		cagdSetSegmentColor(id, 30, 60, 0);
		m_grid.push_back(id);
		id = cagdAddPolyline(vert, 2, CAGD_SEGMENT_POLYLINE);
		cagdSetSegmentColor(id, 30, 60, 0);
		m_grid.push_back(id);
	}
	for (int i = 0; i < m_grid.size(); i++)
	{
		if (m_showGrid)
		{
			cagdShowSegment(m_grid[i]);
		}
		else
		{
			cagdHideSegment(m_grid[i]);
		}
	}
	
	
	return m_showGrid;
}

bool CurveMgr::SetBSplineSamplingStep(double step)
{
  if(step<0.0)
  { return false; }

  for(int i=0; i<m_curves.size(); ++i)
  {
    if(SplineTypeBspline == m_curves[i].m_type)
    {
      dynamic_cast<BSpline*>(m_curves[i].m_curve)->SetSamplingStep(step);
      RedrawCurve(i);

    }
  }
}

bool CurveMgr::RaiseDegree(int curveIdx)
{
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return false; }
	if (m_curves[curveIdx].m_type != SplineTypeBezier)
		return false;
	Bezier* pb = dynamic_cast<Bezier*>(m_curves[curveIdx].m_curve);
	if (NULL == pb)
	{
		throw std::exception("Dynamic cast of Bezier failed to cast Bezier");
	}
	pb->RaiseDegree();
	m_curves[curveIdx].m_wc.clear();
	vector<WeightedPt> newCtrl = pb->ControlPoints();
	for (int i = 0; i < newCtrl.size(); i++)
	{
		WeightedPt p = newCtrl[i];
		m_curves[curveIdx].m_wc.push_back(WeightControl(p.m_pt, p.m_weight));
	}
	RedrawCurve(curveIdx);
	return true;
}

bool CurveMgr::Subdivide(int curveIdx)
{
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return false; }
	if (m_curves[curveIdx].m_type != SplineTypeBezier || m_curves[curveIdx].m_curve->polygonSize() < 2)
		return false;
	Bezier* pb = dynamic_cast<Bezier*>(m_curves[curveIdx].m_curve);
	if (NULL == pb)
	{
		throw std::exception("Dynamic cast of Bezier failed to cast Bezier");
	}

	vector<WeightedPt> secondPart = pb->Subdivide();

	//=============================================================================
	// Update first part of the curve
	//=============================================================================
	m_curves[curveIdx].m_wc.clear();
	vector<WeightedPt> newCtrl = pb->ControlPoints();
	for (int i = 0; i < newCtrl.size(); i++)
	{
		WeightedPt p = newCtrl[i];
		m_curves[curveIdx].m_wc.push_back(WeightControl(p.m_pt, p.m_weight));
	}
	RedrawCurve(curveIdx);

	//=============================================================================
	// Add second part
	//=============================================================================
	m_curves.push_back(CurveWrp(SplineTypeBezier));
	curveIdx = m_curves.size() - 1;
	m_curves[curveIdx].m_curve->SetPoly(secondPart);
	for (int i = 0; i < secondPart.size(); i++)
	{
		WeightedPt p = secondPart[i];
		m_curves[curveIdx].m_wc.push_back(WeightControl(p.m_pt, p.m_weight));
	}
	RedrawCurve(curveIdx);

	return true;
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

bool CurveMgr::ToggleShowPolygon(int curveIdx)
{
	if((curveIdx < 0) || (curveIdx >= m_curves.size()))
	{ return false; }
	m_curves[curveIdx].m_showCtrPoly = !m_curves[curveIdx].m_showCtrPoly;
	return RedrawCurve(curveIdx);
}
