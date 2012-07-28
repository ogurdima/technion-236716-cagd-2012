#pragma once
#include <string>
#include "resource.h"
#include <vector>
// CNewSurfaceDlg dialog

class CNewSurfaceDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewSurfaceDlg)

public:
	CNewSurfaceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewSurfaceDlg();

// Dialog Data
	enum { IDD = IDD_NEWSURFACEDLG };

	int m_ctrlPtCountU;
	int m_ctrlPtCountUSave;

	int m_ctrlPtCountV;
	int m_ctrlPtCountVSave;

	int m_orderU;
	int m_orderUSave;

	int m_orderV;
	int m_orderVSave;

	CString m_txtKnotVectorU;
	CString m_txtKnotVectorUSave;
	std::vector<double> m_knotsU;

	CString m_txtKnotVectorV;
	CString m_txtKnotVectorVSave;
	std::vector<double> m_knotsV;

	void SaveValues()
	{
		m_ctrlPtCountUSave = m_ctrlPtCountU;
		m_ctrlPtCountVSave = m_ctrlPtCountV;
		m_orderUSave = m_orderU;
		m_orderVSave = m_orderV;
		m_txtKnotVectorUSave = m_txtKnotVectorU;
		m_txtKnotVectorVSave = m_txtKnotVectorV;
	}

	void RestoreSavedValues()
	{
		m_ctrlPtCountU = m_ctrlPtCountUSave;
		m_ctrlPtCountV = m_ctrlPtCountVSave;
		m_orderU = m_orderUSave;
		m_orderV = m_orderVSave;
		m_txtKnotVectorU = m_txtKnotVectorUSave;
		m_txtKnotVectorV = m_txtKnotVectorVSave;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSurfdlgImportSurface2();
};
