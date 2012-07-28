// NewSurfaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NewSurfaceDlg.h"
#include "afxdialogex.h"
#include "SurfaceFileParser.h"
#include <string>
#include <sstream>
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
	DDX_Text(pDX, IDC_SURFDLG_CTRLPTS_U, m_ctrlPtCountU);
	DDX_Text(pDX, IDC_SURFDLG_CTRLPTS_V, m_ctrlPtCountV);
	DDX_Text(pDX, IDC_SURFDLG_ORDER_U, m_orderU);
	DDX_Text(pDX, IDC_SURFDLG_ORDER_V, m_orderV);
	DDX_Text(pDX, IDC_SURFDLG_KVTXTU, m_txtKnotVectorU);
	DDX_Text(pDX, IDC_SURFDLG_KVTXTV, m_txtKnotVectorV);

	if(pDX->m_bSaveAndValidate)
	{
		m_knotsU.clear();
		if(!m_txtKnotVectorU.IsEmpty())
		{
			std::string strKnotsU((LPCSTR)m_txtKnotVectorU);
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
		if(!m_txtKnotVectorV.IsEmpty())
		{
			std::string strKnotsV((LPCSTR)m_txtKnotVectorV);
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


BEGIN_MESSAGE_MAP(CNewSurfaceDlg, CDialog)
	ON_BN_CLICKED(IDC_SURFDLG_IMPORT_SURFACE2, &CNewSurfaceDlg::OnBnClickedSurfdlgImportSurface2)
END_MESSAGE_MAP()


// CNewSurfaceDlg message handlers


void CNewSurfaceDlg::OnBnClickedSurfdlgImportSurface2()
{
	CFileDialog fileDialog(TRUE, ".dat", NULL, 0, NULL, NULL, 0);
	INT_PTR nResult = fileDialog.DoModal();
	if(IDCANCEL == nResult) 
	{
		Invalidate();
		return;
	}


	SurfaceFileParser parser;


	CString pathName = fileDialog.GetPathName();
	CString fileName = fileDialog.GetFileTitle();
	std::string filename = std::string(pathName);
	bool parseRes = false;
	parseRes = parser.parse_file(filename);

	m_ctrlPtCountU = parser.m_temp_surf.m_points[0].size();
	m_ctrlPtCountV = parser.m_temp_surf.m_points.size();
	m_orderU = parser.m_temp_surf.m_order.m_u;
	m_orderV = parser.m_temp_surf.m_order.m_v;

	{
		std::stringstream strmU;
		for(int i=0; i < parser.m_temp_surf.m_knots.m_u.size(); ++i)
		{
			strmU << parser.m_temp_surf.m_knots.m_u[i] << " ";
		}
		std::string strKnotsU = strmU.str();
		m_txtKnotVectorU = CString((LPCSTR)strKnotsU.c_str());
	}

	{
		std::stringstream strmV;
		for(int i=0; i < parser.m_temp_surf.m_knots.m_v.size(); ++i)
		{
			strmV << parser.m_temp_surf.m_knots.m_v[i] << " ";
		}
		std::string strKnotsV = strmV.str();
		m_txtKnotVectorV = CString((LPCSTR)strKnotsV.c_str());
	}

	UpdateData(FALSE);
}
