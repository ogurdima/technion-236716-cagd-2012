#include "stdafx.h"
#include "KVgui.h"
#include <algorithm>


const double KVgui::distDivFactor = 80;

KVgui::KVgui(void) :
m_lineId(0),
m_changed(false),
m_l(0),
m_r(0),
m_anchIdx(-1),
m_minK(0),
m_maxK(0),
m_y(0)
{
}


KVgui::~KVgui(void)
{
}


bool KVgui::setDimensions(int width, int height)
{
	if (width < 0 || height < 0)
		return false;
	double lpx = 5.0;
	double lpy = 5.0;
	double rpx = width - 5;
	double rpy = lpy;

	CCagdPoint tmp[2];
	cagdToObject(lpx, lpy, tmp);
	m_l = tmp[0].x;

	cagdToObject(rpx, rpy, tmp);
	m_r = tmp[0].x;

	m_y = tmp[0].y;
	freeGui();
	return true;
}

void KVgui::setVector(vector<double> kv)
{
	dismiss();
	m_v = kv;
	sort(m_v.begin(), m_v.end());
	m_reported = kv;
	m_minK = (m_v.size() >= 1) ? m_v[0] : 0;
	m_maxK = (m_v.size() >= 1) ? m_v[m_v.size() - 1] : 0;
}

bool KVgui::attemptAnchor(double x, double y)
{
	m_anchIdx = -1;
	if (0 == m_lineId)
		return false;
	int idx = idxUnderCursor(x, y);
	if (idx != -1)
	{
		m_anchIdx = idx;
		return true;
	}
	return false;
}

bool KVgui::isUnderCursor(double x, double y)
{
	if (0 == m_lineId)
		return false;
	int idx = idxUnderCursor(x, y);
	if (idx != -1)
		return true;
	cagdPick(x, y);
	while (UINT someId = cagdPickNext())
	{
		if (m_lineId == someId)
			return true;
	}
	return false;
}

int KVgui::idxUnderCursor(int x, int y)
{
	cagdPick(x, y);
	while (UINT someId = cagdPickNext())
	{
		for (int i = 0; i < m_ids.size(); i++)
		{
			if (someId == m_ids[i])
			{
				return i;
			}
		}
	}
	return -1;
}

void KVgui::dropAnchor()
{
	m_anchIdx = -1;
}

bool KVgui::isAnchored()
{
	return (m_anchIdx != -1);
}

bool KVgui::changedSinceLastGet(double delta)
{
	for (int i = 0; i < m_v.size(); i++)
	{
		if (abs(m_v[i] - m_reported[i]) > delta)
			return true;
	}
	return false;
}

vector<double> KVgui::getVector()
{
	m_reported = m_v;
	return m_reported;
}

void KVgui::dismiss()
{
	freeGui();

	m_v.clear();
	m_reported.clear();
	m_changed = false;
	m_anchIdx = -1;
	m_minK = 0;
	m_maxK = 0;
}

void KVgui::show()
{
	freeGui();

	sort(m_v.begin(), m_v.end());

	CCagdPoint* line = guiLine();
	m_lineId = cagdAddPolyline(line, 2, CAGD_SEGMENT_POLYLINE);
	delete[] line;

	if (0 == m_v.size())
		return;

	double rPart = 0;
	double gPart = 255;
	int inc =  255/(m_v.size());

	for (int i = 0; i < m_v.size(); i++)
	{
		CCagdPoint* currItem = knotToGuiItem(m_v[i]);
		int otherKnots = countePrevKnotsNearby(i);
		translateDown(currItem, otherKnots);
		m_ids.push_back( cagdAddPolyline(currItem, 4, CAGD_SEGMENT_POLYLINE) );
		cagdSetSegmentColor(m_ids[m_ids.size() - 1], rPart, 20, gPart);
		rPart += inc;
		gPart -= inc;
		delete[] currItem;
	}
}

void KVgui::freeGui()
{
	for (int i = 0; i < m_ids.size(); i++)
	{
		cagdFreeSegment(m_ids[i]);
	}
	cagdFreeSegment(m_lineId);
	m_lineId = 0;
	m_ids.clear();
}

double KVgui::knotToGuiX(double k)
{
	double rLeft = m_l + (m_r - m_l)/10;
	double rRight = m_r - (m_r - m_l)/10;
	return rLeft + (k - m_minK) * (rRight - rLeft) / (m_maxK - m_minK);
}

double KVgui::guiXtoknot(double k)
{
	double rLeft = m_l + (m_r - m_l)/10;
	double rRight = m_r - (m_r - m_l)/10;
	return m_minK + (k - rLeft) * (m_maxK - m_minK) / (rRight - rLeft);
}

CCagdPoint* KVgui::knotToGuiItem(double k)
{
	double xCoord = knotToGuiX(k);
	double dist = (m_r - m_l)/distDivFactor;
	CCagdPoint* tri = new CCagdPoint[4];
	tri[0] = CCagdPoint(xCoord, m_y, 0);
	tri[1] = CCagdPoint(xCoord + dist * sin(PI/12), m_y - dist * cos(PI/12), 0);
	tri[2] = CCagdPoint(xCoord - dist * sin(PI/12), m_y - dist * cos(PI/12), 0);
	tri[3] = tri[0];
	return tri;
}

CCagdPoint* KVgui::guiLine()
{
	CCagdPoint* ln = new CCagdPoint[2];
	ln[0] = CCagdPoint(m_l, m_y, 0);
	ln[1] = CCagdPoint(m_r, m_y, 0);
	return ln;
}


void KVgui::updateLastAnchor(int x, int y)
{
	if (m_anchIdx == -1)
		throw std::exception();

	CCagdPoint tmp[2];
	cagdToObject(x, y, tmp);
	double newX = tmp[0].x;
	if (newX <= m_l || newX >= m_r)
		return;
	m_v[m_anchIdx] = guiXtoknot(newX);
	show();
}

int KVgui::countePrevKnotsNearby(int idx)
{
	double dist = 2 * sin(PI/12) * (m_r - m_l)/distDivFactor;
	double currX = m_v[idx];
	int count = 0;
	for (int i = 0; i < idx; i++)
	{
		if ( abs(m_v[i] - currX) < dist)
			count++;
	}
	return count;
}

void KVgui::translateDown(CCagdPoint* tri, int times)
{
	double dist = cos(PI/12) * (m_r - m_l)/100;
	for (int i = 0; i < 4; i++)
	{
		tri[i].y -= dist * times;
	}
}

int KVgui::idxAtPoint(CCagdPoint p)
{
	if (0 == m_lineId)
		return -1;
	int x, y;
	cagdToWindow(&p, &x, &y);
	return idxUnderCursor(x, y);
}


bool KVgui::addKnotAtPoint(CCagdPoint p)
{
	if (0 == m_lineId)
		return false;
	double k = guiXtoknot(p.x);
	int i = 0;
	for (i; i < m_v.size(); i++)
	{
		if (m_v[i] > k)
			break;
	}
	vector<double>::iterator pos = (m_v.begin() + i);
	m_v.insert(pos, k);
	show();
}
