// InsertKnotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InsertKnotDlg.h"
#include "afxdialogex.h"
#include <sstream>


// CInsertKnotDlg dialog

IMPLEMENT_DYNAMIC(CInsertKnotDlg, CDialog)

CInsertKnotDlg::CInsertKnotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInsertKnotDlg::IDD, pParent)
	, m_strKnots(_T(""))
	, m_newKnotValue(0)
{

}

CInsertKnotDlg::~CInsertKnotDlg()
{
}

void CInsertKnotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if(!pDX->m_bSaveAndValidate)
	{
		{
			std::stringstream strmU;
			for(int i=0; i < m_knots.size(); ++i)
			{
				strmU << m_knots[i] << " ";
			}
			std::string strKnots = strmU.str();
			m_strKnots = CString((LPCSTR)strKnots.c_str());
		}
	}

	DDX_Text(pDX, IDC_INSERT_KNOTS_U, m_strKnots);

	if(pDX->m_bSaveAndValidate)
	{
		m_knots.clear();
		if(!m_strKnots.IsEmpty())
		{
			std::string strKnots((LPCSTR)m_strKnots);
			std::stringstream strmKnots(strKnots);
			while(!strmKnots.eof())
			{
				double val;
				if(strmKnots >> val)
				{
					m_knots.push_back(val);
				}

			}
		}
	
	}

	DDX_Text(pDX, IDC_INSERT_NEW_KNOT_VALUE, m_newKnotValue);
}

BOOL CInsertKnotDlg::OnInitDialog()
{
	bool res = CDialog::OnInitDialog();
	UpdateData(FALSE);
	return res;
}

BEGIN_MESSAGE_MAP(CInsertKnotDlg, CDialog)
END_MESSAGE_MAP()


// CInsertKnotDlg message handlers
