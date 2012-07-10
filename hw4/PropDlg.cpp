// PropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCKit2004.h"
#include "PropDlg.h"


// CPropDlg dialog

IMPLEMENT_DYNAMIC(CPropDlg, CDialog)
CPropDlg::CPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropDlg::IDD, pParent)
	, m_degree(0)
	, m_step(0.01)
{
}

CPropDlg::~CPropDlg()
{
}

void CPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_X_DEGREE, m_degree);
	DDX_Text(pDX, IDC_X_SAMPLING_STEP, m_step);
}


BEGIN_MESSAGE_MAP(CPropDlg, CDialog)
END_MESSAGE_MAP()


// CPropDlg message handlers
