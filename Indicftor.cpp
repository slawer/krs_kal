// Indicftor.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "Indicftor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIndicftor
CString GetStrData(COleDateTime tm)
{
	CString day, month; 
	int n_wmonth = tm.GetMonth();
	int n_day = tm.GetDay();

	switch(n_wmonth)
	{
	case 1: month  = " Январь "; break;
	case 2: month  = "Февраль "; break;
	case 3: month  = "  Март  "; break;
	case 4: month  = " Апрель "; break;
	case 5: month  = "  Май   "; break;
	case 6: month  = "  Июнь  "; break;
	case 7: month  = "  Июль  "; break;
	case 8: month  = " Август "; break;
	case 9: month  = "Сентябрь"; break;
	case 10: month = "Октябрь "; break;
	case 11: month = " Ноябрь "; break;
	case 12: month = _T("Декабрь "); break;
	}
	day.Format("%02d  ", n_day);

	return day + month;
}
CString GetStrTime(COleDateTime tm)
{
	CString str_time;
	int hour = tm.GetHour();
	int minute = tm.GetMinute();
	int second = tm.GetSecond();
	str_time.Format("%02d:%02d:%02d", hour, minute, second);
	return str_time;
}
CIndicftor::CIndicftor()
{
    m_pDCMem = NULL;
    m_pBitmap = NULL;
    m_pbitmapOld = NULL;

    LOGFONT lf = {
		-15, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
		OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	lfW = lf;
	numStateW = 0;

	colorText	= RGB(255,255,255);
	colorFon	= RGB(255,100,100);
	colorTextDop= RGB(0,0,0);
	colorFonDop = RGB(200,255,200);
	colorTextCur = colorText;
	colorFonCur = colorFon;
	color_fon_blink1 = RGB(0,0,255);
	color_fon_blink2 = RGB(255,0,0);

	numStateW = 0;
	if(fFont.m_hObject) fFont.DeleteObject();
	fFont.CreateFontIndirect(&lfW);
	text = "Text";
	fl_blink = 0;
	m_style = 0;
}

CIndicftor::~CIndicftor()
{
    if (m_pDCMem != NULL)
    {
        ASSERT(m_pbitmapOld != NULL);
        delete m_pDCMem;
        if(m_pBitmap) delete m_pBitmap;
    }
    else
    {
        ASSERT(m_pbitmapOld == NULL);
        ASSERT(m_pBitmap == NULL);
    }
}


BEGIN_MESSAGE_MAP(CIndicftor, CStatic)
	//{{AFX_MSG_MAP(CIndicftor)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIndicftor message handlers
void CIndicftor::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawInd(&dc);
}

void CIndicftor::InitInd(CString str, int sizeFont, int state)
{
	numStateW = state;

	if(fFont.m_hObject) fFont.DeleteObject();
	fFont.CreateFontIndirect(&lfW);

	colorTextCur = RGB(0,0,0);
	colorFonCur = GetSysColor(COLOR_MENU);

	text = str;
}

void CIndicftor::Update()
{
	if(m_hWnd == NULL) return;
	Invalidate(true);
    UpdateWindow();
}

BOOL CIndicftor::CreateMemoryPaintDC(CDC* pDC)
{
    CPaintDC* pDCPaint = (CPaintDC*)pDC;
    rectClip = pDCPaint->m_ps.rcPaint;
    int cxClip = rectClip.Width();
    int cyClip = rectClip.Height();
    if(cxClip <= 0 || cyClip <= 0) return false;

    if (m_pDCMem == NULL)
    {
        m_pDCMem = new CDC();
        ASSERT(m_pDCMem != NULL);
        VERIFY(m_pDCMem->CreateCompatibleDC(pDC));
    }

    if (m_pBitmap == NULL)
        m_pBitmap = new CBitmap();

    if (cxClip > m_sizeBitmap.cx || cyClip > m_sizeBitmap.cy)
    {
        if (m_pbitmapOld != NULL)
        {
            m_pDCMem->SelectObject(m_pbitmapOld);
            m_pBitmap->DeleteObject();
        }
        VERIFY(m_pBitmap->CreateCompatibleBitmap(pDC, cxClip, cyClip));
        m_pbitmapOld  = m_pDCMem->SelectObject(m_pBitmap);
        m_sizeBitmap.cx = cxClip;
        m_sizeBitmap.cy = cyClip;
    }
	return true;
}

void CIndicftor::DrawInd(CDC* pDC)
{
	CRect rec;
	GetClientRect(&rec);

    if(!CreateMemoryPaintDC(pDC)) return;
	m_pDCMem->FillSolidRect(&rec, colorFonCur);

	CFont *pFont;
	pFont = m_pDCMem->SelectObject(&fFont);
	ASSERT(pFont);

	CSize sz;
	int xb, yb;
	m_pDCMem->SetTextColor(colorTextCur);
	m_pDCMem->SetBkColor(colorFonCur);

	if(m_style == 0)
	{
		sz = m_pDCMem->GetTextExtent(text);
		xb = (rec.Width() - sz.cx)/2;
		xb = (xb > 0) ? xb : 0; 

		yb = (rec.Height() - sz.cy)/2;
		yb = (yb > 0) ? yb : 0; 
		
		m_pDCMem->ExtTextOut(xb, yb, ETO_CLIPPED|ETO_OPAQUE, NULL, text, text.GetLength(), NULL);
	}
	else if(m_style == 1)
	{
		sz = m_pDCMem->GetTextExtent(str_data);
		int sz_ty = sz.cy*2.0;
		xb = (rec.Width() - sz.cx)/2;
		xb = (xb > 0) ? xb : 0; 

		yb = (rec.Height() - sz_ty)/2;
		yb = (yb > 0) ? yb : 0; 
		yb -= 1;

		m_pDCMem->ExtTextOut(xb, yb, ETO_CLIPPED|ETO_OPAQUE, NULL, str_data, str_data.GetLength(), NULL);
		sz = m_pDCMem->GetTextExtent(str_time);
		xb = (rec.Width() - sz.cx)/2;
		xb = (xb > 0) ? xb : 0; 

		yb = yb + sz.cy*1.0 - 2;
		yb = (yb > 0) ? yb : 0; 

		m_pDCMem->ExtTextOut(xb, yb, ETO_CLIPPED|ETO_OPAQUE, NULL, str_time, str_time.GetLength(), NULL);
	}
	m_pDCMem->SelectObject(pFont);
	pDC->BitBlt(rec.left,rec.top,rec.Width(),rec.Height(),m_pDCMem,0,0,SRCCOPY);
}

void CIndicftor::SetText(CString str, BOOL flg)
{
	text = str;
	if(flg) Update();
}

void CIndicftor::SetColor(COLORREF colorText, COLORREF colorFon, BOOL flg)
{
	colorText = colorText;
	colorFon = colorFon;
	if(flg) Update();
}

void CIndicftor::SetDopColor(COLORREF colorText, COLORREF colorFon, BOOL flg)
{
	colorTextDop = colorText;
	colorFonDop = colorFon;
	if(flg) Update();
}

void CIndicftor::SetColorBlink(COLORREF color1, COLORREF color2, BOOL flg)
{
	color_fon_blink1 = color1;
	color_fon_blink2 = color2;
	if(flg) Update();
}

void CIndicftor::SetPosition(CRect& rec)
{
	SetWindowPos(&wndTop, rec.left, rec.top, rec.Width(), rec.Height(), SWP_SHOWWINDOW);
}

void CIndicftor::SetState(int numState, BOOL fl_redraw)
{
	if(numStateW == numState && !fl_redraw) return;
	numStateW = numState;

	if(numStateW == 0)
	{
		colorTextCur = colorText;
		colorFonCur = colorFon;
	}
	else if(numStateW == 1)
	{
		colorTextCur = colorTextDop;
		colorFonCur = colorFonDop;
	}

	Update();
}

int CIndicftor::GetState()
{
	return numStateW;
}

LOGFONT CIndicftor::SetFont(LOGFONT lf)
{
	LOGFONT slf = lfW;
	lfW = lf;

	if(fFont.m_hObject) fFont.DeleteObject();
	fFont.CreateFontIndirect(&lfW);

	Update();
	return slf;
}

LOGFONT CIndicftor::GetFont()
{
	return lfW;
}

int CIndicftor::SetSizeText(int sze_text, BOOL flg)
{
	int sz = -lfW.lfHeight;
	lfW.lfHeight = -sze_text;

	if(fFont.m_hObject) fFont.DeleteObject();
	fFont.CreateFontIndirect(&lfW);

	if(flg) Update();
	return sz;
}
void CIndicftor::SetStyle(int fl_style, COLORREF color_text, COLORREF color_fon, int size_text) 
{
	m_style = fl_style;
	colorText = color_text;
	colorFon = color_fon;
	if(size_text != 0 || lfW.lfHeight != -size_text)
	{
		fFont.DeleteObject();
		LOGFONT lf = lfW;
		lf.lfHeight = -size_text;
		lf.lfWeight = FW_BOLD;
		fFont.CreateFontIndirect(&lf);;
	}
	if (m_style == 1)
	{
		colorTextCur = colorText;
		colorFonCur = colorFon;

		COleDateTime tm = COleDateTime::GetCurrentTime();
		str_data = GetStrData(tm);
		str_time = GetStrTime(tm);
		Update();
		SetTimer(2, 100, NULL);
	}
}

#include <Mmsystem.h>
void CIndicftor::SetBlink(int flag_blink) 
{
	fl_blink = flag_blink;
	if (fl_blink)
	{
		SetTimer(1, 400, NULL);
		num_post_blink = 4;
		numStateWS = numStateW;
		clr1 = colorFon;
		clr2 = colorFonDop;
		colorFon = color_fon_blink1;
		colorFonDop = color_fon_blink2;
	}
    TIMECAPS mtim;
    timeGetDevCaps(&mtim, sizeof(mtim));
}


void CIndicftor::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		SetState(numStateW ? 0 : 1, true);
		if(!fl_blink)
		{
			if (--num_post_blink == 0)
			{
				KillTimer(1);
				numStateW = numStateWS;
				colorFon = clr1;
				colorFonDop = clr2;
				SetState(numStateWS, true);
			}
		}
	}
	else if(nIDEvent == 2)
	{
		COleDateTime tm = COleDateTime::GetCurrentTime();
		str_data = GetStrData(tm);
		str_time = GetStrTime(tm);
		Update();
	}
	CStatic::OnTimer(nIDEvent);
}


