#pragma once


// CPropDlg dialog

class CPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CPropDlg)

public:
	CPropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  int m_degree;
  double m_step;
};
