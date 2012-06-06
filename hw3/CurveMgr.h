#pragma once

#include "cagd.h"
#include "Bezier.h"
#include "Bspline.h"
#include <vector>
#include "Curve.h"
#include "assert.h"

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
	SplineTypeBspline,
	SplineTypeCount
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

struct WeightControl
{
	WeightControl() : m_id(0), m_radius(0), m_show(false), m_center(0,0,0) {};
	~WeightControl() 
	{
		cagdFreeSegment(m_id);
	}
	WeightControl(CCagdPoint center, double radius, bool show = false) 
		: m_id(0), m_radius(radius), m_show(show), m_center(center)
	{
		if (m_show)
			Show();
	}
	void SetCenter(CCagdPoint center)
	{
		m_center = center;
		if (m_show)
			Show();
	}
	CCagdPoint Center()
	{
		return m_center;
	}
	void SetRadius(double radius)
	{
		m_radius = radius;
		if (m_show)
			Show();
	}
	double Radius()
	{
		return m_radius;
	}
	void SetShow(bool show)
	{
		m_show = show;
		if (m_show)
			Show();
	}
	bool ToggleShow()
	{
		m_show = !m_show;
		if (m_show)
			Show();
		else
			Remove();
		return m_show;
	}
	void Remove()
	{
		cagdFreeSegment(m_id);
		m_id = NULL;
	}
	void Show()
	{
		cagdFreeSegment(m_id);
		m_id = DrawCircle(m_center, m_center + CCagdPoint(m_radius,0,0), CCagdPoint(0,0,1), m_radius);
		cagdSetSegmentColor(m_id, 255,255,0);
	}
	bool IsUnderCursor(int x, int y)
	{
		cagdPick(x, y);
		while (UINT someId = cagdPickNext())
		{
			if (someId == m_id)
				return true;
		}
		return false;
	}
private:
	UINT m_id;
	double m_radius;
	CCagdPoint m_center;
	bool m_show;
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
		else if(SplineTypeBspline == t)
		{
			m_curve = new BSpline();
		}
		else
		{
			assert((SplineTypeUnknown < t) && (SplineTypeCount > t));
		}
	}
	CurveWrp(const CurveWrp& rhs)
	{
		m_curveId		= rhs.m_curveId;
		m_ctrPolyId		= rhs.m_ctrPolyId;
		m_showCurve		= rhs.m_showCurve;
		m_showCtrPoly	= rhs.m_showCtrPoly;
		m_type			= rhs.m_type;	
		m_wc			= rhs.m_wc;
		if (SplineTypeBezier == m_type)
		{
			m_curve = new Bezier();
			*dynamic_cast<Bezier*>(m_curve)		= *dynamic_cast<Bezier*>(rhs.m_curve);
		}
		else
		{
			m_curve = new BSpline();
			*dynamic_cast<BSpline*>(m_curve)		= *dynamic_cast<BSpline*>(rhs.m_curve);
		}

		
	}
	~CurveWrp()
	{
		delete m_curve;
	}
	CurveWrp& operator=(const CurveWrp& rhs)
	{
		m_curveId		= rhs.m_curveId;
		m_ctrPolyId		= rhs.m_ctrPolyId;
		m_showCurve		= rhs.m_showCurve;
		m_showCtrPoly	= rhs.m_showCtrPoly;
		m_type			= rhs.m_type;	
		m_wc			= rhs.m_wc;
		if (SplineTypeBezier == m_type)
		{
			*dynamic_cast<Bezier*>(m_curve)		= *dynamic_cast<Bezier*>(rhs.m_curve);
		}
		else
		{
			*dynamic_cast<BSpline*>(m_curve)		= *dynamic_cast<BSpline*>(rhs.m_curve);
		}
		return *this;
	}
	Curve* m_curve;
	UINT m_curveId;
	UINT m_ctrPolyId;
	bool m_showCurve;
	bool m_showCtrPoly;
	SplineType m_type;
	vector<WeightControl> m_wc;
};

class CurveMgr
{
public:
	CurveMgr();
	~CurveMgr();

	int NewBezierCurve();
	int NewBsplineCurve(unsigned int order);
	bool AddLastCtrlPt(const CCagdPoint& pt, double weight, int curveIdx, bool noRecalc = false);
	bool AddCtrlPt(const CCagdPoint& pt, double weight = 1, int curveIdx = -1, int polyPointIdx = -1, bool noRecalc = false);
	bool RemoveCtrlPt(const CCagdPoint& pt);
	bool UpdateCtrlPtPos(const ControlPointInfo& ptInfo, const CCagdPoint& pt);
	bool UpdateCtrlPtWeight(const ControlPointInfo& ptInfo, double weight);
	void ClearAll();
	bool RedrawCurve(int curveIdx);

	PtContext getPtContext(const CCagdPoint& p);

	/// using screen coordinates, looks through all points in all curves for a match.
	ControlPointInfo PickControlPoint(int x, int y) const;

	int getCurveIndexByPointOnPolygon(const CCagdPoint& p);

	bool ToggleShowPolygon(int curveIdx);
	bool ToggleWeightConrol(const CCagdPoint& p);
	void ChangeWeight(int curveIdx, int ptIdx, int x, int y);
	ControlPointInfo AttemptWeightAnchor(int x, int y);

	// for bsplines. returns false if curve is not a bspline or if kv is not monotonic
	bool SetKnotVector(int curveIdx, const vector<double> & kv);

	// for bsplines. returns empty vector if curve is not a bspline
	std::vector<double> GetKnotVector(int curveIdx);

  // for bsplines
  bool InsertKnot(int curveIdx, double knotVal);

	// for beziers
	bool RaiseDegree(int curveIdx);
	bool Subdivide(int curveIdx);

	string toDat() const;
	string toIrit() const;

	bool showGrid(double density = 1);

	void connectG0(int it, int to);
	void connectG1(int it, int to);
	void connectC1(int it, int to);

	
private:

	void connectCustom(int it, int to, bool doScale, bool doRotate);

	std::vector<CurveWrp> m_curves;
	std::vector<UINT> m_grid;
	bool m_showGrid;
};