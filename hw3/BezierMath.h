#ifndef _BEZIER_MATH_H_
#define _BEZIER_MATH_H_
#pragma once

#include "cagd.h"
#include "Curve.h"
#include <string>

namespace U
{

// n choose k
unsigned long long choose(unsigned long long n, unsigned long long k);

// float comparison
bool NearlyEq(float a, float b, float epsilon = 0.0001);
bool NearlyEq(double a, double b, double epsilon = 0.0001);

int ptOnLineSegmentAfter(CCagdPoint p, vector<WeightedPt> poly, double epsilon = 5.0);

bool IsIntegerUnsigned(const std::string& str);
bool IsInteger(const std::string& str);
bool IsFloat(const std::string& str);
}



#endif