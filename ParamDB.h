#if !defined(AFX_PARAMDB_H__00AFDCDF_CFCD_4983_9363_4020D4827CF5__INCLUDED_)
#define AFX_PARAMDB_H__00AFDCDF_CFCD_4983_9363_4020D4827CF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParamDB.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParamDB dialog

class CParamDB : public CDialog
{
// Construction
public:
	CParamDB(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParamDB)
	enum { IDD = IDD_DIALOG_DEF_DB };
	CString	m_NameDB;
	CString	m_NameServerDB;
	CString	m_PasswdDB;
	CString	m_NameUserDB;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamDB)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParamDB)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNewDb();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMDB_H__00AFDCDF_CFCD_4983_9363_4020D4827CF5__INCLUDED_)
