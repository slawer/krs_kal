#include "stdafx.h"

#include "KRS.h"
#include "Func.h"

#include <math.h>
#include <mmsystem.h>
#include "MainFrm.h"

#include "SERV.h"
#include "CONV.h"
#include "DB.h"
#include "LOG_V0.h"

#include <afxdisp.h>
#include "excel.h"

#include "InitProperty.h"
#include "DlgNet.h"
#include "ParamDB.h"
#include "MapParamDlg.h"

#include "DlgKalibrovka.h"
#include "DlgSelectChannel.h"
#include "DlgSelectMarkerType.h"
#include "DlgConfigDevices.h"
#include "DlgDebagParam.h"
#include "DlgSettingsBO.h"
#include "DlgStageSettings.h"
#include "DlgWaitDB.h"

#include "CementView.h"
#include "KRSDoc.h"
#include "DBDoc.h"
#include "KRSView.h"
#include "DBView.h"
#include "STAGES.h"

#include "DlgDevicesMemory.h"
#include "DlgWork.h"
#include "DlgStagesLibrary.h"
#include "MainView.h"

#include "DlgWaiting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_Timer3 40

int iteration = 0;

static DWORD g_start_tick = GetTickCount();

int flVes = 2, flDavl = 2, flDol = 2, flShurf = 2, flVesSave = 0, flGlub = 0;
int prevflVes = 2, prevflDavl = 2, prevflDol = 2, prevflShurf = 2;
DWORD ttSave;

BOOL flgSPO, flagVesSniat, flagShurf; 
float saveTalblock1, saveTalblock2, saveVes, deltaTalBlock;
BOOL flagKBK_plus = false, flagKBK_minus = false, flagHighPoint = false, flShurfSave = false;

int stepChangeKBK;

BOOL knWes, flZaboi, knZahod, knKNBLplus, knKNBLminus;

BS_Measurement g_measurement_to_send;
int g_measurement_to_send_real_size = 0;

CParam** g_quasi_glubina = NULL;

CString sKNBK;
extern UINT startMarker1RT;
double TIME_current_time = 0;

BOOL flgRabotaBezShurfaChanged = 0;
BOOL flg_GlI_Changed = 0;;
BOOL flg_GlX_Changed = 0;
float gGlI = 0;
float gGlX = 0;
BOOL  g_flgRabotaBezShurfa = 0;

float fixGlub = 0;
BOOL flMehInterval = false;


IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

#define WM_START_EXIT (WM_USER + 1)
//#define WM_PARAM_LIST_READY (WM_USER + 2)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_SCALE_TIM, OnRadioScaleTim)
	ON_BN_CLICKED(IDC_RADIO_SCALE_GLUB, OnRadioScaleGlub)
	ON_BN_CLICKED(IDC_BUTTON_GET_PARAM, OnButtonGetParam)
	ON_COMMAND(ID_START_PROPERTY, OnStartProperty)
	ON_COMMAND(ID_EDIT_PROJECT, OnEditProject)
	ON_COMMAND(ID_OPEN_WORK, OnOpenWork)
	ON_COMMAND(ID_NEW_WORK, OnNewWork)
	ON_COMMAND(ID_IMPORT_WORK, OnImportWork)
	ON_COMMAND(ID_MENU_PROP_DB, OnMenuPropDb)
	ON_COMMAND(ID_MENU_TCPIT, OnMenuTcpit)
	ON_UPDATE_COMMAND_UI(ID_START_PROPERTY, OnUpdateStartProperty)
    ON_UPDATE_COMMAND_UI(ID_MENU_PROP_DB, OnUpdateMenuPropDb)

	ON_MESSAGE(PM_DATAACPFRM, OnReadDataMessage)
	ON_MESSAGE(WM_ON_CONV_LIST_READY, OnListReadyCONV)
	ON_MESSAGE(WM_ERROR_CONV, OnErrorCONV)
	ON_MESSAGE(WM_INITED_CONV, OnInitedCONV)
	ON_MESSAGE(WM_DATA_SERV, OnDataSERV)
	ON_MESSAGE(WM_ERROR_SERV, OnErrorSERV)
	ON_MESSAGE(WM_INITED_SERV, OnInitedSERV)
    ON_MESSAGE(PM_MAXIMIZE_FRAME, OnMaximizeFrame)
	ON_MESSAGE(PM_CHANGE_PARAM_PROPERT, OnChangeParamProperty)
	ON_MESSAGE(PM_ADD_PARAM, OnAddParam)
	ON_MESSAGE(PM_CHANGE_TYPE_VIEW, OnChangeTypeView)

// -- Menu ЭТАПЫ >>
	ON_UPDATE_COMMAND_UI(ID_STAGES_LOAD, CDialogBarCM::OnUpdateLoad)
	ON_COMMAND(ID_STAGES_LOAD, OnStagesLoad)
	ON_COMMAND(ID_STAGES_SAVE, OnStagesSave)
	ON_COMMAND(ID_STAGES_LIB, OnStagesLib)
		
	ON_UPDATE_COMMAND_UI(ID_STAGES_START, CDialogBarCM::OnUpdateButtonStartPauseStage)
	ON_UPDATE_COMMAND_UI(ID_STAGES_FINISH, CDialogBarCM::OnUpdateButtonFinishStage)
	ON_UPDATE_COMMAND_UI(ID_STAGES_FINISH_ALL, CDialogBarCM::OnUpdateMenuBreakFinishAll)
	ON_UPDATE_COMMAND_UI(ID_STAGES_RESTART, CDialogBarCM::OnUpdateButtonRestartStage)
	ON_UPDATE_COMMAND_UI(ID_STAGES_RESTART_ALL, CDialogBarCM::OnUpdateMenuBreakFinishAll)
	ON_COMMAND(ID_STAGES_START, CDialogBarCM::OnButtonStartPauseStage)
	ON_COMMAND(ID_STAGES_FINISH, CDialogBarCM::OnButtonFinishStage)
	ON_COMMAND(ID_STAGES_FINISH_ALL, CDialogBarCM::OnButtonFinishAllStages)
	ON_COMMAND(ID_STAGES_RESTART, CDialogBarCM::OnButtonRestartStage)
	ON_COMMAND(ID_STAGES_RESTART_ALL, CDialogBarCM::OnButtonRestartAllStages)

	ON_UPDATE_COMMAND_UI(ID_STAGES_GET_FROM_DB, CDialogBarCM::OnUpdateUseAsTemplate)
	ON_COMMAND(ID_STAGES_GET_FROM_DB, OnStagesGetDB)
	
	ON_UPDATE_COMMAND_UI(ID_MENU_TCPIT, OnUpdateMenuTcpit)
	ON_COMMAND(ID_KALIBROVKA, OnKalibrovka)
    ON_UPDATE_COMMAND_UI(ID_KALIBROVKA, OnUpdateMenuKalibrovka)
	ON_COMMAND(ID_KALIBROVKA_M, OnKalibrovkaM)
    ON_UPDATE_COMMAND_UI(ID_KALIBROVKA_M, OnUpdateMenuKalibrovkaM)
	ON_COMMAND(ID_PARAMETRY, OnParamatry)
    ON_UPDATE_COMMAND_UI(ID_PARAMETRY, OnUpdateMenuParamatry)
	ON_COMMAND(ID_UPLOAD_MEM, OnUploadMem)
	ON_COMMAND(ID_DOWNLOAD_MEM, OnDownloadMem)
    ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_MEM, OnUpdateMenuDownloadMem)

	ON_COMMAND(ID_STAGES_CHOOSE_FONT, OnStagesChooseFont)
	ON_COMMAND(ID_STAGES_CHOOSE_FONT_SEL, OnStagesChooseFontSel)

	ON_BN_CLICKED(IDC_STATIC_Wk, OnWk)
	ON_COMMAND(ID_MARKER_TYPES, OnMarkerTypes)
	ON_BN_CLICKED(IDC_CHECK_NGR1, OnCheckNgr1)
	ON_BN_CLICKED(IDC_CHECK_NGR2, OnCheckNgr2)
	ON_BN_CLICKED(IDC_CHECK_NGR3, OnCheckNgr3)
	ON_COMMAND(ID_CONFIG_DEVICES, OnConfigDevices)
    ON_UPDATE_COMMAND_UI(ID_CONFIG_DEVICES, OnUpdateMenuConfigDevices)
	ON_COMMAND(ID_DEBUG_INFO, OnDebugInfo)
	ON_COMMAND(ID_BLOCK_OTOBRAZHENIYA, OnSettingsBO)
    ON_UPDATE_COMMAND_UI(ID_BLOCK_OTOBRAZHENIYA, OnUpdateMenuSettingsBO)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
    ON_WM_ACTIVATE()
	ON_UPDATE_COMMAND_UI(ID_MARKER_TYPES, &CMainFrame::OnUpdateMarkerTypes)
	ON_COMMAND(ID_MENU_OPISANIE_MESTOROGDENIJA, &CMainFrame::OnMenuOpisanieMestorogdenija)
	ON_MESSAGE(WM_START_EXIT, &CMainFrame::OnStartMessage)
	ON_MESSAGE(WM_PARAM_LIST_READY, &CMainFrame::OnListReadyDB)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_DEBUG_INFO, &CMainFrame::OnUpdateDebugInfo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROJECT, &CMainFrame::OnUpdateEditProject)
	ON_UPDATE_COMMAND_UI(ID_OPEN_WORK, &CMainFrame::OnUpdateOpenWork)
	ON_UPDATE_COMMAND_UI(ID_NEW_WORK, &CMainFrame::OnUpdateNewWork)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_WORK, &CMainFrame::OnUpdateImportWork)
	ON_UPDATE_COMMAND_UI(ID_MENU_OPISANIE_MESTOROGDENIJA, &CMainFrame::OnUpdateMenuOpisanieMestorogdenija)
	ON_UPDATE_COMMAND_UI(ID_UPLOAD_MEM, &CMainFrame::OnUpdateUploadMem)
	ON_UPDATE_COMMAND_UI(ID_NASTROIKA_MODULES, &CMainFrame::OnUpdateNastroikaModules)
END_MESSAGE_MAP()

CMainFrame::CMainFrame()
{
    pFrm = this;

	m_lMainLst.RemoveAll();
//-----------------------------------------------------------------
	m_start_time = 0;
	m_start_glub = 0;
	m_start_timeDB = 0;
	m_start_glubDB = 0;

	gDeltaTimeAlert = 0;
	m_bFlagTimeAlert = false;

	m_flaChangeList = false;
	m_flaChangeListDB = false;
	m_flagTimeGlub = false;

//-----------------------------------------------------------------
	m_MapParam.RemoveAll();
	m_type_view = 0;

	gfTalblokDeltaH = 0;

	m_MapParamCurent = &m_MapParam;
	ttSave = GetTickCount();

	flVesSave = 0;

	gflagButtonNull = false;

	gPorogTime = 0.1f;
	gPorogVisota = 0.1f;
	
	gWk = 17;
	gflZaboiIsSetted = false;

    m_CurCursor = LoadCursor(NULL, IDC_ARROW);
    m_CurNorm = LoadCursor(NULL, IDC_ARROW);
    m_CurHSpl = AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR);
    m_CurVSpl = AfxGetApp()->LoadCursor(AFX_IDC_VSPLITBAR);
    m_bFlagDown = false;
    m_CusorSplit = false;
}

CMainFrame::~CMainFrame()
{
}

HANDLE g_stop_timer_event = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE g_timer_stopped_event = CreateEvent(NULL, FALSE, FALSE, NULL);

static UINT TimerThreadProcedure(LPVOID param)
{
	DWORD prev_tick = 0, tick, diff, total_diff = 0, max_diff = 0, num = 0;
	double middle_diff;
	while (true)
	{
		prev_tick = GetTickCount();
		Sleep(100);
		if (WaitForSingleObject(g_stop_timer_event, 0) == WAIT_OBJECT_0)
			break;
		pFrm->OnReadDataMessage(0, 0);
		tick = GetTickCount();
		diff = tick - prev_tick;
		if (diff > max_diff)
			max_diff = diff;
		total_diff += diff;
		num++;
		middle_diff = double(total_diff)/num;
		if (false)
		{
			max_diff = 0;
			total_diff = 0;
			num = 0;
		}
	}
	SetEvent(g_timer_stopped_event);
	return +1;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	if (!m_wndDialogBarH.Create(this, IDD_DIALOG_PANEL_H,WS_CHILD | CBRS_ALIGN_TOP,
								IDD_DIALOG_PANEL_H))
	{
		TRACE("Failed to create DlgBar\n");
		return -1;      // fail to create CDialogBar
	} 

	if (!m_wndDialogBar.Create(this, IDD_DIALOG_PANEL0,WS_CHILD | CBRS_ALIGN_LEFT,
								IDD_DIALOG_PANEL0))
	{
		TRACE("Failed to create DlgBar\n");
		return -1;      // fail to create CDialogBar
	}

	if (!m_wndDialogBarDB.Create(this, IDD_DIALOG_BAR_DB,WS_CHILD | CBRS_ALIGN_LEFT,
								IDD_DIALOG_BAR_DB))
	{
		TRACE("Failed to create DlgBar\n");
		return -1;      // fail to create CDialogBar
	} 

    if (!m_wndDialogBarCM.Create(this, IDD_DIALOG_BAR_CEMENT,WS_CHILD | CBRS_ALIGN_LEFT,
        IDD_DIALOG_BAR_CEMENT))
    {
        TRACE("Failed to create DlgBar\n");
        return -1;      // fail to create CDialogBar
    } 

	if (!m_wndDialogPanelCement.Create(this, IDD_DIALOG_PANEL_CEMENT,WS_CHILD | CBRS_ALIGN_BOTTOM,
		IDD_DIALOG_PANEL_CEMENT))
	{
		TRACE("Failed to create DlgBar\n");
		return -1;      // fail to create CDialogBar
	} 


	if (!m_wndDaligLineParam.Create(this, IDD_DIALOG_LINE_PARAM,WS_CHILD | CBRS_ALIGN_TOP,
								IDD_DIALOG_LINE_PARAM))
	{
		TRACE("Failed to create DlgBar\n");
		return -1;      // fail to create CDialogBar
	} 

	m_wndDialogBarDB.ShowWindow(SW_HIDE);
	m_wndDaligLineParam.ShowWindow(SW_HIDE);
	m_wndDialogPanelCement.ShowWindow(SW_HIDE);
	m_wndDaligLineParam.ShowWindow(SW_HIDE);

	ptmData	= pgtmData;
	pcurGlub = pgcurGlub;
	pBufX = (LPVOID)ptmData; 

    ptmDataDB	= pgtmDataDB;
    pcurGlubDB = pgcurGlubDB;
    pBufXDB = (LPVOID)ptmDataDB; 

	KRS_syncro_ole_time = m_prev_time = COleDateTime::GetCurrentTime();
	KRS_syncro_tick = GetTickCount();

	m_wndDialogBarH.UpdateData(false);
	m_wndDialogBar.m_Regim.SetText("НЕТ ДАННЫХ");

	ptmData->Set_flg_view_reper(gflgViewMarkers);
	pcurGlub->Set_flg_view_reper(gflgViewMarkers);
	m_wndDialogBar.m_IndDataTime.SetStyle(1, RGB(0,255,0), RGB(0,0,255),15);

    SetTimer(ID_Timer3, 10*60*1000, NULL);
    SetTimer(1, 100, NULL);
	SetTimer(2, 10000, NULL); 

    pApp->NewDoc(t_archive);
    pApp->NewDoc(t_cement_rt);
	KRS_CheckFinishInitialization();

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

//	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
//		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;// | WS_MAXIMIZE;
//Maximize	 on start
//    | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
void CMainFrame::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	// storing code
	}
	else
	{	// loading code
	}
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

LRESULT CMainFrame::OnMaximizeFrame(WPARAM wParam, LPARAM lParam) 
{
    ShowWindow(SW_SHOWMAXIMIZED);
    UpdateWindow();
    return true;
}

LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == 162 || message == 163) 
		return 0;
	return CFrameWnd::DefWindowProc(message, wParam, lParam);
}

LRESULT CMainFrame::OnChangeParamProperty(WPARAM wParam, LPARAM lParam)
{
	return true;
}

LRESULT CMainFrame::OnAddParam(WPARAM wParam, LPARAM lParam)
{
	pApp->SendMessageToActiveView(PM_ADD_PARAM, wParam, lParam);
	return true;
}

LRESULT CMainFrame::OnChangeTypeView(WPARAM wParam, LPARAM lParam)
{
	if (m_type_view == int(wParam))
		return 0;

	m_type_view = int(wParam);
    CButton* pbRT = (CButton*)m_wndDialogBarH.GetDlgItem(IDC_RADIO_REAL_TIME);
    CButton* pbAR = (CButton*)m_wndDialogBarH.GetDlgItem(IDC_RADIO_ARCHIVE);
    CButton* pbCM = (CButton*)m_wndDialogBarH.GetDlgItem(IDC_RADIO_CEMENT);
    if(!pbRT || !pbAR || !pbCM) return 0;

	if(m_type_view == 0)
	{
        if(m_wndDialogBar.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBar), gflgViewDgtRT, false);
        if(m_wndDialogBarDB.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBarDB), false, false);
        if(m_wndDialogBarCM.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBarCM), false, false);
        if(m_wndDialogPanelCement.m_hWnd != NULL) ShowControlBar(&(m_wndDialogPanelCement), false, false);
//        if(m_wndDialogBarH.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBarH), gflgIndHighPanel, false);
        SetViewPanelBur(true);
        if(m_wndDialogBarH.m_hWnd != NULL)
        {
            pbRT->SetCheck(1);
            pbAR->SetCheck(0);
            pbCM->SetCheck(0);
        }
        m_MapParamCurent = &m_MapParam;
	}
	else if(m_type_view == 1)
	{
		if(m_wndDialogBarDB.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBarDB), gflgViewDgtBD, false);
        if(m_wndDialogBar.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBar), false, false);
        if(m_wndDialogBarCM.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBarCM), false, false);
        if(m_wndDialogPanelCement.m_hWnd != NULL) ShowControlBar(&(m_wndDialogPanelCement), false, false);
//        if(m_wndDialogBarH.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBarH), gflgIndHighPanel, false);
        if(m_wndDialogBarH.m_hWnd != NULL)
        {
            pbRT->SetCheck(0);
            pbAR->SetCheck(1);
            pbCM->SetCheck(0);
        }
        SetViewPanelBur(false);
//        m_wndDialogBarDB.SetFocus();
		m_MapParamCurent = &m_MapParamDB;
	}
    else if(m_type_view == 2 || m_type_view == 3)
    {
        if(m_wndDialogBarCM.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBarCM), true, false);
        if(m_wndDialogBar.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBar), false, false);
        if(m_wndDialogBarDB.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBarDB), false, false);
        if(m_wndDialogPanelCement.m_hWnd != NULL) ShowControlBar(&(m_wndDialogPanelCement), true, false);
        //        if(m_wndDialogBarH.m_hWnd != NULL) ShowControlBar(&(m_wndDialogBarH), gflgIndHighPanel, false);
        if(m_wndDialogBarH.m_hWnd != NULL)
        {
            pbRT->SetCheck(0);
            pbAR->SetCheck(0);
            pbCM->SetCheck(1);
        }
        SetViewPanelBur(false);
		m_wndDialogBar.SetIndRegimeBur(gflgViewPanelBur);
        //if(m_type_view == 2 || !DB_connection_info.IsReady()) m_MapParamCurent = &m_MapParamCM;
		//else m_MapParamCurent = &m_MapParamDB;
		m_MapParamCurent = &m_MapParamCM;
		STAGES_UpdateMapParam();
    }
	return true;
}

void CMainFrame::SetXBufToParam(CParam *prm)
{
	if(prm != NULL && prm->pBufParam != NULL) prm->pBufParam->pBufBase = ptmData;
}

void CMainFrame::ChangeTypeView(WPARAM wParam, LPARAM lParam)
{
    OnChangeTypeView(wParam, lParam);
}
////////////////////////////////////////////////////////////////////////////////
static bool g_redraw_once = false;
LRESULT CMainFrame::OnReadDataMessage(WPARAM wParam, LPARAM lParam)
{
	if (KRS_start_counter != 100) return false;
    if (gflZaboiIsSetted)
    {
        gflZaboiIsSetted = false;
        if(gIZaboiValue < pGlubinaZaboia->fCurAvg) 
        {
            pZaboi->fCurPrevious = gIZaboiValue;

            ptmData->Clear0();
            ptmData->AddFragment(0);
            pcurGlub->Clear0();
            pcurGlub->AddFragment(0);

            int key;
            CParam *pPar;

            POSITION pos = m_MapParam.GetStartPosition();

            int ii = 0;
            while (pos != NULL)
            {
                m_MapParam.GetNextAssoc(pos, key, pPar);
                if(!pPar) continue;
                if(!pPar->pBufParam) continue;
                pPar->m_index_of_last_measurement = -1;
                pPar->pBufParam->Clear0();
                pPar->pBufParam->AddFragment(0);
            }

            pGlubinaZaboia->fCur = gIZaboiValue;
            COleDateTime tim = COleDateTime::GetCurrentTime();
            pGlubinaZaboia->AddTail(tim.m_dt);
            pGlubinaZaboia->fCurAvg = gIZaboiValue;
            pGlubinaZaboia->fCurPrevious = gIZaboiValue;

            pGlubina->fCur = gIZaboiValue;
            pGlubina->AddTail(tim.m_dt);
            pGlubina->fCurAvg = gIZaboiValue;
            pGlubina->fCurPrevious = gIZaboiValue;


			CView* pView = pApp->GetView(t_real_time);
			if(pView && pApp->IsActiveView(t_real_time))
			{
				pView->SendMessage(PM_RECALC_ACTIVE_LIST, 0, 0);
			}
			startMarker1RT = 0;

			KRS_syncro_ole_time = m_prev_time = COleDateTime::GetCurrentTime();
			KRS_syncro_tick = GetTickCount();

            return true; 
        }
        else
        {

        }
    }

    iteration++;
	ParamCalculation();
	if (flVes != prevflVes)
	{
		m_wndDialogBar.m_ind_weight.SetState(flVes);
		prevflVes = flVes;
	}
	if (flDavl != prevflDavl)
	{
		m_wndDialogBar.m_ind_pressure.SetState(flDavl);
		prevflDavl = flDavl;
	}
	if (flDol != prevflDol)
	{
		m_wndDialogBar.m_ind_load.SetState(flDol);
		prevflDol = flDol;
	}
	m_wndDialogBar.m_ind_shurf.SetState(flShurf);

	if (KRS_running)
	{
		pApp->PostMessageToView(t_real_time, PM_DATAACP, 0, NULL);
		pApp->PostMessageToView(t_cement_rt, PM_DATAACP, 0, NULL);
		g_redraw_once = true;
	}
	else
	{
		if (g_redraw_once)
			Invalidate(FALSE);
		g_redraw_once = false;
	}
	
	return true; 
}

int g_pos_in_time_buffer = 0;
SERV_DeviceCfg* g_convertor_cfg;
CMainFrame* g_frm;

float CParam::GetCurrentValue(byte &status)
{
    if (!bParamActive) 
    {
        status = m_calc_status = FORMULA_CALC_STATUS_TIMEOUT;
        return 0;
    }

    if (m_calc_status == FORMULA_CALC_STATUS_IN_PROGRESS) 
		m_calc_status = FORMULA_CALC_STATUS_CYCLE;
	if (m_calc_status != FORMULA_CALC_STATUS_NOT_READY) 
	{
		status = m_calc_status;
		return fCurAvg;
	}

    if (pBufParam == NULL)
    {
        //        MessageBeep(MB_OK);
        return 0;
    }

	m_calc_status = FORMULA_CALC_STATUS_IN_PROGRESS;
	SaveAvrg();
	if (m_nTypePar == PARAM_TYPE_FORMULA)
	{
		if (m_formula == NULL)
		{
			StartTimeOut();
			status = FORMULA_CALC_STATUS_TIMEOUT;
			return fCurAvg;
		}
		status = FORMULA_CALC_STATUS_NOT_READY;
		fCur = m_formula->Calculate(status);
		if (status != FORMULA_CALC_STATUS_COMPLETE)
		{
			m_calc_status = status;
			return fCurAvg;
		}
	}
	else 
	{
		SERV_DeviceCfg* cfg;
		SERV_Channel *channel, *attr_channel;
		float val = 0, attr_koeff = 1;

		if (m_channel_num == -1)
		{
			if (m_nTypePar != PARAM_TYPE_CALCULATED)
			{
				StartTimeOut();
				status = FORMULA_CALC_STATUS_TIMEOUT;
				return fCurAvg;
			}
		}
		if (m_channel_num != -1) 
		{
			cfg = (m_channel_conv_num == 0) ? g_convertor_cfg : SERV_current_device_cfg;
			if (cfg == NULL)
			{
				StartTimeOut();
				status = FORMULA_CALC_STATUS_TIMEOUT;
				return fCurAvg;
			}
			channel = cfg->GetChannel(m_channel_num);
			if (channel == NULL || channel->IsTimedOut())
			{
				StartTimeOut();
				status = FORMULA_CALC_STATUS_TIMEOUT;
				return fCurAvg;
			}

			if (channel->GetDevice() == NULL || !channel->GetDevice()->InCalibrationPause())
			{
				val = channel->GetLastValue();

				if (m_attr_channel_num != -1)
				{
					cfg = (m_attr_channel_conv_num == 0) ? g_convertor_cfg : SERV_current_device_cfg;
					if (cfg == NULL)
					{
						StartTimeOut();
						status = FORMULA_CALC_STATUS_TIMEOUT;
						return fCurAvg;
					}
					attr_channel = cfg->GetChannel(m_attr_channel_num);
					if (attr_channel == NULL || attr_channel->IsTimedOut())
					{
						StartTimeOut();
						status = FORMULA_CALC_STATUS_TIMEOUT;
						return fCurAvg;
					}
					attr_koeff = (float)pow(10.0, -(0x03 & (WORD)(attr_channel->GetLastValue())));
					val *= attr_koeff;
				}
				fCur = val;
			}
		}
	}
	if ((KRS_project_flag & KRS_PRJ_FLAG_BURENIE) && m_num_par <= MAX_PARAM) 
	{
		fCur = g_frm->CalcMainParamA(m_num_par, TIME_current_time);
		if (m_calc_status == FORMULA_CALC_STATUS_TIMEOUT)
		{
			status = FORMULA_CALC_STATUS_TIMEOUT;
			return fCurAvg;
		}
	}

    AddTail(TIME_current_time);
	fCurAvg = GetAvrg();
    *pBufParam << fCurAvg;
	m_last_good_val = fCurAvg;
    FinishTimeOut(g_pos_in_time_buffer);

    g_frm->TestPickAlertParam(this);

	status = m_calc_status = FORMULA_CALC_STATUS_COMPLETE;
	return fCurAvg;
}

bool g_first_time = true;
double g_time_delta;

void ResetCalcStatus(CParamMap* map_param)
{
	CParam* param;
	int key;
	POSITION pos = map_param->GetStartPosition();
	while (pos != NULL)
	{
		map_param->GetNextAssoc(pos, key, param);
		if (param != NULL)
		{
			param->m_prev_calc_status = param->m_calc_status;
			param->m_calc_status = FORMULA_CALC_STATUS_NOT_READY;
		}
	}
}

void KRS_SendCurrentParamsToBD()
{
	CParam* param;
	int index;
	BS_OneParamValue *pv;
	POSITION pos;
	CParamMap *map_param = &m_MapParam;

	if ((g_measurement_to_send.m_param_values == NULL && map_param->GetCount() > 0) ||
		g_measurement_to_send_real_size < map_param->GetCount())
	{
		if (g_measurement_to_send.m_param_values)
			delete [] g_measurement_to_send.m_param_values;
		g_measurement_to_send_real_size = map_param->GetCount();
		g_measurement_to_send.m_param_values = new BS_OneParamValue[g_measurement_to_send_real_size];
	}

	int steps = (DB_connection_info.m_db_version <= 0) ? 1 : 2;
	for (int i = 0; i < steps; i++) 
	{
		g_measurement_to_send.m_param_values_size = 0;
		pv = g_measurement_to_send.m_param_values;
		pos = map_param->GetStartPosition();
		while (pos != NULL)
		{
			map_param->GetNextAssoc(pos, index, param);
			if (param == NULL || param->flDB == FALSE) 
				continue;

			if (DB_connection_info.m_db_version <= 0)
			{
				if (param->IsTimedOut())
					continue;
				pv->m_param_value = param->fCurAvg;
			}
			else
			{
				if (!param->IsTimedOut())
				{
					param->m_send_to_db_status = PARAM_DB_STATUS_NORMAL;
					pv->m_param_value = param->fCurAvg;
				}
				else
				{
					if (param->m_send_to_db_status == PARAM_DB_STATUS_NORMAL) 
					{
						param->m_send_to_db_status = PARAM_DB_STATUS_BEFORE_TO;
						pv->m_param_value = param->m_last_good_val;
					}
					else
					if (param->m_send_to_db_status == PARAM_DB_STATUS_BEFORE_TO) 
					{
						param->m_send_to_db_status = PARAM_DB_STATUS_TO;
						pv->m_param_value = PARAM_TO_VAL;
					}
					else
					{
						if (i == 1)
							continue;
						param->m_send_to_db_status = PARAM_DB_STATUS_TO;
						pv->m_param_value = PARAM_TO_VAL;
					}
				}
			}
			pv->m_param_index = index;
			g_measurement_to_send.m_param_values_size++;
			pv++;
		}

		if (g_measurement_to_send.m_param_values_size > 0)
		{
			g_measurement_to_send.m_time = TIME_current_time;
			g_measurement_to_send.m_deepness = (KRS_project_flag & KRS_PRJ_FLAG_BURENIE) ? (*g_quasi_glubina)->fCurAvg : 0;
			DB_SendParamValues(&g_measurement_to_send);
		}
	}
}

void CMainFrame::ParamCalculation()
{
	g_frm = this;
	g_convertor_cfg = &m_convertor_cfg;

	TIME_current_time = KRS_syncro_ole_time.m_dt + TicksToOleTime(GetTickCount() - KRS_syncro_tick);


	if (TIME_current_time <= m_prev_time)
		return;

	m_prev_time = TIME_current_time;

    *ptmData << TIME_current_time; 
	g_pos_in_time_buffer = ptmData->GetCur(); 

	if (!KRS_running)
	{
		if (g_quasi_glubina == NULL)
			return;
		*pcurGlub << (*g_quasi_glubina)->fCurAvg; 
		return;
	}

	BS_OneParamValue *pv;
	CONV_Channel* conv_channel;
	CParam *param;
	POSITION pos;
	int i, key;
	CParamMap *map_param;
	if (STAGES_IsEvaluating() || KRS_need_send_to_db_additional_to)
		map_param = &m_MapParamCM;
	else
		map_param = &m_MapParam;

	if (pTest1)
		pTest1->fCur = 	m_regimBurenia.GetCurRegime();

	if (CONV_connection_info.IsConnected())
	{
		bool timeout;
		BS_Measurement* m = CONV_GetMeasurement(timeout);
		if (m_wndDialogBar.m_ind_conv.m_hWnd != NULL)
			m_wndDialogBar.m_ind_conv.SetState(timeout?STATE_OFF:STATE_ON);
		if (!timeout && m)
		{
			if (m_convertor_device.GetChannelsSize() > 0)
			{
				pv = m->m_param_values;
				for (i = 0; i < m->m_param_values_size; i++, pv++)
				{
					conv_channel = (CONV_Channel*)m_convertor_cfg.GetChannel(pv->m_param_index);
					if (conv_channel == NULL)
						continue;
					conv_channel->SetLastValue(pv->m_param_value);
				}
			}
			m_convertor_device.m_last_received_tick = CONV_GetLastTick();
		}
	}
	if ((g_measurement_to_send.m_param_values == NULL && map_param->GetCount() > 0) ||
		g_measurement_to_send_real_size < map_param->GetCount())
	{
		if (g_measurement_to_send.m_param_values)
			delete [] g_measurement_to_send.m_param_values;
		g_measurement_to_send_real_size = map_param->GetCount();
		g_measurement_to_send.m_param_values = new BS_OneParamValue[g_measurement_to_send_real_size];
	}
	g_measurement_to_send.m_param_values_size = 0;
	pv = g_measurement_to_send.m_param_values;
	ResetCalcStatus(&m_MapParamDefault);
	if (STAGES_IsEvaluating())
		ResetCalcStatus(&m_MapParamCM_additional_only);
	byte status;
	if (KRS_project_flag & KRS_PRJ_FLAG_BURENIE)
	{
		pKruk->GetCurrentValue(status);
		if (status == FORMULA_CALC_STATUS_COMPLETE)
			flVes = pKruk->fCurAvg > gVesBlok;
		else
			flVes = 2;

		pDavlVhoda->GetCurrentValue(status);
		if (status == FORMULA_CALC_STATUS_COMPLETE)
			flDavl = pDavlVhoda->fCurAvg > gDavlenieBlok;
		else
			flDavl = 2;

		pDoloto->GetCurrentValue(status);
		if (status == FORMULA_CALC_STATUS_COMPLETE)
			flDol = pDoloto->fCurAvg > gDolBlok;
		else
			flDol = 2;

		pLinstrumenta->GetCurrentValue(status);
		pZahodKvadrata->GetCurrentValue(status);
		pTalblock->GetCurrentValue(status);

		pZaboi->GetCurrentValue(status); // ???
		pGlubinaZaboia->GetCurrentValue(status); // ???
		flZaboi	= pZaboi->fCurAvg > pGlubinaZaboia->fCurAvg;
		if (gflZaboiIsSetted)
		{
			gflZaboiIsSetted = false;
			pZaboi->fCurPrevious = gIZaboiValue;
		}
		knWes		= (int(pState->fCur)) & 0x00000001;
		knZahod		= (int(pState->fCur)) & 0x00000002;
		knKNBLplus	= (int(pState->fCur)) & 0x00000004;
		knKNBLminus	= (int(pState->fCur)) & 0x00000008;

		flGlub	= (pGlubina->fCurAvg - pGlubinaZaboia->fCurAvg) < gPorogBur;
	}
	pos = map_param->GetStartPosition();
	bool send;
	byte db_status;
	while (pos != NULL)
	{
		map_param->GetNextAssoc(pos, key, param);
		if (param == NULL)
			continue;
	
		if (KRS_need_send_to_db_additional_to && param->m_num_par >= STAGES_START_ADDITIONAL_PARAM_NUM)
			status = FORMULA_CALC_STATUS_TIMEOUT;
		else
		{
			status = FORMULA_CALC_STATUS_NOT_READY;
			param->GetCurrentValue(status);
		}

		if (param->flDB == FALSE) 
			continue;

		if (DB_connection_info.m_db_version <= 0)
		{
			send = !param->IsTimedOut();
			if (send)
			{
				pv->m_param_value = param->fCurAvg;
				db_status = PARAM_DB_STATUS_NORMAL;
			}
		}
		else
		{
			send = false;
			if (!param->IsTimedOut())
			{
				if (param->m_send_to_db_status != PARAM_DB_STATUS_NORMAL || // выход из таймаута, или "не вход"
					TIME_current_time - param->m_last_time_send_to_db > param->m_db_time_interval || // интервал по времени
					fabs(param->m_last_value_send_to_db - param->fCurAvg) > param->m_db_data_interval) // интервал по значению
				{
					db_status = PARAM_DB_STATUS_NORMAL;
					send = true;
					pv->m_param_value = param->fCurAvg;
				}
			}
			else
			{
				if (param->m_send_to_db_status == PARAM_DB_STATUS_NORMAL) // начали вход в таймаут
				{
					db_status = PARAM_DB_STATUS_BEFORE_TO;
					send = true;
					pv->m_param_value = param->m_last_good_val;
				}
				if (param->m_send_to_db_status == PARAM_DB_STATUS_BEFORE_TO) // прописали таймаут
				{
					db_status = PARAM_DB_STATUS_TO;
					send = true;
					pv->m_param_value = PARAM_TO_VAL;
				}
			}
		}
	
		if (send)
		{
			param->m_send_to_db_status = db_status;
			param->m_last_value_send_to_db = pv->m_param_value;
			param->m_last_time_send_to_db = TIME_current_time;
			pv->m_param_index = key;
			g_measurement_to_send.m_param_values_size++;
			pv++;
		}
	}

	if (KRS_project_flag & KRS_PRJ_FLAG_BURENIE)
	{
		*pcurGlub << (*g_quasi_glubina)->fCurAvg;
	}
	if (KRS_project_flag & KRS_PRJ_FLAG_BURENIE)
	{
		flShurf	= abs(pShurf->fCur) > 0 ? true : false;
		float ff;
		if (pTalblock != NULL) 
		{
			ff = pTalblock->GetDiffValue(gPorogTime);
			if (fabs(ff) > gPorogVisota)
				gnTalblok = ff > 0 ? 1 : -1;
			else
				gnTalblok = 0;
		}
	}

	if(flgRabotaBezShurfaChanged)
	{
		ChangeflgRabotaBezShurfa();
		flgRabotaBezShurfaChanged = false;
		gflgRabotaBezShurfa = g_flgRabotaBezShurfa;
	}
	else if(flg_GlI_Changed)
	{
		SetGlI();
		flg_GlI_Changed = false;
	}
	else if(flg_GlX_Changed)
	{
		SetGlX();
		fixGlub = gGlX;
		flg_GlX_Changed = false;
	}

	if (KRS_project_flag & KRS_PRJ_FLAG_BURENIE)
	{
		m_wndDialogBarH.OutputTalblock(0);
		m_wndDialogBarH.OutputWk(0, false);
		m_wndDialogBarH.OutTextNKBNK();
		m_wndDialogBar.m_Regim.SetText(m_regimBurenia.TestCurRegime(flShurf, flVes, flDavl, bool(flDol > 0), gnTalblok, flGlub), true);
	}
	if (g_measurement_to_send.m_param_values_size > 0)
	{
		g_measurement_to_send.m_time = TIME_current_time;
		g_measurement_to_send.m_deepness = (KRS_project_flag & KRS_PRJ_FLAG_BURENIE) ? (*g_quasi_glubina)->fCurAvg : 0;
		DB_SendParamValues(&g_measurement_to_send);
	}

	if (KRS_need_send_to_db_additional_to)
		KRS_need_send_to_db_additional_to = false;
}

BOOL CMainFrame::TestTimedOutPar(CParam *pPrm, BOOL fl_timedout, int pos)
{
	if (fl_timedout)
	{
        pPrm->m_calc_status = FORMULA_CALC_STATUS_TIMEOUT;
		return true;
	}
	return false;
}

DWORD llll;

float CMainFrame::CalcMainParamA(int key, double curTime)
{
	int pos_in_time_buffer = ptmData->GetCur(); //!!!
	double val = 0, ff = 0;
	DWORD tt;
	double cur_Komponovka, komponovka, f_cur, dd;
	int cur_element;
	byte status;
	COleDateTimeSpan time_span;


	DWORD hmss;
	int h, m, s, ss;
/* ! */
	m_wndDialogBar.m_Regim.SetState(1);
/* ! */

	if(flDol) flagKBK_plus = flagKBK_minus = false;
	BOOL flg_timedout;
	CParam *pPar; 
    if(!m_MapParam.Lookup(key, pPar)) return 0;
    bool correction;

	SetXBufToParam(pPar);

	switch (key)
	{
		case 1: 
			ff = (flZaboi) ? pZaboi->fCurPrevious : pGlubinaZaboia->fCur;
			if (ff > gIZaboiValue)
				gIZaboiValue = ff;
			return gIZaboiValue;
		case 2: 

			correction = false;
			if (knZahod || gflagButtonNull)
			{
				gflagButtonNull = false;
				if (gflgRabotaBezShurfa)
				{
					if (flVes && flDavl)
					{
						correction = true;
						ff = gKvadratDlina + gfKvadratPerehodnik;
					}
				}
				else
				{
					if (flShurf && flVes)
					{
						correction = true;
						ff = gKvadratDlina + gfKvadratPerehodnik;
					}
					if (!flShurf)
					{
						correction = true;
						ff = gfKvadratElevator;
					}
				}
			}
			if (!correction)
			{
				ff = pTalblock->fCur;
			}
			if (ff + gfTalblokDeltaH < 0)
			{
				correction = true;
				ff = 0.0f;
				gfTalblokDeltaH = 0;
			}
			if (correction || gfTalblokDeltaH != 0)
			{
				SERV_SendParamToBKSD(pTalblock, (ff + gfTalblokDeltaH) * 1000);
				gfTalblokDeltaH = 0;
			}
			return ff;			
		case 3: 
			pKruk->GetCurrentValue(status);
			pShurf->GetCurrentValue(status);
			pZahodKvadrata->GetCurrentValue(status);
			pTalblock->GetCurrentValue(status);
			pLinstrumenta->GetCurrentValue(status);;
			flg_timedout = pKruk->IsTimedOut() | pShurf->IsTimedOut() | pZahodKvadrata->IsTimedOut() | pTalblock->IsTimedOut();
			if (flg_timedout)
				return pPar->fCurPrevious;
			if (!gflgRabotaBezShurfa)
			{
				if (flVes)
				{
					if (flShurf)
						ff = pLinstrumenta->fCurAvg + pZahodKvadrata->fCurAvg;
					else
						ff = pLinstrumenta->fCurAvg - (pTalblock->fCurAvgPrevious - gfKvadratElevator);
					ff += gfDLi;
				}
				else
					ff = pGlubinaZaboia->fCurPrevious;
			}
			else
			{
				if (flVes)
					ff = pGlubinaZaboia->fCurPrevious - (pTalblock->fCurAvg - pTalblock->fCurAvgPrevious); // приращение тальблока
				else
					ff = pGlubinaZaboia->fCurPrevious;
			}
			gGlubinaZaboia = ff;
			return ff;
		case 4: 
			flg_timedout = pKruk->IsTimedOut() || pTalblock->IsTimedOut() || pShurf->IsTimedOut();
			if (flg_timedout)
				return pPar->fCurPrevious;
			pKruk->GetCurrentValue(status);
			pTalblock->GetCurrentValue(status);
			pShurf->GetCurrentValue(status);
			if (gflgRabotaBezShurfa)
			{
				if (flVes) 
					gfLinstrumenta = pGlubinaZaboia->fCurAvgPrevious + (pTalblock->fCurAvg - gfKvadratElevator);
				else
					gfLinstrumenta = pLinstrumenta->fCurPrevious;
				return gfLinstrumenta;
			}
			cur_Komponovka = gtblElement.GetSaveCurLen();
			flg_timedout = pKruk->IsTimedOut() || pTalblock->IsTimedOut() || pShurf->IsTimedOut();
			if (!flg_timedout && !flShurf && flVes && (pKruk->fCurAvgPrevious <= gVesBlok)) 
			{
				float cur_L = pGlubinaZaboia->fCurAvgPrevious + (pTalblock->fCurAvg - gfKvadratElevator);
				komponovka = gtblElement.FindComponovka(cur_L); 
			}
			else
			{
				komponovka = gtblElement.GetLenFromBegToCurElem(); 
			}
			cur_element = gtblElement.GetNumCurElement();

			if(fabs(cur_Komponovka - komponovka) > 0.1)
			{
				CString markerStr;
				markerStr.Format("№%d %s ", cur_element, gtblElement.GetNameCE());
				ptmData->CreateMarker(-1, markerStr, RGB(0,0,255), RGB(0,0,255), ptmData->GetCurData(), 0);
			}
			return komponovka;
		case 5: 
			if (gflgRabotaBezShurfa)
				return 0;
			if (!pState->IsTimedOut() && (knKNBLplus || knKNBLminus))
			{
				tt = GetTickCount() - ttSave;
				if (tt > 2000)
				{
					if (knKNBLplus)
						gtblElement.IncCurElement();
					else
					if (knKNBLminus)
						gtblElement.DecrCurElement();
					ttSave = GetTickCount();
				}
			}
			return (float)gtblElement.GetNumCurElement();
		case 6: 
			flg_timedout = pKruk->IsTimedOut() || pShurf->IsTimedOut() || pTalblock->IsTimedOut();
			if (TestTimedOutPar(pPar, flg_timedout, pos_in_time_buffer))
				return pPar->fCurPrevious;
			pTalblock->GetCurrentValue(status);
			if (flVes && flShurf)
				ff = gKvadratDlina - (pTalblock->fCurAvgPrevious - gfKvadratPerehodnik);
			else
				ff = pZahodKvadrata->fCurPrevious;
			if (ff > gKvadratDlina)
				ff = gKvadratDlina;
			if (knZahod || !flShurf)
				ff = 0;
			return ff;
		case 7: 
			flg_timedout = pDavlVhoda->IsTimedOut() || pShurf->IsTimedOut() || pTalblock->IsTimedOut();
			if (TestTimedOutPar(pPar, flg_timedout, pos_in_time_buffer))
				return pPar->fCurPrevious;
			pTalblock->GetCurrentValue(status);
			if ((!flDavl) && (!flShurf))
				ff = pTalblock->fCurAvgPrevious - gfKvadratElevator;
			else
				ff = 0;
			if (flShurf)
				ff = 0;
			return ff;
		case 8: 
			return pGlubina->fCur - pGlubinaZaboia->fCur;
		case 9: 
			if (knWes)
			{
				gWk = pKruk->fCur;
				SERV_SendParamToBKSD(pKruk, gWk);
			}
			if (pRotorMoment->fCurAvg >= gPorogRotorBur && gControlRotorBur) 
				pKruk->fCur += gVesRotorBur; 
			return pKruk->fCur;
		case 10: 
			flg_timedout = pKruk->IsTimedOut() || pDavlVhoda->IsTimedOut();
			if (TestTimedOutPar(pPar, flg_timedout, pos_in_time_buffer))
				return pPar->fCurPrevious;
			if (flDavl)
				ff = gWk - pKruk->fCur;
			else
				ff = 0.0f;
			return ff;
		case 11: 
			return pKluch->fCur;
		case 12: 
			return pRotorMoment->fCur;
		case 13: 
			return pDavlVhoda->fCur;
		case 14: 
			flg_timedout = pDavlVhoda->IsTimedOut() || pDoloto->IsTimedOut() || pKluch->IsTimedOut();
			if (TestTimedOutPar(pPar, flg_timedout, pos_in_time_buffer))
				return pPar->fCurPrevious;
			ff = 0;
			if (flDavl && flDol) 
			{
				if (!flMehInterval)
					ff = pPar->fCurPrevious; 
				else
				{
					ff = pGlubina->pBufParam->GetDrvInv(gMexInterval) / 24;
					flMehInterval = false;
				}
			}
			return ff;
		case 15: 
			flg_timedout = pDavlVhoda->IsTimedOut();
			if (TestTimedOutPar(pPar, flg_timedout, pos_in_time_buffer))
				return pPar->fCurPrevious;
			pTalblock->GetCurrentValue(status);
			ff = 0;
			if(!flDavl) 
			{
				ff = (float)pTalblock->pBufParam->GetDrv(gSPOInterval/(24*60*60.0f));
				ff = ff/(24*60*60.0f);
			}
			return ff;
		case 16: 
			flg_timedout = pDavlVhoda->IsTimedOut();
			if (TestTimedOutPar(pPar, flg_timedout, pos_in_time_buffer))
				return pPar->fCurPrevious;
			ff = 0;
			if(flDavl) 
				ff = pVremiaCirkuliacii->pBufParam->GetGetLastDeltaX() * 24.0;
			KRS_time_circ_lifetime += ff;
			return KRS_time_circ_lifetime;
		case 17: 
			flg_timedout = pDavlVhoda->IsTimedOut() || pDoloto->IsTimedOut();
			if (TestTimedOutPar(pPar, flg_timedout, pos_in_time_buffer))
				return pPar->fCurPrevious;
			ff = 0;
			if(flDavl && flDol && flGlub)
				ff = pTimeBurenia->pBufParam->GetGetLastDeltaX() * 24.0;
			KRS_time_bur_lifetime += ff;
			return KRS_time_bur_lifetime;
		case 18: 
			hmss = (DWORD)pTime->fCur;
			h = ((hmss >> 24) & 0xFF), m = ((hmss >> 16) & 0xFF), s = ((hmss >> 8) & 0xFF), ss = (hmss & 0xFF);
			pTime->fCur = (float)(h * 1.0/24 + m * 1.0/(24*60) + (s + ss*1.0/128)*1.0/(24*60*60));
			return pTime->fCur;//???
		case 19: 
			if (gflgRabotaBezShurfa)
				pShurf->fCur = 1;
			return (float)((pShurf->fCur > 0) ? 1 : 0);
		case 20: 
			return pState->fCur;
		case 21: 
			ff = max(pGlubinaZaboia->fCur, pGlubina->fCurPrevious);
			gGlubina = ff;
			if (gGlubina < fixGlub) 
				fixGlub = gGlubina;
			if(gGlubina - fixGlub >= gMexInterval)
			{
				int q = gGlubina/gMexInterval;
				fixGlub = q * gMexInterval;
				flMehInterval = true;
			}
			return ff;
		case 22: 
			return pRotorBur->fCur;
		case 23: 
			return pTest1->fCur;
		case 24: 
			return pTest2->fCur;
		case 25: 
			return pImpl->fCur;
		default:;
	}
	return 0;
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		Param_blink_counter++;
		if (Param_blink_counter > 10 || Param_blink_counter < 0)
			Param_blink_counter = 0;
		PostMessage(PM_DATAACPFRM, 0, 0);
	}
	else
	if (nIDEvent == 2)
	{
		LOG_V0_Flush();
		if ((KRS_project_flag & KRS_PRJ_FLAG_STAGES) && PRJ_timer_saving_available)
			STAGES_SaveStages(false, PRJ_NowSelectedBranch);
	}
	else
	if (nIDEvent == 3)
	{
		KillTimer(3);
		SetTimer(4, KRS_send_to_bo_interval, NULL);
	}
	else
	if (nIDEvent == 4)
	{
		if (KRS_running)
			SERV_SendIndication();
	}
	else
	if(nIDEvent == ID_Timer3)
	{
		KRS_syncro_ole_time = COleDateTime::GetCurrentTime();
		KRS_syncro_tick = GetTickCount();
	}
	CFrameWnd::OnTimer(nIDEvent);
}

////////////////////////////////////////////////////////////////////////////

void CMainFrame::SetInitialData()
{
	CParam *pParam;
	int line_width = 2;
	int ii = 1;

	if ((KRS_project_flag & KRS_PRJ_FLAG_BURENIE) == 0)
	{
		pZaboi = pTalblock = pGlubinaZaboia = pLinstrumenta = pNkbk = pZahodKvadrata = pVisotaElevatora = pPologenieKnbk = 
		pKruk = pDoloto = pKluch = pRotorMoment = pDavlVhoda = pMechanikSkorost = pSkorostSPO = pVremiaCirkuliacii = pTimeBurenia = 
		pTime = pShurf = pState = pRotorBur = pTest1 = pTest2 = pImpl = pGlubina = NULL;
		g_quasi_glubina = NULL;
		return;
	}

	pParam = new CParam;
	pParam->sName = "Глубина";
	pParam->sGraphName = "ГлX";
	pParam->sRazmernPar = "м";
	pParam->m_num_par = 21;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 50;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x000000;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pGlubina = pParam;
	g_quasi_glubina = &pGlubina;
	pParam = new CParam;
	pParam->sName = "iЗабой";
	pParam->sGraphName = "iЗб";
	pParam->sRazmernPar = "м";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = 1000;
	pParam->m_color = 0x0088AA;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pZaboi = pParam;
	pParam = new CParam;
	pParam->sName = "Высота тальблока";
	pParam->sGraphName = "ВысТ";
	pParam->sRazmernPar = "м";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CHANNEL;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 50;
	pParam->fBlock = 0;
	pParam->fAvaria = 55;
	pParam->nKodControl = 3;
	pParam->fDiapazon = 60;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x00AA00;
	pParam->m_line_width = line_width;
	pParam->nFlag1 = 0;
	pParam->flGraph = true;
	m_MapParamDefault[pParam->m_num_par] = pTalblock = pParam;
	pParam = new CParam;
	pParam->sName = "Глубина забоя";
	pParam->sGraphName = "Глуб";
	pParam->sRazmernPar = "м";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 50;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x000000;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pGlubinaZaboia = pParam;
	pParam = new CParam;
	pParam->sName = "L инструмент";
	pParam->sGraphName = "L и";
	pParam->sRazmernPar = "м";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 1000;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = 1000;
	pParam->m_color = 0xAB4980;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pLinstrumenta = pParam;
	pParam = new CParam;
	pParam->sName = "№ КБК";
	pParam->sGraphName = "№кбк";
	pParam->sRazmernPar = "шт";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 0;
	pParam->fMin = 0;
	pParam->fMax = 1000;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 1000;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = 1000;
	pParam->m_color = 0x808080;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pNkbk = pParam;
	pParam = new CParam;
	pParam->sName = "Заход квадрата";
	pParam->sGraphName = "ЗахКв";
	pParam->sRazmernPar = "м";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 1000;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 50;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x008000;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pZahodKvadrata = pParam;
	pParam = new CParam;
	pParam->sName = "Высота элеватора";
	pParam->sGraphName = "ВысЭ";
	pParam->sRazmernPar = "м";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 1000;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 50;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x5500CC;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pVisotaElevatora = pParam;
	pParam = new CParam;
	pParam->sName = "Положение КБК";
	pParam->sGraphName = "Пол";
	pParam->sRazmernPar = "м";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 1000;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 5000;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0xCC3333;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pPologenieKnbk = pParam;
	pParam = new CParam;
	pParam->sName = "Вес на крюке";
	pParam->sGraphName = "Вес";
	pParam->sRazmernPar = "Тс";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CHANNEL;
	pParam->m_shown_digits = 3;
	pParam->fMin = 0;
	pParam->fMax = 150;
	pParam->fBlock = 5;
	pParam->fAvaria = 250;
	pParam->nKodControl = 3;
	pParam->fDiapazon = 350;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x0000FF;
	pParam->m_line_width = line_width;
	pParam->flGraph = true;
	pParam->nFlag1 = 2;
	m_MapParamDefault[pParam->m_num_par] = pKruk = pParam;
	pParam = new CParam;
	pParam->sName = "Нагрузка на долото";
	pParam->sGraphName = "Нагр";
	pParam->sRazmernPar = "Тс";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 2;
	pParam->fMin = 0;
	pParam->fMax = 50;
	pParam->fBlock = 10;
	pParam->fAvaria = 0;
	pParam->nKodControl = 2;
	pParam->fDiapazon = pKruk->fDiapazon;  // равен диапазону веса на крюке
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x808000;
	pParam->m_line_width = line_width;
	pParam->flGraph = true;
	m_MapParamDefault[pParam->m_num_par] = pDoloto = pParam;
	pParam = new CParam;
	pParam->sName = "Момент на ключе";
	pParam->sGraphName = "МомК";
	pParam->sRazmernPar = "кн*м";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CHANNEL;
	pParam->m_shown_digits = 3;
	pParam->fMin = 0;
	pParam->fMax = 50;
	pParam->fBlock = 10;
	pParam->fAvaria = 0;
	pParam->nKodControl = 3;
	pParam->fDiapazon = 100;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x800080;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pKluch = pParam;
	pParam = new CParam;
	pParam->sName = "Момент на роторе";
	pParam->sGraphName = "МомР";
	pParam->sRazmernPar = "кн*м";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CHANNEL;
	pParam->m_shown_digits = 3;
	pParam->fMin = 0;
	pParam->fMax = 50;
	pParam->fBlock = 10;
	pParam->fAvaria = 0;
	pParam->nKodControl = 3;
	pParam->fDiapazon = 60;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x008080;
	pParam->m_line_width = line_width;
	pParam->nFlag2 = 0;
	pParam->nScale = 60;
	pParam->fPodklinkaKof = 2;
	pParam->nFlag1 = 0;
	pParam->flGraph = true;
	pParam->timeAvrg = 0.1/(24*60*60);
	m_MapParamDefault[pParam->m_num_par] = pRotorMoment = pParam;
	pParam = new CParam;
	pParam->sName = "Давление на входе";
	pParam->sGraphName = "Давл";
	pParam->sRazmernPar = "Атм";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CHANNEL;
	pParam->m_shown_digits = 3;
	pParam->fMin = 20;
	pParam->fMax = 200;
	pParam->fBlock = 60;
	pParam->fAvaria = 350;
	pParam->nKodControl = 3;
	pParam->fDiapazon = 400;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x3080F0;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pDavlVhoda = pParam;
	pParam = new CParam;
	pParam->sName = "Механическая скорость";
	pParam->sGraphName = "МехC";
	pParam->sRazmernPar = "м/час";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 200;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x008080;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pMechanikSkorost = pParam;
	pParam = new CParam;
	pParam->sName = "Скорость СПО";
	pParam->sGraphName = "Скор";
	pParam->sRazmernPar = "м/с";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 30;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 3;
	pParam->fDiapazon = 50;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x802080;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pSkorostSPO = pParam;
	pParam = new CParam;
	pParam->sName = "Время циркуляции";
	pParam->sGraphName = "ВреЦ";
	pParam->sRazmernPar = "часы";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 2;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 100;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0xAAAAAA;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pVremiaCirkuliacii = pParam;
	pParam = new CParam;
	pParam->sName = "Время бурения";
	pParam->sGraphName = "ВреБ";
	pParam->sRazmernPar = "часы";
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 2;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 200;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x666666;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pTimeBurenia = pParam;
	pParam = new CParam;
	pParam->sName = "Время";
	pParam->sGraphName = "Время";
	pParam->sRazmernPar = "час.мин";
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CHANNEL;
	pParam->m_shown_digits = 0xFF;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 200;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x222222;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pTime = pParam;
	pParam = new CParam;
	pParam->fCur = 0;
	pParam->sName = "Шурф";
	pParam->sGraphName = "Шурф";
	pParam->sRazmernPar = "бит";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CHANNEL;
	pParam->m_shown_digits = 0;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 3;
	pParam->fGraphMIN = -1;
	pParam->fGraphMAX = +2;
	pParam->m_color = 0x800050;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pShurf = pParam;
	pParam = new CParam;
	pParam->sName = "Состояние кнопок";
	pParam->sGraphName = "Кнпк";
	pParam->sRazmernPar = "байт";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CHANNEL;
	pParam->m_shown_digits = 0;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 0xFFFF;
	pParam->fGraphMIN = -1;
	pParam->fGraphMAX = 0xFFFF + 1;
	pParam->m_color = 0x805000;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pState = pParam;
	ii++;
	pParam = new CParam;
	pParam->sName = "Бурение ротором";
	pParam->sGraphName = "Рбур";
	pParam->sRazmernPar = "xx";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CHANNEL;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 50;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x000000;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pRotorBur = pParam;
	pParam = new CParam;
	pParam->sName = "Test1";
	pParam->sGraphName = "xx";
	pParam->sRazmernPar = "xx";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 50;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x000000;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pTest1 = pParam;
	pParam = new CParam;
	pParam->sName = "Test2";
	pParam->sGraphName = "xx";
	pParam->sRazmernPar = "xx";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CALCULATED;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 50;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x000000;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pTest2 = pParam;
	pParam = new CParam;
	pParam->sName = "Импульсы лебедки";
	pParam->sGraphName = "xx";
	pParam->sRazmernPar = "xx";
	pParam->m_num_par = ii++;
	pParam->m_nTypePar = PARAM_TYPE_CHANNEL;
	pParam->m_shown_digits = 1;
	pParam->fMin = 0;
	pParam->fMax = 0;
	pParam->fBlock = 0;
	pParam->fAvaria = 0;
	pParam->nKodControl = 0;
	pParam->fDiapazon = 50;
	pParam->fGraphMIN = 0;
	pParam->fGraphMAX = pParam->fDiapazon;
	pParam->m_color = 0x000000;
	pParam->m_line_width = line_width;
	m_MapParamDefault[pParam->m_num_par] = pImpl = pParam;
	for(int j = 1; j < m_MapParamDefault.GetCount(); j++) 
	{
		pParam = m_MapParamDefault[j];
		pParam->fCurPrevious = 0;
		pParam->fCurAvgPrevious = 0;
		pParam->fCurAvg = 0;
        pParam->fl_db = 0;
	}
	m_regimBurenia.max_regime = 0;
	m_regimBurenia.AddRegim("Пустой крюк",    1,   0,   0,   0,   0,   0);
	m_regimBurenia.AddRegim("Пустой крюк",    1,   0,   0,   0,   0,   1);
	m_regimBurenia.AddRegim("Пустой крюк",    1,   0,   0,   0,   1,   0);
	m_regimBurenia.AddRegim("Пустой крюк",    1,   0,   0,   0,  -1,   0);
	m_regimBurenia.AddRegim("Пустой крюк",    1,   0,   0,   0,   1,   1);
	m_regimBurenia.AddRegim("Пустой крюк",    1,   0,   0,   0,  -1,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   0,   0,   1,   0,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   0,   0,   1,   0,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   0,   0,   1,   1,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   0,   0,   1,  -1,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   0,   0,   1,   1,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   0,   0,   1,  -1,   1);
	m_regimBurenia.AddRegim("ошибка № 2",     1,   0,   1,   0,   0,   0);
	m_regimBurenia.AddRegim("ошибка № 2",     1,   0,   1,   0,   0,   1);
	m_regimBurenia.AddRegim("ошибка № 2",     1,   0,   1,   0,   1,   0);
	m_regimBurenia.AddRegim("ошибка № 2",     1,   0,   1,   0,  -1,   0);
	m_regimBurenia.AddRegim("ошибка № 2",     1,   0,   1,   0,   1,   1);
	m_regimBurenia.AddRegim("ошибка № 2",     1,   0,   1,   0,  -1,   1);
	m_regimBurenia.AddRegim("Промывка",       1,   0,   1,   1,   0,   0);
	m_regimBurenia.AddRegim("Бурение",        1,   0,   1,   1,   0,   1);
	m_regimBurenia.AddRegim("ошибка № 3",     1,   0,   1,   1,   1,   0);
	m_regimBurenia.AddRegim("Проработка",     1,   0,   1,   1,  -1,   0);
	m_regimBurenia.AddRegim("ошибка № 3",     1,   0,   1,   1,   1,   1);
	m_regimBurenia.AddRegim("Бурение",        1,   0,   1,   1,  -1,   1);
	m_regimBurenia.AddRegim("Над забоем",     1,   1,   0,   0,   0,   0);
	m_regimBurenia.AddRegim("Над забоем",     1,   1,   0,   0,   0,   1);
	m_regimBurenia.AddRegim("Подьем",         1,   1,   0,   0,   1,   0);
	m_regimBurenia.AddRegim("Спуск",          1,   1,   0,   0,  -1,   0);
	m_regimBurenia.AddRegim("Подьем",         1,   1,   0,   0,   1,   1);
	m_regimBurenia.AddRegim("Спуск",          1,   1,   0,   0,  -1,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   1,   0,   1,   0,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   1,   0,   1,   0,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   1,   0,   1,   1,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   1,   0,   1,  -1,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   1,   0,   1,   1,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     1,   1,   0,   1,  -1,   1);
	m_regimBurenia.AddRegim("Промывка",       1,   1,   1,   0,   0,   0);
	m_regimBurenia.AddRegim("Промывка",       1,   1,   1,   0,   0,   1);
	m_regimBurenia.AddRegim("Проработка",     1,   1,   1,   0,   1,   0);
	m_regimBurenia.AddRegim("Проработка",     1,   1,   1,   0,  -1,   0);
	m_regimBurenia.AddRegim("Проработка",     1,   1,   1,   0,   1,   1);
	m_regimBurenia.AddRegim("Проработка",     1,   1,   1,   0,  -1,   1);
	m_regimBurenia.AddRegim("Промывка",       1,   1,   1,   1,   0,   0);
	m_regimBurenia.AddRegim("Бурение",        1,   1,   1,   1,   0,   1);
	m_regimBurenia.AddRegim("Проработка***",  1,   1,   1,   1,   1,   0);
	m_regimBurenia.AddRegim("Проработка",     1,   1,   1,   1,  -1,   0);
	m_regimBurenia.AddRegim("Бурение",        1,   1,   1,   1,   1,   1);
	m_regimBurenia.AddRegim("Бурение",        1,   1,   1,   1,  -1,   1);
	m_regimBurenia.AddRegim("Пустой крюк",    0,   0,   0,   0,   0,   0);
	m_regimBurenia.AddRegim("Пустой крюк",    0,   0,   0,   0,   0,   1);
	m_regimBurenia.AddRegim("Пустой крюк",    0,   0,   0,   0,   1,   0);
	m_regimBurenia.AddRegim("Пустой крюк",    0,   0,   0,   0,  -1,   0);
	m_regimBurenia.AddRegim("Пустой крюк",    0,   0,   0,   0,   1,   1);
	m_regimBurenia.AddRegim("Пустой крюк",    0,   0,   0,   0,  -1,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   0,   0,   1,   0,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   0,   0,   1,   0,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   0,   0,   1,   1,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   0,   0,   1,  -1,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   0,   0,   1,   1,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   0,   0,   1,  -1,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   0,   0,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   0,   0,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   0,   1,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   0,  -1,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   0,   1,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   0,  -1,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   1,   0,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   1,   0,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   1,   1,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   1,  -1,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   1,   1,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   0,   1,   1,  -1,   1);
	m_regimBurenia.AddRegim("Над забоем",     0,   1,   0,   0,   0,   0);
	m_regimBurenia.AddRegim("Над забоем",     0,   1,   0,   0,   0,   1);
	m_regimBurenia.AddRegim("Подьем",         0,   1,   0,   0,   1,   0);
	m_regimBurenia.AddRegim("Спуск",          0,   1,   0,   0,  -1,   0);
	m_regimBurenia.AddRegim("Подьем",         0,   1,   0,   0,   1,   1);
	m_regimBurenia.AddRegim("Спуск",          0,   1,   0,   0,  -1,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   1,   0,   1,   0,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   1,   0,   1,   0,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   1,   0,   1,   1,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   1,   0,   1,  -1,   0);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   1,   0,   1,   1,   1);
	m_regimBurenia.AddRegim("Ошибка № 0",     0,   1,   0,   1,  -1,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   0,   0,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   0,   0,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   0,   1,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   0,  -1,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   0,   1,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   0,  -1,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   1,   0,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   1,   0,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   1,   1,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   1,  -1,   0);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   1,   1,   1);
	m_regimBurenia.AddRegim("Ошибка № 1",     0,   1,   1,   1,  -1,   1);
}

void CMainFrame::OnDestroy() 
{
    KRS_start_counter = 0;

	CFrameWnd::OnDestroy();

//-------------------------------------------------------------
	KRS_ClearMap(&m_MapParamDB);

	KRS_ClearMap(&m_MapParamDefault);
//-------------------------------------------------------------
	delete ptmData;
	ptmData = NULL;

	delete ptmDataDB;
	ptmDataDB = NULL;

	delete pcurGlub;
	pcurGlub = NULL;

	delete pcurGlubDB;
	pcurGlubDB = NULL;
//-------------------------------------------------------------
}

void CMainFrame::OnCheckNgr1() 
{
	CView* pView = pApp->GetActiveView();
	int n_state = ((CButton *)(m_wndDialogBarH.GetDlgItem(IDC_CHECK_NGR1)))->GetCheck();
	n_state = 1;
	if(pView != NULL) pView->PostMessage(PM_CHANGE_NUM_GRF, 0, n_state);
}

void CMainFrame::OnCheckNgr2() 
{
	CView* pView = pApp->GetActiveView();
	int n_state = ((CButton *)(m_wndDialogBarH.GetDlgItem(IDC_CHECK_NGR2)))->GetCheck();
	if(pView != NULL) pView->PostMessage(PM_CHANGE_NUM_GRF, 1, n_state);
}

void CMainFrame::OnCheckNgr3() 
{
	CView* pView = pApp->GetActiveView();
	int n_state = ((CButton *)(m_wndDialogBarH.GetDlgItem(IDC_CHECK_NGR3)))->GetCheck();
	if(pView != NULL) pView->SendMessage(PM_CHANGE_NUM_GRF, 2, n_state);
}

void CMainFrame::OnRadioScaleTim() 
{
	CView* pView = pApp->GetActiveView();
	if(pView != NULL) pView->SendMessage(PM_CHANGESCALE, 1, 0);
}

void CMainFrame::OnRadioScaleGlub() 
{
	CView* pView = pApp->GetActiveView();
	if(pView != NULL) pView->SendMessage(PM_CHANGESCALE, 0, 0);
}


//===============================================================================

LRESULT CMainFrame::OnListReadyCONV(WPARAM, LPARAM)
{
	CONV_Channel *channel;

	m_convertor_device.DeleteAllChannels();

	map<int, BS_ParamDescriptionField*>::iterator current_field;
	map<int, BS_ParamDescription>::iterator current_param = CONV_connection_info.m_param_descriptions.begin();
	while (current_param != CONV_connection_info.m_param_descriptions.end())
	{
		channel = new CONV_Channel("", &m_convertor_device, current_param->first);
		m_convertor_device.AddChannel(channel);
		current_field = current_param->second.m_param_description_fields.begin();
		while (current_field != current_param->second.m_param_description_fields.end())
		{
 			if (current_field->second->m_field_index == BS_FIELD_NAME)
				channel->SetName(((BS_StringParamDescriptionField*)current_field->second)->m_text_string);
			current_field++;
		}
		current_param++;
	}

	UpdateParamListInDB();

	return 1;
}

LRESULT CMainFrame::OnListReadyDB(WPARAM, LPARAM)
{
	bool only_change_map = m_MapParamDB.GetCount() > 0;
	CParam *param_db;

	if (!only_change_map)
	{
		KRS_ClearMap(&m_MapParamDB);
	}

	bool need_new_parameter;
	BS_ParamDescription *db_param_description;
	map<int, BS_ParamDescriptionField*>::iterator current_field;
	map<int, BS_ParamDescription>::iterator current_db_param = DB_params_from_db.begin();
	while (current_db_param != DB_params_from_db.end())
	{
		db_param_description = &(current_db_param->second);

		need_new_parameter = !only_change_map;

		if (m_MapParamCM_additional_only.Lookup(db_param_description->m_param_index, param_db))
		{
			param_db->flDB = TRUE;
		}
		
		if (!need_new_parameter)
			need_new_parameter = !m_MapParamDB.Lookup(db_param_description->m_param_index, param_db);
		if (need_new_parameter)
		{
			param_db = new CParam;
			if (param_db == NULL)
			{
				current_db_param++;
				continue;
			}
			param_db->m_num_par = db_param_description->m_param_index;
            param_db->fl_db = 1;

			CParam* template_param;
			if (m_MapParam.Lookup(db_param_description->m_param_index, template_param) && template_param)
			{
				param_db->m_color = template_param->m_color;
				param_db->m_line_width = template_param->m_line_width;
				param_db->sGraphName = template_param->sGraphName;
			}
		}

		if (param_db == NULL)
		{
			current_db_param++;
			continue;
		}

		current_field = db_param_description->m_param_description_fields.begin();
		while (current_field != db_param_description->m_param_description_fields.end())
		{
			int index = current_field->second->m_field_index;
			BS_StringParamDescriptionField* str_ptr = (BS_StringParamDescriptionField*)current_field->second;
			BS_ParamValueParamDescriptionField* pv_ptr = (BS_ParamValueParamDescriptionField*)current_field->second;
			
 			if (current_field->second->m_field_index == BS_FIELD_NAME)
			{
				char *sss = ((BS_StringParamDescriptionField*)current_field->second)->m_text_string;
				param_db->sName = sss;
				if (param_db->sGraphName.GetLength() == 0) 
					param_db->sGraphName = param_db->sName.Left(4);
			}
			if (current_field->second->m_field_index == BS_FIELD_MU)
			{
				char *sss = ((BS_StringParamDescriptionField*)current_field->second)->m_text_string;
				param_db->sRazmernPar = sss;
			}
			if (current_field->second->m_field_index == BS_FIELD_MINIMAL_VALUE)
				param_db->fMin = ((BS_ParamValueParamDescriptionField*)current_field->second)->m_param_value;
			if (current_field->second->m_field_index == BS_FIELD_MAXIMAL_VALUE)
				param_db->fMax = ((BS_ParamValueParamDescriptionField*)current_field->second)->m_param_value;
			if (current_field->second->m_field_index == BS_FIELD_UPPER_BLOCK_VALUE)
				param_db->fBlock = ((BS_ParamValueParamDescriptionField*)current_field->second)->m_param_value;
			if (current_field->second->m_field_index == BS_FIELD_CRUSH_VALUE)
				param_db->fAvaria = ((BS_ParamValueParamDescriptionField*)current_field->second)->m_param_value;
			if (current_field->second->m_field_index == BS_FIELD_GRAPH_DIAPAZON)
				param_db->fDiapazon = ((BS_ParamValueParamDescriptionField*)current_field->second)->m_param_value;
			if (current_field->second->m_field_index == BS_FIELD_GRAPH_MIN)
				param_db->fGraphMIN = ((BS_ParamValueParamDescriptionField*)current_field->second)->m_param_value;
			if (current_field->second->m_field_index == BS_FIELD_GRAPH_MAX)
				param_db->fGraphMAX = ((BS_ParamValueParamDescriptionField*)current_field->second)->m_param_value;
			current_field++;
		}
		if (need_new_parameter)
		{
			m_MapParamDB[param_db->m_num_par] = param_db; 
		}

		current_db_param++;
	}

	CDBView* pView = (CDBView*)pApp->GetView(t_archive);
	if (pView != NULL)
	{
		if (!only_change_map)
			pView->SendMessage(PM_SET_LIST_DB, 0, 0);
		else
			Invalidate(FALSE);
	}

	if (UpdateParamListInDB())
	{
		if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
		{
			PRJ_Project* project = PRJ_GetProject();
			if (project && project->m_ok && !project->m_stages_loaded)
			{
				STAGES_LoadResult res = STAGES_LoadStages(false, false);
				if (res == STAGES_LoadOK)
				{
					project->m_stages_loaded = true;
					PRJ_OnStagesReadyAfterLoad();
				}
				m_wndDialogBarH.GetDlgItem(IDC_RADIO_CEMENT)->EnableWindow(res == STAGES_LoadOK);
			}
		}
		KRS_SendCurrentParamsToBD();
		m_wndDialogBarH.GetDlgItem(IDC_RADIO_ARCHIVE)->EnableWindow(TRUE);
		m_wndDialogBar.m_ind_db.SetState(STATE_ON);

		KRS_SendTOsToDB();
		if (::IsWindow(DlgWaitDB_dlg.m_hWnd))
			DlgWaitDB_dlg.PostMessage(WM_DlgWaitDB_END_DIALOG, IDOK, IDOK);
	}
	return 0;
}

bool CMainFrame::UpdateParamListInDB()
{
	MutexWrap param_db_access(m_params_db_map_mutex);

	if (!DB_connection_info.IsParamListReady())
		return false;

	bool need_to_update, need_to_reqest_db_params = false;
	int num;
	CParam *param, *param_db;

	POSITION pos = m_MapParamDefault.GetStartPosition();
	while (pos != NULL)
	{
		m_MapParamDefault.GetNextAssoc(pos, num, param);
		CHECK(param != NULL);
		if (!param->bParamActive)
			continue;

		need_to_update = false;

		if (m_MapParamDB.Lookup(num, param_db))
		{
			CHECK(param_db != NULL);
			if (param->sName.Compare(param_db->sName) != 0)
				need_to_update = true;
			if (param->sRazmernPar.Compare(param_db->sRazmernPar) != 0)
				need_to_update = true;
			if (fabs(param->fMin - param_db->fMin) > 1e-6)
				need_to_update = true;
			if (fabs(param->fMax - param_db->fMax) > 1e-6)
				need_to_update = true;
			if (fabs(param->fBlock - param_db->fBlock) > 1e-6)
				need_to_update = true;
			if (fabs(param->fAvaria - param_db->fAvaria) > 1e-6)
				need_to_update = true;
			if (fabs(param->fDiapazon - param_db->fDiapazon) > 1e-6)
				need_to_update = true;
			if (fabs(param->fGraphMIN - param_db->fGraphMIN) > 1e-6)
				need_to_update = true;
			if (fabs(param->fGraphMAX - param_db->fGraphMAX) > 1e-6)
				need_to_update = true;
		}
		else
			need_to_update = true;

		if (need_to_update)
		{
			need_to_reqest_db_params = true;
			DB_SendParamDescriptionToDB(param);
		}
	}
	pos = m_MapParamCM_additional_only.GetStartPosition();
	while (pos != NULL)
	{
		m_MapParamCM_additional_only.GetNextAssoc(pos, num, param);
		if (param->flDB != TRUE)
			DB_SendParamDescriptionToDB(param);
	}
	if (need_to_reqest_db_params)
	{
		DB_connection_info.RequestParamList();
		return false;
	}
	return true;
}


LRESULT CMainFrame::OnErrorCONV(WPARAM code, LPARAM)
{
	if (code == BS_CONNECTION_BROKEN)
	{
		m_wndDialogBar.m_ind_conv.SetState(STATE_UNKNOWN);
		CONV_InitConnection();
	}
	return 1;
}

LRESULT CMainFrame::OnInitedCONV(WPARAM, LPARAM)
{
	m_wndDialogBar.m_ind_conv.SetState(STATE_ON);

	{
		int num;
		CParam *param;	
		POSITION pos = m_MapParamCM_additional_only.GetStartPosition();
		while (pos != NULL)
		{
			m_MapParamCM_additional_only.GetNextAssoc(pos, num, param);
			param->flDB = 2;
		}
	}
	CONV_RequestParamInfo();
	CONV_StartMeasure();
	return 1;
}

LRESULT CMainFrame::OnDataSERV(WPARAM, LPARAM)
{
	return 1;
}

LRESULT CMainFrame::OnErrorSERV(WPARAM code, LPARAM)
{
	if (code == BS_CONNECTION_BROKEN)
	{
		m_wndDialogBar.m_ind_serv.SetState(STATE_UNKNOWN);
		SERV_InitConnection();
	}
	return 1;
}

LRESULT CMainFrame::OnInitedSERV(WPARAM, LPARAM)
{
	m_wndDialogBar.m_ind_serv.SetState(STATE_ON);
	return 1;
}

void CMainFrame::BreakDBConnection(bool send_msg)
{
	m_wndDialogBarH.OnRadioRealTime();
	m_wndDialogBarH.CheckRadioButton(IDC_RADIO_ARCHIVE, IDC_RADIO_OTCHET, IDC_RADIO_REAL_TIME);
	m_wndDialogBarH.GetDlgItem(IDC_RADIO_ARCHIVE)->EnableWindow(FALSE);
	DB_ShutDownConnection(true, send_msg);
	DB_InitConnection();
}

void CMainFrame::OnButtonGetParam() 
{
	CONV_RequestParamInfo();
	CONV_StartMeasure();
}

void CMainFrame::OnStartProperty() 
{
	CInitProperty propInit(ID_PROP_INIT, this);
	propInit.DoModal();
}

void CMainFrame::OnEditProject()
{
	DlgWork dlg("Редактирование проекта", DlgWork_EDIT);
	dlg.DoModal();
}

void CMainFrame::OnOpenWork()
{
	DlgWork dlg("Открыть проект", DlgWork_OPEN);
	dlg.DoModal();
}

void CMainFrame::OnNewWork()
{
	DlgWork dlg("Создать новый проект", DlgWork_NEW);
	dlg.DoModal();
}

void CMainFrame::OnImportWork()
{
	DlgWork dlg("Импорт работы", DlgWork_IMPORT);
	dlg.DoModal();
}

void CMainFrame::OnUpdateStartProperty(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gflDisablePassWd && ((KRS_project_flag & KRS_PRJ_FLAG_BURENIE) != 0));
}

UINT SaveThreadProcedure(LPVOID param)
{
	DlgWaiting *dlg = (DlgWaiting*)param;

	while (!::IsWindow(dlg->m_hWnd) || !::IsWindow(dlg->m_progress_bar.m_hWnd) ||PRJ_GetProject()->m_now_saving)
		Sleep(20);

	dlg->m_progress_bar.SetRange32(0, 100);
	dlg->m_progress_bar.SetPos(0);

	PRJ_Project* prj = PRJ_GetProject();
	if (prj->m_ok)
	{
		dlg->SetWindowText(" Сохранение проекта");
		PRJ_SaveProject(PRJ_NowSelectedBranch, PRJ_SaveAllProject, &dlg->m_progress_bar);
	}
	else
		dlg->m_progress_bar.SetPos(100);
	dlg->SignalToClose(IDOK);
	return +1;
}

void CMainFrame::OnMenuPropDb() 
{
	CParamDB dlg;
	if(dlg.DoModal() == IDOK)
	{
		KillTimer(2);
		DlgWaiting dlg(false);
		AfxBeginThread(SaveThreadProcedure, &dlg, THREAD_PRIORITY_ABOVE_NORMAL);
		dlg.DoModal();
		KSR_StopAllConnections();
		PRJ_LoadProject(PRJ_GetProjectPath());
		KSR_StartAllConnections();
		SetTimer(2, 10000, NULL); 
	}
}
void CMainFrame::OnUpdateMenuPropDb(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFlgAccess());
}

void CMainFrame::OnMenuTcpit() 
{
	CDlgNet	dlg;
	if(dlg.DoModal() == IDOK)
	{
		KillTimer(2);
		DlgWaiting dlg(false);
		AfxBeginThread(SaveThreadProcedure, &dlg, THREAD_PRIORITY_ABOVE_NORMAL);
		dlg.DoModal();
		KSR_StopAllConnections();
		PRJ_LoadProject(PRJ_GetProjectPath());
		KSR_StartAllConnections();
		SetTimer(2, 10000, NULL); 
	}
}
void CMainFrame::OnUpdateMenuTcpit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFlgAccess());
}

void CMainFrame::OnKalibrovka() 
{
	CDlgKalibrovka	dlg;
	dlg.DoModal();
}
void CMainFrame::OnUpdateMenuKalibrovka(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CMainFrame::OnStagesLib()
{
	DlgStagesLibrary dlg;
	dlg.DoModal();
}

void CMainFrame::OnStagesLoad()
{
	STAGES_LoadStages(true, false);
	m_wndDialogBarCM.AdjustButtonNames();
}

void CMainFrame::OnStagesSave()
{
	STAGES_SaveStages(true, PRJ_NowSelectedBranch);
}

void CMainFrame::OnStagesChooseFont()
{
	STAGE_ChooseFont(false);
	CCementView* view = (CCementView*)pApp->GetView(gt_cement);
	view->RecalcStagesList();
}

void CMainFrame::OnStagesChooseFontSel()
{
	STAGE_ChooseFont(true);
	CCementView* view = (CCementView*)pApp->GetView(gt_cement);
	view->RecalcStagesList();
}

void CMainFrame::OnStagesGetDB()
{
	STAGES_GetFromDB();
}

void CMainFrame::OnUploadMem()
{
	DlgDevicesMemory_UploadMem();
}
void CMainFrame::OnDownloadMem()
{
	DlgDevicesMemory_DownloadMem();
}
void CMainFrame::OnUpdateMenuDownloadMem(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetFlgAccess());
}

void CMainFrame::OnParamatry()
{
	MapParamDlg dlg;
	dlg.DoModal();
	UpdateParamListInDB();

	CKRSApp* app = (CKRSApp*)AfxGetApp();
}
void CMainFrame::OnUpdateMenuParamatry(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetFlgAccess());
}

#include "DlgCalibrNew.h"

void CMainFrame::OnKalibrovkaM() 
{
	LPARAM lParam = 0;
	CDlgAddParam dlgp(lParam);
	int res = dlgp.DoModal();
	if (res != IDOK)
	{
		if (res == IDABORT)
			MessageBox("Нет каналов для калибровки", " Калибровка невозможна", MB_OK);
		return;
	}
	CParam* param;
	if (!m_MapParam.Lookup(dlgp.m_key, param))
	{
		MessageBox("Параметр не активен: внутренняя ошибка", " Параметр не может быть откалиброван");
		return;
	}
	if (param == NULL)
	{
		MessageBox("Параметр == NULL: внутренняя ошибка", " Параметр не может быть откалиброван");
		return;
	}
	if (param->m_channel_conv_num != 1)
	{
		MessageBox("Калибровке подлежат только параметры каналов внутренней конфигурации", " Параметр не может быть откалиброван");
		return;
	}
	if (param->m_channel_num == 0)
	{
		MessageBox("Параметр не связан ни с каким каналом", " Параметр не может быть откалиброван");
		return;
	}
	
	DlgCalibrNew dlgcn(param, this);
	res = dlgcn.DoModal();
	if (res == IDABORT)
	{
		MessageBox("Устройство не отвечает", " Калибровка невозможна", MB_OK);
	}
	if (res == IDIGNORE)
	{
		MessageBox("Калибровка прервана пользователем", " Калибровка отменена", MB_OK);
	}
	return;
}
void CMainFrame::OnUpdateMenuKalibrovkaM(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFlgAccess());
}


void CMainFrame::OnWk() 
{
	gWk = pKruk->fCur;

	CString cs;
	cs.Format("Wk %0.1f т", gWk);
	m_wndDialogBarH.SetDlgItemText(IDC_STATIC_Wk, cs);
}

void CMainFrame::OnMarkerTypes() 
{
	DlgSelectMarkerType dlg(false);
	dlg.DoModal();
}

void CMainFrame::OnConfigDevices() 
{
	DlgConfigDevices dlg;
	dlg.DoModal();
}
void CMainFrame::OnUpdateMenuConfigDevices(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(gflDisablePassWd);
}

void CMainFrame::OnDebugInfo() 
{
	CDlgDebagParam dlg;
	dlg.DoModal();
}

void CMainFrame::OnSettingsBO() 
{
	DlgSettingsBO dlg;
	dlg.DoModal();
}
void CMainFrame::OnUpdateMenuSettingsBO(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(gflDisablePassWd);
}

int CMainFrame::GetPickAlertion(CParam* pPrm)
{
    float fValue = pPrm->GetAvrg();
    float value = fValue;

    int alert = 0;

    switch(pPrm->nKodControl)
    {
    case 1:
        if(fValue > pPrm->fAvaria) 
			alert = 1;
        break;

    case 2:
		if(fValue > pPrm->fAvaria) alert = 1;
        else if(fValue > pPrm->fMax) alert = 2;
        break;

    case 3:
		if(fValue > pPrm->fAvaria) alert = 1;
		else if(fValue > pPrm->fMax) alert = 2;
		else if(fValue < pPrm->fMin) alert = 3;
        break;
	case 4:
		if(fValue > pPrm->fAvaria) alert = 1;
		else if(fValue < pPrm->fMin) alert = 3;
		break;
	case 5:
		if(fValue > pPrm->fMax) alert = 2;
		break;
	case 6:
		if(fValue > pPrm->fMax) alert = 2;
		else if(fValue < pPrm->fMin) alert = 3;
		break;
	case 7:
		if(fValue < pPrm->fMin) alert = 3;
        break;
    }
    pPrm->numAlert = alert;
    return alert;
}

CString CMainFrame::GetFileAlertion(CParam* pPrm)
{
	if(pPrm->numAlert == 0 || pPrm->nKodControl == 0) return "";

	CString cs = "";

	switch(pPrm->numAlert)
	{
		case 1:
			cs = pPrm->sFile1;
		break;

		case 2:
			cs = pPrm->sFile2;
		break;

		case 3:
			cs = pPrm->sFile3;
		break;
	}
	return cs;
}

void CMainFrame::TestPickAlertParam(CParam* pPrm)
{
	CString nameAlert;
	if(GetPickAlertion(pPrm))
	{
		nameAlert = GetFileAlertion(pPrm);
		if(nameAlert.GetLength() != 0) 
			PlaySound(nameAlert, NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP | SND_NOWAIT);
		
	}
}

void CMainFrame::TestPickAlertAllParam()
{
	int key;
	CParam *pPar;
	POSITION pos;
	
	pos = m_MapParamDB.GetStartPosition();

	while (pos != NULL)
	{
		m_MapParamDB.GetNextAssoc(pos, key, pPar);
		if(pPar != NULL) TestPickAlertParam(pPar);
	}
}
void CMainFrame::TestPickDFAlertParam()
{
}

void CMainFrame::TestAlertParam()
{
}
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	if (cy < 300)
		cy = 300;
	CFrameWnd::OnSize(nType, cx, cy);
}

void CMainFrame::SetViewPanelBur(BOOL flg)
{
    BOOL pan_v = gflgViewPanelBur & flg;
    m_wndDialogBarH.GetDlgItem(IDC_BUTTON_WK__MINUS)->ShowWindow(pan_v);
    m_wndDialogBarH.GetDlgItem(IDC_STATIC_Wk)->ShowWindow(pan_v);
    m_wndDialogBarH.GetDlgItem(IDC_BUTTON_WK__PLUS)->ShowWindow(pan_v);
    m_wndDialogBarH.GetDlgItem(IDC_BUTTON_NULL)->ShowWindow(pan_v);
    m_wndDialogBarH.GetDlgItem(IDC_BUTTON_NULL2_MINUS)->ShowWindow(pan_v);
    m_wndDialogBarH.GetDlgItem(IDC_STATIC_DELTA_TB)->ShowWindow(pan_v);
    m_wndDialogBarH.GetDlgItem(IDC_BUTTON_NULL2_PLUS)->ShowWindow(pan_v);
    m_wndDialogBarH.GetDlgItem(IDC_BUTTON_KNBK)->ShowWindow(pan_v);
    m_wndDialogBarH.GetDlgItem(IDC_BUTTON_LBT_MINUS)->ShowWindow(pan_v);
    m_wndDialogBarH.GetDlgItem(IDC_STATIC_NKBK)->ShowWindow(pan_v);
    m_wndDialogBarH.GetDlgItem(IDC_BUTTON_LBT_PLUS)->ShowWindow(pan_v);
	m_wndDialogBar.SetIndRegimeBur(gflgViewPanelBur);
}
void CMainFrame::SetViewDgtRT()
{
    CView* pView = pApp->GetView(t_real_time);
    if(pView && pApp->IsActiveView(t_real_time))
    {
        ShowControlBar(&(m_wndDialogBar), gflgViewDgtRT, false);
        pView->SendMessage(PM_RECALC_ACTIVE_LIST, 0, 0);
    }
}
void CMainFrame::SetViewDgtBD()
{
    CView* pView = pApp->GetView(t_archive);
    if(pView && pApp->IsActiveView(t_archive))
    {
        ShowControlBar(&(m_wndDialogBarDB), gflgViewDgtBD, false);
        pView->SendMessage(PM_RECALC_ACTIVE_LIST, 0, 0);
    }
}
void CMainFrame::SetHighPanel()
{
    ShowControlBar(&(m_wndDialogBarH), gflgIndHighPanel, false);
    m_wndDialogBar.RecalcPosIndicators();
    pApp->SendMessageToActiveView(PM_RECALC_ACTIVE_LIST, 0, 0);
}
void CMainFrame::SetGraphDigit()
{
    pApp->SendMessageToActiveView(PM_RECALC_ACTIVE_LIST, 0, 0);
}


void CMainFrame::OnUpdateMarkerTypes(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
}

#include "DlgOpisanieProekta.h"
void CMainFrame::OnMenuOpisanieMestorogdenija()
{
	CDlgOpisanieProekta dlg;
	dlg.DoModal();
}

static bool g_closed = false;
static bool g_closing = false;

static bool g_exit_thread_started = false;
static HANDLE g_exit_event = CreateEvent(NULL, FALSE, FALSE, NULL);

UINT ExitThreadProcedure(LPVOID param)
{
	DlgWaiting *dlg = (DlgWaiting*)param;

	while (!::IsWindow(dlg->m_hWnd) || !::IsWindow(dlg->m_progress_bar.m_hWnd) ||PRJ_GetProject()->m_now_saving)
		Sleep(20);

	dlg->m_progress_bar.SetRange32(0, 200);
	dlg->m_progress_bar.SetPos(0);

	KRS_SendCurrentParamsToBD();

	PRJ_Project* prj = PRJ_GetProject();
	if (prj->m_ok)
	{
		dlg->SetWindowText(" Сохранение проекта");
		PRJ_SaveProject(PRJ_NowSelectedBranch, PRJ_SaveAllProject, &dlg->m_progress_bar);
	}
	else
		dlg->m_progress_bar.SetPos(100);

	NET_shutdown = true;
	KRS_shutdown = true;

	dlg->SetWindowText(" Закрытие соединений");
	dlg->m_progress_bar.SetPos(160);
	SERV_ShutDownConnection(true);
	dlg->m_progress_bar.SetPos(170);
	CONV_ShutDownConnection(true);
	dlg->m_progress_bar.SetPos(180);
    DB_ShutDownConnection(true);
    dlg->m_progress_bar.SetPos(190);

	dlg->SetWindowText(" Деинициализация модулей");
	dlg->m_progress_bar.SetPos(195);
	PRJ_CleanUp();
	STAGES_Shutdown();
	dlg->m_progress_bar.SetPos(200);

	Sleep(300);

	dlg->SignalToClose(IDOK);
	return +1;
}

LRESULT CMainFrame::OnStartMessage(WPARAM, LPARAM)
{
	KillTimer(2);
	PRJ_timer_saving_available = false;

	DlgWaiting dlg(false);
	AfxBeginThread(ExitThreadProcedure, &dlg, THREAD_PRIORITY_ABOVE_NORMAL);
	dlg.DoModal();
	g_closed = true;
	PostMessage(WM_CLOSE, 0, 0);
	return +1;
}

void CMainFrame::OnClose()
{
	if (g_closed)
		CMDIFrameWnd::OnClose();
	if (g_closing)
		return;
	if (MessageBox("Действительно хотите прекратить работу программы?", " Выход из программы", MB_YESNO) == IDNO)
		return;
	KRS_running = false;
	g_closing = true;
	PostMessage(WM_START_EXIT, 0, 0);
}

BOOL CMainFrame::GetFlgAccess()
{
	if(gflDisablePassWd) return true;
	else if(gflPassWd) return true;

	return false;
}
void CMainFrame::OnUpdateDebugInfo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(gflDisablePassWd && ((KRS_project_flag & KRS_PRJ_FLAG_BURENIE) != 0));
}

void CMainFrame::OnUpdateEditProject(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFlgAccess());
}

void CMainFrame::OnUpdateOpenWork(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFlgAccess());
}

void CMainFrame::OnUpdateNewWork(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFlgAccess());
}

void CMainFrame::OnUpdateImportWork(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFlgAccess());
}

void CMainFrame::OnUpdateMenuOpisanieMestorogdenija(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMainFrame::OnUpdateUploadMem(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFlgAccess());
}

void CMainFrame::OnUpdateNastroikaModules(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFlgAccess());
}

void CMainFrame::SetParValue(CParam *pPar, float value)
{
	COleDateTime tim = COleDateTime::GetCurrentTime();
	pPar->fCur = value;
	pPar->AddTail(tim.m_dt);
	pPar->fCurAvg = value;
}

void CMainFrame::ChangeflgRabotaBezShurfa()
{
	int flShurf	= abs(pShurf->fCur) > 0 ? true : false;
	int flVes = pKruk->fCurAvg > gVesBlok;
	if(!gflgRabotaBezShurfa)
	{
		gtblElement.SetNumCurElement(0);
		gtblElement.SetSaveCurLen(0);
		double LL;
		if(flVes)
		{
			if(flShurf)
			{
				double zahod = gKvadratDlina - (pTalblock->fCurAvgPrevious - gfKvadratPerehodnik);
				if(zahod > gKvadratDlina) zahod = gKvadratDlina;
				LL = pGlubinaZaboia->fCurAvg - zahod;
			}
			else
			{
				LL = pGlubinaZaboia->fCurAvg + (pTalblock->fCurAvg - gfKvadratElevator);//
			}
		}
		else
		{
			LL = pLinstrumenta->fCurAvgPrevious;
		}

		double komponovka = gtblElement.FindComponovka(LL);
		SetParValue(pLinstrumenta, komponovka);
		gfDLi = LL - pLinstrumenta->fCurAvg;
	}
	else
	{
		SetParValue(pLinstrumenta, gfLinstrumenta + gfDLi);
	}
}

void CMainFrame::SetGlX() 
{
	SetParValue(pGlubina, gGlX);
	if(gGlX < pLinstrumenta->fCur)
	{
		gGlI = gGlX;
		SetGlI();
	}
}

void CMainFrame::SetGlI() 
{
	float LL;
	SetParValue(pGlubinaZaboia, gGlI);

	if(gflgRabotaBezShurfa) 
	{
		float ff = pGlubinaZaboia->fCurAvg + (pTalblock->fCurAvg - gfKvadratElevator);
		SetParValue(pLinstrumenta, ff);
	}
	else
	{
		{
			if(flShurf)
			{
				LL = gGlI + (pTalblock->fCurAvg - gfKvadratPerehodnik - gKvadratDlina);
			}
			else
			{
				LL = gGlI + (pTalblock->fCurAvg - gfKvadratElevator);
			}
		}
		gtblElement.SetNumCurElement(0);
		gtblElement.SetSaveCurLen(0);
		double komponovka = gtblElement.FindComponovka(LL);

		float ff = (float)gtblElement.GetNumCurElement();
		SetParValue(pNkbk, ff);
		SetParValue(pLinstrumenta, komponovka);

		gfDLi = LL - komponovka;
	}
}
