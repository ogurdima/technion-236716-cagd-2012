#pragma once

#include "cagd.h"
#include "Bezier.h"
#include "Bspline.h"
#include <vector>
#include "Curve.h"

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

enum SplineType
{
	SplineTypeUnknown,
	SplineTypeBezier,
	SplineTypeBspline
};

struct CurveWrp
{
	CurveWrp(SplineType t) 
		: m_curve(NULL), m_curveId(0), m_ctrPolyId(0), m_showCurve(true), m_showCtrPoly(true), m_type(t)
	{
		if (SplineTypeBezier == t)
		{
			m_curve = new Bezier();
		}
		else
		{
			m_curve = new BSpline();
		}
	}
	~CurveWrp()
	{
		delete m_curve;
	}
	Curve* m_curve;
	UINT m_curveId;
	UINT m_ctrPolyId;
	bool m_showCurve;
	bool m_showCtrPoly;
	SplineType m_type;
};

class CurveMgr
{
public:
	CurveMgr();
	~CurveMgr();

	int NewBezierCurve();
	int NewBsplineCurve();
	bool AddLastCtrlPt(const CCagdPoint& pt, double weight, int curveIdx);
	bool AddCtrlPt(const CCagdPoint& pt, double weight = 1, int curveIdx = -1, int polyPointIdx = -1);
	void ClearAll();
	bool RedrawCurve(int curveIdx);

	PtContext getPtContext(const CCagdPoint& p);

	void InsertCtrlPt(const CCagdPoint& p);
	int getCurveIndexByPointOnPolygon(const CCagdPoint& p);

	bool ToggleShowPolygon(int curveIdx);


	
private:
	std::vector<CurveWrp> m_curves;
	
};