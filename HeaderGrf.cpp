// HeaderGrf1.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "HeaderGrf.h"
#include "Param.h"
#include <BS.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHeaderGrf::CHeaderGrf()
{
	colorLineHead = RGB(0, 200, 0);
	m_num_grf = 0;
}

CHeaderGrf::~CHeaderGrf()
{
}


BEGIN_MESSAGE_MAP(CHeaderGrf, CStatic)
	//{{AFX_MSG_MAP(CHeaderGrf)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CHeaderGrf::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GetParent()->PostMessage(PM_CHANGE_PARAM_PROPERT, ((ParView*)pParamV)->m_num_par_local, NULL);
	CStatic::OnRButtonDown(nFlags, point);
}

void CHeaderGrf::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ClientToScreen(&point);
	ParView *pParView = (ParView*)pParamV; 
	GetParent()->PostMessage(PM_ADD_PARAM, pParView->m_num_par_local, (point.x<<16) | (point.y));
}

void CHeaderGrf::OnPaint() 
{
	CPaintDC dc(this);
	DrawHeader(&dc);
}

COLORREF CHeaderGrf::GetColorAlert(void* pPrm)
{
	CParam* pPar = (CParam*)pPrm;
	switch(pPar->numAlert) 
	{
		case 1:
			return ((CGraph*)pGraph)->GetColorHeadAvaria();
		break;
		
		case 2:
			return ((CGraph*)pGraph)->GetColorHeadMin();
		break;

		case 3:
			return ((CGraph*)pGraph)->GetColorHeadMax();
		break;
			
		default:
			return GetSysColor(COLOR_MENU);
	}
}

void CHeaderGrf::DrawHeaderVertGrf(CDC* pDC, CWnd *pWndP, float sclX, float sclY, CSize *prnShift)
{
	ParView *pParView = (ParView*)pParamV; 
	CParam *pPar = pParView->pPrm;

	CWnd *pWnd = GetParent();
	if (pWnd == NULL) return;

//--------------------------------------------
	CRect recHead;
	GetClientRect(&recHead);

	CRect reFldGrf = ((CGraph*)pGraph)->GetRectFldGrf();
	ScreenToClient(&reFldGrf);
	reFldGrf.NormalizeRect();
	F Fmin, Fmax;
	((CGraph*)pGraph)->GetMinMaxF(Fmin, Fmax, pParView->num_grf);

//---------------------------------------------------------------------
	COLORREF clr_fon = GetColorAlert(pPar);

	float sX = 1.0, sY = 1.0;
	DWORD bk_color = RGB(255,255,255);

	if(pDC->IsPrinting()) 
	{
		if(!pWndP) return;

		sX = sclX;
		sY = sclY;

		ClientToScreen(&recHead);
		ClientToScreen(&reFldGrf);
        fFontName.GetLogFont(&m_logfontN);
		SetPrintFont(sclY, fFontName, false);
	}
	else
    {
		if (pPar->nKodControl > 0 && Param_blink_counter > 5 && !pPar->IsTimedOut())
		{
			if ((pPar->nKodControl == 3 || pPar->nKodControl == 4 || pPar->nKodControl == 6 || pPar->nKodControl == 7) && pPar->fCurAvg < pPar->fMin) 
				bk_color = Param_clr_min;
			if ((pPar->nKodControl == 2 || pPar->nKodControl == 3 || pPar->nKodControl == 5 || pPar->nKodControl == 6) && pPar->fCurAvg > pPar->fMax) 
				bk_color = Param_clr_max;
			if ((pPar->nKodControl >= 1 && pPar->nKodControl <= 4) && pPar->fCurAvg > pPar->fAvaria) 
				bk_color = Param_clr_avaria;
		}
        pDC->FillSolidRect(reFldGrf.left - 1, recHead.top, recHead.Width(), recHead.Height() - 1, bk_color);
        int nKodControl = pPar->nKodControl;
        if (nKodControl > 0)
        {
            float dd = Fmax - Fmin;
            int w = reFldGrf.Width()+1;
            int h = recHead.Height()-6;
            float d_min = 0, d_norm = 0, d_max = 0, d_avaria = 0;
            if(dd > 0 && w != 0)
            {
                int left = reFldGrf.left;
                float kof = float(w)/dd;
                if(nKodControl == 3 || nKodControl == 4 || nKodControl == 6 || nKodControl == 7)
                {
                    d_min = (pPar->fMin - Fmin)*kof;
                    if(d_min > 0)
                    {
                        pDC->FillSolidRect(left, recHead.top + 1, d_min, h, Param_clr_min);
                        left += d_min;
                    }
                }
                if(nKodControl == 7) d_norm = (Fmax - pPar->fMin)*kof;
                else if(nKodControl == 6) d_norm = (pPar->fMax - pPar->fMin)*kof;
                else if(nKodControl == 5) d_norm = (pPar->fMax - Fmin)*kof;
                else if(nKodControl == 4) d_norm = (pPar->fAvaria - pPar->fMin)*kof;
                else if(nKodControl == 3) d_norm = (pPar->fMax - pPar->fMin)*kof;
                else if(nKodControl == 2) d_norm = (pPar->fMax - Fmin)*kof;
                else if(nKodControl == 1) d_norm = (pPar->fAvaria - Fmin)*kof;

                if(d_norm > 0)
                {
                    pDC->FillSolidRect(left, recHead.top + 1, d_norm, h, Param_clr_norm);
                    left += d_norm;
                }
                if(nKodControl == 6 || nKodControl == 5) d_max = (Fmax - pPar->fMax)*kof;
                else if(nKodControl == 3 || nKodControl == 2) d_max = (pPar->fAvaria - pPar->fMax)*kof;

                if(d_max > 0)
                {
                    pDC->FillSolidRect(left, recHead.top + 1, d_max, h, Param_clr_max);
                    left += d_max;
                }
                if(nKodControl == 4 || nKodControl == 3 || nKodControl == 2 || nKodControl == 1)
                {
                    d_avaria = (Fmax - pPar->fAvaria)*w/dd;
//                    d_avaria = w - left;    
                    if(d_avaria > 0)
                        pDC->FillSolidRect(left, recHead.top + 1, d_avaria, h, Param_clr_avaria);
                }
            }
        }
    }
	int xbeg = reFldGrf.left;
	int ybeg = recHead.bottom - int(2*sY);
	int xend = reFldGrf.right;
	int yend = ybeg;
	int y = recHead.bottom - 5;
	if(pDC->IsPrinting()) y -= (int)(3*sY);

	colorLineHead = pParView->colorGraph;
	int field_w = reFldGrf.right - reFldGrf.left;
	if(!pDC->IsPrinting()) pDC->FillSolidRect(reFldGrf.left, y+1, field_w, 2, colorLineHead);
	else
    {
        pDC->FillSolidRect(reFldGrf.left, y, field_w, (int)(2*sY), colorLineHead);
    }

	if(Fmin < 0 && Fmax > 0)
	{
		float k = float((0 - Fmin)/(Fmax - Fmin));
		int x = int(reFldGrf.left + field_w * k);
		pDC->FillSolidRect(x, y - 1, 1, 6, colorLineHead);
	}

	CString str;
	CFont* pFont = pDC->SelectObject(&fFontName);
	ASSERT(pFont);
	pDC->SetBkMode(TRANSPARENT);

    CSize sz;
	CString str4 = "???";

    str = (pPar == NULL)? str4 : pPar->sGraphName;
	if (str.GetLength() == 0)
		str = str4;

    sz = pDC->GetTextExtent(str);
    y -= sz.cy + 1;

	CRect recName(CPoint(recHead.left + 1, recHead.top), CPoint(reFldGrf.left, recHead.bottom - 1));
	pDC->FillSolidRect(recName, bk_color);
	pDC->ExtTextOut(recHead.left + int(10*sX), y + 2, ETO_CLIPPED, recName, str, str.GetLength(), NULL);

	if (pPar != NULL && pPar->m_shown_digits == 0xFF)
	{
		COleDateTimeSpan odts((double)Fmin);
		str = BS_FloatWOZeros(odts.GetTotalMinutes(), 0);
	}
	else
		str = BS_FloatWOZeros(Fmin, 3);
	pDC->TextOut(xbeg + int(1*sX), y, str);

	if (pPar != NULL && pPar->m_shown_digits == 0xFF)
	{
		COleDateTimeSpan odts((double)Fmax);
		str = BS_FloatWOZeros(odts.GetTotalMinutes(), 0);
	}
	else
		str = BS_FloatWOZeros(Fmax, 3);
	CSize sz1 = pDC->GetTextExtent(str);
	pDC->TextOut(xend - sz1.cx - int(2*sX), y, str);

	if (pPar != NULL && pPar->sRazmernPar.GetLength() > 0)
	{
		str = "[" + pPar->sRazmernPar + "]";
		sz1 = pDC->GetTextExtent(str);
		pDC->TextOut(xbeg + (xend - xbeg)/2 - sz1.cx/2, y, str);
	}

	if (pPar && pParView && !pPar->IsTimedOut())
	{
		y += sz.cy + 1;
		DWORD arrow_col;
		int x;
		pDC->BeginPath();
		if (pPar->fCurAvg < Fmin)
		{
			arrow_col = 0x0000FF;
			x = reFldGrf.left;
			pDC->MoveTo(x, y-2);
			pDC->LineTo(x+4, y-6);
			pDC->LineTo(x+4, y+2);
			pDC->LineTo(x, y-2);
		}
		else
		if (pPar->fCurAvg > Fmax)
		{
			arrow_col = 0x0000FF;
			x = reFldGrf.right;
			pDC->MoveTo(x, y-2);
			pDC->LineTo(x-4, y-6);
			pDC->LineTo(x-4, y+2);
			pDC->LineTo(x, y-2);
		}
		else
		{
			arrow_col = 0x000000;
			float k = float((pPar->fCurAvg - Fmin)/(Fmax - Fmin));
			int x = int(reFldGrf.left + field_w * k);
			pDC->MoveTo(x, y+1);
			pDC->LineTo(x-4, y-3);
			pDC->LineTo(x+4, y-3);
			pDC->LineTo(x, y+1);
		}
		pDC->EndPath();
		CPen pen(PS_SOLID, 1, 0xFFFFFF), *prev_pen;
		CBrush brush, *prev_brush;
		brush.CreateSolidBrush(arrow_col);
		prev_pen = pDC->SelectObject(&pen);
		prev_brush = pDC->SelectObject(&brush);
		pDC->StrokeAndFillPath();
		pDC->SelectObject(prev_pen);
		pDC->SelectObject(prev_brush);
	}

	pDC->SelectObject(pFont);

	if(pDC->IsPrinting()) 
	{
		if(!pWnd) return;
		SetNormalFont();
	}
}

void CHeaderGrf::DrawHeaderHorGrf(CDC* pDC, CWnd *pWndP, float sclX, float sclY, CSize *prnShift)
{
    ParView *pParView = (ParView*)pParamV; 
    CParam *pPar = pParView->pPrm;

    CWnd *pWnd = GetParent();
    if (pWnd == NULL) return;

    //--------------------------------------------
    CRect recHead;
    GetClientRect(&recHead);

    CRect reFldGrf = ((CGraph*)pGraph)->GetRectFldGrf();
    ScreenToClient(&reFldGrf);
    reFldGrf.NormalizeRect();
    F Fmin, Fmax;
    ((CGraph*)pGraph)->GetMinMaxF(Fmin, Fmax, pParView->num_grf);

//---------------------------------------------------------------------
    COLORREF clr_fon = GetColorAlert(pPar);

    float sX = 1.0, sY = 1.0;

    if(pDC->IsPrinting()) 
    {
        if(!pWndP) return;
        sX = sclX;
        sY = sclY;

        ClientToScreen(&recHead);
        ClientToScreen(&reFldGrf);
        fFontName.GetLogFont(&m_logfontN);
        SetPrintFont(sclX, fFontName, true);
    }
    else
    {
        pDC->FillSolidRect(recHead.left, recHead.top, recHead.Width(), reFldGrf.Height(), RGB(255,255,255));
        int nKodControl = pPar->nKodControl;

        if(pPar->nKodControl != 0)
        {
            COLORREF clr_min = RGB(216,247,246), clr_norm = RGB(0,255,0), 
                clr_max = RGB(255,255,0), clr_avaria = RGB(254,132,28);
            float dd = Fmax - Fmin;
            int w = recHead.Width()-6;
            int h = reFldGrf.Height()+1;
            float d_min = 0, d_norm = 0, d_max = 0, d_avaria = 0;
            if(dd > 0 && w != 0)
            {
                int top = reFldGrf.bottom;
                float kof = float(h)/dd;
                if(nKodControl == 3 || nKodControl == 4 || nKodControl == 6 || nKodControl == 7)
                {
                    d_min = (pPar->fMin - Fmin)*kof;
                    if(d_min > 0)
                    {
						top -= d_min;
                        pDC->FillSolidRect(recHead.left, top, w, d_min, clr_min);
                    }
                }
                if(nKodControl == 7) d_norm = (Fmax - pPar->fMin)*kof;
                else if(nKodControl == 6) d_norm = (pPar->fMax - pPar->fMin)*kof;
                else if(nKodControl == 5) d_norm = (pPar->fMax - Fmin)*kof;
                else if(nKodControl == 4) d_norm = (pPar->fAvaria - pPar->fMin)*kof;
                else if(nKodControl == 3) d_norm = (pPar->fMax - pPar->fMin)*kof;
                else if(nKodControl == 2) d_norm = (pPar->fMax - Fmin)*kof;
                else if(nKodControl == 1) d_norm = (pPar->fAvaria - Fmin)*kof;

                if(d_norm > 0)
                {
					top -= d_norm;
                    pDC->FillSolidRect(recHead.left, top, w, d_norm, clr_norm);
                }
                if(nKodControl == 6 || nKodControl == 5) d_max = (Fmax - pPar->fMax)*kof;
                else if(nKodControl == 3 || nKodControl == 2) d_max = (pPar->fAvaria - pPar->fMax)*kof;

                if(d_max > 0)
                {
					top -= d_max;
                    pDC->FillSolidRect(recHead.left, top, w, d_max, clr_max);
                }
                if(nKodControl == 4 || nKodControl == 3 || nKodControl == 2 || nKodControl == 1)
                {
                    d_avaria = (Fmax - pPar->fAvaria)*h/dd;
                    //                    d_avaria = w - left;    
                    if(d_avaria > 0)
                        pDC->FillSolidRect(recHead.left, recHead.top, w, d_avaria, clr_avaria);
                }
            }
        }
    }
//---------------------------------------------------------------------------------------------------
    int xbeg = recHead.left;
    int ybeg = reFldGrf.bottom - int(2*sY);
    int xend = recHead.right;
    int yend = recHead.top;
    int x = recHead.right - 5;
	if(pDC->IsPrinting()) x -= (int)(3*sX);
    colorLineHead = pParView->colorGraph;
    int field_h = reFldGrf.bottom - reFldGrf.top;
    if(!pDC->IsPrinting()) pDC->FillSolidRect(x+1, reFldGrf.top, 2, field_h, colorLineHead);
    else
    {
		pDC->FillSolidRect(x+1, reFldGrf.top, (int)(2*sY), field_h, colorLineHead);
    }
    if(Fmin < 0 && Fmax > 0)
    {
        float k = float((0 - Fmin)/(Fmax - Fmin));
        int y = int(reFldGrf.bottom - field_h * k);
        pDC->FillSolidRect(x, y - 1, 6, 1, colorLineHead);
    }

    CString str;
    CFont* pFont = pDC->SelectObject(&fFontName);
    ASSERT(pFont);
    pDC->SetBkMode(TRANSPARENT);

    CSize sz;

    str = (pPar == NULL)?"???":pPar->sGraphName;
    if (str.GetLength() == 0)
        str = "???";

    sz = pDC->GetTextExtent(str);
    x -= sz.cy + 1;

    CRect recName(CPoint(recHead.left, recHead.bottom), CPoint(reFldGrf.left + sz.cx, reFldGrf.bottom));
    pDC->ExtTextOut(x + 2, recHead.bottom - int(10*sX), ETO_CLIPPED, recName, str, str.GetLength(), NULL);

    str = BS_FloatWOZeros((float)Fmin, 1);
    pDC->TextOut(x, ybeg - int(1*sX), str);

    str = BS_FloatWOZeros((float)Fmax, 1);
    CSize sz1 = pDC->GetTextExtent(str);
    pDC->TextOut(x, yend + sz1.cx + int(4*sX), str);

    if (pPar != NULL && pPar->sRazmernPar.GetLength() > 0)
    {
        str = "[" + pPar->sRazmernPar + "]";
        sz1 = pDC->GetTextExtent(str);
        pDC->TextOut(x, reFldGrf.bottom - (reFldGrf.bottom - reFldGrf.top)/2 + sz1.cy/2, str);
    }
    if (pPar && pParView && !pPar->IsTimedOut())
    {
        x += sz.cy + 1;
        DWORD arrow_col;
        int y;
        pDC->BeginPath();
        if (pPar->fCurAvg < Fmin)
        {
            arrow_col = 0x0000FF;
            y = reFldGrf.bottom;
            pDC->MoveTo(x-2, y);
            pDC->LineTo(x-6, y-4);
            pDC->LineTo(x+2, y-4);
            pDC->LineTo(x-2, y);
        }
        else
        if (pPar->fCurAvg > Fmax)
        {
            arrow_col = 0x0000FF;
            y = reFldGrf.top;
            pDC->MoveTo(x-2, y);
            pDC->LineTo(x-6, y+4);
            pDC->LineTo(x+2, y+4);
            pDC->LineTo(x-1, y);
        }
        else
        {
            arrow_col = 0x000000;
            float k = float((pPar->fCurAvg - Fmin)/(Fmax - Fmin));
            y = int(reFldGrf.bottom - field_h * k);
            pDC->MoveTo(x+1, y);
            pDC->LineTo(x-3, y-4);
            pDC->LineTo(x-3, y+4);
            pDC->LineTo(x+1, y);
        }
        pDC->EndPath();
        CPen pen(PS_SOLID, 1, 0xFFFFFF), *prev_pen;
        CBrush brush, *prev_brush;
        brush.CreateSolidBrush(arrow_col);
        prev_pen = pDC->SelectObject(&pen);
        prev_brush = pDC->SelectObject(&brush);
        pDC->StrokeAndFillPath();
        pDC->SelectObject(prev_pen);
        pDC->SelectObject(prev_brush);
    }
    pDC->SelectObject(pFont);

    if(pDC->IsPrinting()) 
    {
        if(!pWnd) return;
        SetNormalFont();
    }
}

void CHeaderGrf::DrawHeader(CDC* pDC, CWnd *pWndP, float sclX, float sclY, CSize *prnShift)
{
    ParView *pParView = (ParView*)pParamV; 
    if (pParView == NULL) return;
    if(pParView->m_flag_orient) 
        DrawHeaderVertGrf(pDC, pWndP, sclX, sclY, prnShift);
    else 
        DrawHeaderHorGrf(pDC, pWndP, sclX, sclY, prnShift);
}

CSize CHeaderGrf::GetSizeText(CString cs)
{
	CClientDC dc(this);
	CFont *pFont = dc.SelectObject(&fFontName);
	ASSERT(pFont);
	CSize sz = dc.GetTextExtent(cs);
	dc.SelectObject(pFont);
	return sz;
}
void CHeaderGrf::SetFontDgt()
{
	if(pParamV == NULL) return;
	ParView *pParView = (ParView*)pParamV; 
	if(fFontName.m_hObject) fFontName.DeleteObject();

	LOGFONT lf = pParView->font_dgt;
	lf.lfEscapement = lf.lfOrientation = 900*int(!pParView->m_flag_orient);
	fFontName.CreateFontIndirect(&lf);
	Invalidate(false);
}

void CHeaderGrf::SetParamHeader(void* pPrmV, void* pGrf)
{
	pParamV = pPrmV;
	pGraph = pGrf;
	m_num_grf = ((ParView*)pParamV)->m_num_par_local;
	CParam* prm = ((ParView*)pParamV)->pPrm;
	m_MinHead = prm->fGraphMIN;
	m_MaxHead = prm->fGraphMAX;
	colorLineHead = ((ParView*)pParamV)->colorGraph;
}

int CHeaderGrf::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyle(0, SS_NOTIFY);

	LOGFONT  logfontN = {
		-8, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	m_logfontN = logfontN;

	if(fFontName.m_hObject) fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&m_logfontN);
	
	return 0;
}

void CHeaderGrf::OnDestroy() 
{
	CStatic::OnDestroy();
	
	fFontName.DeleteObject();
}
/////////////////////////////////////////////////////////////////////////////
// COutParamPanel

COutParamPanel::COutParamPanel()
{
	LOGFONT  logfontN = {
		-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	m_logfontN = logfontN;
}

COutParamPanel::~COutParamPanel()
{
}


BEGIN_MESSAGE_MAP(COutParamPanel, CStatic)
	//{{AFX_MSG_MAP(COutParamPanel)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutParamPanel message handlers
void COutParamPanel::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GetParent()->PostMessage(PM_CHANGE_PARAM_PROPERT, 
		((ParView*)pParamV)->m_num_par_local, NULL);
	CStatic::OnRButtonDown(nFlags, point);
}

void COutParamPanel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ParView *pPrmV = (ParView*)pParamV;
	CPoint pnt = point;
	ClientToScreen(&pnt);
	GetParent()->PostMessage(PM_ADD_PARAM, ((ParView*)pParamV)->m_num_par_local, (pnt.x<<16) | (pnt.y));
	CStatic::OnLButtonDown(nFlags, point);
}

int COutParamPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyle(0, SS_NOTIFY);
	if(fFontName.m_hObject) fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&m_logfontN);
	
	return 0;
}

void COutParamPanel::OnDestroy() 
{
	CStatic::OnDestroy();
	
	fFontName.DeleteObject();
}

void COutParamPanel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawHeader(&dc);
}
void COutParamPanel::SetFontDgt()
{
	if(pParamV == NULL) return;
	if(fFontName.m_hObject) fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&((ParView*)pParamV)->font_dgt);
	Invalidate(false);
}
void COutParamPanel::SetParamHeader(void* pPrmV, void* pGrf)
{
	pParamV = pPrmV;
	pGraph = pGrf;
	CParam* prm = ((ParView*)pParamV)->pPrm;
	if(prm == NULL) return;
	m_MinHead = prm->fGraphMIN;
	m_MaxHead = prm->fGraphMAX;
}

void COutParamPanel::DrawHeader(CDC* pDC, CWnd *pWndP, float sclX, float sclY, CSize *prnShift)
{
	CParam *pPrm = ((ParView*)pParamV)->pPrm; 
	if(pPrm == NULL) return;

	CWnd *pWnd = GetParent();
	if(pWnd == NULL) return;

	CRect recHead;
	GetClientRect(&recHead);
	DWORD bk_color = g_color_interface.color_panel_dgt_bk;
	if (pPrm->nKodControl > 0 && Param_blink_counter > 5 && !pPrm->IsTimedOut())
	{
		if ((pPrm->nKodControl == 3 || pPrm->nKodControl == 4 || pPrm->nKodControl == 6 || pPrm->nKodControl == 7) && pPrm->fCurAvg < pPrm->fMin) // контроль минимального
			bk_color = Param_clr_min;
		if ((pPrm->nKodControl == 2 || pPrm->nKodControl == 3 || pPrm->nKodControl == 5 || pPrm->nKodControl == 6) && pPrm->fCurAvg > pPrm->fMax) // контроль максимального
			bk_color = Param_clr_max;
		if ((pPrm->nKodControl >= 1 && pPrm->nKodControl <= 4) && pPrm->fCurAvg > pPrm->fAvaria) // контроль аварийного
			bk_color = Param_clr_avaria;
	}
	pDC->FillSolidRect(recHead, bk_color);
	pDC->SetBkColor(bk_color);
	pDC->SetBkMode(OPAQUE);

	int y = recHead.top + 0;
	int dx = 3;

	pDC->SetTextColor(((ParView*)pParamV)->color_font_dgt);
	CString text = (pPrm == NULL)?"???":(pPrm->sGraphName + " [" + pPrm->sRazmernPar + "]");
	CFont* pFont = pDC->SelectObject(&fFontName);
	ASSERT(pFont);
	pDC->TextOut(recHead.left + dx, y, text);
	if (pPrm->IsTimedOut())
	{
		text = "ОТКЛ";
		pDC->SetTextColor(RGB(255,0,0));
	}
	else
	{
		CString fmt;
		if (pPrm->m_shown_digits == 0xFF)
		{
			COleDateTime otd;
			otd.m_dt = pPrm->fCurAvg;
			text = otd.Format("%H:%M:%S");
		}
		else
		{
			if (pPrm->m_shown_digits >=0 && pPrm->m_shown_digits <= 6)
				fmt.Format("%%.%df", pPrm->m_shown_digits);
			else
				fmt = "%.3f";
			text.Format(fmt, pPrm->fCurAvg);
		}
		pDC->SetTextColor(RGB(0,0,255));
	}
	CSize sz2 = pDC->GetTextExtent(text);
	pDC->TextOut(recHead.right - dx - sz2.cx, y, text);

	CPen pn(PS_DOT, 1, RGB(170,170,170));
	CPen* p_pn = pDC->SelectObject(&pn);
	pDC->MoveTo(recHead.left, recHead.bottom);
	pDC->LineTo(recHead.right, recHead.bottom);
	pDC->SelectObject(p_pn);

	pDC->SelectObject(pFont);
}

CSize COutParamPanel::GetSizeText(CString cs)
{
	CClientDC dc(this);
	CFont *pFont = dc.SelectObject(&fFontName);
	ASSERT(pFont);
	CSize sz = dc.GetTextExtent(cs);
	dc.SelectObject(pFont);
	return sz;
}

CDgtHeadParam::CDgtHeadParam()
{
}

CDgtHeadParam::~CDgtHeadParam()
{
}

BEGIN_MESSAGE_MAP(CDgtHeadParam, CStatic)
	//{{AFX_MSG_MAP(CDgtHeadParam)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDgtHeadParam::SetShowDgtLabel(BOOL bfShow)
{
	if(m_hWnd != NULL) 
	{
		if(bfShow) ShowWindow(SW_SHOW);
		else ShowWindow(SW_HIDE);
	}
}

void CDgtHeadParam::SetPrintFont(float scl, CFont& fFontN, BOOL flOrient) 
{
    LOGFONT logfontN;
    fFontName.GetLogFont(&logfontN);

    logfontN.lfEscapement = logfontN.lfOrientation = 900*int(flOrient);
    logfontN.lfHeight *= scl/1.3f;

    if(fFontN.m_hObject) fFontN.DeleteObject();
    fFontN.CreateFontIndirect(&logfontN);
}

void CDgtHeadParam::SetNormalFont() 
{
	fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&m_logfontN);
}



