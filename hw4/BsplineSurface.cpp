#include "stdafx.h"
#include "BsplineSurface.h"
#include <assert.h>
#include "BezierMath.h"
#include "cagd.h" 

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
	
	
	m_idTangentU = 0;
	m_idTangentV = 0;
	m_idNormal = 0;
	m_idDir1 = 0;
	m_idDir2 = 0;
	
	m_setAnimStart = true;
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
	m_invisIdToUV = rhs.m_invisIdToUV;
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
void BsplineSurface::invalidate()
{
	m_draggedPtId = 0;
	m_isValid = false;
}

//-----------------------------------------------------------------------------
void BsplineSurface::ClearSegments(void)
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
BsplineSurface::~BsplineSurface(void)
{
	ClearSegments();
}

//-----------------------------------------------------------------------------
void BsplineSurface::fixEmptyKnotsU()
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

		if (m_points[0].size() > atEachSide)
		{
			int toFill = m_points[0].size() - atEachSide;
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
}
void BsplineSurface::fixEmptyKnotsV()
{
	// same for the second dimension
	if (m_knots.m_v.empty())
	{
		int atEachSide = m_order.m_v;
		for (int i = 0; i < atEachSide; i++)
		{
			m_knots.m_v.push_back(0);
		}

		if (m_points.size() > atEachSide)
		{
			int toFill = m_points.size() - atEachSide;
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
		if (m_idToIdx.count(id))
		{
			PickCtrlMeshPoint(id);
			return;
		}
		if (m_invisIdToUV.count(id))
		{
			PickInvisiblePoint(id);
			return;
		}
		for (int i = 0; i < m_dataIds.size(); i++)
		{
			if (id == m_dataIds[i])
			{
				Pick3dPoint(x, y, id);
				return;
			}
		}
		
	}
}


void BsplineSurface::Pick3dPoint(int x, int y, int id)
{
	int vertexIdx = cagdGetNearestVertex(id, x, y);
	int len = cagdGetSegmentLength(id);
	CCagdPoint* ptr = (CCagdPoint*) malloc(sizeof(CCagdPoint) * len);

	cagdGetSegmentLocation(id, ptr);
	CCagdPoint _3dp = ptr[vertexIdx];
	
	if (m_setAnimStart)
	{
		m_animStart = m_3dToUV[_3dp];
	}
	else
	{
		m_animEnd = m_3dToUV[_3dp];
	}
	m_setAnimStart = ! m_setAnimStart;

	free(ptr);
}

void BsplineSurface::PickCtrlMeshPoint(int id)
{
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

void BsplineSurface::PickInvisiblePoint(int id)
{
	if (m_setAnimStart)
	{
		m_animStart = m_invisIdToUV[id];
	}
	else
	{
		m_animEnd = m_invisIdToUV[id];
	}
	m_setAnimStart = ! m_setAnimStart;
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
	DrawSurface();
	DrawAttributesAt(m_drawUV.m_u, m_drawUV.m_v);
}

//-----------------------------------------------------------------------------
void BsplineSurface::DrawAttributesOnly()
{
	DrawAttributesAt(m_drawUV.m_u, m_drawUV.m_v);
}

//-----------------------------------------------------------------------------
void BsplineSurface::DrawSurface()
{
	// clear all of the points
	for (int i = 0; i < m_dataIds.size(); i++)
		cagdFreeSegment(m_dataIds[i]);
	m_dataIds.clear();

	m_3dToUV.clear();

	if (! m_isValid)
		return;
	if (! m_extentsUV.m_extU.m_min < m_extentsUV.m_extU.m_max ||
		! m_extentsUV.m_extV.m_min < m_extentsUV.m_extV.m_max)
	{
		return;
	}

	DrawCtrlMesh();
	DrawIsocurves(UVAxisU);
	DrawIsocurves(UVAxisV);
}

//-----------------------------------------------------------------------------
void BsplineSurface::DrawAttributesAt(double u, double v)
{
	if (! m_isValid)
		return;
	if (! m_extentsUV.m_extU.m_min < m_extentsUV.m_extU.m_max ||
		! m_extentsUV.m_extV.m_min < m_extentsUV.m_extV.m_max)
	{
		return;
	}

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

	if (0 == incOuter || 0 == incInner)
		return;

// now do the evaluation
	double t = lOuter;
	for (int i = 0; i < 2; i++)
	{
		for (; t <= rOuter; t += incOuter)
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

			vector<double> idToInnerArg = bsInner.GetIdxToArg();
			vector<CCagdPoint> dataInner = bsInner.DataPoints();
			for (int i = 0; i < idToInnerArg.size(); i++)
			{
				UVspace dest = (axis == UVAxisU) ? UVspace(t, idToInnerArg[i]) : UVspace(idToInnerArg[i], t);
				m_3dToUV.insert(std::pair<CCagdPoint, UVspace>(dataInner[i], dest) );
			}

			if(0 != id)
			{
				m_dataIds.push_back(id);
				if(axis == UVAxisU)
				{
					cagdSetSegmentColor(id, 255, 255, 0);
				}
				else
				{
					cagdSetSegmentColor(id, 0, 255, 255);
				}
			}		
			tmpctrp.clear();
		}
		t = rOuter - incOuter/20.0;
	}

}

//-----------------------------------------------------------------------------
void BsplineSurface::SetKnotVectorU(vector<double> kv)
{
	m_knots.m_u = kv;

	// clamp the draw value to the extents of the new knot vector
	fixEmptyKnotsU();
	UpdateExtentsU();
}

//-----------------------------------------------------------------------------
void BsplineSurface::SetKnotVectorV(vector<double> kv)
{
	m_knots.m_v = kv;

	// clamp the draw value to the extents of the new knot vector	
	fixEmptyKnotsV();
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

void BsplineSurface::SetOrder(Order orderUV)
{
	m_order = orderUV;
}

Order BsplineSurface::GetOrder() const
{
	return m_order;
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

CCagdPoint BsplineSurface::CalculateAtPoint(double u, double v)
{
	BSpline isocurve = CalcIsocurve(UVAxisV, v);
	return isocurve.CalculateAtPoint(u);
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

	CCagdPoint ptu;
	try
	{
		ptu = CalcSurfacePoint(u,v);
	}
	catch(const std::exception& e)
	{
		m_tangentU = CCagdPoint(0,0,0);
		m_tangentV = CCagdPoint(0,0,0);
		return;
	}
	CCagdPoint dSdu = CalcTangentAtPoint(UVAxisU, u, v);
	CCagdPoint dSdv = CalcTangentAtPoint(UVAxisV, u, v);

	CCagdPoint tangentU[2];
	tangentU[0] = ptu;
	tangentU[1] = ptu + normalize(dSdu);
	CCagdPoint tangentV[2];
	tangentV[0] = ptu;
	tangentV[1] = ptu + normalize(dSdv);
	
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

CCagdPoint BsplineSurface::CalcTangentAtPoint(UVAxis axis, double u, double v)
{
	if (UVAxisU == axis)
	{
		BSpline bsu = CalcIsocurve(UVAxisV, v);
		CCagdPoint dSdu = bsu.DerivativeAtPoint(u, 1);
		return dSdu;
	}
	if (UVAxisV == axis)
	{
		BSpline bsv = CalcIsocurve(UVAxisU, u);
		CCagdPoint dSdv = bsv.DerivativeAtPoint(v, 1);
		return dSdv;
	}
	assert(false);
	return CCagdPoint();
}

CCagdPoint BsplineSurface::CalcSurfacePoint(double u, double v)
{
	BSpline bsu = CalcIsocurve(UVAxisV, v);
	CCagdPoint ptu;
	ptu = bsu.CalculateAtPoint(u);
	return ptu;
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

	if(U::NearlyEq(length(m_tangentU), 0) || U::NearlyEq(length(m_tangentV), 0))
	{
		return;
	}
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

	// normal deriv, trying using theorem from class instead of using d2Sdudv
	CCagdPoint normalDeriv = CalcNumNormalDeriv(u, v);


	// II
	double l11 = dot(d2Sdu2, m_normal);
	double l22 = dot(d2Sdv2, m_normal);
	//double l12 = dot(d2Sdudv, m_normal);
	double l12 = - dot ( m_tangentU, normalDeriv );

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

	CCagdPoint direction1 = GetPrincipalDir(E, F, L, M, k1);
	bool dir1Valid = !U::NearlyEq(length(direction1), 0);
	CCagdPoint direction2 = GetPrincipalDir(E, F, L, M, k2);
	bool dir2Valid = !U::NearlyEq(length(direction2), 0);

	if(dir1Valid && dir2Valid)
	{
		if (! U::NearlyEq( dot( direction1, direction2 ), 0 ) )
		{
			assert(false);
		}
	}

	if(dir1Valid)
	{
		// k1 curvature
		CCagdPoint vec[2];
		vec[0] = m_point;
		vec[1] = 0.5*normalize(direction1) + m_point;
		double k1Rad = 1.0 / k1;
		CCagdPoint k1Ctr = m_point + k1Rad*normalize(m_normal);
		
		cagdAddPoint(&k1Ctr);
		m_idDir1 = DrawCircle(k1Ctr, normalize(m_normal), cross(m_normal, normalize(direction1)), k1Rad);
		cagdSetSegmentColor(m_idDir1, 1,1,1);
	}


	if(dir2Valid)
	{
		// k2 curvature
		CCagdPoint vec[2];
		vec[0] = m_point;
		vec[1] = 0.5*normalize(direction2) + m_point;
		double k2Rad = 1.0 / k2;
		CCagdPoint k2Ctr = m_point + k2Rad*normalize(m_normal);
		m_idDir2 = DrawCircle(k2Ctr, normalize(m_normal), cross(m_normal, direction2), k2Rad);
		cagdSetSegmentColor(m_idDir2, 255,50,255);	
	}


}
CCagdPoint BsplineSurface::GetPrincipalDir(double E, double F, double L, double M, double k)
{
	if(U::NearlyEq(k, 0.0))
	{
		return CCagdPoint(0,0,0);
	}

	double kDenom = sqrt( (M-k*F)*(M-k*F) + (L - k*E)*(L - k*E) );

	double uk = 0.0;
	double vk = 0.0;
	if(kDenom != 0)
	{
		uk = (M - k*F) / kDenom;
		vk = -(L - k*E) / kDenom;
	}
	else
	{
		//if(U::NearlyEq((M - k*F), 0.0, 0.00000001))
		//{
		//	uk = 0.0;
		//}	
		//else if(U::NearlyEq((M - k*F), kDenom, 0.00000001))
		//{
		//	uk = 1.0;
		//}
		//else
		//{
		//	return CCagdPoint(0,0,0);
		//}

		//if(U::NearlyEq(-(L - k*E), 0.0))
		//{
		//	vk = 0.0;
		//}	
		//else if(U::NearlyEq(-(L - k*E), kDenom, 0.00000001))
		//{
		//	vk = 1.0;
		//}
		//else
		//{
		//	return CCagdPoint(0,0,0);
		//}
	}


	double normVal = sqrt(uk*uk + vk*vk);
	if(U::NearlyEq(normVal, 0))
	{
		return CCagdPoint(0,0,0);
	}

	double ukn = uk / normVal;
	double vkn = vk / normVal;
	CCagdPoint p3 = m_point + (m_tangentU * ukn) + (m_tangentV * vkn);
	CCagdPoint direction = p3 - m_point;
	return direction;

}


CCagdPoint BsplineSurface::CalcNumNormalDeriv(double u, double v)
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

	CCagdPoint dSdUplus = CalcTangentAtPoint(UVAxisU, u, hi);
	CCagdPoint dSdVplus = CalcTangentAtPoint(UVAxisV, u, hi);
	CCagdPoint normalPlus = normalize( cross(dSdUplus, dSdVplus) );

	CCagdPoint dSdUminus = CalcTangentAtPoint(UVAxisU, u, lo);
	CCagdPoint dSdVminus = CalcTangentAtPoint(UVAxisV, u, lo);
	CCagdPoint normalMinus = normalize( cross(dSdUminus, dSdVminus) );

	CCagdPoint der = ( normalPlus - normalMinus ) / sum;
	return der;
}

void BsplineSurface::DrawInvisiblePoints()
{
	if (! m_isValid)
		return;

	m_invisIdToUV.clear();
	for (	double u = m_extentsUV.m_extU.m_min; 
			u <= m_extentsUV.m_extU.m_max; 
			u += (m_extentsUV.m_extU.m_max - m_extentsUV.m_extU.m_min) / m_isoNum.m_u)
	{
		for (double v = m_extentsUV.m_extV.m_min; 
			v <= m_extentsUV.m_extV.m_max; 
			v += (m_extentsUV.m_extV.m_max - m_extentsUV.m_extV.m_min) / m_isoNum.m_v)
		{
			CCagdPoint p = CalculateAtPoint(u, v);
			int id = cagdAddPoint(&p);
			//cagdHideSegment(id);
			m_invisIdToUV.insert( std::pair<int, UVspace>(id, UVspace(u, v)) );
		}
	}
}