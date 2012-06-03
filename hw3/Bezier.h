#pragma once
#include "Curve.h"

class Bezier : public Curve
{
public:
	Bezier();
	~Bezier();

	virtual string toIrit(int id);
	virtual string toDat(int id = 0);
	virtual void RaiseDegree();
	virtual std::pair<vector<WeightedPt>, vector<WeightedPt>> Subdivide();
	
protected:
	virtual void Calculate();
};