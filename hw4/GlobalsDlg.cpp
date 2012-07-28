// GlobalsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GlobalsDlg.h"
#include "afxdialogex.h"
#include <string>
#include <sstream>


// CGlobalsDlg dialog

IMPLEMENT_DYNAMIC(CGlobalsDlg, CDialog)

CGlobalsDlg::CGlobalsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGlobalsDlg::IDD, pParent)
	, m_orderU(0)
	, m_orderV(0)
	, m_samplesPerCurveU(0)
	, m_samplesPerCurveV(0)
	, m_isoCurvesU(0)
	, m_isoCurvesV(0)
	, m_strKnotsU(_T(""))
	, m_strKnotsV(_T(""))
{

}

CGlobalsDlg::~CGlobalsDlg()
{
}

void CGlobalsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SURF_GLOBALS_ORDER_U, m_orderU);
	DDX_Text(pDX, IDC_SURF_GLOBALS_ORDER_V, m_orderV);
	DDX_Text(pDX, IDC_SURF_GLOBALS_SAMPLES_U, m_samplesPerCurveU);
	DDX_Text(pDX, IDC_SURF_GLOBALS_SAMPLES_V, m_samplesPerCurveV);
	DDX_Text(pDX, IDC_SURF_GLOBALS_ISOS_U, m_isoCurvesU);
	DDX_Text(pDX, IDC_SURF_GLOBALS_ISOS_V, m_isoCurvesV);

	if(!pDX->m_bSaveAndValidate)
	{
		{
			std::stringstream strmU;
			for(int i=0; i < m_knotsU.size(); ++i)
			{
				strmU << m_knotsU[i] << " ";
			}
			std::string strKnotsU = strmU.str();
			m_strKnotsU = CString((LPCSTR)strKnotsU.c_str());
		}

		{
			std::stringstream strmV;
			for(int i=0; i < m_knotsV.size(); ++i)
			{
				strmV << m_knotsV[i] << " ";
			}
			std::string strKnotsV = strmV.str();
			m_strKnotsV = CString((LPCSTR)strKnotsV.c_str());
		}
	}

	DDX_Text(pDX, IDC_GLOBALS_KNOTSU, m_strKnotsU);
	DDX_Text(pDX, IDC_GLOBALS_KNOTSV, m_strKnotsV);

	if(pDX->m_bSaveAndValidate)
	{
		m_knotsU.clear();
		if(!m_strKnotsU.IsEmpty())
		{
			std::string strKnotsU((LPCSTR)m_strKnotsU);
			std::stringstream strmKnotsU(strKnotsU);
			while(!strmKnotsU.eof())
			{
				double val;
				if(strmKnotsU >> val)
				{
					m_knotsU.push_back(val);
				}

			}
		}

		m_knotsV.clear();
		if(!m_strKnotsV.IsEmpty())
		{
			std::string strKnotsV((LPCSTR)m_strKnotsV);
			std::stringstream strmKnotsV(strKnotsV);
			while(!strmKnotsV.eof())
			{
				double val;
				if(strmKnotsV >> val)
				{
					m_knotsV.push_back(val);
				}
			}
		}		
	}
}

BOOL CGlobalsDlg::OnInitDialog()
{
	bool res = CDialog::OnInitDialog();
	UpdateData(FALSE);
	return res;
}


BEGIN_MESSAGE_MAP(CGlobalsDlg, CDialog)
END_MESSAGE_MAP()


// CGlobalsDlg message handlers
