#if !defined(AFX_DLGEDITDE_H__7FFF12CD_23CD_4CAB_AE97_D2C6D0BB710D__INCLUDED_)
#define AFX_DLGEDITDE_H__7FFF12CD_23CD_4CAB_AE97_D2C6D0BB710D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SERV.h"

class DlgEditDE : public CDialog
{
public:
	DlgEditDE(SERV_DE* de, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(DlgEditDE)
	enum { IDD = IDD_DIALOG_EDIT_DE };
	CComboBox	m_combo_length;
	//}}AFX_DATA

	SERV_DE* m_de;
	FORMULA_Formula* m_formula;

	//{{AFX_VIRTUAL(DlgEditDE)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	void CheckComplete();
	void OutputFormiula();

	//{{AFX_MSG(DlgEditDE)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonEdit();
	virtual void OnOK();
	afx_msg void OnCheckUstavka();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITDE_H__7FFF12CD_23CD_4CAB_AE97_D2C6D0BB710D__INCLUDED_)
