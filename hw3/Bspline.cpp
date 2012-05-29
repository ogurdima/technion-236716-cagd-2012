#include "StdAfx.h"
#include "Bspline.h"
#include <vector>
using std::vector;

BSpline::BSpline()
	: m_order(0)
{
}

BSpline::~BSpline()
{
}

bool BSpline::InsertPt(const CCagdPoint& pt, double weight, int ptIdxAt)
{
	if(!Curve::InsertPt(pt, weight, ptIdxAt))
	{ return false; }

	// update the knot vector
	//m_kv.push_back(ptIdxAt);
	return true;
}

void BSpline::SetOrder(unsigned long order)
{
	m_order = order;
}

unsigned long BSpline::GetOrder(unsigned long order) const
{
	return m_order;
}

bool BSpline::SetKnotVector(const vector<double>& kv)
{
	if(2 <= kv.size())
	{
		for(int i=0; i<kv.size()-1; ++i)
		{
			if(kv[i+1] < kv[i])
			{ return false; }
		}
	}

	m_kv.clear();
	m_kv = kv;
	return true;
}

vector<double> BSpline::GetKnotVector()
{
	return m_kv;
}

#include <iosfwd>
#include <fstream>

void BSpline::Calculate()
{
	m_dataPts.clear();

	// calculate domain of the curve: [t_k, t_N_k)
	// k = degree, N = #knots+1
	unsigned long N			= m_kv.size()-1;
	unsigned long k			= m_order-1;
	unsigned long ta_idx	= k;
	unsigned long tb_idx	= N-k;
	if((ta_idx > N) || (tb_idx <= 0))
	{ return; }

	if(m_kv.empty())
	{ return; }

	double ta				= m_kv[ta_idx];
	double tb				= m_kv[tb_idx];
	if(ta >= tb)
	{ return; }

	//if(m_kv.size() == 10)
	//{
	//	TestBasisFunctions();
	//}

	// j is the current knot idx. start at the first allowable knot (ta_idx)
	int curr_j = ta_idx;

	//std::ofstream o0("basis.csv");
	//if(o0)
	//{
	//	o0 << "time,value" << std::endl;
	//	double time = 0.0;
	//	//for(int i=0; i<basis_0.size(); ++i)
	//	//{
	//	//	o0 << time << "," << 
	//	//		basis_0[i] << "," <<
	//	//		basis_1[i] << "," <<
	//	//		basis_2[i] << "," <<
	//	//		basis_3[i] <<
	//	//		std::endl;
	//	//	time += 0.001;
	//	//}
	//	
	//}


	// at each t, store a vector of basis function values
	vector<double> basis_values;
	for(double t=ta; t<=tb; t+=0.1)
	{
		while(t >= m_kv[curr_j+1])
		{
			++curr_j;
		}

		// pre-compute basis function values
		basis_values.clear();
		for(int i=0; i<m_ctrlPts.size(); ++i)
		{
			double basis_val = BSplineBasis(t, i, k);
			basis_values.push_back(basis_val);
		}
		
		// find the normalizing sum (for rational bsplines)
		double norm_sum = 0.0;
		for(int i=0; i<m_ctrlPts.size(); ++i)
		{
			norm_sum += basis_values[i]*m_ctrlPts[i].m_weight;
		}
		//double norm_sum = 1.0;

		// finally, compute the value
		CCagdPoint val_at_t;
		for(int i=0; i<m_ctrlPts.size(); ++i)
		{
			val_at_t += (basis_values[i] * m_ctrlPts[i].m_weight * m_ctrlPts[i].m_pt) / norm_sum;
		}
		//o0 << "Basis: " << basis_values[i] << "
		m_dataPts.push_back(val_at_t);
	}

	//o0.close();

}

// evaluates at time t, ctrl pt index i, degree k
double BSpline::BSplineBasis(double t, int i, int k)
{
	if(t < m_kv[i])
	{
		return 0.0;
	}

	if(k == 0)
	{
		if((t >= m_kv[i]) && (t < m_kv[i+1]))
		{
			return 1.0;
		}
		else
		{
			return 0.0;
		}

	}

	// otherwise, interpolate normally:
	// N_i_k = f_i_n(t) * N_i_n-1		+	g_i_n(t) * N_i+1_n-1
	double f_i_n = 0.0;
	double N_i = 0.0;
	if(m_kv[i+k]-m_kv[i] > 0.0)
	{
		f_i_n			= (t - m_kv[i]) / (m_kv[i+k] - m_kv[i]);
		N_i				= BSplineBasis(t, i, k-1);
	}
	double g_i_plus_1_n = 0.0;
	double N_i_plus_1 = 0.0;
	if(m_kv[i+1+k]-m_kv[i+1] > 0) 
	{
		g_i_plus_1_n	= (m_kv[i+1+k] - t) / (m_kv[i+1+k]-m_kv[i+1]);
		N_i_plus_1		= BSplineBasis(t, i+1, k-1);
	}
	return f_i_n*N_i + g_i_plus_1_n*N_i_plus_1;
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
		for(int i=0; i<basis_0.size(); ++i)
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