// DialogBarCM.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"
#include "DialogBarCM.h"
#include "STAGES.h"
#include "CementView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDialogBarCM

CDialogBarCM::CDialogBarCM()
{
}

CDialogBarCM::~CDialogBarCM()
{
}

void CDialogBarCM::DoDataExchange(CDataExchange* pDX)
{
    CDialogBar::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_START_PAUSE_STAGE, m_button_start_pause_stage);
    DDX_Control(pDX, IDC_FINISH_STAGE, m_button_finish_stage);
    DDX_Control(pDX, IDC_RESTART_STAGE, m_button_restart_stage);
    DDX_Control(pDX, IDC_LIST_PHASE, m_list_selected_stage);
    DDX_Control(pDX, IDC_PANEL_KNOPKI, m_PanelKnopki);
}

BEGIN_MESSAGE_MAP(CDialogBarCM, CDialogBar)
	ON_UPDATE_COMMAND_UI(IDC_START_PAUSE_STAGE, OnUpdateButtonStartPauseStage)
	ON_UPDATE_COMMAND_UI(IDC_FINISH_STAGE, OnUpdateButtonFinishStage)
	ON_UPDATE_COMMAND_UI(IDC_RESTART_STAGE, OnUpdateButtonRestartStage)
    ON_BN_CLICKED(IDC_START_PAUSE_STAGE, OnButtonStartPauseStage)
    ON_BN_CLICKED(IDC_FINISH_STAGE, OnButtonFinishStage)
    ON_BN_CLICKED(IDC_RESTART_STAGE, OnButtonRestartStage)
    ON_MESSAGE(WM_INITDIALOG, OnMyInitDialog)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_PAINT()

END_MESSAGE_MAP()

void CDialogBarCM::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	m_list_selected_stage.GetClientRect(&rect);
	if (m_list_selected_stage.m_column_width != m_list_selected_stage.m_list_width)
		rect.right = m_list_selected_stage.m_column_width - 1;
	rect.InflateRect(1, 1);
	m_list_selected_stage.ClientToScreen(&rect);
	ScreenToClient(&rect);
	dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(&rect);
}

void CDialogBarCM::ResizeOtherBars()
{
    if(m_wndDialogPanelCement.m_hWnd == NULL) return;
    m_wndDialogPanelCement.Invalidate(true);
}

// CDialogBarCM message handlers
LRESULT CDialogBarCM::OnMyInitDialog(WPARAM /*wParam*/, LPARAM /*lParam*/) 
{
    UpdateData(false);
    GetClientRect(&m_rec_s);
    ClientToScreen(&m_rec_s);
    pFrm->ScreenToClient(&m_rec_s);
    AdjustButtonNames();
//    m_PanelKnopki.Init(STAGES_GetStagesNumber(), CSize(20,20), pApp->GetView(gt_cement));
    return true;
}

void CDialogBarCM::RecalcListWidth()
{
    CRect rect;
    GetClientRect(&rect);
    m_list_selected_stage.m_list_width = rect.Width() - 2*STAGES_zazor - 2;
    m_list_selected_stage.m_column_width = m_list_selected_stage.m_list_width;
    if (m_list_selected_stage.m_hWnd != NULL)
	{
		if (m_list_selected_stage.GetItemCount() > m_list_selected_stage.GetCountPerPage())
			m_list_selected_stage.m_column_width -= 16 + STAGES_zazor;
		m_list_selected_stage.SetColumnWidth(0, m_list_selected_stage.m_column_width);
	}
}

void CDialogBarCM::OnUpdateLoad(CCmdUI* pCmdUi)
{
	pCmdUi->Enable(STAGES_GetCurrentMode() != STAGES_MODE_EXECUTION);
}

void CDialogBarCM::OnUpdateUseAsTemplate(CCmdUI* pCmdUi)
{
	pCmdUi->Enable(STAGES_GetCurrentMode() == STAGES_MODE_ARCHIVE);
}

void CDialogBarCM::OnUpdateStart(CCmdUI* pCmdUi)
{
	pCmdUi->Enable(STAGES_GetCurrentMode() == STAGES_MODE_EDIT && STAGES_GetStagesNumber() > 0);
}

void CDialogBarCM::OnUpdateNext(CCmdUI* pCmdUi)
{
	pCmdUi->Enable(STAGES_GetCurrentMode() == STAGES_MODE_EXECUTION && STAGES_GetExecutingStage() < STAGES_GetStagesNumber());
}

void CDialogBarCM::OnUpdateFinish(CCmdUI* pCmdUi)
{
	pCmdUi->Enable(STAGES_GetCurrentMode() == STAGES_MODE_EXECUTION && STAGES_GetExecutingStage() == STAGES_GetStagesNumber());
}

void CDialogBarCM::OnUpdateBreak(CCmdUI* pCmdUi)
{
	pCmdUi->Enable(STAGES_GetCurrentMode() == STAGES_MODE_EXECUTION && STAGES_GetExecutingStage() != STAGES_GetStagesNumber());
}

void CDialogBarCM::OnUpdateButtonStartPauseStage(CCmdUI* pCmdUi)
{
	switch(STAGES_GetCurrentMode())
	{
		case STAGES_MODE_EDIT: OnUpdateStart(pCmdUi); break;
		case STAGES_MODE_ARCHIVE: pCmdUi->Enable(FALSE); break;
		default: pCmdUi->Enable(TRUE);
	}
}

void CDialogBarCM::OnUpdateMenuBreakFinishAll(CCmdUI* pCmdUi)
{
	switch(STAGES_GetCurrentMode())
	{
		case STAGES_MODE_EXECUTION:
		case STAGES_MODE_PAUSE:
		case STAGES_MODE_BREAK:
			pCmdUi->Enable(TRUE); break;
		default: pCmdUi->Enable(FALSE);
	}
}

void CDialogBarCM::OnUpdateButtonFinishStage(CCmdUI* pCmdUi)
{
	switch(STAGES_GetCurrentMode())
	{
		case STAGES_MODE_EXECUTION:
		case STAGES_MODE_PAUSE:
			pCmdUi->Enable(TRUE); break;
		default: pCmdUi->Enable(FALSE);
	}
}

void CDialogBarCM::OnUpdateButtonRestartStage(CCmdUI* pCmdUi)
{
	switch(STAGES_GetCurrentMode())
	{
		case STAGES_MODE_EXECUTION:
		case STAGES_MODE_PAUSE:
			pCmdUi->Enable(TRUE); break;
		default: pCmdUi->Enable(FALSE);
	}
}

void CDialogBarCM::OnButtonStartPauseStage() 
{
	STAGES_StartPauseStage();
    AdjustButtonNames();
}

void CDialogBarCM::OnButtonFinishStage()
{
	STAGES_FinishStage();
	AdjustButtonNames();
}

void CDialogBarCM::OnButtonFinishAllStages()
{
	STAGES_FinishAll();
	AdjustButtonNames();
}

void CDialogBarCM::OnButtonRestartStage()
{
	STAGES_RestartStage();
	AdjustButtonNames();
}

void CDialogBarCM::OnButtonRestartAllStages()
{
	STAGES_RestartAll();
	AdjustButtonNames();
}

void CDialogBarCM::AdjustButtonNames()
{
	STAGES_Mode mode = STAGES_GetCurrentMode();
	int stage_num = STAGES_GetExecutingStage();

	CString big_button_name, menu_name;
	if (mode == STAGES_MODE_BREAK || mode == STAGES_MODE_EDIT || mode == STAGES_MODE_ARCHIVE)
	{
		big_button_name.Format("Начать этап № %d", (mode == STAGES_MODE_BREAK)?stage_num:1);
		menu_name = "Начать этап";
	}
	else
	if (mode == STAGES_MODE_EXECUTION)
	{
		big_button_name = "Пауза";
		menu_name = "Пауза";
	}
	else
	if (STAGES_MODE_PAUSE)
	{
		big_button_name.Format("Продолжить этап № %d", stage_num);
		menu_name = "Продолжить этап";
	}
	m_wndDialogBarCM.m_button_start_pause_stage.SetWindowText(big_button_name); // для вызова из меню


	big_button_name.Format("Закончить этап № %d", max(1, stage_num));
	m_wndDialogBarCM.m_button_finish_stage.SetWindowText(big_button_name); // для вызова из меню

	big_button_name.Format("Сбросить данные этапа № %d", max(1, stage_num));
	m_wndDialogBarCM.m_button_restart_stage.SetWindowText(big_button_name); // для вызова из меню
}

void CDialogBarCM::OnSize(UINT nType, int cx, int cy)
{
    CDialogBar::OnSize(nType, cx, cy);
    RecalcListWidth();
}

