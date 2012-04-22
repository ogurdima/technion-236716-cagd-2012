// PropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCKit2004.h"
#include "PropDlg.h"


// CPropDlg dialog

IMPLEMENT_DYNAMIC(CPropDlg, CDialog)
CPropDlg::CPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropDlg::IDD, pParent)
	, m_xt(_T(""))
	, m_yt(_T(""))
	, m_zt(_T(""))
	, m_minT(0)
	, m_maxT(0)
	, m_step(0)
	, m_offsetD(0)
	, m_animSpeed(0)
{
}

CPropDlg::~CPropDlg()
{
}

void CPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_X_T, m_xt);
	DDV_MaxChars(pDX, m_xt, 100);
	DDX_Text(pDX, IDC_Y_T, m_yt);
	DDV_MaxChars(pDX, m_yt, 100);
	DDX_Text(pDX, IDC_Z_T, m_zt);
	DDV_MaxChars(pDX, m_zt, 100);
	DDX_Text(pDX, IDC_MIN_T, m_minT);
	DDX_Text(pDX, IDC_MAX_T, m_maxT);
	DDX_Text(pDX, IDC_T_STEP, m_step);
	DDV_MinMaxDouble(pDX, m_step, 0.001, 100);
	DDX_Text(pDX, IDC_OFFSET_D, m_offsetD);
	DDX_Text(pDX, IDC_ANIM_SPEED, m_animSpeed);
	DDV_MinMaxInt(pDX, m_animSpeed, 1, 100);
}


BEGIN_MESSAGE_MAP(CPropDlg, CDialog)
END_MESSAGE_MAP()


// CPropDlg message handlers
