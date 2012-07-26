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

	m_points = p.m_points;

	// Important: copy order and points before knots, because fixEmptyKnots() uses them
	SetKnotVectorU(p.m_knots.m_u);
	SetKnotVectorU(p.m_knots.m_v);
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

	DrawPt pt(0,0);
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

const Extents2D& BsplineSurface::GetExtentsUV() const
{
	return m_extentsUV;
}


//-----------------------------------------------------------------------------
BsplineSurface::~BsplineSurface(void)
{

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
BSpline BsplineSurface::CalcIsocurve(UVAxis axis, double t, int deriv)
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
		if(0 == deriv) 
		{
			coeff = bsOuter.CalculateAtPoint(t);
		} else 
		{
			coeff = bsOuter.DerivativeAtPoint(t, deriv);
		}
			
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
			cagdSetSegmentColor(id, 255, 255, 100);
		}
		else
		{
			cagdSetSegmentColor(id, 100, 255, 255);
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


	CCagdPoint ptu = CalcIsocurve(UVAxisV, v, 0).CalculateAtPoint(u);
	BSpline bsu = CalcIsocurve(UVAxisV, v, 1);
	CCagdPoint dSdu = bsu.CalculateAtPoint(u);
	
	CCagdPoint ptv = CalcIsocurve(UVAxisU, u, 0).CalculateAtPoint(v);
	BSpline bsv = CalcIsocurve(UVAxisU, u, 1);
	CCagdPoint dSdv = bsv.CalculateAtPoint(v);

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

//-----------------------------------------------------------------------------
void BsplineSurface::DrawPrincipalCurvatureAtPoint(double u, double v)
{
	cagdFreeSegment(m_idDir1);
	m_idDir1 = 0;
	cagdFreeSegment(m_idDir2);
	m_idDir2 = 0;


	// calculate G
	double g11 = dot(m_tangentU, m_tangentU);
	double g12 = dot(m_tangentU, m_tangentV);
	double g22 = dot(m_tangentV, m_tangentV);

	BSpline bsu = CalcIsocurve(UVAxisV, v, 2);
	CCagdPoint d2Sdu2 = bsu.CalculateAtPoint(u);
	BSpline bsv = CalcIsocurve(UVAxisU, u, 2);
	CCagdPoint d2Sdv2 = bsu.CalculateAtPoint(v);

	//dudv
	CCagdPoint d2Sdudv;
	double h = 0.01;
	if(U::NearlyEq(v, 0.0, 0.001))
	{
		bsu = CalcIsocurve(UVAxisV, v + h, 1);
		CCagdPoint p1 = bsu.CalculateAtPoint(u);
		bsu = CalcIsocurve(UVAxisV, v, 1);
		CCagdPoint p0 = bsu.CalculateAtPoint(u);
		d2Sdudv = (p1 - p0) / (h);
	}
	else
	{
		bsu = CalcIsocurve(UVAxisV, v + h, 1);
		CCagdPoint p1 = bsu.CalculateAtPoint(u);
		bsu = CalcIsocurve(UVAxisV, v - h, 1);
		CCagdPoint p0 = bsu.CalculateAtPoint(u);
		d2Sdudv = (p1 - p0) / (2*h);
	}

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


	double k1Denom = sqrt( (M-k1*F)*(M-k1*F) + (L - k1*E)*(L - k1*E) );
	double k2Denom = sqrt( (M-k2*F)*(M-k2*F) + (L - k2*E)*(L - k2*E) );

	double uk1 = (M - k1*F) / k1Denom;
	double vk1 = (L - k1*E) / k1Denom;
	CCagdPoint p3 = m_point + (normalize(m_tangentV) * uk1) + (normalize(cross(m_tangentV, m_normal)) * vk1);
	CCagdPoint direction1 = p3 - m_point;


	double uk2 = (M - k2*F) / k2Denom;
	double vk2 = (L - k2*E) / k2Denom;
	p3 = m_point + (normalize(m_tangentU) * uk2) + (normalize(cross(m_tangentU, m_normal)) * vk2);
	CCagdPoint direction2 = p3 - m_point;

	CCagdPoint vec[2];

	vec[0] = m_point;
	vec[1] = normalize(direction1) + m_point;
	m_idDir1 = cagdAddPolyline(vec, 2, CAGD_SEGMENT_POLYLINE);
	cagdSetSegmentColor(m_idDir1, 1,1,1);

	vec[0] = m_point;
	vec[1] = normalize(direction2) + m_point;
	m_idDir2 = cagdAddPolyline(vec, 2, CAGD_SEGMENT_POLYLINE);
	cagdSetSegmentColor(m_idDir2, 255,255,50);

}


