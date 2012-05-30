#include "StdAfx.h"
#include "Bezier.h"
#include "BezierMath.h"

Bezier::Bezier()
{
}

Bezier::~Bezier()
{
}

void Bezier::Calculate()
{
	m_dataPts.clear();
	int order = m_ctrlPts.size() - 1;
	//Calculating coefficients for
	vector<double> BCoeff;
	for (int i = 0; i <= order; i++)
	{
		BCoeff.push_back(U::choose(order, i));
	}
	for (double t = 0; t <= 1; t += 0.001)
	{
		CCagdPoint curvePt = CCagdPoint(0,0,0);
		double divFactor = 0;
		for (int i = 0; i <= order; i++)
		{
			double theta = (BCoeff[i] * pow((1-t),order-i) * pow(t,i));
			curvePt = curvePt + m_ctrlPts[i].m_pt * m_ctrlPts[i].m_weight * theta;
			divFactor += (m_ctrlPts[i].m_weight * theta);
		}
		m_dataPts.push_back(curvePt/divFactor);
	}
}

string Bezier::toIrit(int id)
{
	int numOfPoints = m_ctrlPts.size();
	if (0 == numOfPoints)
		return string("");

	std::ostringstream buf = std::ostringstream();
	buf << "[OBJECT BEZIER" << id << std::endl;
	buf << "\t[CURVE BEZIER " << numOfPoints << " P2" << std::endl;
	for (int i = 0; i < numOfPoints; i++)
	{
		buf << "\t\t[" << (m_ctrlPts[i].m_weight) << " " << (m_ctrlPts[i].m_pt.x) << 
			" " << (m_ctrlPts[i].m_pt.y) << "]" << std::endl;
	}
	buf << "\t]" << std::endl << "]" << std::endl; 
	return buf.str();
}

string Bezier::toDat(int id)
{
	int numOfPoints = m_ctrlPts.size();
	if (0 == numOfPoints)
		return string("");

	std::ostringstream buf = std::ostringstream();
	buf << numOfPoints << std::endl;
	for (int i = 0; i < numOfPoints; i++)
	{
		buf << (m_ctrlPts[i].m_weight) << " " << (m_ctrlPts[i].m_pt.x) << 
			" " << (m_ctrlPts[i].m_pt.y) << std::endl;
	}
	buf << std::endl; 
	return buf.str();
}

