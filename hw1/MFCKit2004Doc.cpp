// MFCKit2004Doc.cpp : implementation of the CMFCKit2004Doc class
//

#include "stdafx.h"
#include "MFCKit2004.h"

#include "MFCKit2004Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004Doc

IMPLEMENT_DYNCREATE(CMFCKit2004Doc, CDocument)

BEGIN_MESSAGE_MAP(CMFCKit2004Doc, CDocument)
	//{{AFX_MSG_MAP(CMFCKit2004Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004Doc construction/destruction

CMFCKit2004Doc::CMFCKit2004Doc()
{
	// TODO: add one-time construction code here

}

CMFCKit2004Doc::~CMFCKit2004Doc()
{
}

BOOL CMFCKit2004Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004Doc serialization

void CMFCKit2004Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004Doc diagnostics

#ifdef _DEBUG
void CMFCKit2004Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCKit2004Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004Doc commands
