/************************************************************************
* Copyright (C) 1996, by Dmitri Bassarab.				*
* Send bugs and/or frames to /dev/null or dima@cs.technion.ac.il	*
*************************************************************************/
#ifndef _CAGD_H_
#define _CAGD_H_

#include <math.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#ifndef PI
#define PI (3.1415926535897932384626433832795)
#endif

class CCagdPoint { /* 3D point */
public:
	CCagdPoint(GLdouble _x=0,GLdouble _y=0,GLdouble _z=0) :
	  x(_x),y(_y),z(_z),id(0),circle(0),father(0){}
	GLdouble x, y, z;
	UINT id;	 // not currently used
	UINT circle; // if the point is a control point, otherwise -1
	UINT father; // if the point belongs to a polyline(for point) or
};

enum { /* supported types of segment */
  CAGD_SEGMENT_UNUSED = 0,
  CAGD_SEGMENT_POINT,
  CAGD_SEGMENT_TEXT,
  CAGD_SEGMENT_POLYLINE,
  CAGD_SEGMENT_WCIRCLE,
  CAGD_SEGMENT_CTLPLYGN
};


typedef struct {
  UINT        type;
  bool        visible;
  UINT        length;
  PSTR        text;
  GLubyte     color[3];
  CCagdPoint *where;
  CCagdPoint center; // for circles
  CCagdPoint radius; // same
  CCagdPoint *ctlPt; // the actual control point
} SEGMENT;
 

void cagdSetHelpText(PCSTR);
bool cagdShowHelp();

/************************************************************************
* Geometric transformations and view functions				*
************************************************************************/
void cagdRotate(GLdouble, GLdouble, GLdouble, GLdouble);
void cagdTranslate(GLdouble, GLdouble, GLdouble);
void cagdScale(GLdouble, GLdouble, GLdouble);
void cagdReset();
bool cagdToObject(int, int, CCagdPoint [2]);
bool cagdToWindow(CCagdPoint *, int *, int *);

/************************************************************************
* General segment functions						*
************************************************************************/
void cagdSetColor(BYTE, BYTE, BYTE);
void cagdGetColor(BYTE *, BYTE *, BYTE *);
bool cagdSetSegmentColor(UINT, BYTE, BYTE, BYTE);
bool cagdGetSegmentColor(UINT, BYTE *, BYTE *, BYTE *);
bool cagdShowSegment(UINT);
bool cagdHideSegment(UINT);
bool cagdIsSegmentVisible(UINT);
bool cagdFreeSegment(UINT);
void cagdFreeAllSegments();
UINT cagdGetSegmentType(UINT);
UINT cagdGetSegmentLength(UINT);
bool cagdGetSegmentLocation(UINT, CCagdPoint *);
void cagdPick(int x, int y);
UINT cagdPickNext();

/************************************************************************
* Point segment functions						*
************************************************************************/
UINT cagdAddPoint(const CCagdPoint *);
bool cagdReusePoint(UINT, const CCagdPoint *);

/************************************************************************
* Text segment functions						*
************************************************************************/
UINT cagdAddText(const CCagdPoint *, PCSTR);
bool cagdReuseText(UINT, const CCagdPoint *, PCSTR);
bool cagdGetText(UINT, PSTR);

/************************************************************************
* Polyline segment functions						*
***********************************************************************/
UINT cagdAddPolyline(const CCagdPoint *, UINT, UINT);
bool cagdReusePolyline(UINT, const CCagdPoint *, UINT);
CCagdPoint *cagdGetVertex(UINT, UINT);
bool cagdSetVertex(UINT, UINT, const CCagdPoint *);
UINT cagdGetNearestVertex(UINT, int, int);

/************************************************************************
* Control Polygon/Circle functions						*
***********************************************************************/
UINT cagdCtrlPlygnAddPoint(UINT id, int index, CCagdPoint location);
CCagdPoint *cagdGetCtrlPt(UINT CtlPlygn, int ptIndex);
UINT addCircle(CCagdPoint center, CCagdPoint radius, CCagdPoint *ctlPt, int segments=300);
UINT cagdChangeCircleCenter(UINT id, CCagdPoint center, int segments = 300);
UINT cagdChangeCircleRadius(UINT id, int oldX, int oldY, int newX, int newY);
CCagdPoint cagdGetRadius(UINT id);
CCagdPoint cagdGetCenter(UINT id);
UINT cagdGetCircleOfCtlPt(UINT polylineId, int pointIndex);

/************************************************************************
* Weight / radius conversion functions						*
***********************************************************************/
double cagdWeightToRadius(double weight);
double cagdRadiusToWeight(double radius);

/************************************************************************
* Math functions											*
***********************************************************************/
// add +(vector, vector)
CCagdPoint operator+(const CCagdPoint& p1, const CCagdPoint& p2);
// sub -(vector, vector)
CCagdPoint operator-(const CCagdPoint& p1, const CCagdPoint& p2);
// mult *(vector, scalar)
CCagdPoint operator*(const CCagdPoint& p1, double scalar);
// mult *(scalar, vector)
CCagdPoint operator*(double scalar, const CCagdPoint& p1);
// div /(vector, scalar)
CCagdPoint operator/(const CCagdPoint& p1, double scalar);
// dot (vector, vector);
double dot(const CCagdPoint& p1, const CCagdPoint& p2);
// cross (vector, vector);
CCagdPoint cross(const CCagdPoint& p1, const CCagdPoint& p2);
// length (vector)
double length(const CCagdPoint& p1);
// norm (vector)
CCagdPoint normalize(const CCagdPoint& p1);
// cmp ==(vector, vector)
bool operator==(const CCagdPoint& p1, const CCagdPoint& p2);


/*********************************************************************
* Custom drawing
*********************************************************************/
UINT DrawVector(const CCagdPoint& pt, const CCagdPoint& dir, double length, BYTE color[3]);
UINT DrawLineSegment(const CCagdPoint& p1, const CCagdPoint& p2, double length, BYTE color[3]);
UINT DrawCircle(const CCagdPoint& center, const CCagdPoint& planar, const CCagdPoint& normal, double radius, int ptCount = 360);
UINT DrawCircleSegment(const CCagdPoint& center, const CCagdPoint& planar, const CCagdPoint& normal, double radius, double startAngle, double stopAngle, int ptCount = 360);
UINT DrawSpiral(const CCagdPoint& center, const CCagdPoint& planar, const CCagdPoint& normal, double startRadius, double stopRadius, double startAngle, double stopAngle, int ptCount = 360);
#endif
