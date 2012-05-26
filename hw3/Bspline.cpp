#include "StdAfx.h"
#include "Bspline.h"


BSpline::BSpline()
{
}

BSpline::~BSpline()
{
}

void BSpline::SetKnotVector(const vector<int>& kv)
{
	m_kv.clear();
	m_kv = kv;
}

vector<int> BSpline::GetNodeVector()
{
	return m_kv;
}

void BSpline::Calculate()
{

}