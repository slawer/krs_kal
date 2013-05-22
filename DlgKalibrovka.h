#if !defined(AFX_DLGKALIBROVKA_H__896EB74A_C74C_4557_845F_E9461750B6C6__INCLUDED_)
#define AFX_DLGKALIBROVKA_H__896EB74A_C74C_4557_845F_E9461750B6C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgKalibrovka.h : header file
//
#include "Graph.h"
#include "Indicftor.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgKalibrovka dialog

class CDlgKalibrovka : public CDialog
{
// Construction
public:
	CDlgKalibrovka(CWnd* pParent = NULL);   // standard constructor

	CGraph m_Graph;

// Dialog Data
	//{{AFX_DATA(CDlgKalibrovka)
	enum { IDD = IDD_DIALOG_KALIBROVKA };
	CIndicftor	m_Test;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgKalibrovka)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgKalibrovka)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGKALIBROVKA_H__896EB74A_C74C_4557_845F_E9461750B6C6__INCLUDED_)
