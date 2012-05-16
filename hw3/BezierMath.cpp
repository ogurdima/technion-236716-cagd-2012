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
bool U::NearlyEq(float a, float b, float epsilon)
{
	return abs(a-b) < epsilon;
}

//-----------------------------------------------------------------------------
bool U::NearlyEq(double a, double b, double epsilon)
{
	return abs(a-b) < epsilon;
}

int U::ptOnLineSegmentAfter(CCagdPoint p, vector<BezierPt> poly, double epsilon)
{
	CCagdPoint curr, next;
	double equation = 0;
	for (int i = 0; i < poly.size() - 1; i++)
	{
		curr = poly[i].m_pt;
		next = poly[i+1].m_pt;
		equation = (p.y - next.y) - ( ((next.y - curr.y)/(next.x - curr.x)) * (p.x - next.x) );
		if (equation < epsilon && (p.y-curr.y)*(p.y-next.y) < 0 && (p.x-curr.x)*(p.x-next.x) < 0)
		{
			return i+1;
		}
	}
	return -1;
}