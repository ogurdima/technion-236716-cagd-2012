#include "stdafx.h"
#include "FrenetFrame.h"
#include "expr2tree.h"
#include "assert.h"

//-----------------------------------------------------------------------------
FrenetFrameMgr::FrenetFrameMgr(void)
: m_eqnX(NULL)
, m_eqnY(NULL)
, m_eqnZ(NULL)
, m_paramStart(0)
, m_paramFinish(0)
, m_D(0)
, m_incr(0)
, m_Tid(0)
, m_Nid(0)
, m_Bid(0)
, m_dxdt1(NULL)
, m_dydt1(NULL)
, m_dzdt1(NULL)
, m_dxdt2(NULL)
, m_dydt2(NULL)
, m_dzdt2(NULL)
, m_dxdt3(NULL)
, m_dydt3(NULL)
, m_dzdt3(NULL)
, m_evoluteId(0)
, m_offsetId(0)
, m_torsionId(0)
, m_oscCircleId(0)
, m_oscSphereId(0)
, m_oscCircleVisible(false)
, m_oscSphereVisible(false)
{
	
}

//-----------------------------------------------------------------------------
FrenetFrameMgr::~FrenetFrameMgr(void)
{
	//e2t_freetree(m_eqnX);
	m_eqnX = NULL;
	//e2t_freetree(m_eqnY);
	m_eqnY = NULL;
	//e2t_freetree(m_eqnZ);
	m_eqnZ = NULL;
}

//-----------------------------------------------------------------------------
void FrenetFrameMgr::SetEquations(e2t_expr_node* eqnX, e2t_expr_node* eqnY, e2t_expr_node* eqnZ)
{
	m_eqnX = eqnX;
	m_eqnY = eqnY;
	m_eqnZ = eqnZ;
}

//-----------------------------------------------------------------------------
void FrenetFrameMgr::SetD(double d)
{
	m_D = d;
}

//-----------------------------------------------------------------------------
FrenetFrame FrenetFrameMgr::CalculateAtPoint(double t)
{
	if((!m_eqnX) || (!m_eqnY) || (!m_eqnZ))
	{
		assert(false);
		return FrenetFrame();
	}
	FrenetFrame ff;

	// evaluate 0th, 1st, 2nd, 3rd derivates of curve in the given point
	e2t_setparamvalue(t, E2T_PARAM_T);
	ff.m_origin = CCagdPoint(e2t_evaltree(m_eqnX), e2t_evaltree(m_eqnY), e2t_evaltree(m_eqnZ));
	CCagdPoint curvePtd1 = CCagdPoint(e2t_evaltree(m_dxdt1), e2t_evaltree(m_dydt1), e2t_evaltree(m_dzdt1));
	CCagdPoint curvePtd2 = CCagdPoint(e2t_evaltree(m_dxdt2), e2t_evaltree(m_dydt2), e2t_evaltree(m_dzdt2));
	CCagdPoint curvePtd3 = CCagdPoint(e2t_evaltree(m_dxdt3), e2t_evaltree(m_dydt3), e2t_evaltree(m_dzdt3));

	// calculate T = B'(t) / |B'(t)|
	ff.m_T = curvePtd1;
	ff.m_T = normalize(ff.m_T);
	
	ff.m_B = cross(curvePtd1, curvePtd2);
	ff.m_B = normalize(ff.m_B);

	ff.m_N = cross(cross(curvePtd1, curvePtd2), curvePtd1);
	ff.m_N = normalize(ff.m_N);

	//assert(dot(cross(ff.m_T, ff.m_N),ff.m_B) > 0);

	double d1Length = length(curvePtd1);
	
	ff.m_k = ( length(cross(curvePtd1, curvePtd2)) ) / ( d1Length*d1Length*d1Length );

	double first = (1/length(cross(curvePtd1, ff.m_origin)));
	double second = dot(cross(curvePtd1, ff.m_origin), cross(curvePtd2, curvePtd1));
	double third = d1Length*d1Length*d1Length;
	double fourth = length(cross(curvePtd1, ff.m_origin));
	double fifth = ( 3/(d1Length) ) * dot(curvePtd2, curvePtd1);
	double sixth = third * third;
	ff.m_kPrime = ((first * second * third) - (fourth * fifth)) / sixth;

	ff.m_torsion = ( dot(curvePtd3 , cross(curvePtd1, curvePtd2) ) ) / ( length(cross(curvePtd1, curvePtd2))*length(cross(curvePtd1, curvePtd2)) );


	return ff;
}

//-----------------------------------------------------------------------------
bool FrenetFrameMgr::Calculate(double start, double finish, double stepIncr)
{
	m_paramStart = start;
	m_paramFinish = finish;
	m_incr = stepIncr;
	m_data.clear();
	m_evolute.clear();
	m_offset.clear();

	if((!m_eqnX) || (!m_eqnY) || (!m_eqnZ))
	{
		assert(false);
		return false;
	}

	// calculate 1st, 2nd, 3rd derivates for curve(x,y,z) 
	m_dxdt1 = e2t_derivtree(m_eqnX, E2T_PARAM_T);
	m_dydt1 = e2t_derivtree(m_eqnY, E2T_PARAM_T);
	m_dzdt1 = e2t_derivtree(m_eqnZ, E2T_PARAM_T);
	m_dxdt2 = e2t_derivtree(m_dxdt1, E2T_PARAM_T);
	m_dydt2 = e2t_derivtree(m_dydt1, E2T_PARAM_T);
	m_dzdt2 = e2t_derivtree(m_dzdt1, E2T_PARAM_T);
	m_dxdt3 = e2t_derivtree(m_dxdt2, E2T_PARAM_T);
	m_dydt3 = e2t_derivtree(m_dydt2, E2T_PARAM_T);
	m_dzdt3 = e2t_derivtree(m_dzdt2, E2T_PARAM_T);

	CCagdPoint oscCircleCenter(0,0,0);
	CCagdPoint offsetPt(0,0,0);
	for (double t = m_paramStart; t < m_paramFinish; t += m_incr)
	{
		// calculate Frenet frame
		m_data.push_back(CalculateAtPoint(t));
		// calculate evolute
		const FrenetFrame& lastFrame = m_data[m_data.size()-1];
		double radius = 1 / lastFrame.m_k;
		oscCircleCenter = (lastFrame.m_origin + (lastFrame.m_N*radius));
		m_evolute.push_back(oscCircleCenter);
		// calculate offset
		offsetPt = (lastFrame.m_origin + (lastFrame.m_N*m_D));
		m_offset.push_back(offsetPt);
	}

	// free everything
	e2t_freetree(m_dxdt1);
	e2t_freetree(m_dydt1);
	e2t_freetree(m_dzdt1);
	e2t_freetree(m_dxdt2);
	e2t_freetree(m_dydt2);
	e2t_freetree(m_dzdt2);
	e2t_freetree(m_dxdt3);
	e2t_freetree(m_dydt3);
	e2t_freetree(m_dzdt3);
	m_dxdt1 = NULL;
	m_dydt1 = NULL;
	m_dzdt1 = NULL;
	m_dxdt2 = NULL;
	m_dydt2 = NULL;
	m_dzdt2 = NULL;
	m_dxdt3 = NULL;
	m_dydt3 = NULL;
	m_dzdt3 = NULL;
	return true;
}

//-----------------------------------------------------------------------------
void FrenetFrameMgr::ClearLastFrame()
{
	cagdFreeSegment(m_Tid);
	m_Tid = 0;
	cagdFreeSegment(m_Nid);
	m_Nid = 0;
	cagdFreeSegment(m_Bid);
	m_Bid = 0;
}

//-----------------------------------------------------------------------------
void FrenetFrameMgr::DrawCurve()
{
	std::vector<CCagdPoint> curvePts;

	for (size_t i = 0; i < m_data.size(); i++)
	{
		curvePts.push_back(m_data[i].m_origin);
	}
	cagdAddPolyline(&curvePts[0], curvePts.size(), CAGD_SEGMENT_POLYLINE);

	DrawEvolute();
	DrawOffset();
}

//-----------------------------------------------------------------------------
void FrenetFrameMgr::DrawEvolute()
{
	m_evoluteId = cagdAddPolyline(&m_evolute[0], m_evolute.size(), CAGD_SEGMENT_POLYLINE);
	cagdSetSegmentColor(m_evoluteId, 255, 128, 0);
	cagdHideSegment(m_evoluteId);
}

//-----------------------------------------------------------------------------
void FrenetFrameMgr::DrawOffset()
{
	m_offsetId = cagdAddPolyline(&m_offset[0], m_offset.size(), CAGD_SEGMENT_POLYLINE);
	cagdSetSegmentColor(m_offsetId, 0, 128, 255);
	cagdHideSegment(m_offsetId);
}

//-----------------------------------------------------------------------------
void FrenetFrameMgr::ShowEvolute(bool show)
{
	if(show)
	{
		cagdShowSegment(m_evoluteId);
	}
	else 
	{
		cagdHideSegment(m_evoluteId);
	}
}
//-----------------------------------------------------------------------------
void FrenetFrameMgr::ShowOffset(bool show)
{
	if(show)
	{
		cagdShowSegment(m_offsetId);
	}
	else 
	{
		cagdHideSegment(m_offsetId);
	}
}

//-----------------------------------------------------------------------------
int FrenetFrameMgr::PickFrame(int x, int y, double thresh) const
{
	int ptX = 0;
	int ptY = 0;
	CCagdPoint pickpt(x, y, 0);
	CCagdPoint curvept(0,0,0);
	CCagdPoint curveptPrev(0,0,0);
	CCagdPoint diff(0,0,0);

	for(size_t i=0; i<m_data.size(); ++i)
	{
		cagdToWindow(const_cast<CCagdPoint*>(&m_data[i].m_origin), &ptX, &ptY);
		curvept.x = ptX;
		curvept.y = ptY;
		//CString msg;
		//msg.Format("length: %f\n", length(pickpt-curvept));
		//::OutputDebugString((LPCSTR)msg);
		if(length(pickpt-curvept) < thresh)
		{
			return i;
		}
		
		//if(i > 0) 
		//{
		//	CCagdPoint segVec(ptX-curveptPrev.x, ptY-curveptPrev.y, 0.0);
		//	CCagdPoint segPtToPickPtVec(x-ptX, y-ptY, 0.0);
		//	double sinTheta = length(cross(normalize(segPtToPickPtVec), normalize(segVec)));
		//	double r = length(segPtToPickPtVec);
		//	double dist = r*sinTheta;
		//	if(dist < thresh) 
		//	{
		//		return i;
		//	}
		//}

		// update prev
		//curveptPrev.x = ptX;
		//curveptPrev.y = ptY;
	}
	return -1;
}

//-----------------------------------------------------------------------------
const CCagdPoint* FrenetFrameMgr::GetOffsetAtIndex(int idx) const
{
	if((idx < 0) || (idx >= GetFrameCount()))
	{
		return NULL;
	}

	return &m_offset[idx];

}
//-----------------------------------------------------------------------------
const CCagdPoint* FrenetFrameMgr::GetEvoluteAtIndex(int idx) const
{
	if((idx < 0) || (idx >= GetFrameCount()))
	{
		return NULL;
	}

	return &m_evolute[idx];
}

//-----------------------------------------------------------------------------
size_t FrenetFrameMgr::GetFrameCount() const
{
	return m_data.size();
}

//-----------------------------------------------------------------------------
const FrenetFrame& FrenetFrameMgr::GetFrame(int idx) const
{
	return m_data[idx];
}

//-----------------------------------------------------------------------------
void FrenetFrameMgr::DrawFrenetFrame(int idx)
{
	ClearLastFrame();
	FrenetFrame ff =  GetFrame(idx);
	BYTE R[3] = {255, 0, 0};
	BYTE G[3] = {0, 255, 0};
	BYTE B[3] = {30, 30, 255};
	m_Tid = DrawVector(ff.m_origin, ff.m_T, 1, R);
	m_Nid = DrawVector(ff.m_origin, ff.m_N, 1, G);
	m_Bid = DrawVector(ff.m_origin, ff.m_B, 1, B);	
}

void FrenetFrameMgr::DrawOscCircle(int idx)
{
	ClearLastOscCircle();
	FrenetFrame ff =  GetFrame(idx);
	//glLineWidth(5);
	m_oscCircleId = DrawCircle(m_evolute[idx], ff.m_N, ff.m_B, (1.0/ff.m_k));
	//m_oscCircleId = DrawCircleSegment(m_evolute[idx], ff.m_N, ff.m_B, (1.0/ff.m_k), PI/4, 3*PI/4);
	//m_oscCircleId = DrawSpiral(m_evolute[idx], ff.m_N, ff.m_B, (1.0/ff.m_k), (1.0/ff.m_k)*2.0, 0, 4*PI);
	cagdSetSegmentColor(m_oscCircleId, 255, 0, 255);
}

void FrenetFrameMgr::DrawTorsion(int idx)
{
	ClearLastTorsion();
	FrenetFrame ff =  GetFrame(idx);
	//m_oscCircleId = DrawCircle(m_evolute[idx], ff.m_N, ff.m_B, (1.0/ff.m_k));
	m_torsionId = DrawSpiral(ff.m_origin, ff.m_B, ff.m_T, 1, 1.0 + abs(ff.m_torsion/5.0), 0.0, ff.m_torsion);
	//CString msg;
	//msg.Format("Torsion: %f\n", ff.m_torsion);
	//::OutputDebugString((LPCSTR)msg);
	cagdSetSegmentColor(m_torsionId, 255, 255, 0);
}

void FrenetFrameMgr::ClearLastOscCircle()
{
	cagdFreeSegment(m_oscCircleId);
	m_oscCircleId = 0;
}
void FrenetFrameMgr::ClearLastTorsion()
{
	cagdFreeSegment(m_torsionId);
	m_torsionId = 0;
}
CCagdPoint FrenetFrameMgr::GetOscSphereCenter(int idx)
{
	FrenetFrame ff =  GetFrame(idx);
	return ff.m_origin + (1/ff.m_k)*ff.m_N - (ff.m_kPrime/(ff.m_k*ff.m_k*ff.m_torsion))*ff.m_B;
}
