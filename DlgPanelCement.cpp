// DlgPanelCement.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"
#include "DlgPanelCement.h"
#include "STAGES.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgPanelCement

CDlgPanelCement::CDlgPanelCement()
{
}

CDlgPanelCement::~CDlgPanelCement()
{
}

void CDlgPanelCement::DoDataExchange(CDataExchange* pDX)
{
    CDialogBar::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgPanelCement)
    DDX_Control(pDX, IDC_LIST_WORK, m_list_stages);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgPanelCement, CDialogBar)
    ON_MESSAGE(WM_INITDIALOG, OnMyInitDialog)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CDlgPanelCement::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	m_list_stages.GetClientRect(&rect);
	if (m_list_stages.m_column_width != m_list_stages.m_list_width)
		rect.right = m_list_stages.m_column_width - 1;
	rect.InflateRect(1, 1);
	m_list_stages.ClientToScreen(&rect);
	ScreenToClient(&rect);
	dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(&rect);
}

void CDlgPanelCement::ResizeOtherBars()
{
    if(m_wndDialogBarCM.m_hWnd == NULL) return;
    m_wndDialogBarCM.Invalidate(true);
}

LRESULT CDlgPanelCement::OnMyInitDialog(WPARAM /*wParam*/, LPARAM /*lParam*/) 
{
    UpdateData(false);
    return true;
}

void CDlgPanelCement::RecalcListWidth()
{
    if(m_list_stages.m_hWnd == NULL) return;

	CRect rect;
	GetClientRect(&rect);
	m_list_stages.m_list_width = rect.Width()-4;
	int y = STAGES_zazor + 1;
	int h = rect.Height() - y - STAGES_zazor - 2;
	m_list_stages.SetWindowPos(NULL, 3, y, m_list_stages.m_list_width, h, SWP_NOZORDER);

	m_list_stages.m_column_width = m_list_stages.m_list_width;
	SCROLLINFO si;
	if (m_list_stages.GetScrollInfo(SB_VERT, &si))
	{
		if ((int)si.nPage <= si.nMax)
			m_list_stages.m_column_width -= 16 + STAGES_zazor;
		m_list_stages.SetColumnWidth(0, m_list_stages.m_column_width);
	}
}

void CDlgPanelCement::OnSize(UINT nType, int cx, int cy)
{
    CDialogBar::OnSize(nType, cx, cy);
    RecalcListWidth();
}

void CDlgPanelCement::Serialize(CArchive& ar) 
{
    CDialogBarMain::Serialize(ar);
    if (ar.IsStoring())
    {	
    }
    else
    {	
    }
}
