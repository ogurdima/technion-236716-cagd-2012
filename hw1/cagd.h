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
//#include <gl/glaux.h>

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
//CCagdPoint *cagdGetPoint(UINT Point);
bool cagdGetPoint(UINT id, CCagdPoint *where);

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
bool cagdGetVertex(UINT, UINT, CCagdPoint *);
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

// math
CCagdPoint cagdMultiply(const CCagdPoint*, double scalar);
CCagdPoint cagdAdd(const CCagdPoint*, const CCagdPoint*);

#endif
