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
	m_eqnX = NULL;
	m_eqnY = NULL;
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

	// evaluate 1st, 2nd, 3rd derivates for x,y,z 
	e2t_expr_node* dxdt1 = e2t_derivtree(m_eqnX, E2T_PARAM_T);
	e2t_expr_node* dydt1 = e2t_derivtree(m_eqnY, E2T_PARAM_T);
	e2t_expr_node* dzdt1 = e2t_derivtree(m_eqnZ, E2T_PARAM_T);
	e2t_expr_node* dxdt2 = e2t_derivtree(dxdt1, E2T_PARAM_T);
	e2t_expr_node* dydt2 = e2t_derivtree(dydt1, E2T_PARAM_T);
	e2t_expr_node* dzdt2 = e2t_derivtree(dzdt1, E2T_PARAM_T);
	e2t_expr_node* dxdt3 = e2t_derivtree(dxdt2, E2T_PARAM_T);
	e2t_expr_node* dydt3 = e2t_derivtree(dydt2, E2T_PARAM_T);
	e2t_expr_node* dzdt3 = e2t_derivtree(dzdt2, E2T_PARAM_T);

	e2t_setparamvalue(t, E2T_PARAM_T);

	// calculate T = B'(t) / |B'(t)|
	m_T = CCagdPoint();
	m_T.x = e2t_evaltree(dxdt1);
	m_T.y = e2t_evaltree(dydt1);
	m_T.z = e2t_evaltree(dzdt1);
	m_T = normalize(m_T);
	




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

