// DlgInterface.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"
#include "DlgInterface.h"
#include "DlgColorInterface.h"


// CDlgInterface dialog

IMPLEMENT_DYNAMIC(CDlgInterface, CDialog)

CDlgInterface::CDlgInterface(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInterface::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInterface)
	m_flgViewMarkers = FALSE;
	m_flgViewPanelBur = FALSE;
	m_flgViewDiag = FALSE;
	m_flgDataBegEnd = FALSE;
	m_gflgGrfStupenki = false;
	m_gflgScrollAll = false;
	m_gflgIndParBur = false;
	m_gflgIndHighPanel = false;
	m_gflgDgtGraphPanel = false;
	m_gflgIndDataTime = false;
	m_gflgIndConverter = false;
	m_gflgIndRegimeBur = false;
	m_gflgSelectOnlyY = false;
	//}}AFX_DATA_INIT
}

CDlgInterface::~CDlgInterface()
{
}

void CDlgInterface::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInterface)
	DDX_Check(pDX, IDC_CHECK_VIEW_REPER, m_flgViewMarkers);
	DDX_Check(pDX, IDC_CHECK_VIEW_PANEL_BUR, m_flgViewPanelBur);
	DDX_Check(pDX, IDC_CHECK_VIEW_DAGN, m_flgViewDiag);
	DDX_Check(pDX, IDC_CHECK_VIEW_DGT_RT, m_flgViewDgtRT);
	DDX_Check(pDX, IDC_CHECK_VIEW_DGT_BD, m_flgViewDgtBD);
	DDX_Check(pDX, IDC_CHECK_VIEW_REAL_ARCH, m_flgViewRealArch);
	DDX_Check(pDX, IDC_CHECK_VIEW_TIME_GLUB, m_flgViewTimeGlub);
	DDX_Check(pDX, IDC_CHECK_DATA_BEGEND, m_flgDataBegEnd);
	DDX_Check(pDX, IDC_CHECK_STUPENKI, m_gflgGrfStupenki);
	DDX_Check(pDX, IDC_CHECK_SCROLL_ALL, m_gflgScrollAll);
	DDX_Check(pDX, IDC_CHECK_PARAM_BUR, m_gflgIndParBur);
	DDX_Check(pDX, IDC_CHECK_HIGH_PANEL, m_gflgIndHighPanel);
	DDX_Check(pDX, IDC_CHECK_GRAPH_DIGIT, m_gflgDgtGraphPanel);
	DDX_Check(pDX, IDC_CHECK_DATA_TIME, m_gflgIndDataTime);
	DDX_Check(pDX, IDC_CHECK_CONVERTER, m_gflgIndConverter);
	DDX_Check(pDX, IDC_CHECK_REGIME_BUR, m_gflgIndRegimeBur);
	DDX_Check(pDX, IDC_CHECK_SELECT_ONLY_Y, m_gflgSelectOnlyY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInterface, CDialog)
	//{{AFX_MSG_MAP(CDlgInterface)
	ON_BN_CLICKED(IDC_CHECK_VIEW_REPER, OnCheckViewReper)
	ON_BN_CLICKED(IDC_CHECK_VIEW_PANEL_BUR, OnCheckViewPanelBur)
	ON_BN_CLICKED(IDC_CHECK_VIEW_DAGN, OnCheckViewDiag)
	ON_BN_CLICKED(IDC_CHECK_VIEW_DGT_RT, OnCheckViewDgtRT)
	ON_BN_CLICKED(IDC_CHECK_VIEW_DGT_BD, OnCheckViewDgtBD)
	ON_BN_CLICKED(IDC_CHECK_VIEW_REAL_ARCH, OnCheckViewRealArch)
	ON_BN_CLICKED(IDC_CHECK_VIEW_TIME_GLUB, OnCheckViewTimeGlub)
	ON_BN_CLICKED(IDC_CHECK_DATA_BEGEND, OnCheckDataBegEnd)
	ON_BN_CLICKED(IDC_CHECK_STUPENKI, OnCheckGrfStupenki)
	ON_BN_CLICKED(IDC_CHECK_SCROLL_ALL, OnCheckScrollAll)
	ON_BN_CLICKED(IDC_CHECK_PARAM_BUR, OnCheckParamBur)
	ON_BN_CLICKED(IDC_CHECK_HIGH_PANEL, OnCheckHighPanel)
	ON_BN_CLICKED(IDC_CHECK_GRAPH_DIGIT, OnCheckGraphDigit)
	ON_BN_CLICKED(IDC_CHECK_DATA_TIME, OnCheckIndDataTime)
	ON_BN_CLICKED(IDC_CHECK_CONVERTER, OnCheckIndConverter)
	ON_BN_CLICKED(IDC_CHECK_REGIME_BUR, OnCheckIndRegimeBur)
	ON_BN_CLICKED(IDC_CHECK_SELECT_ONLY_Y, OnCheckSelectOnlyY)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_COLOR_INTERFACE, &CDlgInterface::OnBnClickedButtonColorInterface)
END_MESSAGE_MAP()


// CDlgInterface message handlers

BOOL CDlgInterface::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_flgViewMarkers    = gflgViewMarkers;
	m_flgViewPanelBur   = gflgViewPanelBur;
	m_flgViewDiag       = gflgViewDiag;
	m_flgViewDgtRT      = gflgViewDgtRT;
	m_flgViewDgtBD      = gflgViewDgtBD;
	m_flgViewRealArch   = gflgViewRealArch;
	m_flgViewTimeGlub   = gflgViewTimeGlub;
	m_flgDataBegEnd     = gflgDataBegEnd;	
	m_gflgGrfStupenki   = gflgGrfStupenki;	
	m_gflgScrollAll     = gflgScrollAll;
	m_gflgIndParBur     = gflgIndParBur;
	m_gflgIndHighPanel  = gflgIndHighPanel;
	m_gflgDgtGraphPanel = gflgDgtGraphPanel;
	m_gflgIndDataTime   = gflgIndDataTime;
	m_gflgIndConverter  = gflgIndConverter;
	m_gflgIndRegimeBur  = gflgIndRegimeBur;
	m_gflgSelectOnlyY   = gflgSelectOnlyY;
	UpdateData(false);

	return TRUE; 
}

void CDlgInterface::OnOK() 
{
	UpdateData(true);

	gflgViewMarkers	= m_flgViewMarkers;
	gflgViewPanelBur= m_flgViewPanelBur;
	gflgViewDiag    = m_flgViewDiag;
	gflgViewDgtRT   = m_flgViewDgtRT;
	gflgViewDgtBD   = m_flgViewDgtBD;
	gflgViewRealArch= m_flgViewRealArch;
	gflgViewTimeGlub= m_flgViewTimeGlub;
	gflgDataBegEnd  = m_flgDataBegEnd;	
	gflgGrfStupenki  = m_gflgGrfStupenki;	
	gflgScrollAll    = m_gflgScrollAll;
	gflgIndParBur     = m_gflgIndParBur;
	gflgIndHighPanel  = m_gflgIndHighPanel;
	gflgDgtGraphPanel = m_gflgDgtGraphPanel;
	gflgIndDataTime   = m_gflgIndDataTime;
	gflgIndConverter    = m_gflgIndConverter;

	CDialog::OnOK();
}

void CDlgInterface::OnCheckViewReper() 
{
	UpdateData(true);
	gflgViewMarkers	= m_flgViewMarkers;
	pApp->WriteIntegerValueToRegistry("InitData", "flgViewMarkers", gflgViewMarkers);

	ASSERT(pFrm);
	pFrm->ptmData->Set_flg_view_reper(gflgViewMarkers);

	CView* pView = pApp->GetActiveView();
	if(pView != NULL) pView->PostMessage(PM_UPDATE_VIEW, 0, 0);
}
void CDlgInterface::OnCheckViewPanelBur() 
{
	UpdateData(true);
	gflgViewPanelBur	= m_flgViewPanelBur;
	ASSERT(pFrm);
	pFrm->SetViewPanelBur(true);
}
void CDlgInterface::OnCheckViewDiag() 
{
	UpdateData(true);
	gflgViewDiag	= m_flgViewDiag;
	ASSERT(pFrm);
	m_wndDialogBar.SetViewDiag();
}
void CDlgInterface::OnCheckViewDgtRT() 
{
	UpdateData(true);
	gflgViewDgtRT   = m_flgViewDgtRT;
	ASSERT(pFrm);
	pFrm->SetViewDgtRT();
}
void CDlgInterface::OnCheckViewDgtBD() 
{
	UpdateData(true);
	gflgViewDgtBD   = m_flgViewDgtBD;
	ASSERT(pFrm);
	pFrm->SetViewDgtBD();
}
void CDlgInterface::OnCheckViewRealArch() 
{
	UpdateData(true);
	gflgViewRealArch   = m_flgViewRealArch;
	ASSERT(pFrm);
	m_wndDialogBarH.SetViewRealArch();
}
void CDlgInterface::OnCheckViewTimeGlub() 
{
	UpdateData(true);
	gflgViewTimeGlub   = m_flgViewTimeGlub;
	ASSERT(pFrm);
	m_wndDialogBarH.SetViewTimeGlub();
}
void CDlgInterface::OnCheckDataBegEnd() 
{
	UpdateData(true);
	gflgDataBegEnd   = m_flgDataBegEnd;
	pApp->ChangeStyleAllView(FLD_DATA_ON_FIELD, gflgDataBegEnd);
}
void CDlgInterface::OnCheckGrfStupenki() 
{
	UpdateData(true);
	gflgGrfStupenki  = m_gflgGrfStupenki;	
	pApp->ChangeStyleAllView(FLD_LINE_JAGGIES, gflgGrfStupenki);
}
void CDlgInterface::OnCheckScrollAll() 
{
	UpdateData(true);
	gflgScrollAll    = m_gflgScrollAll;
}
void CDlgInterface::OnCheckParamBur() 
{
	UpdateData(true);
	gflgIndParBur    = m_gflgIndParBur;
	ASSERT(pFrm);
	m_wndDialogBar.SetIndParamBur();
}
void CDlgInterface::OnCheckHighPanel() 
{
	UpdateData(true);
	gflgIndHighPanel    = m_gflgIndHighPanel;
	ASSERT(pFrm);
	pFrm->SetHighPanel();
}
void CDlgInterface::OnCheckGraphDigit() 
{
	UpdateData(true);
	gflgDgtGraphPanel    = m_gflgDgtGraphPanel;
	ASSERT(pFrm);
	pFrm->SetGraphDigit();
}
void CDlgInterface::OnCheckIndDataTime() 
{
	UpdateData(true);
	gflgIndDataTime    = m_gflgIndDataTime;
	ASSERT(pFrm);
	m_wndDialogBar.SetIndDataTime();
}
void CDlgInterface::OnCheckIndConverter() 
{
	UpdateData(true);
	gflgIndConverter    = m_gflgIndConverter;
	m_wndDialogBar.SetIndConverter();
}

void CDlgInterface::OnCheckIndRegimeBur() 
{
	UpdateData(true);
	gflgIndRegimeBur    = m_gflgIndRegimeBur;
	m_wndDialogBar.SetIndRegimeBur(true);//
}
void CDlgInterface::OnCheckSelectOnlyY() 
{
	UpdateData(true);
	gflgSelectOnlyY    = m_gflgSelectOnlyY;
	pApp->ChangeStyleAllView(FLD_SELECT_ONLY_Y, gflgSelectOnlyY);
}

void CDlgInterface::OnBnClickedButtonColorInterface()
{
	CDlgColorInterface dlg;
	dlg.DoModal();
}
