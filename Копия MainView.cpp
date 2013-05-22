// KRSView.cpp : implementation of the CMainView class
//

#include "stdafx.h"
#include "KRS.h"
#include "MainFrm.h"
#include "KRSDoc.h"
#include "KRSView.h"
#include "Func.h"

#include "src/SERV.h"
#include "src/CONV.h"
#include "src/DB.h"
#include "src/PRJ.h"

#include "ChangeParam.h"

#include <CHECK.h>

#include <afxdisp.h>
#include <math.h>
#include "STAGES.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT startMarker1RT = 0;
UINT startMarker1DB = 0;

extern map<int, BS_ParamDescription> DB_params_from_db;
float scaleT[11] = {5.0f, 10.0f, 20.0f, 60.0f, 120.0f, 240.0f, 360.0f, 720.0f,  1440.0f, 2880.0f, 18000.0f};
float scaleG[9] = {10.0f, 20.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 5000.0f, 10000.0f};

extern double TIME_current_time;
extern COleDateTime RequestBegTime, RequestEndTime;

/////////////////////////////////////////////////////////////////////////////
// CMainView

IMPLEMENT_DYNCREATE(CMainView, CFormView)

BEGIN_MESSAGE_MAP(CMainView, CFormView)
	ON_MESSAGE(PM_ADD_PARAM, OnAddParam)
	ON_MESSAGE(PM_ADD_PARAM2, OnAddParam2)
	ON_MESSAGE(PM_SET_STYLE, OnSetStyleAllGrf)
	ON_MESSAGE(PM_RESET_STYLE, OnResetStyleAllGrf)	
    ON_MESSAGE(PM_SET_STYLE_ACTYVE_LIST, OnSetStyleActiveList)
    ON_MESSAGE(PM_RESET_STYLE_ACTYVE_LIST, OnResetStyleActiveList)
	ON_MESSAGE(PM_CHANGESCALE, OnChangeTypeScale)
	ON_MESSAGE(PM_CHANGE_ACTIVE_LIST, OnChangeaActiveList)
	ON_MESSAGE(PM_CHANGE_NUM_GRF, OnChangeaNumGrf)
	ON_MESSAGE(PM_CHANGE_PARAM_PROPERT_1, OnChangeParamProperty_1)
	ON_MESSAGE(PM_CHANGE_PARAM_PROPERT, OnChangeParamProperty)
	ON_MESSAGE(PM_RECALC_SIZE, OnRecalcSize)
	ON_MESSAGE(PM_RECALC_SIZE_DGT_PANELS, OnRecalcSizeDgtPanels)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_MESSAGE(PM_CHANGE_NAME_PANEL, OnChangeNamePanel)
	ON_MESSAGE(PM_SAVE_ALL_LIST, OnSaveAllList)
	ON_MESSAGE(PM_UPDATE_VIEW, OnUpdateView)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDR_CONT_MENU_MARKER, OnContMenuMarker)
	ON_MESSAGE(PM_R_BUTTON_DOWN, OnRButtonDown1)
	ON_MESSAGE(PM_DELETE_EXT_PARAM, OnDeleteExtParam)
	ON_COMMAND(R_CONT_MENU_SCALE_5, OnContMenuScale5)
	ON_COMMAND(R_CONT_MENU_SCALE_10, OnContMenuScale10)
	ON_COMMAND(R_CONT_MENU_SCALE_120, OnContMenuScale120)
	ON_COMMAND(R_CONT_MENU_SCALE_1440, OnContMenuScale1440)
	ON_COMMAND(R_CONT_MENU_SCALE_20, OnContMenuScale20)
	ON_COMMAND(R_CONT_MENU_SCALE_240, OnContMenuScale240)
	ON_COMMAND(R_CONT_MENU_SCALE_2880, OnContMenuScale2880)
	ON_COMMAND(R_CONT_MENU_SCALE_360, OnContMenuScale360)
	ON_COMMAND(R_CONT_MENU_SCALE_60, OnContMenuScale60)
	ON_COMMAND(R_CONT_MENU_SCALE_720, OnContMenuScale720)
	ON_COMMAND(NT_MENU_SCALE_9000, OnMenuScaleTest)
	ON_COMMAND(R_CONT_MENU_SCALE_G_10, OnContMenuScaleG10)
	ON_COMMAND(R_CONT_MENU_SCALE_G_100, OnContMenuScaleG100)
	ON_COMMAND(R_CONT_MENU_SCALE_G_20, OnContMenuScaleG20)
	ON_COMMAND(R_CONT_MENU_SCALE_G_50, OnContMenuScaleG50)
	ON_COMMAND(R_CONT_MENU_SCALE_G_200, OnContMenuScaleG200)
	ON_COMMAND(R_CONT_MENU_SCALE_G_500, OnContMenuScaleG500)
	ON_COMMAND(R_CONT_MENU_SCALE_G_1000, OnContMenuScaleG1000)
	ON_COMMAND(R_CONT_MENU_SCALE_G_2000, OnContMenuScaleG2000)
	ON_COMMAND(R_CONT_MENU_SCALE_G_5000, OnContMenuScaleG5000)
	ON_COMMAND(NT_MENU_SCALE_G_10000, OnMenuScaleG10000)
	ON_MESSAGE(PM_SHOW_PANEL, OnPanelGrf)
	ON_MESSAGE(PM_CHANGE_RECT_RANGE, OnChangeRangeGrf)
	ON_MESSAGE(PM_CHANGESCROLL, OnChangeScrollGrf)
	ON_MESSAGE(PM_DELETE_PARAM_FROM_LIST, OnDeleteParamFromList) ///во всех View !!!!!!
	ON_MESSAGE(PM_RECALC_ACTIVE_LIST, OnRecalcActiveLilst)
    ON_MESSAGE(PM_MODULES_CHANGED, OnModulesChanges)
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainView construction/destruction

CMainView::CMainView(UINT nIDTemplate, int n_lists, int n_grf_panel, int n_dgt_panel)
	: CFormView(nIDTemplate)
{
	m_ptmData	= NULL;
	m_pcurGlub	= NULL;
	pBufX = (LPVOID)m_ptmData; 
    m_parOfView = NULL;
    InitDataView(n_lists, n_grf_panel, n_dgt_panel);
	m_flag_activity_db = false;
	m_panel_descrG = m_panel_descrD = NULL;
}

CMainView::CMainView(): CFormView(-1)
{
}

static bool g_exit_thread_started = false;
static HANDLE g_exit_event = CreateEvent(NULL, FALSE, FALSE, NULL);

UINT ExitTreadProcedure(LPVOID param)
{
	PRJ_CleanUp();
	if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
		STAGES_Shutdown();

	NET_shutdown = true;
	KRS_shutdown = true;

	SERV_ShutDownConnection(true);
	CONV_ShutDownConnection(true);
    DB_ShutDownConnection(true);

	SetEvent(g_exit_event);
	return +1;
}

void CMainView::OnDestroy() 
{
	static int zzz = 0;
	if (zzz == 0)
	{
		zzz = 1;
		PRJ_SaveProject("", false, false);
		g_exit_thread_started = true;
		AfxBeginThread(ExitTreadProcedure, NULL, THREAD_PRIORITY_ABOVE_NORMAL);
	}
	else
	if (zzz == 1)
	{
		zzz = 2;
		if (g_exit_thread_started)
			WaitForSingleObject(g_exit_event, INFINITE);
	}

	if (m_parOfView != NULL)
	{
		ParamOfView *pList;
		for(int i = 0; i < m_parOfView->num_lists; i++) 
		{
			pList = m_parOfView->GetList(i);
			if(pList == NULL) continue;
			pList->DeleteAllPanel();
		}
	}
    CFormView::OnDestroy();
}

LRESULT CMainView::OnModulesChanges(WPARAM wParam, LPARAM lParam)
{
    SetModulesExtern();
    return true;
}

void CMainView::InitDataView(int n_lists, int n_grf_panel, int n_dgt_panel)
{
	if (m_parOfView != NULL) delete m_parOfView;
	m_parOfView = new CDataView(n_lists, n_grf_panel, n_dgt_panel);
	num_lists = m_parOfView->num_lists;
	if (num_lists != 0)
	{
		ParamOfView* pList = m_parOfView->GetList(0);
		num_grf_panel = pList->num_grf_panel;
		num_dgt_panel = pList->num_dgt_panel;
	}
	else
	{
		num_grf_panel = 0;
		num_dgt_panel = 0;
	}

	if (::IsWindow(m_hWnd))
	{
		PostMessage(PM_CHANGE_ACTIVE_LIST, -1, NULL);
		OnSetFocus(NULL);
	}
}

CMainView::~CMainView()
{
    delete m_parOfView;

	if(m_panel_descrG != NULL) delete[] m_panel_descrG, m_panel_descrG = NULL;
	if(m_panel_descrD != NULL) delete[] m_panel_descrD, m_panel_descrD = NULL;
}

void CMainView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainView)
	//}}AFX_DATA_MAP
}

void CMainView::OnInitialUpdate()
{
	ASSERT(pFrm);
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(false);
}

void CMainView::DeleteView()
{
	ParamOfView *pList;
	for(int i = 0; i < m_parOfView->num_lists; i++) 
    {
		pList = m_parOfView->GetList(i);
		if (pList == NULL)
			continue;
		pList->DeleteAllPanel();
	}
}

void CMainView::InitList(int num_list, CWnd* pParentForDgtPanel, LPVOID pBufX, CParamMap* pmapPrm) 
{
    CDataView *data_view = GetActiveData();
    if (data_view == NULL) return;

    ParamOfView *pList = m_parOfView->GetList(num_list);
    if (pList == NULL) return;
    pList->DeleteAllPanel();
    pList->Create(pParentForDgtPanel, this, pBufX,	pmapPrm, type_view);
}

void CMainView::InitViews(CWnd* pParentForDgtPanel, CParamMap* pmapPrm)
{
    CDataView *data_view = GetActiveData();
    if (data_view == NULL)
        return;
    pBufX = (LPVOID)m_ptmData; 
//-----------------------------------------------------------
    m_startTime = COleDateTime::GetCurrentTime();	
    data_view->scale_x_start = m_startTime.m_dt;	
    data_view->scale_x_end = data_view->scale_x_start + GetLenX(data_view->GetNumActiveList());
    m_startTick = GetTickCount();

    for(int i = 0; i < num_lists; i++) InitList(i, pParentForDgtPanel, pBufX, pmapPrm);
	RestoreAllLists();
}

void CMainView::SetInfoAllLists()
{
	if (GetActiveData() == NULL) return;

	ASSERT(m_panel_descrG);
	ASSERT(m_panel_descrD);
//---------------------------------------------------------------------------
//восстанавливаем параметры для листa
    list<ParamInPanel>::iterator current_pd;
    ParamInPanel pd;

	int i, j;
	ParamOfView *pList;
	for(i = 0; i < num_lists; i++)
	{
		pList = m_parOfView->GetList(i);
		if(pList == NULL) continue;

		for(j = 0; j < num_grf_panel; j++)
		{
			pList->SetNamePanel(j, m_panel_descrG[i].snamePanel);
			current_pd = m_panel_descrG[i].m_params.begin();
			while (current_pd != m_panel_descrG[i].m_params.end())
			{
				pd = *current_pd;
				pList->AddParam(j, &pd);
				// установить мин макс, цвет и толщину
				current_pd++;
			}
		}

		for(j = 0; j < num_dgt_panel; j++)
		{
			pList->SetNamePanel(j, m_panel_descrD[i].snamePanel);
			current_pd = m_panel_descrD[i].m_params.begin();
			while (current_pd != m_panel_descrD[i].m_params.end())
			{
				pd = *current_pd;
				pList->AddParam(j, &pd);
				// установить мин макс, цвет и толщину
				current_pd++;
			}
		}

	}
}


void CMainView::RestoreList(int num_list)
{
	if (GetActiveData() == NULL) return;

	if(type_view == t_cement_rt || type_view == t_cement_db) STAGES_UpdateMapParam(num_list + 1);
    double stim = GetCurTime();
//---------------------------------------------------------------------------
//восстанавливаем параметры для листa
    ParamOfView *pList = m_parOfView->GetList(num_list);
    if(pList == NULL) return;
//------------------------------------------------------------
    PanelDescription panel_descr;
    int j;

    list<ParamInPanel>::iterator current_pd;
    ParamInPanel pd;

	for(j = 0; j < num_grf_panel; j++)
    {
        pApp->ReadPanelList(num_list, j, &panel_descr, type_view);

        pList->SetNamePanel(j, panel_descr.snamePanel);
        current_pd = panel_descr.m_params.begin();
        while (current_pd != panel_descr.m_params.end())
        {
            pd = *current_pd;
            pList->AddParam(j, &pd);
            // установить мин макс, цвет и толщину
            current_pd++;
        }
    }

    for(j = 0; j < num_dgt_panel; j++)
    {
        pApp->ReadPanelList(num_list, num_grf_panel + j, &panel_descr, type_view);
        pList->SetNamePanel(num_grf_panel + j, panel_descr.snamePanel);

        current_pd = panel_descr.m_params.begin();
        while (current_pd != panel_descr.m_params.end())
        {
            pd = *current_pd;
            pList->AddParam(num_grf_panel + j, &pd);
            current_pd++;
        }
    }
//------------------------------------------------------------

    pList->begTime = stim;
    pList->endTime = pList->begTime + MinuteToOleTime((float)pList->gDeltaTime);
    SetStartX(pList->begTime, num_list, true);
    RecalcScaleFactor(num_list);
    SetParXScale(num_list, true);
}

void CMainView::RestoreAllLists()
{
    if (GetActiveData() == NULL)
        return;
    int num_active_list = m_parOfView->GetNumActiveList();
    SelectRegime();

    for(int i = 0; i < num_lists; i++) RestoreList(i);

    m_parOfView->SetNumActiveList(num_active_list);
    if(type_view == t_cement_rt || type_view == t_cement_db) STAGES_UpdateMapParam();
    SelectRegime();
    RecalcScaleFactor(num_active_list);
    SetParXScale(num_active_list, true);
}

void CMainView::SaveAllLists()
{
	if(GetActiveData() == NULL) return;
	PanelDescription panel_descr;
	ParamOfView *pList;
	int i, j;
	for(i = 0; i < num_lists; i++)
	{
		pList = m_parOfView->GetList(i);
		if(pList == NULL) continue;

		for(j = 0; j < num_grf_panel; j++)
		{
			pList->GetInfoPanel(j, &panel_descr);
			pApp->WritePanelList(i, j, &panel_descr, type_view);
		}

		for(j = 0; j < num_dgt_panel; j++)
		{
			if(pList->GetInfoPanel(num_grf_panel + j, &panel_descr))
				pApp->WritePanelList(i, num_grf_panel + j, &panel_descr, type_view);
		}
	}
}

void CMainView::GetInfoAllLists()
{
	if(GetActiveData() == NULL) return;

	if(m_panel_descrG != NULL) delete[] m_panel_descrG, m_panel_descrG = NULL;
	m_panel_descrG = new PanelDescription[num_lists];

	if(m_panel_descrD != NULL) delete[] m_panel_descrD, m_panel_descrD = NULL;
	m_panel_descrD = new PanelDescription[num_lists];

	ParamOfView *pList;
	int i, j;
	for(i = 0; i < num_lists; i++)
	{
		pList = m_parOfView->GetList(i);
		if(pList == NULL) continue;

		for(j = 0; j < num_grf_panel; j++)
			pList->GetInfoPanel(j, &m_panel_descrG[i]);

		for(j = 0; j < num_dgt_panel; j++)
			pList->GetInfoPanel(num_grf_panel + j, &m_panel_descrD[i]);
	}
}

void CMainView::SetOrientView()
{
	if(GetActiveData() == NULL) return;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return;

	pList->m_flag_orient = !pList->m_flag_orient; 
	pApp->ChangeStyleActiveList(GRF_ORIENT_FLD, pList->m_flag_orient);

	pList->ShowGrfPanels(false);
//	RecalcActiveLilst();
	pList->RecalcPanelsGrf(type_view);
	RecalcLenX(pList->GetLengthFieldGrf());
	pList->ShowGrfPanels(true);

//	RecalcActiveLilst();
}

#ifdef _DEBUG
void CMainView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMainView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void CMainView::RedrawActiveView()
{
	if(GetActiveData() == NULL) return;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return;
	pList->RedrawActiveList();
}

void CMainView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	if(!pFrm) return;
	if(GetActiveData() == NULL)
		return;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL)
		return;

	RecalcActiveLilst();
	RecalcLenX(pList->GetLengthFieldGrf());
//	MessageBeep(MB_OK);

    if (type_view == gt_cement && STAGES_IsInited())
		RecalcStagesList();

//	Invalidate(true);
}

//Масштаб глубины
//1:500
//1метр на диаграмме равен 500 метров проходки
//1метр проходки на диаграмме равен 2мм

//Временные
//1:10
//1час регистрации параметров (на диаграмме) равен 10мм
//1:60
//1час регистрации параметров (на диаграмме) равен 60мм

float CMainView::GetScaleFactorT(CDC* pDC, float scale)
{
	if(scale == 0) scale = 10;
	int Lpg = pDC->GetDeviceCaps(LOGPIXELSY);

	float aa = 25.4f/Lpg;//число миллиметров в одном пикселе
	return (25.4f*60)/(Lpg*scale);//число минут в одном пикселе
	//для данного масштаба	scale
}
float CMainView::GetScaleFactorG(CDC* pDC, float scale)
{
	if(scale == 0) scale = 10;
	int Lpg = pDC->GetDeviceCaps(LOGPIXELSY);

	float aa = 25.4f/Lpg;//число миллиметров в одном пикселе
	return (25.4f*scale)/(Lpg*1000.0f);//число метров глубины в одном пикселе
	//для данного масштаба scale	
}
//-------------------------------------------------------------
//Обратный расчет. Задано промежуток времени в секундах на шкале, нужно найти масштаб
int CMainView::GetScaleT(CDC* pDC, int time_sekund)
{
	if(time_sekund == 0) time_sekund = 1;
	int Lpg = pDC->GetDeviceCaps(LOGPIXELSY);
	CRect rec;
	GetClientRect(&rec);
	float aa = 25.4f/Lpg;//число миллиметров в одном пикселе
	return int((25.4f*60*60*rec.Height())/(Lpg*time_sekund));//масштаб - число мм экрана в одном часе
}

//Обратный расчет. Задан промежуток глубины в сантиметрах на шкале, нужно найти масштаб
int CMainView::GetScaleG(CDC* pDC, int glub_santimetr)
{
	if(glub_santimetr == 0) glub_santimetr = 1;
	int Lpg = pDC->GetDeviceCaps(LOGPIXELSY);

	CRect rec;
	GetClientRect(&rec);
	float aa = 25.4f/Lpg;//число миллиметров в одном пикселе

	float pp = aa*rec.Height();//число метров на экране
	float ss = (1000.0f*glub_santimetr)/(pp*100.0f);//число метров проходки на 1 метр экрана
	return int(ss);
}

void CMainView::CheckGrf()
{
	if(GetActiveData() == NULL) return;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return;

	((CButton *)(m_wndDialogBarH.GetDlgItem(IDC_CHECK_NGR1)))->SetCheck(pList->m_active_grf[0]);
	((CButton *)(m_wndDialogBarH.GetDlgItem(IDC_CHECK_NGR2)))->SetCheck(pList->m_active_grf[1]);
	((CButton *)(m_wndDialogBarH.GetDlgItem(IDC_CHECK_NGR3)))->SetCheck(pList->m_active_grf[2]);
}
void CMainView::CheckRegim()
{
	((CButton *)(m_wndDialogBarH.GetDlgItem(IDC_RADIO_SCALE_TIM)))->SetCheck(GetActiveData()->flag_time_glub);
	((CButton *)(m_wndDialogBarH.GetDlgItem(IDC_RADIO_SCALE_GLUB)))->SetCheck(!GetActiveData()->flag_time_glub);
}

void CMainView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	ASSERT(pFrm);
	if(GetActiveData() == NULL) return;

	CMenu menu;
	if(!pWnd->IsKindOf(RUNTIME_CLASS(CGraph))) return;

	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return;

	if(m_numContextMenu == 0)
	{
		/*
		if(menu.LoadMenu(IDR_MENU_CONT))
		{
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
		point.x, point.y,
		AfxGetMainWnd()); // route commands through main window
		}
		*/
	}
	else if(m_numContextMenu == 1)
	{
		if(menu.LoadMenu(IDR_MENU_CONT))
		{
			CMenu* pPopup = menu.GetSubMenu(1);
			ASSERT(pPopup != NULL);

			int mm  = (int)pList->gTimeScale;

			int nn = -1;
			switch(mm)
			{
			case 5:
				nn = 0;
				break;
			case 10:
				nn = 1;
				break;

			case 20:
				nn = 2;
				break;

			case 60:
				nn = 3;
				break;

			case 120:
				nn = 4;
				break;

			case 240:
				nn = 5;
				break;

			case 360:
				nn = 6;
				break;

			case 720:
				nn = 7;
				break;

			case 1440:
				nn = 8;
				break;

			case 2880:
				nn = 9;
				break;

			case 18000:
				nn = 10;
				break;
			}

			if(nn >= 0) pList->gTimeScalePrev = (float)nn;
			else nn =(int)pList->gTimeScalePrev;

			pPopup->CheckMenuRadioItem(0, 10, nn, MF_BYPOSITION);

			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
				point.x, point.y,
				AfxGetMainWnd()); // route commands through main window
		}
	}
	else if(m_numContextMenu == 2)
	{
		if(menu.LoadMenu(IDR_MENU_CONT))
		{
			CMenu* pPopup = menu.GetSubMenu(2);
			ASSERT(pPopup != NULL);

			int mm  = (int)pList->gGlubScale;

			int nn = -1;
			switch(mm)
			{
			case 10:
				nn = 0;
				break;

			case 20:
				nn = 1;
				break;

			case 50:
				nn = 2;
				break;

			case 100:
				nn = 3;
				break;

			case 200:
				nn = 4;
				break;

			case 500:
				nn = 5;
				break;

			case 1000:
				nn = 6;
				break;

			case 2000:
				nn = 7;
				break;

			case 5000:
				nn = 8;
				break;

			case 10000:
				nn = 9;
				break;
			}

			if(nn >= 0) pList->gGlubScalePrev = (float)nn;
			else nn = (int)pList->gGlubScalePrev;

			pPopup->CheckMenuRadioItem(0, 9, nn, MF_BYPOSITION);

			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
				point.x, point.y,
				AfxGetMainWnd()); // route commands through main window
		}
	}
	return;	
}


/////////////////////////////////////////////////////////////////////////////
// CMainView printing
BOOL CMainView::GetMinMaxScale(GRF_MINMAX &par, int num_list)
{
	if(GetActiveData() == NULL) return 0;
	FRingBuf<double> *pTX, *pGX;
	BOOL flg_stretch = false;
	pTX = m_ptmData;
	pGX = m_pcurGlub;

	if(GetActiveData()->flag_time_glub)
	{
		if(pTX == NULL) {par.Fmin = par.Fmax = GetCurTime(); return false;}
		else
		{
			if(pTX->GetSizeData() == 0) {par.Fmin = par.Fmax = GetCurTime(); return false;}
			else
			{
				if(pTX->GetLastData() - pTX->GetFirstData() < GetLenX(num_list)) 
				{
					par.Fmin = pTX->GetFirstData();
					par.Fmax = pTX->GetLastData();
					flg_stretch = true;
				}
				else if(pTX->GetLastData() < par.Fmax)
				{
					par.Fmax = pTX->GetLastData();
					flg_stretch = true;
				}
			}
		}
	}
	else
	{
		if(pGX == NULL) {par.Fmin = par.Fmax = 0; return false;}
		else
		{
			if(pGX->GetSizeData() == 0) {par.Fmin = par.Fmax = 0; return false;}
			else
			{
				if(pGX->GetLastData() - pGX->GetFirstData() < GetLenX(num_list)) 
				{
					par.Fmin = pGX->GetFirstData();
					par.Fmax = pGX->GetLastData();
					flg_stretch = true;
				}
				else if(pTX->GetLastData() < par.Fmax)
				{
					//                    par.Fmax = pTX->GetLastData();
					par.Fmax = pTX->GetLastData();// - GetLenX();
					flg_stretch = true;
				}
			}
		}
	}
	return flg_stretch;
}
#include <winspool.h>
void CMainView::SetParamPrinting(CDC* pDC)
{
	int Lpg, wx, wy; 

	int shift = 10; // ширина границы в миллиметрах

	Lpg = pDC->GetDeviceCaps(LOGPIXELSX);
	wx = pDC->GetDeviceCaps(HORZSIZE) - 2*shift;
	wy = pDC->GetDeviceCaps(VERTSIZE) - 2*shift;

	CRect rec;
	GetClientRect(&rec);

	m_printScaleX = (float)((wx * Lpg)/(25.4 * rec.Width())); 
	m_printScaleY = (float)((wy * Lpg)/(25.4 * rec.Height()));

	m_printShift.cx = long(shift*(Lpg/25.4f));
	m_printShift.cy = long(shift*(Lpg/25.4f));

	//поправки для размещения шкал
	CRect rec1;
	m_wndDaligLineParam.GetClientRect(&rec1);
	if(type_view == gt_cement) rec1.bottom += 45;//rec1.Height()/5;
    else rec1.bottom += 20;

//	m_printShift.cy += (LONG)(rec1.Height()*(GetOrient() ? m_printScaleY : m_printScaleY * 1.5));
	m_printShift.cy += (LONG)(rec1.Height()*m_printScaleY);
	m_printShift.cy *= 1.2f;

	float k = 1.0f - float(rec1.Height())/rec.Height();

	m_printScaleY *= k;
}

BOOL CMainView::GetOrient()
{
	if(GetActiveData() == NULL) return false;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return false;
	return pList->m_flag_orient;
}

void CMainView::LockGraphData(int n_lock)
{
    if(GetActiveData() == NULL) return;
    if(type_view == gt_cement)
    {
        for(int i = 0; i < num_lists; i++)
        {
            ParamOfView *pList = m_parOfView->GetList(i);
            if(pList == NULL) continue;
            pList->LockDataForPrint(n_lock);
        }
    }
    else
    {
        ParamOfView *pList = GetActiveList();
        if(pList == NULL) return;
        pList->LockDataForPrint(n_lock);
    }
}
static int cur_list = 0;

BOOL CMainView::OnPreparePrinting(CPrintInfo* pInfo)
{
    cur_list = 0;
    if(type_view == gt_cement) pInfo->SetMaxPage(num_lists);
    else pInfo->SetMaxPage(1);

	pInfo->m_nNumPreviewPages = 1;
  
	if(GetActiveData() == NULL) return false;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return false;

	if(pApp->GetPrinterDeviceDefaults(&pInfo->m_pPD->m_pd))
	{
		DEVMODE FAR *pDevMode = (DEVMODE FAR *)::GlobalLock(pInfo->m_pPD->m_pd.hDevMode);
		pDevMode->dmOrientation = pList->m_flag_orient ? DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;
		::GlobalUnlock(pInfo->m_pPD->m_pd.hDevMode);

		if(DoPreparePrinting(pInfo)) 
		{
			LockGraphData(1);
			return true;
		}
	}
    return false;
}

void CMainView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
    CFormView::OnPrepareDC(pDC, pInfo);
}

void CMainView::PrepareListToPrint(CDC* pDC, int n_list)
{
    if(GetActiveData() == NULL) return;
    ParamOfView *pList = m_parOfView->GetList(n_list);
    if(pList == NULL) return;

//получить и сохранит исходные minf, maxf
    pList->GetMinMaxX(minf_print, maxf_print);
    par_print.Fmin = minf_print;
    par_print.Fmax = maxf_print;

    if(type_view == gt_cement) return;

//получить новый диапазон печати
    flg_stretch_print = GetMinMaxScale(par_print, n_list);
//пересчитываем масштабный коэффициент
//    RecalcScaleFactor();
///========================================================================================
//устанавливаем масштаб, чтоб графики растянуть на все поле
// получить новый масштаб по X
    int nn;
    F df = par_print.Fmax - par_print.Fmin;
    int len;
    if(GetActiveData()->flag_time_glub) 
    {
        len = int((df - int(df))*60*60*24);
        nn = GetScaleT(pDC, len);
    }
    else
    {
        len = int(df*100);
        nn = GetScaleG(pDC, len);
    }
//установить новый масштаб
    par_print.textGrf = GetStrScale(nn);
    par_print.flags_minmax.dwFlags = FL_GRF_TEXT;
    if(flg_stretch_print) par_print.flags_minmax.dwFlags |= FL_GRF_MINMAX;//если сменили диапазон печати(растянуть)
    par_print.flag_undo = false;
	par_print.flags_minmax.bitFlags.fl_buf_x_only = 0;

    pList->ChangeScale(NULL, par_print, GetActiveData()->flag_time_glub);

}
void CMainView::RestoreParamList(CDC* pDC, int n_list)
{
    if(type_view == gt_cement) return;
    if(GetActiveData() == NULL) return;
    ParamOfView *pList = m_parOfView->GetList(n_list);
    if(pList == NULL) return;

    //восстановить старые minf, maxf
    if(flg_stretch_print)
    {
        par_print.Fmin = minf_print;
        par_print.Fmax = maxf_print;
        par_print.textGrf = "";//так, чтоб посмотреть
        par_print.flags_minmax.dwFlags = FL_GRF_MINMAX;
		par_print.flags_minmax.bitFlags.fl_buf_x_only = 0;
        pList->ChangeScale(NULL, par_print, GetActiveData()->flag_time_glub);
    }
    UpdateActiveView();
}

void CMainView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	SetParamPrinting(pDC);
	tm_print = COleDateTime::GetCurrentTime();
}

void CMainView::OnEndPrinting(CDC* pDC, CPrintInfo* /*pInfo*/)
{
    int cr_lst = m_parOfView->GetNumActiveList(); 
    ParamOfView *pList = m_parOfView->GetList(cr_lst);

    if(pList == NULL) 
        return;

    pList->RecalcFieldSize(type_view);
    RecalcScaleFactor(cr_lst);
    SetParXScale(cr_lst);

    pList->ShowActiveList(true);
    CheckRegim();
    CheckGrf();
}

void CMainView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	SetParamPrinting(pDC);
    cur_list = pInfo->m_nCurPage;
    cur_list--;
	OnDraw(pDC);                    // Call Draw
}
void CMainView::PrintHeaderPage(CDC* pDC, int n_list) 
{
    CRect rec;
    GetClientRect(&rec); 
    CFont *pFont = NULL;
//.................................................................................	
    rec.OffsetRect(m_printShift.cx, m_printShift.cy/5);

    LOGFONT lf = {
        -24, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

	if(GetOrient())
	{
		lf.lfHeight = (LONG)(lf.lfHeight * m_printScaleY/2);
		lf.lfWidth = (LONG)(lf.lfWidth * m_printScaleX/2);
	}
	else
	{
		lf.lfHeight = (LONG)(lf.lfHeight * m_printScaleY/1.5);
		lf.lfWidth = (LONG)(lf.lfWidth * m_printScaleX/1.5);
	}
    CFont fnt;
    fnt.CreateFontIndirect(&lf);
    pFont = pDC->SelectObject(&fnt);
    pDC->SetTextColor(RGB(100,100,100));


    CSize sz = CScale::GetSizeText(pDC, "PPPPPPPP", &fnt, true);//надо взять размер по Y
    pDC->SetBkMode(TRANSPARENT);

    CString str = GetStrDataTime(tm_print);
    CString sEtap;
    STAGES_Stage* stage = STAGES_GetStage(n_list+1);
	if(type_view == gt_cement && (stage == NULL || num_lists == 0)) return;

    sEtap.Format("Этап %d. \"%s\"", n_list+1, stage->m_name);

    pDC->TextOut(rec.left, rec.top, "Дата : " + str);
    pDC->TextOut(rec.left, rec.top + sz.cy, "Месторождение : " + gMestorogdenie);
    pDC->TextOut(rec.left, rec.top + 2*sz.cy, "Куст : " + gKust);
    pDC->TextOut(rec.left, rec.top + 3*sz.cy, "Скважина : " + gSkvagina);
    pDC->TextOut(rec.left, rec.top + 4*sz.cy, "Работа : " + gRabota);

    str = "За период: с " +
        GetStrDataTime((type_view == gt_cement)?stage->m_start_time:par_print.Fmin) + " по " +
        GetStrDataTime((type_view == gt_cement)?stage->m_start_time + stage->m_duration:par_print.Fmax);
    pDC->TextOut(rec.left, rec.top + 5*sz.cy, str);
    pDC->TextOut(rec.left, rec.top + 5*sz.cy, str);
    if(type_view == gt_cement) pDC->TextOut(rec.left, rec.top + 6*sz.cy, sEtap);

    if(pFont) pDC->SelectObject(pFont);

    m_printShift.cy += 3*sz.cy;
    m_printScaleY *= 0.95f; 
}
void CMainView::OnDraw(CDC* pDC) 
{
	if(GetActiveData() == NULL) return;
	ASSERT(pFrm);
	CRect rec;

	if(pDC->IsPrinting())
	{
//печать
        ParamOfView *pList;
        int cr_lst = 0;

        if(type_view == gt_cement) cr_lst = cur_list;
        else cr_lst = m_parOfView->GetNumActiveList(); 

        pList = m_parOfView->GetList(cr_lst);

        if(pList == NULL) 
            return;

        PrepareListToPrint(pDC, cr_lst);
        PrintHeaderPage(pDC, cr_lst);
        pList->RecalcFieldSize(type_view);
        pList->DrawGrf(pDC, this, m_printScaleX, m_printScaleY, &m_printShift);
        pList->LockDataForPrint(2);
        RestoreParamList(pDC, cr_lst);
    }
	else
	{
		GetClientRect(&rec); 
		pDC->FillSolidRect(rec.left, rec.top + 100, rec.Width(), 2, RGB(0,0,255));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMainView diagnostics

LRESULT CMainView::OnUpdateView(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return 0;
	pList->UpdateReper(false);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CMainView message handlers
LRESULT CMainView::OnChangeParamProperty(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return 0;
	pList->ShowActiveList(false);

	((CPanelWnd*)lParam)->DeleteParam(wParam);

	pList->RecalcFieldSize(type_view);
	pList->ShowActiveList(true);

	return true;
}

LRESULT CMainView::OnChangeParamProperty_1(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	ParamOfView *pList = GetActiveList();
	if (pList == NULL) return 0;
	pList->ShowActiveList(false);

	CDgtHeadParam* p_dgt = (CDgtHeadParam*)lParam;
	if(p_dgt != NULL) p_dgt->SetFontDgt();

	pList->RecalcFieldSize(type_view);
	pList->ShowActiveList(true);

	return true;
}

LRESULT CMainView::OnChangeNamePanel(WPARAM wParam, LPARAM lParam)
{
	//	SaveAllLists();
	return true;
}

double CMainView::GetCurTime()
{
	return m_startTime.m_dt + TicksToOleTime(GetTickCount() - m_startTick);
}
LRESULT CMainView::OnSaveAllList(WPARAM wParam, LPARAM lParam)
{
	if(m_parOfView != NULL)
	{
		ParamOfView *pList;
		//SaveAllLists();
		for(int i = 0; i < m_parOfView->num_lists; i++) 
		{
			pList = m_parOfView->GetList(i);
			if(pList == NULL) continue;
			pList->DeleteAllPanel();
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
//ОТОБРАЖЕНИЯ ГРАФИЧЕСКИХ И ЦИФРОВЫХ ПАНЕЛЕЙ
//////////////////////////////////////////////////////////////////////////
//НАЧАЛЬНЫЕ УСТАНОВКИ

bool CMainView::GetParamFromRestore(int num, PanelDescription *ppanel_descr, ParamInPanel &param_in_panel)
{
	if (num > (int)ppanel_descr->m_params.size())
		return false;
	list<ParamInPanel>::iterator current = ppanel_descr->m_params.begin();
	for (int i = 0; i < num; i++)
		current++;
	param_in_panel = *current;
	return true;
}
//========================================================================
//УСТАНОВКА НОМЕРА ОТОБРАЖАЕМОГО ЛИСТА
LRESULT CMainView::OnChangeaActiveList(WPARAM wParam, LPARAM lParam)
{
	if (KRS_start_counter != 100)
		return true;
	ChangeaActiveListA(wParam, lParam);
	return true;
}

void CMainView::SelectRegime()
{
	if(GetActiveData() == NULL) return;
	if(m_parOfView->flag_time_glub) pBufX = (LPVOID)m_ptmData; 
	else pBufX = (LPVOID)m_pcurGlub; 
}

ParamOfView* CMainView::GetActiveList()
{
	return m_parOfView->GetActive();
}

CDataView* CMainView::GetActiveData()
{
	return m_parOfView;
}

LPVOID CMainView::Get_pBufX()
{
	if(GetActiveData() == NULL) return 0;
	if(GetActiveData()->flag_time_glub == 1) return (LPVOID)m_ptmData;
	else return (LPVOID)m_pcurGlub;
	return NULL;
}
//------------------------------------------------------------------------
//ИЗМЕНЕНИЕ ЧИСЛА ГРАФИЧЕСКИХ ПАНЕЛЕЙ НА ЛИСТЕ
LRESULT CMainView::OnChangeaNumGrf(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	ParamOfView *pList = GetActiveList();
	if (pList == NULL) return 0;

	int num_panel = int(wParam);
	if((num_panel >= 0) && (num_panel < GetActiveList()->num_grf_panel)) 
	{
		pList->ShowActiveList(false);
		pList->m_active_grf[wParam] = int(lParam);
		pList->SetVisibilityFlagAll();
		pList->RecalcFieldSize(type_view);
		pList->ShowActiveList(true);
	}
	return true;
}
//========================================================================
//УСТАНОВКА ТИПА ГРАФИЧЕСКИХ ПАНЕЛЕЙ НА ЛИСТЕ - "ВРЕМЯ" ИЛИ "ГЛУБИНА" 
LRESULT CMainView::OnChangeTypeScale(WPARAM wParam, LPARAM lParam)
{
	ASSERT(pFrm);
	if(lParam != 0) return true;//иначе зациклится

	CDataView *pD = GetActiveData();
	if(pD->flag_time_glub == BOOL(wParam)) return false;
	pD->flag_time_glub = BOOL(wParam);
	GetActiveList()->ShowActiveList(false);
	m_wndDialogBarDB.SetCheckControl(!pD->flag_time_glub);
	CheckRegim();
	SelectRegime();
	SetParXScale(pD->GetNumActiveList(), true);
	GetActiveList()->RecalcFieldSize(type_view);
	GetActiveList()->ShowActiveList(true);
	return true;
}
//========================================================================
//УСТАНОВКА МАСШТАБА ОТОБРАЖЕНИЯ НА ГРАФИЧЕСКИХ ПАНЕЛЯХ ЛИСТА
LRESULT CMainView::OnRButtonDown1(WPARAM wParam, LPARAM lParam)
{
	RButtonDown1A(wParam, lParam);
	return true;
}

void CMainView::OnContMenuScale5()		{SetScaleFactorActiveList(5.0);}
void CMainView::OnContMenuScale10()		{SetScaleFactorActiveList(10.0);}
void CMainView::OnContMenuScale20()		{SetScaleFactorActiveList(20.0);}
void CMainView::OnContMenuScale60()		{SetScaleFactorActiveList(60.0);}
void CMainView::OnContMenuScale120()	{SetScaleFactorActiveList(120.0);}
void CMainView::OnContMenuScale240()	{SetScaleFactorActiveList(240.0);}
void CMainView::OnContMenuScale360()	{SetScaleFactorActiveList(360.0);}
void CMainView::OnContMenuScale720()	{SetScaleFactorActiveList(720.0);}
void CMainView::OnContMenuScale1440()	{SetScaleFactorActiveList(1440.0);}
void CMainView::OnContMenuScale2880()	{SetScaleFactorActiveList(2880.0);}
void CMainView::OnMenuScaleTest()		{SetScaleFactorActiveList(18000.0);}
//========================================================================
void CMainView::OnContMenuScaleG10()	{SetScaleFactorActiveList(10.0);}
void CMainView::OnContMenuScaleG20()	{SetScaleFactorActiveList(20.0);}
void CMainView::OnContMenuScaleG50()	{SetScaleFactorActiveList(50.0);}
void CMainView::OnContMenuScaleG100()	{SetScaleFactorActiveList(100.0);}
void CMainView::OnContMenuScaleG200()	{SetScaleFactorActiveList(200.0);}
void CMainView::OnContMenuScaleG500()	{SetScaleFactorActiveList(500.0);}
void CMainView::OnContMenuScaleG1000()	{SetScaleFactorActiveList(1000.0);}
void CMainView::OnContMenuScaleG2000()	{SetScaleFactorActiveList(2000.0);}
void CMainView::OnContMenuScaleG5000()	{SetScaleFactorActiveList(5000.0);}
void CMainView::OnMenuScaleG10000()		{SetScaleFactorActiveList(10000.0);}

void CMainView::SetScaleFactor(int num_lists, float fscale, BOOL flRedraw) 
{
	if(GetActiveData() == NULL) return;
	ParamOfView *pList = GetActiveData()->GetList(num_lists);
	if (pList == NULL) return;

	if(GetActiveData()->flag_time_glub) pList->gTimeScale = fscale;
	else pList->gGlubScale = fscale;
	pList->flg_nonstandart_scale = false;
	RecalcScaleFactor(num_lists);
	UpdateActiveView(flRedraw);
}
void CMainView::SetScaleFactorActiveList(float fscale, BOOL flRedraw) 
{
    if(GetActiveData() == NULL) return;
    ParamOfView *pList = GetActiveList();
    if (pList == NULL) return;

    if(GetActiveData()->flag_time_glub) pList->gTimeScale = fscale;
    else pList->gGlubScale = fscale;
    pList->flg_nonstandart_scale = false;
    RecalcScaleFactor(GetActiveData()->GetNumActiveList());
    UpdateActiveView(flRedraw);
}
void CMainView::RecalcLenX(CDC *pDC, int num_lists, float scale)
{
	if(GetActiveData() == NULL) return;
	ParamOfView *pList = GetActiveData()->GetList(num_lists);
	if (pList == NULL) return;
	CRect rec;
	GetClientRect(&rec);
	if(GetActiveData()->flag_time_glub)
	{
		pList->gDeltaTime = GetScaleFactorT(pDC, scale)*rec.Height();
		pList->gTimeScale = scale;
	}
	else
	{
		pList->gDeltaH = GetScaleFactorG(pDC, scale)*rec.Height();
		pList->gGlubScale = scale;
	}
}

void CMainView::RecalcLenX(int cy)
{
	if(GetActiveData() == NULL || cy <= 0) return;
    int n_list = m_parOfView->GetNumActiveList();
	ParamOfView *pList = GetActiveData()->GetList(n_list);
	if (pList == NULL) return;

    CClientDC dc(this);
	if(GetActiveData()->flag_time_glub)
	{
		pList->gDeltaTime = GetScaleFactorT(&dc, pList->gTimeScale)*cy;
	}
	else
	{
		pList->gDeltaH = GetScaleFactorG(&dc, pList->gGlubScale)*cy;
	}

//	pList->ShowPanelAll(false);
    pList->endTime = pList->begTime + MinuteToOleTime((float)pList->gDeltaTime);
    SetParXScale(n_list, true);
//	pList->RecalcFieldSize(type_view);
//	pList->ShowPanelAll(true);

}

void CMainView::RecalcScaleFactor(int num_list)
{
    if(GetActiveData() == NULL) return;
    CClientDC dc(this);
    if(!pFrm) return;
    ParamOfView *pList = GetActiveData()->GetList(num_list);
    if (pList == NULL) return;
/*
    float ScaleFactorT = GetScaleFactorT(&dc, pList->gTimeScale);
    float ScaleFactorG = GetScaleFactorG(&dc, pList->gGlubScale);

CRect rec;
    GetClientRect(&rec);
*/
/*
    pList->gDeltaTime = ScaleFactorT*rec.Height();
    pList->gDeltaH = ScaleFactorG*rec.Height();
*/
	RecalcLenX(pList->GetLengthFieldGrf());
//	RecalcLenX(rec.Height());
}

void CMainView::UpdateActiveView(BOOL flRedraw)
{
    if(GetActiveData() == NULL) return;
	SetParXScale(GetActiveData()->GetNumActiveList());
	if(flRedraw) Invalidate(false);
}
void CMainView::SetParXScale(int num_list, BOOL flg)
{
	//SetStartX(0, num_list, flg);
	SetStartX(GetStartX(num_list), num_list, flg);
}
double CMainView::GetLenX(int num_list)
{
	if(GetActiveData() == NULL) return 0;
	float sdX;
    ParamOfView *pList = GetActiveData()->GetList(num_list);
    if (pList == NULL) return 0;

	if(!GetActiveData()->flag_time_glub) return pList->gDeltaH;  //метры

	sdX = float(pList->gDeltaTime);  //минуы

	return MinuteToOleTime(sdX);
}
CString CMainView::GetStrScale(int nn)
{
	CString cs;
	cs.Format("%d", nn);
	return ("1:" + cs);
}
void CMainView::SetTextGrfPanel()
{
	if(GetActiveData() == NULL) return;
	int nn;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return;

	if(GetActiveData()->flag_time_glub) nn = (int)pList->gTimeScale;
	else nn = (int)pList->gGlubScale;

	pList->SetTextAllGrfPanel(GetStrScale(nn));
}
//========================================================================
//ИЗМЕНЕНИЕ ЧИСЛА ГРАФИЧЕСКИХ ПАРАМЕТРОВ НА ГРАФИЧЕСКОЙ ПАНЕЛИ

#include "TextMenu.h"
#include "MainView.h"

struct ParamsMenu : TextMenu
{
	ParamsMenu():
		TextMenu(20, 6, TextMenu_Left, 3), m_message(0), m_num_panel(0), m_window(NULL)
	{
	}
	CWnd* m_window;
	UINT m_message;
	int m_num_panel;
	
	void Show(CPoint point, int hor_align, int vert_align, CWnd* wnd, UINT message, int num_panel = 0)
	{
		m_window = wnd;
		m_num_panel = num_panel;
		m_message = message;
		TextMenu::Show(point, hor_align, vert_align, m_window);
	}
	
	virtual void OnSelect(int pos, DWORD data, CString str)
	{
		m_window->PostMessage(m_message, data, m_num_panel);
	}
};

static ParamsMenu g_params_menu;


LRESULT CMainView::OnAddParam2(WPARAM wParam, LPARAM lParam)
{
	CParam* param;
	ParamOfView *pList = GetActiveList();
	if (pList == NULL) return -1;
	if (!pList->panArray[g_params_menu.m_num_panel]->m_MapParam->Lookup(wParam, param) || param == NULL)
		return -1;

	pList->ShowActiveList(false);

	ParamInPanel param_descr;
	param_descr.m_param_num = param->m_num_par;
	param_descr.m_min_val = param->fGraphMIN;
	param_descr.m_max_val = param->fGraphMAX;
	param_descr.m_line_width = param->m_line_width;
	param_descr.m_color = param->m_color;
	param_descr.text_color = 0;
	param_descr.text_size = -8;
	param_descr.font_weight = 400;

	int numPositionInPanel = GetActiveList()->AddParam(g_params_menu.m_num_panel, &param_descr, true);

	pList->RecalcFieldSize(type_view);
	pList->ShowActiveList(true);

	((CKRSApp*)AfxGetApp())->SaveParamFromList(
		GetActiveData()->GetNumActiveList(),
		g_params_menu.m_num_panel, 
		param_descr,
		numPositionInPanel,
		type_view);

	return -1;
}

LRESULT CMainView::OnAddParam(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return 0;
	int num_panel = ((int)wParam) >> 16;
	int nn = pList->panArray[num_panel]->m_ParamArray.GetCount();
	if (num_panel < G_MAX_FIELDS_GRAPHS && nn > 4) return false;//не больше 5 параметров на графическую панель

	g_params_menu.Clear();

	CString str;
	CParam *param;
	int count = 0, max_count = m_MapParamCurent->GetCount();
	for (int i = 1; i < 10000; i++)
	{
		if (m_MapParamCurent->Lookup(i, param))
		{
			if (param != NULL)
			{
				str.Format("%03d  %s", param->m_num_par, param->sName);
				g_params_menu.Add(str, i);
			}
			count++;
			if (count == max_count)
				break;
		}
	}
	if (g_params_menu.GetCount() > 0)
	{
		g_params_menu.Show(CPoint(lParam>>16, lParam&0xFFFF), 0, +1, this, PM_ADD_PARAM2, ((int)wParam) >> 16);
	}
	
/*	CDlgAddParam m_dlgAddParam(lParam);

	int num_panel = ((int)wParam) >> 16;
	int pos_in_panel = ((int)wParam) & 0xFFFF;
	if (pos_in_panel == 0xFFFF)
		pos_in_panel = -1;

	if (m_dlgAddParam.DoModal() == IDOK) 
	{
		CParam* param;
		ParamOfView *pList = GetActiveList();
		if(pList == NULL) return 0;
		if (!pList->panArray[num_panel]->m_MapParam->Lookup(m_dlgAddParam.m_key, param) || param == NULL)
			return FALSE;

		pList->ShowActiveList(false);

		ParamInPanel param_descr;
		param_descr.m_param_num = param->m_num_par;
		param_descr.m_min_val = param->fGraphMIN;
		param_descr.m_max_val = param->fGraphMAX;
		param_descr.m_line_width = param->m_line_width;
		param_descr.m_color = param->m_color;
		param_descr.text_color = 0;
		param_descr.text_size = -8;
		param_descr.font_weight = 400;

		int numPositionInPanel = GetActiveList()->AddParam(num_panel, &param_descr, true);

		pList->RecalcFieldSize(type_view);
		pList->ShowActiveList(true);

		((CKRSApp*)AfxGetApp())->SaveParamFromList(
			GetActiveData()->GetNumActiveList(), 
			num_panel, 
			param_descr,
			numPositionInPanel,
			type_view);
	}
	else
	{
		SaveAllLists();
	}
*/	
	return true;
}
//========================================================================
//ПЕРЕПАСЧИТАТЬ РАЗМЕР ВСЕХ ГРАФИЧЕСИХ И ЦИФРОВЫХ ПАНЕЛЕЙ АКТИВНОГ ЛИСТА
LRESULT CMainView::OnRecalcSize(WPARAM wParam, LPARAM lParam)
{
	GetActiveList()->RecalcFieldSize(type_view);
	GetActiveList()->ShowActiveList(true);
	return true;
}
//========================================================================
//ПЕРЕПАСЧИТАТЬ РАЗМЕР ЦИФРОВЫХ ПАНЕЛЕЙ АКТИВНОГО ЛИСТА
LRESULT CMainView::OnRecalcSizeDgtPanels(WPARAM wParam, LPARAM lParam)
{
	GetActiveList()->RecalcPanelsDgt(type_view);
	GetActiveList()->ShowActiveList(true);
	return true;
}

//========================================================================
LRESULT CMainView::OnDeleteExtParam(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	ParamOfView *pList, *pActiveList = GetActiveList();
	if(pActiveList == NULL) return 0;
	pActiveList->ShowActiveList(false);
	for(int i = 0; i < m_parOfView->num_lists; i++) 
	{
		if((pList = m_parOfView->GetList(i)) == NULL) continue;
		pList->DeleteExtParam(wParam);
	}
	pActiveList->RecalcFieldSize(type_view);
	pActiveList->ShowActiveList(true);
	return +1;
}

LRESULT CMainView::OnDeleteParamFromList(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	pApp->DeleteParamFromList(GetActiveData()->GetNumActiveList(), lParam, wParam, type_view);
	return true;
}
LRESULT CMainView::OnSetStyleAllGrf(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	ULONG64 style = (ULONG64(wParam)<<32)|ULONG64(lParam);
	ParamOfView* pList;
	for(int i = 0; i < m_parOfView->num_lists; i++) 
	{
		if((pList = m_parOfView->GetList(i)) == NULL) continue;
		pList->SetStyleGrf(style);
	}
	Invalidate(false);
	return true;
}
LRESULT CMainView::OnResetStyleAllGrf(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	ULONG64 style = (ULONG64(wParam)<<32)|ULONG64(lParam);
	ParamOfView* pList;
	for(int i = 0; i < m_parOfView->num_lists; i++) 
	{
		if((pList = m_parOfView->GetList(i)) == NULL) continue;
		pList->ResetStyleGrf(style);
	}
	Invalidate(false);
	return true;
}
LRESULT CMainView::OnSetStyleActiveList(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
    ULONG64 style = (ULONG64(wParam)<<32)|ULONG64(lParam);
	ParamOfView* pList = GetActiveList();
	if(pList == NULL) return 0;
	pList->SetStyleGrf(style);
    Invalidate(false);
    return true;
}
LRESULT CMainView::OnResetStyleActiveList(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
    ULONG64 style = (ULONG64(wParam)<<32)|ULONG64(lParam);
	ParamOfView* pList = GetActiveList();
	if(pList == NULL) return 0;
    pList->ResetStyleGrf(style);
    Invalidate(false);
    return true;
}
LRESULT CMainView::OnPanelGrf(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	ParamOfView* pList = GetActiveList();
	if(pList == NULL) return 0;
	pList->SubstituteBitStyleGrf(wParam, SCLY_VIEW_DGT, lParam, true);
	return true;
}
LRESULT CMainView::OnChangeRangeGrf(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	CClientDC *pDC = new CClientDC(this);
	if((int)wParam < 0 || (int)wParam >= GetActiveList()->num_grf_panel) return false;

	ParamOfView* pList = GetActiveList();
	if(pList == NULL) return 0;

	startMarker1RT = pList->GetStartMarkerX(wParam);

	pList->UpdateRectRangeAll(wParam);

	int nn;
	if(GetActiveData()->flag_time_glub) nn = GetScaleT(pDC, lParam);
	else nn = GetScaleG(pDC, lParam);
	CString str = GetStrScale(nn);
	pList->SetTextAllGrfPanel(str);
	RecalcLenX(pDC, GetActiveData()->GetNumActiveList(), float(nn));
	delete pDC;
	pList->flg_nonstandart_scale = true;
	return true;
}

LRESULT CMainView::OnChangeScrollGrf(WPARAM wParam, LPARAM lParam)
{
	if(GetActiveData() == NULL) return 0;
	if(gflgScrollAll) 
	{
		ParamOfView* pList = GetActiveList();
		if(pList == NULL) return 0;
		pList->UpdateScrollRangeAll(wParam, lParam);
	}
	return true;
}
void CMainView::OnContMenuMarker() 
{
	// TODO: Add your command handler code here

}
void CMainView::RecalcActiveLilst()
{
	if(GetActiveData() == NULL) return;
	ParamOfView *pList = GetActiveList();
	if(pList == NULL) return;
	pList->ShowActiveList(false);
	pList->RecalcFieldSize(type_view);
	pList->ShowActiveList(true);
}

LRESULT CMainView::OnRecalcActiveLilst(WPARAM, LPARAM)
{
	RecalcActiveLilst();
	return true;
}

void CMainView::Serialize(CArchive& ar)
{
	if(GetActiveData() == NULL) return;
	ParamOfView *pList;
	if (ar.IsStoring())
	{	// storing code
		ar << m_parOfView->GetNumActiveList();
		for(int i = 0; i < num_lists; i++)
		{
			if((pList = m_parOfView->GetList(i))== NULL) return;
			for(int j = 0; j < num_grf_panel; j++)
			{
				ar << pList->m_active_grf[j];
			}

			ar << pList->gTimeScale;
			ar << pList->gGlubScale;
			ar << pList->gDeltaTime;
			ar << pList->gDeltaH;
			ar << pList->m_flag_orient;
		}
	}
	else
	{	// loading code
		int n_active_list;
		ar >> n_active_list; 
//        InitDataView(num_lists, num_grf_panel, num_dgt_panel);
		m_parOfView->SetNumActiveList(n_active_list);
		for(int i = 0; i < num_lists; i++)
		{
			if((pList = m_parOfView->GetList(i))== NULL) return;
			for(int j = 0; j < num_grf_panel; j++)
			{
				ar >> pList->m_active_grf[j];
			}

			ar >> pList->gTimeScale;
			ar >> pList->gGlubScale;
			ar >> pList->gDeltaTime;
			ar >> pList->gDeltaH;
			ar >> pList->m_flag_orient;
		}
	}
}

//Чистим все буферы
void CMainView::ClearAllParamsBuffers(CParamMap *mapParam, FRingBuf<double> *ptmData, FRingBuf<double> *pcurGlub)
{
	if(ptmData) 
	{
		ptmData->Clear0();
		ptmData->AddFragment(0);
	}
	if(pcurGlub) 
	{
		pcurGlub->Clear0();
		pcurGlub->AddFragment(0);
	}

	int mKey;
	CParam *pPar;
	POSITION pos = mapParam->GetStartPosition();
	while (pos != NULL)
	{
		mapParam->GetNextAssoc(pos, mKey, pPar);
		if (!pPar) continue;
		if (!pPar->pBufParam)
			continue;
		pPar->m_index_of_last_measurement = -1;
		pPar->pBufParam->Clear0();
		pPar->pBufParam->AddFragment(0);
	}
}

void CMainView::SetColorInterface(MColorInterface& color_interface)
{
	ParamOfView *pList;
	for(int i = 0; i < m_parOfView->num_lists; i++) 
    {
		pList = m_parOfView->GetList(i);
		if (pList == NULL)
			continue;
		pList->ChangeGraphColor();
	}
	
	
	Invalidate(true);
	m_wndDialogBar.Invalidate(true);
}
