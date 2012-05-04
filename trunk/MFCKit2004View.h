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
#include "FrenetParser.h"
#include "MainFrm.h"
#include <vector>
#include "FrenetFrame.h"
struct e2t_expr_node;

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
	CPoint prevMouseLocation;	// last position of the mouse
	double modelView[16];		// the modelview Matrix
	bool LButtonDown, RButtonDown;
	bool CtrlKeyDown; // not needed?
	double selectedT;
	double step; // stepping delta for the curve parameter
	CString xt,yt,zt; // the expressions for the parametric functions
	int m_animSpeed;
	bool m_animStarted;
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
	FrenetParser m_parser;
	std::vector<std::string> m_curveParamEqn; //x(t),y(t),z(t)
	e2t_expr_node* m_curveNodes[3];
	FrenetFrameMgr m_ffmgr;
	GLUquadric* m_quadSphere;
	double m_paramStartVal;
	double m_paramEndVal;
	double m_paramStepIncr;
	
	double m_lastCurvature;
	double m_lastTorsion;

	bool m_showEvolute;
	bool m_showOffset;
	bool m_showCurvature;
	bool m_showFrenetFrame;
	bool m_showTorsion;
	bool m_showOscSphere;

	//CPoint m_selectedCurvePt;

	/// Added methods
	void DrawAxes();
	void DrawCurve();
	void TestCagdMath();
	void RecalculateCurve();
	void DrawFrenetComponents(int idx);
	void FrenetOnPaintExtend();

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
	afx_msg void OnStuff2();
	afx_msg void OnUpdateStuff2(CCmdUI* pCmdUI);
	afx_msg void OnDemoPolyline();
	afx_msg void OnUpdateDemoPolyline(CCmdUI* pCmdUI);
	afx_msg void OnDemoAnimation();
	afx_msg void OnUpdateDemoAnimation(CCmdUI* pCmdUI);
	afx_msg void OnDemoNone();
	afx_msg void OnUpdateDemoNone(CCmdUI* pCmdUI);
	afx_msg void OnStuff3();
	afx_msg void OnStuff4();
	afx_msg void OnUpdateStuff4(CCmdUI* pCmdUI);
	afx_msg void OnStuff5();
	afx_msg void OnUpdateStuff5(CCmdUI* pCmdUI);
	afx_msg void OnNewsomethingVvv();
	afx_msg void OnUpdateNewsomethingVvv(CCmdUI* pCmdUI);
	afx_msg void OnNewstuffJ();
	afx_msg void OnUpdateNewstuffJ(CCmdUI* pCmdUI);
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
	afx_msg void OnFrenetProperties();
	afx_msg void OnFrenetShowaxes();
	afx_msg void OnUpdateFrenetShowaxes(CCmdUI *pCmdUI);
	afx_msg void OnFrenetShowfrenetframe();
	afx_msg void OnUpdateFrenetShowfrenetframe(CCmdUI *pCmdUI);
	afx_msg void OnFrenetShowcurvature();
	afx_msg void OnUpdateFrenetShowcurvature(CCmdUI *pCmdUI);
	afx_msg void OnFrenetShowtorsion();
	afx_msg void OnUpdateFrenetShowtorsion(CCmdUI *pCmdUI);
	afx_msg void OnFrenetShow();
	afx_msg void OnUpdateFrenetShow(CCmdUI *pCmdUI);
	afx_msg void OnFrenetDrawevolute();
	afx_msg void OnUpdateFrenetDrawevolute(CCmdUI *pCmdUI);
	afx_msg void OnFrenetDrawoffset();
	afx_msg void OnUpdateFrenetDrawoffset(CCmdUI *pCmdUI);
	afx_msg void OnAnimationStart();
	afx_msg void OnAnimationStop();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFrenetShowOscSphere(CCmdUI *pCmdUI);
	afx_msg void OnFrenetShowOscSphere();


	//afx_msg void OnUpdateTorsion(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateCurvature(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in MFCKit2004View.cpp
inline CMFCKit2004Doc* CMFCKit2004View::GetDocument()
   { return (CMFCKit2004Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCKIT2004VIEW_H__293B5162_E060_4B57_A061_73A6A91E528C__INCLUDED_)
