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

struct ControlPointInfo
{
	ControlPointInfo() : m_curveIdx(-1), m_pointIdx(-1) {}
	ControlPointInfo(int curveIdx, int ptIdx) : m_curveIdx(curveIdx), m_pointIdx(ptIdx) {}
	void Invalidate() { *this = ControlPointInfo(); }
	bool IsValid() const { return (0 <= m_curveIdx) && (0 <= m_pointIdx); }
	int m_curveIdx;
	int m_pointIdx;
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
	CurveWrp(const CurveWrp& rhs)
	{
		m_curveId		= rhs.m_curveId;
		m_ctrPolyId		= rhs.m_ctrPolyId;
		m_showCurve		= rhs.m_showCurve;
		m_showCtrPoly	= rhs.m_showCtrPoly;
		m_type			= rhs.m_type;	

		if (SplineTypeBezier == m_type)
		{
			m_curve = new Bezier();
		}
		else
		{
			m_curve = new BSpline();
		}

		*m_curve		= *rhs.m_curve;
	}
	~CurveWrp()
	{
		delete m_curve;
	}
	CurveWrp& operator=(const CurveWrp& rhs)
	{
		*m_curve		= *rhs.m_curve;
		m_curveId		= rhs.m_curveId;
		m_ctrPolyId		= rhs.m_ctrPolyId;
		m_showCurve		= rhs.m_showCurve;
		m_showCtrPoly	= rhs.m_showCtrPoly;
		m_type			= rhs.m_type;		
		return *this;
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
	bool UpdateCtrlPtPos(const ControlPointInfo& ptInfo, const CCagdPoint& pt);
	bool UpdateCtrlPtWeight(const ControlPointInfo& ptInfo, double weight);
	void ClearAll();
	bool RedrawCurve(int curveIdx);

	PtContext getPtContext(const CCagdPoint& p);

	/// using screen coordinates, looks through all points in all curves for a match.
	ControlPointInfo PickControlPoint(int x, int y) const;

	int getCurveIndexByPointOnPolygon(const CCagdPoint& p);

	bool ToggleShowPolygon(int curveIdx);


	
private:
	std::vector<CurveWrp> m_curves;
	
};