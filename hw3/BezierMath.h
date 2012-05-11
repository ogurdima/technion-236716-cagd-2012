#ifndef _BEZIER_MATH_H_
#define _BEZIER_MATH_H_
#pragma once

namespace U
{

// n choose k
unsigned long long choose(unsigned long long n, unsigned long long k);

// float comparison
bool NearlyEq(float a, float b, float epsilon = 0.0001);
bool NearlyEq(double a, double b, double epsilon = 0.00001);


}


#endif