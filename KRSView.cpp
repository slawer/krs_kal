// KRSView.cpp : implementation of the CKRSView class
//

#include "stdafx.h"
#include "KRS.h"
#include "MainFrm.h"
#include "KRSDoc.h"
#include "KRSView.h"
#include "Func.h"

#include "SERV.h"
#include "CONV.h"
#include "DB.h"

#include "ChangeParam.h"

#include <CHECK.h>

#include "LOG_V0.h"

#include <afxdisp.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UINT startMarker1RT;
extern UINT startMarker1DB;

extern map<int, BS_ParamDescription> DB_params_from_db;

extern double TIME_current_time;
extern COleDateTime RequestBegTime, RequestEndTime;

#define RT_NUM_LISTS 5				
#define RT_NUM_FIELDS_GRAPHS 3
#define RT_NUM_FIELDS_PANELS 3

IMPLEMENT_DYNCREATE(CKRSView, CMainView)

BEGIN_MESSAGE_MAP(CKRSView, CMainView)
	ON_MESSAGE(PM_DATAACP, OnReadDataMessage)
    ON_MESSAGE(PM_SYNCHRONIZE_GRAPH, OnSynchronizeGraph)
	ON_MESSAGE(PM_EDITPOINT, OnLButtonDown1)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

CKRSView::CKRSView()
: CMainView(CKRSView::IDD, RT_NUM_LISTS, RT_NUM_FIELDS_GRAPHS, RT_NUM_FIELDS_PANELS)
{
    m_parOfView->name_type_view = "נואכüםמו גנולÿ";
    m_parOfView->SetNumActiveList(0);
    m_parOfView->flag_time_glub = 1;
	flag_scroll = false;
    m_flag_init = false;
	type_view = t_real_time;
}

CKRSView::~CKRSView()
{
    pRealV = NULL;
}

void CKRSView::DoDataExchange(CDataExchange* pDX)
{
    CMainView::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CKRSView)
    //}}AFX_DATA_MAP
}

LRESULT CKRSView::OnSynchronizeGraph(WPARAM wParam, LPARAM lParam)
{
	if(pApp->IsActiveView(type_view))
		GetActiveList()->SynchronizeGraph(int(wParam));
    return true;
}

void CKRSView::ReloadDataView()
{
	m_ptmData	= pgtmData;
	m_pcurGlub	= pgcurGlub;

	ClearAllParamsBuffers(&m_MapParam, m_ptmData, m_pcurGlub);
	DeleteView();
	InitViews(&m_wndDialogBar, &m_MapParam);

	CheckRegim();
	m_wndDialogBar.m_PanelKnopki.Init(RT_NUM_LISTS, CSize(20,20), this);
	m_wndDialogBar.m_PanelKnopki.PushBtn(m_parOfView->GetNumActiveList());
	CheckGrf();
}

void CKRSView::OnInitialUpdate()
{
    if(m_flag_init) return;
    m_flag_init = true;
    CMainView::OnInitialUpdate();

	ReloadDataView();

    KRS_CheckFinishInitialization();

	pRealV = this;

	CClientDC dc(this);
	G_HORZRES  = dc.GetDeviceCaps(HORZRES);
	G_VERTSIZE = dc.GetDeviceCaps(VERTRES);
}

void CKRSView::ChangeaActiveListA(WPARAM wParam, LPARAM lParam)
{
    CDataView *pD = GetActiveData();
    if(pD == NULL) return;
    ParamOfView *pList = GetActiveList();
    if(pList == NULL) return;

	if(pD->GetNumActiveList() == int(wParam)) return;
	pList->ShowActiveList(false);
	if (wParam != -1)
		pD->SetNumActiveList(int(wParam));

    pList = GetActiveList();
    if(pList == NULL) return;

	ReadDataMessageA();
	pList->RecalcFieldSize(type_view);
	RecalcScaleFactor(pD->GetNumActiveList());
//	SetParXScale(pD->GetNumActiveList());

	pList->ShowActiveList(true);
	CheckRegim();
	CheckGrf();
}

void CKRSView::RButtonDown1A(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 1)
	{
		startMarker1RT = (UINT)lParam;

		if(GetActiveData()->flag_time_glub) m_numContextMenu = 1;
		else m_numContextMenu = 2;
	}
	else m_numContextMenu = wParam;
}
double CKRSView::GetStartX(int num_list)
{
	double start_value;
    if(GetActiveData() == NULL) return 0;

	if(m_parOfView->flag_time_glub)
	{
		if(m_ptmData == NULL) return GetCurTime();
		if(m_ptmData->GetSizeData() == 0) start_value = GetCurTime();
		else 
		{
			if(m_ptmData->GetLastData() - m_ptmData->GetFirstData() < GetLenX(num_list)) 
				start_value = m_ptmData->GetFirstData();
			else start_value = m_ptmData->GetLastData() - GetLenX(num_list);
		}
	}
	else
	{
		if(m_pcurGlub == NULL)
			return (pGlubina == NULL) ? 0 : pGlubina->fCur;
		if(m_pcurGlub->GetSizeData() == 0) start_value = pGlubina->fCur;
		else 
		{
			if(m_pcurGlub->GetLastData() - m_pcurGlub->GetFirstData() < GetLenX(num_list)) 
				start_value = m_pcurGlub->GetFirstData();
			else start_value = m_pcurGlub->GetLastData() - GetLenX(num_list);
		}
	}
	return start_value;
}

void CKRSView::SetStartX(double start_value, int num_list, BOOL flg)
{
	if(pFrm == NULL) return;
	int nn;
    ParamOfView* pV = m_parOfView->GetList(num_list);
	if (pV == NULL) return;

	if(m_parOfView->flag_time_glub) nn = (int)pV->gTimeScale;
	else nn = (int)pV->gGlubScale;

	GRF_MINMAX par;
	par.Fmin = start_value;
	par.Fmax = start_value + GetLenX(num_list);
	par.FminS = par.Fmin;
	par.FmaxS = par.Fmax;
	par.textGrf = GetStrScale(nn);
	par.flags_minmax.dwFlags = FL_GRF_MINMAX | FL_GRF_TEXT;
    if(flg) par.flags_minmax.dwFlags |= FL_GRF_DIAPAZON; 
	par.flag_undo = !pV->flg_nonstandart_scale;
	par.flags_minmax.bitFlags.fl_buf_x = 1;
	par.flags_minmax.bitFlags.fl_buf_x_only = 0;
	
	pV->ChangeScale(pBufX, par, m_parOfView->flag_time_glub);
}
LRESULT CKRSView::OnLButtonDown1(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 150258)
	{
		flag_scroll = lParam;
		SetParXScale(m_parOfView->GetNumActiveList());
	}
	return true;
}
BOOL CKRSView::ReadDataMessageA()
{
	if(TIME_current_time == 0) return false;
	int nn1 = GetTickCount();

    CDataView *pD = GetActiveData();
	if(pD == NULL) return false;
	ParamOfView *pV = GetActiveList();
	if(pV == NULL) return 0;
	GetActiveList()->UpdateInputData(1);
	int nn2 = GetTickCount();
	return true;
}


int CKRSView::ClearBeginAllData()
{
	MessageBeep(MB_OK);

	int nn = int((MAX_SIIZE_BUF * 15.0)/100.0);

	int i;
	ParamOfView *pList;
	for(i = 0; i < m_parOfView->num_lists; i++) 
	{
		pList = m_parOfView->GetList(i);
		if (pList != NULL) pList->ClearBeginDataY(nn);
	}

	int mm = pgtmData->ClearBeginData(nn);
	pgcurGlub->ClearBeginData(mm);

	for(i = 0; i < m_parOfView->num_lists; i++) 
	{
		pList = m_parOfView->GetList(i);
		if (pList != NULL) pList->ResetFlagsClearedY();
	}

	return mm;
}

int CKRSView::TestSizeRingBuf()
{
	if(pgtmData == NULL) return 0;
	int nn = pgtmData->GetFreePositions();
	if(nn < 10) 
		return ClearBeginAllData();
	return 0;
}

LRESULT CKRSView::OnReadDataMessage(WPARAM wParam, LPARAM lParam)
{
	ReadDataMessageA();
	if(TestSizeRingBuf() > 0) 
		UpdateActiveList();

	int nn2 = GetTickCount();
	return true;
}

int CKRSView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMainView::OnCreate(lpCreateStruct) == -1)
		return -1;

	pRealV = this;
	return 0;
}

void CKRSView::OnSetFocus(CWnd* pOldWnd)
{
	CFormView::OnSetFocus(pOldWnd);
	ASSERT(pFrm);

	CheckRegim();
	CheckGrf();
	pFrm->ChangeTypeView((int)type_view, 0);
}

void CKRSView::OnTimer(UINT_PTR nIDEvent)
{
	CMainView::OnTimer(nIDEvent);
}

