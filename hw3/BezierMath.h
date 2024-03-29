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

int ptOnLineSegmentAfter(CCagdPoint p, vector<WeightedPt> poly, double epsilon = -1);

bool IsIntegerUnsigned(const std::string& str);
bool IsInteger(const std::string& str);
bool IsFloat(const std::string& str);

WeightedPt convexCombination(WeightedPt p1, WeightedPt p2, double t = 0.5);
CCagdPoint convexCombination(CCagdPoint p1, CCagdPoint p2, double t = 0.5);
WeightedPt constructiveAlgorithm(vector<WeightedPt> pts, int subIdx, int superIdx, double t = 0.5);

double DistanceFromPointToLine(CCagdPoint p, CCagdPoint p1, CCagdPoint p2);
vector<CCagdPoint> rotatePolyRoundFirstPt(vector<CCagdPoint> orig, CCagdPoint direction);
vector<WeightedPt> rotatePolyRoundFirstPt(vector<WeightedPt> orig, CCagdPoint direction);
vector<CCagdPoint> rotatePolyRoundLastPt(vector<CCagdPoint> orig, CCagdPoint direction);
vector<WeightedPt> rotatePolyRoundLastPt(vector<WeightedPt> orig, CCagdPoint direction);

vector<CCagdPoint> scalePoly(vector<CCagdPoint> orig, double factor);
vector<WeightedPt> scalePoly(vector<WeightedPt> orig, double factor);

vector<CCagdPoint> translateFirstPointTo(vector<CCagdPoint> orig, CCagdPoint dest);
vector<WeightedPt> translateFirstPointTo(vector<WeightedPt> orig, CCagdPoint dest);

vector<CCagdPoint> translateLastPointTo(vector<CCagdPoint> orig, CCagdPoint dest);
vector<WeightedPt> translateLastPointTo(vector<WeightedPt> orig, CCagdPoint dest);

}



#endif