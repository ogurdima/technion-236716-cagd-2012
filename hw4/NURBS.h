#pragma once

#include "Curve.h"
#include <vector>

class NURBS
{
public:

	static vector<CCagdPoint> RatCurveDervs(vector<CCagdPoint> Aders, vector<double> wders, int d);
	static int BinomialCoefficients(int n, int k);
	static int FindSpan(int n, int p , double u, vector<double> U);
	static vector<CCagdPoint> CurveDerivsAlg1(int n, int p, vector<double> U, vector<CCagdPoint> P, double u, int d);
	static vector<vector<double>> DersBasisFuns(int i, double u, int p, int n, vector<double> U);
	static CCagdPoint ActualDerivative(double u, int d, vector<double> kv, vector<WeightedPt> pts, int degree);

};

