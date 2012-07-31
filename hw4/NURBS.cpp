#include "stdafx.h"
#include "NURBS.h"
#include "BezierMath.h"

vector<CCagdPoint> NURBS::RatCurveDervs(vector<CCagdPoint> Aders, vector<double> wders, int d)
{
	vector<CCagdPoint> CK; // output vector of derivatives
	for (int k = 0; k <= d; k++)
	{
		CCagdPoint v = Aders[k];
		for (int i = 1; i <= k; i++)
		{
			v -= BinomialCoefficients(k, i) * wders[i] * CK[k - i];
		}
		CK.push_back( v / wders[0] );
	}
	return CK;
}

int NURBS::BinomialCoefficients(int n, int k)
{
	if (n < k || n < 0 || k < 0)
		return 0;
	if (n == k)
		return 1;
	int prod = 1;
	for (int i = 2; i <= k; i++)
	{
		prod *= ( (n - k + i) / i );
	}
	return (n - k + 1) * prod;
}

int NURBS::FindSpan(int n, int p , double u, vector<double> U)
{
	if(U::NearlyEq(u, U[n + 1]))
		return n;
	int low = p;
	int high = n + 1;
	int mid = (low + high) / 2;
	while (u < U[mid] || u >= U[mid + 1])
	{
		if (u < U[mid])
			high = mid;
		else
			low = mid;
		mid = (low + high) / 2;
	}
	return mid;
}

vector<CCagdPoint> NURBS::CurveDerivsAlg1(int n, int p, vector<double> U, vector<CCagdPoint> P, double u, int d)
{
	vector<CCagdPoint> CK;
	CK.resize(d+1);
	int du = (d < p) ? d : p;
	for (int k = p + 1; k <= d; k++)
		CK[k] = 0.0;
	int span = FindSpan(n, p, u, U);
	vector<vector<double>> nders = DersBasisFuns(span, u, p, du, U);
	for (int k = 0; k <= du; k++)
	{
		CK[k] = 0;
		for (int j = 0; j <= p; j++)
			CK[k] += nders[k][j] * P[span - p + j];
	}
	return CK;
}

vector<vector<double>> NURBS::DersBasisFuns(int i, double u, int p, int n, vector<double> U)
{
	vector<vector<double>> ders;
	ders.resize(n + 1);
	for (int ti = 0; ti < ders.size(); ti++)
		ders[ti].resize(p+1);

	vector<vector<double>> a;
	a.resize(2);
	a[0].resize(p+1);
	a[1].resize(p+1);

	vector<vector<double>> ndu;
	ndu.resize(p + 1);
	for (int ti = 0; ti < ndu.size(); ti++)
		ndu[ti].resize(p + 1);
	ndu[0][0] = 1;

	vector<double> left, right;
	left.resize(p+1);
	right.resize(p+1);

	for (int j = 1; j <= p; j++)
	{
		left[j] = u - U[i + 1 - j];
		right[j] = U[i + j] - u;
		double saved = 0.0;
		for (int r = 0; r < j; r++)
		{
			ndu[j][r] = right[r+1] + left[j-r];
			double temp = ndu[r][j-1]/ndu[j][r];

			ndu[r][j] = saved + right[r+1]*temp;
			saved = left[j-r]*temp;
		}
		ndu[j][j] = saved;
	}

	// load basis functions
	for (int j = 0; j <= p; j++)
	{
		ders[0][j] = ndu[j][p];
	}

	for (int r = 0; r <= p; r++)
	{
		int s1 = 0;
		int s2 = 1;
		a[0][0] = 1.0;
		for (int k = 1; k <= n; k++)
		{
			double d = 0.0;
			int rk = r - k;
			int pk = p - k;
			int j1;
			int j2;
			if (r >= k)
			{
				a[s2][0] = a[s1][0]/ndu[pk+1][rk];
				d = a[s2][0]*ndu[rk][pk];
			}
			if (rk >= -1)
				j1 = 1;
			else
				j1 = -rk;
			if (r-1 <= pk)
				j2 = k - 1;
			else
				j2 = p - r;

			for (int j = j1; j <= j2; j++)
			{
				a[s2][j] = (a[s1][j] - a[s1][j-1])/ndu[pk+1][rk+j];
				d += a[s2][j]*ndu[rk+j][pk];

			}
			if (r <= pk)
			{
				a[s2][k] = -a[s1][k-1]/ndu[pk+1][r];
				d += a[s2][k]*ndu[r][pk];
			}
			ders[k][r] = d;

			int tmp = s1;
			s1 = s2;
			s2 = tmp;

		}
	}
	int r = p;
	for (int k = 1; k <= n; k++)
	{
		for (int j = 0; j <= p; j++)
		{
			ders[k][j] *= r;
		}
		r *= (p-k);
	}
	return ders;
}

CCagdPoint NURBS::ActualDerivative(double u, int d, vector<double> kv, vector<WeightedPt> pts, int degree)
{
	vector<CCagdPoint> ptsForAders;
	for (int i = 0; i < pts.size(); i++)
	{
		ptsForAders.push_back(pts[i].m_pt * pts[i].m_weight);
	}
	vector<CCagdPoint> ptsForWders;
	for (int i = 0; i < pts.size(); i++)
	{
		ptsForWders.push_back(CCagdPoint(1,1,1) * pts[i].m_weight);
	}
	vector<CCagdPoint> Aders = CurveDerivsAlg1(ptsForAders.size(), degree, kv, ptsForAders, u, d);
	vector<CCagdPoint> Twders = CurveDerivsAlg1(ptsForWders.size(), degree, kv, ptsForWders, u, d);

	vector<double> wders;
	for (int i = 0; i < Twders.size(); i++)
	{
		wders.push_back(Twders[i].x);
	}
	return RatCurveDervs(Aders, wders, d)[d];
}