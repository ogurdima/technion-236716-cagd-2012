#pragma once

#include "SurfaceFileParser.h"
#include "Bspline.h"
#include <map>

using std::map;

struct Order
{
	Order() : m_u(0), m_v(0){}
	Order(int u, int v) : m_u(u), m_v(v){}
	int m_u;
	int m_v;
};

struct Knots
{
	vector<double> m_u;
	vector<double> m_v;
};

struct IsocurvesNumber
{
	IsocurvesNumber(): m_u(0), m_v(0) {}
	IsocurvesNumber(int u, int v): m_u(u), m_v(v) {}
	int m_u;
	int m_v;
};

struct SamplingFreq
{
	SamplingFreq(): m_u(0), m_v(0) {}
	SamplingFreq(int u, int v): m_u(u), m_v(v) {}
	int m_u;
	int m_v;
};

struct MatrixIdx
{
	MatrixIdx() : m_row(-1), m_col(-1) {}
	MatrixIdx(int i, int j) : m_row(i), m_col(j) {}
	int m_row;
	int m_col;
};

struct UVspace
{
	UVspace() : m_u(0), m_v(0) {}
	UVspace(double u, double v) : m_u(u), m_v(v) {}
	double m_u;
	double m_v;
};

struct DrawPt
{
	DrawPt() : m_u(0.0), m_v(0.0) {}
	DrawPt(double u, double v) : m_u(u), m_v(v) {}
	double m_u;
	double m_v;
};

struct Extents2D
{
	Extents1D m_extU;
	Extents1D m_extV;
};

class BsplineSurface
{
public:

	BsplineSurface();
	BsplineSurface(ParsedSurface p);
	~BsplineSurface(void);

	void invalidate();

	void ClearSegments(void);

	void SetPoints(const vector<vector<CCagdPoint>>& pts);

	void SetKnotVectorU(vector<double> kv);
	void SetKnotVectorV(vector<double> kv);
	
	vector<double> KnotVectorU();
	vector<double> KnotVectorV();

	void SetOrder(Order orderUV);
	Order GetOrder() const;

	void numberOfIsocurves(IsocurvesNumber n);
	IsocurvesNumber numberOfIsocurves();

	void samplesPerCurve(SamplingFreq f);
	SamplingFreq samplesPerCurve();

	DrawPt GetDrawPt() const;
	void SetDrawPt(const DrawPt& pt);
	
	void Draw();

	const Extents2D& GetExtentsUV() const;
	BsplineSurface& operator=(const BsplineSurface& rhs);

	enum Deriv
	{
		du,
		dv,
		d2u,
		d2v,
		dudv
	};
	CCagdPoint DerivativeAtPoint(Deriv der, double u, double v);
	CCagdPoint CalculateAtPoint(double u, double v);

	CCagdPoint FirstDerivU(double t);
	CCagdPoint FirstDerivV(double t);

	void OnLButtonDown(int x, int y);

	void OnMouseMove(CCagdPoint diff);

	void OnLButtonUp(int x = 0, int y = 0);

private:
	//order for u and v splines
	Order m_order;

	// u and v knot vectors
	Knots m_knots;

	// control points
	vector<vector<CCagdPoint>> m_points;

	// ids of control points
	map<int, MatrixIdx> m_idToIdx;

	map<int, UVspace> m_invisIdToUV;

	map<CCagdPoint, UVspace> m_3dToUV;

	// ids of data points
	vector<int> m_dataIds;

	// Number of isocurves for each dimension
	IsocurvesNumber m_isoNum;

	//Number of samples (points) for each curve
	SamplingFreq m_samplesPerCurve;

	bool m_isValid;

	CCagdPoint m_draggedPt;
	int m_draggedPtId;

	UVspace m_animStart;
	UVspace m_animEnd;
	bool m_setAnimStart;

	void fixEmptyKnotsU();
	void fixEmptyKnotsV();
	vector<vector<CCagdPoint>> transposeMatrixVectorOfPoints(vector<vector<CCagdPoint>> original);
	//void DrawIsocurvesConstU();
	//void DrawIsocurvesConstV();

	// top-level drawing functions
	void DrawSurface();
	void DrawAttributesAt(double u, double v);


	// individual drawing/calculating functions
	void DrawCtrlMesh();
	void DrawInvisiblePoints();
	BSpline CalcIsocurve(UVAxis axis, double t);
	void DrawIsocurves(UVAxis axis);
	void DrawTangentsAtPoint(double u, double v);
	void DrawSurfaceNormalAtPoint(double u, double v);
	void DrawTangentPlaneAtPoint(double u, double v);
	void DrawPrincipalCurvatureAtPoint(double u, double v);

	void BsplineSurface::UpdateExtentsU();
	void BsplineSurface::UpdateExtentsV();

	CCagdPoint CalcTangentAtPoint(UVAxis axis, double u, double v);
	CCagdPoint CalcSurfacePoint(double u, double v);
	CCagdPoint CalcNumNormalDeriv(double u, double v);


	void PickCtrlMeshPoint(int id);
	void PickInvisiblePoint(int id);
	void Pick3dPoint(int x, int y, int id);

	Extents2D m_extentsUV;

	// where to draw tangents, and all surface attributes. Only use x,y
	DrawPt m_drawUV;

	CCagdPoint m_tangentU;
	CCagdPoint m_tangentV;
	CCagdPoint m_normal;
	CCagdPoint m_point;

	UINT m_idTangentU;
	UINT m_idTangentV;
	UINT m_idNormal;
	UINT m_idDir1;
	UINT m_idDir2;

};

