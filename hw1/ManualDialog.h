#pragma once


// ManualDialog dialog

class ManualDialog : public CDialog
{
	DECLARE_DYNAMIC(ManualDialog)

public:
	ManualDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~ManualDialog();

// Dialog Data
	enum { IDD = IDD_USERSMANUAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
