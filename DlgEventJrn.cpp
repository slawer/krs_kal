// DlgEventJrn.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"
#include "DlgEventJrn.h"
#include <DB.h>
#include <CHECK.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgEventJrn dialog


CDlgEventJrn::CDlgEventJrn(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEventJrn::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEventJrn)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	item.mask = LVIF_TEXT;
	item.iImage = 0;
}


void CDlgEventJrn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEventJrn)
	DDX_Control(pDX, IDC_LIST_JOURNAL, m_LstJourn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEventJrn, CDialog)
	//{{AFX_MSG_MAP(CDlgEventJrn)
	ON_BN_CLICKED(IDC_BUTTON_ALERTAVARIA, OnButtonAlertAvaria)
	ON_BN_CLICKED(IDC_BUTTON_ALERTAMAX, OnButtonAlertaMax)
	ON_BN_CLICKED(IDC_BUTTON_ALERTMIN, OnButtonAlertMin)
	ON_BN_CLICKED(IDC_BUTTON_ALERTALL, OnButtonAlertAll)
	ON_BN_CLICKED(IDC_BUTTON_JRNLHELP, OnButtonJrnlHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEventJrn message handlers

BOOL CDlgEventJrn::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(false);

	CRect rec;
	m_LstJourn.GetClientRect(&rec);

	int mm = 4;
	m_LstJourn.InsertColumn(0,"Название параметра",LVCFMT_LEFT,rec.Width() - rec.Width()*(mm - 1)/mm,-1);
	m_LstJourn.InsertColumn(1,"Значение параметра",LVCFMT_LEFT,rec.Width()/mm,-1);
	m_LstJourn.InsertColumn(2,"Причина события",LVCFMT_LEFT,rec.Width()/mm,-1);
	m_LstJourn.InsertColumn(3,"Дата и время события",LVCFMT_LEFT,rec.Width()/mm,-1);

	return true;
}

void CDlgEventJrn::AddCaseToList(CString& sName, float fVal, CString& cs, double dTime)
{
	tmEvnt.m_dt = dTime;
	tmEvnt.GetAsSystemTime(sytime);
	
	item.iItem = m_LstJourn.GetItemCount();

	item.iSubItem = 0;
	item.pszText = (LPTSTR)((LPCTSTR)(sName)); 
	m_LstJourn.InsertItem(&item);
	
	cs1.Format("%.2f", fVal);
	item.iSubItem = 1;
	item.pszText = (LPTSTR)((LPCTSTR)(cs1));
	m_LstJourn.SetItem(&item);

	item.iSubItem = 2;
	item.pszText = (LPTSTR)((LPCTSTR)(cs));
	m_LstJourn.SetItem(&item);

	cs1.Format("%02d/%02d/%02d %02d:%02d:%02d", sytime.wMonth, sytime.wDay, sytime.wYear, 
																sytime.wHour, sytime.wMinute,sytime.wSecond);
	item.iSubItem = 3;
	item.pszText = (LPTSTR)((LPCTSTR)(cs1));
	m_LstJourn.SetItem(&item);
}

void CDlgEventJrn::FillListEvent(int nCase) 
{
	if (!DB_connection_info.IsReady())
	{
		MessageBox("Невозможно отправить запрос к ДБ: нет готовности соединения", "Ошибка работы с БД");
		return;
	}
	DB_RequestParamValuesPacket packet;
	packet.m_from_time = bgTime;
	packet.m_to_time = enTime;
	packet.m_type_of_request = nCase;

	CParam* param_db = NULL;
	int key;
	POSITION pos = m_MapParamDB.GetStartPosition();
	while (pos != NULL)
	{
		m_MapParamDB.GetNextAssoc(pos, key, param_db);
		CHECK(param_db != NULL);
		packet.m_params_indexes.push_back(key);
	}
	if (packet.m_params_indexes.size() <= 0)
	{
		MessageBox("Невозможно отправить запрос к ДБ: нет ни одного параметра", "Ошибка работы с БД");
		return;
	}
	if (!DB_SendPacket(&packet))
	{
		MessageBox("Невозможно отправить запрос к ДБ: буфер переполнен", "Ошибка работы с БД");
		return;
	}
	m_wndDialogBarDB.StartRequestDB();
}


void CDlgEventJrn::OnButtonAlertAvaria() 
{
	FillListEvent(DB_ValuesRequestType_CrushEvents);
}

void CDlgEventJrn::OnButtonAlertaMax() 
{
	FillListEvent(DB_ValuesRequestType_MaxEvents);
}

void CDlgEventJrn::OnButtonAlertMin() 
{
	FillListEvent(DB_ValuesRequestType_MinEvents);
}

void CDlgEventJrn::OnButtonAlertAll() 
{
	FillListEvent(DB_ValuesRequestType_AllEvents);
}

void CDlgEventJrn::OnButtonJrnlHelp() 
{
	// TODO: Add your control notification handler code here
	
}
