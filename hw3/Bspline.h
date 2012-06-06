#pragma once
#include "Curve.h"

class BSpline : public Curve
{
public:
	BSpline();
	~BSpline();
	
	bool InsertPt(const CCagdPoint& pt, double weight, int ptIdxAt = -1);
 	void SetOrder(unsigned long order);
	unsigned long GetOrder() const;	
  inline unsigned long GetDegree() const;
  bool InsertKnotBoehm(double val);
	bool InsertKnot(int idx);
	bool DeleteKnot(int idx);
	bool SetKnotVector(const vector<double> & kv);
	vector<double> GetKnotVector();
	virtual void Calculate();

	virtual string toIrit(int id);
	virtual string toDat(int id = 0);

private:
	void NormalizeKnotValues();
	// evaluates at time t, ctrl pt index i, degree k
	double BSplineBasis(double t, int i, int k);
	void TestBasisFunctions(int k);
protected:
	vector<double> m_kv;
	unsigned long m_order;
  bool m_openEnd;
};