#pragma once
#include "cagd.h"
struct e2t_expr_node;


class FrenetFrame
{
public:
	FrenetFrame(void);
	~FrenetFrame(void);
	
	void SetEquations(e2t_expr_node* eqnX, e2t_expr_node* eqnY, e2t_expr_node* eqnZ);
	bool Calculate(double t);

	e2t_expr_node* m_eqnX;
	e2t_expr_node* m_eqnY;
	e2t_expr_node* m_eqnZ;
	
	CCagdPoint m_T;
	CCagdPoint m_N;
	CCagdPoint m_B;
	CCagdPoint m_k;
	CCagdPoint m_torsion;
};
