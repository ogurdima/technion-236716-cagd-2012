#pragma once


// CRhoDialog dialog

class CRhoDialog : public CDialog
{
	DECLARE_DYNAMIC(CRhoDialog)

public:
	CRhoDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRhoDialog();

// Dialog Data
	enum { IDD = IDD_RHODIALOG };
	double m_lRhoValue;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
