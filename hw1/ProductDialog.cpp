// ProductDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MFCKit2004.h"
#include "ProductDialog.h"


// CProductDialog dialog

IMPLEMENT_DYNAMIC(CProductDialog, CDialog)
CProductDialog::CProductDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CProductDialog::IDD, pParent)
{
}

CProductDialog::~CProductDialog()
{
}

void CProductDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PRODUCT, m_lProductValue);
}


BEGIN_MESSAGE_MAP(CProductDialog, CDialog)
END_MESSAGE_MAP()


// CProductDialog message handlers
