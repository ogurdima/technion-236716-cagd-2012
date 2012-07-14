#pragma once

#include "SurfaceFileParser.h"
#include "Bspline.h"

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


private:
	//order for u and v splines
	Order m_order;

	// u and v knot vectors
	Knots m_knots;

	// control points
	vector<vector<CCagdPoint>> m_points;

	// Number of isocurves for each dimension
	IsocurvesNumber m_isoNum;

	//Number of samples (points) for each curve
	SamplingFreq m_samplesPerCurve;

	bool m_isValid;

	void fixEmptyKnots();
	vector<vector<CCagdPoint>> transposeMatrixVectorOfPoints(vector<vector<CCagdPoint>> original);
	void DrawIsocurvesConstU();
	void DrawIsocurvesConstV();

};

