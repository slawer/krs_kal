#include "stdafx.h"
#include "KRS.h"
#include "CementChildWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CCementChildWnd, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CCementChildWnd, CMDIChildWnd)
END_MESSAGE_MAP()

CCementChildWnd::CCementChildWnd()
{
}

CCementChildWnd::~CCementChildWnd()
{
}

BOOL CCementChildWnd::PreCreateWindow(CREATESTRUCT& cs)
{
//    cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION
//        | FWS_ADDTOTITLE | WS_THICKFRAME | WS_MAXIMIZE;
    if( !CMDIChildWnd::PreCreateWindow(cs) )
        return FALSE;

    return TRUE;
}

void CCementChildWnd::ActivateFrame(int nCmdShow)
{
    nCmdShow = SW_SHOWMAXIMIZED;
    CMDIChildWnd::ActivateFrame(nCmdShow);
}


// CCementChildWnd message handlers
