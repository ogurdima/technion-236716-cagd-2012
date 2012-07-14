#include "stdafx.h"
#include "BsplineSurface.h"



BsplineSurface::BsplineSurface()
{
	m_isValid = false;
}

BsplineSurface::BsplineSurface(ParsedSurface p)
{
	m_order.m_u = p.m_order.m_u;
	m_order.m_v = p.m_order.m_v;

	m_points = p.m_points;

	// Important: copy order and points before knots, because fixEmptyKnots() uses them
	m_knots.m_u = p.m_knots.m_u;
	m_knots.m_v = p.m_knots.m_v;
	fixEmptyKnots();
	

	

	m_samplesPerCurve.m_u = 500;
	m_samplesPerCurve.m_v = 500;

	m_isoNum.m_u = 70;
	m_isoNum.m_v = 70;

	m_isValid = true;
}


BsplineSurface& BsplineSurface::operator=(const BsplineSurface& rhs)
{
	m_isoNum = rhs.m_isoNum;
	m_isValid = rhs.m_isValid;
	m_knots = rhs.m_knots;
	m_order = rhs.m_order;
	m_points = rhs.m_points;
	m_samplesPerCurve = rhs.m_samplesPerCurve;
	return *this;
}


BsplineSurface::~BsplineSurface(void)
{

}

void BsplineSurface::invalidate()
{
	m_isValid = false;
}

void BsplineSurface::fixEmptyKnots()
{
	// If knot vector is empty, then we create uniform open-end condition by
	// putting |order| knots at the ends of the interval (0,1)
	if (m_knots.m_u.empty())
	{
		int atEachSide = m_order.m_u;
		for (int i = 0; i < atEachSide; i++)
		{
			m_knots.m_u.push_back(0);
		}

		if (m_points.size() > atEachSide)
		{
			int toFill = m_points.size() - atEachSide;
			double inc = 1 / (double)(toFill + 1);
			double t = inc;
			for (int i = 0; i < toFill; i++, t += inc)
			{
				m_knots.m_u.push_back(t);
			}
		}

		for (int i = 0; i < atEachSide; i++)
		{
			m_knots.m_u.push_back(1);
		}
	}
	// same for the second dimension
	if (m_knots.m_v.empty())
	{
		int atEachSide = m_order.m_v;
		for (int i = 0; i < atEachSide; i++)
		{
			m_knots.m_v.push_back(0);
		}

		if (m_points[0].size() > atEachSide)
		{
			int toFill = m_points[0].size() - atEachSide;
			double inc = 1 / (double)(toFill + 1);
			double t = inc;
			for (int i = 0; i < toFill; i++, t += inc)
			{
				m_knots.m_v.push_back(t);
			}
		}

		for (int i = 0; i < atEachSide; i++)
		{
			m_knots.m_v.push_back(1);
		}
	}
}


void BsplineSurface::numberOfIsocurves(IsocurvesNumber n)
{
	m_isoNum = n;
}

IsocurvesNumber BsplineSurface::numberOfIsocurves()
{
	return m_isoNum;
}

void BsplineSurface::samplesPerCurve(SamplingFreq f)
{
	m_samplesPerCurve = f;
}

SamplingFreq BsplineSurface::samplesPerCurve()
{
	return m_samplesPerCurve;
}


void BsplineSurface::Draw()
{
	if (! m_isValid)
		return;

	DrawIsocurvesConstU();
	DrawIsocurvesConstV();
}

vector<vector<CCagdPoint>> BsplineSurface::transposeMatrixVectorOfPoints(vector<vector<CCagdPoint>> original)
{
	if (original.size() == 0)
		return original;
	vector<vector<CCagdPoint>> res;
	// works only for matrixes, which means no gaps are allowed (full rectangle)
	for (int column = 0; column < original[0].size(); column++)
	{
		vector<CCagdPoint> tv;
		res.push_back(tv);
		for (int row = 0; row < original.size(); row++)
		{
			res[column].push_back(original[row][column]);
		}
	}
	return res;
}

void BsplineSurface::DrawIsocurvesConstU()
{
	double lU = m_knots.m_u[m_order.m_u - 1];
	double rU = m_knots.m_u[m_knots.m_u.size() - m_order.m_u];
	double uInc = (rU - lU) / ( (double) (m_isoNum.m_u + 1) );

	double lV = m_knots.m_v[m_order.m_v];
	double rV = m_knots.m_v[m_knots.m_v.size() - m_order.m_v];
	double vInc = (rV - lV) / ( (double) (m_samplesPerCurve.m_v) );

	BSpline bsv;
	bsv.SetDegree(m_order.m_v - 1);
	bsv.SetKnotVector(m_knots.m_v);
	bsv.SetSamplingStep(vInc);

	BSpline bsu;
	bsu.SetDegree(m_order.m_u - 1);
	bsu.SetKnotVector(m_knots.m_u);
	bsu.SetSamplingStep(uInc);



	for (double t = lU; t <= rU; t += uInc)
	{
		vector<CCagdPoint> tmpctrp;
		CCagdPoint coeff;
		for (int i = 0; i < m_points.size(); i++)
		{
			bsu.SetPoly(m_points[i]);
			coeff = bsu.CalculateAtPoint(t);
			tmpctrp.push_back(coeff);
		}

		//now we have control polygon for specific u = t.
		bsv.SetPoly(tmpctrp);
		cagdSetSegmentColor(bsv.DrawCurve(), 200, 255, 200);
		tmpctrp.clear();
	}
}


void BsplineSurface::DrawIsocurvesConstV()
{
	double lV = m_knots.m_v[m_order.m_v - 1];
	double rV = m_knots.m_v[m_knots.m_v.size() - m_order.m_v];
	double vInc = (rV - lV) / ( (double) (m_isoNum.m_v + 1) );

	double lU = m_knots.m_u[m_order.m_u - 1];
	double rU = m_knots.m_u[m_knots.m_u.size() - m_order.m_u];
	double uInc = (rU - lU) / ( (double) (m_samplesPerCurve.m_u) );

	BSpline bsv;
	bsv.SetDegree(m_order.m_v - 1);
	bsv.SetKnotVector(m_knots.m_v);
	bsv.SetSamplingStep(vInc);

	BSpline bsu;
	bsu.SetDegree(m_order.m_u - 1);
	bsu.SetKnotVector(m_knots.m_u);
	bsu.SetSamplingStep(uInc);



	vector<vector<CCagdPoint>> pointsTransposed = transposeMatrixVectorOfPoints(m_points);

	for (double t = lV; t <= rV; t += vInc)
	{
		vector<CCagdPoint> tmpctrp;
		CCagdPoint coeff;
		for (int i = 0; i < pointsTransposed.size(); i++)
		{
			bsv.SetPoly(pointsTransposed[i]);
			coeff = bsv.CalculateAtPoint(t);
			tmpctrp.push_back(coeff);
		}

		//now we have control polygon for specific v = t.
		bsu.SetPoly(tmpctrp);
		cagdSetSegmentColor(bsu.DrawCurve(), 255, 200, 200);
		tmpctrp.clear();
	}
}


void BsplineSurface::SetKnotVectorU(vector<double> kv)
{
	m_knots.m_u = kv;
}
void BsplineSurface::SetKnotVectorV(vector<double> kv)
{
	m_knots.m_v = kv;
}

vector<double> BsplineSurface::KnotVectorU()
{
	return m_knots.m_u;
}

vector<double> BsplineSurface::KnotVectorV()
{
	return m_knots.m_v;
}

