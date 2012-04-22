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
#include <string>

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
	ON_COMMAND(ID_STUFF_2, OnStuff2)
	ON_UPDATE_COMMAND_UI(ID_STUFF_2, OnUpdateStuff2)
	ON_COMMAND(ID_STUFF_3, OnStuff3)
	ON_COMMAND(ID_STUFF_4, OnStuff4)
	ON_UPDATE_COMMAND_UI(ID_STUFF_4, OnUpdateStuff4)
	ON_COMMAND(ID_STUFF_5, OnStuff5)
	ON_UPDATE_COMMAND_UI(ID_STUFF_5, OnUpdateStuff5)
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
	ON_COMMAND(ID_FRENET_DRAWEVOLUTE, OnFrenetDrawevolute)
	ON_UPDATE_COMMAND_UI(ID_FRENET_DRAWEVOLUTE, OnUpdateFrenetDrawevolute)
	ON_COMMAND(ID_FRENET_DRAWOFFSET, OnFrenetDrawoffset)
	ON_UPDATE_COMMAND_UI(ID_FRENET_DRAWOFFSET, OnUpdateFrenetDrawoffset)
	ON_COMMAND(ID_ANIMATION_START, OnAnimationStart)
	ON_COMMAND(ID_ANIMATION_STOP, OnAnimationStop)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004View construction/destruction

CMFCKit2004View::CMFCKit2004View() {
	// TODO: add construction code here
	SSense = RSense = TSense = 0.5;
	RButtonDown = LButtonDown = false;
	CtrlKeyDown = false;
	minT = 0;
	maxT = 1;
	selectedT = -1; // outside of bounds -> nothing drawn
	draggedPolyline = 0;		
	draggedPoint = -1;			
	draggedCircle = 0;			
	draggedCtlPt = NULL;	
	step = 0.01; 
	animSpeed = 50;
	offsetD = 0.5;

	m_curveParamEqn.push_back("0");
	m_curveParamEqn.push_back("0");
	m_curveParamEqn.push_back("0");
	for (int i = 0; i < 3; i++)
	{
		m_curveNodes[i] = NULL;
	}
	m_paramStartVal = 0;
	m_paramEndVal = 0;
	m_curvePts.resize(CURVE_PTS_COUNT);

	m_curveIdx = 0;
}

CMFCKit2004View::~CMFCKit2004View() {
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

//  load a new curve from a file
void CMFCKit2004View::OnFileOpen() {
	// TODO: Add your command handler code here
	CFileDialog fileDialog(TRUE, ".dat", NULL, 0, NULL, NULL, 0);
	INT_PTR nResult = fileDialog.DoModal();
	if(IDCANCEL == nResult) 
	{
		Invalidate();
		return;
	}
	
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

	m_paramStartVal = m_parser.m_paramA;
	m_paramEndVal = m_parser.m_paramB;
	for (int i = 0; i < 3; i++)
	{
		m_curveParamEqn[i] = m_parser.m_equations[i];
		m_curveParamEqn[i][m_curveParamEqn[i].size()-1] = '\0'; //remove Line feed
		if (m_curveNodes[i] != NULL)
		{
			e2t_freetree(m_curveNodes[i]);
		}
		m_curveNodes[i] = e2t_expr2tree(m_curveParamEqn[i].c_str());
	}
	
	DrawCurve();
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
	// TODO: Add your message handler code here and/or call default
	SetCapture();	// capture the mouse 'right button up' command
	prevMouseLocation = point;
	LButtonDown = true;
	findCtlPoint(point.x, point.y);
}

void CMFCKit2004View::OnLButtonUp(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() == this) ::ReleaseCapture();
	LButtonDown = false;
}

void CMFCKit2004View::OnRButtonDown(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	SetCapture();	// capture the mouse 'right button up' command
	prevMouseLocation = point;
	RButtonDown = true;
}

void CMFCKit2004View::OnRButtonUp(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() == this) ::ReleaseCapture();
	RButtonDown = false;
/*
	// this code pops up the menu called "IDR_POPUPMENU" on the screen at coordinate "point"
	CMenu popupMenu;
	popupMenu.LoadMenu(IDR_POPUPMENU); // here you can choose which menu will be shown...
	CMenu* subMenu = popupMenu.GetSubMenu(0);
	ClientToScreen(&point);
	subMenu->TrackPopupMenu(0, point.x, point.y,
	   AfxGetMainWnd(), NULL);
*/
}

void CMFCKit2004View::OnMouseMove(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() == this) {		// 'this' has the mouse capture
		if (LButtonDown) {
			// This is the movement ammount 
			int valuex(point.x - prevMouseLocation.x);
			int valuey(point.y - prevMouseLocation.y);
			if (!CtrlKeyDown) {		
				if (RButtonDown && LButtonDown)  // translateXY
					translateXY(valuex*TSense,-valuey*TSense);
				else if (LButtonDown) // rotateXY
					RotateXY(valuey*RSense,valuex*RSense);
				else if (RButtonDown) // rotateZ
					RotateZ(valuex);
			}
		}
	}
	Invalidate();					// redraw scene
	prevMouseLocation = point;
}


BOOL CMFCKit2004View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	// TODO: Add your message handler code here and/or call default
	UINT state = ::GetAsyncKeyState(VK_CONTROL);
	if(state)
	{
		int newVal;
		if(zDelta>0) 
		{
			newVal = m_curveIdx + 5;
		}
		else
		{
			newVal = m_curveIdx - 5;
		}
		if(newVal < 0) 
		{
			newVal = CURVE_PTS_COUNT-1;
		} 
		else if(newVal > CURVE_PTS_COUNT-1)
		{
			newVal = 0;
		}
		m_curveIdx = newVal;
		DrawCurve();
	}
	else
	{
		Scale(1 + double(zDelta)/300);
	}
	Invalidate();
	return true;
}

void CMFCKit2004View::OnTimer(UINT nIDEvent) {
	// TODO: Add your message handler code here and/or call default
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
}

void CMFCKit2004View::OnFuzzinessLess() {
	fuzziness /= 2;
}
void CMFCKit2004View::OnOptionsReset() {
	cagdReset();
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

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//    these are demo methods!  can be removed!!!
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


void CMFCKit2004View::OnStuff2() {
	MessageBox("something something","user error");
}

void CMFCKit2004View::OnUpdateStuff2(CCmdUI* pCmdUI)  {
}

void CMFCKit2004View::OnStuff3() { // generate random polyline
	CCagdPoint points[6];
	srand((unsigned)time(NULL));
	for (int i=0;i<6;i++) {
		points[i].x = 10*i + rand()%10;
		points[i].y = 10*i + rand()%10;
		points[i].z = 0;//10*i + rand()%10;
	}
	cagdSetSegmentColor(cagdAddPolyline(points,6, CAGD_SEGMENT_CTLPLYGN),255,0,0);
	Invalidate();
}

void CMFCKit2004View::OnStuff4() { // add control circle
	CCagdPoint points[2];
	CCagdPoint *p = findCtlPoint(prevMouseLocation.x, prevMouseLocation.y);
	if (p == NULL)
		return;
	points[0].x = p->x;
	points[0].y = p->y;
	points[0].z = p->z;
	points[1].x = 0;
	points[1].y = 0;
	points[1].z = 5;
	p->circle = addCircle(points[0], points[1], p);
	Invalidate();
}

void CMFCKit2004View::OnUpdateStuff4(CCmdUI* pCmdUI) { // add control point
}

void CMFCKit2004View::OnStuff5() { // add control point - still a bug here
	UINT id;
	for (cagdPick(prevMouseLocation.x,prevMouseLocation.y);id = cagdPickNext();) {
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
				CCagdPoint *back;
				CCagdPoint *nearest;
				CCagdPoint *forward;
				nearest = cagdGetVertex(id,index);
				back = cagdGetVertex(id,index-1);
				forward = cagdGetVertex(id,index+1);
				int bx,by,fx,fy,nx,ny;
				cagdToWindow(back,&bx,&by);
				cagdToWindow(forward,&fx,&fy);
				cagdToWindow(nearest,&nx,&ny);
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
	dlg.m_minT = minT;
	dlg.m_maxT = maxT;
	dlg.m_animSpeed = animSpeed;
	dlg.m_offsetD = offsetD;
	dlg.m_step = step;
	dlg.m_xt = xt;
	dlg.m_yt = yt;
	dlg.m_zt = zt;
	if (dlg.DoModal() == IDOK) {
		minT = dlg.m_minT;
		maxT = dlg.m_maxT;
		step = dlg.m_step;
		animSpeed = dlg.m_animSpeed;
		offsetD = dlg.m_offsetD;
		xt = dlg.m_xt;
		yt = dlg.m_yt;
		zt = dlg.m_zt;
		// do the rest of your stuff here:
	}
	Invalidate();
}

void CMFCKit2004View::OnFrenetShowaxes() {
	// TODO: Add your command handler code here
}

void CMFCKit2004View::OnUpdateFrenetShowaxes(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
}

void CMFCKit2004View::OnFrenetShowfrenetframe() {
	// TODO: Add your command handler code here
}

void CMFCKit2004View::OnUpdateFrenetShowfrenetframe(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
}

void CMFCKit2004View::OnFrenetShowcurvature() {
	// TODO: Add your command handler code here
}

void CMFCKit2004View::OnUpdateFrenetShowcurvature(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
}

void CMFCKit2004View::OnFrenetShowtorsion() {
	// TODO: Add your command handler code here
}

void CMFCKit2004View::OnUpdateFrenetShowtorsion(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
}

void CMFCKit2004View::OnFrenetShow() {
	// TODO: Add your command handler code here
}

void CMFCKit2004View::OnUpdateFrenetShow(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
}

void CMFCKit2004View::OnFrenetDrawevolute() {
	// TODO: Add your command handler code here
}

void CMFCKit2004View::OnUpdateFrenetDrawevolute(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
}

void CMFCKit2004View::OnFrenetDrawoffset() {
	// TODO: Add your command handler code here
}

void CMFCKit2004View::OnUpdateFrenetDrawoffset(CCmdUI *pCmdUI) {
	// TODO: Add your command update UI handler code here
}

void CMFCKit2004View::OnAnimationStart() {
	// TODO: Add your command handler code here
}

void CMFCKit2004View::OnAnimationStop() {
	// TODO: Add your command handler code here
}


void CMFCKit2004View::DrawCurve()
{
	cagdFreeAllSegments();
	int i = 0;
	double step = (m_paramEndVal - m_paramStartVal)/m_curvePts.size();
	double currT = m_paramStartVal;
	for (i = 0; i < m_curvePts.size(); i++, currT+=step)
	{
		e2t_setparamvalue(currT, E2T_PARAM_T);
		double xCoord = e2t_evaltree(m_curveNodes[0]);
		double yCoord = e2t_evaltree(m_curveNodes[1]);
		double zCoord = e2t_evaltree(m_curveNodes[2]);
		m_curvePts[i] = CCagdPoint(xCoord, yCoord, zCoord);
	}

	cagdAddPolyline(&m_curvePts[0], CURVE_PTS_COUNT, CAGD_SEGMENT_POLYLINE);

	// draw vector 
	FrenetFrame ff;
	ff.SetEquations(m_curveNodes[0],m_curveNodes[1], m_curveNodes[2]);
	double tval = m_paramStartVal + (m_curveIdx*step);
	ff.Calculate(tval);
	CCagdPoint ptOnCurve = m_curvePts[m_curveIdx];
	CCagdPoint Tvec[2];
	Tvec[0] = ptOnCurve;
	Tvec[1] = ptOnCurve + (ff.m_T*5.0);

	cagdAddPolyline(Tvec, 2, CAGD_SEGMENT_POLYLINE);

	

}