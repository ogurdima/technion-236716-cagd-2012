
#include "cagd.h"
#include <vector>
using std::vector;

struct BezierPt
{
	BezierPt() : m_weight(1.0) {}
	BezierPt(const CCagdPoint& pt, double weight) 
		: m_pt(pt), m_weight(weight)
	{}
	CCagdPoint m_pt;
	double m_weight;
};

class Bezier
{
public:
	// constructor/destructor
	Bezier();
	~Bezier();

	// 
	UINT DrawCtrlPolygon();
	UINT DrawCurve();	
private:
	void Calculate();

public:
	// curve manipulation
	void SetPoly(const vector<CCagdPoint>& pts);
	void SetPoly(const vector<BezierPt>& pts);
	bool SetWeight(int idx, double weight);
	double GetWeight(int idx);
	//bool InsertPt(int ptIdxAfter);
	//bool MovePt(int ptIdx);

private:
	// the control polygon
	vector<BezierPt> m_ctrlPts;
	// curve data
	vector<CCagdPoint> m_dataPts;
};