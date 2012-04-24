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

	bool DrawAll(double multFactor = 1, double* tptr = NULL);
	bool DrawTangent(double multFactor = 1, double* tptr = NULL);
	bool DrawBinormal(double multFactor = 1, double* tptr = NULL);
	bool DrawNormal(double multFactor = 1, double* tptr = NULL);
	bool DrawVector(CCagdPoint second, double multFactor = 1, double* tptr = NULL);

	bool StateIsLegal();

	e2t_expr_node* m_eqnX;
	e2t_expr_node* m_eqnY;
	e2t_expr_node* m_eqnZ;
	
	CCagdPoint m_T;
	CCagdPoint m_N;
	CCagdPoint m_B;
	double m_k;
	double m_torsion;

	double m_lastParamVal;
};
