#pragma once
#include "resource.h"
#include <vector>
// CGlobalsDlg dialog

class CGlobalsDlg : public CDialog
{
	DECLARE_DYNAMIC(CGlobalsDlg)

public:
	CGlobalsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGlobalsDlg();

// Dialog Data
	enum { IDD = IDD_SURF_GLOBALS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	int m_orderU;
	int m_orderV;
	int m_samplesPerCurveU;
	int m_samplesPerCurveV;
	int m_isoCurvesU;
	int m_isoCurvesV;
	CString m_strKnotsU;
	CString m_strKnotsV;
	std::vector<double> m_knotsU;
	std::vector<double> m_knotsV;
};
