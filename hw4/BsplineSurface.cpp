#include "stdafx.h"
#include "BsplineSurface.h"
#include <assert.h>
#include "BezierMath.h"

//-----------------------------------------------------------------------------
BsplineSurface::BsplineSurface()
{
	m_isValid = false;
}

//-----------------------------------------------------------------------------
BsplineSurface::BsplineSurface(ParsedSurface p)
{
	m_draggedPtId = 0;

	m_order.m_u = p.m_order.m_u;
	m_order.m_v = p.m_order.m_v;

	SetPoints(p.m_points);

	// Important: copy order and points before knots, because fixEmptyKnots() uses them
	SetKnotVectorU(p.m_knots.m_u);
	SetKnotVectorV(p.m_knots.m_v);
	fixEmptyKnots();
	
	m_idTangentU = 0;
	m_idTangentV = 0;
	m_idNormal = 0;
	m_idDir1 = 0;
	m_idDir2 = 0;
	

	m_samplesPerCurve.m_u = 300;
	m_samplesPerCurve.m_v = 300;

	m_isoNum.m_u = 40;
	m_isoNum.m_v = 40;

	DrawPt pt(0.5,0.5);
	SetDrawPt(pt);

	m_isValid = true;
}

//-----------------------------------------------------------------------------
BsplineSurface& BsplineSurface::operator=(const BsplineSurface& rhs)
{
	m_isoNum = rhs.m_isoNum;
	m_isValid = rhs.m_isValid;
	m_knots = rhs.m_knots;
	m_order = rhs.m_order;
	m_points = rhs.m_points;
	m_samplesPerCurve = rhs.m_samplesPerCurve;
	m_draggedPt = rhs.m_draggedPt;
	m_draggedPtId = rhs.m_draggedPtId;
	m_idToIdx =	rhs.m_idToIdx;
	m_dataIds = rhs.m_dataIds;
	m_drawUV = rhs.m_drawUV;
	m_idTangentU = rhs.m_idTangentU;
	m_idTangentV = rhs.m_idTangentV;
	m_idNormal = rhs.m_idNormal;
	m_extentsUV = rhs.m_extentsUV;
	return *this;
}

//-----------------------------------------------------------------------------
void BsplineSurface::SetPoints(const vector<vector<CCagdPoint>>& pts)
{
	m_points = pts;
}

//-----------------------------------------------------------------------------
const Extents2D& BsplineSurface::GetExtentsUV() const
{
	return m_extentsUV;
}


//-----------------------------------------------------------------------------
BsplineSurface::~BsplineSurface(void)
{
	// clear all of the points
	for (int i = 0; i < m_dataIds.size(); i++)
		cagdFreeSegment(m_dataIds[i]);
	m_dataIds.clear();

	cagdFreeSegment(m_idTangentU);
	m_idTangentU = 0;
	cagdFreeSegment(m_idTangentV);
	m_idTangentV = 0;

	cagdFreeSegment(m_idDir1);
	m_idDir1 = 0;
	cagdFreeSegment(m_idDir2);
	m_idDir2 = 0;

	cagdFreeSegment(m_idNormal);
	m_idNormal = 0;
}

//-----------------------------------------------------------------------------
void BsplineSurface::invalidate()
{
	m_draggedPtId = 0;
	m_isValid = false;
}

//-----------------------------------------------------------------------------
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
		UpdateExtentsU();
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
		UpdateExtentsV();
	}
}

//-----------------------------------------------------------------------------
void BsplineSurface::numberOfIsocurves(IsocurvesNumber n)
{
	m_isoNum = n;
}

//-----------------------------------------------------------------------------
IsocurvesNumber BsplineSurface::numberOfIsocurves()
{
	return m_isoNum;
}

//-----------------------------------------------------------------------------
void BsplineSurface::samplesPerCurve(SamplingFreq f)
{
	m_samplesPerCurve = f;
}

//-----------------------------------------------------------------------------
SamplingFreq BsplineSurface::samplesPerCurve()
{
	return m_samplesPerCurve;
}

//-----------------------------------------------------------------------------
DrawPt BsplineSurface::GetDrawPt() const
{
	return m_drawUV;
}

//-----------------------------------------------------------------------------
void BsplineSurface::SetDrawPt(const DrawPt& pt)
{
	DrawPt pt1;
	// clamp the draw value to the extents of the new knot vector
	pt1.m_u = U::Clamp(pt.m_u, m_extentsUV.m_extU.m_min, m_extentsUV.m_extU.m_max);
	pt1.m_v = U::Clamp(pt.m_v, m_extentsUV.m_extV.m_min, m_extentsUV.m_extV.m_max);
	m_drawUV = pt1;
}

//-----------------------------------------------------------------------------
void BsplineSurface::OnLButtonDown(int x, int y)
{
	cagdPick(x, y);
	UINT id = 0;
	m_draggedPtId = 0;
	while (id = cagdPickNext())
	{
		if (! m_idToIdx.count(id))
		{
			continue;
		}
		UINT type = cagdGetSegmentType(id);
		if (CAGD_SEGMENT_POINT == type)
		{
			CCagdPoint p;
			cagdGetSegmentLocation(id, &p);
			m_draggedPt = p;
			m_draggedPtId = id;
		}
		else
		{
			m_draggedPtId = 0;
		}
	}
}

//-----------------------------------------------------------------------------
void BsplineSurface::OnMouseMove(CCagdPoint diff)
{
	if (! m_draggedPtId)
		return;
	
	CCagdPoint n = m_draggedPt + diff;
	cagdReusePoint(m_draggedPtId, &n);
	cagdSetSegmentColor(m_draggedPtId, 0, 255, 0);
	m_draggedPt = n;

	MatrixIdx midx = m_idToIdx[m_draggedPtId];
	m_points[midx.m_row][midx.m_col] = m_draggedPt;

	//if (m_draggedPtId)
	//{
	//	// User moved control points
	//	for (int i = 0; i < m_dataIds.size(); i++)
	//		cagdFreeSegment(m_dataIds[i]);
	//	m_dataIds.clear();
	//	DrawIsocurvesConstU();
	//	DrawIsocurvesConstV();
	//}
}

//-----------------------------------------------------------------------------
void BsplineSurface::OnLButtonUp(int x, int y)
{
	if (! m_isValid)
		return;

	if (m_draggedPtId)
	{
		// User moved control points
		for (int i = 0; i < m_dataIds.size(); i++)
			cagdFreeSegment(m_dataIds[i]);
		m_dataIds.clear();
		DrawIsocurves(UVAxisU);
		DrawIsocurves(UVAxisV);
	}
	m_draggedPtId = 0;
}

//-----------------------------------------------------------------------------
void BsplineSurface::Draw()
{
	if (! m_isValid)
		return;

	// clear all of the points
	for (int i = 0; i < m_dataIds.size(); i++)
		cagdFreeSegment(m_dataIds[i]);
	m_dataIds.clear();

	DrawSurface();
	DrawAttributesAt(m_drawUV.m_u, m_drawUV.m_v);
}

//-----------------------------------------------------------------------------
void BsplineSurface::DrawSurface()
{
	DrawCtrlMesh();
	DrawIsocurves(UVAxisU);
	DrawIsocurves(UVAxisV);
}

//-----------------------------------------------------------------------------
void BsplineSurface::DrawAttributesAt(double u, double v)
{
	DrawTangentsAtPoint(u, v);
	DrawSurfaceNormalAtPoint(u, v);
	DrawPrincipalCurvatureAtPoint(u, v);
}

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
void BsplineSurface::DrawCtrlMesh()
{
	m_idToIdx.clear();
	for (int i = 0; i < m_points.size(); i++)
	{
		for (int j = 0; j < m_points[i].size(); j++)
		{
			int id = cagdAddPoint(&(m_points[i][j]));
			cagdSetSegmentColor(id, 255, 20, 20);
			m_idToIdx.insert( std::pair<int, MatrixIdx>(id, MatrixIdx(i,j)) );
		}
	}
}

//-----------------------------------------------------------------------------
BSpline BsplineSurface::CalcIsocurve(UVAxis axis, double t)
{
	double lOuter, rOuter, incOuter;
	double lInner, rInner, incInner;

	BSpline bsOuter;
	BSpline bsInner;

	vector<vector<CCagdPoint>> ctrlPtsToUse;

	switch(axis)
	{
	case UVAxisU:
		{
			lOuter = m_knots.m_u[m_order.m_u - 1];
			rOuter = m_knots.m_u[m_knots.m_u.size() - m_order.m_u];
			incOuter = (rOuter - lOuter) / ( (double) (m_isoNum.m_u + 1) );

			lInner = m_knots.m_v[m_order.m_v - 1];
			rInner = m_knots.m_v[m_knots.m_v.size() - m_order.m_v];
			incInner = (rInner - lInner) / ( (double) (m_samplesPerCurve.m_v) );

			bsOuter.SetDegree(m_order.m_u - 1);
			bsOuter.SetKnotVector(m_knots.m_u);
			bsOuter.SetSamplingStep(incOuter);

			bsInner.SetDegree(m_order.m_v - 1);
			bsInner.SetKnotVector(m_knots.m_v);
			bsInner.SetSamplingStep(incInner);

			ctrlPtsToUse = m_points;

		}
		break;
	case UVAxisV:
		{
			lOuter = m_knots.m_v[m_order.m_v - 1];
			rOuter = m_knots.m_v[m_knots.m_v.size() - m_order.m_v];
			incOuter = (rOuter - lOuter) / ( (double) (m_isoNum.m_v + 1) );

			lInner = m_knots.m_u[m_order.m_u - 1];
			rInner = m_knots.m_u[m_knots.m_u.size() - m_order.m_u];
			incInner = (rInner - lInner) / ( (double) (m_samplesPerCurve.m_u) );

			bsOuter.SetDegree(m_order.m_v - 1);
			bsOuter.SetKnotVector(m_knots.m_v);
			bsOuter.SetSamplingStep(incOuter);

			bsInner.SetDegree(m_order.m_u - 1);
			bsInner.SetKnotVector(m_knots.m_u);
			bsInner.SetSamplingStep(incInner);

			ctrlPtsToUse = transposeMatrixVectorOfPoints(m_points);
		}
		break;
	default:
		assert(false);
	}


	vector<CCagdPoint> tmpctrp;
	CCagdPoint coeff;
	for (int i = 0; i < ctrlPtsToUse.size(); i++)
	{
		bsOuter.SetPoly(ctrlPtsToUse[i]);
		coeff = bsOuter.CalculateAtPoint(t);			
		tmpctrp.push_back(coeff);
	}

	//now we have control polygon for specific u = t.
	bsInner.SetPoly(tmpctrp);
	return bsInner;
}

//-----------------------------------------------------------------------------
void BsplineSurface::DrawIsocurves(UVAxis axis)
{
	double lOuter, rOuter, incOuter;
	double lInner, rInner, incInner;

	BSpline bsOuter;
	BSpline bsInner;

	vector<vector<CCagdPoint>> ctrlPtsToUse;

	switch(axis)
	{
	case UVAxisU:
		{
			lOuter = m_knots.m_u[m_order.m_u - 1];
			rOuter = m_knots.m_u[m_knots.m_u.size() - m_order.m_u];
			incOuter = (rOuter - lOuter) / ( (double) (m_isoNum.m_u + 1) );

			lInner = m_knots.m_v[m_order.m_v - 1];
			rInner = m_knots.m_v[m_knots.m_v.size() - m_order.m_v];
			incInner = (rInner - lInner) / ( (double) (m_samplesPerCurve.m_v) );

			bsOuter.SetDegree(m_order.m_u - 1);
			bsOuter.SetKnotVector(m_knots.m_u);
			bsOuter.SetSamplingStep(incOuter);

			bsInner.SetDegree(m_order.m_v - 1);
			bsInner.SetKnotVector(m_knots.m_v);
			bsInner.SetSamplingStep(incInner);

			ctrlPtsToUse = m_points;

		}
		break;
	case UVAxisV:
		{
			lOuter = m_knots.m_v[m_order.m_v - 1];
			rOuter = m_knots.m_v[m_knots.m_v.size() - m_order.m_v];
			incOuter = (rOuter - lOuter) / ( (double) (m_isoNum.m_v + 1) );

			lInner = m_knots.m_u[m_order.m_u - 1];
			rInner = m_knots.m_u[m_knots.m_u.size() - m_order.m_u];
			incInner = (rInner - lInner) / ( (double) (m_samplesPerCurve.m_u) );

			bsOuter.SetDegree(m_order.m_v - 1);
			bsOuter.SetKnotVector(m_knots.m_v);
			bsOuter.SetSamplingStep(incOuter);

			bsInner.SetDegree(m_order.m_u - 1);
			bsInner.SetKnotVector(m_knots.m_u);
			bsInner.SetSamplingStep(incInner);

			ctrlPtsToUse = transposeMatrixVectorOfPoints(m_points);
		}
		break;
	default:
		assert(false);
	}


	// now do the evaluation
	for (double t = lOuter; t <= rOuter; t += incOuter)
	{
		vector<CCagdPoint> tmpctrp;
		CCagdPoint coeff;
		for (int i = 0; i < ctrlPtsToUse.size(); i++)
		{
			bsOuter.SetPoly(ctrlPtsToUse[i]);
			coeff = bsOuter.CalculateAtPoint(t);			
			tmpctrp.push_back(coeff);
		}

		//now we have control polygon for specific u = t.
		bsInner.SetPoly(tmpctrp);
		UINT id = bsInner.DrawCurve();
		m_dataIds.push_back(id);
		if(axis == UVAxisU)
		{
			cagdSetSegmentColor(id, 255, 255, 0);
		}
		else
		{
			cagdSetSegmentColor(id, 0, 255, 255);
		}
		
		tmpctrp.clear();
	}

}
//
//
//void BsplineSurface::DrawIsocurvesConstU()
//{
//	double lU = m_knots.m_u[m_order.m_u - 1];
//	double rU = m_knots.m_u[m_knots.m_u.size() - m_order.m_u];
//	double uInc = (rU - lU) / ( (double) (m_isoNum.m_u + 1) );
//
//	double lV = m_knots.m_v[m_order.m_v - 1];
//	double rV = m_knots.m_v[m_knots.m_v.size() - m_order.m_v];
//	double vInc = (rV - lV) / ( (double) (m_samplesPerCurve.m_v) );
//
//	BSpline bsv;
//	bsv.SetDegree(m_order.m_v - 1);
//	bsv.SetKnotVector(m_knots.m_v);
//	bsv.SetSamplingStep(vInc);
//
//	BSpline bsu;
//	bsu.SetDegree(m_order.m_u - 1);
//	bsu.SetKnotVector(m_knots.m_u);
//	bsu.SetSamplingStep(uInc);
//
//
//
//	for (double t = lU; t <= rU; t += uInc)
//	{
//		vector<CCagdPoint> tmpctrp;
//		CCagdPoint coeff;
//		for (int i = 0; i < m_points.size(); i++)
//		{
//			bsu.SetPoly(m_points[i]);
//			coeff = bsu.CalculateAtPoint(t);
//			tmpctrp.push_back(coeff);
//		}
//
//		//now we have control polygon for specific u = t.
//		bsv.SetPoly(tmpctrp);
//		UINT id = bsv.DrawCurve();
//		m_dataIds.push_back(id);
//		cagdSetSegmentColor(id, 255, 255, 100);
//		tmpctrp.clear();
//	}
//}
//
//void BsplineSurface::DrawIsocurvesConstV()
//{
//	double lV = m_knots.m_v[m_order.m_v - 1];
//	double rV = m_knots.m_v[m_knots.m_v.size() - m_order.m_v];
//	double vInc = (rV - lV) / ( (double) (m_isoNum.m_v + 1) );
//
//	double lU = m_knots.m_u[m_order.m_u - 1];
//	double rU = m_knots.m_u[m_knots.m_u.size() - m_order.m_u];
//	double uInc = (rU - lU) / ( (double) (m_samplesPerCurve.m_u) );
//
//	BSpline bsv;
//	bsv.SetDegree(m_order.m_v - 1);
//	bsv.SetKnotVector(m_knots.m_v);
//	bsv.SetSamplingStep(vInc);
//
//	BSpline bsu;
//	bsu.SetDegree(m_order.m_u - 1);
//	bsu.SetKnotVector(m_knots.m_u);
//	bsu.SetSamplingStep(uInc);
//
//	
//
//	vector<vector<CCagdPoint>> pointsTransposed = transposeMatrixVectorOfPoints(m_points);
//
//	for (double t = lV; t <= rV; t += vInc)
//	{
//		vector<CCagdPoint> tmpctrp;
//		CCagdPoint coeff;
//		for (int i = 0; i < pointsTransposed.size(); i++)
//		{
//			bsv.SetPoly(pointsTransposed[i]);
//			coeff = bsv.CalculateAtPoint(t);
//			tmpctrp.push_back(coeff);
//		}
//
//		//now we have control polygon for specific v = t.
//		bsu.SetPoly(tmpctrp);
//		UINT id = bsu.DrawCurve();
//		m_dataIds.push_back(id);
//		cagdSetSegmentColor(id, 100, 255, 255);
//		tmpctrp.clear();
//	}
//}


//-----------------------------------------------------------------------------
void BsplineSurface::SetKnotVectorU(vector<double> kv)
{
	m_knots.m_u = kv;

	// clamp the draw value to the extents of the new knot vector
	UpdateExtentsU();
}

//-----------------------------------------------------------------------------
void BsplineSurface::SetKnotVectorV(vector<double> kv)
{
	m_knots.m_v = kv;

	// clamp the draw value to the extents of the new knot vector	
	UpdateExtentsV();
}

//-----------------------------------------------------------------------------
void BsplineSurface::UpdateExtentsU()
{
	if(m_knots.m_u.size())
	{
		int deg_u = m_order.m_u-1;
		m_extentsUV.m_extU.m_min = m_knots.m_u[deg_u];
		m_extentsUV.m_extU.m_max = m_knots.m_u[m_knots.m_u.size() - 1 - deg_u];
	}
}

//-----------------------------------------------------------------------------
void BsplineSurface::UpdateExtentsV()
{
	if(m_knots.m_v.size())
	{
		int deg_v = m_order.m_v-1;
		m_extentsUV.m_extV.m_min = m_knots.m_v[deg_v];
		m_extentsUV.m_extV.m_max = m_knots.m_v[m_knots.m_v.size() - 1 - deg_v];
	}
}
//-----------------------------------------------------------------------------
vector<double> BsplineSurface::KnotVectorU()
{
	return m_knots.m_u;
}

//-----------------------------------------------------------------------------
vector<double> BsplineSurface::KnotVectorV()
{
	return m_knots.m_v;
}

//-----------------------------------------------------------------------------
CCagdPoint BsplineSurface::FirstDerivU(double t)
{

	//BSpline bsv;
	//bsv.SetDegree(m_order.m_v - 1);
	//bsv.SetKnotVector(m_knots.m_v);
	//bsv.SetSamplingStep(vInc);

	//BSpline bsu;
	//bsv.SetDegree(m_order.m_v - 1);
	//bsv.SetKnotVector(m_knots.m_v);
	//bsv.SetSamplingStep(vInc);

	return CCagdPoint(0,0,0);
}

//-----------------------------------------------------------------------------
CCagdPoint BsplineSurface::DerivativeAtPoint(Deriv der, double u, double v)
{

	switch(der)
	{
	case du:
		{
			BSpline isocurve = CalcIsocurve(UVAxisV, v);
			return isocurve.DerivativeAtPoint(u, 1);
		}
		break;
	case dv:
		{
			BSpline isocurve = CalcIsocurve(UVAxisU, u);
			return isocurve.DerivativeAtPoint(v, 1);
		}
		break;
	case d2u:
		{
			BSpline isocurve = CalcIsocurve(UVAxisV, v);
			return isocurve.DerivativeAtPoint(u, 2);
		}
		break;
	case d2v:
		{
			BSpline isocurve = CalcIsocurve(UVAxisU, u);
			return isocurve.DerivativeAtPoint(v, 2);
		}
		break;
	case dudv:
		{
			double h=0.01;
			double lo = v-h;
			double hi = v+h;
			double sum = 2*h;
			if(lo < m_extentsUV.m_extV.m_min)
			{
				lo = v;
				sum = h;
			}

			if(hi > m_extentsUV.m_extV.m_max)
			{
				hi = v;
				sum -= h;
				if(U::NearlyEq(sum, 0.0))
				{
					assert(false);
				}
			}
			BSpline isocurvePlus = CalcIsocurve(UVAxisV, hi);
			CCagdPoint pPlus = isocurvePlus.DerivativeAtPoint(u, 1);
			BSpline isocurveMinus = CalcIsocurve(UVAxisV, lo);
			CCagdPoint pMinus = isocurveMinus.DerivativeAtPoint(u, 1);
			CCagdPoint res = (pPlus-pMinus) / sum;
			return res;
		}
		break;
	default:
		assert(false);
	}
}
//-----------------------------------------------------------------------------
CCagdPoint BsplineSurface::FirstDerivV(double t)
{
	return CCagdPoint(0,0,0);
}

//-----------------------------------------------------------------------------
void BsplineSurface::DrawTangentsAtPoint(double u, double v)
{
	cagdFreeSegment(m_idTangentU);
	m_idTangentU = 0;
	cagdFreeSegment(m_idTangentV);
	m_idTangentV = 0;

	BSpline bsu = CalcIsocurve(UVAxisV, v);
	CCagdPoint ptu = bsu.CalculateAtPoint(u);
	CCagdPoint dSdu = bsu.DerivativeAtPoint(u, 1);

	BSpline bsv = CalcIsocurve(UVAxisU, u);
	CCagdPoint ptv = bsv.CalculateAtPoint(v);
	CCagdPoint dSdv = bsv.DerivativeAtPoint(v, 1);

	CCagdPoint tangentU[2];
	tangentU[0] = ptu;
	tangentU[1] = ptu + normalize(dSdu);
	CCagdPoint tangentV[2];
	tangentV[0] = ptv;
	tangentV[1] = ptv + normalize(dSdv);
	
	UINT idu = cagdAddPolyline(tangentU, 2, CAGD_SEGMENT_POLYLINE);
	cagdSetSegmentColor(idu, 255, 0, 0);
	UINT idv = cagdAddPolyline(tangentV, 2, CAGD_SEGMENT_POLYLINE);
	cagdSetSegmentColor(idv, 0, 0, 255);

	m_point = ptu;
	m_tangentU = dSdu;
	m_tangentV = dSdv;
	m_idTangentU = idu;
	m_idTangentV = idv;

}

//-----------------------------------------------------------------------------
void BsplineSurface::DrawSurfaceNormalAtPoint(double u, double v)
{
	cagdFreeSegment(m_idNormal);
	m_idNormal = 0;

	m_normal = normalize(cross(m_tangentU, m_tangentV));
	CCagdPoint norm[2];
	norm[0] = m_point;
	norm[1] = m_point + m_normal;
	m_idNormal = cagdAddPolyline(norm, 2, CAGD_SEGMENT_POLYLINE);
	cagdSetSegmentColor(m_idNormal, 255, 128, 0);
}

//-----------------------------------------------------------------------------
void BsplineSurface::DrawTangentPlaneAtPoint(double u, double v)
{
}

#include <iosfwd>
#include <fstream>
//-----------------------------------------------------------------------------
void BsplineSurface::DrawPrincipalCurvatureAtPoint(double u, double v)
{
	cagdFreeSegment(m_idDir1);
	m_idDir1 = 0;
	cagdFreeSegment(m_idDir2);
	m_idDir2 = 0;

	//double maxXdiff = 0;
	//bool writeToFile = true;
	//{
	//	/*BSpline splineC = CalcIsocurve(UVAxisV, 0, 0);		
	//	BSpline splinedCdu = CalcIsocurve(UVAxisV, 0, 1);*/
	//	
	//	std::ofstream fd("fd1.csv");
	//	std::ofstream fd2("fd2.csv");
	//	std::ofstream fd3("fd3.csv");
	//	if(fd && fd2 && fd3)
	//	{
	//		for(double u=0.0; u<0.999; u+=0.01)
	//		{
	//			CCagdPoint pt = DerivativeAtPoint(d2u, u, 0.5);
	//			fd << pt.x << "," << pt.y << "," << pt.z << std::endl;

	//			CCagdPoint pt2 = DerivativeAtPoint(du, 0.5, u/*this is v*/);
	//			fd2 << pt2.x << "," << pt2.y << "," << pt2.z << std::endl;

	//			CCagdPoint pt3 = DerivativeAtPoint(dudv, 0.5, u/*this is v*/);
	//			fd3 << pt3.x << "," << pt3.y << "," << pt3.z << std::endl;

	//			if (maxXdiff < abs(pt.x - pt2.x))
	//				maxXdiff = abs(pt.x - pt2.x);
	//		}



	//		fd.close();
	//		fd2.close();
	//		fd3.close();
	//	}
	//}


	// calculate G
	double g11 = dot(m_tangentU, m_tangentU);
	double g12 = dot(m_tangentU, m_tangentV);
	double g22 = dot(m_tangentV, m_tangentV);

	// ds2du2
	CCagdPoint d2Sdu2 = DerivativeAtPoint(d2u, u, v);
	
	// ds2dv2
	CCagdPoint d2Sdv2 = DerivativeAtPoint(d2v, u, v);

	//dudv
	CCagdPoint d2Sdudv = DerivativeAtPoint(dudv, u, v);

	// II
	double l11 = dot(d2Sdu2, m_normal);
	double l22 = dot(d2Sdv2, m_normal);
	double l12 = dot(d2Sdudv, m_normal);

	// Different names
	double E = g11;
	double F = g12;
	double G = g22;
	double L = l11;
	double M = l12;
	double N = l22;

	// Quadratic equation
	double a = ( E*G - F*F );
	double b = -( G*L + E*N -2*F*M );
	double c = ( L*N - M*M );
	double descriminant = ( b*b - 4*a*c);

	// Roots
	double k1 = (-b - sqrt(descriminant)) / (2*a);
	double k2 = (-b + sqrt(descriminant)) / (2*a);

	//if(!U::NearlyEq(k1, 0.0))
	//{
	//	return;
	//}

	//if(U::NearlyEq(k2, 0.0))
	//{
	//	return;
	//}


	double k1Denom = sqrt( (M-k1*F)*(M-k1*F) + (L - k1*E)*(L - k1*E) );
	double k2Denom = sqrt( (M-k2*F)*(M-k2*F) + (L - k2*E)*(L - k2*E) );

	double uk1 = (M - k1*F) / k1Denom;
	double vk1 = (L - k1*E) / k1Denom;
	double uk1n = uk1 / sqrt(uk1*uk1 + vk1*vk1);
	double vk1n = vk1 / sqrt(uk1*uk1 + vk1*vk1);
	CCagdPoint p3 = m_point + (normalize(m_tangentU) * uk1n) + (normalize(m_tangentV) * vk1n);
	CCagdPoint direction1 = p3 - m_point;


	double uk2 = (M - k2*F) / k2Denom;
	double vk2 = (L - k2*E) / k2Denom;
	double uk2n = uk2 / sqrt(uk2*uk2 + vk2*vk2);
	double vk2n = vk2 / sqrt(uk2*uk2 + vk2*vk2);
	p3 = m_point + (normalize(m_tangentU) * uk2n) + (normalize(m_tangentV) * vk2n);
	CCagdPoint direction2 = p3 - m_point;

	CCagdPoint vec[2];

	vec[0] = m_point;
	vec[1] = 0.5*normalize(direction1) + m_point;
	m_idDir1 = cagdAddPolyline(vec, 2, CAGD_SEGMENT_POLYLINE);
	cagdSetSegmentColor(m_idDir1, 1,1,1);

	vec[0] = m_point;
	vec[1] = 0.5*normalize(direction2) + m_point;
	m_idDir2 = cagdAddPolyline(vec, 2, CAGD_SEGMENT_POLYLINE);
	cagdSetSegmentColor(m_idDir2, 255,50,255);

}


