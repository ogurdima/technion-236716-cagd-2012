#pragma once

#include "Bspline.h"



class KVgui
{
public:

	KVgui();
	bool setDimensions(int width, int height);
	void setVector(vector<double> kv);
	bool attemptAnchor(double x, double y);
	bool isUnderCursor(double x, double y);
	bool isAnchored();
	void dropAnchor();
	void updateLastAnchor(int x, int y);
	bool changedSinceLastGet(double delta = 0);
	vector<double> getVector();
	void dismiss();
	void show();
	int idxAtPoint(CCagdPoint p);
	bool addKnotAtPoint(CCagdPoint p);
	~KVgui();
	double knotToGuiX(double k);
	double guiXtoknot(double k);

	static const double distDivFactor;

private:

	void freeGui();
	CCagdPoint* knotToGuiItem(double k);
	CCagdPoint* guiLine();
	int countePrevKnotsNearby(int idx);
	void translateDown(CCagdPoint* tri, int times);
	int idxUnderCursor(int x, int y);

	vector<double> m_v;
	vector<double> m_reported;
	vector<UINT> m_ids;
	UINT m_lineId;
	bool m_changed;
	double m_l;
	double m_r;
	double m_minK;
	double m_maxK;
	double m_y;
	int m_anchIdx;
};

