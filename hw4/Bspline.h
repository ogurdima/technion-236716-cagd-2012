#pragma once
#include "Curve.h"
#include "NURBS.h"

struct Extents1D
{
	double m_min;
	double m_max;
};

class BSpline : public Curve
{
public:
	BSpline();
	~BSpline();

	unsigned long GetOrder() const;	
	inline unsigned long GetDegree() const;
	vector<double> GetKnotVector() const;

	void SetDegree(unsigned long degree);
	bool SetKnotVector(const vector<double> & kv);
	void SetSamplingStep(double step);

	bool InsertPt(const CCagdPoint& pt, double weight, int ptIdxAt = -1);
	bool InsertKnotBoehm(double val);
	
	virtual void Calculate();

	virtual string toIrit(int id);
	virtual string toDat(int id = 0);
	bool stateIsLegal();

	virtual UINT DrawCurve();

	CCagdPoint CalculateAtPoint(double t);
	CCagdPoint DerivativeAtPoint(double t, int j);
	CCagdPoint QforDeriv(int i, int ptidx, int k);
	CCagdPoint RationalDerivativeAtPoint(double t, int j);

	Extents1D GetExtents() const;

	vector<double> GetIdxToArg();


private:
	void NormalizeKnotValues();
	void UpdateKnotVector();
	// evaluates at time t, ctrl pt index i, degree k
	double BSplineBasis(double t, int i, int k);
	void TestBasisFunctions(int k);
protected:
	vector<double> m_kv;
	unsigned long m_degree;
	bool m_autoKv;
	double m_samplingStep;
	vector<double> m_idxToArg;
};