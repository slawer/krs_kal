// DlgMestorogdenie.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "DlgMestorogdenie.h"


// CDlgMestorogdenie dialog

IMPLEMENT_DYNAMIC(CDlgMestorogdenie, CDialog)

CDlgMestorogdenie::CDlgMestorogdenie(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMestorogdenie::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMestorogdenie)
	m_Mestorogdenie = _T("");
	m_Kust = _T("");
	m_Rabota = _T("");
	m_Skvagina = _T("");
	//}}AFX_DATA_INIT
}

CDlgMestorogdenie::~CDlgMestorogdenie()
{
}

void CDlgMestorogdenie::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMestorogdenie)
	DDX_Text(pDX, IDC_EDIT_MESTOROGDENIE, m_Mestorogdenie);
	DDX_Text(pDX, IDC_EDIT_KUST, m_Kust);
	DDX_Text(pDX, IDC_EDIT_RABOTA, m_Rabota);
	DDX_Text(pDX, IDC_EDIT_SKVAGINA, m_Skvagina);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMestorogdenie, CDialog)
	//{{AFX_MSG_MAP(CDlgMestorogdenie)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CDlgMestorogdenie message handlers

BOOL CDlgMestorogdenie::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Mestorogdenie     = gMestorogdenie;
	m_Kust			    = gKust;
	m_Rabota		    = gRabota;
	m_Skvagina		    = gSkvagina;
	UpdateData(false);
	return TRUE; 
}

void CDlgMestorogdenie::OnOK() 
{
	UpdateData(true);

	gMestorogdenie	= m_Mestorogdenie;
	gKust			= m_Kust;
	gRabota			= m_Rabota;
	gSkvagina		= m_Skvagina;

	pApp->WriteProfileString("InitData", "Mestorogdenie", gMestorogdenie);
	pApp->WriteProfileString("InitData", "Kust", gKust);
	pApp->WriteProfileString("InitData", "Rabota", gRabota);
	pApp->WriteProfileString("InitData", "Skvagina", gSkvagina);

	CDialog::OnOK();
}
