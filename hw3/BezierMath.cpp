#include "stdafx.h"
#include "BezierMath.h"
#include <math.h>


//-----------------------------------------------------------------------------
unsigned long long U::choose(unsigned long long n, unsigned long long k) 
{
    if (k > n) {
        return 0;
    }
    unsigned long long r = 1;
    for (unsigned long long d = 1; d <= k; ++d) {
        r *= n--;
        r /= d;
    }
    return r;
}

//-----------------------------------------------------------------------------
bool NearlyEq(float a, float b, float epsilon = 0.0001)
{
	return abs(a-b) < epsilon;
}

//-----------------------------------------------------------------------------
bool NearlyEq(double a, double b, double epsilon = 0.00001)
{
	return abs(a-b) < epsilon;
}