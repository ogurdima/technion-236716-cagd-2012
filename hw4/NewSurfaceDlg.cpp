// NewSurfaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NewSurfaceDlg.h"
#include "afxdialogex.h"


// CNewSurfaceDlg dialog

IMPLEMENT_DYNAMIC(CNewSurfaceDlg, CDialog)

CNewSurfaceDlg::CNewSurfaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewSurfaceDlg::IDD, pParent)
	, m_ctrlPtCountU(3)
	, m_ctrlPtCountV(3)
	, m_orderU(3)
	, m_orderV(3)
{
	m_txtKnotVectorU = "";
	m_txtKnotVectorV = "";
}

CNewSurfaceDlg::~CNewSurfaceDlg()
{
}

void CNewSurfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNewSurfaceDlg, CDialog)
END_MESSAGE_MAP()


// CNewSurfaceDlg message handlers
