// EditClr.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "EditClr.h"
#include "Param.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditClr

CEditClr::CEditClr()
{
	m_pParam = NULL;
}

CEditClr::~CEditClr()
{
	fFontName.DeleteObject();
}


BEGIN_MESSAGE_MAP(CEditClr, CStatic)
	//{{AFX_MSG_MAP(CEditClr)
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditClr message handlers

int CEditClr::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	LOGFONT  logfontN = {
		-14, 0, 0, 0, FW_BOLD, 1, 0, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	m_logfontN = logfontN;

	if(fFontName.m_hObject) fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&m_logfontN);
	
	return 0;
}

void CEditClr::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rec;
	GetClientRect(&rec);

	dc.DrawEdge(&rec, EDGE_SUNKEN,BF_RECT);
//------------------------------------------------------------------
	if(!m_pParam) return;
	CParam *pPrm = (CParam *)m_pParam;
	CFont *pFont = dc.SelectObject(&fFontName);

	rec.InflateRect(-2,-2);

	CString cs;
	cs.Format("%.2f", pPrm->GetAvrg());

	dc.IntersectClipRect(&rec);

	dc.SetTextColor(RGB(0,0,255));
	dc.TextOut(rec.left, rec.top, cs);

	dc.SelectObject(pFont);
}

/////////////////////////////////////////////////////////////////////////////
// CItemClr

CItemClr::CItemClr()
{
	m_pParam = NULL;
}

CItemClr::~CItemClr()
{
}


BEGIN_MESSAGE_MAP(CItemClr, CStatic)
	//{{AFX_MSG_MAP(CItemClr)
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemClr message handlers

int CItemClr::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	CPoint point(100, 5);
	CSize size(100, 20);
	CRect rec(point, size);

	GetClientRect(&rec);
	rec.InflateRect(-5,-5);
	rec.left = rec.right - 100;

	m_eEdit.Create(NULL, WS_CHILD|WS_VISIBLE, rec, this);

	LOGFONT  logfontN = {
		-14, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	m_logfontN = logfontN;

	if(fFontName.m_hObject) fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&m_logfontN);

	return 0;
}

void CItemClr::OnPaint() 
{
	if(!m_pParam) return;
	CParam *pPrm = (CParam *)m_pParam;

	CPaintDC dc(this); // device context for painting

	COLORREF clr = RGB(255,0,0);
	CFont *pFont = dc.SelectObject(&fFontName);

	CRect rec, rec1;
	GetClientRect(&rec);

	dc.DrawEdge(&rec, EDGE_RAISED,BF_RECT);
	rec.InflateRect(-3,-3);
	dc.DrawEdge(&rec, EDGE_ETCHED,BF_RECT);

//------------------------------------------------------------------
	GetClientRect(&rec);
	rec.InflateRect(-5,-5);
	rec.OffsetRect(2,2);

	m_eEdit.GetClientRect(&rec1);
	m_eEdit.ClientToScreen(&rec1);
	ScreenToClient(&rec1);
	rec.right = rec1.left - 4;

	dc.IntersectClipRect(&rec);

	dc.SetBkColor(GetSysColor(COLOR_MENU));
	dc.SetTextColor(RGB(0,0,0));
	dc.TextOut(rec.left, rec.top, pPrm->sGraphName);

	dc.SelectObject(pFont);
}

/////////////////////////////////////////////////////////////////////////////////////

void CItemClr::SetParam(LPVOID pParam)
{
	m_pParam = pParam;
	m_eEdit.m_pParam = pParam;

	Invalidate(true);
}

void CItemClr::UpdateDataParam()
{
	if(m_eEdit.m_hWnd) m_eEdit.Invalidate(true);
}
/////////////////////////////////////////////////////////////////////////////
// CEditClr2

CEditClr2::CEditClr2()
{
}

CEditClr2::~CEditClr2()
{
}


BEGIN_MESSAGE_MAP(CEditClr2, CStatic)
	//{{AFX_MSG_MAP(CEditClr2)
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditClr2 message handlers
int CEditClr2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	LOGFONT  logfontN = {
		-14, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	m_logfontN = logfontN;

	if(fFontName.m_hObject) fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&m_logfontN);
	
	return 0;
}

void CEditClr2::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	
	CRect rec;
	GetClientRect(&rec);

//------------------------------------------------------------------
	if(!m_pParam) return;
	CParam *pPrm = (CParam *)m_pParam;
	if(iNumActiveList != pPrm->iItemParamA.m_numList) return;

	
	CFont *pFont = dc.SelectObject(&fFontName);


	rec.InflateRect(-2,-2);

	CString cs;
	cs.Format("%.2f", pPrm->GetAvrg());

	dc.IntersectClipRect(&rec);

	dc.SetTextColor(RGB(0,0,255));
	dc.TextOut(rec.left, rec.top, cs);

	dc.SelectObject(pFont);
}

/////////////////////////////////////////////////////////////////////////////
// CItemClr2

CItemClr2::CItemClr2()
{
	m_pParam = NULL;
	m_numList = 0;
}

CItemClr2::~CItemClr2()
{
}


BEGIN_MESSAGE_MAP(CItemClr2, CStatic)
	//{{AFX_MSG_MAP(CItemClr2)
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CItemClr2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	CPoint point(100, 5);
	CSize size(100, 20);
	CRect rec(point, size);

	GetClientRect(&rec);
	rec.InflateRect(-5,-5);
	rec.left = rec.right - 40;
	rec.right += 25;

	m_eEdit.Create(NULL, WS_CHILD|WS_VISIBLE, rec, this);

	LOGFONT  logfontN = {
		-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	m_logfontN = logfontN;

	if(fFontName.m_hObject) fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&m_logfontN);

	return 0;
}

void CItemClr2::OnPaint() 
{
	if(!m_pParam) return;
	CParam *pPrm = (CParam *)m_pParam;

	CPaintDC dc(this); 

	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if(pFrm == NULL) return;
	if(iNumActiveList != m_numList) return;
	
	
	COLORREF clr = RGB(255,0,0);
	CFont *pFont = dc.SelectObject(&fFontName);

	CRect rec, rec1;
	GetClientRect(&rec);

	GetClientRect(&rec);
	rec.InflateRect(-5,-5);
	rec.OffsetRect(2,2);

	m_eEdit.GetClientRect(&rec1);
	m_eEdit.ClientToScreen(&rec1);
	ScreenToClient(&rec1);
	rec.right = rec1.left - 4;


	dc.IntersectClipRect(&rec);

	CString cs = pPrm->sGraphName + " " + pPrm->sRazmernPar + ":";
	dc.SetBkColor(GetSysColor(COLOR_MENU));
	dc.SetTextColor(RGB(0,0,0));
	dc.TextOut(rec.left, rec.top, cs);

	dc.SelectObject(pFont);

}

void CItemClr2::SetParam(LPVOID pParam, int numList)
{
	m_pParam = pParam;
	m_eEdit.m_pParam = pParam;

	m_numList = numList;

	CParam *pPrm = (CParam *)m_pParam;
	Invalidate(true);
}

void CItemClr2::UpdateDataParam()
{
	if(m_eEdit.m_hWnd) m_eEdit.Invalidate(true);
}
