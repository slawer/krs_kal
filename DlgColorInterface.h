#pragma once

#include "Indicator.h"

// CDlgColorInterface dialog

class CDlgColorInterface : public CDialog
{
	DECLARE_DYNAMIC(CDlgColorInterface)

public:
	CDlgColorInterface(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgColorInterface();
	void SetColor();

	MColorInterface m_color_interface;

// Dialog Data
	enum { IDD = IDD_DIALOG_COLOR_INTERFACE };
	Indicator	m_ind_color_graph_bk;
	Indicator	m_ind_color_graph_dgt;
	Indicator	m_ind_color_graph_greed;
	Indicator	m_ind_color_panel_grf_bk;
	Indicator	m_ind_color_panel_dgt_bk;
	Indicator	m_ind_color_graph_dgt_bk;
	CComboBox	m_StyleGreedGraph;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedStaticColorFieldgraph();
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnClickedStaticColorGrfDgt();
public:
	afx_msg void OnClickedStaticColorGrfGreed();
public:
	afx_msg void OnClickedStaticColorPanelGrf();
public:
	afx_msg void OnClickedStaticColorPanelDgt();
	afx_msg void OnClickedStaticColorGrfDgtBk();
public:
	afx_msg void OnBnClickedButtonResetDefault();
};
