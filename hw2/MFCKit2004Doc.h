// MFCKit2004Doc.h : interface of the CMFCKit2004Doc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCKIT2004DOC_H__BE2EA5CD_6036_4F8D_9A0C_EFFC85934AAA__INCLUDED_)
#define AFX_MFCKIT2004DOC_H__BE2EA5CD_6036_4F8D_9A0C_EFFC85934AAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMFCKit2004Doc : public CDocument
{
protected: // create from serialization only
	CMFCKit2004Doc();
	DECLARE_DYNCREATE(CMFCKit2004Doc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCKit2004Doc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMFCKit2004Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMFCKit2004Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCKIT2004DOC_H__BE2EA5CD_6036_4F8D_9A0C_EFFC85934AAA__INCLUDED_)
