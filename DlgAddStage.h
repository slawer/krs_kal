#pragma once

#include "STAGES.h"

class DlgAddStage : public CDialog
{
	DECLARE_DYNAMIC(DlgAddStage)

public:
	DlgAddStage(LPARAM lp, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgAddStage();

	enum { IDD = IDD_DIALOG_ADD_STAGE };
	CListBox m_list_stages;

	int m_selected_stage_template;

protected:
	int m_left, m_top;
	bool m_bind_to_mouse;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeListStages();
	afx_msg LRESULT OnEnforceFlose(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};
