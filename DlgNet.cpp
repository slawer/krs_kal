// DlgNet.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "DlgNet.h"

#include "DB.h"
#include "CONV.h"
#include "SERV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgNet::CDlgNet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNet::IDD, pParent)
{
	m_PathDB = _T("");
	m_PathKonv = _T("");
	m_PathServer = _T("");
	m_PortKonv = 0;
	m_PortDB = 0;
	m_PortServer = 0;
}

void CDlgNet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH_DB, m_PathDB);
	DDX_Text(pDX, IDC_EDIT_PATH_KONV, m_PathKonv);
	DDX_Text(pDX, IDC_EDIT_PATH_SERVER, m_PathServer);
	DDX_Text(pDX, IDC_EDIT_PORT_KONV, m_PortKonv);
	DDX_Text(pDX, IDC_EDIT_PORT_DB, m_PortDB);
	DDX_Text(pDX, IDC_EDIT_PORT_SERVER, m_PortServer);
}

BEGIN_MESSAGE_MAP(CDlgNet, CDialog)
END_MESSAGE_MAP()

void CDlgNet::OnOK() 
{
	UpdateData(true);

	DB_connection_info.m_server_ip		= m_PathDB;
	CONV_connection_info.m_server_ip	= m_PathKonv;
	SERV_connection_info.m_server_ip	= m_PathServer;
	DB_connection_info.m_server_port	= m_PortDB;
	CONV_connection_info.m_server_port	= m_PortKonv;
	SERV_connection_info.m_server_port	= m_PortServer;
	
	CDialog::OnOK();
}

BOOL CDlgNet::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_PathDB		= DB_connection_info.m_server_ip;
	m_PathKonv		= CONV_connection_info.m_server_ip;
	m_PathServer	= SERV_connection_info.m_server_ip;
	m_PortDB		= DB_connection_info.m_server_port;
	m_PortKonv		= CONV_connection_info.m_server_port;
	m_PortServer	= SERV_connection_info.m_server_port;
	
	UpdateData(false);
	
	return TRUE;
}
