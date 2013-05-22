// DlgOpisanieProekta.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "DlgOpisanieProekta.h"


// CDlgOpisanieProekta dialog

IMPLEMENT_DYNAMIC(CDlgOpisanieProekta, CDialog)

CDlgOpisanieProekta::CDlgOpisanieProekta(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOpisanieProekta::IDD, pParent)
{

}

CDlgOpisanieProekta::~CDlgOpisanieProekta()
{
}

void CDlgOpisanieProekta::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MESTOROGDENIE, m_Mestorogdenie);
	DDX_Text(pDX, IDC_EDIT_KUST, m_Kust);
	DDX_Text(pDX, IDC_EDIT_RABOTA, m_Rabota);
	DDX_Text(pDX, IDC_EDIT_SKVAGINA, m_Skvagina);
}


BEGIN_MESSAGE_MAP(CDlgOpisanieProekta, CDialog)
END_MESSAGE_MAP()


// CDlgOpisanieProekta message handlers

BOOL CDlgOpisanieProekta::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Mestorogdenie     = gMestorogdenie;
	m_Kust			    = gKust;
	m_Rabota		    = gRabota;
	m_Skvagina		    = gSkvagina;
	UpdateData(false);
	
	return TRUE; 
}
