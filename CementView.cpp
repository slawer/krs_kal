// CementView.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "MainFrm.h"
#include "CementDoc.h"
#include "CementView.h"

#include "ChangeParam.h"
#include "STAGES.h"
#include "Func.h"

#include <CHECK.h>

#include "LOG_V0.h"

#include <afxdisp.h>
#include <math.h>

extern double TIME_current_time;
extern UINT startMarker1RT;

IMPLEMENT_DYNCREATE(CCementView, CMainView)

BEGIN_MESSAGE_MAP(CCementView, CMainView)
    ON_MESSAGE(STAGES_LBD_MSG, OnStagesLBD)
    ON_MESSAGE(STAGES_SELECTED_LBD_MSG, OnSelectedStageLBD)
	ON_MESSAGE(STAGES_RBD_MSG, OnStagesRBD)
	ON_MESSAGE(STAGES_SELECTED_RBD_MSG, OnSelectedStageRBD)
    ON_WM_CREATE()
    ON_WM_TIMER()
END_MESSAGE_MAP()

CCementView::CCementView():
	CMainView(CCementView::IDD, CM_NUM_LISTS, CM_NUM_FIELDS_GRAPHS, CM_NUM_FIELDS_PANELS),
	m_need_update_from_db(false)
{
    pCementV = this;
    num_lists = 0;
}

CCementView::~CCementView()
{
	RemoveActivityDB();
    pCementV = NULL;
}

void CCementView::RequestDataFromDB()
{
	if (m_flag_activity_db)
	{
		m_need_update_from_db = false;
		STAGES_GetFromDB();
	}
	else
		m_need_update_from_db = true;
}

void CCementView::DoDataExchange(CDataExchange* pDX)
{
    CMainView::DoDataExchange(pDX);
}
/*
BOOL CCementView::AddSubMenuItem(CMenu* pMenu, int num_menu_item, UINT ID_submenu_item, CString item_name, HICON hIcon)
{
	if(item_name.GetLength() == 0) return false;
    if(pMenu)
    {
        int iMaxPos;
        CMenu *submenu = NULL;
		iMaxPos = (int)pMenu->GetMenuItemCount();
		if(num_menu_item > iMaxPos) return false;

		submenu = pMenu->GetSubMenu(num_menu_item);
		if(submenu == NULL) return false;
		iMaxPos = (int)submenu->GetMenuItemCount();

        BOOL flg = submenu->AppendMenu(MF_STRING | MF_ENABLED, ID_submenu_item, item_name);

        if(hIcon != NULL)
        {
			int cx = ::GetSystemMetrics(SM_CXMENUCHECK);
			int cy = ::GetSystemMetrics(SM_CYMENUCHECK);
            ICONINFO iconinfo;
            GetIconInfo(hIcon, &iconinfo);
            HANDLE hLoad = CopyImage(HANDLE(iconinfo.hbmColor), IMAGE_BITMAP, cx+1, cy+1, LR_COPYFROMRESOURCE);
            CBitmap *pBmp = CBitmap::FromHandle(HBITMAP(hLoad));
            submenu->SetMenuItemBitmaps(iMaxPos, MF_BYPOSITION, pBmp, pBmp);
        }
	}
	return true;
}*/

void CCementView::ReloadDataView()
{
	m_ptmData	= pgtmData;
	m_pcurGlub	= pgcurGlub;
	pBufX = m_ptmData;

	STAGES_Init(&m_wndDialogPanelCement.m_list_stages, &m_wndDialogBarCM.m_list_selected_stage, this);
	m_wndDialogBarCM.AdjustButtonNames();

    SetVisibilityPanelEtap();

    CheckRegim();
	m_wndDialogBarCM.m_PanelKnopki.Init(STAGES_GetStagesNumber(), CSize(20,20), this);
    m_wndDialogBarCM.m_PanelKnopki.PushBtn(m_parOfView->GetNumActiveList());
    CheckGrf();
}

void CCementView::OnInitialUpdate()
{
	CMainView::OnInitialUpdate();

	ReloadDataView();

    SetTimer(1, 50, NULL);

    pCementV = this;
}

void CCementView::UpdateInputDataDB()
{
	UpdateListInputDataDB(STAGES_GetCurrentStageDB() - 1);
}

void CCementView::SetActivityDB()
{
	TRACE0("CementView SetActivity\n");
	DB_AddActivity(this, DB_TransferParamValues, STAGES_DB_Reaction);
	DB_AddActivity(this, DB_Report, STAGES_DB_Reaction);
	m_flag_activity_db = true;
}

void CCementView::RemoveActivityDB()
{
	TRACE0("CementView RemoveActivity\n");
	STAGES_StopGettingFromDB();
	DB_RemoveActivity(this);
	m_flag_activity_db = false;
}

void CCementView::AddList()
{
	num_lists++;
	m_parOfView->AddList();
	ParamOfView *pList = m_parOfView->GetList(m_parOfView->num_lists-1);
	pList->Create(&m_wndDialogBarCM, this, pBufX, &m_MapParamCM, type_view);
    pApp->ChangeStyleAllView(FLD_DATA_ON_FIELD, gflgDataBegEnd);
}

void CCementView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		if (!KRS_running)
			STAGES_InvalidateLists();
	}
}

void CCementView::DeleteList(int num_list)
{
	num_lists--;
	m_parOfView->DeleteList(num_list);
}

void CCementView::SwapLists(int num_list1, int num_list2)
{
	m_parOfView->SwapLists(num_list1, num_list2);
}

LRESULT CCementView::OnStagesLBD(WPARAM w, LPARAM l)
{
    STAGES_OnClick(w, l, STAGES_LEFT_BUTTON, STAGES_WHO_ALL_STAGES);
    return 0;
}

LRESULT CCementView::OnSelectedStageLBD(WPARAM w, LPARAM l)
{
	STAGES_OnClick(w, l, STAGES_LEFT_BUTTON, STAGES_WHO_SELECTED_STAGE);
	return 0;
}

LRESULT CCementView::OnStagesRBD(WPARAM w, LPARAM l)
{
	STAGES_OnClick(w, l, STAGES_RIGHT_BUTTON, STAGES_WHO_ALL_STAGES);
	return 0;
}

LRESULT CCementView::OnSelectedStageRBD(WPARAM w, LPARAM l)
{
	STAGES_OnClick(w, l, STAGES_RIGHT_BUTTON, STAGES_WHO_SELECTED_STAGE);
	return 0;
}

void CCementView::RecalcStagesList()
{
	if (!pFrm || m_wndDialogBarCM.m_hWnd == NULL)
		return;

	CRect rect;
	m_wndDialogBarCM.GetClientRect(&rect);
	int bh = 25;
	int h = bh, w0 = m_wndDialogBarCM.m_list_selected_stage.m_list_width + 2;
	int w = (w0 - STAGES_zazor)/2;
	int y = rect.Height();

    if (m_wndDialogBarCM.m_hWnd != NULL && m_wndDialogBarCM.m_PanelKnopki.m_hWnd != NULL)
    {
        if (m_wndDialogBarCM.m_PanelKnopki.GetCount() == 0)
        {
            m_wndDialogBarCM.m_PanelKnopki.ShowWindow(SW_HIDE);
        }
        else
        {
            CSize sz;
            sz = m_wndDialogBarCM.m_PanelKnopki.GetSize();
            y -= sz.cy + STAGES_zazor;
            m_wndDialogBarCM.m_PanelKnopki.SetWindowPos(NULL, STAGES_zazor, y, w0, sz.cy, SWP_NOZORDER);
            m_wndDialogBarCM.m_PanelKnopki.RepositionBtn();
			CSize new_sz = m_wndDialogBarCM.m_PanelKnopki.GetSize();
			if (new_sz.cy != sz.cy)
			{
				y += sz.cy - new_sz.cy;
				m_wndDialogBarCM.m_PanelKnopki.SetWindowPos(NULL, STAGES_zazor, y, w0, new_sz.cy, SWP_NOZORDER);
			}
            m_wndDialogBarCM.m_PanelKnopki.ShowWindow(SW_SHOW);
        }
    }

	w = m_wndDialogBarCM.m_list_selected_stage.m_list_width + 2;
	y -= h + STAGES_zazor;
	m_wndDialogBarCM.m_button_restart_stage.SetWindowPos(NULL, STAGES_zazor, y, w, h, SWP_NOZORDER);
	y -= h + STAGES_zazor;
	m_wndDialogBarCM.m_button_finish_stage.SetWindowPos(NULL, STAGES_zazor, y, w, h, SWP_NOZORDER);
	y -= h + STAGES_zazor;
	m_wndDialogBarCM.m_button_start_pause_stage.SetWindowPos(NULL, STAGES_zazor, y, w, h, SWP_NOZORDER);
	
	y -= STAGES_zazor;
	h = y - STAGES_zazor;
	y = STAGES_zazor;
	
	m_wndDialogBarCM.m_list_selected_stage.SetWindowPos(NULL, STAGES_zazor + 1, y, w - 2, h, SWP_NOZORDER);
	m_wndDialogBarCM.RecalcListWidth();
	
	m_wndDialogPanelCement.RecalcListWidth();
}

//показать панель этапов
void CCementView::SetVisibilityPanelEtap()
{
    if (m_wndDialogBarCM.m_list_selected_stage.m_hWnd != NULL)
		m_wndDialogBarCM.m_list_selected_stage.ShowWindow(SW_NORMAL);
    if (m_wndDialogPanelCement.m_list_stages.m_hWnd != NULL)
        m_wndDialogPanelCement.m_list_stages.ShowWindow(SW_NORMAL);
    m_wndDialogBarCM.m_list_selected_stage.ShowWindow(SW_NORMAL);
    m_wndDialogBarCM.m_button_start_pause_stage.ShowWindow(SW_NORMAL);
    m_wndDialogBarCM.m_button_finish_stage.ShowWindow(SW_NORMAL);
    m_wndDialogBarCM.m_button_restart_stage.ShowWindow(SW_NORMAL);
    RecalcStagesList();
    
    if (m_need_update_from_db)
	{
		m_need_update_from_db = false;
		STAGES_GetFromDB();
	}
}

void CCementView::ChangeaActiveListA(WPARAM wParam, LPARAM lParam)
{
	CDataView *data_view = GetActiveData();
	if (data_view == NULL)
		return;
		
	int num_prev_list = data_view->GetNumActiveList(), num_new_list = int(wParam);
/*
	if (num_prev_list == num_new_list &&
		num_prev_list != 0)
		return;
*/
	ParamOfView* pov = GetActiveList();
	if (pov == NULL || num_new_list == -1)
		return;
	pov->ShowActiveList(false);

	STAGES_SetSelectedStage(num_new_list + 1);
	STAGES_UpdateMapParam();
	data_view->SetNumActiveList(num_new_list);

    ParamOfView* pList = m_parOfView->GetActive();
    if (pList == NULL)
		return;
    pList->RecalcFieldSize(type_view);
	RecalcScaleFactor(num_new_list);
//	SetParXScale(num_new_list);

	pov = GetActiveList();
	if (pov == NULL)
		return;
	pov->ShowActiveList(true);

	CheckRegim();
	CheckGrf();
}

void CCementView::RButtonDown1A(WPARAM wParam, LPARAM lParam)
{
    if(wParam == 1)
    {
        startMarker1RT = (UINT)lParam;

        if(GetActiveData()->flag_time_glub) m_numContextMenu = 1;
        else m_numContextMenu = 2;
    }
    else m_numContextMenu = wParam;
}

void CCementView::HideAllGraphsInStage(int n_list)
{
    ParamOfView *pList = m_parOfView->GetList(n_list);
    if (pList == NULL) return;
    VFRingBuf<double> *m_bufX;
    for(int i = 0; i < pList->num_grf_panel; i++)
    {
        m_bufX = GetVXBuffer(pList, i);
        if(m_bufX == NULL) return;
        m_bufX->ResetAllUMarkers();
    }
    pList->begTime = pgtmData->GetCur();
    SetStartX(pList->begTime, n_list, true);
//    RecalcScaleFactor(n_list);
    SetParXScale(n_list, true);
}

void CCementView::HideAllGraphsInStages()
{
	int max_i = STAGES_GetStagesNumber();
    for(int i = 0; i < max_i; i++) HideAllGraphsInStage(i);
}

void CCementView::SetValidIndexBeginData(int num_list)
{
	ParamOfView *pList = m_parOfView->GetList(num_list);
    if (pList == NULL) return;
	VFRingBuf<double> *m_bufX;
    for(int i = 0; i < pList->num_grf_panel; i++)
    {
        m_bufX = GetVXBuffer(pList, i);
        if(m_bufX == NULL) continue;
	    m_bufX->SetPosUMarker(0, pgtmData->GetCur());
        m_bufX->SetPosUMarker23ToEndBuf();
    }
    pList->begTime = pgtmData->GetLastData();
    RecalcScaleFactor(num_list);
    SetStartX(pList->begTime, num_list, true);
//    SetParXScale(num_list, true);
}

VFRingBuf<double>* CCementView::GetVXBuffer(ParamOfView* pList, int n_grf_panel)
{
	if (pList == NULL) return NULL;
	return pList->GetVXBuffer(n_grf_panel);
}

double CCementView::GetStartX(int num_list)
{
	ParamOfView* pV = m_parOfView->GetList(num_list);
	if (pV == NULL) return 0;

	if (STAGES_GetCurrentMode() == STAGES_MODE_ARCHIVE)
	{
		STAGES_Stage* stage = STAGES_GetStage(num_list + 1);
		double time = 0;
		if (stage != 0 && stage->m_start_time > 0)
		{
			time = stage->m_start_time;
		}

		if(m_parOfView->flag_time_glub) 
		{
			if(m_ptmData == NULL) return pV->begTime;
			if(m_ptmData->GetSizeData() == 0) return  pV->begTime;
				else return time;// m_ptmData->GetElement(startMarker1DB);
		}
		else
		{
			if(m_pcurGlub == NULL) return pV->begGlub;
			if(m_pcurGlub->GetSizeData() == 0) return  pV->begGlub;
			else return time;// m_pcurGlub->GetElement(startMarker1DB);
		}
		return time;
	}

	GRF_MINMAX par;
	par.flags_minmax.bitFlags.fl_buf_x = 1;
	par.flags_minmax.bitFlags.fl_buf_x_only = 1;
	pV->ChangeScale(pBufX, par, m_parOfView->flag_time_glub);
	
	double start_value;
	VFRingBuf<double> *m_bufX = GetVXBuffer(m_parOfView->GetList(num_list), 0);

    if(m_parOfView->flag_time_glub)
    {
        if(m_bufX == NULL) return GetCurTime();
        if(m_bufX->GetSizeData() == 0) start_value = GetCurTime();
        else 
        {	
            if(m_bufX->GetLastData() - m_bufX->GetFirstData() < GetLenX(num_list)) 
                start_value = m_bufX->GetFirstData();
            else start_value = m_bufX->GetLastData() - GetLenX(num_list);
        }
    }
    else
    {
        if(m_bufX == NULL) return pGlubina->fCur;
        if(m_bufX->GetSizeData() == 0) start_value = pGlubina->fCur;
        else 
        {
            if(m_bufX->GetLastData() - m_bufX->GetFirstData() < GetLenX(num_list)) 
                start_value = m_bufX->GetFirstData();
            else start_value = m_bufX->GetLastData() - GetLenX(num_list);
        }
    }
    return start_value;
}

void CCementView::OnScrollGraphs()
{
	ParamOfView* pV = m_parOfView->GetActive();
	if (pV == NULL) return;

	int i = pV->GetFirstNotEmptyPanel();
	if (i == -1)
		return;

	GRF_MINMAX par = pV->panArray[i]->m_parGraph.GetParMinMax(0);
}

void CCementView::SetStartX(double start_value, int num_list, BOOL flg)
{
	if(pFrm == NULL) return;
	int nn;
	ParamOfView* pV = m_parOfView->GetList(num_list);
	if (pV == NULL) return;

	pV->begTime = start_value;

	nn = (m_parOfView->flag_time_glub) ? (int)pV->gTimeScale : (int)pV->gGlubScale;

	GRF_MINMAX par;
	double len_x = GetLenX(num_list);

	STAGES_Stage* stage = STAGES_GetStage(num_list + 1);

	par.FminS = (stage == NULL) ? start_value : stage->m_start_time;
	par.FmaxS = par.FminS + ((stage == NULL || stage->m_duration == 0) ? len_x : stage->m_duration); // при len_x хотя бы масштаб меняется

	par.Fmin = start_value;
	par.Fmax = start_value + len_x;

	par.textGrf = GetStrScale(nn);
	par.flags_minmax.dwFlags = FL_GRF_MINMAX | FL_GRF_TEXT;
	if(flg) par.flags_minmax.dwFlags |= FL_GRF_DIAPAZON; 
	par.flag_undo = !pV->flg_nonstandart_scale;
	par.flags_minmax.bitFlags.fl_buf_x = 1;
	par.flags_minmax.bitFlags.fl_buf_x_only = 0;

	pV->ChangeScale(pBufX, par, m_parOfView->flag_time_glub);
}

//========================================================================
void CCementView::Serialize(CArchive& ar)
{
	int zzz = 0;
}

int CCementView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMainView::OnCreate(lpCreateStruct) == -1)
        return -1;

    pCementV = this;
    SetTimer(10, 30, NULL); 
    return 0;
}
