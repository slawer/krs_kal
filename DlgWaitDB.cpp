#include "stdafx.h"
#include "DlgWaitDB.h"
#include "src/SERV.h"
#include "src/CONV.h"
#include "src/DB.h"
#include "src/TXT.h"

#include "DlgNet.h"
#include "ParamDB.h"

DlgWaitDB DlgWaitDB_dlg;

CDlgNet g_dlg_net;
CParamDB g_dlg_db;
bool g_ending;

IMPLEMENT_DYNAMIC(DlgWaitDB, CDialog)

DlgWaitDB::DlgWaitDB(CWnd* pParent /*=NULL*/)
	: CDialog(DlgWaitDB::IDD, pParent)
{
}

DlgWaitDB::~DlgWaitDB()
{
}

void DlgWaitDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(DlgWaitDB, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &DlgWaitDB::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &DlgWaitDB::OnBnClickedButtonChange)
	ON_MESSAGE(WM_DlgWaitDB_END_DIALOG, &DlgWaitDB::OnEnforceClose)
END_MESSAGE_MAP()

void DlgWaitDB::SetupText()
{
	SetDlgItemText(IDC_STATIC_IP_CLIENT, DB_connection_info.m_server_ip + " : " + (TXT("%d") << DB_connection_info.m_server_port));
	SetDlgItemText(IDC_STATIC_IP_DB, DB_connection_info.m_db_server);
	SetDlgItemText(IDC_STATIC_DB_NAME, DB_connection_info.m_db_name);
}

BOOL DlgWaitDB::OnInitDialog()
{
	CDialog::OnInitDialog();
	g_ending = false;
	SetupText();
	SetTimer(1, 20, NULL);
	return TRUE;
}

void DlgWaitDB::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1)
	{
		SetDlgItemText(IDC_STATIC_STATUS, DB_current_status_str + "...");
		byte state = m_wndDialogBar.m_ind_db.GetState();
		if (state == STATE_ERROR || state == STATE_UNKNOWN)
		{
			GetDlgItem(IDC_BUTTON_CHANGE)->EnableWindow(TRUE);
			GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_CHANGE)->EnableWindow(FALSE);
			GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void DlgWaitDB::OnCancel()
{
}

void DlgWaitDB::OnBnClickedCancel()
{
	EndDialog(IDCANCEL);
}

void DlgWaitDB::OnBnClickedButtonChange()
{
	bool changes = false;
	
	CString client_ip = DB_connection_info.m_server_ip;
	CString conv_ip = CONV_connection_info.m_server_ip;
	CString serv_ip = SERV_connection_info.m_server_ip;
	WORD client_port = DB_connection_info.m_server_port;
	WORD conv_port = CONV_connection_info.m_server_port;
	WORD serv_port = SERV_connection_info.m_server_port;
	CString	name_db = DB_connection_info.m_db_name;
	CString	serv_db = DB_connection_info.m_db_server;
	CString	login = DB_connection_info.m_login;
	CString	password = DB_connection_info.m_password;

	if (g_dlg_net.DoModal() == IDOK)
		changes = true;
	if (!g_ending)
	{
		if (g_dlg_db.DoModal() == IDOK)
			changes = true;
	}
	if (g_ending)
	{
		DB_connection_info.m_server_ip = client_ip;
		CONV_connection_info.m_server_ip = conv_ip;
		SERV_connection_info.m_server_ip = serv_ip;
		DB_connection_info.m_server_port = client_port;
		CONV_connection_info.m_server_port = conv_port;
		SERV_connection_info.m_server_port = serv_port;
		DB_connection_info.m_db_name = name_db;
		DB_connection_info.m_db_server = serv_db;
		DB_connection_info.m_login = login;
		DB_connection_info.m_password = password;
		return;
	}
	if (changes)
	{
		SERV_ShutDownConnection(true);
		SERV_InitConnection();
		CONV_ShutDownConnection(true);
		CONV_InitConnection();
		DB_ShutDownConnection(true);
		DB_InitConnection();
		SetupText();
	}
}

LRESULT DlgWaitDB::OnEnforceClose(WPARAM id, LPARAM)
{
	g_ending = true;
	if (::IsWindow(g_dlg_net.m_hWnd))
		g_dlg_net.EndDialog(IDCANCEL);
	if (::IsWindow(g_dlg_db.m_hWnd))
		g_dlg_db.EndDialog(IDCANCEL);
	EndDialog(id);
	return +1;
}