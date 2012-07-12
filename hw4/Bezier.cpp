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
		buf  << (m_ctrlPts[i].m_pt.x) << " " << (m_ctrlPts[i].m_pt.y)  << " " << (m_ctrlPts[i].m_pt.z) << std::endl;
	}
	buf << std::endl; 
	return buf.str();
}

void Bezier::RaiseDegree()
{
	vector<WeightedPt> newCtrl;
	int oldOrder = m_ctrlPts.size();
	for (int i = 0; i < oldOrder + 1; i++)
	{
		CCagdPoint prevP = (i == 0) ? CCagdPoint() : m_ctrlPts[i-1].m_pt;
		CCagdPoint currP = (i == oldOrder) ? CCagdPoint() : m_ctrlPts[i].m_pt;
		double prevW = (i == 0) ? 0 : m_ctrlPts[i-1].m_weight;
		double currW = (i == oldOrder) ? 0 : m_ctrlPts[i].m_weight;

		CCagdPoint newP = (i*prevP + (oldOrder - i) * currP) / (oldOrder);
		double newW = (i * prevW + (oldOrder - i) * currW) / (oldOrder);

		newCtrl.push_back(WeightedPt(newP, newW));
	}
	m_ctrlPts = newCtrl;
}

vector<WeightedPt> Bezier::Subdivide()
{
	vector<WeightedPt> firstCtrl;
	vector<WeightedPt> secondCtrl;
	vector<WeightedPt> secondCtrlReversed;
	for (unsigned int i = 0; i < m_ctrlPts.size(); i++)
	{
		firstCtrl.push_back(U::constructiveAlgorithm(m_ctrlPts, i, i, 0.5));
		secondCtrlReversed.push_back(U::constructiveAlgorithm(m_ctrlPts, m_ctrlPts.size() - 1, i, 0.5));
	}
	for (unsigned int i = 0; i < secondCtrlReversed.size(); i++)
	{
		secondCtrl.push_back(secondCtrlReversed[i]);
	}
	//m_ctrlPts = secondCtrl;
	m_ctrlPts = firstCtrl;
	//return std::pair<vector<WeightedPt>, vector<WeightedPt>>(firstCtrl, firstCtrl);
	return secondCtrl;
}

