#pragma once
#include "Curve.h"

class Bezier : public Curve
{
public:
	Bezier();
	~Bezier();
	
protected:
	virtual void Calculate();
};