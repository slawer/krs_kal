// DLgBarH.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "DLgBarH.h"
#include "MainFrm.h"
#include "DBView.h"
#include "CementView.h"
#include "KRSView.h"

#include "ApiFunc.h"
#include "DlgEditText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int KNBKList;
extern CString sKNBK;

/////////////////////////////////////////////////////////////////////////////
// CDLgBarH dialog


CDLgBarH::CDLgBarH()
{
	//{{AFX_DATA_INIT(CDLgBarH)
	//}}AFX_DATA_INIT
	m_type_view = 0;
}

CDLgBarH::~CDLgBarH()
{
}

void CDLgBarH::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	OutputWk(0, false);
	OutputNKBK(0);
	OutputTalblock(0);

	//{{AFX_DATA_MAP(CDLgBarH)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLgBarH, CDialogBar)
	ON_MESSAGE(WM_INITDIALOG, OnMyInitDialog)
	//{{AFX_MSG_MAP(CDLgBarH)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_WK__MINUS, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_WK__PLUS, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_NULL, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_NULL2_MINUS, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_NULL2_PLUS, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_KNBK, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_LBT_MINUS, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_LBT_PLUS, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_START_STOP, OnUpdateEnable)
	ON_BN_CLICKED(IDC_BUTTON_WK__MINUS, OnButtonWkMinus)
	ON_BN_CLICKED(IDC_BUTTON_WK__PLUS, OnButtonWkPlus)
	ON_BN_CLICKED(IDC_BUTTON_NULL, OnButtonNull)
	ON_BN_CLICKED(IDC_BUTTON_NULL2_MINUS, OnButtonNull2Minus)
	ON_BN_CLICKED(IDC_BUTTON_NULL2_PLUS, OnButtonNull2Plus)
	ON_BN_CLICKED(IDC_BUTTON_LBT_MINUS, OnButtonLbtMinus)
	ON_BN_CLICKED(IDC_BUTTON_LBT_PLUS, OnButtonLbtPlus)
	ON_BN_CLICKED(IDC_RADIO_ARCHIVE, OnRadioArchive)
	ON_BN_CLICKED(IDC_RADIO_REAL_TIME, OnRadioRealTime)
    ON_BN_CLICKED(IDC_RADIO_CEMENT, OnRadioCement)
	ON_BN_CLICKED(IDC_RADIO_OTCHET, OnRadioOtchet)
	ON_BN_CLICKED(IDC_BUTTON_KNBK, OnButtonKnbk)
	ON_BN_CLICKED(IDC_STATIC_DELTA_TB, OnStaticDeltaTB)
	ON_BN_CLICKED(IDC_STATIC_Wk, OnSTATICWk)
	ON_BN_CLICKED(IDC_STATIC_NKBK, OnStaticNkbk)
	ON_BN_CLICKED(IDC_BUTTON_START_STOP, OnButtonStartStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLgBarH message handlers

int CDLgBarH::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;


	return 0;
}

LRESULT CDLgBarH::OnMyInitDialog(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CButton *b;
	b = (CButton*)GetDlgItem(IDC_BUTTON_WK__MINUS);
	b->SetIcon(KRS_app.LoadIcon(IDI_ICON_MINUS));
	b = (CButton*)GetDlgItem(IDC_BUTTON_WK__PLUS);
	b->SetIcon(KRS_app.LoadIcon(IDI_ICON_PLUS));
	b = (CButton*)GetDlgItem(IDC_BUTTON_NULL2_MINUS);
	b->SetIcon(KRS_app.LoadIcon(IDI_ICON_MINUS));
	b = (CButton*)GetDlgItem(IDC_BUTTON_NULL2_PLUS);
	b->SetIcon(KRS_app.LoadIcon(IDI_ICON_PLUS));
	b = (CButton*)GetDlgItem(IDC_BUTTON_LBT_MINUS);
	b->SetIcon(KRS_app.LoadIcon(IDI_ICON_MINUS));
	b = (CButton*)GetDlgItem(IDC_BUTTON_LBT_PLUS);
	b->SetIcon(KRS_app.LoadIcon(IDI_ICON_PLUS));


	b = (CButton*)GetDlgItem(IDC_BUTTON_START_STOP);
	b->SetIcon(KRS_app.LoadIcon(KRS_running?IDI_ICON_STOP:IDI_ICON_START));

	return true;
}

void CDLgBarH::EnableViewSwitchers(BOOL enable)
{
	m_wndDialogBarH.GetDlgItem(IDC_RADIO_ARCHIVE)->EnableWindow(enable);
	m_wndDialogBarH.GetDlgItem(IDC_RADIO_REAL_TIME)->EnableWindow(enable);
	m_wndDialogBarH.GetDlgItem(IDC_RADIO_OTCHET)->EnableWindow(enable);
	m_wndDialogBarH.GetDlgItem(IDC_RADIO_CEMENT)->EnableWindow(enable);
}

void CDLgBarH::OnUpdateEnable(CCmdUI* pCmdUi)
{
	pCmdUi->Enable(true);
}

void CDLgBarH::OutputWk(float dWk, bool send)
{
	gWk += dWk;
	CString cs;
	cs.Format("Wk %0.1f т", gWk);
	SetDlgItemText(IDC_STATIC_Wk, cs);
	if (send)
		SERV_SendParamToBKSD(pKruk, gWk);
}

void CDLgBarH::OutputTalblock(float dTB)
{
	if ((KRS_project_flag & KRS_PRJ_FLAG_BURENIE) == 0)
		return;

	gfTalblokDeltaH += dTB;
	CString cs;
	cs.Format("%0.1f м", pTalblock->fCurAvg);
	SetDlgItemText(IDC_STATIC_DELTA_TB, cs);
}
void CDLgBarH::OutTextNKBNK()
{
	CString cs;
	int cur = gtblElement.GetNumCurElement();
	cs.Format("№%d %s", cur, gtblElement.GetName(cur));
	SetDlgItemText(IDC_STATIC_NKBK, cs);
}
void CDLgBarH::OutputNKBK(int dkbk)
{
	if (dkbk != 0)
	{
		gtblElement.GetLenFromBegToCurElem();
		gtblElement.SetNumCurElement(gtblElement.GetNumCurElement() + dkbk);
	}
	OutTextNKBNK();
}
void CDLgBarH::OnButtonWkMinus() 
{
	OutputWk(-0.1f, true);
}
void CDLgBarH::OnButtonWkPlus() 
{
	OutputWk(+0.1f, true);
}
void CDLgBarH::OnButtonNull() 
{
	gflagButtonNull = true;	
}
void CDLgBarH::OnButtonNull2Minus() 
{
	gfTalblokDeltaH -= 0.1f;
}

void CDLgBarH::OnButtonNull2Plus() 
{
	gfTalblokDeltaH += 0.1f;
}

void CDLgBarH::OnButtonLbtMinus() 
{
	OutputNKBK(-1);
}

void CDLgBarH::OnButtonLbtPlus() 
{
	OutputNKBK(+1);
}

void CDLgBarH::HideShowTimeDeepnessButtons(bool visible)
{
    GetDlgItem(IDC_RADIO_SCALE_TIM)->ShowWindow(gflgViewTimeGlub && visible);
    GetDlgItem(IDC_RADIO_SCALE_GLUB)->ShowWindow(gflgViewTimeGlub && visible);
}

void CDLgBarH::OnRadioArchive() 
{
	if(pFrm == NULL) return;
	HideShowTimeDeepnessButtons(true);


	if (m_type_view == t_cement_rt)
	{
		CCementView* cement_view = (CCementView*)pApp->GetView(t_cement_rt);
		cement_view->RemoveActivityDB();
	}

	m_type_view = 1;

	CDBView* db_view = (CDBView*)pApp->GetView(t_archive);
	db_view->SetActivityDB();

	pApp->SetActiveView(t_archive);
}

void CDLgBarH::OnRadioRealTime() 
{
	if(pFrm == NULL) return;
	HideShowTimeDeepnessButtons(true);


	if (m_type_view == t_archive)
	{
		CDBView* db_view = (CDBView*)pApp->GetView(t_archive);
		db_view->RemoveActivityDB();
	}
	else
	if (m_type_view == t_cement_rt)
	{
		CCementView* cement_view = (CCementView*)pApp->GetView(t_cement_rt);
		cement_view->RemoveActivityDB();
	}

	m_type_view = 0;

	pApp->SetActiveView(t_real_time);
}

void CDLgBarH::OnRadioCement() 
{
	if(pFrm == NULL) return;
	pFrm->OnRadioScaleTim(); 
	HideShowTimeDeepnessButtons(false); 

	if (m_type_view == t_archive)
	{
		CDBView* db_view = (CDBView*)pApp->GetView(t_archive);
		db_view->RemoveActivityDB();
	}

	m_type_view = 2;

	CCementView* cement_view = (CCementView*)pApp->GetView(t_cement_rt);
	cement_view->SetActivityDB();

	pApp->SetActiveView(t_cement_rt);
}

void CDLgBarH::OnRadioOtchet() 
{
	if(pFrm == NULL) return;

	if (m_type_view == t_archive)
	{
		CDBView* db_view = (CDBView*)pApp->GetView(t_archive);
		db_view->RemoveActivityDB();
	}
	else
	if (m_type_view == t_cement_rt)
	{
		CCementView* cement_view = (CCementView*)pApp->GetView(t_cement_rt);
		cement_view->RemoveActivityDB();
	}

	m_type_view = 4;
	pFrm->SendMessage(PM_CHANGE_TYPE_VIEW, 2, 0);

	CView* pView = pApp->GetActiveView();
	if(pView != NULL) pView->SendMessage(PM_CHANGE_TYPE_VIEW, 2, 0);
}

void StartTimeOut(CParamMap& map_param)
{
	CParam* param;
	int num;
	POSITION pos = map_param.GetStartPosition();
	while (pos != NULL)
	{
		map_param.GetNextAssoc(pos, num, param);
		if (param == NULL)
			continue;
		if (param->bParamActive)
		{
			param->StartTimeOut();
			param->m_prev_calc_status = FORMULA_CALC_STATUS_TIMEOUT;
		}
	}
}

void CDLgBarH::OnButtonStartStop() 
{
	KRS_running = !KRS_running;
	if (!KRS_running)
	{
		StartTimeOut(m_MapParamDefault);
		if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
			StartTimeOut(m_MapParamCM_additional_only);
		KRS_SendTOsToDB();
	}
	CButton* b = (CButton*)GetDlgItem(IDC_BUTTON_START_STOP);
	b->SetIcon(KRS_app.LoadIcon(KRS_running?IDI_ICON_STOP:IDI_ICON_START));
}

void CDLgBarH::OnButtonKnbk() 
{
	KNBK(KNBKList, gKNBKPath);
}

void CDLgBarH::OnSTATICWk() 
{
	CRect r;
	GetDlgItem(IDC_STATIC_Wk)->GetWindowRect(&r);
	r.DeflateRect(2, 2);

	CString str;
	gWk = pKruk->fCurAvg;
	str.Format("%.1f", gWk);
	DlgEditText dlg(str, r, false, 0);
	if (dlg.DoModal() == IDOK)
	{
		str = dlg.GetText();
		str.Replace(",", ".");
		float fl;
		if (sscanf(str, "%f", &fl) != 1)
			MessageBox("Неправильный формат надписи", "Параметр не будет изменён");
		else
		{
			gWk = fl;
			OutputWk(0, true);
		}
	}
}

void CDLgBarH::OnStaticDeltaTB() 
{
	CRect r;
	GetDlgItem(IDC_STATIC_DELTA_TB)->GetWindowRect(&r);
	r.DeflateRect(2, 2);

	CString str;
	str.Format("%.1f", pTalblock->fCurAvg);
	DlgEditText dlg(str, r, false, 0);
	if (dlg.DoModal() == IDOK)
	{
		str = dlg.GetText();
		str.Replace(",", ".");
		float fl;
		if (sscanf(str, "%f", &fl) != 1)
			MessageBox("Неправильный формат надписи", "Параметр не будет изменён");
		else
		{
			if(fl < 0) fl = 0;
			float IT = pTalblock->fCurAvg - gfTalblokDeltaH;
			gfTalblokDeltaH = -(IT - fl);
		}
	}
}

void CDLgBarH::OnStaticNkbk() 
{
	CRect r;
	GetDlgItem(IDC_STATIC_NKBK)->GetWindowRect(&r);
	r.DeflateRect(2, 2);

	CString str;
	str.Format("%d", gtblElement.GetNumCurElement());
	DlgEditText dlg(str, r, false, 0);
	if (dlg.DoModal() == IDOK)
	{
		str = dlg.GetText();
		str.Replace(",", ".");
		int i;
		if (sscanf(str, "%d", &i) != 1)
			MessageBox("Неправильный формат надписи", "Параметр не будет изменён");
		else
		{
			gtblElement.SetNumCurElement(i);
			OutputNKBK(0);
		}
	}	
}

void CDLgBarH::SetViewRealArch()
{
    GetDlgItem(IDC_RADIO_ARCHIVE)->ShowWindow(gflgViewRealArch);
    GetDlgItem(IDC_RADIO_REAL_TIME)->ShowWindow(gflgViewRealArch);
    GetDlgItem(IDC_RADIO_CEMENT)->ShowWindow(gflgViewRealArch && (KRS_project_flag & KRS_PRJ_FLAG_STAGES));
}
void CDLgBarH::SetViewTimeGlub()
{
	HideShowTimeDeepnessButtons(true);

	int style = gflgViewTimeGlub ? SW_SHOW : SW_HIDE;
	CWnd* w = &m_wndDialogBarDB;
	w->GetDlgItem(IDC_STATIC_R)->ShowWindow(style);
	w->GetDlgItem(IDC_RADIO_SELCT_TIMEGLUB2)->ShowWindow(style);
	w->GetDlgItem(IDC_EDIT_START_GLUB)->ShowWindow(style);
	w->GetDlgItem(IDC_STATIC_START_GLUB)->ShowWindow(style);
	w->GetDlgItem(IDC_EDIT_LEN_GLUB)->ShowWindow(style);
	w->GetDlgItem(IDC_STATIC_MONTH2)->ShowWindow(style);
}