// DBChildFrm.cpp : implementation file
//
#include "stdafx.h"
#include "KRS.h"
#include "DBChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDBChildFrm

IMPLEMENT_DYNCREATE(CDBChildFrm, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CDBChildFrm, CMDIChildWnd)
END_MESSAGE_MAP()

CDBChildFrm::CDBChildFrm()
{
}

CDBChildFrm::~CDBChildFrm()
{
}

BOOL CDBChildFrm::PreCreateWindow(CREATESTRUCT& cs)
{
//	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION
//		| FWS_ADDTOTITLE | WS_THICKFRAME;
//	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
//		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	if( !CMDIChildWnd::PreCreateWindow(cs) )
        return FALSE;

    return TRUE;
}

// CDBChildFrm message handlers

void CDBChildFrm::ActivateFrame(int nCmdShow)
{
	nCmdShow = SW_SHOWMAXIMIZED;
	CMDIChildWnd::ActivateFrame(nCmdShow);
}
