// ParamDB.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "ParamDB.h"
#include "DB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParamDB dialog


CParamDB::CParamDB(CWnd* pParent /*=NULL*/)
	: CDialog(CParamDB::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParamDB)
	m_NameDB = _T("");
	m_NameServerDB = _T("");
	m_PasswdDB = _T("");
	m_NameUserDB = _T("");
	//}}AFX_DATA_INIT
}


void CParamDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParamDB)
	DDX_Text(pDX, IDC_EDIT_NAME_DB, m_NameDB);
	DDX_Text(pDX, IDC_EDIT_NANE_SERVER_DB, m_NameServerDB);
	DDX_Text(pDX, IDC_EDIT_PASSWD_DB, m_PasswdDB);
	DDX_Text(pDX, IDC_EDIT_NAME_USER_DB, m_NameUserDB);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParamDB, CDialog)
	//{{AFX_MSG_MAP(CParamDB)
	ON_BN_CLICKED(IDC_BUTTON_NEW_DB, OnButtonNewDb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamDB message handlers

BOOL CParamDB::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_NameDB		= DB_connection_info.m_db_name;
	m_NameServerDB	= DB_connection_info.m_db_server;
	m_PasswdDB		= DB_connection_info.m_password;
	m_NameUserDB	= DB_connection_info.m_login;

	UpdateData(false);
	
	return TRUE;  
}

void CParamDB::OnButtonNewDb() 
{
	if (DB_connection_info.IsConnected())
	{
		UpdateData(true);
		DB_SendPacketToCreateDB(m_NameServerDB, m_NameDB, m_NameUserDB, m_PasswdDB);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE); 
	}
}

void CParamDB::OnCancel()
{
	CWnd *wnd = GetDlgItem(IDCANCEL);
	if (wnd->IsWindowEnabled())
		CDialog::OnCancel();
}

void CParamDB::OnOK() 
{
	UpdateData(true);

	DB_connection_info.m_db_server = m_NameServerDB;
	DB_connection_info.m_db_name = m_NameDB;
	DB_connection_info.m_login = m_NameUserDB;
	DB_connection_info.m_password = m_PasswdDB;
	
	CDialog::OnOK();
}
