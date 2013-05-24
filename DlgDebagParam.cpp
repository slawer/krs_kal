// DlgDebagParam.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"
#include "DlgDebagParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDebagParam dialog


//CDlgDebagParam::CDlgDebagParam(CWnd* pParent /*=NULL*/)
//	: CDialog(CDlgDebagParam::IDD, pParent)
/*
CDlgDebagParam::CDlgDebagParam(CWnd* pParent /)
	: CDialog( pParent)
{
	//{{AFX_DATA_INIT(CDlgDebagParam)
	m_MexInterval = 1.0;
	m_SPOInterval = 0.0;
	m_PorogRotorBur = 10.0;
	m_VesRotorBur = 0.0;
	//}}AFX_DATA_INIT
}
*/

void CDlgDebagParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDebagParam)
//	KRS_DDX_TextDouble(pDX, IDC_EDIT_MEX_INTERVAL, m_MexInterval);
	DDV_MinMaxDouble(pDX, m_MexInterval, 0., 1000.);
//	KRS_DDX_TextDouble(pDX, IDC_EDIT_MEX_INTERVAL2, m_SPOInterval);
	DDV_MinMaxDouble(pDX, m_SPOInterval, 0., 1000.);
//	KRS_DDX_TextDouble(pDX, IDC_EDIT_POROG_MOMENT, m_PorogRotorBur);
//	KRS_DDX_TextDouble(pDX, IDC_EDIT_VES_ROTOR_BUR, m_VesRotorBur);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDebagParam, CDialog)
	//{{AFX_MSG_MAP(CDlgDebagParam)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDebagParam message handlers

BOOL CDlgDebagParam::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_MexInterval = gMexInterval;
	m_SPOInterval = gSPOInterval;
	m_PorogRotorBur = gPorogRotorBur;
	m_VesRotorBur = gVesRotorBur;
	
	UpdateData(false);
	
	return TRUE;  
}

void CDlgDebagParam::OnOK() 
{
	UpdateData(true);

	gMexInterval = m_MexInterval;
	gSPOInterval = m_SPOInterval;
	gPorogRotorBur = m_PorogRotorBur;
	gVesRotorBur = m_VesRotorBur;

	PRJ_WriteWorkFile();

	CDialog::OnOK();
}
