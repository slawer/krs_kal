#if !defined(AFX_DLGEDITCALCULATION_H__258C30B0_8685_493D_9982_F14EB07F956C__INCLUDED_)
#define AFX_DLGEDITCALCULATION_H__258C30B0_8685_493D_9982_F14EB07F956C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "STAGES.h"
#include "FORMULA.h"
#include "Indicator.h"

class DlgEditCalculation : public CDialog
{
public:
	DlgEditCalculation(STAGES_CalculationTemplate* ct, CWnd* pParent = NULL);
	~DlgEditCalculation();

	void SetupForCalculation(STAGES_CalculationTemplate* ct);

	//{{AFX_DATA(DlgEditCalculation)
	enum { IDD = IDD_DIALOG_EDIT_CALCULATION };
	Indicator	m_ind_color;	
	//}}AFX_DATA

	//{{AFX_VIRTUAL(DlgEditCalculation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	STAGES_CalculationTemplate* m_calc_template;
	// копии всех данных >>
	CString m_name;
	FORMULA_Formula* m_formula;
	bool m_has_graph, m_show_column[6];
	byte m_auto_graph_mode;
	float m_preset_value, m_min_graph, m_max_graph;
	int m_line_width, m_param_num;
	DWORD m_line_color;
	double m_db_time_interval, m_db_data_interval;
	CParam* m_param;
	// копии всех данных <<

	void OutputThick();
	void UpdateBlockDB();
	
	//{{AFX_MSG(DlgEditCalculation)
	afx_msg void OnButtonChoose();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckGraph();
	afx_msg void OnBnClickedCheckPreset();
	afx_msg void OnBnClickedCheckMax();
	afx_msg void OnBnClickedCheckMin();
	afx_msg void OnBnClickedCheckMid();
	afx_msg void OnBnClickedButtonPlus();
	afx_msg void OnBnClickedButtonMinus();
	afx_msg void OnStnClickedStaticColor();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITCALCULATION_H__258C30B0_8685_493D_9982_F14EB07F956C__INCLUDED_)
