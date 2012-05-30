#pragma once
#include "Curve.h"

class Bezier : public Curve
{
public:
	Bezier();
	~Bezier();

	virtual string toIrit(int id);
	virtual string toDat(int id = 0);
	
protected:
	virtual void Calculate();
};