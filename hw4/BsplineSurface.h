#pragma once

#include "SurfaceFileParser.h"
#include "Bspline.h"
#include <map>

using std::map;

struct Order
{
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
	int m_u;
	int m_v;
};

struct SamplingFreq
{
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

class BsplineSurface
{
public:

	BsplineSurface();
	BsplineSurface(ParsedSurface p);
	~BsplineSurface(void);

	void invalidate();

	void SetKnotVectorU(vector<double> kv);
	void SetKnotVectorV(vector<double> kv);

	vector<double> KnotVectorU();
	vector<double> KnotVectorV();

	void numberOfIsocurves(IsocurvesNumber n);
	IsocurvesNumber numberOfIsocurves();

	void samplesPerCurve(SamplingFreq f);
	SamplingFreq samplesPerCurve();

	void Draw();

	BsplineSurface& operator=(const BsplineSurface& rhs);


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

	// ids of data points
	vector<int> m_dataIds;

	// Number of isocurves for each dimension
	IsocurvesNumber m_isoNum;

	//Number of samples (points) for each curve
	SamplingFreq m_samplesPerCurve;

	bool m_isValid;

	CCagdPoint m_draggedPt;

	int m_draggedPtId;

	void fixEmptyKnots();
	vector<vector<CCagdPoint>> transposeMatrixVectorOfPoints(vector<vector<CCagdPoint>> original);
	void DrawIsocurvesConstU();
	void DrawIsocurvesConstV();
	void DrawCtrlMesh();

};

