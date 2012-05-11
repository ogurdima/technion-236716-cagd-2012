#pragma once


// CProductDialog dialog

class CProductDialog : public CDialog
{
	DECLARE_DYNAMIC(CProductDialog)

public:
	CProductDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProductDialog();

// Dialog Data
	enum { IDD = IDD_PRODUCTDIALOG };
	double m_lProductValue;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
