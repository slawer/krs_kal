#if !defined(AFX_CHANGEPARAM_H__FDCB8693_6FC4_4495_A5E7_98EB0F5520B8__INCLUDED_)
#define AFX_CHANGEPARAM_H__FDCB8693_6FC4_4495_A5E7_98EB0F5520B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Indicator.h"

////////////////////////////////////////////////////////////////////////////////
// CChangeParam dialog
////////////////////////////////////////////////////////////////////////////////
class CChangeParam : public CDialog
{
public:
	CChangeParam(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CChangeParam)
	enum { IDD = IDD_DIALOG_CHANGE_PARAM };
	Indicator	m_ind_color;
	float	m_min_val_gr;
	float	m_max_val_gr;
	float m_min_val;
	float m_max_val;
	float m_block_val;
	float m_crush_val;
	int m_type_of_control;
	CComboBox	m_combo_control_type;
	//}}AFX_DATA

	void OutputThick();

	COLORREF m_line_color;
	int m_line_width;

	int num_cmd, m_num_param;
	CString name_param;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeParam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CChangeParam)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnButtonDeleteParam();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonMinus();
	afx_msg void OnButtonPlus();
	afx_msg void OnStaticColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

////////////////////////////////////////////////////////////////////////////////
// CDlgAddParam dialog
////////////////////////////////////////////////////////////////////////////////
class CDlgAddParam : public CDialog
{
public:
	CDlgAddParam(LPARAM lp, CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DIALOG_ADD_PARAM };
	CListBox	m_ListParam;

	int m_key;
	bool m_show_additional_cement_params;

protected:
	bool m_for_calibration;
	bool m_bind_to_mouse;
	int m_left, m_top;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeListAddParam();
	afx_msg LRESULT OnEnforceFlose(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEPARAM_H__FDCB8693_6FC4_4495_A5E7_98EB0F5520B8__INCLUDED_)
