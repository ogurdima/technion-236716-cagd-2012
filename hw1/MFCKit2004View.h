// MFCKit2004View.h : interface of the CMFCKit2004View class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCKIT2004VIEW_H__293B5162_E060_4B57_A061_73A6A91E528C__INCLUDED_)
#define AFX_MFCKIT2004VIEW_H__293B5162_E060_4B57_A061_73A6A91E528C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cagd.h"
#include "MainFrm.h"


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
	CCagdPoint *findPoint(int x, int y);
	UINT findCircle(int x, int y);

	CCagdPoint gamma(double t, double a, double b, double w1, double w2, double w) const;
	static double lambdaToK(double lambda);
	static double kToLambda(double k);
	static double cToLambda(double c);
	void RenderCurve();
	enum CurveSection
	{
		INSIDE,
		OUTSIDE
	};
	enum ConicSection {
		ConicSectionUnknown,
		ConicSectionEllipse,
		ConicSectionParabola,
		ConicSectionHyperbola
	};
	/// fills in m_curvePoints with the curve itself. 
	/// also fills in 
	void CalculateCurve();
	void DrawCurveByK();
	void DrawCurveBy4thPoint();
	void DrawCurveSection(CurveSection s, BYTE r, BYTE g, BYTE b) const;
	static double CalculateC(const CCagdPoint& P1, const CCagdPoint& T, const CCagdPoint& P2, const CCagdPoint& P5);
	static double CalculateLambda(const CCagdPoint& P1, const CCagdPoint& T, const CCagdPoint& P2, const CCagdPoint& P5);

	static CCagdPoint xyToUV(const CCagdPoint& P1, 
				const CCagdPoint& T, 
				const CCagdPoint& P2, 
				const CCagdPoint& P5);
	static CCagdPoint uvToXY(const CCagdPoint& P1, 
				const CCagdPoint& T, 
				const CCagdPoint& P2, 
				const CCagdPoint& P5);
	static CCagdPoint CMFCKit2004View::FindCrossingPoint(const CCagdPoint& l1p1, 
											const CCagdPoint& l1p2,
											const CCagdPoint& l2p1,
											const CCagdPoint& l2p2);
	static double KfromPoints(const CCagdPoint& T, 
		const CCagdPoint& P1,
		const CCagdPoint& P2,
		const CCagdPoint& A1,
		const CCagdPoint& A2);
	static double colinearVectorRatio(	const CCagdPoint& v1p1,
										const CCagdPoint& v1p2,
										const CCagdPoint& v2p1,
										const CCagdPoint& v2p2);
	static double Length(const CCagdPoint& pt1, const CCagdPoint& p2);
	bool CursorIsOnPoint(const CCagdPoint& cursorpt);
	CCagdPoint* GetPointUnderCursor(const CCagdPoint& cursorpt);
	static ConicSection ConicSectionFromLambda(double lambda);

	double SSense, TSense, RSense;

	HGLRC    m_hRC;			// holds the Rendering Context
	CDC*     m_pDC;			// holds the Device Context
	int m_WindowWidth;			// hold the windows width
	int m_WindowHeight;			// hold the windows height
	double m_AspectRatio;		// hold the fixed Aspect Ration
	CPoint prevMouseLocation;	// last position of the mouse
	double modelView[16];		// the modelview Matrix
	bool LButtonDown, RButtonDown;
	bool CtrlKeyDown;
	int m_Mode;
	int demoMode;				// can be removed
	UINT draggedPolyline;		// can be removed
	int draggedPolyPoint;		// can be removed
	UINT draggedCircle;			// can be removed
	CCagdPoint * draggedCtlPt;	// can be removed
	int m_ConicsSelection;
	int m_DrawMode;
	int m_CurveOrStringArt;
	int draggedPoint;
	bool m_ShowAsymptotes;
	
	/// 
	CCagdPoint m_T;
	CCagdPoint m_P1;
	CCagdPoint m_P2;

	/// for 5-point construction
	CCagdPoint m_P5;

	/// for tangent construction
	CCagdPoint m_tangentP1;
	CCagdPoint m_tangentP2;

	CCagdPoint* m_draggedPt;

	CCagdPoint m_draggedViewPoint;

	double m_K;
	double m_Rho;

	int m_pointCount;


	ConicSection m_conicSection;
	
#define CURVE_MAX_POINT_COUNT 1000
	CCagdPoint m_curvePoints[CURVE_MAX_POINT_COUNT];
	int m_idxParabolaLastInside;
	int m_idxParabolaLastOutside;
	int m_idxParabolaBreak;		// last point *before* break

	int m_idxHyperbolaLastInside;
	int m_idxHyperbolaLastOutside;
	int m_idxHyperbolaBreak1;
	int m_idxHyperbolaBreak2;

	CCagdPoint m_asymptote1[2];
	CCagdPoint m_asymptote2[2];
	CCagdPoint m_asymptoteCenter;

	int m_idxEllipseLastInside;
	int m_idxEllipseLastOutside;
	
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
	afx_msg void OnModeView();
	afx_msg void OnModeControl();
	afx_msg void OnUpdateModeControl(CCmdUI* pCmdUI);
	afx_msg void OnUpdateModeView(CCmdUI* pCmdUI);
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnConicsSet4thpoint();
	afx_msg void OnUpdateConicsSet4thpoint(CCmdUI *pCmdUI);
	afx_msg void OnConicsSetproduct();
	afx_msg void OnUpdateConicsSetproduct(CCmdUI *pCmdUI);
	afx_msg void OnUpdateConicsInternalptp(CCmdUI *pCmdUI);
	afx_msg void OnConicsInternalptp();
	afx_msg void OnConicsEntireconic();
	afx_msg void OnUpdateConicsEntireconic(CCmdUI *pCmdUI);
	afx_msg void OnStuffClearall();
	afx_msg void OnConicsSetrho();
	afx_msg void OnUpdateConicsSetrho(CCmdUI *pCmdUI);
	afx_msg void OnConicsSettangent();
	afx_msg void OnUpdateConicsSettangent(CCmdUI *pCmdUI);
	afx_msg void OnConicsCurve();
	afx_msg void OnUpdateConicsCurve(CCmdUI *pCmdUI);
	afx_msg void OnConicsStringart();
	afx_msg void OnUpdateConicsStringart(CCmdUI *pCmdUI);
	afx_msg void OnConicsBoth();
	afx_msg void OnUpdateConicsBoth(CCmdUI *pCmdUI);
	afx_msg void OnConicsShowasymptotes();
	afx_msg void OnUpdateConicsShowasymptotes(CCmdUI *pCmdUI);
	afx_msg void OnHelpUser();
public:
	afx_msg void OnViewClearview();
public:
	afx_msg void OnViewResetcamera();

	afx_msg void OnUpdateLambda(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in MFCKit2004View.cpp
inline CMFCKit2004Doc* CMFCKit2004View::GetDocument()
   { return (CMFCKit2004Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCKIT2004VIEW_H__293B5162_E060_4B57_A061_73A6A91E528C__INCLUDED_)
