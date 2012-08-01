#include "StdAfx.h"
#include "Bspline.h"
#include "BezierMath.h"
#include <vector>
#include <iosfwd>
#include <fstream>
#include <assert.h>

//#define BSPLINE_RATIONAL

using std::vector;

//=============================================================================
// Constructors/Destructors
//=============================================================================

BSpline::BSpline()
	: m_degree(1)
	, m_autoKv(true)
	, m_samplingStep(0.01)
{
}

BSpline::~BSpline()
{
}

//=============================================================================
// Getters
//=============================================================================

unsigned long BSpline::GetOrder() const
{
	return GetDegree() + 1;
}

inline unsigned long BSpline::GetDegree() const
{
	return m_degree;
}

vector<double> BSpline::GetKnotVector() const
{
	return m_kv;
}

//=============================================================================
// Setters
//=============================================================================

void BSpline::SetDegree(unsigned long degree)
{
	m_degree = degree;
}

bool BSpline::SetKnotVector(const vector<double>& kv)
{
	if(2 <= kv.size())
	{
		for(unsigned int i=0; i<kv.size()-1; ++i)
		{
			if(kv[i+1] < kv[i])
			{ return false; }
		}
	}

	m_kv.clear();
	m_kv = kv;
	m_autoKv = false;
	return true;
}

void BSpline::SetSamplingStep(double step) 
{
	m_samplingStep = step; 
}

//=============================================================================
// Insertion
//=============================================================================

bool BSpline::InsertPt(const CCagdPoint& pt, double weight, int ptIdxAt)
{
	if(!Curve::InsertPt(pt, weight, ptIdxAt))
	{ 
		return false; 
	}
	UpdateKnotVector();
	return true;
}

bool BSpline::InsertKnotBoehm(double val)
{
	int insertedIn = -1;

	vector<double> refinedKv;
	for(unsigned int i = 0; i < m_kv.size(); ++i)
	{
		if( m_kv[i] > val && insertedIn == -1)
		{
			insertedIn = i;
			refinedKv.push_back(val);
		}
		refinedKv.push_back(m_kv[i]);
	}
	if (-1 == insertedIn)
	{
		refinedKv.push_back(val);
		insertedIn = refinedKv.size() - 1;
	}

	if (insertedIn <= m_degree)
	{
		return false;
	}
	else if(insertedIn > m_ctrlPts.size())
	{
		return false;
	}

	int initialI = insertedIn - m_degree;
	if (initialI < 0)
		initialI = 0;
	vector<WeightedPt> newCtrl;
	int stopped = -1;
	for (unsigned int i = 0; i <= m_ctrlPts.size(); i++)
	{
		if (i < insertedIn - m_degree)
			newCtrl.push_back(m_ctrlPts[i]);
		else if (i <= insertedIn)
		{
			double coeffA = ( abs(refinedKv[i+m_degree+1] - refinedKv[i]) > 0.0001) ? (val - refinedKv[i]) / ( refinedKv[i+m_degree+1] - refinedKv[i]) : 0;
			double coeffB = ( abs(refinedKv[i+m_degree+1] - refinedKv[i]) > 0.0001) ? (refinedKv[i+m_degree+1] - val) / ( refinedKv[i+m_degree+1] - refinedKv[i]) : 0;
			CCagdPoint np(0,0,0);
			double nw = 0.0;
			if(!U::NearlyEq(coeffA,0.0))
			{
				np += coeffA * m_ctrlPts[i].m_pt;  
				nw += coeffA * m_ctrlPts[i].m_weight;
			}
			else{
				bool ok =true;
			}
			if(!U::NearlyEq(coeffB,0.0))
			{
				np += coeffB * m_ctrlPts[i-1].m_pt;
				nw += coeffB * m_ctrlPts[i-1].m_weight;
			}
			else{
				bool ok =true;
			}

			newCtrl.push_back(WeightedPt(np, nw));
		}
		else
		{
			stopped = i;
			break;
		}
	}
	if (stopped != -1)
	{
		for (int i = stopped - 1; i < m_ctrlPts.size(); i++)
			newCtrl.push_back(m_ctrlPts[i]);
	}
	m_ctrlPts = newCtrl;
	SetKnotVector(refinedKv);
	return true;
}


//=============================================================================
// 
//=============================================================================

void BSpline::Calculate()
{
	m_dataPts.clear();
	m_idxToArg.clear();
	
	if (m_degree > m_kv.size() - 1 - m_degree)
		return;

	int j = 0;
	for (double t = m_kv[m_degree]; t <= m_kv[m_kv.size() - 1 - m_degree]; t += m_samplingStep, j++)
	{
		CCagdPoint cur(0,0,0);
		double w = 0;
		for (int i = 0; i < m_ctrlPts.size(); i++)
		{
			double basis = BSplineBasis(t, i, m_degree); 
#ifdef BSPLINE_RATIONAL
			cur = cur +  m_ctrlPts[i].m_pt * m_ctrlPts[i].m_weight * basis;
			w += m_ctrlPts[i].m_weight * basis;
#else
			cur = cur +  m_ctrlPts[i].m_pt * basis;
#endif
		}
#ifdef BSPLINE_RATIONAL
		m_dataPts.push_back(cur / w);
#else
		m_dataPts.push_back(cur);
#endif
		m_idxToArg.push_back(t);
	}
}

vector<double> BSpline::GetIdxToArg()
{
	return m_idxToArg;
}

CCagdPoint BSpline::CalculateAtPoint(double t)
{
	if (m_degree > m_kv.size() - 1 - m_degree)
		throw std::exception();
	if (t < m_kv[m_degree] || t > m_kv[m_kv.size() - 1 - m_degree])
		throw std::exception();

	CCagdPoint cur(0,0,0);
	double w = 0;
	for (int i = 0; i < m_ctrlPts.size(); i++)
	{
		double basis = BSplineBasis(t, i, m_degree); 
#ifdef BSPLINE_RATIONAL
		cur = cur +  m_ctrlPts[i].m_pt * m_ctrlPts[i].m_weight * basis;
		w += m_ctrlPts[i].m_weight * basis;
#else
		cur = cur +  m_ctrlPts[i].m_pt * basis;
#endif
	}
	
#ifdef BSPLINE_RATIONAL
	if(U::NearlyEq(w, 0.0))
	{
		throw std::exception();
	}
	
	return (cur / w);
#else
	return cur;
#endif
}

CCagdPoint BSpline::DerivativeAtPoint(double t, int j)
{
	CCagdPoint cur(0,0,0);
	double w = 0;
	for (int i = 0; i < m_ctrlPts.size(); i++)
	{
		CCagdPoint Q = QforDeriv(j, i, m_degree);
		double basis = BSplineBasis(t, i, m_degree-j); 
#ifdef BSPLINE_RATIONAL
		cur = cur +  Q * m_ctrlPts[i].m_weight * basis;
		w += m_ctrlPts[i].m_weight * basis;
#else
		cur = cur +  Q * basis;
#endif
	}
#ifdef BSPLINE_RATIONAL
	return (cur / w);
#else
	return cur;
#endif

/*
	CCagdPoint p0, p1;

	double h = 0.1;

	if (0 == j)
	{
		return CalculateAtPoint(t);
	}

	CCagdPoint res;
	if(U::NearlyEq(t, 0.0, 0.001))
	{
		p1 = DerivativeAtPoint(t + h, j - 1);
		p0 = DerivativeAtPoint(t, j - 1);
		res = (p1 - p0) / h;
	}
	else
	{
		p1 = DerivativeAtPoint(t + h, j - 1);
		p0 = DerivativeAtPoint(t - h, j - 1);
		res = (p1 - p0) / 2*h;
	}
	return res;*/
}

CCagdPoint BSpline::RationalDerivativeAtPoint(double t, int j)
{
	return NURBS::ActualDerivative(t, j, m_kv, m_ctrlPts, m_degree);
}

CCagdPoint BSpline::QforDeriv(int j, int ptidx, int k)
{
	if(0 == j)
	{
		return m_ctrlPts[ptidx].m_pt;
	}
	
	CCagdPoint Q = (k-j+1) * QforDeriv(j-1, ptidx, k);
	if(ptidx > 0)
	{
		Q -= (k-j+1)*QforDeriv(j-1, ptidx-1, k);
	}
	Q /= (m_kv[ptidx + k - j + 1] - m_kv[ptidx]);

	return Q;

}

Extents1D BSpline::GetExtents() const
{
	Extents1D ext;
	ext.m_min = m_kv[m_degree];
	ext.m_max = m_kv[m_kv.size() - 1 - m_degree];
	return ext;
}


//=============================================================================
// 
//=============================================================================

void BSpline::UpdateKnotVector()
{

	if(m_autoKv)
	{
		/*m_kv.clear();
		for (int i = 0; i <= m_degree; i++)
		{
			m_kv.push_back(0.0);
		}
		if (m_ctrlPts.size() + m_degree + 1 > (2 * m_degree))
		{
			int innerKnots = m_ctrlPts.size() + m_degree + 1 - (2 * m_degree);
			double inc = 1.0 / (innerKnots + 2);
			for (double i = inc; i < 1.0; i += inc)
			{
				m_kv.push_back(i);
			}
		}
		
		for (int i = 0; i <= m_degree; i++)
		{
			m_kv.push_back(1.0);
		}*/
		m_kv.clear();
		int numOfKnots = m_ctrlPts.size() + m_degree + 1;
		double inc = 1.0 / (numOfKnots);
		for (double i = 0; i <= 1.0; i += inc)
		{
			m_kv.push_back(i);
		}
	}
	else
	{
		bool OK = true;
	}
}

// everything gets transformed to a value between 0 and 1
void BSpline::NormalizeKnotValues()
{
	if(m_kv.empty())
	{ 
		return; 
	}

	double low_value = m_kv[0];
	for(unsigned int i=0; i<m_kv.size(); ++i)
	{
		m_kv[i] -= low_value;
	}

	double hi_value = m_kv[m_kv.size()-1];

	// if the high value is zero they should all be zero anyway
	if(U::NearlyEq(hi_value, 0.0))
	{ 
		return; 
	}
	for(unsigned int i=0; i<m_kv.size(); ++i)
	{
		m_kv[i] /= hi_value;
	}
}

// evaluates at time t, ctrl pt index i, degree k
double BSpline::BSplineBasis(double t, int i, int k)
{
	int m = m_kv.size() - 1;

	if (i < 0 || i > m - k - 1) //Ith Bspline not defined here
	{
		return 0.0;
	}
	
	//=============================================================================
	// Basis of the recursion
	//=============================================================================
	if (k == 0)
	{
		double lowT = m_kv[i];
		double highT = m_kv[i + 1];
		if (t < lowT || t >= highT)
		{
			return 0.0;
		}
		return 1.0;
	}

	//=============================================================================
	// Recursion itself
	//=============================================================================

	if (m_kv[i] >= m_kv[i + 1 + k])
	{
		return 0.0;
	}
	if (t < m_kv[i] || t >= m_kv[i+1+k])
	{
		return 0.0;
	}

	double resA = 0;
	double resB = 0;
	if (abs(m_kv[i+k] - m_kv[i]) > 0.0001) // else 0
	{
		double coefA = (t - m_kv[i]) / (m_kv[i+k] - m_kv[i]);
		double recA = BSplineBasis(t, i, k-1);
		resA = coefA * recA;
	}
	else 
	{
		resA = 0;
	}
	if (abs(m_kv[i+1+k] - m_kv[i+1]) > 0.0001) // else 0
	{
		double coefB = (m_kv[i+1+k] - t) / (m_kv[i+1+k] - m_kv[i+1]);
		double recB = BSplineBasis(t, i+1, k-1);
		resB = coefB * recB;
	}
	else 
	{
		resB = 0;
	}

	return resA + resB;
}

void BSpline::TestBasisFunctions(int k)
{
	vector<double> basis_0;
	vector<double> basis_1;
	vector<double> basis_2;
	vector<double> basis_3;
	for(double t=0.0; t < m_kv[m_kv.size()-1]; t+=0.001)
	{
		basis_0.push_back(BSplineBasis(t, k, 0));
		basis_1.push_back(BSplineBasis(t, k, 1));
		basis_2.push_back(BSplineBasis(t, k, 2));
		basis_3.push_back(BSplineBasis(t, k, 3));
	}

	std::ofstream o0("basis.csv");
	if(o0)
	{
		o0 << "time,value" << std::endl;
		double time = 0.0;
		for(unsigned int i=0; i<basis_0.size(); ++i)
		{
			o0 << time << "," << 
				basis_0[i] << "," <<
				basis_1[i] << "," <<
				basis_2[i] << "," <<
				basis_3[i] <<
				std::endl;
			time += 0.001;
		}
		o0.close();
	}


}

//=============================================================================
// Printing
//=============================================================================

string BSpline::toIrit(int id)
{
	unsigned int numOfPoints = m_ctrlPts.size();
	if (0 == numOfPoints)
		return string("");

	std::ostringstream buf = std::ostringstream();
	buf << "[OBJECT BSPLINE" << id << std::endl;
	buf << "\t[CURVE BSPLINE " << numOfPoints << " " << m_degree + 1 << " P2" << std::endl;
	buf << "\t\t[KV";
	for (unsigned int i = 0; i < m_kv.size(); i++)
	{
		buf << " " << m_kv[i];
	}
	buf << "]" << std::endl;
	for (unsigned int i = 0; i < numOfPoints; i++)
	{
		buf << "\t\t[" << (m_ctrlPts[i].m_weight) << " " << (m_ctrlPts[i].m_pt.x) << 
			" " << (m_ctrlPts[i].m_pt.y) << "]" << std::endl;
	}
	buf << "\t]" << std::endl << "]" << std::endl; 
	return buf.str();
}

string BSpline::toDat(int id)
{
	int numOfPoints = m_ctrlPts.size();
	if (0 == numOfPoints)
		return string("");

	std::ostringstream buf = std::ostringstream();
	buf << m_degree + 1 << std::endl;
	buf << "knots[" << m_kv.size() << "] = " << std::endl << "\t";
	for (unsigned int i = 0; i < m_kv.size(); i++)
	{
		buf <<  m_kv[i] << " ";
		if (i > 0 && i % 5 == 0)
		{
			buf << std::endl << "\t";
		}
	}
	buf << std::endl;
	for (int i = 0; i < numOfPoints; i++)
	{
		buf << (m_ctrlPts[i].m_pt.x) << " " << (m_ctrlPts[i].m_pt.y) << " " << (m_ctrlPts[i].m_pt.z) << std::endl;
	}
	buf << std::endl; 
	return buf.str();
}


//=============================================================================
// Misc
//=============================================================================

bool BSpline::stateIsLegal()
{
	return (m_ctrlPts.size() + m_degree + 1 <= m_kv.size()) && m_kv.size() != 0 && ! (m_degree > m_kv.size() - 1 - m_degree);
}

UINT BSpline::DrawCurve()
{
	if (stateIsLegal())
	{
		return Curve::DrawCurve();
	}
	return 0;
}