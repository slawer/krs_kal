// DlgBar1.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "MainFrm.h"
#include "DlgBar1.h"
#include "Indicftor.h"
#include "KRSView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBar1 dialog


void SetupIndicator(Indicator* ind)
{
	ind->SetStateText(STATE_OFF, "0", false);
	ind->SetStateText(STATE_UNKNOWN, "?", false);
	ind->SetStateText(STATE_ON, "X", false);
	ind->m_state = STATE_UNKNOWN;
}

CDlgBar1::CDlgBar1()
{
	//{{AFX_DATA_INIT(CDlgBar1)
	//}}AFX_DATA_INIT
	//if(m_hWnd != 0)
	{
		SetupIndicator(&m_ind_weight);
		SetupIndicator(&m_ind_pressure);
		SetupIndicator(&m_ind_load);
		SetupIndicator(&m_ind_shurf);

		SetupIndicator(&m_ind_serv);
		SetupIndicator(&m_ind_conv);
		SetupIndicator(&m_ind_db);
		m_ind_db.SetStateText(STATE_OFF, "!", false);
		m_ind_db.SetStateBkColor(STATE_OFF, 0x00FFFF);
		m_ind_db.SetStateTextColor(STATE_OFF, 0x000000);

		m_ind_db.SetStateText(STATE_ERROR, "E", false);
		m_ind_db.SetStateBkColor(STATE_ERROR, 0x0000FF);
		m_ind_db.SetStateTextColor(STATE_ERROR, 0x000000);
	}
}

CDlgBar1::~CDlgBar1()
{
}

void CDlgBar1::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBar1)
	DDX_Control(pDX, IDC_STATIC_REGIM_INDICATOR, m_Regim);
	DDX_Control(pDX, IDC_STATIC_INDICATOR_DATATIME, m_IndDataTime);
	DDX_Control(pDX, IDC_IND_SHURF, m_ind_shurf);
	DDX_Control(pDX, IDC_IND_LOAD, m_ind_load);
	DDX_Control(pDX, IDC_IND_PRESSURE, m_ind_pressure);
	DDX_Control(pDX, IDC_IND_WEIGHT, m_ind_weight);
	DDX_Control(pDX, IDC_IND_SERV, m_ind_serv);
	DDX_Control(pDX, IDC_IND_CONV, m_ind_conv);
	DDX_Control(pDX, IDC_IND_DB, m_ind_db);
	DDX_Control(pDX, IDC_PANEL_KNOPKI, m_PanelKnopki);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBar1, CDialogBar)
	//{{AFX_MSG_MAP(CDlgBar1)
	ON_MESSAGE(WM_INITDIALOG, OnMyInitDialog)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBar1 message handlers

LRESULT CDlgBar1::OnMyInitDialog(WPARAM /*wParam*/, LPARAM /*lParam*/) 
{
	UpdateData(false);
    GetClientRect(&m_rec_s);
    ClientToScreen(&m_rec_s);
    pFrm->ScreenToClient(&m_rec_s);
    return true;
}
void CDlgBar1::OnSize(UINT nType, int cx, int cy)
{
    CRect rec, rec1;
    GetClientRect(&rec1);

    if(m_IndDataTime.m_hWnd != NULL)
    {
        m_IndDataTime.GetClientRect(&rec);
        rec.left = rec1.left;
        rec.right = rec1.right;
        m_IndDataTime.ClientToScreen(&rec);
        ScreenToClient(&rec);
        m_IndDataTime.SetWindowPos(NULL, rec.left, rec.top, rec.Width(), rec.Height(), SWP_NOZORDER);
        m_IndDataTime.Invalidate(true);
    }

    if(m_Regim.m_hWnd != NULL)
    {
        m_Regim.GetClientRect(&rec);
        rec.left = rec1.left;
        rec.right = rec1.right;
        m_Regim.ClientToScreen(&rec);
        ScreenToClient(&rec);
        m_Regim.SetWindowPos(NULL, rec.left, rec.top, rec.Width(), rec.Height(), SWP_NOZORDER);
        m_Regim.Invalidate(true);
    }

    CDialogBar::OnSize(nType, cx, cy);
    if(m_hWnd) RecalcPosIndicators();
}

void CDlgBar1::RecalcPosIndicators()
{
	if(m_hWnd == NULL || m_PanelKnopki.m_hWnd == NULL) return;
    UINT flag = SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER;
    CRect rec;
    GetClientRect(&rec);
    int cx = rec.Width();
    int cy = rec.Height();

    CWnd *w;
    int y = cy;//!gflgIndHighPanel ? cy - 53 : cy - 30;
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
    w->SetWindowPos(NULL, 0, y, cx, 2, flag);

    int sz = 17;
 
    if(gflgViewDiag)
    {
        y -= 3 + (sz + 1);
        m_ind_db.SetWindowPos(NULL, 8, y, 17, 17, flag);
        GetDlgItem(IDC_STATIC_DB)->SetWindowPos(NULL, 30, y, 70, sz, flag);

        if(gflgIndConverter == 0)
        {
            m_ind_conv.ShowWindow(SW_HIDE);
            GetDlgItem(IDC_STATIC_CONV)->ShowWindow(SW_HIDE);
        }
        else 
        {
            m_ind_conv.ShowWindow(SW_SHOW);
            GetDlgItem(IDC_STATIC_CONV)->ShowWindow(SW_SHOW);

            y -= (sz + 1);
            m_ind_conv.SetWindowPos(NULL, 8, y, 17, 17, flag);
            GetDlgItem(IDC_STATIC_CONV)->SetWindowPos(NULL, 30, y, 70, sz, flag);
        }

        y -= (sz + 1);
        m_ind_serv.SetWindowPos(NULL, 8, y, sz, sz, flag);
        GetDlgItem(IDC_STATIC_SERV)->SetWindowPos(NULL, 30, y, 70, sz, flag);
    }

 //   if(gflgViewDiag || gflgIndParBur)
    {
        y -= 6;
        GetDlgItem(IDC_STATIC_SEPARATOR2)->SetWindowPos(NULL, 0, y, cx, 2, flag);
    }
    //else 
  //      GetDlgItem(IDC_STATIC_SEPARATOR2)->ShowWindow(SW_HIDE);

    if(gflgIndParBur)
    {
        y -= 3 + (sz + 1);
        m_ind_shurf.SetWindowPos(NULL, 8, y, sz, sz, flag);
        GetDlgItem(IDC_STATIC_SHURF)->SetWindowPos(NULL, 30, y, 70, sz, flag);

        y -= (sz + 1);
        m_ind_load.SetWindowPos(NULL, 8, y, 17, 17, flag);
        GetDlgItem(IDC_STATIC_LOAD)->SetWindowPos(NULL, 30, y, 110, sz, flag);

        y -= (sz + 1);
        m_ind_pressure.SetWindowPos(NULL, 8, y, 17, 17, flag);
        GetDlgItem(IDC_STATIC_PRESSURE)->SetWindowPos(NULL, 30, y, 70, sz, flag);

        y -= (sz + 1);
        m_ind_weight.SetWindowPos(NULL, 8, y, 17, 17, flag);
        GetDlgItem(IDC_STATIC_WEIGHT)->SetWindowPos(NULL, 30, y, 70, sz, flag);

        y -= 6;
        GetDlgItem(IDC_STATIC_SEPARATOR3)->SetWindowPos(NULL, 0, y, cx, 2, flag);
    }
}

void CDlgBar1::SetViewDiag()
{
    RecalcPosIndicators();

    m_wndDialogBar.GetDlgItem(IDC_STATIC_SEPARATOR1)->ShowWindow(gflgViewDiag);
    m_wndDialogBar.GetDlgItem(IDC_IND_SERV)->ShowWindow(gflgViewDiag);
    m_wndDialogBar.GetDlgItem(IDC_STATIC_SERV)->ShowWindow(gflgViewDiag);
    m_wndDialogBar.GetDlgItem(IDC_IND_CONV)->ShowWindow(gflgViewDiag && gflgIndConverter);
    m_wndDialogBar.GetDlgItem(IDC_STATIC_CONV)->ShowWindow(gflgViewDiag && gflgIndConverter);
    m_wndDialogBar.GetDlgItem(IDC_IND_DB)->ShowWindow(gflgViewDiag);
    m_wndDialogBar.GetDlgItem(IDC_STATIC_DB)->ShowWindow(gflgViewDiag);
    CKRSView* pView = (CKRSView*)pApp->GetView(t_real_time);
    m_wndDialogBar.Invalidate(false);
}

void CDlgBar1::SetIndParamBur()
{
    RecalcPosIndicators();

    m_wndDialogBar.GetDlgItem(IDC_IND_WEIGHT)->ShowWindow(gflgIndParBur);
    m_wndDialogBar.GetDlgItem(IDC_STATIC_WEIGHT)->ShowWindow(gflgIndParBur);
    m_wndDialogBar.GetDlgItem(IDC_IND_PRESSURE)->ShowWindow(gflgIndParBur);
    m_wndDialogBar.GetDlgItem(IDC_STATIC_PRESSURE)->ShowWindow(gflgIndParBur);
    m_wndDialogBar.GetDlgItem(IDC_IND_LOAD)->ShowWindow(gflgIndParBur);
    m_wndDialogBar.GetDlgItem(IDC_STATIC_LOAD)->ShowWindow(gflgIndParBur);
    m_wndDialogBar.GetDlgItem(IDC_IND_SHURF)->ShowWindow(gflgIndParBur);
    m_wndDialogBar.GetDlgItem(IDC_STATIC_SHURF)->ShowWindow(gflgIndParBur);

    m_wndDialogBar.GetDlgItem(IDC_STATIC_SEPARATOR3)->ShowWindow(gflgIndParBur);
    CKRSView* pView = (CKRSView*)pApp->GetView(t_real_time);
    m_wndDialogBar.Invalidate(false);
}
void CDlgBar1::SetIndDataTime()
{
    m_IndDataTime.ShowWindow(gflgIndDataTime);
    CRect rec1, rec2;
    m_IndDataTime.GetClientRect(&rec1);
    m_Regim.GetClientRect(&rec2);
    rec2.OffsetRect(0, int(gflgIndDataTime)*rec1.Height()+3);
    m_Regim.SetWindowPos(NULL, rec2.left, rec2.top, rec2.Width(), rec2.Height(), gflgIndDataTime);
    pApp->SendMessageToView(t_real_time, PM_RECALC_ACTIVE_LIST, 0, 0);
}

void CDlgBar1::SetIndRegimeBur(BOOL flg)
{
	m_Regim.ShowWindow(gflgIndRegimeBur && flg);
	pApp->SendMessageToView(t_real_time, PM_RECALC_ACTIVE_LIST, 0, 0);
}
void CDlgBar1::SetIndConverter()
{
    RecalcPosIndicators();
}
