#if !defined(AFX_DLGARGPIECEWISE_H__F2EA6AF1_1249_4488_A243_17A0ED448DC1__INCLUDED_)
#define AFX_DLGARGPIECEWISE_H__F2EA6AF1_1249_4488_A243_17A0ED448DC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FORMULA.h"
#include "PointsList.h"

class DlgArgPiecewise : public CDialog
{
public:
	DlgArgPiecewise(FORMULA_ArgumentPiecewise *arg, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(DlgArgPiecewise)
	enum { IDD = IDD_DIALOG_ARG_PIECEWISE };
	//}}AFX_DATA

    PointsList m_points_list;

	bool GetValue(float &fl, UINT id);
	void CheckComplete();

	FORMULA_ArgumentPiecewise *m_arg;
	FORMULA_PiecewiseTable m_table;
	FORMULA_Formula* m_formula;

	//{{AFX_VIRTUAL(DlgArgPiecewise)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(DlgArgPiecewise)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnButtonEditFormula();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonAdd();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern DlgArgPiecewise* DlgArgPiecewise_dlg;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGARGPIECEWISE_H__F2EA6AF1_1249_4488_A243_17A0ED448DC1__INCLUDED_)
