#if !defined(AFX_EDITCLR_H__05F128A5_C290_11D3_B95B_0040C7952451__INCLUDED_)
#define AFX_EDITCLR_H__05F128A5_C290_11D3_B95B_0040C7952451__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditClr.h : header file
//

#define ID_EDITCLR	200


/////////////////////////////////////////////////////////////////////////////
// CEditClr window

class CEditClr : public CStatic
{
// Construction
public:
	CEditClr();

// Attributes
public:

	LPVOID m_pParam;

// Operations
public:
	LOGFONT m_logfontN;
	CFont	fFontName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditClr)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditClr();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditClr)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CItemClr window

class CItemClr : public CStatic
{
// Construction
public:
	CItemClr();

// Attributes
public:
	LPVOID m_pParam;

	CEditClr m_eEdit;
	CString  m_strName;
	CString  m_strValue;

	LOGFONT m_logfontN;
	CFont	fFontName;

// Operations
public:
	void SetParam(LPVOID pParam);	
	void UpdateDataParam();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemClr)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CItemClr();

	// Generated message map functions
protected:
	//{{AFX_MSG(CItemClr)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CEditClr2 window

class CEditClr2 : public CStatic
{
// Construction
public:
	CEditClr2();

// Attributes
public:
	LPVOID m_pParam;

// Operations
public:
	LOGFONT m_logfontN;
	CFont	fFontName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditClr2)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditClr2();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditClr2)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CItemClr2 window

class CItemClr2 : public CStatic
{
// Construction
public:
	CItemClr2();

// Attributes
public:
	LPVOID m_pParam;
	int m_numList;

	CEditClr2 m_eEdit;
	CString  m_strName;
	CString  m_strValue;

	LOGFONT m_logfontN;
	CFont	fFontName;

// Operations
public:
	void SetParam(LPVOID pParam, int numList);	
	void UpdateDataParam();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemClr2)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CItemClr2();

	// Generated message map functions
protected:
	//{{AFX_MSG(CItemClr2)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITCLR_H__05F128A5_C290_11D3_B95B_0040C7952451__INCLUDED_)
