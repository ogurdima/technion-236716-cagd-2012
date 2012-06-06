// MFCKit2004View.cpp : implementation of the CMFCKit2004View class
//

#include "stdafx.h"
#include "MFCKit2004.h"

#include "MFCKit2004Doc.h"
#include "MFCKit2004View.h"
#include "internal.h"
#include "propdlg.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include "expr2tree.h"
#include "FrenetFrame.h"
#include "KVgui.h"
#include <string>
#include <fstream>

#pragma warning (disable : 4800)
#pragma warning (disable : 4018)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double modelView[16];		// the modelview Matrix
int viewPort[4];			// viewport - actually [width, height, 0, 0]

extern SEGMENT *list;
extern GLint fuzziness;
extern GLdouble sensitive;
/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004View

IMPLEMENT_DYNCREATE(CMFCKit2004View, CView)

BEGIN_MESSAGE_MAP(CMFCKit2004View, CView)
	//{{AFX_MSG_MAP(CMFCKit2004View)
	ON_WM_CHAR()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FUZZINESS_MORE, OnFuzzinessMore)
	ON_COMMAND(ID_FUZZINESS_LESS, OnFuzzinessLess)
	ON_COMMAND(ID_OPTIONS_RESET, OnOptionsReset)
	ON_COMMAND(ID_ROTATION_MORE, OnRotationMore)
	ON_COMMAND(ID_ROTATION_LESS, OnRotationLess)
	ON_COMMAND(ID_TRANSLATION_MORE, OnTranslationMore)
	ON_COMMAND(ID_TRANSLATION_LESS, OnTranslationLess)
	ON_COMMAND(ID_SCALING_MORE, OnScalingMore)
	ON_COMMAND(ID_SCALING_LESS, OnScalingLess)
	ON_COMMAND(ID_FRENET_PROPERTIES, OnFrenetProperties)
	ON_COMMAND(ID_FRENET_SHOWAXES, OnFrenetShowaxes)
	ON_UPDATE_COMMAND_UI(ID_FRENET_SHOWAXES, OnUpdateFrenetShowaxes)
	ON_COMMAND(ID_FRENET_SHOWFRENETFRAME, OnFrenetShowfrenetframe)
	ON_UPDATE_COMMAND_UI(ID_FRENET_SHOWFRENETFRAME, OnUpdateFrenetShowfrenetframe)
	ON_COMMAND(ID_FRENET_SHOWCURVATURE, OnFrenetShowcurvature)
	ON_UPDATE_COMMAND_UI(ID_FRENET_SHOWCURVATURE, OnUpdateFrenetShowcurvature)
	ON_COMMAND(ID_FRENET_SHOWTORSION, OnFrenetShowtorsion)
	ON_UPDATE_COMMAND_UI(ID_FRENET_SHOWTORSION, OnUpdateFrenetShowtorsion)
	ON_COMMAND(ID_FRENET_SHOW, OnFrenetShow)
	ON_UPDATE_COMMAND_UI(ID_FRENET_SHOW, OnUpdateFrenetShow)
	ON_COMMAND(ID_FRENET_SHOW_OSC_SPHERE, OnFrenetShowOscSphere)
	ON_UPDATE_COMMAND_UI(ID_FRENET_SHOW_OSC_SPHERE, OnUpdateFrenetShowOscSphere)
	ON_COMMAND(ID_FRENET_DRAWEVOLUTE, OnFrenetDrawevolute)
	ON_UPDATE_COMMAND_UI(ID_FRENET_DRAWEVOLUTE, OnUpdateFrenetDrawevolute)
	ON_COMMAND(ID_FRENET_DRAWOFFSET, OnFrenetDrawoffset)
	ON_UPDATE_COMMAND_UI(ID_FRENET_DRAWOFFSET, OnUpdateFrenetDrawoffset)
	ON_COMMAND(ID_ANIMATION_START, OnAnimationStart)
	ON_COMMAND(ID_ANIMATION_STOP, OnAnimationStop)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)

	ON_COMMAND(ID_CONTEXTBG_NEWBEZIERCURVE, &CMFCKit2004View::OnContextbgNewbeziercurve)
	ON_COMMAND(ID_CONTEXTBG_CLEARALL, &CMFCKit2004View::OnContextbgClearall)
	ON_COMMAND(ID_CONTEXTPOLYGON_INSERTPOINT, &CMFCKit2004View::OnContextpolygonInsertpoint)
	ON_COMMAND(ID_CONTEXTPOLYGON_APPENDPOINT, &CMFCKit2004View::OnContextpolygonAppendpoint)
	ON_COMMAND(ID_CONTEXTPOLYGON_SHOW, &CMFCKit2004View::OnContextpolygonShowHideControlPolygon)
	ON_COMMAND(ID_CONTEXTBG_NEWBSPLINECURVE, &CMFCKit2004View::OnContextbgNewbsplinecurve)
	ON_COMMAND(ID_CONTEXTPT_ADJUSTWEIGHT, &CMFCKit2004View::OnContextptAdjustweight)
	ON_UPDATE_COMMAND_UI(ID_CONTEXTPT_ADJUSTWEIGHT, &CMFCKit2004View::OnUpdateContextptAdjustweight)
	ON_COMMAND(ID_CONTEXTPOLYGON_RAISEDEGREE, &CMFCKit2004View::OnContextpolygonRaisedegree)
	ON_COMMAND(ID_CONTEXTPOLYGON_SUBDIVIDEATT, &CMFCKit2004View::OnContextpolygonSubdivide)
	ON_COMMAND(ID_CONTEXTPT_REMOVEPOINT, &CMFCKit2004View::OnContextptRemovepoint)
	ON_COMMAND(ID_OPTIONS_SHOWGRID, &CMFCKit2004View::OnOptionsShowgrid)
	ON_COMMAND(ID_FILE_SAVE32808, &CMFCKit2004View::OnFileSaveGeometry)
	ON_COMMAND(ID_FILE_SAVE, &CMFCKit2004View::OnFileSaveGeometry)
	ON_COMMAND(ID_CONTEXTBSPLINEPOLY_MODIFYKNOTVECTOR, &CMFCKit2004View::OnContextbsplinepolyModifyknotvector)
	ON_COMMAND(ID_CONNECTTOWITHCONTINUITY_G0, &CMFCKit2004View::OnConnecttowithcontinuityG0)
	ON_COMMAND(ID_CONNECTTOWITHCONTINUITY_G1, &CMFCKit2004View::OnConnecttowithcontinuityG1)
	ON_COMMAND(ID_CONNECTTOWITHCONTINUITY_C1, &CMFCKit2004View::OnConnecttowithcontinuityC1)
	ON_COMMAND(ID_KNOTGUI_REMOVEKNOT, &CMFCKit2004View::OnKnotguiRemoveknot)
	ON_COMMAND(ID_KNOTGUI_INSERTKNOT, &CMFCKit2004View::OnKnotguiInsertknot)
	ON_COMMAND(ID_CONTEXTBSPLINEPOLY_APPENDPOINT, &CMFCKit2004View::OnContextbsplinepolyAppendpoint)
	ON_COMMAND(ID_KNOTGUI_INSERTKNOT32820, &CMFCKit2004View::OnKnotguiInsertknotBoehm)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004View construction/destruction

CMFCKit2004View::CMFCKit2004View() {
	SSense = RSense = TSense = 0.5;
	RButtonDown = LButtonDown = false;
	CtrlKeyDown = false;
	selectedT = -1; // outside of bounds -> nothing drawn
	draggedPolyline = 0;		
	draggedPoint = -1;			
	draggedCircle = 0;			
	draggedCtlPt = NULL;	
	step = 0.01;





	m_animSpeed = 50;
	m_animStarted = false;
	m_offsetD = 0.5;

	m_curveParamEqn.push_back("0");
	m_curveParamEqn.push_back("0");
	m_curveParamEqn.push_back("0");
	for (int i = 0; i < 3; i++)
	{
		m_curveNodes[i] = NULL;
	}

	m_curveIdx = 0;
	m_lastCurvature = -1;
	m_lastTorsion = -1;
	m_paramStartVal = 0;
	m_paramEndVal = 1;
	m_paramStepIncr = 0.01;
	m_quadSphere = gluNewQuadric();

	m_showAxes = false;
	m_showEvolute = false;
	m_showOffset = false;
	m_showCurvature = false;
	m_showFrenetFrame = false;
	m_showTorsion = false;
	m_showOscSphere = false;



	// bezier stuff below here
	m_state = StateIdle;
	m_currCurveIdx = -1;
	m_lastWeightControlStatus = false;
	m_bsplineDegree = 3;
	m_modifiedCurveIdx = -1;

}

CMFCKit2004View::~CMFCKit2004View() {
	gluDeleteQuadric(m_quadSphere);
}

BOOL CMFCKit2004View::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return CView::PreCreateWindow(cs);
}
//  reset the scene, wither create new curve by specifying the params in the
// dialog box, or proceed to load a new curve from a file:
void CMFCKit2004View::OnFileNew() {
	// TODO: Add your command handler code here
}


#include <sstream>
//  load a new curve from a file
void CMFCKit2004View::OnFileOpen() 
{

	CFileDialog fileDialog(TRUE, ".dat", NULL, 0, NULL, NULL, 0);
	INT_PTR nResult = fileDialog.DoModal();
	if(IDCANCEL == nResult) 
	{
		Invalidate();
		return;
	}
	m_kvmgr.dismiss();
	CString pathName = fileDialog.GetPathName();
	CString fileName = fileDialog.GetFileTitle();
	m_filename = std::string(pathName);
	bool parseRes = false;
	
	parseRes = m_parser.ParseFile(m_filename);
	if (! parseRes)
	{
		Invalidate();
		return;
	}

	m_mgr.ClearAll();
	for(int i=0; i<m_parser.m_curves.size(); ++i)
	{
		ParsedCurve& crv = m_parser.m_curves[i];
		if(SplineTypeBezier == crv.m_type)
		{
			m_currCurveIdx = m_mgr.NewBezierCurve();
			for(int i=0; i<crv.m_pts.size(); ++i)
			{
				m_mgr.AddLastCtrlPt(crv.m_pts[i], 1.0, m_currCurveIdx);
			}
		}
		else if(SplineTypeBspline == crv.m_type)
		{
			m_currCurveIdx = m_mgr.NewBsplineCurve(crv.m_order - 1);
			for(int i=0; i<crv.m_pts.size(); ++i)
			{
				CCagdPoint pt = crv.m_pts[i];
				m_mgr.AddLastCtrlPt(CCagdPoint(pt.x, pt.y, 1.0), pt.z, m_currCurveIdx, true);
			}
			if(!m_mgr.SetKnotVector(m_currCurveIdx, crv.m_knots))
			{
				assert(false);
			}
		}
		else
		{
			// fail
		}
		
	}




	//m_paramStartVal = m_parser.m_paramA;
	//m_paramEndVal = m_parser.m_paramB;
	//for(int i=0; i<3; ++i)
	//{
	//	m_curveParamEqn[i] = m_parser.m_equations[i];
	//	m_curveParamEqn[i][m_curveParamEqn[i].size()-1] = '\0'; //remove Line feed
	//}
	//RecalculateCurve();
	//DrawCurve();
	Invalidate();
}

// This method initialized the OpenGL system.
bool CMFCKit2004View::InitializeOpenGL()
{
    m_pDC = new CClientDC(this);

    if ( NULL == m_pDC ) { // failure to get DC
	    ::AfxMessageBox("Couldn't get a valid DC.");
        return FALSE;
	}

    if ( !SetupPixelFormat() ) {
		::AfxMessageBox("SetupPixelFormat failed.\n");
        return FALSE;
	}

    if ( 0 == (m_hRC = ::wglCreateContext( m_pDC->GetSafeHdc() ) ) ) {
		::AfxMessageBox("wglCreateContext failed.");
        return FALSE;
	}

    if ( FALSE == ::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC ) ) {
	    ::AfxMessageBox("wglMakeCurrent failed.");
        return FALSE;
	}

    // specify black as clear color
    ::glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    // specify the back of the buffer as clear depth (0 closest, 1 farmost)
    ::glClearDepth( 1.0f );
    // enable depth testing (Enable zbuffer - hidden surface removal)
    ::glEnable( GL_DEPTH_TEST );
	// Set default black as background color.
	::glClearColor(0.0, 0.0, 0.0, 1.0f);


    return TRUE;
}



bool CMFCKit2004View::SetupPixelFormat(PIXELFORMATDESCRIPTOR* pPFD)
{
    // default pixel format for a single-buffered,
    // OpenGL-supporting, hardware-accelerated, 
    // RGBA-mode format. Pass in a pointer to a different
    // pixel format if you want something else
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),// size of this pfd
        1,                      // version number
        PFD_DRAW_TO_WINDOW |    // support window
          PFD_SUPPORT_OPENGL |  // support OpenGL
		  PFD_DOUBLEBUFFER,     // double buffered
        PFD_TYPE_RGBA,          // RGBA type
        24,                     // 24-bit color depth
        0, 0, 0, 0, 0, 0,       // color bits ignored
        0,                      // no alpha buffer
        0,                      // shift bit ignored
        0,                      // no accumulation buffer
        0, 0, 0, 0,             // accum bits ignored
        16,                     // 16-bit z-buffer
        0,                      // no stencil buffer
        0,                      // no auxiliary buffer
        PFD_MAIN_PLANE,         // main layer
        0,                      // reserved
        0, 0, 0                 // layer masks ignored
	};

    int pixelformat;
    PIXELFORMATDESCRIPTOR* pPFDtoUse;

    // let the user override the default pixel format
    pPFDtoUse = (0 == pPFD)? &pfd : pPFD; 
 
    if ( 0 == (pixelformat = ::ChoosePixelFormat( m_pDC->GetSafeHdc(), pPFDtoUse )) ) {
	    ::AfxMessageBox("ChoosePixelFormat failed.");
	    return FALSE;
    }

    if ( FALSE == ::SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, pPFDtoUse ) ) {
	    ::AfxMessageBox("SetPixelFormat failed.");
        return FALSE;
    }

    return TRUE;
}


bool CMFCKit2004View::SetupViewingFrustum(void)
{
    // select a default perspective viewing volumn
    ::gluPerspective( 40.0f, m_AspectRatio, 0.1f, 20.0f );

    // NOTE: Other commands you could have here are
    // glFrustum, which gives you much more control over
    // the perspective view, or glOrtho which is used for
    // parallel projections. No matter what you use, check
    // the error status when you set the viewing frustum!

    // if ( GL_NO_ERROR != ::glGetError() ) {
	//   ::AfxMessageBox("Error while trying to set viewing frustum.");
	//    return FALSE;
    //}

    return TRUE;
}


bool CMFCKit2004View::SetupViewingOrtho(void)
{
    // select a default perspective viewing volumn
    ::glOrtho( -2, 2, -2, 2, -2, 2);
	// This gives a projection around the (-2,-2,-2), (2,2,2) coordinate.
	// The fifth and six parameter are near and far. Since it is used as
	// -near and -far, than this gives us the actual near (positive z) to be 2
	// and far (nefative z) to be -2.
	// Note that the view point is from (0,0,0) (yes, inside) towards the negative z.

    // NOTE: Other commands you could have here are
    // glFrustum, which gives you much more control over
    // the perspective view, or glOrtho which is used for
    // parallel projections. No matter what you use, check
    // the error status when you set the viewing frustum!

    //if ( GL_NO_ERROR != ::glGetError() ) {
	//    ::AfxMessageBox("Error while trying to set viewing frustum.");
	//    return FALSE;
    //}

    return TRUE;
}


// This viewing projection gives us a constant aspect ration. This is done by
// increasing the corresponding size of the ortho cube.
bool CMFCKit2004View::SetupViewingOrthoConstAspect(void)
{
	double windowSize = 200;	// the size of the window in GL coord system.


	if ( m_AspectRatio > 1 ) {	// x is bigger than y.
		// Maintain y size and increase x size (and z) accordingly
		// by MULTIPLYING by the aspect ration.
	    ::glOrtho(  -windowSize*m_AspectRatio/2.0, windowSize*m_AspectRatio/2.0,
					-windowSize/2.0, windowSize/2.0,
					-30*windowSize*m_AspectRatio/2.0, 30*windowSize*m_AspectRatio/2.0);
//					0, 30*windowSize*m_AspectRatio);
	} else {
		// Maintain x size and increase y size (and z) accordingly, 
		// by DIVIDING the aspect Ration (because it's smaller than 1).
	    ::glOrtho(  -windowSize/2.0, windowSize/2.0,
					-windowSize/m_AspectRatio/2.0, windowSize/m_AspectRatio/2.0,
//					-windowSize/m_AspectRatio/2.0, windowSize/m_AspectRatio/2.0);
					0, 3*windowSize/m_AspectRatio);
	}

//    if ( GL_NO_ERROR != ::glGetError() ) {
//	    ::AfxMessageBox("Error while trying to set viewing frustum.");
//	    return FALSE;
//	}

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004View drawing

void CMFCKit2004View::OnDraw(CDC* pDC) {
//	CMFCKit2004Doc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);
	drawSegments(GL_RENDER);
	glFlush();
	SwapBuffers(wglGetCurrentDC());

	// TODO: add draw code for native data here
}


void CMFCKit2004View::OnPaint() {
	CPaintDC dc(this); // device context for painting
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// clear screen and zbuffer
	drawSegments(GL_RENDER);

	FrenetOnPaintExtend();

	glFlush();
	SwapBuffers(wglGetCurrentDC());

//	OnDraw(&dc);
//	drawSegments(GL_RENDER);
//	glFlush();
//	SwapBuffers(wglGetCurrentDC());
}
/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004View diagnostics

#ifdef _DEBUG
void CMFCKit2004View::AssertValid() const {
	CView::AssertValid();
}

void CMFCKit2004View::Dump(CDumpContext& dc) const {
	CView::Dump(dc);
}

CMFCKit2004Doc* CMFCKit2004View::GetDocument() /* non-debug version is inline*/ {
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCKit2004Doc)));
	return (CMFCKit2004Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004View message handlers

int CMFCKit2004View::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitializeOpenGL();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glGetDoublev(GL_MODELVIEW_MATRIX,modelView);
//	SetupViewingOrthoConstAspect();
	OnOptionsReset();
	
	//Scale(100.0);

	return 0;
}

BOOL CMFCKit2004View::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	//return CView::OnEraseBkgnd(pDC);
	return true;
}

void CMFCKit2004View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (nChar == 61) //+
	{
		Scale(2.0);
	}
	if (nChar == 45) //-
	{
		Scale(0.5);
	}
	Invalidate();
//	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CMFCKit2004View::OnLButtonDown(UINT nFlags, CPoint point) {
	SetCapture();	// capture the mouse 'right button up' command
	prevMouseLocation = point;
	LButtonDown = true;
	findCtlPoint(point.x, point.y);


	if(m_state == StateIdle)
	{
		bool found = m_kvmgr.attemptAnchor(point.x, point.y);
		if (found)
			return;
		m_weightCtrlAnchor = m_mgr.AttemptWeightAnchor(point.x, point.y);
		// look for the point within the curves
		m_draggedPt.m_pt = m_mgr.PickControlPoint(point.x, point.y);
		if(m_draggedPt.m_pt.IsValid())
		{
			// dragging stuff
			m_draggedPt.m_startPosScreen = CCagdPoint(point.x, point.y);
		}
	}
	

	/*int frameIdx = m_ffmgr.PickFrame(point.x, point.y);
	if(frameIdx >= 0) 
	{
		ASSERT(m_ffmgr.GetFrameCount() > frameIdx);
		m_curveIdx = frameIdx;
		DrawFrenetComponents(m_curveIdx);
	}*/

}

void CMFCKit2004View::OnLButtonUp(UINT nFlags, CPoint point) {
	if (GetCapture() == this) ::ReleaseCapture();
	LButtonDown = false;
	
	// release the dragged point
	m_draggedPt.m_pt.Invalidate();
	m_weightCtrlAnchor.Invalidate();
	m_kvmgr.dropAnchor();

	CCagdPoint cp[2];
	cagdToObject(point.x, point.y, cp);
	m_lastLbuttonUp = cp[0];
	m_lastLbuttonUp.z = 0.0;

	if(StateAddBezierPts == m_state)
	{
		CCagdPoint coord[2];
		cagdToObject(point.x, point.y, coord);
		coord[0].z = 0.0;
		m_mgr.AddLastCtrlPt(coord[0], 1, m_currCurveIdx);
		
	}
	else if(StateAddBSplinePts == m_state)
	{
		CCagdPoint coord[2];
		cagdToObject(point.x, point.y, coord);
		coord[0].z = 0.0;
		m_mgr.AddLastCtrlPt(coord[0], 1, m_currCurveIdx);
		if (m_modifiedCurveIdx == m_currCurveIdx)
		{
			vector<double> kv = m_mgr.GetKnotVector(m_currCurveIdx);
			m_kvmgr.setVector(kv);
			m_kvmgr.show();
		}
	}
	else if (m_state == StateConnectingCurvesG0 || m_state == StateConnectingCurvesG1 || m_state == StateConnectingCurvesC1)
	{
		int idx2 = m_mgr.getCurveIndexByPointOnPolygon(m_lastLbuttonUp);
		if (idx2 != -1)
		{
			if (m_state == StateConnectingCurvesG0)
				m_mgr.connectG0(m_currCurveIdx, idx2);
			else if (m_state == StateConnectingCurvesG1)
				m_mgr.connectG1(m_currCurveIdx, idx2);
			else if (m_state == StateConnectingCurvesC1)
				m_mgr.connectC1(m_currCurveIdx, idx2);

			m_state = StateIdle;
		}
	}

	Invalidate();					// redraw scene
}

void CMFCKit2004View::OnRButtonDown(UINT nFlags, CPoint point) {
	SetCapture();	// capture the mouse 'right button up' command
	prevMouseLocation = point;
	RButtonDown = true;

}

void CMFCKit2004View::OnRButtonUp(UINT nFlags, CPoint point) {
	if (GetCapture() == this) ::ReleaseCapture();
	RButtonDown = false;

	

	if (StateIdle != m_state)
	{
		m_state = StateIdle;
		return;
	}
	// this code pops up the menu called "IDR_POPUPMENU" on the screen at coordinate "point"
	CCagdPoint cp[2];
	cagdToObject(point.x, point.y, cp);

	m_lastRbuttonUp = cp[0];
	m_lastRbuttonUp.z = 0.0;
	CMenu popupMenu;

	if (m_kvmgr.isUnderCursor(point.x, point.y))
	{
		popupMenu.LoadMenu(IDR_MENU4);
	}
	else
	{
		PtContext cxt = m_mgr.getPtContext(m_lastRbuttonUp);
		switch (cxt)
		{
		case ContextEmpty:
			popupMenu.LoadMenu(IDR_POPUPMENU);
			break;

		case ContextBezierPoly:
			popupMenu.LoadMenu(IDR_MENU1);
			break;

		case ContextBsplinePoly:
			popupMenu.LoadMenu(IDR_MENU3);
			break;

		case ContextBezierPt:
			popupMenu.LoadMenu(IDR_MENU2);
			break;
		case ContextBsplinePt:
			popupMenu.LoadMenu(IDR_MENU2);
			break;
		default:
			popupMenu.LoadMenu(IDR_POPUPMENU);
		}
	}

	m_draggedPt.m_pt = m_mgr.PickControlPoint(point.x, point.y);
	if(m_draggedPt.m_pt.IsValid())
	{
		CCagdPoint(point.x, point.y);
	}
	

	CMenu* subMenu = popupMenu.GetSubMenu(0);
	ClientToScreen(&point);
	subMenu->TrackPopupMenu(0, point.x, point.y,
	   AfxGetMainWnd(), NULL);
	Invalidate();
}

void CMFCKit2004View::OnMouseMove(UINT nFlags, CPoint point) {

	if ((GetCapture() != this) || (!LButtonDown)) 		// 'this' has the mouse capture
	{ return; }

	if (m_state != StateIdle)
		return;

	if(::GetAsyncKeyState(VK_CONTROL))
	{ 
		// This is the movement ammount 
		int valuex(point.x - prevMouseLocation.x);
		int valuey(point.y - prevMouseLocation.y);	
		if (LButtonDown) // rotateXY
			RotateXY(valuey*RSense,valuex*RSense);
		else if (RButtonDown) // rotateZ
			RotateZ(valuex);
	}
	else if (::GetAsyncKeyState(VK_SHIFT))
	{
		int valuex(point.x - prevMouseLocation.x);
		int valuey(point.y - prevMouseLocation.y);	
		if (LButtonDown)  // translateXY
			translateXY(valuex*TSense,-valuey*TSense);

	}
	else
	{
		if (m_kvmgr.isAnchored())
		{
			m_kvmgr.updateLastAnchor(point.x, point.y);
			if (m_kvmgr.changedSinceLastGet(0.05))
			{
				vector<double> vec = m_kvmgr.getVector();
				m_mgr.SetKnotVector(m_modifiedCurveIdx, vec);
			}
		}
		else if (m_weightCtrlAnchor.IsValid())
		{
			m_mgr.ChangeWeight(m_weightCtrlAnchor.m_curveIdx, m_weightCtrlAnchor.m_pointIdx, point.x, point.y);
		}
		else if(m_draggedPt.m_pt.IsValid())
		{
			CCagdPoint pts[2];
			cagdToObject(point.x, point.y, pts);
			pts[0].z = 0.0;
			m_mgr.UpdateCtrlPtPos(m_draggedPt.m_pt, pts[0]);
		}
	}

	Invalidate();					// redraw scene
	prevMouseLocation = point;
}


BOOL CMFCKit2004View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	UINT state = ::GetAsyncKeyState(VK_CONTROL);
	if(state)
	{
		if(NULL == m_curveNodes[0])
		{
			Invalidate();
			return true;
		}
		int newVal;
		if(zDelta>0) 
		{
			newVal = m_curveIdx + 1;
		}
		else
		{
			newVal = m_curveIdx - 1;
		}
		if(newVal < 0) 
		{
			newVal = m_ffmgr.GetFrameCount()-1;
		} 
		else if(newVal > m_ffmgr.GetFrameCount() -1)
		{
			newVal = 0;
		}
		m_curveIdx = newVal;
		DrawFrenetComponents(m_curveIdx);
	}
	else
	{
		Scale(1 + double(zDelta)/300);
		m_kvmgr.setDimensions(m_WindowWidth, m_WindowHeight);
		m_kvmgr.show();
	}
	Invalidate();
	return true;
}

void CMFCKit2004View::OnTimer(UINT nIDEvent) {
	if(NULL == m_curveNodes[0])
	{
		Invalidate();
	}
	int newVal;
	if(m_animSpeed > 0) 
	{
		newVal = m_curveIdx + 1;
	}
	else
	{
		newVal = m_curveIdx - 1;
	}
	if(newVal < 0) 
	{
		newVal = m_ffmgr.GetFrameCount()-1;
	} 
	else if(newVal > m_ffmgr.GetFrameCount() -1)
	{
		newVal = 0;
	}
	m_curveIdx = newVal;
	DrawFrenetComponents(m_curveIdx);
	Invalidate();
}


void CMFCKit2004View::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
    if ( 0 >= cx || 0 >= cy ) {
        return;
	}

    // save the width and height of the current window
    m_WindowWidth = cx;
    m_WindowHeight = cy;
	viewPort[0] = viewPort[1] = 0;
	viewPort[2] = m_WindowWidth;
	viewPort[3] = m_WindowHeight;
    // compute the aspect ratio
    // this will keep all dimension scales equal
    m_AspectRatio = (GLdouble)m_WindowWidth/(GLdouble)m_WindowHeight;

    // Now, set up the viewing area-select the full client area
    ::glViewport(0, 0, m_WindowWidth, m_WindowHeight);

    //if ( GL_NO_ERROR != ::glGetError() ) {
	//	::AfxMessageBox("Error while trying to set viewport.");
    //}

    // select the projection matrix as the recipient of
    // matrix operations (there's three to choose from)
    ::glMatrixMode(GL_PROJECTION);
    // initialize the projection matrix to a pristine state
    ::glLoadIdentity();

    // select the viewing volume. You do it after you
    // get the aspect ratio and set the viewport
    //SetupViewingFrustum( );
	//SetupViewingOrtho();
	SetupViewingOrthoConstAspect();

    // now select the modelview matrix and clear it
    // this is the mode we do most of our calculations in
    // so we leave it as the default mode.
    ::glMatrixMode(GL_MODELVIEW);
//    ::glLoadIdentity();
	
}

// find a control point
CCagdPoint *CMFCKit2004View::findCtlPoint(int x, int y) {
	UINT id;
	for (cagdPick(x,y);id = cagdPickNext();) {
		if (cagdGetSegmentType(id) == CAGD_SEGMENT_CTLPLYGN) {
			draggedPolyline = id;
			draggedPoint = cagdGetNearestVertex(id,x,y)-1;
			CCagdPoint *ret;
			ret = cagdGetVertex(id,draggedPoint);
			int retx,rety;
			cagdToWindow(ret,&retx,&rety);
			if (abs(retx-x) > 5 || abs(rety-y) > 5)
				continue;
			return ret;
		}
	}
	draggedPoint = -1;
	draggedCtlPt = NULL;
	return NULL;
}

UINT CMFCKit2004View::findCircle(int x, int y) {
	UINT id;
	for (cagdPick(x,y);id = cagdPickNext();) {
		if (list[id].type == CAGD_SEGMENT_WCIRCLE) {
			draggedCircle = id;
			draggedCtlPt = list[id].ctlPt;
			return id;
		}
	}
	return -1;
}


void CMFCKit2004View::RotateXY(double valX, double valY) {
	glLoadIdentity();
	glRotated(valX,1.0, 0, 0);
	glRotated(valY,0, 1.0, 0);
	glMultMatrixd (modelView);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
}

void CMFCKit2004View::RotateZ (double val) {
	glLoadIdentity();
	glRotated(val,0, 0, 1.0);
	glMultMatrixd (modelView);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
}

void CMFCKit2004View::translateXY(double valX, double valY) {
	glLoadIdentity();
	glTranslated(valX, valY, 0);
	glMultMatrixd (modelView);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
}

void CMFCKit2004View::Scale (double val) {
	glLoadIdentity();
	glScaled(val, val, val);
	glMultMatrixd (modelView);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
}

void CMFCKit2004View::OnFuzzinessMore() {
	fuzziness *= 2;
	moreFuzziness();
}

void CMFCKit2004View::OnFuzzinessLess() {
	fuzziness /= 2;
	lessFuzziness();
}
void CMFCKit2004View::OnOptionsReset() {
	cagdReset();
	Scale(20.0);
	Invalidate();
}

void CMFCKit2004View::OnRotationMore() {
	RSense*= 1.05;
}

void CMFCKit2004View::OnRotationLess() {
	RSense*= 0.95;
}

void CMFCKit2004View::OnTranslationMore() {
	TSense*= 1.05;
}

void CMFCKit2004View::OnTranslationLess() {
	TSense*= 0.95;
}

void CMFCKit2004View::OnScalingMore() {
	SSense*= 1.05;
}

void CMFCKit2004View::OnScalingLess() {
	SSense*= 0.95;
}

void CMFCKit2004View::TestCagdMath()
{
	// test code
	CCagdPoint a(1.0, 2.0, 3.0);
	CCagdPoint b(3.5, 7.5, 4.0);
	CCagdPoint c = a + b;
	CCagdPoint d = b - CCagdPoint(0.5, 0.1, 0.4);
	CCagdPoint e = a * 5.0;
	CCagdPoint f = 3.0 * b;
	CCagdPoint g = a / 5.0;
	double h = dot(a, b);
	CCagdPoint k = cross(CCagdPoint(1,0,0), CCagdPoint(0,0,1));
	CCagdPoint l = length(b);
	CCagdPoint n = normalize(b);
}



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//    Frenet methods:  
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


void CMFCKit2004View::OnFrenetProperties() {
	CPropDlg dlg;
	dlg.m_degree = m_bsplineDegree;
	dlg.m_step = m_paramStepIncr;
	if (dlg.DoModal() == IDOK) 
	{
		bool changed = false;
		if(m_paramStepIncr != dlg.m_step)
		{
			changed = true;
		}

		m_paramStepIncr = dlg.m_step;
		m_bsplineDegree = dlg.m_degree;
		//m_curveParamEqn[i][m_curveParamEqn[i].size()-1] = '\0'; //remove Line feed
		// do the rest of your stuff here:

		if(changed)
		{
			m_mgr.SetBSplineSamplingStep(m_paramStepIncr);

			for(int i=0; i<m_mgr.CurveCount(); ++i)
			{
				m_mgr.RedrawCurve(i);
			}
		}


	}
	Invalidate();
}

void CMFCKit2004View::OnFrenetShowaxes() {
	m_showAxes = !m_showAxes;
	RecalculateCurve();
	Invalidate();
}

void CMFCKit2004View::OnUpdateFrenetShowaxes(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(m_showAxes);
}

void CMFCKit2004View::OnFrenetShowfrenetframe() {
	m_showFrenetFrame = !m_showFrenetFrame;
	DrawFrenetComponents(m_curveIdx);
}

void CMFCKit2004View::OnUpdateFrenetShowfrenetframe(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(m_showFrenetFrame);
}

void CMFCKit2004View::OnFrenetShowcurvature() {
	m_showCurvature = !m_showCurvature;
	DrawFrenetComponents(m_curveIdx);
}

void CMFCKit2004View::OnUpdateFrenetShowcurvature(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(m_showCurvature);
}

void CMFCKit2004View::OnFrenetShowOscSphere() {
	m_showOscSphere = !m_showOscSphere;
}

void CMFCKit2004View::OnUpdateFrenetShowOscSphere(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(m_showOscSphere);
}

void CMFCKit2004View::OnFrenetShowtorsion() {
	m_showTorsion = !m_showTorsion;
	DrawFrenetComponents(m_curveIdx);
}

void CMFCKit2004View::OnUpdateFrenetShowtorsion(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(m_showTorsion);
}

void CMFCKit2004View::OnFrenetShow() {
}

void CMFCKit2004View::OnUpdateFrenetShow(CCmdUI *pCmdUI) {
	
}

void CMFCKit2004View::OnFrenetDrawevolute() {
	m_showEvolute = ! m_showEvolute;
	m_ffmgr.ShowEvolute(m_showEvolute);
}

void CMFCKit2004View::OnUpdateFrenetDrawevolute(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(m_showEvolute);
}

void CMFCKit2004View::OnFrenetDrawoffset() {
	m_showOffset = ! m_showOffset;
	m_ffmgr.ShowOffset(m_showOffset);
}

void CMFCKit2004View::OnUpdateFrenetDrawoffset(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(m_showOffset);
}

void CMFCKit2004View::OnAnimationStart() {
  UINT timeMs = (int)(1000.0 * (1.0 / double(m_animSpeed)));
  this->SetTimer(1, timeMs, NULL);
  m_animStarted = true;
}

void CMFCKit2004View::OnAnimationStop() {
  this->KillTimer(1);
  m_animStarted = false;
}

void CMFCKit2004View::RecalculateCurve()
{
	// 
	for (int i = 0; i < 3; i++)
	{
		if (NULL != m_curveNodes[i])
		{
			e2t_freetree(m_curveNodes[i]);
			m_curveNodes[i] = NULL;
		}
		m_curveNodes[i] = e2t_expr2tree(m_curveParamEqn[i].c_str());
	}

	cagdFreeAllSegments();

	// draw axes at origin if requested
	if(m_showAxes)
	{
		DrawAxes();
	}

	// set up curve
	m_ffmgr.SetEquations(m_curveNodes[0], m_curveNodes[1], m_curveNodes[2]);
	m_ffmgr.SetD(m_offsetD);
	m_ffmgr.Calculate(m_paramStartVal, m_paramEndVal, m_paramStepIncr);
	m_ffmgr.DrawCurve();
	m_ffmgr.ShowEvolute(m_showEvolute);
	m_ffmgr.ShowOffset(m_showOffset);

	if(m_ffmgr.GetFrameCount() <= m_curveIdx)
	{
		m_curveIdx = 0;
	}
	DrawFrenetComponents(m_curveIdx);


}

void CMFCKit2004View::DrawAxes()
{
	CCagdPoint xAxis[2];
	CCagdPoint yAxis[2];
	CCagdPoint zAxis[2];

	double axisLength = 1000.0;
	xAxis[0] = yAxis[0] = zAxis[0] = CCagdPoint(0,0,0);
	xAxis[1] = CCagdPoint(1,0,0) * axisLength;
	yAxis[1] = CCagdPoint(0,1,0) * axisLength;
	zAxis[1] = CCagdPoint(0,0,1) * axisLength;
	cagdSetSegmentColor(cagdAddPolyline(xAxis, 2,  CAGD_SEGMENT_POLYLINE), 128,0,0);
	cagdSetSegmentColor(cagdAddPolyline(yAxis, 2,  CAGD_SEGMENT_POLYLINE), 0,128,0);
	cagdSetSegmentColor(cagdAddPolyline(zAxis, 2,  CAGD_SEGMENT_POLYLINE), 30,100,128);


}

void CMFCKit2004View::DrawFrenetComponents(int idx)
{
	if((m_ffmgr.GetFrameCount() <= idx) || (0 > idx))
	{ return;	}

	if (m_showFrenetFrame)
	{
		m_ffmgr.DrawFrenetFrame(idx);
	}
	else
	{
		m_ffmgr.ClearLastFrame();
	}
	if (m_showCurvature)
	{
		m_ffmgr.DrawOscCircle(idx);
	}
	else
	{
		m_ffmgr.ClearLastOscCircle();
	}
	if (m_showTorsion)
	{
		m_ffmgr.DrawTorsion(idx);
	}
	else
	{
		m_ffmgr.ClearLastTorsion();
	}
	
}

void CMFCKit2004View::FrenetOnPaintExtend()
{
	if(0 < m_ffmgr.GetFrameCount())
	{
		if (m_showCurvature)
		{
			const FrenetFrame& ff = m_ffmgr.GetFrame(m_curveIdx);

			const CCagdPoint* circleCtr = m_ffmgr.GetEvoluteAtIndex(m_curveIdx);
			if(NULL != circleCtr)
			{
				//CCagdPoint ctr = ff.m_origin;
				//CCagdPoint tpt = ff.m_T*3*(1/ff.m_k) + ctr;
				//CCagdPoint mtpt = ctr - ff.m_T*3*(1/ff.m_k);
				//CCagdPoint tr = (tpt - ff.m_N*3*(1/ff.m_k)); // top right
				//CCagdPoint tl = (tpt + ff.m_N*3*(1/ff.m_k)); // top left
				//CCagdPoint br = (mtpt - ff.m_N*3*(1/ff.m_k)); // bottom right
				//CCagdPoint bl = (mtpt + ff.m_N*3*(1/ff.m_k)); // bottom left

				CCagdPoint up = normalize(ff.m_B);
				CCagdPoint zAxis(0,0,1);
				//double radius = length(center - origin);
				CCagdPoint rotAxis = cross(zAxis, up);
				double angle = asin(length(rotAxis)) * (180.0 / PI);
				double dotProd = dot(zAxis, up);
				if(dotProd < 0) {
					angle = 180.0 - angle;
				}
				

				//glColor4ub(0, 50, 0, 240); // dark green, with alpha less than 1
				glColor4ub(128, 0, 128, 35);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE); // need for blending
				glEnable(GL_BLEND); // turn on blending
				glDisable(GL_DEPTH_TEST); // don't think we need this
				glPointSize(40);

				// make a counter-clockwise set of 2 triangles
				//glBegin(GL_TRIANGLES);
				//glVertex3dv((GLdouble *)&tr);
				//glVertex3dv((GLdouble *)&br);
				//glVertex3dv((GLdouble *)&bl);
				//glVertex3dv((GLdouble *)&tr);
				//glVertex3dv((GLdouble *)&bl);
				//glVertex3dv((GLdouble *)&tl);
				//glEnd();
				
				glPushMatrix();
					
					glTranslated(circleCtr->x, circleCtr->y, circleCtr->z);
					glRotated(angle, rotAxis.x, rotAxis.y, rotAxis.z);
					glScaled(1.0, 1.0, 0.001);

					gluSphere(m_quadSphere, (1.0/ff.m_k)*1.5, 100, 5);
				glPopMatrix();
			

				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
			}
		}



		if (m_showOscSphere)
		{
			CCagdPoint origin = m_ffmgr.GetFrame(m_curveIdx).m_origin;
			CCagdPoint center = m_ffmgr.GetOscSphereCenter(m_curveIdx);
			CCagdPoint up = normalize(origin-center);
			CCagdPoint zAxis(0,0,1);
			double radius = length(center - origin);
			CCagdPoint rotAxis = cross(zAxis, up);
			double angle = asin(length(rotAxis)) * (180.0 / PI);
			double dotProd = dot(zAxis, up);
			if(dotProd < 0) {
				angle = 180.0 - angle;
			}

			glColor4ub(100, 20, 0, 50);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE); // need for blending
			glEnable(GL_BLEND); // turn on blending
			glDisable(GL_DEPTH_TEST); // don't think we need this
			glPointSize(40);

			glPushMatrix();
				glTranslated(center.x, center.y, center.z);


				glPushMatrix();
					glRotated(angle, rotAxis.x, rotAxis.y, rotAxis.z);

					// finally, draw the spheres
					gluSphere(m_quadSphere, radius, 100, 100);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glColor4ub(0,175,0,175);
					gluSphere(m_quadSphere, radius, 25, 8);			
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glPopMatrix();

			glPopMatrix();
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}

		if(m_showEvolute)
		{
			const CCagdPoint* evolutePt = m_ffmgr.GetEvoluteAtIndex(m_curveIdx);
			if(NULL != evolutePt)
			{
				CCagdPoint ePt = *evolutePt;
				glColor4ub(255, 128, 0, 150);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE); // need for blending
				glEnable(GL_BLEND); // turn on blending
				glDisable(GL_DEPTH_TEST); // don't think we need this
				glPushMatrix();
					glTranslated(ePt.x, ePt.y, ePt.z);
					gluSphere(m_quadSphere, 0.01, 15, 15);
				glPopMatrix();

				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
			}
		}

		if(m_showOffset)
		{
			const CCagdPoint* offsetPt = m_ffmgr.GetOffsetAtIndex(m_curveIdx);
			if(NULL != offsetPt)
			{
				CCagdPoint oPt = *offsetPt;
				glColor4ub(0, 128, 255, 150);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE); // need for blending
				glEnable(GL_BLEND); // turn on blending
				glDisable(GL_DEPTH_TEST); // don't think we need this

				glPushMatrix();
					glTranslated(oPt.x, oPt.y, oPt.z);
					gluSphere(m_quadSphere, 0.01, 15, 15);
				glPopMatrix();

				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);

			}

		}
	}
}

void CMFCKit2004View::OnContextbgNewbeziercurve()
{
	m_state = StateAddBezierPts;
	m_currCurveIdx = m_mgr.NewBezierCurve();
	// change cursor
}

void CMFCKit2004View::OnContextbgNewbsplinecurve()
{
	m_state = StateAddBSplinePts;
	m_currCurveIdx = m_mgr.NewBsplineCurve(m_bsplineDegree);
}

void CMFCKit2004View::OnContextbgClearall()
{
	m_mgr.ClearAll();
	m_kvmgr.dismiss();
	m_modifiedCurveIdx = -1;
	Invalidate();
}

void CMFCKit2004View::OnContextpolygonInsertpoint()
{
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	m_mgr.AddCtrlPt(m_lastRbuttonUp, 1, m_currCurveIdx);
	if (m_modifiedCurveIdx == m_currCurveIdx)
	{
		vector<double> kv = m_mgr.GetKnotVector(m_currCurveIdx);
		m_kvmgr.setVector(kv);
		m_kvmgr.show();
	}
	Invalidate();
}

void CMFCKit2004View::OnContextpolygonAppendpoint()
{
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	if (-1 != m_currCurveIdx)
	{
		m_state = StateAddBezierPts;
	}
	Invalidate();
}

void CMFCKit2004View::OnContextpolygonShowHideControlPolygon()
{
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	m_mgr.ToggleShowPolygon(m_currCurveIdx);
	Invalidate();
}


void CMFCKit2004View::OnContextptAdjustweight()
{
	m_lastWeightControlStatus = m_mgr.ToggleWeightConrol(m_lastRbuttonUp);
	Invalidate();
}

void CMFCKit2004View::OnUpdateContextptAdjustweight(CCmdUI *pCmdUI)
{
	//pCmdUI->SetCheck(m_lastWeightControlStatus);
}


void CMFCKit2004View::OnContextpolygonRaisedegree()
{
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	if (-1 != m_currCurveIdx)
	{
		m_mgr.RaiseDegree(m_currCurveIdx);
	}
	Invalidate();
}


void CMFCKit2004View::OnContextpolygonSubdivide()
{
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	if (-1 != m_currCurveIdx)
	{
		m_mgr.Subdivide(m_currCurveIdx);
	}
	Invalidate();
}

void CMFCKit2004View::OnContextptRemovepoint()
{
	m_mgr.RemoveCtrlPt(m_lastRbuttonUp);
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	if (m_modifiedCurveIdx == m_currCurveIdx)
	{
		vector<double> kv = m_mgr.GetKnotVector(m_currCurveIdx);
		m_kvmgr.setVector(kv);
		m_kvmgr.show();
	}
	Invalidate();
}


void CMFCKit2004View::OnOptionsShowgrid()
{
	m_mgr.showGrid();
	Invalidate();
}


void CMFCKit2004View::OnFileSaveGeometry()
{
	char strFilter[] = { "ITD Files (*.itd)|*.itd|DAT Diles (*.dat)|*.dat||" };
	CFileDialog fileDialog = CFileDialog(FALSE, "itd", NULL, OFN_OVERWRITEPROMPT, strFilter, NULL);
	INT_PTR nResult = fileDialog.DoModal();
	if(IDCANCEL == nResult) 
	{
		Invalidate();
		return;
	}
	CString pathName = fileDialog.GetPathName();
	CString fileName = fileDialog.GetFileTitle();
	string fileExt	 = std::string(fileDialog.GetFileExt());
	string filename = std::string(pathName);
	string res;
	if ("itd" == fileExt)
	{
		res = m_mgr.toIrit();
	}
	else if ("dat" == fileExt)
	{
		res = m_mgr.toDat();
	}
	else
	{
		Invalidate();
		return;
	}
	std::ofstream out(filename.c_str());
	out << res;
	out.close();
	Invalidate();	
}


void CMFCKit2004View::OnContextbsplinepolyModifyknotvector()
{
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	if (-1 != m_currCurveIdx)
	{
		//do stuff
		m_modifiedCurveIdx = m_currCurveIdx;
		vector<double> kv = m_mgr.GetKnotVector(m_currCurveIdx);
		m_kvmgr.setDimensions(m_WindowWidth, m_WindowHeight);
		m_kvmgr.setVector(kv);
		m_kvmgr.show();
	}
	Invalidate();
}


void CMFCKit2004View::OnConnecttowithcontinuityG0()
{
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	m_state = StateConnectingCurvesG0;
}


void CMFCKit2004View::OnConnecttowithcontinuityG1()
{
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	m_state = StateConnectingCurvesG1;
}


void CMFCKit2004View::OnConnecttowithcontinuityC1()
{
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	m_state = StateConnectingCurvesC1;
}


void CMFCKit2004View::OnKnotguiRemoveknot()
{
	vector<double> kv = m_kvmgr.getVector();
	int idx = m_kvmgr.idxAtPoint(m_lastRbuttonUp);
	vector<double> newKv;
	if (idx != -1)
	{
		for (int i = 0; i < kv.size(); i++)
		{
			if (i != idx)
				newKv.push_back(kv[i]);
		}
		m_mgr.SetKnotVector(m_modifiedCurveIdx, newKv);
		m_kvmgr.setVector(newKv);
		m_kvmgr.show();
	}
}


void CMFCKit2004View::OnKnotguiInsertknot()
{
	m_kvmgr.addKnotAtPoint(m_lastRbuttonUp);
	vector<double> kv = m_kvmgr.getVector(); // It sorts it inside
	m_mgr.SetKnotVector(m_modifiedCurveIdx, kv);
	m_kvmgr.show();
}


void CMFCKit2004View::OnContextbsplinepolyAppendpoint()
{
	m_currCurveIdx = m_mgr.getCurveIndexByPointOnPolygon(m_lastRbuttonUp);
	if (-1 != m_currCurveIdx)
	{
		m_state = StateAddBSplinePts;
	}
	Invalidate();
}


void CMFCKit2004View::OnKnotguiInsertknotBoehm()
{
	double knotval = m_kvmgr.guiXtoknot(m_lastRbuttonUp.x);
	if(m_mgr.InsertKnot(m_modifiedCurveIdx, knotval))
	{
		m_kvmgr.addKnotAtPoint(m_lastRbuttonUp);
	}
	//vector<double> kv = m_kvmgr.getVector(); // It sorts it inside
	//m_mgr.SetKnotVector(m_modifiedCurveIdx, kv);
}
