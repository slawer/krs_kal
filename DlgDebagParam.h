#if !defined(AFX_DLGDEBAGPARAM_H__403E57E1_E5AD_4192_BC5A_4CCD721E6208__INCLUDED_)
#define AFX_DLGDEBAGPARAM_H__403E57E1_E5AD_4192_BC5A_4CCD721E6208__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDebagParam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDebagParam dialog

class CDlgDebagParam : public CDialog
{
// Construction
public:
	CDlgDebagParam(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDebagParam)
	enum { IDD = IDD_DIALOG_DEBUG };
	double	m_MexInterval;
	double	m_SPOInterval;
	double	m_PorogRotorBur;
	double	m_VesRotorBur;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDebagParam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDebagParam)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDEBAGPARAM_H__403E57E1_E5AD_4192_BC5A_4CCD721E6208__INCLUDED_)
