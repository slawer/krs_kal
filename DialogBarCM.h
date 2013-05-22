#pragma once

#include "DialogBarMain.h"
#include "STAGES.h"
#include "PanelKnopki.h"

// CDialogBarCM

class CDialogBarCM : public CDialogBarMain
{
public:
	CDialogBarCM();
	virtual ~CDialogBarCM();

    //{{AFX_DATA(CDialogBarCM)
    enum { IDD = IDD_DIALOG_BAR_CEMENT };
    CButton	m_button_finish_stage;
    CButton	m_button_restart_stage;
    CButton	m_button_start_pause_stage;
    STAGES_List m_list_selected_stage;
    CPanelKnopki	m_PanelKnopki;
    //}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDialogBarCM)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
    virtual void ResizeOtherBars();

    // Implementation
public:
    void RecalcListWidth();
    void AdjustButtonNames();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateLoad(CCmdUI* pCmdUi);
	afx_msg void OnUpdateUseAsTemplate(CCmdUI* pCmdUi);

	afx_msg void OnUpdateStart(CCmdUI* pCmdUi);
	afx_msg void OnUpdateNext(CCmdUI* pCmdUi);
	afx_msg void OnUpdateFinish(CCmdUI* pCmdUi);
	afx_msg void OnUpdateBreak(CCmdUI* pCmdUi);

	afx_msg void OnUpdateButtonStartPauseStage(CCmdUI* pCmdUi);
	afx_msg void OnUpdateButtonFinishStage(CCmdUI* pCmdUi);
	afx_msg void OnUpdateButtonRestartStage(CCmdUI* pCmdUi);
	afx_msg void OnUpdateMenuBreakFinishAll(CCmdUI* pCmdUi);
    afx_msg void OnButtonStartPauseStage();
    afx_msg void OnButtonFinishStage();
    afx_msg void OnButtonFinishAllStages();
    afx_msg void OnButtonRestartStage();
    afx_msg void OnButtonRestartAllStages();

    afx_msg LRESULT OnMyInitDialog(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
};


