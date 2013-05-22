// DlgKalibrovka.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "DlgKalibrovka.h"
#include <MATH.H>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgKalibrovka dialog


CDlgKalibrovka::CDlgKalibrovka(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgKalibrovka::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgKalibrovka)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgKalibrovka::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgKalibrovka)
	DDX_Control(pDX, IDC_STATIC_TEST, m_Test);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgKalibrovka, CDialog)
	//{{AFX_MSG_MAP(CDlgKalibrovka)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgKalibrovka message handlers

#define MAX_DATA 1000
BOOL CDlgKalibrovka::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(false);

	double data[MAX_DATA];
	double ddata[MAX_DATA];
	
	CRect recGraph(0,30,450,350);//(10,300,400,600);

//	m_Graph = CGraph("ñåê","Uâõ", 15, 15, 0x6000033 | GRF_ORIENTVERT/* | GRF_STYLE_TYME*/);
	m_Graph = CGraph("ñåê","Uâõ", 15, 15, 0x100F3/* | GRF_STYLE_TYME*/);
	m_Graph.Create(NULL, WS_CHILD|WS_VISIBLE, recGraph, this);

	for(int i = 0; i < MAX_DATA; i++)
	{
		data[i] = (float)sin(3.14*12*i/MAX_DATA + 0.9*(((float)rand())/RAND_MAX - 0.5f));
		ddata[i] = i;
	}

	m_Graph.CreateBufer(MAX_DATA, data, ddata);

	m_Graph.SetMinMaxXDiapazon(0, 100);
	m_Graph.SetMinMaxF(0, 20);
//	m_Graph.SetSizeFontX(11);
	m_Graph.SetOrientFontX(false);
	m_Graph.SetOrientFontY(true);

	m_Graph.SetTypeGraph(1);
	m_Graph.ShowScroll(false, false);

	m_Graph.SetColorFieldGraph(RGB(255,255,255));
	m_Graph.SetStyleLineFunc(gline);


	
	return TRUE;
}

LRESULT CDlgKalibrovka::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == 272) 
		UpdateData(false);
	
	return CDialog::WindowProc(message, wParam, lParam);
}
