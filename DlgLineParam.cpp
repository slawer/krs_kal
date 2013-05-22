// DlgLineParam.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "DlgLineParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLineParam dialog


CDlgLineParam::CDlgLineParam()
{
	//{{AFX_DATA_INIT(CDlgLineParam)
	//}}AFX_DATA_INIT
}

CDlgLineParam::~CDlgLineParam()
{
}

void CDlgLineParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLineParam)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLineParam, CDialogBar)
	//{{AFX_MSG_MAP(CDlgLineParam)
	ON_WM_PAINT()
	ON_MESSAGE(PM_CHANGE_POSITION, OnChangePosition)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLineParam message handlers

void CDlgLineParam::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect recHead;
	GetClientRect(&recHead);
//	dc.FillSolidRect(&recHead, GetSysColor(COLOR_MENU));
    dc.FillSolidRect(&recHead, RGB(255,255,255));

//	TRACE0("\nCDlgLineParam::OnPaint");

}


LRESULT CDlgLineParam::OnChangePosition(WPARAM wParam, LPARAM lParam)
{
	CWnd *pWnd = (CWnd*)wParam;
	SIZE sz = *((SIZE*)lParam);

	CRect rec;
	GetClientRect(&rec);
	pWnd->SetWindowPos(&wndTop, sz.cx, 0, sz.cy, rec.Height(), 
		SWP_SHOWWINDOW);

	return true;
}

int CDlgLineParam::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyle(0, SS_NOTIFY);
	
	return 0;
}
