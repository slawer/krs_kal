// DialogBarMain.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"
#include "DialogBarMain.h"
#include "afxadv.h"

// CDialogBarMain

#define BORDER 5

IMPLEMENT_DYNAMIC(CDialogBarMain, CDialogBar)

CDialogBarMain::CDialogBarMain()
{
    m_bFlagDown = m_bInflateBrder = m_bSplitCurs = false;
    m_CurNorm = LoadCursor(NULL, IDC_ARROW);
    m_CurHSpl = LoadCursor(NULL, IDC_SIZEWE);
    m_CurVSpl = LoadCursor(NULL, IDC_SIZENS);
}

CDialogBarMain::~CDialogBarMain()
{
}

BEGIN_MESSAGE_MAP(CDialogBarMain, CDialogBar)
END_MESSAGE_MAP()

CSize CDialogBarMain::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	CSize sizeDefault = CControlBar::CalcDynamicLayout(nLength, dwMode);
	CSize sz = sizeDefault;
	CRect rec;

	if(dwMode & LM_HORZ)
	{
		CRect rec1;
		m_wndDialogBarCM.GetWindowRect(&rec1);

		pFrm->GetClientRect(&rec);

		int w = rec.Width() - rec1.Width();

		if (dwMode & LM_HORZDOCK)
			sz = CSize(w+4, STAGES_RecalcBarHeight(sizeDefault.cy)/*sizeDefault.cy*/);
		else if(dwMode & LM_MRUWIDTH)
			sz = CSize(w/2, rec.Height()/3);
	}
	else
	{
		CRect rec1;
		if (m_wndDialogBarH.IsVisible())
			m_wndDialogBarH.GetWindowRect(&rec1);
		else
			rec1.bottom = rec1.top = 0;
		
		pFrm->GetClientRect(&rec);
		
		int h = rec.Height() - rec1.Height();
		
		if(dwMode & LM_VERTDOCK)
			sz = CSize(sizeDefault.cx, h);
		else if(dwMode & LM_MRUWIDTH)
			sz = CSize(rec.Width()/2, rec.Height()/3);
	}
	m_sizeDefault = sz;
	return sz;
}

void CDialogBarMain::OnPaint() 
{
    CPaintDC dc(this); 
/*
    int dwStyle = GetBarStyle();
    if(dwStyle & CBRS_FLOATING) return;

    COLORREF color = RGB(190,190,190);

    CRect rec, recSplit;//, recBottom, recFrm;
    GetClientRect(&rec);	

    if(dwStyle & CBRS_ALIGN_BOTTOM)
        recSplit = CRect(rec.left, rec.top - 2, rec.right, rec.top + 7);
    else if(dwStyle & CBRS_ALIGN_TOP)
        recSplit = CRect(rec.left, rec.bottom - 7, rec.right, rec.bottom + 2);
    else if(dwStyle & CBRS_ALIGN_LEFT)
        recSplit = CRect(rec.right - 7, rec.top, rec.right + 2, rec.bottom);
    else if(dwStyle & CBRS_ALIGN_RIGHT)
        recSplit = CRect(rec.left - 2, rec.top, rec.left + 7, rec.bottom);
    else return;

    dc.DrawEdge(recSplit, EDGE_RAISED,BF_TOP | BF_BOTTOM | BF_LEFT | BF_RIGHT);	
    if(m_bSplitCurs)
    {
        OnInvertTracker(m_PointOld);
        OnInvertTracker(m_PointOld);
    }
*/
}

void CDialogBarMain::OnInvertTracker(CPoint point)
{
    DWORD style = GetBarStyle();
    if(style & CBRS_FLOATING) return;
    style &= 0xF000;

    CRect rect1;
    GetClientRect(&m_recBar);
    CRect rect = m_recBar;

    if(style == CBRS_ALIGN_BOTTOM)
    {
        rect.top = point.y - 2;
        rect.bottom = rect.top + 4;
        rect.left += 2;
    }
    else if(style == CBRS_ALIGN_LEFT)
    {
        rect.top += 2;
        rect.right = point.x + 2;
        rect.left = rect.right - 4;
    }

    ClientToScreen(&rect);
    pFrm->ScreenToClient(&rect);

    CDC* pDC = pFrm->GetDC();
    CBrush* pBrush = CDC::GetHalftoneBrush();
    HBRUSH hOldBrush = NULL;
    if (pBrush != NULL)
        hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);
    pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
    if (hOldBrush != NULL)
        SelectObject(pDC->m_hDC, hOldBrush);
    ReleaseDC(pDC);
}

void CDialogBarMain::SetCurCurssor(CPoint point) 
{
    DWORD style = GetBarStyle();
    style &= 0xF000;

    GetClientRect(&m_recBar);

    if(style == CBRS_ALIGN_BOTTOM/* || style == CBRS_ALIGN_TOP*/)
    {
		if (style == CBRS_ALIGN_BOTTOM)
        {
			m_recBar.top -= 2;
			m_recBar.bottom = m_recBar.top + 7;
		}
        if(point.y > m_recBar.top && point.y < m_recBar.bottom) m_curCursor = m_CurVSpl;
        else m_curCursor = m_CurNorm;
    }
    else if(style == CBRS_ALIGN_LEFT)
    {
        m_recBar.right += 2;
        m_recBar.left = m_recBar.right - 7;
        if(point.x > m_recBar.left && point.x < m_recBar.right) m_curCursor = m_CurHSpl;
        else m_curCursor = m_CurNorm;
    }

    if(GetCursor() != m_curCursor) SetCursor(m_curCursor);
    m_oldCursor = m_curCursor;
}

void CDialogBarMain::SetSplitCurssor() 
{
}


void CDialogBarMain::SetNormCurssor() 
{
}

void CDialogBarMain::OnLButtonDown(UINT nFlags, CPoint point) 
{
    m_bFlagDown = true;

    SetCursor(m_curCursor);
    if(m_curCursor != m_CurNorm) 
    {
        OnInvertTracker(point);
        m_bSplitCurs = true;
    }
    else
    {

        CRect rec;
        CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
        pFrm->GetClientRect(&rec);
        pFrm->ClientToScreen(&rec);

        GetWindowRect(rec);		
        ScreenToClient(&rec);
        rec.InflateRect(2,2);
        m_sizeDefault.cy -= BORDER;
        point.y -= BORDER;

        m_bInflateBrder = true;

        int style = GetBarStyle();
        style = GetBarStyle() & CBRS_ALIGN_ANY;
        CDialogBar::OnLButtonDown(nFlags, point);
    }

}

void CDialogBarMain::OnLButtonUp(UINT nFlags, CPoint point) 
{

    m_bFlagDown = false;
    if(m_bSplitCurs)
    {
        CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
        OnInvertTracker(m_PointOld);
        m_bSplitCurs = false;
        SetCurCurssor(point);

        CRect rec1;
        GetClientRect(&rec1);
        rec1.top = point.y;
        rec1.right = point.x;

        ClientToScreen(&rec1);
        pFrm->ScreenToClient(&rec1);
        m_sizeDefault.cy = rec1.Height();
        m_sizeDefault.cx = rec1.Width();
        
        ResizeOtherBars();

        pFrm->RecalcLayout(true);
        pFrm->RecalcLayout(true);
        Invalidate(TRUE);
    }
    else
    {
        CDialogBar::OnLButtonUp(nFlags, point);
    }
}


void CDialogBarMain::OnMouseMove(UINT nFlags, CPoint point) 
{
    if(m_bSplitCurs)
    {
        OnInvertTracker(m_PointOld);
        OnInvertTracker(point);
    }
    else
    {
        SetCurCurssor(point);
        if(m_curCursor != m_CurNorm)
        {
            if(GetCapture() != this) SetCapture();
        }
        else
        {
            SetCurCurssor(point);
            if(GetCapture() == this) ReleaseCapture();
        }
        CDialogBar::OnMouseMove(nFlags, point);
    }
    m_PointOld = point;
}

void CDialogBarMain::Serialize(CArchive& ar) 
{
    int cx = 50, cy = 50;
    if (ar.IsStoring())
    {	
        if(m_sizeDefault.cx < cx)
            ar << cx;
        else
            ar << m_sizeDefault.cx;

        if(m_sizeDefault.cy < cy)
            ar << cy;
        else
            ar << m_sizeDefault.cy;
    }
    else
    {	
        ar >> m_sizeDefault.cx;
        ar >> m_sizeDefault.cy;
    }
}


// CDialogBarMain message handlers


