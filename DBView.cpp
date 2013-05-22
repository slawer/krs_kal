// DBView.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "MainFrm.h"
#include "DBDoc.h"
#include "DBView.h"


#include "Func.h"

#include "SERV.h"
#include "CONV.h"
#include "DB.h"
#include "DBGET.h"
#include "PRJ.h"

#include "ChangeParam.h"

#include <CHECK.h>

#include "LOG_V0.h"

#include <afxdisp.h>
#include <math.h>


// Кострома мон амур >>
#include "STAGES.h"
// Кострома мон амур <<

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UINT startMarker1RT;
extern UINT startMarker1DB;

extern map<int, BS_ParamDescription> DB_params_from_db;
extern float scaleT[11];
extern float scaleG[9];

extern double TIME_current_time;
extern COleDateTime RequestBegTime, RequestEndTime;

#define DB_NUM_LISTS 5				
#define DB_NUM_FIELDS_GRAPHS 3
#define DB_NUM_FIELDS_PANELS 0

IMPLEMENT_DYNCREATE(CDBView, CMainView)

BEGIN_MESSAGE_MAP(CDBView, CMainView)
	ON_MESSAGE(WM_DATA_DB, OnDataDB)
	ON_MESSAGE(WM_ERROR_DB, OnErrorDB)
	ON_MESSAGE(WM_INITED_DB, OnInitedDB)
	ON_MESSAGE(PM_SET_LIST_DB, OnSetListDB)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CDBView construction/destruction

CDBView::CDBView()
	: CMainView(CDBView::IDD, DB_NUM_LISTS, DB_NUM_FIELDS_GRAPHS, DB_NUM_FIELDS_PANELS)
{
    m_parOfView->name_type_view = "архив";
    m_parOfView->SetNumActiveList(0);
    m_parOfView->flag_time_glub = 1;
    flag_scroll = false;
    m_flag_init = false;
	type_view = t_archive;
}

CDBView::~CDBView()
{
    pDBV = NULL;
}

void CDBView::DoDataExchange(CDataExchange* pDX)
{
    CMainView::DoDataExchange(pDX);
}

void CDBView::ReloadDataView()
{
	m_ptmData	= pgtmDataDB;
	m_pcurGlub	= pgcurGlubDB;

	ClearAllParamsBuffers(&m_MapParamDB, m_ptmData, m_pcurGlub);
	DeleteView();
	InitViews(&m_wndDialogBarDB, &m_MapParamDB);
SetColorInterface();
	m_wndDialogBarDB.m_PanelKnopki.Init(DB_NUM_LISTS, CSize(20,20), this);
	m_wndDialogBarDB.m_PanelKnopki.PushBtn(m_parOfView->GetNumActiveList());

	m_bflag_begin_read_DB = false;
    m_startTime = COleDateTime::GetCurrentTime();	
}

void CDBView::OnInitialUpdate()
{
    if(m_flag_init) return;
    m_flag_init = true;
	CMainView::OnInitialUpdate();

	ReloadDataView();

    KRS_CheckFinishInitialization();
}


// CTestDocView diagnostics

#ifdef _DEBUG
void CDBView::AssertValid() const
{
    CFormView::AssertValid();
}

void CDBView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif //_DEBUG


// CDBView message handlers

/////////////////////////////////////////////////////////////////////////////
// CDBView message handlers
LRESULT CDBView::OnSetListDB(WPARAM wParam, LPARAM lParam)
{
    MessageBeep(MB_OK);
    CString work_path = PRJ_GetWorkPath();
    if (work_path.IsEmpty())
		return 0;
    KRS_app.SetIniFileMode(work_path + "\\Панели.ini");
    RestoreAllLists();
    KRS_app.SetRegistryMode();
    return true;
}

void CDBView::ChangeaActiveListA(WPARAM wParam, LPARAM lParam)
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

    pList->RecalcFieldSize(type_view);
    RecalcScaleFactor(pD->GetNumActiveList());
//    SetParXScale(pD->GetNumActiveList());

    pList->ShowActiveList(true);
    CheckRegim();
    CheckGrf();
}
void CDBView::RButtonDown1A(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 1)
	{
		startMarker1DB = (UINT)lParam;

		if(GetActiveData()->flag_time_glub) m_numContextMenu = 1;
		else m_numContextMenu = 2;
	}
	else m_numContextMenu = wParam;
}
double CDBView::GetStartX(int num_list)
{
	ParamOfView* pV = m_parOfView->GetList(num_list);
	if(m_parOfView->flag_time_glub) 
	{
		if(m_ptmData == NULL) return pV->begTime;
		if(m_ptmData->GetSizeData() == 0) return  pV->begTime;
		else return  m_ptmData->GetElement(startMarker1DB);
	}
	else
	{
		if(m_pcurGlub == NULL) return pV->begGlub;
		if(m_pcurGlub->GetSizeData() == 0) return  pV->begGlub;
		else return  m_pcurGlub->GetElement(startMarker1DB);
	}
}
void CDBView::SetStartX(double start_value, int num_list, BOOL flg)
{
	if(pFrm == NULL) return;
	int nn;
//	GRF_MINMAX par;
	ParamOfView* pV = m_parOfView->GetList(num_list);

//------------------------------------------------------------------------
//	MessageBeep(MB_OK);
	if(flg)
	{
		if(m_parOfView->flag_time_glub) 
		{
			par.FminS = pV->begTime;
			par.FmaxS = pV->endTime;
			nn = (int)pV->gTimeScale;
		}
		else
		{
			par.FminS = pV->begGlub;
			par.FmaxS = pV->endGlub;
			nn = (int)pV->gGlubScale;
		}
	}
	par.Fmin = start_value >= par.FminS ? start_value : par.FminS;
	par.Fmax = par.Fmin + GetLenX(num_list);
//------------------------------------------------------------------------
	if(m_parOfView->flag_time_glub) nn = (int)pV->gTimeScale;
	else nn = (int)pV->gGlubScale;

	par.textGrf = GetStrScale(nn);
	par.flags_minmax.dwFlags = FL_GRF_MINMAX | FL_GRF_TEXT;
	if(flg) par.flags_minmax.dwFlags |= FL_GRF_DIAPAZON; //это только при инициализации
	par.flag_undo = !pV->flg_nonstandart_scale;
	par.flags_minmax.bitFlags.fl_buf_x = 1;
	par.flags_minmax.bitFlags.fl_buf_x_only = 0;

	pV->ChangeScale(pBufX, par, m_parOfView->flag_time_glub);
}

void CDBView::SetScaleDB()
{
	int num_active_list = m_parOfView->GetNumActiveList();
	if (m_parOfView->flag_time_glub)
	{
		if (m_ptmData == NULL || m_ptmData->GetSizeData() == 0)
			gfTimeLengthData = GetLenX(num_active_list); // на случай глюка
		else 
			gfTimeLengthData = m_ptmData->GetLastData() - m_ptmData->GetFirstData();
	}
	else
	{
		if (m_pcurGlub == NULL || m_pcurGlub->GetSizeData() == 0)
			gfGlubLengthData = GetLenX(num_active_list); // на случай глюка
		else 
			gfGlubLengthData = m_pcurGlub->GetLastData() - m_pcurGlub->GetFirstData();
	}
	CClientDC dc(this);
	if (!pFrm)
		return;
	CRect rec;

	ParamOfView *pList = GetActiveList();
	if (pList == NULL) 
		return;

	int dh = pList->GetLengthFieldGrf();
	double dx;
	int i;
	ParamOfView* pV = m_parOfView->GetActive();

	if(m_parOfView->flag_time_glub)
	{
		dx = gfTimeLengthData;
		for(i = 0; i < 11; i++)
		{
			double dd = MinuteToOleTime(dh * GetScaleFactorT(&dc, scaleT[10 - i]));
			if(dd > dx) break;
		}
		if (i == 11)
			i = 10;
		pV->gTimeScale = scaleT[10 - i];
		pV->gDeltaTime = dh * GetScaleFactorT(&dc, pV->gTimeScale);
	}
	else
	{
		dx = gfGlubLengthData;
		for(i = 0; i < 9; i++)
		{
			double dd = dh * GetScaleFactorG(&dc, scaleG[i]);
			if(dd > dx) break;
		}

		//        if(i > 0) i--;
		pV->gGlubScale = scaleG[i];
		pV->gDeltaH = dh * GetScaleFactorG(&dc, pV->gGlubScale);
	}
	pV->flg_nonstandart_scale = false;
	num_active_list = m_parOfView->GetNumActiveList();
	if (m_parOfView->flag_time_glub)
	{
		if(m_ptmData == NULL || m_ptmData->GetSizeData() == 0)
		{
			m_parOfView->GetActive()->begTime = GetCurTime();
			m_parOfView->GetActive()->endTime = m_parOfView->GetActive()->begTime + GetLenX(num_active_list);
		}
		else 
		{
			m_parOfView->GetActive()->begTime = m_ptmData->GetFirstData();
			m_parOfView->GetActive()->endTime = m_parOfView->GetActive()->begTime + GetLenX(num_active_list);
		}
	}
	else
	{
		if(m_pcurGlub == NULL || m_pcurGlub->GetSizeData() == 0)
		{
			m_parOfView->GetActive()->begGlub = 0;
			m_parOfView->GetActive()->endGlub = GetLenX(num_active_list);
		}
		else 
		{
			m_parOfView->GetActive()->begGlub = m_pcurGlub->GetFirstData();
			m_parOfView->GetActive()->endGlub = m_parOfView->GetActive()->begGlub + GetLenX(num_active_list);
		}
	}
}

void Reason2(int flag, CString& str)
{
    if (flag == 0)
        return;
    if (str.GetLength() > 0)
        str += ", ";
    int one_flag = 0;
    if (flag & DB_ValuesRequestType_UpperBlockEvents)
    {
        one_flag = DB_ValuesRequestType_UpperBlockEvents;
        str += "нижнее блокировочное значение";
    }
    else
        if (flag & DB_ValuesRequestType_LowerBlockEvents)
        {
            one_flag = DB_ValuesRequestType_LowerBlockEvents;
            str += "нижнее блокировочное значение";
        }
        else
            if (flag & DB_ValuesRequestType_CrushEvents)
            {
                one_flag = DB_ValuesRequestType_CrushEvents;
                str += "аварийное значение";
            }
            else
                if (flag & DB_ValuesRequestType_MinEvents)
                {
                    one_flag = DB_ValuesRequestType_MinEvents;
                    str += "минимальное значение";
                }
                else
                    if (flag & DB_ValuesRequestType_MaxEvents)
                    {
                        one_flag = DB_ValuesRequestType_MaxEvents;
                        str += "максимальное значение";
                    }
                    CHECK (one_flag > 0);
                    flag ^= one_flag;
                    Reason2(flag, str);
}

CString Reason(int flag)
{
    CString res;
    Reason2(flag, res);
    return res;
}
CString Time(double time)
{
	CString str;
	return str;
}

void DBView_Activity(CWnd* wnd, DB_Packet *packet)
{
	CParam *pPar;
	CDBView* view = (CDBView*)wnd;

	if (packet->m_packet_type == DB_PacketsSet)
	{
		DB_PacketsSetPacket* packet_set = (DB_PacketsSetPacket*)packet;
		list<DB_Packet*>::iterator current = packet_set->m_packets.begin();
		while (current != packet_set->m_packets.end())
		{
			DBView_Activity(wnd, *current);
			current++;
		}
		return;
	}
	else
	if (packet->m_packet_type == DB_TransferParamValues)
	{
		DBGET_HandlePacket((DB_TransferParamValuesPacket*)packet, view);
	}
	else
	if (packet->m_packet_type == DB_TransferSpecialValues)
	{
		MutexWrap list_access(m_wndDialogBarDB.m_dlgEventJrn.m_list_mutex);
		CListCtrl *list = &m_wndDialogBarDB.m_dlgEventJrn.m_LstJourn;
		CHECK(IsWindow(list->m_hWnd));
		list->DeleteAllItems();

		int i, j, index;
		BS_OneParamValueExt* om;
		DB_TransferSpecialValuesPacket* transfer_packet = (DB_TransferSpecialValuesPacket*)packet;
		BS_Measurement *m = transfer_packet->m_measurements;
		for (i = 0; i < transfer_packet->m_measurements_size; i++, m++)
		{
			view->AddTimeGlub(m->m_time, (double)m->m_deepness);
			om = (BS_OneParamValueExt*)m->m_param_values;
			for (j = 0; j < m->m_param_values_size; j++, om++)
			{
				if (!m_MapParamDB.Lookup(om->m_param_index, pPar))
				{
					CString error_str;
					error_str.Format("Нет параметра %d", om->m_param_index);
					CRUSH(error_str);
					return;
				}
				index = list->InsertItem(j, pPar->GetNameForShow());
				list->SetItem(index, 1, LVIF_TEXT, BS_FloatWOZeros(om->m_param_value, 6), 0, 0, 0, NULL);
				list->SetItem(index, 2, LVIF_TEXT, Reason(om->m_flag), 0, 0, 0, NULL);
				list->SetItem(index, 3, LVIF_TEXT, Time(m->m_time), 0, 0, 0, NULL);
			}
		}
		m_wndDialogBarDB.m_progress_db.SetRange32(0, transfer_packet->m_measurements_total); 
		m_wndDialogBarDB.m_progress_db.SetPos(transfer_packet->m_measurements_start_from + transfer_packet->m_measurements_size);
		if (transfer_packet->m_measurements_start_from + transfer_packet->m_measurements_size >= transfer_packet->m_measurements_total)
			m_wndDialogBarDB.FinishRequestDB();
	}
	else
	if (packet->m_packet_type == DB_Report) // рапорт
	{
		DB_ReportPacket *report_packet = (DB_ReportPacket*)packet;
		if (report_packet->m_report_code == 27)
			DBGET_Start();
	}
}


LRESULT CDBView::OnDataDB(WPARAM, LPARAM)
{
	MessageBox("OnDataDB", "Вызов метода потерял смысл. Это ошибка. Сообщите туда, сами знаете куда", MB_OK);
    /*
    DB_Packet *packet;
    while (true)
    {
        packet = DB_Packet::GetFromBuffer(&DB_connection_info.m_in_buffer);
        if (packet == NULL)
            break;
        LOG_V0_AddMessage(LOG_V0_FROM_DB, packet->GetLog());
        AnalyzePacketDB(packet);
        delete packet;
    }
    */
    return 1;
}

LRESULT CDBView::OnErrorDB(WPARAM code, LPARAM)
{
    ASSERT(pFrm);
    if (code == BS_CONNECTION_BROKEN)
    {
        pFrm->BreakDBConnection(false);
    }
    return 0;
}

LRESULT CDBView::OnInitedDB(WPARAM, LPARAM)
{
	return 1;
}


int CDBView::GetActiveParamDB(CParamMap *pActive)
{
	pActive->RemoveAll();
	if(m_parOfView != NULL)
	{
		ParamOfView *pList;
		SaveAllLists();
		for(int i = 0; i < m_parOfView->num_lists; i++) 
		{
			pList = m_parOfView->GetList(i);
			if(pList == NULL) continue;
			pList->AddActiveParam(pActive);
		}
	}
	return pActive->GetCount();
}

void CDBView::ResetIndexParamDB()
{
    int mKey;
    CParam *pPar;

    POSITION pos = m_MapParamDB.GetStartPosition();

    int ii = 0;
    while (pos != NULL)
    {
        m_MapParamDB.GetNextAssoc(pos, mKey, pPar);
        if(!pPar) continue;
        if(!pPar->pBufParam) continue;
        pPar->m_index_of_last_measurement = -1;
        pPar->pBufParam->Clear0();
        pPar->pBufParam->AddFragment(0);
    }
}

bool CDBView::GetDataFromDB(int type_request_db)
{
    CParamMap ActiveParam;
    int mKey;
    CParam *pPar;

	DB_RequestParamValuesPacket packet;

	//GetInfoAllLists();
	PRJ_WritePanels(PRJ_NowSelectedBranch, false);
	ReloadDataView();
	RestoreAllLists();
	KRS_app.SetRegistryMode();
	//SetInfoAllLists();
    pApp->ChangeStyleAllView(FLD_DATA_ON_FIELD, gflgDataBegEnd);
	pApp->ChangeStyleAllView(FLD_LINE_JAGGIES, gflgGrfStupenki);
	pApp->ChangeStyleAllView(FLD_SELECT_ONLY_Y, gflgSelectOnlyY);

	CDataView *pD = GetActiveData();
    if(pD == NULL) return false;
    ParamOfView *pList = GetActiveList();

    if(pList == NULL) return false;
 
	pList->ShowActiveList(false);
	pList->RecalcFieldSize(type_view);
    RecalcScaleFactor(pD->GetNumActiveList());
//    SetParXScale(pD->GetNumActiveList());
	pList->SetVisibilityFlagActiveList();
    pList->ShowActiveList(true);

	if(type_request_db == 0)
	{
		m_parOfView->GetActive()->begTime = RequestBegTime.m_dt;
		m_parOfView->GetActive()->endTime = RequestEndTime.m_dt;

		packet.m_from_time = RequestBegTime;
		packet.m_to_time = RequestEndTime;
	}
	else
	{
		m_parOfView->GetActive()->begGlub = gfRequestGlubStart;
		m_parOfView->GetActive()->endGlub = gfRequestGlubStart + gfRequestGlubLen;

		packet.m_from_time = -gfRequestGlubStart;
		packet.m_to_time = gfRequestGlubStart + gfRequestGlubLen;
	}

//	ClearAllParamsBuffers(&m_MapParamDB, m_ptmData, m_pcurGlub);

    //ResetIndexParamDB();

    SetScaleDB();
    SelectRegime();
    SetParXScale(m_parOfView->GetNumActiveList(), true);

    int nn = GetActiveParamDB(&ActiveParam);

    POSITION pos = ActiveParam.GetStartPosition();

    int ii = 0;
    while (pos != NULL)
    {
        ActiveParam.GetNextAssoc(pos, mKey, pPar);
        if (!pPar)
			continue;
        packet.m_params_indexes.push_back(mKey);
    }
    if (packet.m_params_indexes.size() == 0)
        return false;
    m_bflag_begin_read_DB = true;
    DBGET_Get(&packet);
	return true;
}
void CDBView::RequestDBFinished(int flag_time_glub)
{
	m_parOfView->flag_time_glub = flag_time_glub;
    SelectRegime();

    SetScaleDB();

    int num_list = m_parOfView->GetNumActiveList();
    ParamOfView* pov = m_parOfView->GetList(num_list), *pov2;
	for (int i = 0; i < m_parOfView->num_lists; i++)
	{
		pov2 = m_parOfView->GetList(i);
		if (pov2 == NULL)
			continue;
		pov2->begTime = pov->begTime;
		pov2->endTime = max(pov->endTime, pgtmDataDB->GetLastData());
		pov2->begGlub = pov->begGlub;
		pov2->endGlub = max(pov->endGlub, pgcurGlubDB->GetLastData());
		pov2->gTimeScale = pov->gTimeScale;
		pov2->gDeltaTime = pov->gDeltaTime;
		pov2->gGlubScale = pov->gGlubScale;
		pov2->gDeltaH = pov->gDeltaH;
		SetStartX(m_parOfView->flag_time_glub ? pov->begTime : pov->begGlub, i, true);
	}
//	SetColorInterface();
}
/*
void CDBView::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {	// storing code
        ar << m_parOfView->num_active_list;
        for(int i = 0; i < DB_NUM_LISTS; i++)
        {
            for(int j = 0; j < DB_NUM_FIELDS_GRAPHS; j++)
            {
                ar << m_parOfView->parListView[i]->m_active_grf[j];
            }
            ar << m_parOfView->parListView[i]->gTimeScale;
            ar << m_parOfView->parListView[i]->gGlubScale;
            ar << m_parOfView->parListView[i]->gDeltaTime;
            ar << m_parOfView->parListView[i]->gDeltaH;
			ar << m_parOfView->parListView[i]->m_flag_orient;
       }
    }
    else
    {	// loading code
        ar >> m_parOfView->num_active_list;
        for(int i = 0; i < DB_NUM_LISTS; i++)
        {
            for(int j = 0; j < DB_NUM_FIELDS_GRAPHS; j++)
            {
                ar >> m_parOfView->parListView[i]->m_active_grf[j];
            }
            ar >> m_parOfView->parListView[i]->gTimeScale;
            ar >> m_parOfView->parListView[i]->gGlubScale;
            ar >> m_parOfView->parListView[i]->gDeltaTime;
            ar >> m_parOfView->parListView[i]->gDeltaH;
			ar >> m_parOfView->parListView[i]->m_flag_orient;
        }
    }
}
*/
int CDBView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMainView::OnCreate(lpCreateStruct) == -1)
		return -1;

	pDBV = this;

	return 0;
}
void CDBView::OnSetFocus(CWnd* pOldWnd)
{
	CFormView::OnSetFocus(pOldWnd);
	ASSERT(pFrm);
	CheckRegim();
	CheckGrf();
	pFrm->ChangeTypeView((int)type_view, 1);
}

void CDBView::SetActivityDB()
{
	TRACE0("DBView SetActivity\n");
	DB_AddActivity(this, DB_TransferParamValues, DBView_Activity);
	DB_AddActivity(this, DB_TransferSpecialValues, DBView_Activity);
	DB_AddActivity(this, DB_PacketsSet, DBView_Activity);
	DB_AddActivity(this, DB_Report, DBView_Activity);
}

void CDBView::RemoveActivityDB()
{
	TRACE0("DBView RemoveActivity\n");
	DB_RemoveActivity(this);
}
void CDBView::OnTimer(UINT_PTR nIDEvent)
{
	CMainView::OnTimer(nIDEvent);
}
