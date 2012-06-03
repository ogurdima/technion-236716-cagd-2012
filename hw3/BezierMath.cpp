#include "stdafx.h"
#include "BezierMath.h"
#include <math.h>

#pragma warning (disable : 4800)
#pragma warning (disable : 4018)

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

//-----------------------------------------------------------------------------
int U::ptOnLineSegmentAfter(CCagdPoint p, vector<WeightedPt> poly, double epsilon)
{
	bool localEpsilon = false;
	if (poly.size() < 2)
	{
		return -1;
	}
	if (0 > epsilon)
	{
		localEpsilon = true;
	}
	CCagdPoint curr, next;
	double equation = 0;
	for (int i = 0; i < poly.size() - 1; i++)
	{
		curr = poly[i].m_pt;
		next = poly[i+1].m_pt;
		double dist = DistanceFromPointToLine(p, curr, next);
		if (localEpsilon)
			epsilon = length(next - curr)/20;
		CString msg;
		msg.Format("Index is: %d Distance is: %f, epsilon is: %f\n", i, dist, epsilon);
		::OutputDebugString(msg);		

		if (dist <= epsilon && (p.y-curr.y)*(p.y-next.y) <= 0 && (p.x-curr.x)*(p.x-next.x) <= 0)
			return i + 1;
	}
	return -1;
}

//-----------------------------------------------------------------------------
bool U::IsIntegerUnsigned(const std::string& str)
{
	bool isNumber = true;
	for(std::string::const_iterator k = str.begin(); k != str.end(); ++k)
	{
		isNumber &= isdigit(*k);
	}
	return isNumber;
}

//-----------------------------------------------------------------------------
bool U::IsInteger(const std::string& str)
{
	std::string word;
	bool isNumber = true;
	for(std::string::const_iterator k = str.begin(); k != str.end(); ++k)
	{
		isNumber &= (isdigit(*k) || ('-'==*k));
	}
	return isNumber;
}

//-----------------------------------------------------------------------------
bool U::IsFloat(const std::string& str)
{
	std::string::const_iterator k = str.begin();
	if('-' == *k)
	{
		++k;
	}

	bool found_decimal = false;
	while(k != str.end())
	{
		if('.' == *k)
		{
			if(found_decimal)
			{
				return false;
			}
			found_decimal = true;
		}
		else
		{
			if(!isdigit(*k))
			{
				return false;
			}
		}
		++k;
	}
	return true;
}

//-----------------------------------------------------------------------------

double U::DistanceFromPointToLine(CCagdPoint p, CCagdPoint p1, CCagdPoint p2)
{
	CCagdPoint v1 = p - p1;
	CCagdPoint v2 = p - p2;
	CCagdPoint vLine = p2 - p1;
	return length(cross(v1, v2))/length(vLine);
}

WeightedPt U::convexCombination(WeightedPt p1, WeightedPt p2, double t)
{
	return WeightedPt( (p1.m_pt * (1-t) * p1.m_weight) + (t * p2.m_pt * p2.m_weight), (p1.m_weight * (1-t)) + (t * p2.m_weight));
}

CCagdPoint U::convexCombination(CCagdPoint p1, CCagdPoint p2, double t)
{
	return (p1 * (1-t)) + (p2 * t);
}

WeightedPt U::constructiveAlgorithm(vector<WeightedPt> pts, int subIdx, int superIdx, double t)
{
	if (superIdx < 0 || subIdx > pts.size() || subIdx < 0 || subIdx < superIdx)
		throw std::exception();
	vector<CCagdPoint> cpts;
	vector<double> weight;
	for (int i = 0; i < pts.size(); i++)
	{
		weight.push_back(pts[i].m_weight);
		cpts.push_back(pts[i].m_pt);
		cpts[i].z = pts[i].m_weight;
	}
	for (int j = 1; j <= superIdx; j++)
	{
		//for (int i = 0; i <= pts.size() - 1 - j; i++)
		for (int i = cpts.size() - 1; i >= j; i--)
		{
			//double cWij = pts[i-1].m_weight;
			//double cWip1j = pts[i].m_weight;
			//double nWij = (1-t) * pts[i-1].m_weight + t * pts[i].m_weight;
			// 

			//pts[i].m_pt = ((1-t) * cWij * pts[i-1].m_pt) + (t * cWip1j * pts[i].m_pt); 
			////pts[i].m_pt = pts[i].m_pt/nWij;
			//pts[i].m_weight = nWij;

			cpts[i] = convexCombination(cpts[i-1], cpts[i]);
		}
	}
	WeightedPt res = WeightedPt(cpts[subIdx], weight[subIdx]);
	res.m_pt.z = 0;
	return res;
}


