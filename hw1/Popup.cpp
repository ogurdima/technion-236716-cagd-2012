// Popup.cpp : implementation file
//

#include "stdafx.h"
#include "MFCKit2004.h"
#include "Popup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPopup dialog


CPopup::CPopup(CWnd* pParent /*=NULL*/)
	: CDialog(CPopup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPopup)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPopup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPopup)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPopup, CDialog)
	//{{AFX_MSG_MAP(CPopup)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopup message handlers
