#if !defined(AFX_DLGSELECTMARKERTYPE_H__A8A272E0_5AAB_404E_940E_813473F7E414__INCLUDED_)
#define AFX_DLGSELECTMARKERTYPE_H__A8A272E0_5AAB_404E_940E_813473F7E414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectMarkerType.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgSelectMarkerType dialog

class DlgSelectMarkerType : public CDialog
{
// Construction
public:
	DlgSelectMarkerType(bool for_select = true, CWnd* pParent = NULL);   // standard constructor

	bool m_for_select;
// Dialog Data
	//{{AFX_DATA(DlgSelectMarkerType)
	enum { IDD = IDD_DIALOG_SELECT_MARKER_TYPE };
	CListBox	m_marker_types_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgSelectMarkerType)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgSelectMarkerType)
	afx_msg void OnButtonSelect();
	afx_msg void OnButtonAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListMarkerTypes();
	afx_msg void OnButtonDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTMARKERTYPE_H__A8A272E0_5AAB_404E_940E_813473F7E414__INCLUDED_)
