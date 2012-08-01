#pragma once
#include <vector>
#include "resource.h"

// CInsertKnotDlg dialog

class CInsertKnotDlg : public CDialog
{
	DECLARE_DYNAMIC(CInsertKnotDlg)

public:
	CInsertKnotDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInsertKnotDlg();

// Dialog Data
	enum { IDD = IDD_SURFACES_INSERTKNOTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CString m_strKnots;
	std::vector<double> m_knots;
	double m_newKnotValue;
};
