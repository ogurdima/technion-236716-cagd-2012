// ManualDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MFCKit2004.h"
#include "ManualDialog.h"


// ManualDialog dialog

IMPLEMENT_DYNAMIC(ManualDialog, CDialog)
ManualDialog::ManualDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ManualDialog::IDD, pParent)
{
}

ManualDialog::~ManualDialog()
{
}

void ManualDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ManualDialog, CDialog)
END_MESSAGE_MAP()


// ManualDialog message handlers
