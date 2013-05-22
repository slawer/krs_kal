#if !defined(AFX_DLGEDITBO_H__F66AAB7A_114C_4980_B589_DFA838EBCDDF__INCLUDED_)
#define AFX_DLGEDITBO_H__F66AAB7A_114C_4980_B589_DFA838EBCDDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SERV.h"

class DlgEditBO : public CDialog
{
public:
	DlgEditBO(SERV_BO* bo, SERV_BO*exist, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(DlgEditBO)
	enum { IDD = IDD_DIALOG_EDIT_BO };
	CListBox	m_list_de;
	//}}AFX_DATA

	SERV_BO *m_bo, *m_exist;

	//{{AFX_VIRTUAL(DlgEditBO)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	void RebuildListDE();
	void CheckComplete();

	//{{AFX_MSG(DlgEditBO)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonEdit();
	afx_msg void OnSelchangeListDe();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITBO_H__F66AAB7A_114C_4980_B589_DFA838EBCDDF__INCLUDED_)
