/************************************************************************
* Copyright (C) 1996, by Dmitri Bassarab.				*
* Send bugs and/or frames to /dev/null or dima@cs.technion.ac.il	*
*************************************************************************/
#include "stdafx.h"
#include "cagd.h"
#include "internal.h"

#define Z_NEAR  (0.001)
#define Z_SHIFT (2)
#define MAX_HITS (100)
#define POINT_SIZE (3)
#define PI (3.1415926535897932384626433832795)

static bool cue = FALSE;
extern GLdouble modelView[16];
static GLdouble projection[16];
extern GLint viewPort[4];
static GLuint hits[MAX_HITS];
static GLint nHits;
GLint fuzziness = 4;
GLdouble sensitive = 1;

extern SEGMENT *list;
extern UINT nSegments;

void cagdPick(int x, int y) {
  glMatrixMode(GL_PROJECTION);
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  glPushMatrix();
  glLoadIdentity();
  gluPickMatrix(x, viewPort[3] - y, fuzziness * 2, fuzziness * 2, viewPort);
  glMultMatrixd(projection);
  glSelectBuffer(MAX_HITS, hits);
  glRenderMode(GL_SELECT);
  drawSegments(GL_SELECT);
  nHits = glRenderMode(GL_RENDER); 
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

UINT cagdPickNext() {
  if(nHits > 0)
    return hits[--nHits * 4 + 3];
  return 0;
}

bool cagdToObject(int x, int y, CCagdPoint where[2])
{
  glGetDoublev(GL_MODELVIEW_MATRIX,modelView);
  GLdouble X = x, Y = viewPort[3] - y;
  return 
    gluUnProject(X, Y, 0.,
		 modelView, projection, viewPort,
		 &where[0].x, &where[0].y, &where[0].z) &&
		   gluUnProject(X, Y, 1.,
				modelView, projection, viewPort,
				&where[1].x, &where[1].y, &where[1].z);
  
} 

bool cagdToWindow(CCagdPoint *where, int *x, int *y)
{
  GLdouble X, Y, z;
  glGetDoublev(GL_MODELVIEW_MATRIX,modelView);
  bool r = bool(gluProject(where->x, where->y, where->z,
		      modelView, projection, viewPort,
		      &X, &Y, &z));
  *x = (int)X;
  *y = viewPort[3] - (int)Y;
  return r;
}

void resize(GLsizei width, GLsizei height) {
  GLdouble s = (GLdouble)width / (height? height: 1);
  glViewport(0, 0, viewPort[2] = width, viewPort[3] = height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if(width > height)
    glOrtho(-1 * s, 1 * s, -1, 1, Z_NEAR, 1/Z_NEAR);
  else
    glOrtho(-1, 1, -1 / s, 1 / s, Z_NEAR, 1/Z_NEAR);
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  glMatrixMode(GL_MODELVIEW);
}	

void saveModelView()
{
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
}

static void shift()
{
  glLoadIdentity();
  glTranslated(0, 0, -Z_SHIFT - Z_NEAR);
}

static void multModelView()
{
  glTranslated(0, 0,  Z_SHIFT + Z_NEAR);
  glMultMatrixd(modelView);
}


void cagdRotate(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
  shift();
  glRotated(angle, x, y, z);
  multModelView();
  saveModelView();
}

void cagdTranslate(GLdouble x, GLdouble y, GLdouble z)
{
  shift();
  multModelView();
  glTranslated(x, y, z);
  saveModelView();
}

void cagdScale(GLdouble x, GLdouble y, GLdouble z)
{
  shift();
  glScaled(x, y, z);
  multModelView();
  saveModelView();
}

void rotateXY(int dX, int dY)
{
  GLdouble x = 1, y = 0, angle, sign = (dY < 0)? -1: 1;
  shift();
  if(dX){
    x = (GLdouble)dY / dX;
    y = 1;
    sign = (dX < 0)? -1: 1;
  }
  angle = sqrt((GLdouble)dX * dX + dY * dY) * 360
    / min(viewPort[2], viewPort[3]);
  glRotated(sensitive * sign * angle, x, y, 0);
  multModelView();
}

void translateXY(int dX, int dY)
{
  CCagdPoint origin, where[2];
  cagdToObject(0, 0, where);
  origin = where[0];
  cagdToObject(dX, dY, where);
  shift();
  multModelView();
  glTranslated(sensitive * (where[0].x - origin.x), 
	       sensitive * (where[0].y - origin.y), 
	       sensitive * (where[0].z - origin.z));
}

void rotateZ(int dX, int dY)
{
  shift();
  glRotated(sensitive * dX * 180 / min(viewPort[2], viewPort[3]), 0, 0, 1);
  multModelView();
}

void translateZ(int dX, int dY)
{
  shift();
  glTranslated(0, 0, sensitive * dX / min(viewPort[2], viewPort[3]));
  multModelView();
}

void scale(GLdouble factor)
{
  cagdScale(factor, factor, factor);
}

void cagdReset()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  shift();
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
}

void cagdBegin(PCSTR title, int width, int height)
{
//  auxReshapeFunc(resize);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_START, 1.f);
  glFogf(GL_FOG_END, 2.f * Z_SHIFT);
  glPointSize((GLfloat)POINT_SIZE); 
  cagdReset();
//  createMenu();
}

UINT addCircle(CCagdPoint center, CCagdPoint radius, CCagdPoint *ctlPt, int segments) {
	double u[3],v[3],w[3],r;
	double t,step=2*PI/segments;
	CCagdPoint *points = new CCagdPoint[segments];
	
	//create base	
	u[0]=radius.x;
	u[1]=radius.y;
	u[2]=radius.z;

	if(radius.x!=0 || radius.y!=0){
		v[0]=radius.y;
		v[1]=-radius.x;
		v[2]=0;
	}
	else{
		v[0]=radius.z;
		v[1]=0;
		v[2]=-radius.x;
	}

	w[0]=u[1]*v[2]-u[2]*v[1];
	w[1]=u[2]*v[0]-u[0]*v[2];
	w[2]=u[0]*v[1]-u[1]*v[0];
	
	//normalize
	r=sqrt(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]);
	if(r==0)//check for valid radius
		return -1;
	u[0]/=r;
	u[1]/=r;
	u[2]/=r;

	r=sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[0]/=r;
	v[1]/=r;
	v[2]/=r;

	r=sqrt(w[0]*w[0]+w[1]*w[1]+w[2]*w[2]);
	w[0]/=r;
	w[1]/=r;
	w[2]/=r;

	r=sqrt(radius.x*radius.x+radius.y*radius.y+radius.z*radius.z);

	points[0].x=center.x+r*v[0];
	points[0].y=center.y+r*v[1];
	points[0].z=center.z+r*v[2];
	int i;
	for(t=step,i=0;t<=2*PI;t+=step,i++){
		points[i].x=center.x+r*cos(t)*v[0]+r*sin(t)*w[0];
		points[i].y=center.y+r*cos(t)*v[1]+r*sin(t)*w[1];
		points[i].z=center.z+r*sin(t)*w[2]+r*cos(t)*v[2];
	}
	//close the circle
	points[segments-1].x=points[0].x;
	points[segments-1].y=points[0].y;
	points[segments-1].z=points[0].z;
	UINT nId = cagdAddPolyline(points,segments, CAGD_SEGMENT_WCIRCLE);
	list[nId].center = center;
	list[nId].radius = radius;
	if (ctlPt != NULL) {
		list[nId].ctlPt = ctlPt; //fill in details
		ctlPt->circle = nId;
	}
	return nId;
}

UINT cagdCtrlPlygnAddPoint(UINT id,int index, CCagdPoint location) {
	if (cagdGetSegmentType(id) != CAGD_SEGMENT_POLYLINE && 
		cagdGetSegmentType(id) != CAGD_SEGMENT_CTLPLYGN)
		return -1;
	int len = list[id].length;
	CCagdPoint *newPolyLine = new CCagdPoint[len+1];
	for (int i=0;i<=index;i++) {
		newPolyLine[i] = list[id].where[i];
	}
	newPolyLine[index+1] = location;
	for (int i=index+2;i<len+1;i++) {
		newPolyLine[i] = list[id].where[i-1];
	}
	GLubyte color[3];
	cagdGetSegmentColor(id,&color[0], &color[1], &color[2]);
	UINT type = cagdGetSegmentType(id);
	cagdFreeSegment(id);
	UINT newID = cagdAddPolyline(newPolyLine, len+1, type);
	cagdSetSegmentColor(newID,color[0], color[1], color[2]);
	return newID;
}

UINT cagdChangeCircleCenter(UINT id, CCagdPoint center, int segments) {
	if (cagdGetSegmentType(id) != CAGD_SEGMENT_WCIRCLE)
		return -1;
	CCagdPoint *ctlPt = list[id].ctlPt;
	CCagdPoint radius = list[id].radius;
	cagdFreeSegment(id);
	UINT nId = addCircle(center,radius,ctlPt,segments);
	ctlPt->circle = nId;
	return nId;
}

UINT cagdChangeCircleRadius(UINT id, int oldX, int oldY, int newX, int newY) {
	CCagdPoint center, oldCircum[2], newCircum[2];
	cagdToObject(oldX, oldY,oldCircum);
//	oldCircum.z = 0;
//	cagdToObject(newX, newY,newCircum);
//	newCircum.z = 0;
	// finish this later....
	return 0;
}

CCagdPoint cagdGetRadius(UINT id) {
	if (list[id].type != CAGD_SEGMENT_WCIRCLE) {
		return CCagdPoint(0,0,0);
	}
	return list[id].radius;
}

UINT cagdGetCircleOfCtlPt(UINT polylineId, int pointIndex) {
	if (polylineId < nSegments && list[polylineId].type == CAGD_SEGMENT_CTLPLYGN) {
		if (int(list[polylineId].length) > pointIndex) {
			return list[polylineId].where[pointIndex].circle;
		}
	}
	return 0;
}

CCagdPoint cagdGetCenter(UINT id) {
	if (list[id].type != CAGD_SEGMENT_WCIRCLE) {
		return CCagdPoint(0,0,0);
	}
	return list[id].center;	
}

// this can be anything you want:
double cagdWeightToRadius(double weight) {
	return weight;
}

// this can be anything you want:
double cagdRadiusToWeight(double radius) {
	return radius;
}

void moreFuzziness()
{
  fuzziness *= 2;
}

void lessFuzziness()
{
  if(fuzziness > 1)
    fuzziness /= 2;
}

void moreSensitive()
{
  sensitive *= 2;
}

void lessSensitive()
{
  sensitive /= 2;
}


//-----------------------------------------------------------------------------
// Math functions
CCagdPoint operator+(const CCagdPoint& p1, const CCagdPoint& p2)
{
	return CCagdPoint(p1.x+p2.x, p1.y+p2.y, p1.z+p2.z);
}
//-----------------------------------------------------------------------------
CCagdPoint operator-(const CCagdPoint& p1, const CCagdPoint& p2)
{
	return CCagdPoint(p1.x-p2.x, p1.y-p2.y, p1.z-p2.z);
}
//-----------------------------------------------------------------------------
CCagdPoint operator*(const CCagdPoint& p1, double scalar)
{
	return CCagdPoint(p1.x*scalar, p1.y*scalar, p1.z*scalar);
}
//-----------------------------------------------------------------------------
CCagdPoint operator*(double scalar, const CCagdPoint& p1)
{
	return (p1*scalar);
}
//-----------------------------------------------------------------------------
CCagdPoint operator/(const CCagdPoint& p1, double scalar)
{
	return (p1*(1.0/scalar));
}
//-----------------------------------------------------------------------------
double dot(const CCagdPoint& p1, const CCagdPoint& p2)
{
	return (p1.x*p2.x + p1.y*p2.y + p1.z*p2.z);
}
//-----------------------------------------------------------------------------
CCagdPoint cross(const CCagdPoint& p1, const CCagdPoint& p2)
{
	CCagdPoint outCross;
    //vector.x =  (Ay*Bz)-(By*Az);
	outCross.x =  (p1.y*p2.z) - (p2.y*p1.z);
    //vector.y = -(Ax*Bz)+(Bx*Az);
	outCross.y = -(p1.x*p2.z) + (p2.x*p1.z);
    //vector.z =  (Ax*By)-(Ay*Bx);
	outCross.z =  (p1.x*p2.y) - (p1.y*p2.x);
	return outCross;
}
//-----------------------------------------------------------------------------
CCagdPoint normalize(const CCagdPoint& p1)
{
	double len = length(p1);
	return (p1 / len);
}
//-----------------------------------------------------------------------------
double length(const CCagdPoint& p1) 
{
	return sqrt(p1.x*p1.x + p1.y*p1.y + p1.z*p1.z);
}

bool operator==(const CCagdPoint& p1, const CCagdPoint& p2)
{
	return (p1.x == p2.x && p1.y == p2.y && p1.z == p2.z);
}


UINT DrawVector(const CCagdPoint& pt, const CCagdPoint& dir, double length, BYTE color[3])
{
	CCagdPoint vec[2];
	vec[0] = pt;
	vec[1] = pt + (normalize(dir)*length);
	BYTE r = color[0];
	BYTE g = color[1];
	BYTE b = color[2];
	UINT res = cagdAddPolyline(vec, 2, CAGD_SEGMENT_POLYLINE);
	cagdSetSegmentColor(res, r, g, b);
	return res;
}

UINT DrawLineSegment(const CCagdPoint& p1, const CCagdPoint& p2, double length, BYTE color[3])
{
	return DrawVector(p1, (p2-p1), length, color);
}

UINT DrawCircle(const CCagdPoint& center, const CCagdPoint& planar, const CCagdPoint& normal, double radius, int ptCount)
{
	CCagdPoint* pts = new CCagdPoint[ptCount + 1];
	double incr = 2*PI / double(ptCount);
	//++ptCount;
	CCagdPoint nPlanar = normalize(planar);
	CCagdPoint lastAxis = normalize(cross(normal, nPlanar));
	CCagdPoint ptOnCircle(0,0,0);

	int i = 0;
	for(double t = 0; t<2*PI; t+=incr, ++i) 
	{
		ptOnCircle = center + (radius*cos(t)*nPlanar) + ((radius * sin(t))*lastAxis);
		pts[i] = ptOnCircle;
	}
	UINT res = cagdAddPolyline(pts, ptCount, CAGD_SEGMENT_POLYLINE);
	delete pts;
	return res;
}