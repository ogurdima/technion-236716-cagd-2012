// MFCKit2004View.h : interface of the CMFCKit2004View class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCKIT2004VIEW_H__293B5162_E060_4B57_A061_73A6A91E528C__INCLUDED_)
#define AFX_MFCKIT2004VIEW_H__293B5162_E060_4B57_A061_73A6A91E528C__INCLUDED_

#define CURVE_PTS_COUNT 1000


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cagd.h"
#include <string>
#include "BezierParser.h"
#include "SurfaceFileParser.h"
#include "MainFrm.h"
#include <vector>
#include "Bezier.h"
#include "CurveMgr.h"
#include "NewSurfaceDlg.h"
#include "GlobalsDlg.h"
#include "BsplineSurface.h"

#ifndef PI
#define PI (3.1415926535897932384626433832795) 
#endif

class CMFCKit2004View : public CView
{
protected: // create from serialization only
	CMFCKit2004View();
	DECLARE_DYNCREATE(CMFCKit2004View)

// Attributes
public:
	CMFCKit2004Doc* GetDocument();

// Operations
public:

	bool InitializeOpenGL();
	bool SetupPixelFormat(PIXELFORMATDESCRIPTOR* pPFD = 0); // the '= 0' was added.
	bool SetupViewingFrustum(void);
	bool SetupViewingOrtho(void);
	bool SetupViewingOrthoConstAspect(void);
//	CVector WindowToObject(int x, int y);
//	CPoint ObjectToWindow(CVector& location);
	void RotateXY(double valX, double valY);
	void RotateZ (double val);
	void translateXY(double valX, double valY);
	void Scale (double val);
	CCagdPoint *findCtlPoint(int x, int y);
	UINT findCircle(int x, int y);

	double SSense, TSense, RSense;

	HGLRC    m_hRC;			// holds the Rendering Context
	CDC*     m_pDC;			// holds the Device Context
	int m_WindowWidth;			// hold the windows width
	int m_WindowHeight;			// hold the windows height
	double m_AspectRatio;		// hold the fixed Aspect Ration
	CPoint m_prevMouseLocation;	// last position of the mouse
	CPoint m_mouseDownLocation; // where user clicked
	double modelView[16];		// the modelview Matrix
	bool LButtonDown, RButtonDown;
	bool CtrlKeyDown; // not needed?
	double selectedT;
	CString xt,yt,zt; // the expressions for the parametric functions
	double m_offsetD;
	bool m_showAxes;

	UINT draggedPolyline;		
	int draggedPoint;			
	UINT draggedCircle;			
	CCagdPoint * draggedCtlPt;	

	LARGE_INTEGER liStart;
	LARGE_INTEGER liEnd;
	LARGE_INTEGER liFreq;


	/// Added members
	int m_curveIdx;

	std::string m_filename;	
	//BezierParser m_parser;
	SurfaceFileParser m_parser2;
	double m_lastCurvature;
	double m_lastTorsion;

	/// Added methods
	void DrawAxes();
	void DrawCurve();
	void TestCagdMath();

	/// Added Bezier members
	struct DragInfo
	{
		ControlPointInfo m_pt;
		CCagdPoint m_startPosScreen;
	};

	enum ProgramState
	{
		StateIdle,
		StateAddBezierPts,
		StateAddBSplinePts,
		StateConnectingCurvesG0,
		StateConnectingCurvesG1,
		StateConnectingCurvesC1
	};
	ProgramState m_state;

	int m_currCurveIdx;
	CCagdPoint m_lastRbuttonUp;
	CCagdPoint m_lastLbuttonUp;
	bool m_lastWeightControlStatus;
	DragInfo m_draggedPt;
	ControlPointInfo m_weightCtrlAnchor;
	unsigned int m_bsplineDegree;
	int m_modifiedCurveIdx;



	BsplineSurface m_bs;

	enum Axis
	{
		AxisU,
		AxisV,
		AxisUndef
	};

	Axis m_modifiedAxis;
	bool m_animStarted;
	int m_animSteps;
	int m_animCurrStep;
	int m_animSpeed;
	
	void StartAnimation();
	void StopAnimation();

	// globals dialog
	CGlobalsDlg m_globalsDlg;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCKit2004View)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMFCKit2004View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMFCKit2004View)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFuzzinessMore();
	afx_msg void OnFuzzinessLess();
	afx_msg void OnOptionsReset();
	afx_msg void OnRotationMore();
	afx_msg void OnRotationLess();
	afx_msg void OnTranslationMore();
	afx_msg void OnTranslationLess();
	afx_msg void OnScalingMore();
	afx_msg void OnScalingLess();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();

	afx_msg void OnContextbgNewbeziercurve();
	afx_msg void OnContextbgClearall();
	afx_msg void OnContextpolygonInsertpoint();
	afx_msg void OnContextpolygonAppendpoint();
	afx_msg void OnContextpolygonShowHideControlPolygon();
	afx_msg void OnContextbgNewbsplinecurve();
	afx_msg void OnContextptAdjustweight();
	afx_msg void OnUpdateContextptAdjustweight(CCmdUI *pCmdUI);
	afx_msg void OnContextpolygonRaisedegree();
	afx_msg void OnContextpolygonSubdivide();
	afx_msg void OnContextptRemovepoint();
	afx_msg void OnOptionsShowgrid();
	afx_msg void OnFileSaveGeometry();
	afx_msg void OnContextbsplinepolyModifyknotvector();
	afx_msg void OnConnecttowithcontinuityG0();
	afx_msg void OnConnecttowithcontinuityG1();
	afx_msg void OnConnecttowithcontinuityC1();
	afx_msg void OnKnotguiRemoveknot();
	afx_msg void OnKnotguiInsertknot();
	afx_msg void OnContextbsplinepolyAppendpoint();
	afx_msg void OnKnotguiRemoveknot32819();
	afx_msg void OnKnotguiInsertknotBoehm();
	afx_msg void OnModifyknotvectorU();
	afx_msg void OnModifyknotvectorV();
	afx_msg void OnContextbgNewsurface();
	afx_msg void OnSurfacesGlobals();
	afx_msg void OnSurfacesStartAnimation();
	afx_msg void OnSurfacesStopAnimation();
};

#ifndef _DEBUG  // debug version in MFCKit2004View.cpp
inline CMFCKit2004Doc* CMFCKit2004View::GetDocument()
   { return (CMFCKit2004Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCKIT2004VIEW_H__293B5162_E060_4B57_A061_73A6A91E528C__INCLUDED_)
