#if !defined(AFX_DLGFORMULASLIST_H__A75F069E_9041_4DC6_A6F0_827A3A362DEB__INCLUDED_)
#define AFX_DLGFORMULASLIST_H__A75F069E_9041_4DC6_A6F0_827A3A362DEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DlgFormulasList : public CDialog
{
public:
	DlgFormulasList(int initial = -1, int layer = 0, CWnd* pParent = NULL);   // standard constructor

	int m_selected_formula_num, m_layer;

	//{{AFX_DATA(DlgFormulasList)
	enum { IDD = IDD_DIALOG_FORMULAS_LIST };
	CListBox	m_list_formulas;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(DlgFormulasList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	int m_init_num;
	//{{AFX_MSG(DlgFormulasList)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFORMULASLIST_H__A75F069E_9041_4DC6_A6F0_827A3A362DEB__INCLUDED_)
