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
	double newW = (p1.m_weight * (1-t)) + (t * p2.m_weight);
	return WeightedPt( ((p1.m_pt * (1-t) * p1.m_weight) + (t * p2.m_pt * p2.m_weight))/newW, //point (x,y,z)
		newW); //weight w
}

CCagdPoint U::convexCombination(CCagdPoint p1, CCagdPoint p2, double t)
{
	return (p1 * (1-t)) + (p2 * t);
}

WeightedPt U::constructiveAlgorithm(vector<WeightedPt> pts, int subIdx, int superIdx, double t)
{
	if (superIdx < 0 || superIdx >= pts.size() || subIdx >= pts.size() || subIdx < 0)
		throw std::exception();
	for (int j = 1; j <= superIdx; j++)
	{
		for (int i = pts.size() - 1; i >= j; i--)
		{
			pts[i] = convexCombination(pts[i-1], pts[i]);
		}
	}
	return pts[subIdx];
}


vector<WeightedPt> U::rotatePolyRoundFirstPt(vector<WeightedPt> orig, CCagdPoint direction)
{
	vector<CCagdPoint> pts;
	for (int i = 0; i < orig.size(); i++)
	{
		pts.push_back(orig[i].m_pt);
	}
	pts = rotatePolyRoundFirstPt(pts, direction);
	for (int i = 0; i < pts.size(); i++)
	{
		orig[i].m_pt = pts[i];
	}
	return orig;
}

vector<CCagdPoint> U::rotatePolyRoundFirstPt(vector<CCagdPoint> orig, CCagdPoint direction)
{
	if (2 > orig.size() || direction == CCagdPoint(0,0,0))
		return orig;
	CCagdPoint offset = orig[0] - CCagdPoint(0,0,0);

	//=============================================================================
	// Calculating rotation angle
	//=============================================================================
	double sAng = length( cross( direction, (orig[1] - orig[0]) ) );
	double ang = asin(sAng);
	double dotProd = dot(direction, (orig[1] - orig[0]));
	if(dotProd < 0) {
		ang = PI - ang;
	}

	//=============================================================================
	// Moving the polygon to the origin
	//=============================================================================
	for (int i = 0; i < orig.size(); i++)
	{
		orig[i] = orig[i] - offset;
	}

	//=============================================================================
	// Rotating and translating back
	//=============================================================================
	CCagdPoint rotated(0,0,0);
	for (int i = 0; i < orig.size(); i++)
	{
		rotated.x = orig[i].x * cos(ang) - sin(ang) * orig[i].y;
		rotated.y = orig[i].x * sin(ang) + cos(ang) * orig[i].y;
		orig[i] = rotated;
		orig[i] = orig[i] + offset;
	}
	return orig;
}

vector<WeightedPt> U::rotatePolyRoundLastPt(vector<WeightedPt> orig, CCagdPoint direction)
{
	vector<CCagdPoint> pts;
	for (int i = 0; i < orig.size(); i++)
	{
		pts.push_back(orig[i].m_pt);
	}
	pts = rotatePolyRoundLastPt(pts, direction);
	for (int i = 0; i < pts.size(); i++)
	{
		orig[i].m_pt = pts[i];
	}
	return orig;
}

vector<CCagdPoint> U::rotatePolyRoundLastPt(vector<CCagdPoint> orig, CCagdPoint direction)
{
	if (2 > orig.size() || direction == CCagdPoint(0,0,0))
		return orig;
	CCagdPoint offset = orig[orig.size() - 1] - CCagdPoint(0,0,0);

	//=============================================================================
	// Moving the polygon to the origin
	//=============================================================================
	for (int i = 0; i < orig.size(); i++)
	{
		orig[i] = orig[i] - offset;
	}


	//=============================================================================
	// Calculating rotation angle
	//=============================================================================
	CCagdPoint lastLineDir = (orig[orig.size() - 1] - orig[orig.size() - 2]);
	double sAng = length( cross( normalize( lastLineDir ), normalize(direction)) );
	double ang = asin(sAng);
	double dotProd = dot( normalize( lastLineDir ), normalize(direction) );

	
	if(dotProd < 0) {
		ang = PI + ang;
		//ang = -ang;
	}
	if (cross( normalize( lastLineDir ), normalize(direction)).z < 0)
	{
		ang = 2*PI-ang;
	}
	


	//=============================================================================
	// Rotating and translating back
	//=============================================================================
	CCagdPoint rotated(0,0,0);
	for (int i = 0; i < orig.size(); i++)
	{
		rotated.x = orig[i].x * cos(ang) - sin(ang) * orig[i].y;
		rotated.y = orig[i].x * sin(ang) + cos(ang) * orig[i].y;
		orig[i] = rotated;
		orig[i] = orig[i] + offset;
	}
	return orig;
}

vector<CCagdPoint> U::scalePoly(vector<CCagdPoint> orig, double factor)
{
	//=============================================================================
	// Scaling each point will work
	//=============================================================================
	for (int i = 0; i < orig.size(); i++)
	{
		orig[i] = orig[i] * factor;
	}
	return orig;
}

vector<WeightedPt> U::scalePoly(vector<WeightedPt> orig, double factor)
{
	vector<CCagdPoint> pts;
	for (int i = 0; i < orig.size(); i++)
	{
		pts.push_back(orig[i].m_pt);
	}
	pts = scalePoly(pts, factor);
	for (int i = 0; i < pts.size(); i++)
	{
		orig[i].m_pt = pts[i];
		orig[i].m_weight *= factor;
	}
	return orig;
}

vector<CCagdPoint> U::translateFirstPointTo(vector<CCagdPoint> orig, CCagdPoint dest)
{
	if (orig.size() == 0)
		return orig;
	CCagdPoint offset = orig[0] - dest;
	for (int i = 0; i < orig.size(); i++)
	{
		orig[i] = orig[i] - offset;
	}
	return orig;
}


vector<CCagdPoint> U::translateLastPointTo(vector<CCagdPoint> orig, CCagdPoint dest)
{
	if (orig.size() == 0)
		return orig;
	CCagdPoint offset = orig[orig.size() - 1] - dest;
	for (int i = 0; i < orig.size(); i++)
	{
		orig[i] = orig[i] - offset;
	}
	return orig;
}


vector<WeightedPt> U::translateFirstPointTo(vector<WeightedPt> orig, CCagdPoint dest)
{
	if (orig.size() == 0)
		return orig;
	CCagdPoint offset = orig[0].m_pt - dest;
	for (int i = 0; i < orig.size(); i++)
	{
		orig[i].m_pt = orig[i].m_pt - offset;
	}
	return orig;
}

vector<WeightedPt> U::translateLastPointTo(vector<WeightedPt> orig, CCagdPoint dest)
{
	if (orig.size() == 0)
		return orig;
	CCagdPoint offset = orig[orig.size() - 1].m_pt - dest;
	for (int i = 0; i < orig.size(); i++)
	{
		orig[i].m_pt = orig[i].m_pt - offset;
	}
	return orig;
}

