// DlgColorInterface.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "DlgColorInterface.h"
#include "MainView.h"



// CDlgColorInterface dialog
int num_color;
CColorDialog dlg;
UINT_PTR CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

CDlgColorInterface *my_dlg = 0;

void SetColorGrf()
{
	CMainView *pView = (CMainView *)pApp->GetActiveView();
	if(pView == NULL) return;

	ParamOfView *pList;
	for(int i = 0; i < pView->m_parOfView->num_lists; i++) 
	{
		pList = pView->m_parOfView->GetList(i);
		if (pList == NULL)
			continue;
		pList->ChangeGraphColor();
	}
}

void SetColorPanelGrf()
{
	CMainView *pView = (CMainView *)pApp->GetActiveView();
	if(pView == NULL) return;
	pView->Invalidate(true);
	pView->UpdateWindow();
}

void SetColorPanelDgt()
{
	m_wndDialogBar.Invalidate(true);
	m_wndDialogBar.UpdateWindow();
}

IMPLEMENT_DYNAMIC(CDlgColorInterface, CDialog)

CDlgColorInterface::CDlgColorInterface(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColorInterface::IDD, pParent)
{
	my_dlg = this;
	m_color_interface = g_color_interface;
}

CDlgColorInterface::~CDlgColorInterface()
{
}

void CDlgColorInterface::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_COLOR_FIELDGRAPH, m_ind_color_graph_bk);
	DDX_Control(pDX, IDC_STATIC_COLOR_GRF_DGT, m_ind_color_graph_dgt);
	DDX_Control(pDX, IDC_STATIC_COLOR_GRF_GREED, m_ind_color_graph_greed);
	DDX_Control(pDX, IDC_STATIC_COLOR_PANEL_GRF, m_ind_color_panel_grf_bk);
	DDX_Control(pDX, IDC_STATIC_COLOR_PANEL_DGT, m_ind_color_panel_dgt_bk);
	DDX_Control(pDX, IDC_STATIC_COLOR_GRF_DGT_BK, m_ind_color_graph_dgt_bk);
	DDX_Control(pDX, IDC_COMBO_STYLE_GREED, m_StyleGreedGraph);
}


BEGIN_MESSAGE_MAP(CDlgColorInterface, CDialog)
	ON_STN_CLICKED(IDC_STATIC_COLOR_FIELDGRAPH, &CDlgColorInterface::OnClickedStaticColorFieldgraph)
	ON_STN_CLICKED(IDC_STATIC_COLOR_GRF_DGT, &CDlgColorInterface::OnClickedStaticColorGrfDgt)
	ON_STN_CLICKED(IDC_STATIC_COLOR_GRF_GREED, &CDlgColorInterface::OnClickedStaticColorGrfGreed)
	ON_STN_CLICKED(IDC_STATIC_COLOR_PANEL_GRF, &CDlgColorInterface::OnClickedStaticColorPanelGrf)
	ON_STN_CLICKED(IDC_STATIC_COLOR_PANEL_DGT, &CDlgColorInterface::OnClickedStaticColorPanelDgt)
	ON_STN_CLICKED(IDC_STATIC_COLOR_GRF_DGT_BK, &CDlgColorInterface::OnClickedStaticColorGrfDgtBk)
	ON_BN_CLICKED(IDC_BUTTON_RESET_DEFAULT, &CDlgColorInterface::OnBnClickedButtonResetDefault)
END_MESSAGE_MAP()


// CDlgColorInterface message handlers

BOOL CDlgColorInterface::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ind_color_graph_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_bk);
	m_ind_color_graph_bk.SetStateText(STATE_UNKNOWN, "");
	m_ind_color_graph_bk.SetState(STATE_UNKNOWN);

	m_ind_color_graph_dgt.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_dgt);
	m_ind_color_graph_dgt.SetStateText(STATE_UNKNOWN, "");
	m_ind_color_graph_dgt.SetState(STATE_UNKNOWN);

	m_ind_color_graph_greed.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_greed);
	m_ind_color_graph_greed.SetStateText(STATE_UNKNOWN, "");
	m_ind_color_graph_greed.SetState(STATE_UNKNOWN);

	m_ind_color_panel_grf_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_panel_grf_bk);
	m_ind_color_panel_grf_bk.SetStateText(STATE_UNKNOWN, "");
	m_ind_color_panel_grf_bk.SetState(STATE_UNKNOWN);

	m_ind_color_panel_dgt_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_panel_dgt_bk);
	m_ind_color_panel_dgt_bk.SetStateText(STATE_UNKNOWN, "");
	m_ind_color_panel_dgt_bk.SetState(STATE_UNKNOWN);

	m_ind_color_graph_dgt_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_dgt_bk);
	m_ind_color_graph_dgt_bk.SetStateText(STATE_UNKNOWN, "");
	m_ind_color_graph_dgt_bk.SetState(STATE_UNKNOWN);

	m_color_interface = g_color_interface;
	num_color = 0;

	dlg.m_cc.lStructSize = sizeof(CHOOSECOLOR);
	dlg.m_cc.Flags |= CC_ENABLEHOOK | CC_RGBINIT | CC_FULLOPEN;
	dlg.m_cc.lpfnHook = CCHookProc;
	dlg.m_cc.rgbResult = g_color_interface.color_graph_bk;
	int index;
	m_StyleGreedGraph.AddString("PS_SOLID");
	m_StyleGreedGraph.AddString("PS_DASH");
	m_StyleGreedGraph.AddString("PS_DASHDOT");
	m_StyleGreedGraph.AddString("PS_DASHDOTDOT");
	m_StyleGreedGraph.AddString("PS_NULL");

	if(g_StyleGreedGraph == PS_SOLID) index = 0;
	else if(g_StyleGreedGraph == PS_DASH) index = 1;
	else if(g_StyleGreedGraph == PS_DASHDOT) index = 2;
	else if(g_StyleGreedGraph == PS_DASHDOTDOT) index = 3;
	else if(g_StyleGreedGraph == PS_NULL) index = 4;

	m_StyleGreedGraph.SetCurSel(index);

	return TRUE;
}


static UINT old_msg = -1;
DWORD count = 0;

UINT_PTR CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	int aaa = 0;
	if (uiMsg == WM_LBUTTONDOWN) {
		old_msg = uiMsg;
	} else if (uiMsg == WM_LBUTTONUP) {
		old_msg = -1;
	}
	
	if (uiMsg == WM_LBUTTONUP || (old_msg == WM_LBUTTONDOWN && uiMsg == WM_MOUSEMOVE))
	{
		if (GetTickCount() - count < 300) return 0;
		count = GetTickCount();

		CColorDialog *c_dlg = (CColorDialog *)(CWnd::FromHandle(hdlg));
		CWnd* item = c_dlg->GetDlgItem(0x2c5);
		CDC *pdc = item->GetDC();
		COLORREF col = pdc->GetPixel(3, 3);
		CPoint point;
		switch (num_color)
		{
			case 0:
				g_color_interface.color_graph_bk = col;
				my_dlg->m_ind_color_graph_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_bk);
				SetColorGrf();
			break;
			case 1:
				g_color_interface.color_graph_dgt = col;
				my_dlg->m_ind_color_graph_dgt.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_dgt);
				SetColorGrf();
			break;
			case 2:
				g_color_interface.color_graph_greed = col;
				my_dlg->m_ind_color_graph_greed.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_greed);
				SetColorGrf();
			break;
			case 3:
				g_color_interface.color_panel_grf_bk = col;
				my_dlg->m_ind_color_panel_grf_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_panel_grf_bk);
				SetColorPanelGrf();
			break;
			case 4:
				MessageBeep(MB_OK);
				g_color_interface.color_panel_dgt_bk = col;
				my_dlg->m_ind_color_panel_dgt_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_panel_dgt_bk);
				SetColorPanelDgt();
			break;
			case 5:
				MessageBeep(MB_OK);
				g_color_interface.color_graph_dgt_bk = col;
				my_dlg->m_ind_color_graph_dgt_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_dgt_bk);
				SetColorGrf();
			break;
		}
	}
	return 0;
}

void CDlgColorInterface::SetColor()
{
	CMainView *pView = (CMainView *)pApp->GetView(t_real_time);
	if(pView == NULL) return;
	pView->SetColorInterface();

	pView = (CMainView *)pApp->GetView(t_archive);
	if(pView == NULL) return;
	pView->SetColorInterface();

	pView = (CMainView *)pApp->GetView(t_cement_rt);
	if(pView == NULL) return;
	pView->SetColorInterface();
}

void CDlgColorInterface::OnClickedStaticColorFieldgraph()
{
	num_color = 0;
	dlg.m_cc.rgbResult = g_color_interface.color_graph_bk;
	if (dlg.DoModal() == IDCANCEL)	
	{
		g_color_interface = m_color_interface;
		m_ind_color_graph_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_bk);
		SetColor();
	}
}
void CDlgColorInterface::OnClickedStaticColorGrfDgt()
{
	num_color = 1;
	dlg.m_cc.rgbResult = g_color_interface.color_graph_dgt;
	if (dlg.DoModal() == IDOK)
	{
		g_color_interface.color_graph_dgt = dlg.GetColor();
		m_ind_color_graph_dgt.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_dgt);
		SetColor();
	}
}
void CDlgColorInterface::OnClickedStaticColorGrfGreed()
{
	num_color = 2;
	dlg.m_cc.rgbResult = g_color_interface.color_graph_greed;
	if (dlg.DoModal() == IDOK)
	{
		g_color_interface.color_graph_greed = dlg.GetColor();
		m_ind_color_graph_greed.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_greed);
		SetColor();
	}
}
void CDlgColorInterface::OnClickedStaticColorPanelGrf()
{
	num_color = 3;
	dlg.m_cc.rgbResult = g_color_interface.color_panel_grf_bk;
	if (dlg.DoModal() == IDOK)
	{
		g_color_interface.color_panel_grf_bk = dlg.GetColor();
		m_ind_color_panel_grf_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_panel_grf_bk);
		SetColor();
	}
}
void CDlgColorInterface::OnClickedStaticColorPanelDgt()
{
	num_color = 4;
	dlg.m_cc.rgbResult = g_color_interface.color_panel_dgt_bk;
	if (dlg.DoModal() == IDOK)
	{
		g_color_interface.color_panel_dgt_bk = dlg.GetColor();
		m_ind_color_panel_dgt_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_panel_dgt_bk);
		SetColor();
	}
}

void CDlgColorInterface::OnClickedStaticColorGrfDgtBk()
{
	num_color = 5;
	dlg.m_cc.rgbResult = g_color_interface.color_graph_dgt_bk;
	if (dlg.DoModal() == IDOK)
	{
		g_color_interface.color_graph_dgt_bk = dlg.GetColor();
		m_ind_color_graph_dgt_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_dgt_bk);
		SetColor();
	}
}
void CDlgColorInterface::OnOK() 
{
	UpdateData(true);

	int index = m_StyleGreedGraph.GetCurSel();

	if(index == 0) g_StyleGreedGraph = PS_SOLID;
	else if(index == 1) g_StyleGreedGraph = PS_DASH;
	else if(index == 2) g_StyleGreedGraph = PS_DASHDOT;
	else if(index == 3) g_StyleGreedGraph = PS_DASHDOTDOT;
	else if(index == 4) g_StyleGreedGraph = PS_NULL;

	SetColor();
	CDialog::OnOK();
}

void CDlgColorInterface::OnCancel()
{
	UpdateData(true);
	g_color_interface = m_color_interface;
	SetColor();
	CDialog::OnOK();
}

void CDlgColorInterface::OnBnClickedButtonResetDefault()
{
	g_color_interface.color_graph_bk = RGB(204,236,255);
	m_ind_color_graph_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_bk);

	g_color_interface.color_graph_dgt = RGB(0,0,255);
	m_ind_color_graph_dgt.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_dgt);

	g_color_interface.color_graph_greed = RGB(166,202,240);
	m_ind_color_graph_greed.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_greed);

	g_color_interface.color_panel_grf_bk = GetSysColor(COLOR_MENU);
	m_ind_color_panel_grf_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_panel_grf_bk);

	g_color_interface.color_panel_dgt_bk = GetSysColor(COLOR_MENU);
	m_ind_color_panel_dgt_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_panel_dgt_bk);

	g_color_interface.color_graph_dgt_bk = GetSysColor(COLOR_MENU);
	m_ind_color_graph_dgt_bk.SetStateBkColor(STATE_UNKNOWN, g_color_interface.color_graph_dgt_bk);

	SetColor();
}
