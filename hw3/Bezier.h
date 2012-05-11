
#include "cagd.h"


class Bezier
{
public:
	// constructor/destructor
	Bezier();
	~Bezier();

	// curve manipulation
	bool InsertPt(int ptIdxAfter);
	bool MovePt(int ptIdx);

private:

};