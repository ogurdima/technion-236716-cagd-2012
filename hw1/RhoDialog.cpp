// RhoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MFCKit2004.h"
#include "RhoDialog.h"


// CRhoDialog dialog

IMPLEMENT_DYNAMIC(CRhoDialog, CDialog)
CRhoDialog::CRhoDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRhoDialog::IDD, pParent)
{
}

CRhoDialog::~CRhoDialog()
{
}

void CRhoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RHO, m_lRhoValue);
}


BEGIN_MESSAGE_MAP(CRhoDialog, CDialog)
END_MESSAGE_MAP()


// CRhoDialog message handlers
