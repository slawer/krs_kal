#if !defined(AFX_DLGLISTORDER_H__6C0B3665_D4DD_11D3_B9BF_0040C7952451__INCLUDED_)
#define AFX_DLGLISTORDER_H__6C0B3665_D4DD_11D3_B9BF_0040C7952451__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgListOrder.h : header file
//
class CPanelWnd;
/////////////////////////////////////////////////////////////////////////////
// CDlgListOrder dialog

class CDlgListOrder : public CDialog
{
	// Construction
public:
	CDlgListOrder(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CDlgListOrder)
	enum { IDD = IDD_DIALOG_CHANGEORDERLIST };
	CListBox	m_ListOrder;
	//}}AFX_DATA


protected:
	int m_nIndex;

public:
	CPanelWnd *pPanel;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgListOrder)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgListOrder)
	virtual void OnOK();
	afx_msg void OnSelchangeListOrder();
	afx_msg void OnButtonDown();
	afx_msg void OnButtonTop();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLISTORDER_H__6C0B3665_D4DD_11D3_B9BF_0040C7952451__INCLUDED_)
