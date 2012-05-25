#pragma once
#include "Curve.h"

class BSpline : public Curve
{
public:
	BSpline();
	~BSpline();
	
	void SetKnotVector(const vector<int> & kv);
	vector<int> GetNodeVector();
	virtual void Calculate();

protected:
	vector<int> m_kv;
};