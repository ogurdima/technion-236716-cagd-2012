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
	CString m_xt;
	CString m_yt;
	CString m_zt;
	double m_minT;
	double m_maxT;
	double m_step;
	double m_offsetD;
	int m_animSpeed;
};
