#include "stdafx.h"
#include "FrenetFrame.h"
#include "expr2tree.h"


FrenetFrame::FrenetFrame(void)
: m_eqnX(NULL)
, m_eqnY(NULL)
, m_eqnZ(NULL)
{
}

FrenetFrame::~FrenetFrame(void)
{
	//e2t_freetree(m_eqnX);
	m_eqnX = NULL;
	//e2t_freetree(m_eqnY);
	m_eqnY = NULL;
	//e2t_freetree(m_eqnZ);
	m_eqnZ = NULL;
}

void FrenetFrame::SetEquations(e2t_expr_node* eqnX, e2t_expr_node* eqnY, e2t_expr_node* eqnZ)
{
	m_eqnX = eqnX;
	m_eqnY = eqnY;
	m_eqnZ = eqnZ;
}
bool FrenetFrame::Calculate(double t)
{
	if((!m_eqnX) || (!m_eqnY) || (!m_eqnZ))
	{
		return false;
	}

	m_lastParamVal = t;

	// calculate 1st, 2nd, 3rd derivates for curve(x,y,z) 
	e2t_expr_node* dxdt1 = e2t_derivtree(m_eqnX, E2T_PARAM_T);
	e2t_expr_node* dydt1 = e2t_derivtree(m_eqnY, E2T_PARAM_T);
	e2t_expr_node* dzdt1 = e2t_derivtree(m_eqnZ, E2T_PARAM_T);
	e2t_expr_node* dxdt2 = e2t_derivtree(dxdt1, E2T_PARAM_T);
	e2t_expr_node* dydt2 = e2t_derivtree(dydt1, E2T_PARAM_T);
	e2t_expr_node* dzdt2 = e2t_derivtree(dzdt1, E2T_PARAM_T);
	e2t_expr_node* dxdt3 = e2t_derivtree(dxdt2, E2T_PARAM_T);
	e2t_expr_node* dydt3 = e2t_derivtree(dydt2, E2T_PARAM_T);
	e2t_expr_node* dzdt3 = e2t_derivtree(dzdt2, E2T_PARAM_T);

	// evaluate 1st, 2nd, 3rd derivates of curve in the given point
	e2t_setparamvalue(t, E2T_PARAM_T);
	CCagdPoint curvePtd1 = CCagdPoint(e2t_evaltree(dxdt1), e2t_evaltree(dydt1), e2t_evaltree(dzdt1));
	CCagdPoint curvePtd2 = CCagdPoint(e2t_evaltree(dxdt2), e2t_evaltree(dydt2), e2t_evaltree(dzdt2));
	CCagdPoint curvePtd3 = CCagdPoint(e2t_evaltree(dxdt3), e2t_evaltree(dydt3), e2t_evaltree(dzdt3));

	// calculate T = B'(t) / |B'(t)|
	m_T = curvePtd1;
	m_T = normalize(m_T);
	
	m_B = cross(curvePtd1, curvePtd2);
	m_B = normalize(m_B);

	m_N = cross(cross(curvePtd1, curvePtd2), curvePtd1);
	m_N = normalize(m_N);

	
	m_k = ( length(cross(curvePtd1, curvePtd2)) ) / ( length(curvePtd1)*length(curvePtd1)*length(curvePtd1) );

	m_torsion = ( length( dot(curvePtd3 , cross(curvePtd1, curvePtd2) ) ) ) / ( length(cross(curvePtd1, curvePtd2))*length(cross(curvePtd1, curvePtd2)) );

	// free everything
	e2t_freetree(dxdt1);
	e2t_freetree(dydt1);
	e2t_freetree(dzdt1);
	e2t_freetree(dxdt2);
	e2t_freetree(dydt2);
	e2t_freetree(dzdt2);
	e2t_freetree(dxdt3);
	e2t_freetree(dydt3);
	e2t_freetree(dzdt3);



}

bool FrenetFrame::DrawAll(double multFactor, double* tptr)
{
	if (!StateIsLegal())
	{
		return false;
	}
	if (tptr != NULL && *tptr == m_lastParamVal)
	{
		Calculate(*tptr);
	}

	return DrawTangent(multFactor, NULL) && DrawNormal(multFactor, NULL) && DrawBinormal(multFactor, NULL);
}
bool FrenetFrame::DrawTangent(double multFactor, double* tptr)
{
	return DrawVector(m_T, multFactor, tptr);
}
bool FrenetFrame::DrawBinormal(double multFactor, double* tptr)
{
	return DrawVector(m_B, multFactor, tptr);
}
bool FrenetFrame::DrawNormal(double multFactor, double* tptr)
{
	return DrawVector(m_N, multFactor, tptr);
}

bool FrenetFrame::DrawVector(CCagdPoint second, double multFactor, double* tptr)
{
	double t;
	if (!StateIsLegal())
	{
		return false;
	}
	if (tptr != NULL && *tptr == m_lastParamVal)
	{
		Calculate(*tptr);
	}
	t = m_lastParamVal;

	e2t_setparamvalue(t, E2T_PARAM_T);
	double xCoord = e2t_evaltree(m_eqnX);
	double yCoord = e2t_evaltree(m_eqnY);
	double zCoord = e2t_evaltree(m_eqnZ);
	CCagdPoint ptOnCurve = CCagdPoint(xCoord, yCoord, zCoord);

	// draw vector 
	CCagdPoint vec[2];
	vec[0] = ptOnCurve;
	vec[1] = ptOnCurve + (second*multFactor);

	//White color by default
	BYTE r = 255;
	BYTE g = 255;
	BYTE b = 255;
	if (second == m_T)
	{
		g = 0;
		b = 0;
	}
	if (second == m_N)
	{
		r = 0;
		b = 0;
	}
	if (second == m_B)
	{
		g = 0;
		r = 0;
	}

	cagdSetSegmentColor(cagdAddPolyline(vec, 2, CAGD_SEGMENT_POLYLINE), r, g, b);
	return true;
}

bool FrenetFrame::StateIsLegal()
{
	if (m_eqnX == NULL || m_eqnY == NULL || m_eqnZ == NULL)
	{
		return false;
	}
	return true;
}

