// DialogBarDB.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"
#include "DialogBarDB.h"
#include "DBView.h"

#include "DlgEventJrn.h"
#include "DB.h"
#include "TXT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COleDateTime tmStartDBTime;
COleDateTime RequestBegTime, RequestEndTime;

DWORD g_start_tick = 0, g_received_bytes = 0, g_received_time = 0;
extern BOOL FlagChangeScaleRedraw;


/////////////////////////////////////////////////////////////////////////////
// CDialogBarDB dialog


CDialogBarDB::CDialogBarDB()
{
	//{{AFX_DATA_INIT(CDialogBarDB)
	m_iStartTime = 0;
	m_iDiapazonTime = 1;
    m_Hour = _T("0");
    m_Day = _T("1");
    m_Month = _T("1");
    m_Year = _T("07");
    m_TimeLength = _T("1");
    m_GlubStart = _T("0");
    m_GlubLength = _T("100");
    m_CheckTimeGlub = 0;
	//}}AFX_DATA_INIT
}

CDialogBarDB::~CDialogBarDB()
{
}

void CDialogBarDB::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogBarDB)
	DDX_Control(pDX, IDC_STATIC_DB_TEXT, m_static_db);
	DDX_Control(pDX, IDC_PROGRESS_DB, m_progress_db);
    DDX_Radio(pDX, IDC_RADIO_SELCT_TIMEGLUB, m_CheckTimeGlub);
	DDX_Text(pDX, IDC_EDIT_STARTTIME, m_iStartTime);
	DDX_Text(pDX, IDC_EDIT_DIAPAZONTIME, m_iDiapazonTime);
    DDX_Text(pDX, IDC_EDIT_MIN, m_Minut);
    DDX_Text(pDX, IDC_EDIT_HOUR, m_Hour);
    DDX_Text(pDX, IDC_EDIT_DAY, m_Day);
    DDX_Text(pDX, IDC_EDIT_MONTH, m_Month);
    DDX_Text(pDX, IDC_EDIT_YEAR, m_Year);
    DDX_Text(pDX, IDC_EDIT_INTERVAL_TIME, m_TimeLength);
    DDX_Text(pDX, IDC_EDIT_INTERVAL_MINUT, m_TimeLengthMinut);
    DDX_Text(pDX, IDC_EDIT_START_GLUB, m_GlubStart);
    DDX_Text(pDX, IDC_EDIT_LEN_GLUB, m_GlubLength);
	DDX_Control(pDX, IDC_PANEL_KNOPKI, m_PanelKnopki);
//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogBarDB, CDialogBar)
	//{{AFX_MSG_MAP(CDialogBarDB)
	ON_BN_CLICKED(IDC_BUTTON_SELECTDAY, OnButtonSelectDay)
	ON_BN_CLICKED(IDC_BUTTON_SELECTFREE2, OnButtonSelectFree2)
	ON_BN_CLICKED(IDC_BUTTON_OTCHET4, OnButtonOtchet4)
	ON_BN_CLICKED(IDC_BUTTON_OTCHETFREE2, OnButtonOtchetFree2)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_SELECTDAY, OnUpdateControl)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_SELECTFREE2, OnUpdateControl)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_OTCHET4, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_OTCHETFREE2, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_BREAK_DB_REQUEST, OnUpdateEnable)
	ON_MESSAGE(WM_INITDIALOG, OnMyInitDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_BREAK_DB_REQUEST, OnButtonBreakDbRequest)
    ON_EN_CHANGE(IDC_EDIT_MINUT, OnChangeEditMinut)
    ON_EN_CHANGE(IDC_EDIT_HOUR, OnChangeEditHour)
    ON_EN_CHANGE(IDC_EDIT_DAY, OnChangeEditDay)
    ON_EN_CHANGE(IDC_EDIT_MONTH, OnChangeEditMonth)
    ON_EN_CHANGE(IDC_EDIT_YEAR, OnChangeEditYear)
    ON_EN_CHANGE(IDC_EDIT_INTERVAL_TIME, OnChangeEditTimeLen)
    ON_EN_CHANGE(IDC_EDIT_INTERVAL_MINUT, OnChangeEditTimeLenMinut)
    ON_EN_CHANGE(IDC_EDIT_START_GLUB, OnChangeEditGlubStart)
    ON_EN_CHANGE(IDC_EDIT_LEN_GLUB, OnChangeEditGlubLen)
    ON_BN_CLICKED(IDC_BUTTON_DO_REQUEST, OnButtonDbRequest)
    ON_BN_CLICKED(IDC_RADIO_SELCT_TIMEGLUB, OnRadioCheckTimeGlub)
    ON_BN_CLICKED(IDC_RADIO_SELCT_TIMEGLUB2, OnRadioCheckTimeGlub)
    ON_UPDATE_COMMAND_UI(IDC_BUTTON_DO_REQUEST, OnUpdateControl)
 //}}AFX_MSG_MAP
 ON_EN_KILLFOCUS(IDC_EDIT_MINUT, &CDialogBarDB::OnEnKillfocusEditMinut)
 ON_EN_KILLFOCUS(IDC_EDIT_HOUR, &CDialogBarDB::OnEnKillfocusEditHour)
 ON_EN_KILLFOCUS(IDC_EDIT_DAY, &CDialogBarDB::OnEnKillfocusEditDay)
 ON_EN_KILLFOCUS(IDC_EDIT_MONTH, &CDialogBarDB::OnEnKillfocusEditMonth)
 ON_EN_KILLFOCUS(IDC_EDIT_YEAR, &CDialogBarDB::OnEnKillfocusEditYear)
 ON_EN_KILLFOCUS(IDC_EDIT_INTERVAL_TIME, &CDialogBarDB::OnEnKillfocusEditIntervalTime)
 ON_EN_KILLFOCUS(IDC_EDIT_INTERVAL_MINUT, &CDialogBarDB::OnEnKillfocusEditIntervalTimeMinut)
 ON_WM_SIZE()
END_MESSAGE_MAP()


void CDialogBarDB::SetCheckControl(int bflag_chek)
{
    m_CheckTimeGlub = bflag_chek;
    UpdateData(false);
//    GetClientRect(&m_rec_s);
//    ClientToScreen(&m_rec_s);
//    pFrm->ScreenToClient(&m_rec_s);
	CheckControl(1);
}

void CDialogBarDB::CheckControl(int flg_callback)
{
    ASSERT(pFrm);
	CView* pView;
    /*if(pApp == NULL)*/ pApp = (CKRSApp*)AfxGetApp();
    pView = pApp->GetView(t_archive);
    if(pView == NULL) return;

    if(m_CheckTimeGlub == 0)
    {
        GetDlgItem(IDC_EDIT_MINUT)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_HOUR)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_DAY)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_MONTH)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_YEAR)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_INTERVAL_TIME)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_INTERVAL_MINUT)->EnableWindow(true);

        GetDlgItem(IDC_EDIT_START_GLUB)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_LEN_GLUB)->EnableWindow(false);

        ((CEdit*)GetDlgItem(IDC_EDIT_MINUT))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_MINUT))->SetSel(0,2,0);

		pView->SendMessage(PM_CHANGESCALE, 1, flg_callback);
	}
    else
    {
        GetDlgItem(IDC_EDIT_MINUT)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_HOUR)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_DAY)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_MONTH)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_YEAR)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_INTERVAL_TIME)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_INTERVAL_MINUT)->EnableWindow(false);

        GetDlgItem(IDC_EDIT_START_GLUB)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_LEN_GLUB)->EnableWindow(true);

		((CEdit*)GetDlgItem(IDC_EDIT_START_GLUB))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_START_GLUB))->SetSel(0,4,0);

		pView->SendMessage(PM_CHANGESCALE, 0, flg_callback);
	}
}

LRESULT CDialogBarDB::OnMyInitDialog(WPARAM /*wParam*/, LPARAM /*lParam*/) 
{
    UpdateData(false);
    ((CEdit*)GetDlgItem(IDC_EDIT_MINUT))->SetLimitText(2);
    ((CEdit*)GetDlgItem(IDC_EDIT_HOUR))->SetLimitText(2);
    ((CEdit*)GetDlgItem(IDC_EDIT_DAY))->SetLimitText(2);
    ((CEdit*)GetDlgItem(IDC_EDIT_MONTH))->SetLimitText(2);
    ((CEdit*)GetDlgItem(IDC_EDIT_YEAR))->SetLimitText(2);
    ((CEdit*)GetDlgItem(IDC_EDIT_INTERVAL_TIME))->SetLimitText(2);
    ((CEdit*)GetDlgItem(IDC_EDIT_INTERVAL_MINUT))->SetLimitText(2);
    ((CEdit*)GetDlgItem(IDC_EDIT_START_GLUB))->SetLimitText(4);
    ((CEdit*)GetDlgItem(IDC_EDIT_LEN_GLUB))->SetLimitText(4);

    CheckControl();
    RecalcPosIndicators();
    return true;
}

static bool g_enabled_controls = true;
/////////////////////////////////////////////////////////////////////////////
// CDialogBarDB message handlers
void CDialogBarDB::OnUpdateControl(CCmdUI* pCmdUi)
{
	pCmdUi->Enable(g_enabled_controls);
}

void CDialogBarDB::OnUpdateEnable(CCmdUI* pCmdUi)
{
	pCmdUi->Enable(true);
}

void CDialogBarDB::OnButtonBreakDbRequest() 
{
    ASSERT(pFrm);
	if (MessageBox("Прервать обращение к базе данных?", "Прерывание запроса", MB_YESNO) == IDNO)
		return;
	GetDlgItem(IDC_BUTTON_BREAK_DB_REQUEST)->ShowWindow(SW_HIDE);

	FinishRequestDB();
	pFrm->BreakDBConnection(false);
}

void CDialogBarDB::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		if (!g_enabled_controls)
			GetDlgItem(IDC_BUTTON_BREAK_DB_REQUEST)->ShowWindow(SW_NORMAL);
		KillTimer(1);
	}
	if (nIDEvent == 2)
	{
		DWORD ticks = GetTickCount() - g_start_tick;
		int ms = ticks % 1000;
		ticks /= 1000;
		int s = ticks % 60;
		ticks /= 60;
		int m = ticks % 60;
/*		ticks /= 60;
		int h = ticks % 24;
*/
		SetDlgItemText(IDC_STATIC_TIMER, (TXT("%02d:%02d.%d")<<m<<s<<(ms/100)).GetChars());
	}
	CDialogBar::OnTimer(nIDEvent);
}

void CDialogBarDB::StartRequestDB()
{
	g_enabled_controls = false;

	m_progress_db.SetRange32(0, 10);
	m_progress_db.SetPos(0);
	m_progress_db.ShowWindow(SW_NORMAL);
	m_static_db.ShowWindow(SW_NORMAL);

	m_wndDialogBarH.EnableViewSwitchers(FALSE);

	GetDlgItem(IDC_EDIT_MINUT)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_HOUR)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_DAY)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_MONTH)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_YEAR)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_INTERVAL_TIME)->EnableWindow(false);
    GetDlgItem(IDC_EDIT_INTERVAL_MINUT)->EnableWindow(false);

	GetDlgItem(IDC_EDIT_START_GLUB)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_LEN_GLUB)->EnableWindow(false);
	GetDlgItem(IDC_RADIO_SELCT_TIMEGLUB)->EnableWindow(false);
	GetDlgItem(IDC_RADIO_SELCT_TIMEGLUB2)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_DO_REQUEST)->EnableWindow(false);

//	GetDlgItem(IDC_STATIC_TIMER)->ShowWindow(SW_NORMAL);

	SetTimer(1, 1000/**60*5*/, NULL);
	g_start_tick = GetTickCount();
	g_received_bytes = 0;
	g_received_time = 0;
	OnTimer(2);
	IncrementBytes(0);
	IncrementTime(0);
	SetTimer(2, 50, NULL);


    ASSERT(pFrm);
	CDBView* pView = (CDBView*)pApp->GetView(t_archive);
	if (pView == NULL || !pView->GetDataFromDB(m_CheckTimeGlub))
	{
		FinishRequestDB();
		return;
	}
}

#include "LOG_V0.h"

void CDialogBarDB::FinishRequestDB()
{
	m_progress_db.ShowWindow(SW_HIDE);
	m_static_db.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_BREAK_DB_REQUEST)->ShowWindow(SW_HIDE);

	m_wndDialogBarH.EnableViewSwitchers(TRUE);

	GetDlgItem(IDC_RADIO_SELCT_TIMEGLUB)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_SELCT_TIMEGLUB2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DO_REQUEST)->EnableWindow(TRUE);
	CheckControl();

//	GetDlgItem(IDC_STATIC_TIMER)->ShowWindow(SW_HIDE);
	KillTimer(1);
	KillTimer(2);

	CDBView* db_view = (CDBView*)KRS_app.GetView(t_archive);
    if (db_view == NULL) return;
 
	int flag = db_view->GetActiveData()->flag_time_glub;
	ParamOfView* pov = db_view->GetActiveList();
	pov->ShowActiveList(false);
	//FlagChangeScaleRedraw = false;
    db_view->RequestDBFinished(!flag);
    db_view->RequestDBFinished(flag);
	pov->ShowActiveList(true);
	//FlagChangeScaleRedraw = true;
	//db_view->RedrawActiveView();

	g_enabled_controls = true;
}

void CDialogBarDB::IncrementBytes(int bytes)
{
	g_received_bytes += bytes;
	SetDlgItemInt(IDC_STATIC_BYTES, g_received_bytes);
}

void CDialogBarDB::IncrementTime(DWORD inc_ticks)
{
	g_received_time += inc_ticks;
	DWORD ticks = g_received_time;
	int ms = ticks % 1000;
	ticks /= 1000;
	int s = ticks % 60;
	ticks /= 60;
	int m = ticks % 60;
	SetDlgItemText(IDC_STATIC_TIMER2, (TXT("%02d:%02d.%d")<<m<<s<<(ms/100)).GetChars());
}

bool CDialogBarDB::CheckTimeInterval()
{
	if (!UpdateData(TRUE))
		return false;

	if (m_iStartTime > 23)
	{
		MessageBox("Начальное время должно быть в диапазоне 0-23", "Неправильно задан интервал времени");
		return false;
	}
	if (m_iDiapazonTime == 0)
	{
		MessageBox("Интервал запроса должен быть не меньше 1 часа", "Неправильно задан интервал времени");
		return false;
	}
	if (m_iStartTime + m_iDiapazonTime > 24)
	{
		m_iDiapazonTime = 24 - m_iStartTime;
		UpdateData(FALSE);
	}
	return true;
}

void CDialogBarDB::OnButtonSelectDay() 
{
	if (!CheckTimeInterval())
		return;
/*
	tmStartDBTime = COleDateTime::GetCurrentTime();

	begTime = tmStartDBTime.m_dt;
	SYSTEMTIME sytime;

	begTime.GetAsSystemTime(sytime);

	int tmin = 0;
	int ttm = m_iStartTime + m_iDiapazonTime;
	if(ttm > 23) 
	{
		m_iDiapazonTime = 23 - m_iStartTime;
		ttm = m_iStartTime + m_iDiapazonTime;
		tmin = 59;
	}

	begTime.SetDateTime(sytime.wYear, sytime.wMonth, sytime.wDay, m_iStartTime, 0, 0);
	endTime.SetDateTime(sytime.wYear, sytime.wMonth, sytime.wDay, ttm, tmin, 0);

	StartRequestDB();
*/
}

void CDialogBarDB::OnButtonSelectFree2() 
{
	if (!CheckTimeInterval())
		return;
/*
	CDlgSelectDataDay dlgSelectData;
	if(dlgSelectData.DoModal() == IDCANCEL) return;

	tmStartDBTime = dlgSelectData.m_DataDay;

	begTime = dlgSelectData.m_DataDay.m_dt;
	SYSTEMTIME sytime;

	begTime.GetAsSystemTime(sytime);

	int tmin = 0;
	int ttm = m_iStartTime + m_iDiapazonTime;
	if(ttm > 23) 
	{
		m_iDiapazonTime = 23 - m_iStartTime;
		ttm = m_iStartTime + m_iDiapazonTime;
		tmin = 59;
	}

	begTime.SetDateTime(sytime.wYear, sytime.wMonth, sytime.wDay, m_iStartTime, 0, 0);
	endTime.SetDateTime(sytime.wYear, sytime.wMonth, sytime.wDay, ttm, tmin, 0);

	StartRequestDB();
*/
}

void CDialogBarDB::OnButtonOtchet4() 
{
	m_dlgEventJrn.typeTime = 0;
	COleDateTime time = COleDateTime::GetCurrentTime();
	m_dlgEventJrn.bgTime = int(time.m_dt);
	m_dlgEventJrn.enTime = int(time.m_dt) + 1.0;
	m_dlgEventJrn.DoModal();
}

void CDialogBarDB::OnButtonOtchetFree2() 
{
	CDlgSelectDataDay dlgSelectData;
	if(dlgSelectData.DoModal() == IDCANCEL) return;

	COleDateTime begTime = dlgSelectData.m_DataDay.m_dt, endTime;
	SYSTEMTIME sytime;

	begTime.GetAsSystemTime(sytime);
	begTime.SetDateTime(sytime.wYear, sytime.wMonth, sytime.wDay, 0, 0, 0);
	endTime.m_dt = begTime.m_dt + 1.0;

	m_dlgEventJrn.typeTime = 4;
	m_dlgEventJrn.enTime = begTime.m_dt;
	m_dlgEventJrn.bgTime = endTime.m_dt;
	m_dlgEventJrn.DoModal();
}
//////////////////////////////////////////////////////////////////////////////////////

void CDialogBarDB::OnRadioCheckTimeGlub()
{
    UpdateData(true);
    CheckControl();
}
void CDialogBarDB::SetValidData(CString& inp_str, int& value, int max_len, DWORD next_control)
{
	sscanf(inp_str, "%d", &value);
	GetDlgItem(next_control)->SetFocus();
	((CEdit*)GetDlgItem(next_control))->SetSel(0,max_len,0);
}
void CDialogBarDB::SetInValidData(CString& inp_str, int& value, int max_len, DWORD cur_control)
{
	inp_str.Format("%02d", value);
	UpdateData(false);
	GetDlgItem(cur_control)->SetFocus();
	((CEdit*)GetDlgItem(cur_control))->SetSel(0,max_len,0);
}

void CDialogBarDB::GetDataFromEdit(CString& inp_str, int& value, int max_len, int min_value, int max_value, 
								   DWORD cur_control, DWORD next_control)
{
	UpdateData(true);
	if(inp_str.GetLength() == max_len) 
	{
		int nn = 0;
		sscanf(inp_str, "%d", &nn);
		if(nn < min_value || nn > max_value) SetInValidData(inp_str, value, max_len, cur_control);
		else SetValidData(inp_str, value, max_len, next_control);
	}
}
void CDialogBarDB::TestKillFocus(CString& inp_str, int& value, int max_len, int min_value, int max_value, 
								   DWORD cur_control, DWORD next_control)
{
	//if(!TestFocusItem(next_control)) return;
	if(inp_str.GetLength() == 0) SetInValidData(inp_str, value, max_len, cur_control);
	else if(inp_str.GetLength() == 1)
	{
		int nn = 0;
		sscanf(inp_str, "%d", &nn);
		if(nn < min_value || nn > max_value) SetInValidData(inp_str, value, max_len, cur_control);
		else SetValidData(inp_str, value, max_len, next_control);
	}
}

void CDialogBarDB::OnChangeEditMinut() 
{
    GetDataFromEdit(m_Minut, giMinut, 2, 0, 59, IDC_EDIT_MINUT, IDC_EDIT_HOUR);
}
void CDialogBarDB::OnChangeEditHour() 
{
	GetDataFromEdit(m_Hour, giHour, 2, 0, 23, IDC_EDIT_HOUR, IDC_EDIT_DAY);
}
void CDialogBarDB::OnChangeEditDay() 
{
	GetDataFromEdit(m_Day, giDay, 2, 1, 31, IDC_EDIT_DAY, IDC_EDIT_MONTH);
}
void CDialogBarDB::OnChangeEditMonth() 
{
	GetDataFromEdit(m_Month, giMonth, 2, 1, 12, IDC_EDIT_MONTH, IDC_EDIT_YEAR);
}
void CDialogBarDB::OnChangeEditYear() 
{
	GetDataFromEdit(m_Year, giYear, 2, 7, 99, IDC_EDIT_YEAR, IDC_EDIT_INTERVAL_TIME);
}
void CDialogBarDB::OnChangeEditTimeLen() 
{
	GetDataFromEdit(m_TimeLength, giTimeLength, 2, 0, 23, IDC_EDIT_INTERVAL_TIME, IDC_EDIT_INTERVAL_MINUT);
}
void CDialogBarDB::OnChangeEditTimeLenMinut() 
{
    GetDataFromEdit(m_TimeLengthMinut, giTimeLengthMinut, 2, 0, 59, IDC_EDIT_INTERVAL_MINUT, IDC_BUTTON_DO_REQUEST);
}
void CDialogBarDB::OnChangeEditGlubStart() 
{
	GetDataFromEdit(m_GlubStart, gfRequestGlubStart, 4, 0, 9999, IDC_EDIT_START_GLUB, IDC_EDIT_LEN_GLUB);
}
void CDialogBarDB::OnChangeEditGlubLen() 
{
	GetDataFromEdit(m_GlubLength, gfRequestGlubLen, 4, 0, 9999, IDC_EDIT_LEN_GLUB, IDC_BUTTON_DO_REQUEST);
}

BOOL CDialogBarDB::TestFocusItem(DWORD next_control)
{
	CWnd *pWnd = GetDlgItem(next_control);
	CWnd *pWnd1 = GetFocus();
	if(pWnd == NULL || pWnd1 == NULL) return false;
	if(pWnd1->m_hWnd == pWnd->m_hWnd) return true;
	return false;

}

void CDialogBarDB::OnEnKillfocusEditMinut()
{
    TestKillFocus(m_Minut, giMinut, 2, 0, 59, IDC_EDIT_MINUT, IDC_EDIT_HOUR);
}
void CDialogBarDB::OnEnKillfocusEditHour()
{
	TestKillFocus(m_Hour, giHour, 2, 0, 23, IDC_EDIT_HOUR, IDC_EDIT_DAY);
}

void CDialogBarDB::OnEnKillfocusEditDay()
{
	TestKillFocus(m_Day, giDay, 2, 1, 31, IDC_EDIT_DAY, IDC_EDIT_MONTH);
}

void CDialogBarDB::OnEnKillfocusEditMonth()
{
	TestKillFocus(m_Month, giMonth, 2, 1, 12, IDC_EDIT_MONTH, IDC_EDIT_YEAR);
}

void CDialogBarDB::OnEnKillfocusEditYear()
{
	TestKillFocus(m_Year, giYear, 2, 7, 99, IDC_EDIT_YEAR, IDC_EDIT_INTERVAL_TIME);
}

void CDialogBarDB::OnEnKillfocusEditIntervalTime()
{
	TestKillFocus(m_TimeLength, giTimeLength, 2, 0, 23, IDC_EDIT_INTERVAL_TIME, IDC_EDIT_INTERVAL_MINUT);
}
void CDialogBarDB::OnEnKillfocusEditIntervalTimeMinut()
{
    TestKillFocus(m_TimeLengthMinut, giTimeLengthMinut, 2, 0, 59, IDC_EDIT_INTERVAL_MINUT, IDC_BUTTON_DO_REQUEST);
}

void CDialogBarDB::SaveControl()
{
    UpdateData(true);
    sscanf(m_Minut, "%02d", &giMinut);
    sscanf(m_Hour, "%02d", &giHour);
    sscanf(m_Day, "%02d", &giDay);
    sscanf(m_Month, "%02d", &giMonth);
    sscanf(m_Year, "%02d", &giYear);
    sscanf(m_TimeLength, "%02d", &giTimeLength);
    sscanf(m_TimeLengthMinut, "%02d", &giTimeLengthMinut);
    sscanf(m_GlubStart, "%d", &gfRequestGlubStart);
    sscanf(m_GlubLength, "%d", &gfRequestGlubLen);
    gflgCheckTimeGlub = m_CheckTimeGlub;
}
void CDialogBarDB::OnButtonDbRequest() 
{
    if (!CheckTimeInterval())
        return;

    SaveControl();

    RequestBegTime = COleDateTime(giYear + 2000, giMonth, giDay, giHour, giMinut, 0); 
    if (RequestBegTime.m_status != COleDateTime::DateTimeStatus::valid)
		return;
    COleDateTimeSpan d_time = COleDateTimeSpan(0, giTimeLength, giTimeLengthMinut, 0);
    if (d_time.m_status != COleDateTimeSpan::DateTimeSpanStatus::valid)
		return;
    RequestEndTime = RequestBegTime + d_time;

    StartRequestDB();
}
void CDialogBarDB::RecalcPosIndicators()
{
	if(m_hWnd == NULL || m_PanelKnopki.m_hWnd == NULL) return;
    UINT flag = SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER;
    CRect rec;
    GetClientRect(&rec);
    int cx = rec.Width();
    int cy = rec.Height();

    CWnd *w;
    int y = cy;
    int bh = 20;
	int w0 = cx - 2*STAGES_zazor;

	if(m_PanelKnopki.GetCount() == 0)
	{
		m_PanelKnopki.ShowWindow(SW_HIDE);
	}
	else
	{
		CSize sz;
		sz = m_PanelKnopki.GetSize();
		y -= STAGES_zazor + sz.cy;
		m_PanelKnopki.SetWindowPos(NULL, STAGES_zazor, y, w0, sz.cy, SWP_NOZORDER);
		m_PanelKnopki.RepositionBtn();
		CSize new_sz = m_PanelKnopki.GetSize();
		if (new_sz.cy != sz.cy)
		{
			y += sz.cy - new_sz.cy;
			m_PanelKnopki.SetWindowPos(NULL, STAGES_zazor, y, w0, new_sz.cy, SWP_NOZORDER);
		}				
		m_PanelKnopki.ShowWindow(SW_SHOW);
	}
	
	y -= 4;
	w = GetDlgItem(IDC_STATIC_SEPARATOR1);
	if(w == 0) return;
	w->SetWindowPos(NULL, 0, y, 300, 2, flag);

    int sz = 17;
    {
        int y_db = y - 5 - 10;
        GetDlgItem(IDC_PROGRESS_DB)->SetWindowPos(NULL, 5, y_db, 174, 10, flag);
        y_db -= (sz + 1);
        GetDlgItem(IDC_STATIC_DB_TEXT)->SetWindowPos(NULL, 5, y_db, 130, sz, flag);
        GetDlgItem(IDC_STATIC_TIMER)->SetWindowPos(NULL, 139, y_db, 40, sz, flag);
        y_db -= (sz + 1);
        GetDlgItem(IDC_BUTTON_BREAK_DB_REQUEST)->SetWindowPos(NULL, 5, y_db, 174, sz, flag);
        y_db -= (sz + 1);
        GetDlgItem(IDC_STATIC_BYTES)->SetWindowPos(NULL, 75, y_db, 104, sz, flag);
    }
}

void CDialogBarDB::OnSize(UINT nType, int cx, int cy)
{
    CDialogBar::OnSize(nType, cx, cy);
    RecalcPosIndicators();
}

void CDialogBarDB::Serialize(CArchive& ar) 
{
    if (ar.IsStoring())
    {	
        ar << giMinut;
        ar << giHour;
        ar << giDay;
        ar << giMonth;
        ar << giYear;
        ar << giTimeLength;
        ar << giTimeLengthMinut;
        ar << gfRequestGlubStart;
        ar << gfRequestGlubLen;
        ar << gflgCheckTimeGlub;
    }
    else
    {	
        ar >> giMinut;
        ar >> giHour;
        ar >> giDay;
        ar >> giMonth;
        ar >> giYear;
        ar >> giTimeLength;
        ar >> giTimeLengthMinut;
        ar >> gfRequestGlubStart;
        ar >> gfRequestGlubLen;
        ar >> gflgCheckTimeGlub;

        m_Minut.Format("%02d", giMinut);
		m_Hour.Format("%02d", giHour);
		m_Day.Format("%02d", giDay);
		m_Month.Format("%02d", giMonth);
		m_Year.Format("%02d", giYear);
		m_TimeLength.Format("%02d", giTimeLength);
        m_TimeLengthMinut.Format("%02d", giTimeLengthMinut);
		m_GlubStart.Format("%d", gfRequestGlubStart);
		m_GlubLength.Format("%d", gfRequestGlubLen);

		UpdateData(false);
		CheckControl();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectData dialog


CDlgSelectData::CDlgSelectData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectData)
	m_DataBeg = COleDateTime::GetCurrentTime();
	m_DataEnd = COleDateTime::GetCurrentTime();
	//}}AFX_DATA_INIT
}


void CDlgSelectData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectData)
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_DataBeg);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_DataEnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectData, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectData)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectData message handlers

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectDataDay dialog


CDlgSelectDataDay::CDlgSelectDataDay(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectDataDay::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectDataDay)
	m_DataDay = COleDateTime::GetCurrentTime();
	//}}AFX_DATA_INIT
}


void CDlgSelectDataDay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectDataDay)
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_DataDay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectDataDay, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectDataDay)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectDataDay message handlers


