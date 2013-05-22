// DlgListOrder.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "DlgListOrder.h"
#include "MainFrm.h"


// CDlgListOrder dialog

/////////////////////////////////////////////////////////////////////////////
// CDlgListOrder dialog


CDlgListOrder::CDlgListOrder(CWnd* pParent /*=NULL*/)
: CDialog(CDlgListOrder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgListOrder)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	pPanel = NULL;
}


void CDlgListOrder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgListOrder)
	DDX_Control(pDX, IDC_LIST_ORDER, m_ListOrder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgListOrder, CDialog)
	//{{AFX_MSG_MAP(CDlgListOrder)
	ON_LBN_SELCHANGE(IDC_LIST_ORDER, OnSelchangeListOrder)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_TOP, OnButtonTop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgListOrder message handlers

void CDlgListOrder::OnSelchangeListOrder() 
{
	UpdateData(true);
	m_nIndex = m_ListOrder.GetCurSel();
}



void CDlgListOrder::OnButtonDown() 
{
	m_nIndex = m_ListOrder.GetCurSel();
	if(m_nIndex >= (pPanel->m_ParamArray.GetCount() - 1)) return;

	CString cs1, cs2;
	ParView *pParV1, *pParV2;

	pParV1 = pPanel->m_ParamArray[m_nIndex];
	if(pParV1 == NULL) return;

	pParV2 = pPanel->m_ParamArray[m_nIndex+1];
	if(pParV2 == NULL) return;

	pPanel->m_ParamArray[m_nIndex] = pParV2;
	pPanel->m_ParamArray[m_nIndex+1] = pParV1;

	m_ListOrder.GetText(m_nIndex, cs1);
	m_ListOrder.GetText(m_nIndex+1, cs2);

	m_ListOrder.DeleteString(m_nIndex);
	m_ListOrder.DeleteString(m_nIndex);
	m_nIndex = m_ListOrder.InsertString(m_nIndex, cs1);
	m_nIndex = m_ListOrder.InsertString(m_nIndex, cs2);
	m_nIndex++;	

	m_ListOrder.SetCurSel(m_nIndex);
	pPanel->RecalcFieldSize();
}

void CDlgListOrder::OnButtonTop() 
{
	m_nIndex = m_ListOrder.GetCurSel();
	if(m_nIndex <= 0) return;

	CString cs1, cs2;
	ParView *pParV1, *pParV2;

	pParV1 = pPanel->m_ParamArray[m_nIndex];
	if(pParV1 == NULL) return;

	pParV2 = pPanel->m_ParamArray[m_nIndex-1];
	if(pParV2 == NULL) return;

	pPanel->m_ParamArray[m_nIndex] = pParV2;
	pPanel->m_ParamArray[m_nIndex-1] = pParV1;

	m_ListOrder.GetText(m_nIndex, cs1);
	m_ListOrder.GetText(m_nIndex-1, cs2);

	m_ListOrder.DeleteString(m_nIndex);
	m_ListOrder.DeleteString(m_nIndex-1);
	m_nIndex--;	
	m_nIndex = m_ListOrder.InsertString(m_nIndex, cs2);
	m_nIndex = m_ListOrder.InsertString(m_nIndex, cs1);

	m_ListOrder.SetCurSel(m_nIndex);
	pPanel->RecalcFieldSize();
}

BOOL CDlgListOrder::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CParam* pPrm;
	int index;
	CString cs;

	int nn = pPanel->m_ParamArray.GetSize();
//	m_ParamArray[i]
	for(int i = 0; i < nn; i++)
	{
		pPrm = pPanel->m_ParamArray[i]->pPrm;
		if(pPrm == NULL) continue;

		cs = " " + pPrm->sName;
		index = m_ListOrder.AddString(cs);
//		m_ListOrder.SetItemDataPtr(index, (LPVOID) pPanel->m_ParamArray[i]);
	}

	m_nIndex = 0;
	m_ListOrder.SetCurSel(m_nIndex);
	UpdateData(false);

	return TRUE; 
}

void CDlgListOrder::OnOK() 
{
	CDialog::OnOK();
}
