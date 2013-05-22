#if !defined(AFX_FORMULAEDITDLG_H__1474654D_8081_4286_82DC_550E389C4D10__INCLUDED_)
#define AFX_FORMULAEDITDLG_H__1474654D_8081_4286_82DC_550E389C4D10__INCLUDED_

#include "FORMULA.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class FormulaEditDlg : public CDialog
{
public:
	FormulaEditDlg(FORMULA_Formula* root, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(FormulaEditDlg)
	enum { IDD = IDD_DIALOG_EDIT_FORMULA };
	CListBox	m_list_args;
	CComboBox	m_combo_arg_type;
	//}}AFX_DATA
	FORMULA_Formula* m_root;
	int m_max_layer;

	void OnChooseArgument();
	void CheckComplete();

	//{{AFX_VIRTUAL(FormulaEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	void RebuildFormula(FORMULA_Formula* formula, int &layer);
	void RebuildArgumentsList();

	//{{AFX_MSG(FormulaEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonChoose();
	afx_msg void OnSelchangeComboArgType();
	afx_msg void OnSelchangeListArgs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMULAEDITDLG_H__1474654D_8081_4286_82DC_550E389C4D10__INCLUDED_)
