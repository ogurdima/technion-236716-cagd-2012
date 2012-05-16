#pragma once

#include "cagd.h"
#include "Bezier.h"
#include "Bspline.h"
#include <vector>

enum PtContext
{
	ContextBezierPoly,
	ContextBezierCurve,
	ContextBezierPt,
	ContextBsplinePoly,
	ContextBsplineCurve,
	ContextBsplinePt,
	ContextEmpty
};

struct BezierWrp
{
	BezierWrp() 
		: m_curveId(0), m_ctrPolyId(0), m_showCurve(true), m_showCtrPoly(true)
	{}
	Bezier m_curve;
	UINT m_curveId;
	UINT m_ctrPolyId;
	bool m_showCurve;
	bool m_showCtrPoly;
};

class CurveMgr
{
public:
	CurveMgr();
	~CurveMgr();

	int NewBezierCurve();
	bool AddLastBezierCtrlPt(const CCagdPoint& pt, double weight, int curveIdx);
	bool AddBezierCtrlPt(const CCagdPoint& pt, double weight = 1, int curveIdx = -1, int polyPointIdx = -1);
	void ClearAll();
	bool RedrawCurve(int curveIdx);

	PtContext getPtContext(const CCagdPoint& p);

	void InsertBezierCtrlPt(const CCagdPoint& p);
	int getCurveIndexByPointOnPolygon(const CCagdPoint& p);

	bool ToggleShowBezierPolygon(int curveIdx);


	
private:
	std::vector<BezierWrp> m_beziers;
	
};