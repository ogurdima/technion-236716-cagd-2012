// MFCKit2004View.cpp : implementation of the CMFCKit2004View class
//

#include "stdafx.h"
#include "MFCKit2004.h"

#include "MFCKit2004Doc.h"
#include "MFCKit2004View.h"
#include "internal.h"
#include "gl/gl.h"
#include "gl/glu.h"

#include "ProductDialog.h"
#include "RhoDialog.h"
#include "ManualDialog.h"
#include ".\mfckit2004view.h"

#include <iostream>
#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double modelView[16];		// the modelview Matrix
int viewPort[4];			// viewport - actually [width, height, 0, 0]

extern SEGMENT *list;


void clear_all();
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
	ON_COMMAND(ID_MODE_VIEW, OnModeView)
	ON_COMMAND(ID_MODE_CONTROL, OnModeControl)
	ON_UPDATE_COMMAND_UI(ID_MODE_CONTROL, OnUpdateModeControl)
	ON_UPDATE_COMMAND_UI(ID_MODE_VIEW, OnUpdateModeView)
	ON_COMMAND(ID_STUFF_2, OnStuff2)
	ON_UPDATE_COMMAND_UI(ID_STUFF_2, OnUpdateStuff2)
	ON_COMMAND(ID_DEMO_POLYLINE, OnDemoPolyline)
	ON_UPDATE_COMMAND_UI(ID_DEMO_POLYLINE, OnUpdateDemoPolyline)
	ON_COMMAND(ID_DEMO_ANIMATION, OnDemoAnimation)
	ON_UPDATE_COMMAND_UI(ID_DEMO_ANIMATION, OnUpdateDemoAnimation)
	ON_COMMAND(ID_DEMO_NONE, OnDemoNone)
	ON_UPDATE_COMMAND_UI(ID_DEMO_NONE, OnUpdateDemoNone)
	ON_COMMAND(ID_STUFF_3, OnStuff3)
	ON_COMMAND(ID_STUFF_4, OnStuff4)
	ON_UPDATE_COMMAND_UI(ID_STUFF_4, OnUpdateStuff4)
	ON_COMMAND(ID_STUFF_5, OnStuff5)
	ON_UPDATE_COMMAND_UI(ID_STUFF_5, OnUpdateStuff5)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_CONICS_SET4THPOINT, OnConicsSet4thpoint)
	ON_UPDATE_COMMAND_UI(ID_CONICS_SET4THPOINT, OnUpdateConicsSet4thpoint)
	ON_COMMAND(ID_CONICS_SETPRODUCT, OnConicsSetproduct)
	ON_UPDATE_COMMAND_UI(ID_CONICS_SETPRODUCT, OnUpdateConicsSetproduct)
	ON_UPDATE_COMMAND_UI(ID_CONICS_INTERNALPTP, OnUpdateConicsInternalptp)
	ON_COMMAND(ID_CONICS_INTERNALPTP, OnConicsInternalptp)
	ON_COMMAND(ID_CONICS_ENTIRECONIC, OnConicsEntireconic)
	ON_UPDATE_COMMAND_UI(ID_CONICS_ENTIRECONIC, OnUpdateConicsEntireconic)
	ON_COMMAND(ID_STUFF_CLEARALL, OnStuffClearall)
	ON_COMMAND(ID_CONICS_SETRHO, OnConicsSetrho)
	ON_UPDATE_COMMAND_UI(ID_CONICS_SETRHO, OnUpdateConicsSetrho)
	ON_COMMAND(ID_CONICS_SETTANGENT, OnConicsSettangent)
	ON_UPDATE_COMMAND_UI(ID_CONICS_SETTANGENT, OnUpdateConicsSettangent)
	ON_COMMAND(ID_CONICS_CURVE, OnConicsCurve)
	ON_UPDATE_COMMAND_UI(ID_CONICS_CURVE, OnUpdateConicsCurve)
	ON_COMMAND(ID_CONICS_STRINGART, OnConicsStringart)
	ON_UPDATE_COMMAND_UI(ID_CONICS_STRINGART, OnUpdateConicsStringart)
	ON_COMMAND(ID_CONICS_BOTH, OnConicsBoth)
	ON_UPDATE_COMMAND_UI(ID_CONICS_BOTH, OnUpdateConicsBoth)
	ON_COMMAND(ID_CONICS_SHOWASYMPTOTES, OnConicsShowasymptotes)
	ON_UPDATE_COMMAND_UI(ID_CONICS_SHOWASYMPTOTES, OnUpdateConicsShowasymptotes)
	ON_COMMAND(ID_HELP_USER, OnHelpUser)
	ON_COMMAND(ID_VIEW_CLEARVIEW, CMFCKit2004View::OnViewClearview)
	ON_COMMAND(ID_VIEW_RESETCAMERA, CMFCKit2004View::OnViewResetcamera)
	ON_UPDATE_COMMAND_UI(ID_LAMBDA_INDICATOR, OnUpdateLambda)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004View construction/destruction

CMFCKit2004View::CMFCKit2004View() {
	// TODO: add construction code here
	SSense = RSense = TSense = 0.5;
	RButtonDown = LButtonDown = false;
	CtrlKeyDown = false;
	m_Mode = ID_MODE_VIEW;
	demoMode = ID_DEMO_POLYLINE;
	draggedPolyPoint = -1;
	draggedPolyline = 0;
	draggedPoint = 0;
	draggedCircle = 0;
	draggedCtlPt = NULL;
	m_ConicsSelection = ID_CONICS_SET4THPOINT;
	m_DrawMode = ID_CONICS_INTERNALPTP;
	m_CurveOrStringArt = ID_CONICS_CURVE;
	m_ShowAsymptotes = false;

	m_pointCount = 0;
	m_K = -1.0;
	m_Rho = 0.5;
	m_draggedPt = NULL;
}

CMFCKit2004View::~CMFCKit2004View() {
}

BOOL CMFCKit2004View::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return CView::PreCreateWindow(cs);
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
	//if (conics.IsDone() == true)
	//{
	//	conics.CalcK(m_ConicsSelection);
	//	conics.DrawCurve(m_DrawMode,m_WindowWidth+1,m_WindowHeight+1,m_CurveOrStringArt,
	//		m_ShowAsymptotes);
	//}
	drawSegments(GL_RENDER);
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

	return 0;
}

BOOL CMFCKit2004View::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CView::OnEraseBkgnd(pDC);
	return true;
}

void CMFCKit2004View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	// TODO: Add your message handler code here and/or call default
	Invalidate();
//	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CMFCKit2004View::OnLButtonDown(UINT nFlags, CPoint point) {
//	CView::OnLButtonDown(nFlags, point);
	SetCapture();	// capture the mouse 'left button up' command
	prevMouseLocation = point;
	LButtonDown = true;
	findCtlPoint(point.x,point.y);
	findPoint(point.x,point.y);

	// convert point to cagd coordinates
	CCagdPoint temp[2];
	cagdToObject(point.x, point.y, temp);
	temp[0].z=0;

	// YOUR CODE HERE
	if (::GetAsyncKeyState(VK_SHIFT))
	{
		m_draggedViewPoint = temp[0];
		return;
	}
	
	if(CursorIsOnPoint(temp[0]))
	{
		m_draggedPt = GetPointUnderCursor(temp[0]);
		Invalidate();
		return;
	}
	m_draggedPt = NULL;

	//general behavior
	if (m_pointCount < 3) 
	{
		if(0 == m_pointCount)
		{
			m_P1 = temp[0];
			++m_pointCount;
		}
		else if(1 == m_pointCount)
		{
			m_T = temp[0];
			++m_pointCount;
		}
		else if(2 == m_pointCount)
		{
			m_P2 = temp[0];
			++m_pointCount;
		}
	}
	else
	{
		switch(m_ConicsSelection)
		{
		case ID_CONICS_SETPRODUCT:
			{
				m_pointCount = 0;
			}
			break;
		case ID_CONICS_SETRHO:
			{
				m_pointCount = 0;
			}
			break;
		case ID_CONICS_SET4THPOINT:
			{ 
				if(3 == m_pointCount)
				{
					m_P5 = temp[0];
					++m_pointCount;
				}
				else
				{
					m_pointCount = 0;	
				}
			}
			break;
		case ID_CONICS_SETTANGENT:
			{
				if(3 == m_pointCount)
				{
					m_tangentP1 = temp[0];
					++m_pointCount;
				}
				else if(4 == m_pointCount)
				{
					m_tangentP2 = temp[0];
					++m_pointCount;
				}
				else
				{
					m_pointCount = 0;
				}
			}
			break;
		}
	}
	RenderCurve();
	Invalidate();
}

void CMFCKit2004View::OnLButtonUp(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() == this) ::ReleaseCapture();
	draggedPolyline = draggedCircle = 0;
	draggedPolyPoint = -1;
	draggedPoint = 0;
	draggedCtlPt = NULL;
	LButtonDown = false;
	// YOUR CODE HERE ?
	m_draggedPt = NULL;
}

void CMFCKit2004View::OnRButtonDown(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	SetCapture();	// capture the mouse 'right button up' command
	prevMouseLocation = point;
	RButtonDown = true;
	// YOUR CODE HERE ?
}

void CMFCKit2004View::OnRButtonUp(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() == this) ::ReleaseCapture();
	RButtonDown = false;
	if (draggedCircle != 0)
		return;
	CMenu popupMenu;
	popupMenu.LoadMenu(IDR_POPUPMENU); // here you can choose which menu will be shown...
	CMenu* subMenu = popupMenu.GetSubMenu(0);
	ClientToScreen(&point);
	subMenu->TrackPopupMenu(0, point.x, point.y,
	   AfxGetMainWnd(), NULL);

	// YOUR CODE HERE
	// of course you dont HAVE to have a menu if you dont want to.
	// in that case just remove the last 6 lines.

}

void CMFCKit2004View::OnMouseMove(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() != this) 
	{	
		return;
	}
	if (::GetAsyncKeyState(VK_SHIFT))
	{
		CCagdPoint p[2];
		cagdToObject(point.x,point.y,p);
		CCagdPoint diff = CCagdPoint(p[0].x - m_draggedViewPoint.x, p[0].y - m_draggedViewPoint.y, 0);
		cagdTranslate(diff.x, diff.y, 0);
		//translateXY(diff.x, diff.y);
	}
	if (draggedPolyline != 0 && draggedPolyPoint != -1 && LButtonDown) { // move a control point
		CCagdPoint p[2];
		cagdToObject(point.x,point.y,p);
		p[0].z = p[1].z = list[draggedPolyline].where[draggedPolyPoint].z;
		cagdSetVertex(draggedPolyline,draggedPolyPoint,p);
		if (cagdGetCircleOfCtlPt(draggedPolyline,draggedPolyPoint) != 0) {
			UINT circleId = cagdGetCircleOfCtlPt(draggedPolyline, draggedPolyPoint);
			cagdChangeCircleCenter(circleId, p[0]);
		}
		if(NULL != m_draggedPt)
		{
			*m_draggedPt = p[0];
		}
		RenderCurve();
		Invalidate();
		return;
	}
	if (draggedPoint != 0 && LButtonDown) { // move control point
		CCagdPoint p[2];
		cagdToObject(point.x,point.y,p);
		//p[0].z = p[1].z = list[draggedPoint].z;
		//cagdSetVertex(draggedPolyline,draggedPolyPoint,p);
		list[draggedPoint].where[0].x = p[0].x;
		list[draggedPoint].where[0].y = p[0].y;
		if(NULL != m_draggedPt)
		{
			*m_draggedPt = p[0];
		}
		RenderCurve();
		Invalidate();
		return;
	}
	//if (m_Mode == ID_MODE_VIEW && LButtonDown) { // rotation / scaling / translation
	//	// This is the movement ammount 
	//	int valuex(point.x - prevMouseLocation.x);
	//	int valuey(point.y - prevMouseLocation.y);
	//	if (!CtrlKeyDown) {		
	//		if (RButtonDown && LButtonDown)  // translateXY
	//			translateXY(valuex*TSense,-valuey*TSense);
	//		else if (LButtonDown) // rotateXY
	//			RotateXY(valuey*RSense,valuex*RSense);
	//		else if (RButtonDown) // rotateZ
	//			RotateZ(valuex);
	//	}
	//}
	Invalidate();					// redraw scene
	prevMouseLocation = point;
	
}

BOOL CMFCKit2004View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	// TODO: Add your message handler code here and/or call default
	if (m_Mode == ID_MODE_VIEW) {
		Scale(1 + double(zDelta)/300);
		RenderCurve();
		Invalidate();
		return true;
	}
	return true;
}

// this is the method called when SetTimer is called.  see OnDemoAnimation() further down
void CMFCKit2004View::OnTimer(UINT nIDEvent) { 
	// TODO: Add your message handler code here and/or call default
	// a simple rotation:
	RotateXY(5,5);	
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
			draggedPolyPoint = cagdGetNearestVertex(id,x,y)-1;
			return &list[id].where[draggedPolyPoint];
		}
	}
	return NULL;
}

CCagdPoint *CMFCKit2004View::findPoint(int x, int y) {
	UINT id;
	for (cagdPick(x,y);id = cagdPickNext();) {
		if (cagdGetSegmentType(id) == CAGD_SEGMENT_POINT) {
			draggedPoint = id;
			return &list[id].where[0];
		}
	}
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


CCagdPoint CMFCKit2004View::gamma(double t, double a, double b, double w1, double w2, double w) const
{
	double coeff1 = w1*(b-t)*(b-t);
	CCagdPoint N1 = cagdMultiply(&m_P1, coeff1);

	double coeff2 = 2.0*w*(t-a)*(b-t);
	CCagdPoint N2 = cagdMultiply(&m_T, coeff2);

	double coeff3 = w2*(t-a)*(t-a);
	CCagdPoint N3 = cagdMultiply(&m_P2, coeff3);

	CCagdPoint numerator = cagdAdd(&cagdAdd(&N1, &N2),&N3);
	double denominator = coeff1 + coeff2 + coeff3;
	double multiplier = 1.0 / denominator;
	
	CCagdPoint gamma = cagdMultiply(&numerator, multiplier);
	return gamma;
}

double CMFCKit2004View::lambdaToK(double lambda)
{
	double k = (4.0*lambda) / (1.0 - lambda);
	return k;
}

double CMFCKit2004View::kToLambda(double k)
{
	return k / (4 + k);
}

double CMFCKit2004View::cToLambda(double c)
{
	assert(abs(c + 1) > 0.0001);
	return c / (1 + c);
}

void CMFCKit2004View::RenderCurve()
{
	clear_all();

	// no matter the state, do the same thing for the first 3 points
	if(1 == m_pointCount)
	{
		cagdSetSegmentColor(cagdAddPoint(&m_P1), 200, 0, 0);
	}
	else if(2 == m_pointCount)
	{
		//cagdSetSegmentColor(cagdAddPoint(&m_P1), 255, 0, 0);
		//cagdSetSegmentColor(cagdAddPoint(&m_T), 255, 0, 0);

		// the red lines
		CCagdPoint polyLine[3];
		polyLine[0] = m_P1;
		polyLine[1] = m_T;
		cagdSetSegmentColor(cagdAddPolyline(polyLine,2, CAGD_SEGMENT_CTLPLYGN),200,0,0);
	}
	else if(3 <= m_pointCount)
	{
		// the red lines
		CCagdPoint polyLine[3];
		polyLine[0] = m_P1;
		polyLine[1] = m_T;
		polyLine[2] = m_P2;
		cagdSetSegmentColor(cagdAddPolyline(polyLine,3, CAGD_SEGMENT_CTLPLYGN),200,0,0);
	}


	switch(m_ConicsSelection)
	{
	case ID_CONICS_SETPRODUCT:
		{
			if(3 <= m_pointCount)
			{
				DrawCurveByK();
			}
		}
		break;
	case ID_CONICS_SET4THPOINT:
		{
			if(4 == m_pointCount)
			{
				DrawCurveBy4thPoint();
			}
		}
		break;
	case ID_CONICS_SETTANGENT:
		{
			if(m_pointCount == 4)
			{
				cagdAddPoint(&m_tangentP1);
			}
			if(m_pointCount == 5)
			{
				CCagdPoint A1 = FindCrossingPoint(m_tangentP1, m_tangentP2, m_T, m_P1);
				CCagdPoint A2 = FindCrossingPoint(m_tangentP1, m_tangentP2, m_T, m_P2);
				m_K = KfromPoints(m_T, m_P1, m_P2, A1, A2);
				CCagdPoint tempPt[2];
				tempPt[0] = m_tangentP1;
				tempPt[1] = m_tangentP2;
				cagdSetSegmentColor(cagdAddPolyline(tempPt, 2, CAGD_SEGMENT_CTLPLYGN),255,128,0);
				DrawCurveByK();
			}
		}
		break;
	case ID_CONICS_SETRHO:
			if (m_pointCount == 3)
			{
				CCagdPoint M = CCagdPoint((m_P1.x + m_P2.x)/2, (m_P1.y + m_P2.y)/2, 0);
				CCagdPoint V = CCagdPoint(m_T.x - M.x, m_T.y - M.y, 0);
				CCagdPoint other = CCagdPoint(M.x + (V.x * m_Rho), M.y + (V.y * m_Rho), 0);
				CCagdPoint tempPt[2];
				tempPt[0] = M;
				tempPt[1] = other;
				//cagdSetSegmentColor(cagdAddPolyline(tempPt, 2, CAGD_SEGMENT_CTLPLYGN),0,128,256);
				m_P5 = other;
				DrawCurveBy4thPoint();
			}
			break;
	}
}

void CMFCKit2004View::CalculateCurve()
{
	switch(m_conicSection)
	{
	case ConicSectionEllipse:
		{
			int ptCount = 400;
			double aval = 0;
			double bval = 399;

			double w1 = 1.0;
			double w2 = m_K;
			double w = 1.0;
			int outside = 0, inside = 0;
			for(; inside <= ptCount - 1; inside++)
			{
				m_curvePoints[inside] = gamma(inside, aval, bval, w1, w2, w);
			}
			w = -w;
			for(; outside <= ptCount - 1; outside++)
			{
				m_curvePoints[outside + inside] = gamma(outside, aval, bval, w1, w2, w);
			}
			m_idxEllipseLastInside = inside - 1;
			m_idxEllipseLastOutside = outside + inside - 1;
			break;
		}
	case ConicSectionHyperbola:
		{
			int ptCount = 400;
			double aval = 0;
			double bval = 399;

			double w1 = sqrt(m_K);
			double w2 = sqrt(m_K);
			double w = 1.0;
			int firstVal = 0;
			int outside = 0, inside = 0;
			bool foundAsymptote1 = false;
			// calculate the inside part of the hyperbola
			for(; inside <= ptCount - 1; inside++)
			{
				m_curvePoints[inside] = gamma(inside, aval, bval, w1, w2, w);
			}
			
			w = -w;
			for(; outside <= ptCount  - 1; outside++)
			{
				m_curvePoints[outside + inside] = gamma(outside, aval, bval, w1, w2, w);
				if (outside == 0) continue;
				CCagdPoint Puv = xyToUV(m_P1, m_T, m_P2, m_curvePoints[outside + inside]);
				CCagdPoint PrevUv = xyToUV(m_P1, m_T, m_P2, m_curvePoints[outside + inside - 1]);

				if (abs(Puv.x) > 0.0001 && Puv.x*PrevUv.x < 0 && !foundAsymptote1)
				{
					m_asymptote1[0] = m_curvePoints[outside + inside - 1];
					m_asymptote1[1] = m_curvePoints[outside + inside];
					m_idxHyperbolaBreak1 = inside + outside - 1;
					foundAsymptote1 = true;
				}
				if (abs(Puv.y) > 0.0001 && Puv.y*PrevUv.y < 0 && foundAsymptote1) 
				{
					m_asymptote2[0] = m_curvePoints[outside + inside - 1];
					m_asymptote2[1] = m_curvePoints[outside + inside];
					m_idxHyperbolaBreak2 = inside + outside - 1;
				}
			}
			m_idxHyperbolaLastInside = inside - 1;
			m_idxHyperbolaLastOutside = outside + inside - 1;

			// get crossing point
			m_asymptoteCenter = FindCrossingPoint(m_asymptote1[0], m_asymptote1[1], m_asymptote2[0], m_asymptote2[1]);

			break;
		}

	case ConicSectionParabola:
		{
			int ptCount = 400;
			double aval = 0;
			double bval = 399;

			double w1 = 1.0;
			double w2 = m_K;
			double w = 1.0;
			int outside = 0, inside = 0;
			for(; inside <= ptCount - 1; inside++)
			{
				m_curvePoints[inside] = gamma(inside, aval, bval, w1, w2, w);
			}
			w = -w;
			CCagdPoint prevUV = xyToUV(m_P1, m_T, m_P2, m_curvePoints[inside - 1]);
			CCagdPoint currUV = CCagdPoint(0,0,0);
			bool foundBreakpoint = false;
			for(; outside <= ptCount - 1; outside++)
			{
				m_curvePoints[outside + inside] = gamma(outside, aval, bval, w1, w2, w);
				currUV = xyToUV(m_P1, m_T, m_P2, m_curvePoints[outside + inside]);

				if((currUV.x - prevUV.x < 0) && !foundBreakpoint) {
					m_idxParabolaBreak = (inside + outside) - 1;
					foundBreakpoint = true;
				}
					
				prevUV = currUV;
			}
			m_idxParabolaLastInside = inside - 1;
			m_idxParabolaLastOutside = outside + inside - 1;
			break;
		}
	default:
		{
			return;
		}
	}
}


void CMFCKit2004View::DrawCurveByK()
{
	double lambda = kToLambda(m_K);
	m_conicSection = ConicSectionFromLambda(lambda);

	CalculateCurve();
	DrawCurveSection(INSIDE, 0, 255, 0);
	//if (m_DrawMode == ID_CONICS_ENTIRECONIC)
	//{
	//	DrawCurveSection(OUTSIDE, 255, 255, 0);
	//}
}

void CMFCKit2004View::DrawCurveBy4thPoint()
{
	double lambda = CalculateLambda(m_P1, m_T, m_P2, m_P5);
	m_K = lambdaToK(lambda);	
	cagdAddPoint(&m_P5);
	DrawCurveByK();
}

void CMFCKit2004View::DrawCurveSection(CurveSection s, BYTE r, BYTE g, BYTE b) const
{
	if (m_conicSection == ConicSectionUnknown)
	{
		return;
	}
	if (m_CurveOrStringArt == ID_CONICS_BOTH || m_CurveOrStringArt == ID_CONICS_CURVE)
	{

		switch (m_conicSection)
		{
		case ConicSectionEllipse:
			{
				cagdSetSegmentColor(cagdAddPolyline(m_curvePoints, m_idxEllipseLastInside + 1, CAGD_SEGMENT_POLYLINE), 0, 255, 0);
			}
			break;
		case ConicSectionParabola:
			{
				cagdSetSegmentColor(cagdAddPolyline(m_curvePoints, m_idxParabolaLastInside + 1, CAGD_SEGMENT_POLYLINE), 0, 255, 0);
			}
			break;
		case ConicSectionHyperbola:
			{
				cagdSetSegmentColor(cagdAddPolyline(m_curvePoints, m_idxHyperbolaLastInside + 1, CAGD_SEGMENT_POLYLINE), 0, 255, 0);
			}
			break;
		}
		if (m_DrawMode == ID_CONICS_ENTIRECONIC)
		{
			switch (m_conicSection)
			{
			case ConicSectionEllipse:
				{
					int ptsToDraw = (m_idxEllipseLastOutside - m_idxEllipseLastInside);
					cagdSetSegmentColor(cagdAddPolyline(m_curvePoints + m_idxEllipseLastInside + 1, ptsToDraw, CAGD_SEGMENT_POLYLINE), 255, 255, 0);
				}
				break;
			case ConicSectionParabola:
				{
					int ptsToDraw = (m_idxParabolaBreak - m_idxParabolaLastInside);
					cagdSetSegmentColor(cagdAddPolyline(m_curvePoints + m_idxParabolaLastInside + 1, ptsToDraw, CAGD_SEGMENT_POLYLINE), 255, 255, 0);
					ptsToDraw = (m_idxParabolaLastOutside - m_idxParabolaBreak);
					cagdSetSegmentColor(cagdAddPolyline(m_curvePoints + m_idxParabolaBreak + 1, ptsToDraw, CAGD_SEGMENT_POLYLINE), 255, 255, 0);
				}
				break;
			case ConicSectionHyperbola:
				{
					int ptsToDraw = (m_idxHyperbolaBreak1 - m_idxHyperbolaLastInside);
					cagdSetSegmentColor(cagdAddPolyline(m_curvePoints + m_idxHyperbolaLastInside + 1, ptsToDraw, CAGD_SEGMENT_POLYLINE), 255, 255, 0);
					ptsToDraw = (m_idxHyperbolaBreak2 - m_idxHyperbolaBreak1);
					cagdSetSegmentColor(cagdAddPolyline(m_curvePoints + m_idxHyperbolaBreak1 + 1, ptsToDraw, CAGD_SEGMENT_POLYLINE), 255, 255, 0);
					ptsToDraw = (m_idxHyperbolaLastOutside - m_idxHyperbolaBreak2);
					cagdSetSegmentColor(cagdAddPolyline(m_curvePoints + m_idxHyperbolaBreak2 + 1, ptsToDraw, CAGD_SEGMENT_POLYLINE), 255, 255, 0);
				}
				break;
			}
		}
	}
	


	if(ConicSectionHyperbola == m_conicSection) 
	{
		// display asymptotes if requested
		if(m_ShowAsymptotes)
		{
			cagdSetSegmentColor(cagdAddPolyline(m_asymptote1, 2, CAGD_SEGMENT_POLYLINE), 128, 128, 128);
			cagdSetSegmentColor(cagdAddPolyline(m_asymptote2, 2, CAGD_SEGMENT_POLYLINE), 128, 128, 128);
		}
	}

	// string art
	if (m_CurveOrStringArt == ID_CONICS_BOTH || m_CurveOrStringArt == ID_CONICS_STRINGART)
	{
		int ptCount = 500;
		CCagdPoint tempPt[2];
		CCagdPoint tempPtNeg[2];
		double lastV = 100;
		double vSpacing;
		// from 0 < u < 20
		for (int i = 0; i < ptCount - 1; i++)
		{
			double u = (double)(i + 1) / double(20);   

			if (m_DrawMode == ID_CONICS_INTERNALPTP && u > 1)
			{
				return;
			}

			if (abs(u - 1) < 0.0001)
			{
				continue;
			}
			double r1 = u/ (1 - u);
			double r2 = m_K / r1;
			double v = r2 / (1 + r2);


			CCagdPoint Pu = CCagdPoint(u, 0, 0);
			CCagdPoint Pv = CCagdPoint(0, v, 0);

			tempPt[0] = uvToXY(m_P1, m_T, m_P2, Pu);
			tempPt[1] = uvToXY(m_P1, m_T, m_P2, Pv);
			
			if (v < 0) 
			{
				// invert the difference vector
				CCagdPoint diff(tempPt[1].x-tempPt[0].x, tempPt[1].y-tempPt[0].y, 0.0);
				diff.x *= -1.0;
				diff.y *= -1.0;
				tempPt[1].x = tempPt[0].x + (diff.x*100.0);
				tempPt[1].y = tempPt[0].y + (diff.y*100.0);
			}
			if (u < 0)
			{
				CCagdPoint diff(tempPt[0].x-tempPt[1].x, tempPt[0].y-tempPt[1].y, 0.0);
				diff.x *= -1.0;
				diff.y *= -1.0;
				tempPt[0].x = tempPt[1].x + (diff.x*100.0);
				tempPt[0].y = tempPt[1].y + (diff.y*100.0);
			}

			tempPtNeg[0].x = m_asymptoteCenter.x + (m_asymptoteCenter.x - tempPt[0].x);
			tempPtNeg[0].y = m_asymptoteCenter.y + (m_asymptoteCenter.y - tempPt[0].y);
			tempPtNeg[1].x = m_asymptoteCenter.x + (m_asymptoteCenter.x - tempPt[1].x);
			tempPtNeg[1].y = m_asymptoteCenter.y + (m_asymptoteCenter.y - tempPt[1].y);

			cagdSetSegmentColor(cagdAddPolyline(tempPt, 2, CAGD_SEGMENT_POLYLINE), 0, 128, 128);
			if(ConicSectionHyperbola == m_conicSection && m_DrawMode == ID_CONICS_ENTIRECONIC)
			{
				cagdSetSegmentColor(cagdAddPolyline(tempPtNeg, 2, CAGD_SEGMENT_POLYLINE), 0, 128, 128);
			}
			vSpacing = lastV - v;
			lastV = v;
			if (u > 1 && abs(vSpacing) < 0.01) {
				break;
			}
		}

		int iterationCount = (lastV - 1.001) / vSpacing;
		for (int i = 0; i < iterationCount; i++)
		{
			double v = lastV - i*vSpacing;
			if (abs(v - 1) < 0.0001)
			{
				continue;
			}
			double r1 = v/ (1 - v);
			double r2 = m_K / r1;
			double u = r2 / (1 + r2);

			CCagdPoint Pu = CCagdPoint(u, 0, 0);
			CCagdPoint Pv = CCagdPoint(0, v, 0);

			tempPt[0] = uvToXY(m_P1, m_T, m_P2, Pu);
			tempPt[1] = uvToXY(m_P1, m_T, m_P2, Pv);
			
			// and negative ones
			CCagdPoint nPu = CCagdPoint(-u, 0, 0);
			CCagdPoint nPv = CCagdPoint(0, -v, 0);

			if (v < 0) 
			{
				// invert the difference vector
				CCagdPoint diff(tempPt[1].x-tempPt[0].x, tempPt[1].y-tempPt[0].y, 0.0);
				diff.x *= -1.0;
				diff.y *= -1.0;
				tempPt[1].x = tempPt[0].x + (diff.x*100.0);
				tempPt[1].y = tempPt[0].y + (diff.y*100.0);
			}
			if (u < 0)
			{
				CCagdPoint diff(tempPt[0].x-tempPt[1].x, tempPt[0].y-tempPt[1].y, 0.0);
				diff.x *= -1.0;
				diff.y *= -1.0;
				tempPt[0].x = tempPt[1].x + (diff.x*100.0);
				tempPt[0].y = tempPt[1].y + (diff.y*100.0);
			}

			tempPtNeg[0].x = m_asymptoteCenter.x + (m_asymptoteCenter.x - tempPt[0].x);
			tempPtNeg[0].y = m_asymptoteCenter.y + (m_asymptoteCenter.y - tempPt[0].y);
			tempPtNeg[1].x = m_asymptoteCenter.x + (m_asymptoteCenter.x - tempPt[1].x);
			tempPtNeg[1].y = m_asymptoteCenter.y + (m_asymptoteCenter.y - tempPt[1].y);

			cagdSetSegmentColor(cagdAddPolyline(tempPt, 2, CAGD_SEGMENT_POLYLINE), 0, 128, 128);
			if(ConicSectionHyperbola == m_conicSection) 
			{
				cagdSetSegmentColor(cagdAddPolyline(tempPtNeg, 2, CAGD_SEGMENT_POLYLINE), 0, 128, 128);
			}
		}
	}
}
		

CCagdPoint CMFCKit2004View::FindCrossingPoint(const CCagdPoint& l1p1, 
											const CCagdPoint& l1p2,
											const CCagdPoint& l2p1,
											const CCagdPoint& l2p2)
{
	double l1_slope = (l1p2.y - l1p1.y) / (l1p2.x - l1p1.x);
	double l2_slope = (l2p2.y - l2p1.y) / (l2p2.x - l2p1.x);
	double x = (l1_slope * l1p2.x - l2_slope * l2p2.x + l2p2.y - l1p2.y) / (l1_slope - l2_slope);
	double y = l1_slope * x - l1_slope * l1p2.x + l1p2.y;

	return CCagdPoint(x,y,0);
}

double CMFCKit2004View::KfromPoints(const CCagdPoint& T, 
	const CCagdPoint& P1,
	const CCagdPoint& P2,
	const CCagdPoint& A1,
	const CCagdPoint& A2)
{
	//double ratio1 = Length(T, A1) / Length(A1, P1);
	//double ratio2 = Length(T, A2) / Length(A2, P2);
	double ratio1 = colinearVectorRatio(T, A1, P1, A1);
	double ratio2 = colinearVectorRatio(T, A2, P2, A2);
	return ratio1 * ratio2;
}

double CMFCKit2004View::Length(const CCagdPoint& pt1, const CCagdPoint& pt2)
{
	return sqrt( ((pt2.x-pt1.x)*(pt2.x-pt1.x)) + ((pt2.y-pt1.y)*(pt2.y-pt1.y)));
}

double CMFCKit2004View::colinearVectorRatio(const CCagdPoint& v1p1,
											const CCagdPoint& v1p2,
											const CCagdPoint& v2p1,
											const CCagdPoint& v2p2)
{
	return (v1p1.x - v1p2.x)/(v2p1.x - v2p2.x);	
}

bool CMFCKit2004View::CursorIsOnPoint(const CCagdPoint& cursorpt)
{
	CCagdPoint* ptUnderCursor = GetPointUnderCursor(cursorpt);
	return (NULL != ptUnderCursor);
}

CCagdPoint* CMFCKit2004View::GetPointUnderCursor(const CCagdPoint& cursorpt)
{
	const double thresh_dist = 8.0;
	switch(m_pointCount)
	{
	case 5:
		if(thresh_dist >= Length(cursorpt, m_tangentP2)) 
		{ return &m_tangentP2; }
	case 4:
		if(ID_CONICS_SET4THPOINT == m_ConicsSelection)
		{
			if(thresh_dist >= Length(cursorpt, m_P5))
			{ return &m_P5; }
		}
		else if(ID_CONICS_SETTANGENT == m_ConicsSelection)
		{
			if(thresh_dist >= Length(cursorpt, m_tangentP1))
			{ return &m_tangentP1; }
		}
	case 3:
		if(thresh_dist >= Length(cursorpt, m_P1))
		{ return &m_P1; }
		if(thresh_dist >= Length(cursorpt, m_P2))
		{ return &m_P2; }
		if(thresh_dist >= Length(cursorpt, m_T))
		{ return &m_T; }
	}

	return NULL;
}

CMFCKit2004View::ConicSection CMFCKit2004View::ConicSectionFromLambda(double lambda)
{
	if(lambda < 0) 
	{
		return ConicSectionUnknown;
	}	
	else if(lambda < 0.1999) 
	{
		return ConicSectionHyperbola;
	} 
	else if(abs(lambda-0.2) < 0.0001)
	{
		return ConicSectionParabola;
	}
	else if(lambda < 1.0)
	{
		return ConicSectionEllipse;
	}
	else 
	{
		return ConicSectionUnknown;
	}
}

double CMFCKit2004View::CalculateC(const CCagdPoint& P1, 
								   const CCagdPoint& T, 
								   const CCagdPoint& P2, 
								   const CCagdPoint& P5)
{
	return 0.5;
}

CCagdPoint CMFCKit2004View::xyToUV(const CCagdPoint& P1, 
				const CCagdPoint& T, 
				const CCagdPoint& P2, 
				const CCagdPoint& P5)
{
	double u, v, Tx, Ty, P1x, P1y, P2x, P2y, x0, y0;
	P1x = P1.x;
	P1y = P1.y;
	Tx = T.x;
	Ty = T.y;
	P2x = P2.x;
	P2y = P2.y;
	x0 = P5.x;
	y0 = P5.y;
	u = -(P2y*Tx - P2x*Ty - P2y*x0 + P2x*y0 + Ty*x0 - Tx*y0)/(P1y*Tx - P2y*Tx - P1x*Ty + P2x*Ty + P1x*P2y - P2x*P1y);
	v = (P1y*Tx - P1x*Ty - P1y*x0 + P1x*y0 + Ty*x0 - Tx*y0)/(P1y*Tx - P2y*Tx - P1x*Ty + P2x*Ty + P1x*P2y - P2x*P1y);
	return CCagdPoint(u,v,0);
}

CCagdPoint CMFCKit2004View::uvToXY(const CCagdPoint& P1, 
				const CCagdPoint& T, 
				const CCagdPoint& P2, 
				const CCagdPoint& Puv)
{
	double u, v, Tx, Ty, P1x, P1y, P2x, P2y, x0, y0;
	P1x = P1.x;
	P1y = P1.y;
	Tx = T.x;
	Ty = T.y;
	P2x = P2.x;
	P2y = P2.y;
	u = Puv.x;
	v = Puv.y;

	x0 = Tx + u * (P1x - Tx) + v * (P2x - Tx);
	y0 = Ty + u * (P1y - Ty) + v * (P2y - Ty);
	return CCagdPoint(x0, y0, 0);
}

double CMFCKit2004View::CalculateLambda(const CCagdPoint& P1, 
								   const CCagdPoint& T, 
								   const CCagdPoint& P2, 
								   const CCagdPoint& P5)
{
	
	double val = 0.0;
	CCagdPoint P5uv = xyToUV(P1, T, P2, P5);
	double u = P5uv.x;
	double v = P5uv.y;
	val = (u*v)/ (  u * v + (u + v -1)*(u + v - 1)  );
	return val;
}


void CMFCKit2004View::OnModeView() {
	m_Mode = ID_MODE_VIEW;
}

void CMFCKit2004View::OnModeControl() {
	m_Mode = ID_MODE_CONTROL;
}

void CMFCKit2004View::OnUpdateModeControl(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_Mode == ID_MODE_CONTROL);
}

void CMFCKit2004View::OnUpdateModeView(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_Mode == ID_MODE_VIEW);
}

void CMFCKit2004View::OnStuff2() {
	MessageBox("something something","user error");
}

void CMFCKit2004View::OnUpdateStuff2(CCmdUI* pCmdUI)  {
	pCmdUI->Enable(demoMode != ID_DEMO_NONE);
}

void CMFCKit2004View::OnDemoPolyline() {
	demoMode = ID_DEMO_POLYLINE;
	KillTimer(1);
}

void CMFCKit2004View::OnUpdateDemoPolyline(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(demoMode == ID_DEMO_POLYLINE);
}

void CMFCKit2004View::OnDemoAnimation() {
	demoMode = ID_DEMO_ANIMATION;
	SetTimer(1,200,0);
}

void CMFCKit2004View::OnUpdateDemoAnimation(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(demoMode == ID_DEMO_ANIMATION);
}

void CMFCKit2004View::OnDemoNone() {
	demoMode = ID_DEMO_NONE;
	KillTimer(1);
}

void CMFCKit2004View::OnUpdateDemoNone(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(demoMode == ID_DEMO_NONE);
}

void CMFCKit2004View::OnStuff3() { // generate random polyline
	CCagdPoint points[6];
	srand((unsigned)time(NULL));
	for (int i=0;i<6;i++) {
		points[i].x = 10*i + rand()%10;
		points[i].y = 10*i + rand()%10;
		points[i].z = 10*i + rand()%10;
	}
	cagdSetSegmentColor(cagdAddPolyline(points,6, CAGD_SEGMENT_CTLPLYGN),255,0,0);
	Invalidate();
}

void CMFCKit2004View::OnStuff4() { // add control circle
	CCagdPoint points[2];
	CCagdPoint *p = findCtlPoint(prevMouseLocation.x, prevMouseLocation.y);
	if (p == NULL) return;
	points[0].x = p->x;
	points[0].y = p->y;
	points[0].z = p->z;
	points[1].x = 0;
	points[1].y = 0;
	points[1].z = 5;
	p->circle = addCircle(points[0], points[1], p);
	Invalidate();
}

void CMFCKit2004View::OnUpdateStuff4(CCmdUI* pCmdUI) {
}

void CMFCKit2004View::OnStuff5() { // add control point - still a bug here
	cagdPick(prevMouseLocation.x,prevMouseLocation.y);
	UINT id;
	for (;id = cagdPickNext();) {
		if (cagdGetSegmentType(id) == CAGD_SEGMENT_CTLPLYGN) {
			int place;
			int index = cagdGetNearestVertex(id,prevMouseLocation.x,prevMouseLocation.y);
			CCagdPoint loc[2];
			cagdToObject(prevMouseLocation.x,prevMouseLocation.y,loc);
			loc[0].z = 0;
			CPoint clicked(prevMouseLocation.x,prevMouseLocation.y);
			if (index == 0) { 
				place = 0;
			}
			else if (index == int(cagdGetSegmentLength(id))) {
				place = index-1;
			}
			else {
				CCagdPoint back;
				CCagdPoint nearest;
				CCagdPoint forward;
				cagdGetVertex(id,index,&nearest);
				cagdGetVertex(id,index-1,&back);
				cagdGetVertex(id,index+1,&forward);
				int bx,by,fx,fy,nx,ny;
				cagdToWindow(&back,&bx,&by);
				cagdToWindow(&forward,&fx,&fy);
				cagdToWindow(&nearest,&nx,&ny);
				double bsize=sqrt((double)(bx*bx+by*by));
				double fsize=sqrt((double)(fx*fx+fy*fy));
				double vecBackx(bx-nx),vecBacky(by-ny);
				double vecForwardx(fx-nx),vecForwardy(fy-ny);
				double vecClickedx(clicked.x-nx),vecClickedy(clicked.y-ny);
				double clickedSize = sqrt((double)(clicked.x*clicked.x+clicked.y*clicked.y));
				vecBackx /= bsize;
				vecBacky /= bsize;
				vecForwardx /= fsize;
				vecForwardy /= fsize;
				vecClickedx /= clickedSize;
				vecClickedy /= clickedSize; // ahhhhhhhhhhh!!!
				// now all are normalized
				// now for dot products:
				double backVal = vecBackx*vecClickedx + vecBacky*vecClickedy;
				double forwardVal = vecForwardx*vecClickedx + vecForwardy*vecClickedy;
				if (backVal > forwardVal)
					place = index-1;
				else
					place=index;
			}
			cagdCtrlPlygnAddPoint(id,place,loc[0]);
			Invalidate();
		}
	}
}

void CMFCKit2004View::OnUpdateStuff5(CCmdUI* pCmdUI) {
	// TODO: Add your command update UI handler code here
}

void CMFCKit2004View::OnConicsSet4thpoint()
{
	//clear_all();
	m_ConicsSelection = ID_CONICS_SET4THPOINT;
	m_pointCount = min(m_pointCount, 4);
	RenderCurve();
	Invalidate();
}

void CMFCKit2004View::OnUpdateConicsSet4thpoint(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ConicsSelection == ID_CONICS_SET4THPOINT);
}

void CMFCKit2004View::OnConicsSetproduct()
{
	//clear_all();
	m_ConicsSelection = ID_CONICS_SETPRODUCT;
	CProductDialog productDlg;
	productDlg.m_lProductValue = m_K;
	m_pointCount = min(m_pointCount, 3);
	if (productDlg.DoModal() == IDOK) {
		// set new current values
		// YOUR CODE HERE
		m_K = productDlg.m_lProductValue;
	} 
	RenderCurve();

	Invalidate();
}

void CMFCKit2004View::OnUpdateConicsSetproduct(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ConicsSelection == ID_CONICS_SETPRODUCT);
}

void CMFCKit2004View::OnConicsInternalptp()
{
	m_DrawMode = ID_CONICS_INTERNALPTP;
	RenderCurve();
	Invalidate();
}

void CMFCKit2004View::OnUpdateConicsInternalptp(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_DrawMode == ID_CONICS_INTERNALPTP);
}


void CMFCKit2004View::OnConicsEntireconic()
{
	m_DrawMode = ID_CONICS_ENTIRECONIC;
	RenderCurve();
	Invalidate();
}

void CMFCKit2004View::OnUpdateConicsEntireconic(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_DrawMode == ID_CONICS_ENTIRECONIC);
}

void CMFCKit2004View::OnStuffClearall()
{
	clear_all();
	Invalidate();
}

void clear_all()
{
	cagdFreeAllSegments();
	// YOUR CODE HERE
}

void CMFCKit2004View::OnConicsSetrho()
{
	clear_all();
	m_ConicsSelection = ID_CONICS_SETRHO;
	m_pointCount = min(m_pointCount, 3);
	CRhoDialog rhoDlg;
	rhoDlg.m_lRhoValue = m_Rho;
	if (rhoDlg.DoModal() == IDOK) {
		m_Rho = rhoDlg.m_lRhoValue;
	}
	RenderCurve();
	Invalidate();
}

void CMFCKit2004View::OnUpdateConicsSetrho(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ConicsSelection == ID_CONICS_SETRHO);
}

void CMFCKit2004View::OnConicsSettangent()
{
	m_ConicsSelection = ID_CONICS_SETTANGENT;
	m_pointCount = min(m_pointCount, 3);
	RenderCurve();
	Invalidate();
}

void CMFCKit2004View::OnUpdateConicsSettangent(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ConicsSelection == ID_CONICS_SETTANGENT);
}

void CMFCKit2004View::OnConicsCurve()
{
	m_CurveOrStringArt = ID_CONICS_CURVE;
	RenderCurve();
	Invalidate();
}

void CMFCKit2004View::OnUpdateConicsCurve(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_CurveOrStringArt == ID_CONICS_CURVE);
}

void CMFCKit2004View::OnConicsStringart()
{
	m_CurveOrStringArt = ID_CONICS_STRINGART;
	RenderCurve();
	Invalidate();
}

void CMFCKit2004View::OnUpdateConicsStringart(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_CurveOrStringArt == ID_CONICS_STRINGART);
}

void CMFCKit2004View::OnConicsBoth()
{
	m_CurveOrStringArt = ID_CONICS_BOTH;
	RenderCurve();
	Invalidate();
}

void CMFCKit2004View::OnUpdateConicsBoth(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_CurveOrStringArt == ID_CONICS_BOTH);
}

void CMFCKit2004View::OnConicsShowasymptotes()
{
	m_ShowAsymptotes = !m_ShowAsymptotes;
	RenderCurve();
	Invalidate();
}

void CMFCKit2004View::OnUpdateConicsShowasymptotes(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ShowAsymptotes);
}

void CMFCKit2004View::OnHelpUser()
{
	ManualDialog manualDlg;
	manualDlg.DoModal();
}

void CMFCKit2004View::OnViewClearview() {
	clear_all();
	Invalidate();
}

void CMFCKit2004View::OnViewResetcamera() {
	glLoadIdentity();
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);

	Invalidate();
}

void CMFCKit2004View::OnUpdateLambda(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(); 

	//Resize statusbar somehow better
	//pmw->m_wndStatusBar.
	double lambda = kToLambda(m_K);
	CString strPage;
	if(lambda < 0 || lambda > 1) 
	{
		strPage.Format( "Lambda:    ",  lambda);     
	}
	else 
	{
		strPage.Format( "Lambda: %f     ",  lambda); 
	}
    //pCmdUI->SetText( strPage );


	CMainFrame * pmw = (CMainFrame *)AfxGetMainWnd();

	//pmw->m_wndStatusBar.GetPaneInfo(
	//	pmw->m_wndStatusBar.CommandToIndex(ID_LAMBDA_INDICATOR),
	//	nId, nStyle, cxWidth);

	
	pmw->m_wndStatusBar.SetPaneText(
		pmw->m_wndStatusBar.CommandToIndex(ID_LAMBDA_INDICATOR),
		strPage);

	pmw->m_wndStatusBar.SetPaneInfo(
		pmw->m_wndStatusBar.CommandToIndex(ID_LAMBDA_INDICATOR),
		ID_LAMBDA_INDICATOR, 0, 100);

	ConicSection cs = ConicSectionFromLambda(lambda);
	CString strConic = "";
	switch(cs)
	{
	case ConicSectionEllipse:
		{
			strConic = "Ellipse";
			break;	
		}
	case ConicSectionParabola:
		{
			strConic = "Parabola";
			break;	
		}
	case ConicSectionHyperbola:
		{
			strConic = "Hyperbola";
			break;
		}
	default:
		{
			strConic = "";
			break;
		}
	}

	pmw->m_wndStatusBar.SetPaneText(
		pmw->m_wndStatusBar.CommandToIndex(ID_CONIC_SECTION_TYPE),
		strConic);

	pmw->m_wndStatusBar.SetPaneInfo(
		pmw->m_wndStatusBar.CommandToIndex(ID_CONIC_SECTION_TYPE),
		ID_CONIC_SECTION_TYPE, 0, 100);

}