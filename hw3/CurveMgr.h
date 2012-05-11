
#include "cagd.h"
#include "Bezier.h"
#include "Bspline.h"
#include <vector>

struct BezierWrp
{
	Bezier m_curve;
	UINT m_curveId;
	UINT m_ctrPolyId;
	bool m_showCurve;
	bool m_ShowCtrPoly;
};


class CurveMgr
{
public:
	CurveMgr();
	~CurveMgr();

private:
	std::vector<BezierWrp> m_beziers;
};