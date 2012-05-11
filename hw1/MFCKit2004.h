// MFCKit2004.h : main header file for the MFCKIT2004 application
//

#if !defined(AFX_MFCKIT2004_H__3AF3143A_62C1_45F2_8ACA_E6AF0970D3FA__INCLUDED_)
#define AFX_MFCKIT2004_H__3AF3143A_62C1_45F2_8ACA_E6AF0970D3FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMFCKit2004App:
// See MFCKit2004.cpp for the implementation of this class
//

class CMFCKit2004App : public CWinApp
{
public:
	CMFCKit2004App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCKit2004App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMFCKit2004App)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCKIT2004_H__3AF3143A_62C1_45F2_8ACA_E6AF0970D3FA__INCLUDED_)
