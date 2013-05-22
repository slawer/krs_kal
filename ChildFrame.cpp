// ChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "ChildFrame.h"


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

CChildFrame::CChildFrame()
{

}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
//	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION
//		| FWS_ADDTOTITLE | WS_THICKFRAME;
//	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
//		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	if( !CMDIChildWnd::PreCreateWindow(cs) )
        return FALSE;

    return TRUE;
}

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
END_MESSAGE_MAP()


// CChildFrame message handlers

void CChildFrame::ActivateFrame(int nCmdShow)
{
	nCmdShow = SW_SHOWMAXIMIZED;
	CMDIChildWnd::ActivateFrame(nCmdShow);
}
