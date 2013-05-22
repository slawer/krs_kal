// ArgConstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "ArgConstDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ArgConstDlg dialog


ArgConstDlg::ArgConstDlg(float init_value, CWnd* pParent /*=NULL*/)
	: CDialog(ArgConstDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ArgConstDlg)
	m_constant = init_value;
	//}}AFX_DATA_INIT
}

void ArgConstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	KRS_DDX_TextFloat(pDX, IDC_EDIT_CONST, m_constant);
}


BEGIN_MESSAGE_MAP(ArgConstDlg, CDialog)
	//{{AFX_MSG_MAP(ArgConstDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ArgConstDlg message handlers

void ArgConstDlg::OnOK() 
{
	//UpdateData();
	CDialog::OnOK();
}
