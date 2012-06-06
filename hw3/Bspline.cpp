#include "StdAfx.h"
#include "Bspline.h"
#include "BezierMath.h"
#include <vector>
using std::vector;

BSpline::BSpline()
	: m_order(1)
  , m_openEnd(false)
{
}

BSpline::~BSpline()
{
}

bool BSpline::InsertPt(const CCagdPoint& pt, double weight, int ptIdxAt)
{
	if(!Curve::InsertPt(pt, weight, ptIdxAt))
	{ return false; }

  // only start adding knots after the number of points = the order
  if(m_ctrlPts.size() < GetOrder())
  { 
    return true; 
  }
  else
  {
    if(m_kv.empty())
    {
      // impose the open end condition
      m_kv.insert(m_kv.begin(), GetOrder(), 0.0);
      m_kv.insert(m_kv.begin()+GetOrder(), GetOrder(), 1.0);
      m_openEnd = true;
    }
    else 
    {
      if(m_openEnd)
      {

        // update the knot vector
          // which point is this?
          size_t knotCount = m_kv.size();
          size_t knotsInside = knotCount - (2*GetDegree());
          size_t newKnotCountInside = knotsInside + 1;
          double knotIncr = 1.0 / double(newKnotCountInside+1);
          // modify points inside
          unsigned int firstNonEdgeKnot = GetDegree();
          unsigned int i = 0;
          for(; i<knotsInside; ++i)
          {
            m_kv[firstNonEdgeKnot+i] = (i+1)*knotIncr;
          }

          vector<double>::iterator newPtIt = (m_kv.begin()+firstNonEdgeKnot+i);
          double val = (i+1)*knotIncr;
          m_kv.insert(newPtIt, val);

	        // arbitrary number of points. use Boehm's knot insertion
          //vector<double>::const_iterator 
      }

    }
  }


  // print them out
  std::stringstream strmKnots;
  strmKnots << "Knots: ";
  for(int i=0; i<m_kv.size(); ++i)
  {
    strmKnots << m_kv[i] << " ";
  }
  strmKnots << std::endl;
  ::OutputDebugString((LPCSTR)strmKnots.str().c_str());

	//m_kv.push_back(ptIdxAt);
	return true;
}

void BSpline::SetOrder(unsigned long order)
{
	m_order = order;
}

unsigned long BSpline::GetOrder() const
{
	return m_order;
}

inline unsigned long BSpline::GetDegree() const
{
  return m_order-1;
}

// In the following two cases,
// it is natural for the program to decide the knots: 
// 1. uniform float knot vector. 
// 2. uniform open end. 
bool BSpline::InsertKnot(int idx)
{
	return false;
}
bool BSpline::DeleteKnot(int idx)
{
	return false;
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
	unsigned long ta_idx	= 0;//k;
	unsigned long tb_idx	= N;//N-k;
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

	//std::ofstream datafile("data.csv");
	//datafile << "time,x,y,z" << std::endl;

	// at each t, store a vector of basis function values
	vector<double> basis_values;
	for(double t=ta; t<=tb; t+=0.01)
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
		//datafile << t << "," << val_at_t.x << "," << val_at_t.y << "," << val_at_t.z << std::endl;
	}
	//datafile.close();
	//o0.close();

}

// everything gets transformed to a value between 0 and 1
void BSpline::NormalizeKnotValues()
{
	if(m_kv.empty())
	{ return; }

	double low_value = m_kv[0];
	for(int i=0; i<m_kv.size(); ++i)
	{
		m_kv[i] -= low_value;
	}
	
	double hi_value = m_kv[m_kv.size()-1];
	
	// if the high value is zero they should all be zero anyway
	if(U::NearlyEq(hi_value, 0.0))
	{ return; }

	for(int i=0; i<m_kv.size(); ++i)
	{
		m_kv[i] /= hi_value;
	}
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

string BSpline::toIrit(int id)
{
	int numOfPoints = m_ctrlPts.size();
	if (0 == numOfPoints)
		return string("");

	std::ostringstream buf = std::ostringstream();
	buf << "[OBJECT BSPLINE" << id << std::endl;
	buf << "\t[CURVE BSPLINE " << numOfPoints << " " << m_order << " P2" << std::endl;
	buf << "\t\t[KV";
	for (int i = 0; i < m_kv.size(); i++)
	{
		buf << " " << m_kv[i];
	}
	buf << "]" << std::endl;
	for (int i = 0; i < numOfPoints; i++)
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
	buf << m_order << std::endl;
	buf << "knots[" << m_kv.size() << "] = " << std::endl << "\t";
	for (int i = 0; i < m_kv.size(); i++)
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
