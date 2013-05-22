#if !defined(AFX_ARGCONSTDLG_H__961F3E7A_E5B6_433E_A017_443319DE7767__INCLUDED_)
#define AFX_ARGCONSTDLG_H__961F3E7A_E5B6_433E_A017_443319DE7767__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArgConstDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ArgConstDlg dialog

class ArgConstDlg : public CDialog
{
// Construction
public:
	ArgConstDlg(float init_value, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ArgConstDlg)
	enum { IDD = IDD_DIALOG_ARG_CONST };
	float	m_constant;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(ArgConstDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(ArgConstDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARGCONSTDLG_H__961F3E7A_E5B6_433E_A017_443319DE7767__INCLUDED_)
