/************************************************************************
* Copyright (C) 1996, by Dmitri Bassarab.				*
* Send bugs and/or frames to /dev/null or dima@cs.technion.ac.il	*
*************************************************************************/
#include "stdafx.h"
#include "cagd.h"
#include "internal.h"


static GLubyte color[] = { 255, 255, 255 };
UINT nSegments = 0;
SEGMENT *list = NULL;

static bool valid(UINT id)
{
  return (id < nSegments) && (list[id].type != CAGD_SEGMENT_UNUSED);
}

void cagdSetColor(BYTE red, BYTE green, BYTE blue) {
  color[0] = red;
  color[1] = green;
  color[2] = blue;
}

void cagdGetColor(BYTE *red, BYTE *green, BYTE *blue) {
  *red   = color[0];
  *green = color[1];
  *blue  = color[2];
}

bool cagdSetSegmentColor(UINT id, BYTE red, BYTE green, BYTE blue)
{
  SEGMENT *segment;
  if(!valid(id))
    return FALSE;
  segment = &list[id];
  segment->color[0] = red;
  segment->color[1] = green;
  segment->color[2] = blue;
  return TRUE;
}

bool cagdGetSegmentColor(UINT id, BYTE *red, BYTE *green, BYTE *blue)
{
  SEGMENT *segment;
  if(!valid(id))
    return FALSE;
  segment = &list[id];
  *red   = segment->color[0];
  *green = segment->color[1];
  *blue  = segment->color[2];
  return TRUE;
}

static UINT findUnused()
{
  UINT id;
  for(id = 1; id < nSegments; id++)
    if(list[id].type == CAGD_SEGMENT_UNUSED)
      break;
  if(nSegments <= id){
    list = (SEGMENT*)realloc(list, sizeof(SEGMENT) * (nSegments += 20));
    for(id = nSegments - 20; id < nSegments; id++){
      SEGMENT *segment = &list[id];	
      segment->type = CAGD_SEGMENT_UNUSED;
      segment->visible = FALSE;
      segment->length = 0;
      segment->text = NULL;
      segment->where = NULL;
    }
    return findUnused();
  }
  return id;
}

UINT cagdAddPoint(const CCagdPoint *where)
{
  UINT id = findUnused();
  SEGMENT *segment = &list[id];
  segment->type = CAGD_SEGMENT_POINT;
  segment->visible = TRUE;
  memcpy(segment->color, color, sizeof(GLubyte) * 3);
  segment->where = (CCagdPoint *)malloc(sizeof(CCagdPoint));
  *segment->where = *where;
  segment->length = 1;
  return id;
}

bool cagdReusePoint(UINT id, const CCagdPoint *where)
{
  if(!valid(id))
    return FALSE;
  if(list[id].type != CAGD_SEGMENT_POINT)
    return FALSE;
  *list[id].where = *where;
  return TRUE;
}

UINT cagdAddText(const CCagdPoint *where, PCSTR text)
{
  UINT id = findUnused();
  SEGMENT *segment = &list[id];
  if(!text)
    return 0;
  segment->type = CAGD_SEGMENT_TEXT;
  segment->visible = TRUE;
  memcpy(segment->color, color, sizeof(GLubyte) * 3);
  segment->where = (CCagdPoint *)malloc(sizeof(CCagdPoint));
  *segment->where = *where;
  if (!text)
	  text = "";
  segment->text = _strdup(text);
  segment->length = strlen(text);
  return id;
}

bool cagdReuseText(UINT id, const CCagdPoint *where, PCSTR text)
{
  SEGMENT *segment;
  if(!text)
    return FALSE;
  if(!valid(id))
    return FALSE;
  segment = &list[id];
  if(segment->type != CAGD_SEGMENT_TEXT)
    return FALSE;
  *segment->where = *where;
  free(segment->text);
  segment->text = _strdup(text);
  segment->length = strlen(text);
  return TRUE;
}

bool cagdGetText(UINT id, PSTR text)
{
  SEGMENT *segment;
  if(!valid(id))
    return FALSE;
  segment = &list[id];
  if(segment->type != CAGD_SEGMENT_TEXT)
    return FALSE;
  strcpy(text, segment->text);
  return TRUE;
}

UINT cagdAddPolyline(const CCagdPoint *where, UINT length, UINT type)
{
  UINT id = findUnused();
  SEGMENT *segment = &list[id];
  if(length < 2)
    return 0;
  segment->type = type;
  segment->visible = TRUE;
  memcpy(segment->color, color, sizeof(GLubyte) * 3);
  segment->where = (CCagdPoint *)malloc(sizeof(CCagdPoint) * length);
  memcpy(segment->where, where, sizeof(CCagdPoint) * length);
  segment->length = length;
  return id;
}

bool cagdReusePolyline(UINT id, const CCagdPoint *where, UINT length)
{
  SEGMENT *segment;
  if(length < 2)
    return 0;
  if(!valid(id))
    return FALSE;
  segment = &list[id];
  if(segment->type != CAGD_SEGMENT_POLYLINE &&
	 segment->type != CAGD_SEGMENT_WCIRCLE &&
	 segment->type != CAGD_SEGMENT_CTLPLYGN)
    return FALSE;
  free(segment->where);
  segment->where = (CCagdPoint *)malloc(sizeof(CCagdPoint) * length);
  memcpy(segment->where, where, sizeof(CCagdPoint) * length);
  segment->length = length;
  return TRUE;
}


bool cagdGetVertex(UINT id, UINT vertex, CCagdPoint *where)
{
  SEGMENT *segment;
  if(!valid(id))
    return FALSE;
  segment = &list[id];
  if(segment->type != CAGD_SEGMENT_POLYLINE &&
	 segment->type != CAGD_SEGMENT_CTLPLYGN)
    return FALSE;
  if(segment->length <= vertex)
    return FALSE;
  memcpy(where, &segment->where[vertex], sizeof(CCagdPoint));
  return TRUE;
}

bool cagdSetVertex(UINT id, UINT vertex, const CCagdPoint *where)
{
  SEGMENT *segment;
  if(!valid(id))
    return FALSE;
  segment = &list[id];
  if(segment->type != CAGD_SEGMENT_POLYLINE &&
	 segment->type != CAGD_SEGMENT_CTLPLYGN)
    return FALSE;
  if(segment->length <= vertex)
    return FALSE;
//  memcpy(&segment->where[vertex], where, sizeof(CCagdPoint));
    memcpy(&segment->where[vertex], where, 3*sizeof(double));
  return TRUE;
}

bool cagdShowSegment(UINT id)
{
  if(!valid(id))
    return FALSE;
  list[id].visible = TRUE;
  return TRUE;
}

bool cagdHideSegment(UINT id)
{
  if(!valid(id))
    return FALSE;
  list[id].visible = FALSE;
  return TRUE;
}

bool cagdIsSegmentVisible(UINT id)
{
  if(!valid(id))
    return FALSE;
  return list[id].visible;
}

bool cagdFreeSegment(UINT id)
{
  SEGMENT *segment;
  if(!valid(id))
    return FALSE;
  segment = &list[id];
  if(segment->type == CAGD_SEGMENT_TEXT)
    free(segment->text);
  segment->type = CAGD_SEGMENT_UNUSED;
  free(segment->where);
  return TRUE;
}

void cagdFreeAllSegments()
{	
  UINT id;
  for(id = 1; id < nSegments; id++)
    cagdFreeSegment(id);
}

UINT cagdGetSegmentType(UINT id)
{
  if(id < nSegments)
    return list[id].type;
  return CAGD_SEGMENT_UNUSED;
}

UINT cagdGetSegmentLength(UINT id)
{
  if(!valid(id))
    return 0;
  return list[id].length;
}

bool cagdGetSegmentLocation(UINT id, CCagdPoint *where)
{
  UINT length = 1;
  SEGMENT *segment;
  if(!valid(id))
    return 0;
  segment = &list[id];
  if(segment->type == CAGD_SEGMENT_POLYLINE)
    length = segment->length;
  memcpy(where, segment->where, sizeof(CCagdPoint) * length);
  return TRUE;
}

UINT cagdGetNearestVertex(UINT id, int x, int y)
{
  UINT i, minI = 0;
  int X, Y, d, minD;
  SEGMENT *segment;
  if(!valid(id))
    return 0;
  segment = &list[id];
  if(segment->type != CAGD_SEGMENT_POLYLINE &&
	 segment->type != CAGD_SEGMENT_CTLPLYGN)
    return 0;
  for(i = 0; i < segment->length; i++){
    if(!cagdToWindow(&segment->where[i], &X, &Y))
      continue;
    d = (X - x) * (X - x) + (Y - y) * (Y - y);
    if(i == 0)
      minD = d;
    if(d < minD){
      minD = d;
      minI = i;
    }
  }
  return ++minI;
}

void drawSegments(GLenum mode) {
	UINT id, i;
	if(mode == GL_SELECT){
		glInitNames();	
		glPushName(0);
	}
	for(id = 1; id < nSegments; id++){
		SEGMENT *segment = &list[id];
		if(segment->type == CAGD_SEGMENT_UNUSED)
			continue;
		if(!segment->visible)
			continue;
		if(mode == GL_SELECT)
			glLoadName(id);
		glColor3ubv(segment->color);
		switch(segment->type) {
			case CAGD_SEGMENT_POINT:
				glPointSize(3);
				glBegin(GL_POINTS);
				glVertex3dv((GLdouble *)segment->where);
				glEnd();
				break;
			case CAGD_SEGMENT_POLYLINE:
			case CAGD_SEGMENT_WCIRCLE:
				glBegin(GL_LINE_STRIP);
				for(i = 0; i < segment->length; i++)
					glVertex3dv((GLdouble *)&segment->where[i]);
				glEnd();
				break;
			case CAGD_SEGMENT_TEXT:
				if(mode == GL_SELECT)
					break;
				glRasterPos3dv((GLdouble *)segment->where);
//				auxDrawStr(segment->text);
				break;
			case CAGD_SEGMENT_CTLPLYGN:
				glBegin(GL_LINE_STRIP);
				for(i = 0; i < segment->length; i++)
					glVertex3dv((GLdouble *)&segment->where[i]);
				glEnd();
				glPointSize(3);
				glBegin(GL_POINTS);
				for(i = 0; i < segment->length; i++)
					glVertex3dv((GLdouble *)&segment->where[i]);
				glEnd();
		}
	}
}

bool cagdGetPoint(UINT id, CCagdPoint *where)
{
  SEGMENT *segment;
  if(!valid(id))
    return FALSE;
  segment = &list[id];
  if(segment->type != CAGD_SEGMENT_POINT)
    return FALSE;
  memcpy(where, &segment->where[0], sizeof(CCagdPoint));
  return TRUE;
}
