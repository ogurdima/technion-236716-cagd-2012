#pragma once
#include "cagd.h"
#include <vector>
struct e2t_expr_node;

struct FrenetFrame
{
	FrenetFrame(const CCagdPoint& ptOrigin, 
		const CCagdPoint& ptT,
		const CCagdPoint& ptN,
		const CCagdPoint& ptB,
		double k,
		double kPrime,
		double torsion)
	{
		m_origin = ptOrigin;
		m_T = ptT;
		m_B = ptB;
		m_k = k;
		m_kPrime = kPrime;
		m_torsion = torsion;
	}
	FrenetFrame()
	{
		m_origin = m_T = m_B = m_N = CCagdPoint(0,0,0);
		m_k = m_kPrime = m_torsion = 0;
	}
	CCagdPoint m_origin;
	CCagdPoint m_T;
	CCagdPoint m_N;
	CCagdPoint m_B;
	double m_k;
	double m_kPrime;
	double m_torsion;
};

class FrenetFrameMgr
{
public:
	FrenetFrameMgr(void);
	~FrenetFrameMgr(void);
	
	// set up
	void SetEquations(e2t_expr_node* eqnX, e2t_expr_node* eqnY, e2t_expr_node* eqnZ);
	void SetD(double d);
	FrenetFrame CalculateAtPoint(double t);
	bool Calculate(double start, double finish, double stepIncr);
	void ClearLastFrame();
	void ClearLastOscCircle();
	void ClearLastTorsion();
	
	// accessors
	size_t GetFrameCount() const;
	const FrenetFrame& GetFrame(int idx) const;
	void DrawFrenetFrame(int idx);
	void DrawOscCircle(int idx);
	void DrawTorsion(int idx);
	void DrawCurve();
	CCagdPoint GetOscSphereCenter(int idx);

	void ShowEvolute(bool show);
	void ShowOffset(bool show);
	int PickFrame(int x, int y, double thresh = 7.0) const;

	const CCagdPoint* GetOffsetAtIndex(int idx) const;
	const CCagdPoint* GetEvoluteAtIndex(int idx) const;

private:
	void DrawEvolute();
	void DrawOffset();

public:
	e2t_expr_node* m_eqnX;
	e2t_expr_node* m_eqnY;
	e2t_expr_node* m_eqnZ;

	double m_paramStart;
	double m_paramFinish;
	double m_incr;
	double m_D;

	std::vector<FrenetFrame> m_data;
	std::vector<CCagdPoint> m_evolute;
	UINT m_evoluteId;
	std::vector<CCagdPoint> m_offset;
	UINT m_offsetId;
	UINT m_torsionId;

	bool m_oscCircleVisible;
	UINT m_oscCircleId;
	bool m_oscSphereVisible;
	UINT m_oscSphereId;

	UINT m_Tid;
	UINT m_Nid;
	UINT m_Bid;
	

	e2t_expr_node* m_dxdt1;
	e2t_expr_node* m_dydt1;
	e2t_expr_node* m_dzdt1;
	e2t_expr_node* m_dxdt2;
	e2t_expr_node* m_dydt2;
	e2t_expr_node* m_dzdt2;
	e2t_expr_node* m_dxdt3;
	e2t_expr_node* m_dydt3;
	e2t_expr_node* m_dzdt3;
};
