#include "stdafx.h"
#include "Graph.h"
#include <complex>

#include <afxtempl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

#define IDC_STATIC_FIELD 111
#define ID_EDIT_OUT 112

#define C0_TM 36508
#define PM_OUTDIGITGRAPH 32000

#define MIN_DATE                (-657434L)  
#define MAX_DATE                2958465L    


CString shablon("00:00:00");

int indexData = 0;
BOOL testS;
CSize* CFieldGraph::prnShiftXY = NULL;

CRect CFieldGraph::slctRect(0,0,0,0);
int CGraph::scroll_pos = 0, CGraph::scroll_max = 100;

PrinterParam prn;

BOOL FlagChangeScaleRedraw = true;

#define sqr(x) ((x)*(x))
TMarker<F>* pMarker0 = NULL;


void Daraw2Rect(CDC* pDC, CRect& rec, COLORREF colr = 0)
{
    CPen *ppn, pn(PS_SOLID, 2, colr);
    ppn = pDC->SelectObject(&pn);
    ASSERT(ppn);

    pDC->MoveTo(rec.left, rec.top);
    pDC->LineTo(rec.right, rec.top);
    pDC->LineTo(rec.right, rec.bottom);
    pDC->LineTo(rec.left, rec.bottom);
    pDC->LineTo(rec.left, rec.top);

    pDC->SelectObject(ppn);
}

CString GetStrTime(F tim)
{
    COleDateTimeSpan dt = tim;
    return dt.Format("%H:%M:%S");
}

inline int DivRoundOn(int in_val, int div)
{
    int k = in_val/div;
    return in_val > k*div ? k+1 : k; 
}

CValue::CValue()
{
}

CValue::~CValue()
{
}


BEGIN_MESSAGE_MAP(CValue, CToolTipCtrl)
    //{{AFX_MSG_MAP(CValue)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CPanel, CStatic)

CPanel::CPanel()
{
}

CPanel::~CPanel()
{
}


BEGIN_MESSAGE_MAP(CPanel, CStatic)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_BN_CLICKED(10000, OnBnClickedCheckPoint)
	ON_BN_CLICKED(10002, OnBnClickedCheckGreed)
	ON_BN_CLICKED(10004, OnBnClickedCheckFixData)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

int CPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_point_view.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BS_FLAT, CRect(10,10,15,15), this,10000);
	if(pFieldGraph->fldStyle.bitFlags.flTypePoint) m_point_view.SetCheck(BST_CHECKED);
	else m_point_view.SetCheck(BST_UNCHECKED);

	m_greed_view.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BS_FLAT, CRect(10,10,15,15), this,10002);
	if(pFieldGraph->fldStyle.bitFlags.flGreed) m_greed_view.SetCheck(BST_CHECKED);
	else m_greed_view.SetCheck(BST_UNCHECKED);

	m_fix_data.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BS_FLAT, CRect(10,10,15,15), this,10004);

	return 0;
}
void CPanel::OnPaint()
{
	CPaintDC dc(this); 
	DrawPointerGrf(&dc);
}
void CPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	FindePointerGrf(point);

	CStatic::OnLButtonDown(nFlags, point);
}

void CPanel::OnBnClickedCheckPoint()
{
	if(m_point_view.GetCheck() == BST_CHECKED) pFieldGraph->fldStyle.bitFlags.flTypePoint = true;
	else pFieldGraph->fldStyle.bitFlags.flTypePoint = false;

	pFieldGraph->Invalidate(true);
	int a = 0;
}
void CPanel::OnBnClickedCheckGreed()
{
	if(m_greed_view.GetCheck() == BST_CHECKED) pFieldGraph->fldStyle.bitFlags.flGreed = true;
	else pFieldGraph->fldStyle.bitFlags.flGreed = false;

	pFieldGraph->Invalidate(true);
	int a = 0;
}
void CPanel::OnBnClickedCheckFixData()
{
	if(m_fix_data.GetCheck() == BST_CHECKED) 
	{
		pFieldGraph->fldStyle.bitFlags.flFixData = true;
		CRect rec;
		GetClientRect(&rec);
		rec.bottom = rec.top + 15;

		if(pFieldGraph->m_val.m_hWnd != NULL) pFieldGraph->m_val.SetWindowPos(&wndBottom,
			rec.left, 
			rec.top, 
			rec.Width() - 2, 
			rec.Height(), 
			SWP_SHOWWINDOW);
	}
	else
	{
		pFieldGraph->fldStyle.bitFlags.flFixData = false;
		if(pFieldGraph->m_val.m_hWnd != NULL) pFieldGraph->m_val.ShowWindow(SW_HIDE);
	}

	pFieldGraph->Invalidate(true);
	int a = 0;
}
void CPanel::RecalcSize()
{
	CRect rec;
	GetClientRect(&rec);
	size_check = 15;
	rec_ch = CRect(rec.left, rec.top, rec.right+size_check, rec.top+size_check);
	m_point_view.SetWindowPos(&wndBottom, rec_ch.left, rec_ch.top, rec_ch.Width(), 
		rec_ch.Height(), SWP_SHOWWINDOW);

	rec_ch.top += size_check + 2;
	rec_ch.bottom += size_check + 2;
	m_greed_view.SetWindowPos(&wndBottom, rec_ch.left, rec_ch.top, rec_ch.Width(), 
		rec_ch.Height(), SWP_SHOWWINDOW);

	rec_ch.top += size_check + 2;
	rec_ch.bottom += size_check + 2;
	m_fix_data.SetWindowPos(&wndBottom, rec_ch.left, rec_ch.top, rec_ch.Width(), 
		rec_ch.Height(), SWP_SHOWWINDOW);

	index_check = 3;
}

void CPanel::FindePointerGrf(CPoint point)
{
	if(!pFieldGraph || !pVScale) return;
	int count = pFieldGraph->DATA.GetNumBuffers();
	VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS];
	pFieldGraph->DATA.GetTBLPtrBuffers(tblPtrBuffers);

	CRect rec;
	GetClientRect(&rec);
	int save_index_check = index_check;
	rec_ch = CRect(rec.left, rec.top + (size_check + 2)*(index_check), rec.right, rec.top + (size_check + 2)*(index_check + 1) - 2);

	pVScale->colorText = colorText;
	int num_select_buf = 0;
	for(int nb = 0; nb < count; nb++)
	{
		VFRingBuf<F> *pBuffer = tblPtrBuffers[nb];
		if(pBuffer->GetSizeData() == 0 || !pBuffer->Get_flg_view_data()) continue;

		if(rec_ch.PtInRect(point)) 
		{
			pVScale->colorText = pBuffer->colorDraw;
			num_select_buf = pBuffer->num_self;
			break;
		}

		rec_ch.top += size_check + 2;
		rec_ch.bottom += size_check + 2;
	}
	index_check = save_index_check;
	pFieldGraph->DATA.GetMinMax(num_select_buf, pVScale->Fmin, pVScale->Fmax);
	pVScale->Invalidate(false);
}

void CPanel::DrawPointerGrf(CDC* pDC)
{
	if(!pFieldGraph || !pVScale) return;
	int count = pFieldGraph->DATA.GetNumBuffers();
	VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS];
	pFieldGraph->DATA.GetTBLPtrBuffers(tblPtrBuffers);

	CRect rec;
	GetClientRect(&rec);
	int save_index_check = index_check;
	rec_ch = CRect(rec.left, rec.top + (size_check + 2)*(index_check), rec.right, rec.top + (size_check + 2)*(index_check + 1) - 2);

	colorText = pVScale->colorText;
	for(int nb = 0; nb < count; nb++)
	{
		VFRingBuf<F> *pBuffer = tblPtrBuffers[nb];
		if(pBuffer->GetSizeData() == 0 || !pBuffer->Get_flg_view_data()) continue;

		pDC->FillSolidRect(&rec_ch, pBuffer->colorDraw);

		rec_ch.top += size_check + 2;
		rec_ch.bottom += size_check + 2;
	}
	index_check = save_index_check;
}

COuot::COuot()
{
    m_bType = true;
}

COuot::~COuot()
{
}


BEGIN_MESSAGE_MAP(COuot, CStatic)
    //{{AFX_MSG_MAP(COuot)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CString COuot::GetMonth(WORD& wmonth)
{
    CString month;
    switch(wmonth)
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

    return month;
}

void COuot::OnPaint() 
{
    CPaintDC dc(this); 
    CFieldGraph* pFld = (CFieldGraph*)m_pFld;

    VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS], *pBuf;
    int num_buf = (int)pFld->DATA.GetTBLPtrBuffers(tblPtrBuffers);

    CBrush br(RGB(0,0,0));
    CFont *pFont = dc.SelectObject(&fFontName);
    ASSERT(pFont);

    CString str, str1;
    F dTime;
    F fVal;

    CRect rec;
    GetParent()->GetClientRect(&rec);
    rec.bottom = rec.top + (num_buf + 1)*15;

    SetWindowPos(&wndBottom,
        rec.left, 
        rec.top, 
        rec.Width()/2 - 1, 
        rec.Height(), 
        SWP_SHOWWINDOW);

    GetClientRect(&rec);

    dc.DrawEdge(&rec, EDGE_SUNKEN,BF_RECT);
    COLORREF clr = RGB(255, 255,255);
    dc.FillSolidRect(&rec, clr);

    dc.FrameRect(&rec, &br);
    rec.InflateRect(-1,-1);
    dc.IntersectClipRect(&rec);

    dTime = pFld->DATA.GetMouseValue(-1);

    CString name_str;
    if(m_bType)
    {
        COleDateTime m_time;
        m_time.m_dt = dTime;
        int day = m_time.GetDay();
        WORD month = m_time.GetMonth();
        CString monthS = GetMonth(month);
        int year = m_time.GetYear();
        int hour = m_time.GetHour();
        int minute = m_time.GetMinute();
        int second = m_time.GetSecond();
        str.Format("%d ", day);
        str += monthS;
        str1.Format(" %d    %02d:%02d:%02d", year, hour, minute, second);
        str += str1;
    }
    else 
    {
        CString str1;
        str = pFld->DATA.GetName(-1);
        str1.Format(" = %.2f", dTime); 
        str += str1;
    }

    dc.TextOut(rec.left, rec.top, str);

    for(int i = 0; i < num_buf; i++)
    {
        pBuf = tblPtrBuffers[i];
        str = pBuf->sName + " = ";
        fVal = pBuf->mouse_value;
        if(pBuf->mouse_index >= 0) str1.Format("%0.2f", fVal);
        else str1 = "НЕТ";
        str += str1;
        dc.SetTextColor(pBuf->colorDraw);
        dc.TextOut(rec.left, rec.top + (i + 1)*15, str);
    }

    dc.SelectObject(pFont);

}

int COuot::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CStatic::OnCreate(lpCreateStruct) == -1)
        return -1;

    LOGFONT  logfontN = {
        -11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    m_logfontN = logfontN;

    fFontName.CreateFontIndirect(&m_logfontN);

    return 0;
}

void COuot::OnDestroy() 
{
    CStatic::OnDestroy();

    fFontName.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// Common function
void inline RotRect90(CRect& rec, BOOL flRotate) 
{
    if(flRotate) rec = CRect(rec.top, rec.left, rec.bottom, rec.right);
}
void inline RotRectCCW90(CRect& rec, BOOL flRotate) 
{
    if(flRotate) rec = CRect(rec.top, rec.left, rec.bottom, rec.right);
}

void inline RotCoord90(int& xk, int& yk, BOOL flRotate) 
{
    if(flRotate) 
    {
        int ii = xk;
        xk = yk;
        yk = ii;
    }
}

void inline RotSize90(CSize& sz, BOOL flRotate) 
{
    if(flRotate) 
    {
        int ii = sz.cx;
        sz.cx = sz.cy;
        sz.cy = ii;
    }
}

void inline DrawLine(CDC *pDC, int xbeg, int ybeg, int xend, int yend, BOOL flRotate)
{
    RotCoord90(xbeg, ybeg, flRotate);
    pDC->MoveTo(xbeg, ybeg);
    RotCoord90(xend, yend, flRotate);
    pDC->LineTo(xend, yend);
}

void inline DrawLine2(CDC *pDC, int xbeg, int ybeg, int xend, int yend, BOOL flRotate)
{
    CPen *ppn, pn(PS_SOLID, 2, RGB(0,0,0));
    ppn = pDC->SelectObject(&pn);
    ASSERT(ppn);

    RotCoord90(xbeg, ybeg, flRotate);
    pDC->MoveTo(xbeg, ybeg);
    RotCoord90(xend, yend, flRotate);
    pDC->LineTo(xend, yend);

    pDC->SelectObject(ppn);
}

void inline PutPixel(CDC *pDC, int xbeg, int ybeg, COLORREF crColor, BOOL flRotate)
{
    RotCoord90(xbeg, ybeg, flRotate);
    pDC->SetPixelV(xbeg, ybeg, crColor);
}


IMPLEMENT_DYNCREATE(CScale, CStatic)

CScale::CScale()
{
	ResetStyle(0xffff);
    sclName				= "";
    sclNstrich			= 10;

    Fmin = 0.0f; 
    Fmax = 768.0f;

    basePoint	= CPoint(0,0);
    bazeSize	= CSize(0,0);

    sizeMainStrich	= CSize(1,7); 
    sizeSlaveStrich = CSize(1,3);

    colorBaseLine		= RGB(240,240,240); 
    colorMainStrich		= RGB(0,0,255);
    colorSlaveStrich	= RGB(0,0,0);
    colorFon			= GetSysColor(COLOR_MENU);
    colorValue			= RGB(0,0,255);
    colorName			= RGB(0,0,0);
    colorText			= RGB(0,0,255);
    colorHeadAlert		= RGB(255,100,100);
    colorHead			= colorFon;

    uScale = CSize(1,1);

    //.................................................................................

    LOGFONT logfontM = {
        -11, 0, 900, 900, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    LOGFONT  logfontN = {
        -11, 0, 900, 900, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    m_logfontM = logfontM;
    m_logfontN = logfontN;
    //.................................................................................


    typeSlideScale = 0;

    shiftStrichText = 5;
    m_fldGraph = NULL;
}

CScale::CScale(CString sName, UINT Nstrich)
{
	ResetStyle(0xffff);
    sclName					= sName;
    sclNstrich				= Nstrich;

    Fmin = 0.0f; 
    Fmax = 768.0f;

    basePoint	= CPoint(0,0);
    bazeSize	= CSize(0,0);

    sizeMainStrich	= CSize(1,7); 
    sizeSlaveStrich = CSize(1,3);

    colorBaseLine		= RGB(240,240,240); 
    colorMainStrich		= RGB(0,0,255);
    colorSlaveStrich	= RGB(0,0,255);
    colorFon			= GetSysColor(COLOR_MENU);
    colorValue			= RGB(0,0,255);
    colorName			= RGB(0,0,0);
    colorText			= RGB(0,0,255);
    colorHeadAlert		= RGB(255,100,100);
    colorHead			= colorFon;

    uScale = CSize(1,1);

    //.................................................................................
    LOGFONT logfontM = {
        -11, 0, 900, 900, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    LOGFONT  logfontN = {
        -11, 0, 900, 900, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    m_logfontM = logfontM;
    m_logfontN = logfontN;
    //.................................................................................

    typeSlideScale = 0;

    shiftStrichText = 3;
    m_fldGraph = NULL;

    //================================================================================
}

CScale::~CScale()
{
}


CScale& CScale::operator=(CScale& scl)
{
    sclStyle	= scl.sclStyle;
    sclName		= scl.sclName;
    sclNstrich	= scl.sclNstrich;

    Fmin = scl.Fmin; 
    Fmax = scl.Fmax;

    basePoint	= scl.basePoint;
    bazeSize	= scl.bazeSize;

    sizeMainStrich	= scl.sizeMainStrich; 
    sizeSlaveStrich = scl.sizeSlaveStrich;

    colorBaseLine		= scl.colorBaseLine; 
    colorMainStrich		= scl.colorMainStrich;
    colorSlaveStrich	= scl.colorSlaveStrich;
    colorFon			= scl.colorFon;
    colorValue			= scl.colorValue;
    colorName			= scl.colorName;
    colorText			= scl.colorText;
    colorHeadAlert		= scl.colorHeadAlert;
    colorHead			= scl.colorHead;

    m_logfontM = scl.m_logfontM;
    m_logfontN = scl.m_logfontN;

    uScale = scl.uScale;

    typeSlideScale = scl.typeSlideScale;

    shiftStrichText = scl.shiftStrichText;
    m_fldGraph = scl.m_fldGraph;

    return *this;
}


BEGIN_MESSAGE_MAP(CScale, CStatic)
    //{{AFX_MSG_MAP(CScale)
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_RBUTTONDOWN()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CScale::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CStatic::OnCreate(lpCreateStruct) == -1)
        return -1;

    if(fFontMain.m_hObject) fFontMain.DeleteObject();
    if(fFontName.m_hObject) fFontName.DeleteObject();

	m_logfontM.lfEscapement = m_logfontM.lfOrientation = 900*int(sclStyle.flOrientFontM);
	m_logfontN.lfEscapement = m_logfontM.lfOrientation = 900*int(sclStyle.flOrientFontN);

    fFontMain.CreateFontIndirect(&m_logfontM);
    fFontName.CreateFontIndirect(&m_logfontN);

    pnBaseLine.CreatePen(PS_SOLID, 1, colorBaseLine);
    pnMainStrich.CreatePen(PS_SOLID, sizeMainStrich.cx, colorMainStrich);
    pnSlaveStrich.CreatePen(PS_SOLID, sizeSlaveStrich.cx, colorSlaveStrich);

    return 0;
}

void CScale::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {	// storing code
    }
    else
    {	// loading code
    }
}

void CScale::OnDestroy() 
{
    CStatic::OnDestroy();

    if(fFontMain.m_hObject) fFontMain.DeleteObject();
    if(fFontName.m_hObject) fFontName.DeleteObject();

    pnBaseLine.DeleteObject();
    pnMainStrich.DeleteObject();
    pnSlaveStrich.DeleteObject();
}

void CScale::OnRButtonDown(UINT nFlags, CPoint point) 
{
    GetParent()->SendMessage(PM_R_BUTTON_DOWN, 1, 0);

    CStatic::OnRButtonDown(nFlags, point);
}

LOGFONT CScale::SetFontM(LOGFONT& lf)
{
    LOGFONT slf = m_logfontM;
    m_logfontM = lf;

    if(fFontMain.m_hObject) fFontMain.DeleteObject();
    fFontMain.CreateFontIndirect(&m_logfontM);

    if(lf.lfEscapement != 0) sclStyle.flOrientFontM = 1;
    else sclStyle.flOrientFontM = 0;

    if(m_hWnd) Invalidate(true);

    return slf;
}


LOGFONT CScale::SetFontN(LOGFONT& lf)
{
    LOGFONT slf = m_logfontN;
    m_logfontN = lf;

    if(fFontName.m_hObject) fFontName.DeleteObject();
    fFontName.CreateFontIndirect(&m_logfontN);

    if(m_hWnd) Invalidate(true);

    return slf;
}


int CScale::SetSizeFontM(int sz)
{
    int ssz = -m_logfontM.lfHeight;
    m_logfontM.lfHeight = -sz;

    if(fFontMain.m_hObject) fFontMain.DeleteObject();
    fFontMain.CreateFontIndirect(&m_logfontM);

    if(m_hWnd) Invalidate(true);

    return ssz;
}


int CScale::SetSizeFontN(int sz)
{
    int ssz = -m_logfontN.lfHeight;
    m_logfontN.lfHeight = -sz;

    if(fFontName.m_hObject) fFontName.DeleteObject();
    fFontName.CreateFontIndirect(&m_logfontN);

    if(m_hWnd) Invalidate(true);

    return ssz;
}

BOOL CScale::SetOrientFontM(BOOL flg)
{
    BOOL sflg = sclStyle.flOrientFontM;
    sclStyle.flOrientFontM = flg;

    m_logfontM.lfEscapement = m_logfontM.lfOrientation = 900*int(flg);

    if(fFontMain.m_hObject) fFontMain.DeleteObject();
    fFontMain.CreateFontIndirect(&m_logfontM);

    return sflg;
}

BOOL CScale::SetOrientFontN(BOOL flg)
{
    BOOL sflg = sclStyle.flOrientFontN;
    sclStyle.flOrientFontN = flg;

    m_logfontN.lfEscapement = m_logfontN.lfOrientation = 900*int(flg);

    if(fFontName.m_hObject) fFontName.DeleteObject();
    fFontName.CreateFontIndirect(&m_logfontN);
    return sflg;
}

void CScale::UpdateOrientFont()
{
	SetOrientFontN(sclStyle.flOrientFontN);
	SetOrientFontM(sclStyle.flOrientFontM);
}

void CScale::SetViewNameD(BOOL flg)
{
    sclStyle.flName = flg;
}


void CScale::SetColorFon(COLORREF color)
{
    colorFon = color;
}

void CScale::SetPrintFontP(F scl) 
{
    LOGFONT logfontM, logfontN;
    fFontMain.GetLogFont(&logfontM);
    fFontName.GetLogFont(&logfontN);
	SetPrintFont(!sclStyle.flOrient ? scl : scl*1.5, logfontN.lfOrientation != 0, logfontM.lfOrientation != 0);
}

void CScale::SetPrintFont(F scl, bool flOrientN, bool flOrientM) 
{
    LOGFONT logfontN;
    fFontName.GetLogFont(&logfontN);

    logfontN.lfEscapement = logfontN.lfOrientation = 900*int(flOrientN);
    logfontN.lfHeight *= scl;

    if(fFontName.m_hObject) fFontName.DeleteObject();
    fFontName.CreateFontIndirect(&logfontN);
    LOGFONT logfontM;
    fFontMain.GetLogFont(&logfontM);

    logfontM.lfEscapement = logfontM.lfOrientation = 900*int(flOrientM);
    logfontM.lfHeight *= scl;

    if(fFontMain.m_hObject) fFontMain.DeleteObject();
    fFontMain.CreateFontIndirect(&logfontN);
}


void CScale::SetNormalFont() 
{
    fFontMain.DeleteObject();
    fFontName.DeleteObject();

    fFontMain.CreateFontIndirect(&m_logfontM);
    fFontName.CreateFontIndirect(&m_logfontN);
}

CRect CScale::GetInflateSizeA(CDC* pDC)
{
    if(!sclStyle.flView) return CRect(0,0,0,0);

    CRect inflateSize;
    int len;
    CFont *pFont = pDC->SelectObject(&fFontMain);
    ASSERT(pFont);
    CSize inflateSizeM = GetSizeText(pDC, shablon, &fFontMain, sclStyle.flViewDgt);
    pDC->SelectObject(&fFontName);
    CSize inflateSizeN = GetSizeText(pDC, sclName, &fFontName, sclStyle.flName);

    pDC->SelectObject(pFont);

	float szX = 1.0;
	float szY = 1.0;
	if(prn.pDC != NULL)
	{
		szX = prn.k_x;
		szY = prn.k_y;
	}

    if(!sclStyle.flOrient)
    {
        len = (sizeMainStrich.cy + 2*shiftStrichText)*szY;

        if(!sclStyle.flName) len += inflateSizeM.cy;
        else len += __max(inflateSizeM.cy, inflateSizeN.cy);

        inflateSize.left = len;

        inflateSize.top		= DivRoundOn(inflateSizeM.cx, 2);

        if(!sclStyle.flName) inflateSize.right	= DivRoundOn(inflateSizeM.cx, 2);
        else inflateSize.right	= DivRoundOn(inflateSizeN.cx, 2);
    }
    else
    {
        len = (sizeMainStrich.cy + 2*shiftStrichText)*szX;

        if(!sclStyle.flName) len += inflateSizeM.cx;
        else len += __max(inflateSizeM.cx, inflateSizeN.cx);

        inflateSize.left = len;

        inflateSize.top		= DivRoundOn(inflateSizeM.cy, 2);

        if(!sclStyle.flName) inflateSize.right	= DivRoundOn(inflateSizeM.cy, 2);
        else inflateSize.right	= DivRoundOn(inflateSizeN.cy, 2);
    }
    return inflateSize;
}

CRect CScale::GetInflateSize()
{
    if(m_hWnd == NULL) return CRect(0,0,0,0);
//    CClientDC dc(this);
	CClientDC *pDC = new CClientDC(this);
	CRect rec = GetInflateSizeA(pDC);
	delete pDC;
    return rec;
}

void CScale::SetMinMax(F FminX, F FmaxX)
{
    Fmin = FminX;
    Fmax = FmaxX;
    ChangeString();
}

CSize CScale::GetSizeText(CDC *pDC, const CString& str, CFont *pFont, BOOL flgView)
{
    LOGFONT logfont;
    CSize sz(0,0);

    if(flgView)
    {
        sz = pDC->GetTextExtent(str);
        if(pFont->m_hObject != NULL) pFont->GetLogFont(&logfont);

        CSize sz1 = sz;
        if(logfont.lfEscapement != 0)
        {
			sz.cx = sz1.cy; 
			sz.cy = sz1.cx; 
		}
	}
    return sz;
}

void CScale::SetStyle(DWORD dwStyle)
{
    dwordScaleFlags flg;
	flg.dwFlags = 0;
    flg.dwFlags |= dwStyle;
    sclStyle = flg.bitFlags;
}
void CScale::ResetStyle(DWORD dwStyle)
{
	dwStyle = ~dwStyle;
	dwordScaleFlags flg;
	flg.bitFlags = sclStyle;
	flg.dwFlags &= dwStyle;
	sclStyle = flg.bitFlags;
}
DWORD CScale::GetStyle()
{
    dwordScaleFlags flg;
    flg.bitFlags = sclStyle;
    return flg.dwFlags;
}

void CScale::SetColorHaderAlert(COLORREF color)
{
    if(color != colorHead)
    {
        colorHeadAlert = color;
        colorHead = color;
        if(m_hWnd) Invalidate(true);
    }
}

void CScale::CalcScaleStrichX(CRect *rec)
{
    if(!m_hWnd) return;

    F stepMainStrich;
    int LRec, xleft, xright;
    CRect sclRec;

    if(rec != 0) sclRec = *rec;
    else GetClientRect(&sclRec);

    if(sclRec == 0) return;

    if(rec == 0) 
        RotRect90(sclRec, sclStyle.flOrient);

    xleft = sclRec.left;
    xright = sclRec.right;

    LRec = sclRec.Width();
    stepMainStrich = float(LRec)/(sclNstrich - 1.0f);

    strichScaleMain[0] = xleft;
    strichScaleMain[sclNstrich - 1] = xright - 1;
    for(int i = 1; i < sclNstrich - 1; i++) strichScaleMain[i] = xleft + int(stepMainStrich*i);
}


void CScale::DrawXScl(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
    if(!m_hWnd) return;
    if(!sclStyle.flView) return;

    int x1, x2, y1, y2, count, k1, ybs, lr;
    int i;
    CRect sclRec, rec, recClr;
    CSize sz, szt;
    CPoint ltPoint;
    CFont *pFont = NULL;
    CPen *pns = NULL;
    float shft;
	float k_prn = 1.0f;

    CString clr1("                 "), str;
    F df = (Fmax - Fmin)/(sclNstrich - 1.0f);

    GetClientRect(&sclRec);
    pDC->SetBkMode(TRANSPARENT);

    int fl_print = 0;
    if(pDC->IsPrinting()) 
    {
        if(!pWnd) return;
        ClientToScreen(&sclRec);
        fl_print = 1;
		k_prn = !sclStyle.flOrient ? 0.0f : 1.0f;
    }
    else
    {
        pDC->FillSolidRect(&sclRec, colorFon);
    }
    int xleft = sclRec.left, 
        xright = sclRec.right,
        ytop = sclRec.top, 
        ybtm = sclRec.bottom;

    if(sclStyle.flOrientInvert)
    {
        k1 = -1;
        ybs = ybtm-1; 
    }
    else
    {
        k1 = 1;
        ybs = ytop; 
    }

    lr = int(sclStyle.flOrientInvert);

    pns = pDC->SelectObject(&pnMainStrich);
    ASSERT(pns);

    if(sclStyle.flMainStrich)
    {
        y1 = ybs + sclStyle.flOrientInvert;
        for(i = 0; i < sclNstrich; i++)
        {
            x1 = strichScaleMain[i];
            pDC->FillSolidRect(x1, y1, int(sclX*sizeMainStrich.cx), int(k1*sclY*(sizeMainStrich.cy + sclStyle.flBaseLine)), colorMainStrich);
        }
    }
    if(sclStyle.flSlaveStrich)
    {
        y1 = ybs + sclStyle.flOrientInvert;
        shft = (strichScaleMain[1] - strichScaleMain[0])/2.0f;
        for(i = 0; i < sclNstrich - 1; i++)
        {
            x1 = int(shft) + strichScaleMain[i];
            pDC->FillSolidRect(x1, y1, int(sclX*sizeMainStrich.cx), int(k1*sclY*(sizeSlaveStrich.cy + sclStyle.flBaseLine)), colorSlaveStrich);
        }
    }
    if(sclStyle.flBaseLine && !pDC->IsPrinting()) pDC->FillSolidRect(xleft, ybs, xright, 1, colorBaseLine);

	if(sclStyle.flName) count = sclNstrich - 1;
	else count = sclNstrich;

	if(sclStyle.flViewDgt)
    {
        pFont = pDC->SelectObject(&fFontMain);
        ASSERT(pFont);
        pDC->SetTextColor(colorText);

        pDC->SetBkColor(colorFon);

        sz = GetSizeText(pDC, shablon, &fFontMain, sclStyle.flViewDgt);
        int resampling;
        if(sz.cx != 0)
        {
            float mm = float(sclRec.Width())/sz.cx + 1;
            if(int(mm) != 0) resampling = ceil(count/mm);
            else resampling = count; 
        }
        else resampling = 1;

        y1 = ybs + int(k1*sclX*(sizeMainStrich.cy + shiftStrichText))*k_prn + sz.cy*(int(sclStyle.flOrientFontM) - 1)*lr*k_prn;
		for(i = 0; i < count; i+=resampling)
        {
            x1 = -sz.cx/2 + strichScaleMain[i];
            recClr = CRect(CPoint(x1, y1 - sz.cy*int(sclStyle.flOrientFontM)*lr),	sz);
            if(!pDC->IsPrinting()) pDC->FillSolidRect(&recClr, colorFon);

            F ff;
            ff = Fmin + i*df;
            if(!sclStyle.flViewTime)
            {
                F aff = fabs(ff);
                str.Format("%.1f", ff);
            }
            else
            {
                dleta = ff;
                if (dleta.m_dt < MAX_DATE && dleta.m_dt > MIN_DATE)
                    str.Format("%02d:%02d:%02d", dleta.GetHour(), dleta.GetMinute(), dleta.GetSecond()); 
            }
            szt = GetSizeText(pDC, str, &fFontMain, sclStyle.flViewDgt);
            x2 = xleft - szt.cx/2 + (strichScaleMain[i] - xleft);
            y2 = y1 - szt.cy*int(sclStyle.flOrientFontM)*(lr - 1) - (szt.cy*fl_print*(2*lr - 1))/4;

            pDC->TextOut(x2, y2, str);
        }

        pDC->SelectObject(pFont);
    }
    if(sclStyle.flName)
    {
        pFont = pDC->SelectObject(&fFontName);
        ASSERT(pFont);
        pDC->SetTextColor(colorName);

        sz = GetSizeText(pDC, sclName, &fFontName, sclStyle.flName);

        y1 = ybs + k1*(sizeMainStrich.cy + shiftStrichText) + sz.cy*(int(sclStyle.flOrientFontN) - 1)*lr;
        if(sclStyle.flName)
        {
            x1 = xleft - sz.cx/2 + strichScaleMain[count-1];

            recClr = CRect(CPoint(x1, y1 - sz.cy*int(sclStyle.flOrientFontN)*lr),	sz);
            szt = GetSizeText(pDC, sclName, &fFontName, sclStyle.flName);
            x2 = xleft - szt.cx/2 + strichScaleMain[sclNstrich-1];
            y2 = y1 + szt.cy*int(sclStyle.flOrientFontN)*int(!sclStyle.flOrientInvert);

            pDC->TextOut(x2, y2, sclName);
        }
        pDC->SelectObject(pFont);
    }

    pDC->SelectObject(pns);
}

void CScale::SetNumStich(UINT nStrich, CRect* p_rec) 
{
    sclNstrich = nStrich;
}


void CScale::CalcScaleStrichY(CRect* p_rec)
{
    if(!m_hWnd) return;

    F stepMainStrich;
    int LRec, ytop, ybtm;
    CRect sclRec;

    if(p_rec != 0) sclRec = *p_rec;
    else GetClientRect(&sclRec);

    if(sclRec == 0) return;

    if(p_rec == 0) 
        RotRect90(sclRec, !sclStyle.flOrient);

    ytop = sclRec.top;
    ybtm = sclRec.bottom;

    LRec = sclRec.Height();
    stepMainStrich = LRec/(sclNstrich - 1.0f);

    strichScaleMain[0] = ytop;
    strichScaleMain[sclNstrich - 1] = ybtm-1;
    for(int i = 1; i < sclNstrich - 1; i++) strichScaleMain[i] = ytop + (int)(stepMainStrich*i);
}

void CScale::DrawYScl(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
    if(!m_hWnd) return;

    if(!sclStyle.flView) return;

    int x1, x2, y1, count, k1, xbs, lr;
    int i;
    CRect sclRec, rec, recClr;
    CSize sz, szt;
    CPoint ltPoint;
    CFont *pFont = NULL;
    CPen *pns = NULL;
    float shft;

    CString clr1("                 "), str;
    F df = (Fmax - Fmin)/(sclNstrich - 1.0f);

    GetClientRect(&sclRec);
    pDC->SetBkMode(TRANSPARENT);

    if(pDC->IsPrinting()) 
    {
        if(!pWnd) return;

        ClientToScreen(&sclRec);
    }
    else
    {
        pDC->FillSolidRect(&sclRec, colorFon);
    }
    //.................................................................................	
    if(sclRec == 0) return;

    int xleft = sclRec.left, 
        xright = sclRec.right,// - 1, 
        ytop = sclRec.top, 
        ybtm = sclRec.bottom;// - 1;

    if(sclStyle.flOrientInvert)
    {
        k1 = 1;
        xbs = xleft; 
    }
    else
    {
        k1 = -1;
        xbs = xright-1; 
    }

    lr = int(sclStyle.flOrientInvert);
    pns = pDC->SelectObject(&pnMainStrich);
    ASSERT(pns);

    if(sclStyle.flMainStrich)
    {
        x1 = xbs+1;
        for(i = 0; i < sclNstrich; i++)
        {
            y1 = strichScaleMain[i];
            pDC->FillSolidRect(x1, y1, int(k1*sclX*(sizeMainStrich.cy + sclStyle.flBaseLine)), int(sclY*sizeMainStrich.cx), colorMainStrich);
        }
    }
    if(sclStyle.flSlaveStrich)
    {
        x1 = xbs+1;
        shft = (strichScaleMain[1] - strichScaleMain[0])/2.0f;
        for(i = 0; i < sclNstrich - 1; i++)
        {
            y1 = (int)shft + strichScaleMain[i];
            pDC->FillSolidRect(x1, y1, int(k1*sclX*(sizeSlaveStrich.cy + sclStyle.flBaseLine)), int(sclY*sizeSlaveStrich.cx), colorSlaveStrich);
        }
    }
    if(sclStyle.flBaseLine && !pDC->IsPrinting()) pDC->FillSolidRect(xbs, ytop, 1, ybtm, colorBaseLine);

	if(sclStyle.flName) count = sclNstrich - 1;
	else count = sclNstrich;

	if(sclStyle.flViewDgt)
    {
        pFont = pDC->SelectObject(&fFontMain);
        ASSERT(pFont);
        pDC->SetTextColor(colorText);

        pDC->SetBkColor(colorFon);

        sz = GetSizeText(pDC, shablon, &fFontMain, sclStyle.flViewDgt);
        int resampling;
        if(sz.cy != 0)
        {
            float mm = float(sclRec.Height())/sz.cy + 1;
            if(int(mm) != 0) resampling = ceil(count/mm);
            else resampling = count; 
        }
        else resampling = 1;

        x1 = xbs + int(k1*sclX*(sizeMainStrich.cy + shiftStrichText)) + sz.cx*(lr - 1);
        for(i = 0; i < count; i+=resampling)
        {
            y1 = -sz.cy/2 + strichScaleMain[sclNstrich - i - 1];

            recClr = CRect(CPoint(x1 + 2, y1),	sz);
            if(!pDC->IsPrinting()) pDC->FillSolidRect(&recClr, colorFon);

            F ff;
            int sh = 0;

            if(!sclStyle.flTypeScale) ff = Fmin + i*df;
            else ff = Fmax - i*df;

            if(!sclStyle.flViewTime)
            {
                F aff = fabs(ff);
                str.Format("%.1f", ff);
            }
            else
            {
                dleta = ff;
                if (dleta.m_dt < MAX_DATE && dleta.m_dt > MIN_DATE) 
                {
                    if(Fmax - Fmin < 0.5/24.0) 
                    {
						str = dleta.Format("%H:%M:%S");
                        sh = 3;
                    }
                    else str = dleta.Format("%H:%M"); 
                }
            }

            szt = GetSizeText(pDC, str, &fFontMain, sclStyle.flViewDgt);
            x2 = xbs + int(k1*sclX*(sizeMainStrich.cy + shiftStrichText)) + szt.cx*(lr - 1);

            pDC->TextOut(x2 + sh, y1 + (szt.cy + sz.cy)*int(sclStyle.flOrientFontM)/2, str);
        }

        pDC->SelectObject(pFont);
    }
    if(sclStyle.flName)
    {
        pFont = pDC->SelectObject(&fFontName);
        ASSERT(pFont);
        pDC->SetTextColor(colorName);

        sz = GetSizeText(pDC, sclName, &fFontName, sclStyle.flName);

        x1 = xbs + int(k1*sclX*(sizeMainStrich.cy + shiftStrichText)) + sz.cx*(lr - 1);
        if(sclStyle.flName)
        {
            y1 = - sz.cy/2 + strichScaleMain[0];

            recClr = CRect(CPoint(x1, y1),	sz);

            szt = GetSizeText(pDC, sclName, &fFontName, sclStyle.flName);
            x2 = xbs + int(k1*sclX*(sizeMainStrich.cy + shiftStrichText)) + szt.cx*(lr - 1);

            pDC->TextOut(x2, y1 + (szt.cy + sz.cy)*int(sclStyle.flOrientFontN)/2, sclName);
        }

        pDC->SelectObject(pFont);
    }
    pDC->SelectObject(pns);
}

void CScale::DrawGrf(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
    if(!m_hWnd) return;
    CRect sclRec, sclRecS, rec, fldRec;

    m_fldGraph->GetClientRect(&fldRec);
    GetClientRect(&sclRec);

    if(pDC->IsPrinting()) 
    {
        if(!pWnd) return;
        ClientToScreen(&sclRec);
    }

    if(sclStyle.flOrient)
    {
        CalcScaleStrichY(&sclRec);
        DrawYScl(pDC, pWnd, sclX, sclY, prnShift);
    }
    else
    {
        CalcScaleStrichX(&sclRec);
        DrawXScl(pDC, pWnd, sclX, sclY, prnShift);
    }

    if(pDC->IsPrinting()) 
    {
        if(!pWnd) return;
        SetNormalFont();
    }
}

void CScale::OnPaint() 
{
    CPaintDC dc(this);
    DrawGrf(&dc);
}

void CScale::ChangeString() 
{
    return;
    int i, basaX, basaX1, basaY, basaY1;
    F stepMainStrich;
    CSize inflate;
    CRect sclRec, sclRecS;
    int iOrientHorz = (int)(!sclStyle.flOrient);
    int iOrientVert = (int)sclStyle.flOrient;

    GetClientRect(&sclRec);
    if(sclRec == 0) return;

	CClientDC *pDC = new CClientDC(this);
    sclRecS = sclRec;
    if(iOrientHorz)
    {
        sclRec.left += inflate.cx/2; 
        sclRec.right -= inflate.cx/2; 
    }
    else
    {
        sclRec.top += inflate.cy/2; 
        sclRec.bottom -= inflate.cy/2; 
    }

    int xleft = sclRec.left, xright = sclRec.right - 1, 
        ytop = sclRec.top, ybtm = sclRec.bottom - 1;
    int HRec = sclRec.Height();

    int iOrientInvert, iOrientInvertTxt, iOrientInvert1;
    if(!sclStyle.flOrientInvert) 
    {
        iOrientInvert = 1;
        iOrientInvertTxt = 1;
        iOrientInvert1 = 0;
        basaX = xright;
        basaX1 = xleft;
        basaY = ybtm;
        basaY1 = ytop;
    }
    else
    {
        iOrientInvert = -1;
        iOrientInvertTxt = 0;
        iOrientInvert1 = 1;
        basaX = xleft;
        basaX1 = xright;
        basaY = ytop;
        basaY1 = ybtm;
    }

    int iShiftStrichText = shiftStrichText + sizeMainStrich.cy;

    if(!sclStyle.flOrient) stepMainStrich = (xright - xleft)/(sclNstrich - 1.0f);
    else stepMainStrich = (ybtm - ytop)/(sclNstrich - 1.0f);

    if(!sclStyle.flColorFon) colorFon = RGB(255,255,255);
    CFont *pFont = pDC->SelectObject(&fFontMain);
    ASSERT(pFont);
    pDC->SetTextColor(colorText);
    pDC->SetBkColor(colorFon);

    CString clr("                 "), str;
    F df = (Fmax - Fmin)/(sclNstrich - 1.0f);

    CSize sz = pDC->GetTextExtent(clr);
    int shx, shy;
    shy = basaY + HRec*iOrientInvert1*iOrientVert - ((sizeMainStrich.cy + 1)*iOrientInvert + 
        sz.cy*iOrientInvertTxt)*iOrientHorz + (-sz.cy/2)*iOrientVert;
    for(i = 0; i < sclNstrich; i++)
    {
        sz = pDC->GetTextExtent(clr);

        shx = (int)((xleft + stepMainStrich*i)*iOrientHorz + 
            (basaX*iOrientVert - iShiftStrichText*iOrientInvert)*iOrientVert);
        pDC->TextOut(shx - sz.cx/2 - (sz.cx/2)*iOrientVert*iOrientInvertTxt - (sz.cx/2)*iOrientVert*(iOrientInvertTxt - 1),  
            (int)(shy - (stepMainStrich*i)*iOrientVert), clr);

        F ff = Fmin + i*df;
        F aff = fabs(ff);
        if((aff < 99999.0 && aff > 0.1) || aff == 0) str.Format("%.1f", ff);
        else str.Format("%.1e", ff);
        sz = pDC->GetTextExtent(str);
        pDC->TextOut(shx - sz.cx/2 - (sz.cx/2)*iOrientVert*iOrientInvertTxt - (sz.cx/2)*iOrientVert*(iOrientInvertTxt - 1),  
            (int)(shy - (stepMainStrich*i)*iOrientVert), str);

    }

    pDC->SelectObject(pFont);
	delete pDC;
}


void CScale::OnMouseMove(UINT nFlags, CPoint point) 
{
    CStatic::OnMouseMove(nFlags, point);
}

void CScale::RenameScale(CString sName)
{
    sclName = sName;
}

CDataGraph::CDataGraph()
{
    sName = "";
    sizeBuf = sizeData = 0;
    pData = NULL;
}

CDataGraph::~CDataGraph()
{
    if (pData)
        delete[] pData;
}

CDataGraph& CDataGraph::operator=(CDataGraph& pdata)
{
    sName		= pdata.sName;

    sizeBuf		= pdata.sizeBuf;
    sizeData	= pdata.sizeData;

    if(pData) delete pData;
    pData = (F*) new F[sizeBuf];

    if (!pData) TRACE0("Not enough memory.\n");
    else memcpy(pData, pdata.pData, sizeData);

    return *this;
}
IMPLEMENT_DYNCREATE(CFieldGraph, CStatic)

CFieldGraph::CFieldGraph()
{
	m_pDCMem = NULL;
	m_pBitmap = NULL;
	m_pbitmapOld = NULL;
	paint_lock = false;

	fldStyle.dwFlags = 0;

    typeGraph = 0;
    typeView = gline;
    pvReper = NULL;
    numRepers = 0;
    ptsReper = NULL;
    numRepersTS = 0;

    sizeFragm		= 0;
    startFragm = 0;

    ScaleFX	= 1;
    ScaleFY	= 1;

    sclNstrichX = 5;
    sclNstrichY = 5;
    sizeMainStrich = CSize(1,9);
    sizeSlaveStrich = CSize(1,5);

    colorScale			= RGB(255,0,0);
    colorMainStrich		= RGB(200,150,0);
    colorSlaveStrich	= RGB(100,150,0);
    colorGraph[0]			= RGB(0,0,0);
    colorGraph[1]			= RGB(255,0,0);
    colorGraph[2]			= RGB(116,10,236);
    colorGraph[3]			= RGB(10,154,236);
    colorGraph[4]			= RGB(14,215,113);
    colorGraph[5]			= RGB(190,255,0);
    colorGraph[6]			= RGB(190,0,255);
    colorGraph[7]			= RGB(0,190,255);
    colorGraph[8]			= RGB(0,255,190);
    colorGraph[9]			= RGB(190,190,190);
    colorSelectGraph	= RGB(150,150,255);
    colorFon			= RGB(204,236,255);
    colorGreed			= RGB(166,202,240);
    colorSelectFon		= RGB(0,255,255);
    colorAlert1			= RGB(0,255,255);
    colorAlert2			= RGB(200,200,0);
    colorAlert3			= RGB(200,200,200);
    colorAlert4			= RGB(255,0,0);
    colorReper			= RGB(255,0,0);
    colorTextReper		= RGB(0,0,255);
    colorTextGrf		= RGB(0,0,255);

    flagtrack = false;
    flagfragment = false;

    saveList = NULL;
    fieldRec = CRect(0,0,0,0);

    m_psclHor = m_psclVert = NULL;
    nPos = 0;

    LOGFONT logfontTxt = {
        -15, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    LOGFONT logfontTextGrf = {
        -24, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    lfntTextReper	= logfontTxt;
    lfntTextGrf	= logfontTextGrf;

    m_textGrf = "Глубина";
    flag_scroll = false;
    flag_rbtn_down = false;
}

CFieldGraph::~CFieldGraph()
{
    if(saveList) delete saveList;
    saveList = NULL;

    if (pvReper) delete[] pvReper;
    pvReper = NULL;

    if (ptsReper) delete[] ptsReper;
    ptsReper = NULL;

	if (m_pDCMem != NULL)
	{
		delete m_pDCMem;
		delete m_pBitmap;
	}
	else
	{
		ASSERT(m_pbitmapOld == NULL);
		ASSERT(m_pBitmap == NULL);
	}
}

CFieldGraph::CFieldGraph(UINT NstrichX, UINT NstrichY)
{
	m_pDCMem = NULL;
	m_pBitmap = NULL;
	m_pbitmapOld = NULL;
	paint_lock = false;

	fldStyle.dwFlags = 0;

    typeGraph = 0;
    typeView = gline;
    pvReper = NULL;
    numRepers = 0;
    ptsReper = NULL;
    numRepersTS = 0;

    sizeFragm	= 0;
    startFragm	= 0;

    ScaleFX	= 1;
    ScaleFY	= 1;

    sclNstrichX = NstrichX;
    sclNstrichY = NstrichY;
    sizeMainStrich = CSize(1,9);
    sizeSlaveStrich = CSize(1,5);

    colorScale			= RGB(255,0,0);
    colorMainStrich		= RGB(200,150,0);
    colorSlaveStrich	= RGB(100,150,0);
    colorGraph[0]			= RGB(0,0,0);
    colorGraph[1]			= RGB(255,0,0);
    colorGraph[2]			= RGB(116,10,236);
    colorGraph[3]			= RGB(10,154,236);
    colorGraph[4]			= RGB(14,215,113);
    colorGraph[5]			= RGB(190,255,0);
    colorGraph[6]			= RGB(190,0,255);
    colorGraph[7]			= RGB(0,190,255);
    colorGraph[8]			= RGB(0,255,190);
    colorGraph[9]			= RGB(190,190,190);
    colorSelectGraph	= RGB(150,150,255);
    colorFon			= RGB(204,236,255);
    colorGreed			= RGB(166,202,240);
    colorSelectFon		= RGB(0,255,255);
    colorAlert1			= RGB(0,255,255);
    colorAlert2			= RGB(200,200,0);
    colorAlert3			= RGB(200,200,200);
    colorAlert4			= RGB(255,0,0);
    colorReper			= RGB(255,0,0);
    colorTextReper		= RGB(0,0,255);
    colorTextGrf		= RGB(0,0,255);

    flagtrack = false;
    flagfragment = false;

    saveList = NULL;
    fieldRec = CRect(0,0,0,0);

    m_psclHor = m_psclVert = NULL;
    nPos = 0;

    LOGFONT logfontTxt = {
        -15, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    LOGFONT logfontTextGrf = {
        -24, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    lfntTextReper  = logfontTxt;
    lfntTextGrf	= logfontTextGrf;
    m_textGrf = "Глубина";
    flag_scroll = false;
    flag_rbtn_down = false;
}

CFieldGraph& CFieldGraph::operator=(CFieldGraph& fld)
{
	m_pDCMem = NULL;
	m_pBitmap = NULL;
	m_pbitmapOld = NULL;
	paint_lock = false;

	fldStyle.dwFlags = fld.fldStyle.dwFlags;
    typeView = fld.typeView;

    DATA = fld.DATA;
    pvReper = NULL;
    numRepers = 0;
    ptsReper = NULL;
    numRepersTS = 0;

    typeGraph = fld.typeGraph;

    sizeFragm		= fld.sizeFragm;
    startFragm		= fld.startFragm;

    ScaleFX	= fld.ScaleFX;
    ScaleFY	= fld.ScaleFY;
    dAlert1 = fld.dAlert1;
    dAlert2 = fld.dAlert2;
    dAlert3 = fld.dAlert3;
    dAlert4 = fld.dAlert4;

    sclNstrichX		= fld.sclNstrichX;
    sclNstrichY		= fld.sclNstrichY;
    sizeMainStrich	= fld.sizeMainStrich;
    sizeSlaveStrich = fld.sizeSlaveStrich;

    colorScale			= fld.colorScale;
    colorMainStrich		= fld.colorMainStrich;
    colorSlaveStrich	= fld.colorSlaveStrich;
    colorGraph[0]			= fld.colorGraph[0];
    colorGraph[1]			= fld.colorGraph[1];
    colorGraph[2]			= fld.colorGraph[2];
    colorGraph[3]			= fld.colorGraph[3];
    colorGraph[4]			= fld.colorGraph[4];
    colorGraph[5]			= fld.colorGraph[5];
    colorGraph[6]			= fld.colorGraph[6];
    colorGraph[7]			= fld.colorGraph[7];
    colorGraph[8]			= fld.colorGraph[8];
    colorGraph[9]			= fld.colorGraph[9];

    colorSelectGraph	= fld.colorSelectGraph;
    colorFon			= fld.colorFon;
    colorGreed			= fld.colorGreed;
    colorSelectFon		= fld.colorSelectFon;
    colorAlert1			= fld.colorAlert1;
    colorAlert2			= fld.colorAlert2;
    colorAlert3			= fld.colorAlert3;
    colorAlert4			= fld.colorAlert4;
    colorReper			= fld.colorReper;
    colorTextReper		= fld.colorTextReper;
    colorTextGrf		= fld.colorTextGrf;

    flagtrack = flagtrack;
    flagfragment = flagfragment;

    saveList = NULL;

    fieldRec = fieldRec;

    m_psclHor = m_psclHor;
    m_psclVert = m_psclVert;

    nPos = nPos;

    lfntTextReper = fld.lfntTextReper;
    lfntTextGrf = fld.lfntTextGrf;
    flag_scroll = fld.flag_scroll;
    flag_rbtn_down = fld.flag_rbtn_down;

    return *this;
}

BEGIN_MESSAGE_MAP(CFieldGraph, CStatic)
    //{{AFX_MSG_MAP(CFieldGraph)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_MOUSEMOVE()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_MOUSEWHEEL()
	ON_MESSAGE(PM_EDIT_MARKER, OnEditMarker)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
int inline CFieldGraph::GetSrollXPos(int maxXScrollSize)
{
	F Fmax, Fmin, FmaxS, FminS;
	if(DATA.GetMinMax(-1, Fmin, Fmax) == NULL) return 0;
	DATA.GetMinMaxDiapazon(-1, FminS, FmaxS);

	F df = ((FmaxS - FminS) - (Fmax - Fmin));
	if(df == 0) return 0;
	F dd = (maxXScrollSize*(Fmin - FminS))/df;
	if(int(dd) > maxXScrollSize) return maxXScrollSize;

	if(dd - int(dd) > 0.5) return int(dd) + 1;
	else return int(dd);
}
int inline CFieldGraph::GetSrollYPos(int maxYScrollSize)
{
    F Fmax, Fmin, FmaxS, FminS;
    if(DATA.GetMinMax(0, Fmin, Fmax) == NULL) return 0;
    if(DATA.GetMinMaxDiapazon(0, FminS, FmaxS) == 0) return 0;

    F df = ((FmaxS - FminS) - (Fmax - Fmin));
	if(df == 0) return 0;
    F dd = (maxYScrollSize*(Fmin - FminS))/df;
	if(int(dd) > maxYScrollSize) return maxYScrollSize;

    if(dd - int(dd) > 0.5) return int(dd) + 1;
    else return int(dd);
}

int inline CFieldGraph::GetSizeXScroll(int maxXScrollSize)
{
    VFRingBuf<F>* pMain = DATA.GetBuf(-1);
    if(pMain == NULL) return 0;
    F max, min;
    if(DATA.GetMinMax(-1, min, max) == NULL) return 0;
    int size_data;
    if((size_data = pMain->GetSizeData()) == 0) return 0;
    F deltaX = max - min; 
    max = pMain->GetLastData();
    min = pMain->GetFirstData();
    F maxX = max - min;

    if(maxX == 0 || maxX < deltaX) return maxXScrollSize;
    return (int)abs(deltaX*maxXScrollSize/maxX);
}
int inline CFieldGraph::GetSizeYScroll(int maxYScrollSize)
{
	F max, min;
	DATA.GetMinMax(0, min, max);
	if(max == min) return maxYScrollSize;
	else return (int)abs((max - min)*maxYScrollSize/(max - min));
}
BOOL inline CFieldGraph::SetSizeVievX()
{
    int size_data = 0;
    if(DATA.GetSizeData(-1, size_data) == 0 || size_data == 0) return 0;

    F maxX = DATA.GetLast(-1);

    F minViewXold, maxViewXold;
    DATA.GetMinMax(-1, minViewXold, maxViewXold);

    if(maxX < maxViewXold) return false;
    return true;
}

void inline CFieldGraph::GetClientRectMy(CRect *rec) 
{
    if(!m_hWnd) return;

    GetClientRect(rec);
    if(fldStyle.bitFlags.flOrient) *rec = CRect(rec->top, rec->left, rec->bottom, rec->right);
}

void CFieldGraph::SetColorDiagram(int numDiagram, COLORREF clr, BOOL flag_color)
{
    if(numDiagram < 0 || numDiagram > MAX_BUFFERS) return;
    if(DATA.GetBuf(numDiagram) != NULL) DATA.GetBuf(numDiagram)->SetBaseColor(clr, flag_color);
}

int CFieldGraph::GetThicknessDiagram(int numDiagram)
{
    if(numDiagram < 0 || numDiagram >= MAX_BUFFERS) return 0;
    return DATA.GetThicknessDiagram(numDiagram);
}

void CFieldGraph::SetThicknessDiagram(int numDiagram, int thick)
{
    if(numDiagram < 0 || numDiagram >= MAX_BUFFERS) return;
    if(DATA.GetBuf(numDiagram) != NULL) DATA.GetBuf(numDiagram)->ReplacePenThick(thick);
}

LOGFONT CFieldGraph::SetFontReper(LOGFONT& lf)
{
    LOGFONT slf = lfntTextReper;
    lfntTextReper = lf;

    if(fFontTextReper.m_hObject) fFontTextReper.DeleteObject();
    fFontTextReper.CreateFontIndirect(&lfntTextReper);
    return slf;
}

LOGFONT CFieldGraph::SetFontTextGrf(LOGFONT& lf)
{
    LOGFONT slf = lfntTextGrf;
    lfntTextGrf = lf;

    if(fFontTextGrf.m_hObject) fFontTextGrf.DeleteObject();
    fFontTextGrf.CreateFontIndirect(&lfntTextGrf);
    return slf;
}

COLORREF CFieldGraph::SetColorFon(COLORREF color)
{
    COLORREF clr = colorFon;
    colorFon = color;
    return clr;
}

COLORREF CFieldGraph::SetColorGreed(COLORREF color)
{
    COLORREF clr = colorGreed;
    pnGreed.DeleteObject();
    colorGreed = color;
    pnGreed.CreatePen(PS_DASHDOTDOT, 1, colorGreed);
    return clr;
}

void CFieldGraph::SetStyleGreed(int nPenStyle, int nWidth)
{
    pnGreed.DeleteObject();
    pnGreed.CreatePen(nPenStyle, nWidth, colorGreed);
}

COLORREF CFieldGraph::SetColorGraph(COLORREF color)
{
    VFRingBuf<F>* pBuff = DATA.GetBuf(0);
    if(pBuff) return pBuff->ReplacePenColor(color);
    return 0;
}

COLORREF CFieldGraph::SetColorGraphEX(UINT NumGrf, COLORREF color)
{
    VFRingBuf<F>* pBuff = DATA.GetBuf(NumGrf);
    if(pBuff) return pBuff->ReplacePenColor(color);
    return 0;
}

void CFieldGraph::SetNumStrich(int nStrichX, int nStrichY, CRect* p_rec)
{
    sclNstrichX = nStrichX;
    sclNstrichY = nStrichY;
    m_psclHor->SetNumStich(nStrichX, p_rec);
    m_psclVert->SetNumStich(nStrichY, p_rec);
}

void CFieldGraph::SetNameGrf(int numDiagram, CString sname)
{
    if(numDiagram >= MAX_BUFFERS) return;
    if(numDiagram < 0) DATA.SetName(-1, sname);
    else DATA.SetName(numDiagram, sname);
}

COLORREF CFieldGraph::SetColorReper(COLORREF color)
{
    COLORREF clr = colorReper;
    colorReper = color;

    pnReper.DeleteObject();
    pnReper.CreatePen(PS_SOLID, 1, colorReper);
    return clr;
}

COLORREF CFieldGraph::SetColorTextReper(COLORREF color)
{
    COLORREF clr = colorTextReper;
    colorTextReper = color;
    return clr;
}

COLORREF CFieldGraph::SetColorTextGrf(COLORREF color)
{
    COLORREF clr = colorTextGrf;
    colorTextGrf = color;
    return clr;
}

void CFieldGraph::AddReper(F XR, CString TxtR)
{
    if(!m_hWnd) return;

    if(numRepers > MAX_NUM_REPERS -1) return;

    pvReper[numRepers].XR = XR;
    pvReper[numRepers].TxtR = TxtR;
    numRepers++;

    Invalidate(true);
    return;
}


void CFieldGraph::AddReper(F YR, F XR, CString TxtR)
{
    if(!m_hWnd) return;

    if(numRepersTS > MAX_NUM_REPERS -1) return;

    F maxF;
    if((maxF = DATA.GetMaxF(-1)) == 0) return;

    ptsReper[numRepersTS].YR	= YR/maxF;
    ptsReper[numRepersTS].XR	= XR;
    ptsReper[numRepersTS].TxtR	= TxtR;
    numRepersTS++;

    Invalidate(true);
    return;
}

void CFieldGraph::ClearRepers()
{
    if(!m_hWnd) return;

    numRepersTS = 0;
    numRepers = 0;;
    Invalidate(true);
    return;
}

int CFieldGraph::SetTypeGraph(int gtype)
{
    int gt = typeGraph;
    typeGraph = gtype;
    return gt;
}

void CFieldGraph::DrawLineScale(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
	if(!fldStyle.bitFlags.flScale) return;
	if(!m_hWnd || DATA.GetNumBuffers() == 0) return;

	VFRingBuf<F>* parY;
	F Fmin, Fmax;

	VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS];
	UINT num_buf = DATA.GetTBLPtrBuffers(tblPtrBuffers);
	for(int i = 0; i < (int)DATA.GetNumBuffers(); i++)
	{
		if(num_buf && tblPtrBuffers[i]->GetSizeData() == 0) continue;
		int nn = tblPtrBuffers[i]->num_self;
		parY = DATA.GetBuf(nn); 
		if(parY == NULL) continue;

		parY->GetMinMax(Fmin, Fmax);
		if(Fmin == Fmax) continue;
		DrawLineScaleA(pDC, pWnd, sclX, sclY, prnShift, Fmin, Fmax);
	}
}

void CFieldGraph::DrawLineScaleA(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift, F Fmin, F Fmax)
{
    int i, xleft, xright, ytop, ybtm, beg, end;
    CRect rec;
    GetClientRect(&rec);
	ClientToScreen(&rec);
    RotRect90(rec, fldStyle.bitFlags.flOrient);

    xleft = rec.left;
    xright = rec.right;
    ytop = rec.top;
    ybtm = rec.bottom;

    int midlline = (int)(ytop + (ybtm - ytop)*Fmax/(Fmax - Fmin));// - 2*(!fldStyle.bitFlags.flOrient));
    F stepMainStrich = (xright - xleft)/(sclNstrichX - 1.0f);

    CPen *pns = pDC->SelectObject(&pnScaleLine);
    ASSERT(pns);
    DrawLine(pDC, xleft, midlline, xright - 2, midlline, fldStyle.bitFlags.flOrient);

    beg = midlline - sizeMainStrich.cy/2 - 1;
    end = midlline + sizeMainStrich.cy/2;
    if(sclNstrichX > 2)
    {
        pDC->SelectObject(&pnMainStrich);
        for(i = 1; i < sclNstrichX - 1; i++)
        {
            int xx = m_psclHor->strichScaleMain[i] + fldStyle.bitFlags.flOrient;//xleft + stepMainStrichX*i;
            DrawLine(pDC, xx, end, xx, beg, fldStyle.bitFlags.flOrient);
        }

		beg = midlline - sizeSlaveStrich.cy/2 - 1;
		end = midlline + sizeSlaveStrich.cy/2;
        pDC->SelectObject(&pnSlaveStrich);
		int dx = (m_psclHor->strichScaleMain[1] - m_psclHor->strichScaleMain[0])/2;
        for(i = 0; i < sclNstrichX - 1; i++)
        {
			int xx = m_psclHor->strichScaleMain[i] + fldStyle.bitFlags.flOrient;//xleft + stepMainStrichX*i;
			DrawLine(pDC, xx + dx, end, xx + dx, beg, fldStyle.bitFlags.flOrient);
        }
	}
    pDC->SelectObject(pns);
}

void CFieldGraph::DrawGreed(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
    if(!m_hWnd) return;

    if(!fldStyle.bitFlags.flGreed) return;
    BOOL flOrient = fldStyle.bitFlags.flOrient;

    int i, xleft, xright, ytop, ybtm, width, height;
    CRect rec, rec2;
    GetClientRect(&rec);

    int delta = 0;

    CSize szShift(0,0);
	CPen *pns = pDC->SelectObject(&pnGreed);
	ASSERT(pns);

    if(pDC->IsPrinting()) 
    {
        if(!pWnd) 
		{
			pDC->SelectObject(pns);
			return;
		}
        ClientToScreen(&rec);
        delta = 5;
		CPen pnGr(PS_SOLID, int(sclY), RGB(0,0,0));
		pDC->SelectObject(&pnGr);
    }

    xleft = rec.left;
    xright = rec.right;
    ytop = rec.top;
    ybtm = rec.bottom;
    width = rec.Width();
    height = rec.Height();

    //...............................................................................
	int xx, yy;
	if(!flOrient)
	{
		if(sclNstrichX > 1)
		{
			for(i = 0; i < sclNstrichX; i++)
			{
				xx = m_psclHor->strichScaleMain[i];

				DrawLine(pDC, xx, ytop, xx, ybtm-1, fldStyle.bitFlags.flOrient);
			}
		}
		if(sclNstrichY > 1)
		{
			for(i = 0; i < sclNstrichY; i++)
			{
				yy = m_psclVert->strichScaleMain[i];
				DrawLine(pDC, xleft, yy, xright-1, yy, fldStyle.bitFlags.flOrient);
			}
		}
	}
	else
	{
		if(sclNstrichX > 1)
		{
			for(i = 0; i < sclNstrichX; i++)
			{
				yy = m_psclHor->strichScaleMain[i];
				pDC->MoveTo(xleft, yy);
				pDC->LineTo(xright-1, yy);
			}
		}
		if(sclNstrichY > 1)
		{
			for(i = 0; i < sclNstrichY; i++)
			{
				xx = m_psclVert->strichScaleMain[i];
				pDC->MoveTo(xx, ytop);
				pDC->LineTo(xx, ybtm-1);
			}
		}
	}
    pDC->SelectObject(pns);
}

void CFieldGraph::DrawLinesAlert(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
}

void CFieldGraph::UpdateReper()
{
	CClientDC *pDC = new CClientDC(this);
    CRect rec = fieldRec;

    DrawRepers(pDC, &fieldRec, true);
	delete pDC;
}
void CFieldGraph::SynchronizeGraph(double min_x, double max_x)
{
    if(m_hWnd == NULL) return;

    if(flag_scroll) 
    {
        DATA.SetMinMaxFullDiapazon(-1);
        return;	
    }

    m_pBuffer = DATA.GetBuf(-1);
    if(m_pBuffer == NULL) return;
    if(m_pBuffer->GetSizeData() < 2) return;

    DATA.SetMinMax(-1, min_x, max_x);
    m_psclHor->SetMinMax(min_x, max_x);
    DATA.ReplacePosMarker0(0.1);
    m_psclHor->Invalidate(true);
    Invalidate(false);
    UpdateWindow();
}

BOOL CFieldGraph::DrawMarker(CDC* pDC, TMarker<F> *pMarker, CFont& fnt)
{
	FRingBuf<F> *p_curRBufX, *p_curRBufY;
	COLORREF clr;
	CPoint p_point;
	F minX, maxX;

	int nn = pMarker->base_position;

	p_curRBufX = DATA.GetCurRBuf(-1);
	if(!p_curRBufX) return false;

	m_Xdata = p_curRBufX->GetElement(nn);

	F qq = pMarker->value;

	clr = pMarker->color;
	CPen pn(PS_SOLID, 1, clr);
	pDC->SelectObject(&pn);

	p_curRBufY = m_pBuffer->GetCurRBuf();
	if(!p_curRBufY) return false;
	m_Ydata = p_curRBufY->GetElement(nn);

	GetXY(p_point);
	xx = p_point.x;
	yy = p_point.y;

	CRect poitRect(xx - 3, yy - 3, xx + 3, yy + 3);
	if(pMarker->flag_visible)
	{
		pDC->FillSolidRect(&poitRect, clr);	
		CString str;
		str.Format("%4.2f", m_Xdata);
		pDC->SetBkColor(colorFon);

		if(!pMarker->flag_select) pDC->SetTextColor(pMarker->colorName);
		else pDC->SetTextColor(RGB(255,0,0));

		CSize sz = CScale::GetSizeText(pDC, str, &fnt, true);
		pDC->TextOut(xx + pMarker->shift_text.cx, yy + pMarker->shift_text.cy, str);
	}
	return true;
}

void CFieldGraph::DrawRepers(CDC* pDC, CRect rec, BOOL flg_redraw)
{
    if(!PrepareDataXForDraw()) return;

	int max_buffers = (int)DATA.GetNumBuffers();
	if(max_buffers == 0) return;

	if(!DATA.GetCurRBuf(-1)) return;
    int max_rp = DATA.GetNumMarkers(-1);
    TMarker<F> **tblPtrReper = new TMarker<F>*[max_rp];
    numRepers = DATA.GetTBLPtrMarkers(-1, tblPtrReper);

	int xx, yy;
	F xr;
	CRect poitRect;

	F minY, maxY;
	F minX, maxX;

	CPen *pns;
	F stepX;
	LOGFONT lf;
	CFont fnt, *pFont;
	COLORREF clr;
	int nn;
	int xx_s, yy_s;
	CSize sz;

	DATA.GetMinMax(-1, minX, maxX);
	DATA.GetMinMax(0, minY, maxY);

    RotRect90(rec, fldStyle.bitFlags.flOrient);

    if(numRepers == 0) 
    {
        delete tblPtrReper;
        goto MR2;
    }

	if(DATA.GetMaxF(-1) == 0) 
    {
        delete tblPtrReper;
		goto MR2;
    }

    if(numRepers > 0 && DATA.Get_flg_view_reper(-1))
    {
		pns = pDC->SelectObject(&pnReper);
		stepX = (rec.right - rec.left - 2)/(maxX - minX);

        lf = lfntTextReper;
        lf.lfEscapement = int(!fldStyle.bitFlags.flOrient) * (-900);
        //		lf.lfOrientation = 900;

        fnt.CreateFontIndirect(&lf);
        pFont = pDC->SelectObject(&fnt);
        ASSERT(pFont);

        xx_s = 0, yy_s = 0;
        for(int i = 0; i < numRepers; i++)
        {
            xr = tblPtrReper[i]->value;
            if(xr < minX || xr > maxX || !tblPtrReper[i]->flag_visible) continue;

            clr = tblPtrReper[i]->color;
            CPen pn(PS_SOLID, 1, clr);
            pDC->SelectObject(&pn);

            sz = CScale::GetSizeText(pDC, tblPtrReper[i]->name, &fnt, true);

            xx = (int)(rec.left + (xr - minX)*stepX);
            yy = rec.top + 2;
            RotCoord90(xx, yy, fldStyle.bitFlags.flOrient);
            pDC->MoveTo(xx, yy);

            xx = (int)(rec.left + (xr - minX)*stepX);
            yy = rec.bottom - 2;
            RotCoord90(xx, yy, fldStyle.bitFlags.flOrient);
            pDC->LineTo(xx, yy); 

            pDC->SetBkColor(colorFon);
            pDC->SetTextColor(tblPtrReper[i]->colorName);

            if(abs(yy - yy_s) > sz.cy)
            {
                yy_s = yy;
                if(tblPtrReper[i]->typeCh != 0)
                {
                    pDC->TextOut(xx - sz.cx, rec.top + sz.cy + 20, tblPtrReper[i]->name);
                }
                else 
                {
                    pDC->TextOut(xx - sz.cx, yy - (sz.cy + 2), tblPtrReper[i]->name);
                }
            }
        }
		pDC->SelectObject(pns);
        pDC->SelectObject(pFont);
        fnt.DeleteObject();
    }
    delete tblPtrReper;
MR2:;
	TMarker<F> **tblPtrEditPoint;

    TMarker<F> *pMarker = NULL;
    int numEditPoint;
    m_rec = fieldRec;
    CPoint p_point;

    VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS];
    DATA.GetTBLPtrBuffers(tblPtrBuffers);

	FRingBuf<F> *p_curRBuf;

	pns = pDC->SelectObject(&pnReper);

	lf = lfntTextReper;
	fnt.CreateFontIndirect(&lf);
	pFont = pDC->SelectObject(&fnt);
	ASSERT(pFont);

    for(int nb = 0; nb < max_buffers; nb++)
    {
        m_pBuffer = tblPtrBuffers[nb];
        if(m_pBuffer->GetSizeData() == 0 || !m_pBuffer->Get_flg_view_data()) continue;

        if(!PrepareDataYForDraw()) return;

        max_rp = m_pBuffer->GetNumMarkers();
		if(max_rp == 0) continue;
        tblPtrEditPoint = new TMarker<F>*[max_rp];
        numEditPoint = m_pBuffer->GetTBLPtrMarkers(tblPtrEditPoint);

        if(numEditPoint > 0)
        {
            for(int i = 0; i < numEditPoint; i++)
            {
                if((pMarker = tblPtrEditPoint[i]) == NULL) continue;
				if(!DrawMarker(pDC, pMarker, fnt)) continue;
                if(flg_redraw) 
					Invalidate(true);
            }
        }
        delete tblPtrEditPoint;
    }
    pDC->SelectObject(pns);
	pDC->SelectObject(pFont);
	fnt.DeleteObject();
}


BOOL CFieldGraph::TestSize(F stepX)
{
    int size_data = 0;
    if(DATA.GetSizeData(-1, size_data) == NULL || 
        size_data < 2 ||
        DATA.GetDiffPosUMarker(-1,2,1) < 2) return false;

    int ind = DATA.GetPosUMarker(-1, 1);
    if(ind < 0) return false;
    F val1, val2;
    DATA.GetElement(-1, ind + 1, val2);
    DATA.GetElement(-1, ind, val1);
    int dx = (int)abs((val2 - val1)*stepX);
    if(dx >= 1) return true;
    return false;
}
int num_iter = 0;

void CFieldGraph::RedrawGraph()
{
	CClientDC dc(this);
	DrawGraph(&dc);
}

void CFieldGraph::DrawGraph(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
    int num_buf = (int)DATA.GetNumBuffers();
    if(!m_hWnd || (DATA.GetBuf(-1) == NULL) || num_buf == 0) return;
    pGraph->time_graph = GetTickCount();

    prnShiftXY = prnShift;
    int size_data = 0;

    if(DATA.GetSizeData(-1, size_data) == NULL || 
        size_data < 2 ||
        DATA.GetDiffPosUMarker(-1, 2,1) <= 0)
        return;

    F szx;
    if((szx = DATA.GetDiffMaxMin(-1)) == 0)
        return;

    m_flOrient = fldStyle.bitFlags.flOrient;
    m_flSignData = fldStyle.bitFlags.flSignData;
    m_rec = fieldRec;

    if(pDC->IsPrinting()) 
    {
        if(!pWnd) return;

        ClientToScreen(&m_rec);
    }

    RotRectCCW90(m_rec, m_flOrient);
    if(typeView == gpoint1 || typeView == gline1) testS = true;
    //...............................................................................

    CRect poitRect;
    VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS];
    DATA.GetTBLPtrBuffers(tblPtrBuffers);

    typeGraphView TT = typeView;

    if(!PrepareDataXForDraw()) return;
	m_pDCC = pDC;

	FRingBuf<F>* cur_buf;
    num_iter = 0;
	POINT p_iter;
    for(int nb = 0; nb < num_buf; nb++)
    {
        m_pBuffer = tblPtrBuffers[nb];
		cur_buf = m_pBuffer->GetCurRBuf();
		if(cur_buf == NULL) continue;
        if(m_pBuffer->GetSizeData() <= 0 || !m_pBuffer->Get_flg_view_data()) goto EN;

		WaitForSingleObject(cur_buf->hMutex, INFINITE);
		UINT num_grf = m_pBuffer->num_self;
        p_iter = DATA.SetIterPos(m_pBuffer, fldStyle.bitFlags.flColorFromMark);
        if(p_iter.x < 0) goto EN;

        int n_frg = m_pBuffer->Get_nn_iter_frag();
        if(n_frg == 0) goto EN;

        if(!PrepareDataYForDraw()) goto EN;

        UINT pen_thick;
        if(pDC->IsPrinting())
        {
            pen_thick = m_pBuffer->GetPenThick();
            m_pBuffer->ReplacePenThick((int)(pen_thick*sclX));
        }

		CPen *pns = pDC->SelectObject(&m_pBuffer->pnGrf);
		ASSERT(pns);

		for(int j = 0; j < n_frg; j++)
        {
            pDC->SelectObject(&m_pBuffer->pnGrf);
            p_iter = DATA.GetNext0(m_Ydata, m_Xdata);
            
            MoveToA();
            DrawPointB();
            if(p_iter.y < 0) continue;

            int sz_frag = m_pBuffer->GetSizeIterFrag();
            if(sz_frag == 0) continue;

            for(int i = 1; i < sz_frag; i++)  //why 1
            {
                p_iter = DATA.GetNext0(m_Ydata, m_Xdata);

                if(p_iter.y >= 0)
                {
                    if(typeView == gpoint) DrawPointA();
                    else if(typeView == gline) DrawLineA();
                }
                if(p_iter.y < 0) 
                {
                    if(typeView == gpoint) DrawPointA();
                    else if(typeView == gline) 
                    {
                        DrawLineB();
                        DrawPointB();
                    }

                    DATA.SetCurColor(fldStyle.bitFlags.flColorFromMark);
                    break;
                }
            }
        }
        if(pDC->IsPrinting()) m_pBuffer->ReplacePenThick(pen_thick);
        pDC->SelectObject(pns);
EN:		ReleaseMutex(cur_buf->hMutex);
    }
    pGraph->time_graph = GetTickCount() - ((CGraph*)pGraph)->time_graph;
    DrawRepers(pDC, fieldRec);
    prnShiftXY = NULL;
}
BOOL inline CFieldGraph::PrepareDataXForDraw()
{
    BufferExtendedParametrs<F> &parX = *(DATA.GetBuf(-1)); 
    F szx = parX.GetDiffMaxMin();
    if(szx == 0) return false;
    m_stepX = m_rec.Width()/szx;  
    m_FminX = parX.Fmin;
    return true;
}
BOOL inline CFieldGraph::PrepareDataYForDraw()
{
    BufferExtendedParametrs<F> &parY = *m_pBuffer; 
    F szy = parY.GetDiffMaxMin();
    if(szy == 0) return false;
    m_kof   = m_rec.Height()/szy;
    m_FminY = parY.Fmin;
    m_FmaxY = parY.Fmax;
    m_maxF  = parY.maxF;
    return true;
}
void inline CFieldGraph::GetXY(POINT& p)
{
    p.x = (int)((m_Xdata - m_FminX)*m_stepX);
    p.y = m_rec.Height() - (int)((m_flSignData|m_flOrient) ? ((m_FmaxY - m_Ydata*m_maxF)*m_kof) : ((m_Ydata*m_maxF - m_FminY)*m_kof));
}
void inline CFieldGraph::BackConversionXY(POINT& p)
{
    SwapData(p.x, p.y, m_flOrient);
    if(prnShiftXY != NULL) 
    {
        if(!m_flOrient) {p.x += m_rec.left; p.y +=  m_rec.top;}
        else {p.x += m_rec.top; p.y +=  m_rec.left;}
    }
}
void inline CFieldGraph::BackConversionXY(int& x, int& y)
{
    SwapData(x, y, m_flOrient);
    if(prnShiftXY != NULL) 
    {
        if(!m_flOrient) {x += m_rec.left; y +=  m_rec.top;}
        else {x += m_rec.top; y +=  m_rec.left;}
    }
}
POINT inline CFieldGraph::MoveToA()
{
    POINT pXY, p;
    GetXY(pXY);
    p = pXY;
    x_save = pXY.x; 
    y_save = pXY.y;
    i_min = i_max = y_save;
    i_count = 0;

    BackConversionXY(p);
    m_pDCC->MoveTo(p);
    return pXY;
}
void inline CFieldGraph::DrawPointA()
{
    POINT pXY;
    GetXY(pXY);
    BackConversionXY(pXY);
    int xx = pXY.x, yy = pXY.y;
    if(testS && fldStyle.bitFlags.flTypePoint) 
    {
        CRect poitRect(xx - 2, yy - 2, xx + 2, yy + 2);
        m_pDCC->FillSolidRect(&poitRect, m_pBuffer->colorDraw);			
    }
    else m_pDCC->SetPixelV(xx, yy, m_pBuffer->colorDraw);
}
void inline CFieldGraph::DrawPointB()
{
    POINT pXY;
    GetXY(pXY);
    BackConversionXY(pXY);
    int xx = pXY.x, yy = pXY.y;
    if(fldStyle.bitFlags.flTypePoint) 
    {
        CRect poitRect(xx - 2, yy - 2, xx + 2, yy + 2);
        m_pDCC->FillSolidRect(&poitRect, m_pBuffer->colorDraw);			
    }
    else m_pDCC->SetPixelV(xx, yy, m_pBuffer->colorDraw);
}
void inline CFieldGraph::DrawLoosePoint()
{
    xx0 = x_save, yy0 = y_save;
    BackConversionXY(xx, yy);
    BackConversionXY(xx0, yy0);

    xx1 = xx2 = x_save;
    BackConversionXY(xx1, i_min);
    i_max += 1;    
    BackConversionXY(xx2, i_max);

    m_pDCC->MoveTo(xx0, yy0);
    m_pDCC->LineTo(xx, yy);
    m_pDCC->MoveTo(xx1, i_min);
    m_pDCC->LineTo(xx2, i_max);
}
void inline CFieldGraph::DrawLineA()
{
    POINT pXY;
    GetXY(pXY);
    xx = pXY.x;
    yy = pXY.y;
    if(xx <= x_save)
    {
        if(yy > i_max) i_max = yy;
        else if(yy < i_min) i_min = yy;
        y_save = yy;
        i_count++; 
    }
    else 
    {
        if(i_count == 0) 
        {
            xx0 = x_save, yy0 = y_save;
            BackConversionXY(xx, yy);
            BackConversionXY(xx0, yy0);

			if(!pGraph->GetBitStyle(FLD_LINE_JAGGIES))
			{
				m_pDCC->MoveTo(xx0, yy0);
				m_pDCC->LineTo(xx, yy);
			}
			else
			{
				m_pDCC->MoveTo(xx0, yy0);
				m_pDCC->LineTo(xx, yy0);
				m_pDCC->LineTo(xx, yy);
			}
            if(fldStyle.bitFlags.flTypePoint)
            {
                CRect poitRect(xx - 2, yy - 2, xx + 2, yy + 2);
                m_pDCC->FillSolidRect(&poitRect, m_pBuffer->colorDraw);			
            }
        }
        else DrawLoosePoint();

        i_min = i_max = pXY.y;
        x_save = pXY.x;
        y_save = pXY.y;
        i_count = 0;
    }
}
POINT CFieldGraph::DrawLineB()
{
    POINT pXY;
    GetXY(pXY);
    BackConversionXY(pXY);
	if(!pGraph->GetBitStyle(FLD_LINE_JAGGIES)) m_pDCC->LineTo(pXY); 
	else
	{
		BackConversionXY(x_save, y_save);
		m_pDCC->LineTo(x_save, pXY.y);
		m_pDCC->LineTo(pXY);
	}
    return pXY;
}
//----------------------------------------------------------------------------

void CFieldGraph::Pinok(int sz_pinok)
{
}

BOOL CFieldGraph::DrawLastData(int num_grf, int sz_data)
{
    if(pGraph->loc_data_for_print == 1) 
        return false;
    else if(pGraph->loc_data_for_print > 1) 
    {
        pGraph->loc_data_for_print = 0;
        Invalidate(false);
        UpdateWindow();
    }

    if(sz_data > 1) {DrawLastDataN(num_grf, sz_data); return false;}
    if(m_hWnd == NULL) return false;

    if(flag_scroll) 
    {
        DATA.SetMinMaxFullDiapazon(-1);
        return false;	
    }

    m_rec = fieldRec;
    RotRectCCW90(m_rec, m_flOrient);
    testS = false;

    DATA.GetBuf(-1)->SetPosUMarker23ToEndBuf();
	CClientDC dc(this);
	m_pDCC = &dc;

    if(DATA.ReplacePosMarker(0.1)) 
    {
        F minX, maxX;
        DATA.GetMinMax(-1, minX, maxX);
        m_psclHor->SetMinMax(minX, maxX);
        m_psclHor->Invalidate(true);
		Invalidate(false);
        UpdateWindow();
        CView* pView = (CView*)(GetParent()->GetParent());
        if(pView != NULL)
            pView->PostMessage(PM_SYNCHRONIZE_GRAPH, pGraph->num_self, 0);
		return true;
    }
    
    m_pBuffer = DATA.GetBuf(num_grf);
    if(m_pBuffer == NULL) return false;
    if(m_pBuffer->GetSizeData() < 2) return false;

	if(paint_lock) return false;
    m_flOrient = fldStyle.bitFlags.flOrient;
    m_flSignData = fldStyle.bitFlags.flSignData;

    CPen *pns;
    TFragment<F>* cur_frag = m_pBuffer->GetCurFrag();
    if(cur_frag == NULL) return false;
    CPen pn(PS_SOLID, m_pBuffer->thickness, cur_frag->color);

	if(!PrepareDataXForDraw()) return false;
	if(!PrepareDataYForDraw()) return false;
    if(fldStyle.bitFlags.flColorFromMark) 
        pns = m_pDCC->SelectObject(&pn);
    else 
        pns = m_pDCC->SelectObject(&(m_pBuffer->pnGrf));

    ASSERT(pns);

    POINT pXY;
    if(typeView == gpoint) 
    {
        if(DATA.GetLastDataPair(m_pBuffer, m_Ydata, m_Xdata) != 0)  
            DrawPointA();
    }
    else if(typeView == gline)
    {
        if(DATA.GetPrevLastDataPair(m_pBuffer, m_Ydata, m_Xdata) >= 0) 
        {
            pXY = MoveToA();
            if(DATA.GetLastDataPair(m_pBuffer, m_Ydata, m_Xdata) >= 0)
                pXY = DrawLineB();
        }
    }
    m_pDCC->SelectObject(pns);

    if(num_grf == 0)
    {
        dgParam.fValue	= m_Xdata;
        dgParam.time	= m_Ydata;
        dgParam.bflData = fldStyle.bitFlags.flViewTime;
        ::SendMessage(hWndGraph, PM_OUTDIGITGRAPH, DATA.GetSizeData(-1), (LPARAM)&dgParam);
    }
    return false;
}

void CFieldGraph::DrawLastDataN(int num_grf, int sz_data)
{
}

void CFieldGraph::AddPoint(UINT num_grf, F Yvalue, BOOL flg_scl)
{
	if(m_pDCMem == NULL) return;
    if(m_hWnd == NULL) return;
    m_rec = fieldRec;
    RotRect90(m_rec, fldStyle.bitFlags.flOrient);
    testS = false;

    m_pBuffer = DATA.AddToBuf(num_grf, 1, &Yvalue, 0);
    if(m_pBuffer == NULL) return;
    if(m_pBuffer->GetSizeData() < 2) return;

    DATA.SetPosUMarker(-1, 1, 0);
    DATA.SetPosUMarker23ToEndBuf(-1);
	CClientDC dc(this);
	m_pDCC = &dc;

    CPen *pns = m_pDCC->SelectObject(&(m_pBuffer->pnGrf));
    ASSERT(pns);

    if(!PrepareDataXForDraw()) return;
    if(!PrepareDataYForDraw()) return;

    if(typeView == gpoint) 
    {
        DATA.GetLastDataPair(m_pBuffer, m_Ydata, m_Xdata);
        DrawPointA();
    }
    else if(typeView == gline)
    {
        DATA.GetPrevLastDataPair(m_pBuffer, m_Ydata, m_Xdata);
        MoveToA();
        DATA.GetLastDataPair(m_pBuffer, m_Ydata, m_Xdata);
        DrawLineA();
    }
    m_pDCC->SelectObject(pns);

    if(num_grf == 0)
    {
        dgParam.fValue	= m_Xdata;
        dgParam.time	= m_Ydata;
        dgParam.bflData = fldStyle.bitFlags.flViewTime;
        ::SendMessage(hWndGraph, PM_OUTDIGITGRAPH, DATA.GetSizeData(-1), (LPARAM)&dgParam);
    }
}

void CFieldGraph::AddPointXY(F Yvalue, F Xvalue, UINT num_grf)
{
    if(!m_hWnd) return;
    DATA.AddPoint0(Xvalue);
    AddPoint(num_grf, Yvalue);
}


void CFieldGraph::SetViewRangeTime(F startViewTime, F rangeViewTime, F dtime)
{
}
void CFieldGraph::SavePar()
{
    if(!saveList) return;
    if(indexList < 0) indexList = 0;

    if(indexList > indexListMax - 1)
    {
        indexList = indexListMax - 1;
        for(int i = 0; i < indexListMax - 1; i++) 
        {
            saveList[i] = saveList[i+1];
        }
    }

    if((saveList[indexList].startDataView	= DATA.GetPosUMarker(-1, 1)) < 0) return;
    if((saveList[indexList].endDataView		= DATA.GetPosUMarker(-1, 2)) < 0) return;
    for(int i = 0; i < (int)DATA.GetNumBuffers(); i++)
    {
        DATA.GetMinMax(i, saveList[indexList].FminY[i], saveList[indexList].FmaxY[i]);
    }
    DATA.GetMinMax(-1, saveList[indexList].FminX, saveList[indexList].FmaxX);

    maxCountList = indexList;
    indexList++;
}

BOOL CFieldGraph::UndoBack()
{
    if(indexList <= 0) return false;
    if(indexList - 1 >= maxCountList)
    {
        SavePar();
        indexList--;
    }
    indexList--;

    DATA.SetPosUMarker(-1, 1, saveList[indexList].startDataView);
    DATA.SetPosUMarker(-1, 2, saveList[indexList].endDataView);
    for(int i = 0; i < (int)DATA.GetNumBuffers(); i++)
    {
        DATA.GetBuf(i)->SetMinMax(saveList[indexList].FminY[i], saveList[indexList].FmaxY[i]);
    }
    DATA.SetMinMax(-1, saveList[indexList].FminX, saveList[indexList].FmaxX);

    GetParent()->SendMessage(PM_CHANGERECTRANGE, 0, NULL);
    return true;
}

BOOL CFieldGraph::UndoForward()
{
    if(indexList - 1 >= maxCountList - 1) return false;
    indexList++;

    DATA.SetPosUMarker(-1, 1, saveList[indexList].startDataView);
    DATA.SetPosUMarker(-1, 2, saveList[indexList].endDataView);
    for(int i = 0; i < (int)DATA.GetNumBuffers(); i++)
    {
        DATA.SetMinMax(i, saveList[indexList].FminY[i], saveList[indexList].FmaxY[i]);
    }
    DATA.SetMinMax(-1, saveList[indexList].FminX, saveList[indexList].FmaxX);

    GetParent()->SendMessage(PM_CHANGERECTRANGE, 0, NULL);
    return true;
}
void CFieldGraph::UndoStart()
{
	if(indexList == 0 || indexList < 0 || indexList > 99) return;
	indexList = 0;

	DATA.SetPosUMarker(-1, 1, saveList[indexList].startDataView);
	DATA.SetPosUMarker(-1, 2, saveList[indexList].endDataView);
	for(int i = 0; i < (int)DATA.GetNumBuffers(); i++)
	{
		DATA.GetBuf(i)->SetMinMax(saveList[indexList].FminY[i], saveList[indexList].FmaxY[i]);
	}
	DATA.SetMinMax(-1, saveList[indexList].FminX, saveList[indexList].FmaxX);

	GetParent()->SendMessage(PM_CHANGERECTRANGE, 0, NULL);
}
void CFieldGraph::ClearUnDo()
{
	maxCountList = 0;
	indexList = 0;
	indexListMax = 10;
}
//--------------------------------------------------------------------
F CFieldGraph::GetMax(int N, F *V)
{
    if(N == 0) return 1;
    F fmax = -3.4e+38f, a;
    for(int i = 0; i < N; i++) 
    {
        a = fabs(V[i]);
        fmax = _MAX(fmax, a);
    }

    return fmax;
}

F CFieldGraph::NormData(int N, F *V)
{
    if(N == 0) return 1;
    F fmax = GetMax(N, V);
    for(int i = 0; i < N; i++)	V[i] = V[i]/fmax;

    return fmax;
}

BOOL CFieldGraph::XSetScrollGraph(int Pos, int nMax)
{
    F Fmin, Fmax, FminS, FmaxS, diff, fDiapazon;
    diff = DATA.GetMinMax(-1, Fmin, Fmax);
    fDiapazon = DATA.GetMinMaxDiapazon(-1, FminS, FmaxS);

    Fmin = FminS + (Pos * (fDiapazon - diff))/nMax;
    Fmax = Fmin + diff;
    DATA.SetMinMax(-1, Fmin, Fmax);
    int sz_data = DATA.GetSizeData(-1);
    if(sz_data > 0)
    {
        int ind = DATA.FindIndex(-1, 0, sz_data - 1, Fmin);
        if(ind > 0) ind--;
        DATA.SetPosUMarker(-1, 1, ind);
        ind = DATA.FindIndex(-1, ind, sz_data - 1, Fmax);
        if(ind < 0) ind = sz_data - 1;
		if(ind >= sz_data) ind = sz_data - 1;
        DATA.SetPosUMarker(-1, 2, ind);
    }
    m_psclHor->SetMinMax(Fmin, Fmax);
    m_psclHor->Invalidate();
    m_psclHor->UpdateWindow();

    Invalidate(false);
    UpdateWindow();
    return true;
}

BOOL CFieldGraph::YSetScrollGraph(int Pos, int nMax)
{
    VFRingBuf<F>* parY;
	F Fmin, Fmax, FminS, FmaxS, diff, fDiapazon;

    VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS];
    UINT num_buf = DATA.GetTBLPtrBuffers(tblPtrBuffers);
    for(int i = 0; i < (int)DATA.GetNumBuffers(); i++)
    {
        if(num_buf && tblPtrBuffers[i]->GetSizeData() == 0) continue;
		int nn = tblPtrBuffers[i]->num_self;
		parY = DATA.GetBuf(nn); 
		if(parY == NULL) continue;

		diff = parY->GetMinMax(Fmin, Fmax);
		fDiapazon = parY->GetMinMaxDiapazon(FminS, FmaxS);

		Fmin = FminS + (Pos * (fDiapazon - diff))/nMax;
		Fmax = Fmin + diff;
		parY->SetMinMax(Fmin, Fmax);
    }
    m_psclVert->SetMinMax(Fmin, Fmax);

    m_psclVert->Invalidate();
    m_psclVert->UpdateWindow();

    Invalidate(true);
    UpdateWindow();

    return true;
}

BOOL CFieldGraph::HSetScroolGraph(int Pos, int nMax)
{
    BOOL bflOrient = pGraph->GetBitStyle(GRF_ORIENT_FLD);
    if(!bflOrient) return XSetScrollGraph(Pos, nMax);
    else return YSetScrollGraph(Pos, nMax);
}

BOOL CFieldGraph::VSetScroolGraph(int Pos, int nMax)
{
    BOOL bflOrient = pGraph->GetBitStyle(GRF_ORIENT_FLD);
    if(!bflOrient) return YSetScrollGraph(Pos, nMax);
    else return XSetScrollGraph(Pos, nMax);
}
BOOL CFieldGraph::ChangeHScale(F deltaScale)
{
    return true;
}

BOOL CFieldGraph::ChangeVScale(F deltaScale)
{
    return true;
}


int CFieldGraph::GetHPos(int nIndex, int nPage, int nMax)
{
    return 0;
}

int CFieldGraph::GetVPos(F fValue, int nPage, int nMax)
{
    return 0;
}

BOOL CFieldGraph::HTestRange()
{
    if(ScaleFX == 1) return true;
    else return false;
}

BOOL CFieldGraph::VTestRange()
{
    if(ScaleFY == 1) return true;
    else return false;
}

void CFieldGraph::SetGreed(BOOL greed)
{
    if(!m_hWnd) return;
    if (fldStyle.bitFlags.flGreed == greed)
        return;
    fldStyle.bitFlags.flGreed = greed;
    Invalidate();
}

BOOL CFieldGraph::TestGreed()
{
    return fldStyle.bitFlags.flGreed;
}

void CFieldGraph::SetScaleLine(BOOL scaleline)
{
    if(!m_hWnd) return;

    fldStyle.bitFlags.flScale = scaleline;
    Invalidate();
}

void CFieldGraph::SetStyleLineFunc(UINT stypeView)
{
    if(!m_hWnd) return;

    switch(stypeView)
    {
    case 0: typeView = gpoint; break;
    case 1: typeView = gpoint1; break;
    case 2: typeView = gpoint2; break;
    case 3: typeView = gline; break;
    case 4: typeView = gline1; break;
    case 5: typeView = gstick; break;
    }
    Invalidate();
}

void CFieldGraph::SetStyle(DWORD dwStyle)
{
    fldStyle.dwFlags |= dwStyle;
}
void CFieldGraph::ResetStyle(DWORD dwStyle)
{
	dwStyle = ~dwStyle;
	fldStyle.dwFlags &= dwStyle;
}
void CFieldGraph::SetRectRangeA()
{
	if(slctRect.Width() < 20 || slctRect.Height() < 20) return;
	SavePar();

	F kf1, kf2, kf3, minf, maxf;
	F df = DATA.GetDiffMaxMin(-1);
	if(df == 0) return;

	kf3 = df;
	DATA.GetMinMax(-1, minf, maxf);

	BOOL flOrient = fldStyle.bitFlags.flOrient;
	CRect rec = fieldRec, rec1 = slctRect;
	RotRectCCW90(rec, flOrient);
	RotRectCCW90(rec1, flOrient);
	if(flOrient)
	{
		int h = rec1.Height();
		rec1.bottom = rec.bottom - rec1.top;
		rec1.top = rec1.bottom - h;
	}

	kf1 = F((rec1.left - rec.left))/rec.Width();
	kf2 = F((rec1.right - rec.left))/rec.Width();
	maxf = minf + kf3*kf2;
	minf += kf3*kf1;
	DATA.SetMinMax(-1, minf, maxf);
	DATA.SetPosViewMarkersDef();
	m_psclHor->SetMinMax(minf, maxf);
	if(!fldStyle.bitFlags.flTypeScale)
	{
		kf1 = F((rec.bottom - rec1.bottom))/rec.Height();
		kf2 = F((rec.bottom - rec1.top))/rec.Height();
	}
	else
	{
		kf1 = F((rec1.top - rec.top))/rec.Height();
		kf2 = F((rec1.bottom - rec.top))/rec.Height();
	}
	VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS];
	UINT num_buf = DATA.GetTBLPtrBuffers(tblPtrBuffers);
	for(int i = 0; i < (int)DATA.GetNumBuffers(); i++)
	{
		if(num_buf && tblPtrBuffers[i]->GetSizeData() == 0) continue;
		kf3 = tblPtrBuffers[i]->Fmax - tblPtrBuffers[i]->Fmin; 

		minf = tblPtrBuffers[i]->Fmin;
		tblPtrBuffers[i]->SetMinMax(minf + kf3*kf1, minf + kf3*kf2);
	}
	GetParent()->SendMessage(PM_CHANGERECTRANGE, 0, NULL);
}

void CFieldGraph::SetRectRange()
{
    if(!m_hWnd) return;
	SetRectRangeA();
	F df = DATA.GetDiffMaxMin(-1);
	int nn;
	if(fldStyle.bitFlags.flViewTime) nn = int((df - int(df))*60*60*24); //число секунд в шкале
	else nn = int(df*100); 

	GetParent()->GetParent()->PostMessage(PM_CHANGE_RECT_RANGE, pGraph->num_self, nn);
}
void CFieldGraph::SetRectRangeExternal()
{
	SetRectRangeA();
}

F CFieldGraph::GetDistancePoint(CPoint pp1, CPoint pp2)
{
    double dx = abs(pp1.x - pp2.x), dy = abs(pp1.y - pp2.y);
    return sqrt(dx*dx + dy*dy);
}

TMarker<F>* CFieldGraph::GetEditPoint(int& inex_x, int& inex_y, int& num_buf)
{
    m_rec = fieldRec;
    F dist;

    VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS];
    DATA.GetTBLPtrBuffers(tblPtrBuffers);

    int num_buf_find;
    CPoint last_point(20000, 20000), p_index(0,0);
    F distance = GetDistancePoint(last_point, pointEdit);

    if(!PrepareDataXForDraw()) return NULL;
    for(int nb = 0; nb < (int)DATA.GetNumBuffers(); nb++)
    {
        m_pBuffer = tblPtrBuffers[nb];
        if(m_pBuffer->GetSizeData() == 0 || !m_pBuffer->Get_flg_view_data()) continue;

        UINT num_grf = m_pBuffer->num_self;
        POINT p_iter = DATA.SetIterPos(m_pBuffer, 0);
        if(p_iter.x < 0) continue;

        if(!PrepareDataYForDraw()) continue;

        int n_frg = m_pBuffer->Get_nn_iter_frag();
        for(int j = 0; j < n_frg; j++)
        {
            p_iter = DATA.GetNext0(m_Ydata, m_Xdata);
            //------------------------------------------------------------------------
            GetXY(last_point);
            dist = GetDistancePoint(last_point, pointEdit);
            if(dist < distance) 
            {
                distance = dist;  
                p_index = p_iter;
                if(p_index.y < 0)  p_index.y = - p_index.y;
            }
            num_buf_find = j;
            num_buf = nb;
            //------------------------------------------------------------------------
            if(p_iter.y < 0) continue;

            int sz_frag = m_pBuffer->GetSizeIterFrag();
            if(sz_frag == 0) continue;
            for(int i = 1; i < sz_frag; i++)  //why 1
            {
                p_iter = DATA.GetNext0(m_Ydata, m_Xdata);
                //------------------------------------------------------------------------
                GetXY(last_point);
                dist = GetDistancePoint(last_point, pointEdit);
                if(dist < distance) 
                {
                    distance = dist;  
                    p_index = p_iter;
                    if(p_index.y < 0)  p_index.y = - p_index.y;
                }
                num_buf_find = j;
                num_buf = nb;
                //------------------------------------------------------------------------
            }
        }
    }

    inex_x = p_index.x;
    inex_y = p_index.y;

    return FindeMarker(-1, inex_x);
}

TMarker<F>* CFieldGraph::FindeMarker(int num_buf, int index_point)
{
    VFRingBuf<F> *p_tmpBuf = DATA.GetBuf(num_buf);
    if(p_tmpBuf == NULL) return NULL;

    int max_rp = DATA.GetNumMarkers(num_buf);
    TMarker<F> **tblPtrReper = (TMarker<F> **)(new BYTE[max_rp*sizeof(TMarker<F>*)]);
    numRepers = DATA.GetTBLPtrMarkers(num_buf, tblPtrReper);

    if(numRepers == 0) 
    {
        delete tblPtrReper;
        return NULL;
    }

    TMarker<F> *pMarker = NULL;
    for(int i = 0; i < numRepers; i++)
    {
        if(tblPtrReper[i]->base_position == index_point) 
        {
            pMarker = tblPtrReper[i];
            break;
        }
    }

    BYTE *bb = (BYTE*)tblPtrReper;
    delete bb;

    return pMarker;
}

TMarker<F>* CFieldGraph::SetMarker(int num_buf, int index, COLORREF clr, char type_ch)
{
    F value; 
    DATA.GetElement(-1, index, value);
    TMarker<F>* pMarker = DATA.CreateMarker(num_buf, index, CString(type_ch), clr, clr, value, type_ch);

    CClientDC dc(this);
    DrawRepers(&dc, &fieldRec);
	return pMarker;
}

LRESULT CFieldGraph::OnEditMarker(WPARAM wParam, LPARAM lParam)
{
	if(pMarker0 == NULL) return false; 

	VFRingBuf<F>* pBuf;
	FRingBuf<F> *p_curRBuf;
	switch (wParam)
	{
	case 1: 
		pBuf = DATA.GetBuf(pMarker0->num_buf);
		if(pBuf == NULL) return false;
		p_curRBuf = pBuf->GetCurRBuf();
		if(p_curRBuf == NULL) return  false;
		p_curRBuf->RemoveMarker(pMarker0->base_position); 
		pMarker0 = NULL;
		break;

	case 2: 
		pMarker0->flag_select = false;
		pMarker0 = NULL;
		break;

	case 3: 
		pMarker0->base_position--;
		break;

	case 4: 
		pMarker0->base_position++;
		if(pMarker0->base_position < 0) pMarker0->base_position = 0;
		break;
	}

	Invalidate(true);
	return true;
}

void CFieldGraph::SetEditPoint(F XX, F YY, int indexp)
{
    VFRingBuf<F>* pMain = DATA.GetBuf(-1);
    VFRingBuf<F>* pBuf = DATA.GetBuf(0);
    if(pMain == NULL || pBuf == NULL) return;
    if(indexp < 0 || indexp > (int)pMain->GetSizeData()) return;
    pBuf->SetPoint(indexp, YY / pBuf->maxF);
    pMain->SetPoint(indexp, XX);
}

int CFieldGraph::IndexTime(CPoint point, F& dTime, F& fVal)
{
    if(DATA.GetSizeData(-1) == 0) return 0;

    int xleft, xright, ytop, ybtm, index1, i0;
    F szx;

    CRect rec = fieldRec;

    RotRect90(rec, fldStyle.bitFlags.flOrient);

    xleft = rec.left;
    xright = rec.right;
    ytop = rec.top;
    ybtm = rec.bottom;

    szx = DATA.GetDiffMaxMin(-1);
    index1 = DATA.GetSizeData(-1);

    if(fldStyle.bitFlags.flOrient) i0 = point.y;
    else i0 = point.x;

    F Fmin, Fmax;
    DATA.GetMinMax(-1, Fmin, Fmax);
    F value = ((i0 - xleft)*szx)/(xright - xleft) + Fmin;
    UINT ind = DATA.FindIndex(-1, 0, DATA.GetSizeData(-1) - 1, value);

    DATA.GetElement(-1, ind, dTime);
    DATA.GetElement(0, ind, fVal);
    return ind;
}

void CFieldGraph::IndexTime(CPoint point, F& dTime, F& fVal, int &indexp)
{
    indexp = IndexTime(point, dTime, fVal);
}

int CFieldGraph::GetIndex(CPoint point)
{
    F kf1 = F((point.x - fieldRec.left))/fieldRec.Width();
    int indexEnd = DATA.GetSizeData(-1) - 1;
    F sizeHView = indexEnd/ScaleFX;
    int ind = DATA.GetPosUMarker(-1, 1);
    if(ind < 0) return -1;
    return (int)(ind + sizeHView*kf1);
}

int CFieldGraph::GetIndex(F value)
{
    return DATA.FindIndex(-1, 0, DATA.GetSizeData(-1), value);
}

int CFieldGraph::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CStatic::OnCreate(lpCreateStruct) == -1)
        return -1;

    if(fFontTextReper.m_hObject) fFontTextReper.DeleteObject();
    fFontTextReper.CreateFontIndirect(&lfntTextReper);

    if(fFontTextGrf.m_hObject) fFontTextGrf.DeleteObject();
    fFontTextGrf.CreateFontIndirect(&lfntTextGrf);

    pnScaleLine.CreatePen(PS_SOLID, 1, colorScale);
    pnMainStrich.CreatePen(PS_SOLID, sizeMainStrich.cx, colorMainStrich);
    pnSlaveStrich.CreatePen(PS_SOLID, sizeSlaveStrich.cx, colorSlaveStrich);

    pnSelectGraph.CreatePen(PS_SOLID, 1, colorSelectGraph);
    pnGreed.CreatePen(PS_DASHDOT, 1, colorGreed);
    pnAlert1.CreatePen(PS_SOLID, 1, colorAlert1);
    pnAlert2.CreatePen(PS_SOLID, 1, colorAlert2);
    pnAlert3.CreatePen(PS_SOLID, 1, colorAlert3);
    pnAlert4.CreatePen(PS_SOLID, 1, colorAlert4);
    pnReper.CreatePen(PS_SOLID, 1, colorReper);

    cursorNorm = LoadCursor(NULL, IDC_ARROW);
    cursorCross = LoadCursor(NULL, IDC_CROSS);

    saveList = (ParamForSavesAndRestores*)new BYTE[100*sizeof(ParamForSavesAndRestores)];

    if (!saveList)
    {
        TRACE0("Not enough memory.\n");
        return 0;
    }

    ClearUnDo();

    CRect rec;
    GetClientRect(&rec);

    rec.bottom = rec.top + 15;

    m_val.Create(NULL, WS_CHILD|WS_VISIBLE, rec, this, ID_EDIT_OUT);
    m_val.ShowWindow(SW_HIDE);

    m_val.m_pFld = (LPVOID)(this);

    return 0;
}
void CFieldGraph::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {	// storing code
    }
    else
    {	// loading code
    }
}

void CFieldGraph::SetTP()
{
    if(!m_hWnd) return;

    CRect rec;
    GetClientRect(&rec);
}

void CFieldGraph::OnDestroy() 
{
    CStatic::OnDestroy();

    if(fFontTextReper.m_hObject) fFontTextReper.DeleteObject();
    if(fFontTextGrf.m_hObject) fFontTextGrf.DeleteObject();
    pnScaleLine.DeleteObject();
    pnMainStrich.DeleteObject();
    pnSlaveStrich.DeleteObject();
    pnSelectGraph.DeleteObject();
    pnGreed.DeleteObject();
    pnAlert1.DeleteObject();
    pnAlert2.DeleteObject();
    pnAlert3.DeleteObject();
    pnAlert4.DeleteObject();
    pnReper.DeleteObject();

    if(saveList) delete saveList;
    saveList = NULL;
    m_val.DestroyWindow();
}

void CFieldGraph::ConvToScale(CRect& rec, F sclX, F sclY)
{
    rec.left = int(rec.left * sclX);
    rec.top = int(rec.top * sclY);
    rec.right = int(rec.right * sclX);
    rec.bottom = int(rec.bottom * sclY);
}

void CFieldGraph::DrawGraphText(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
    CRect rec;
    GetClientRect(&rec); 
    CFont *pFont = NULL;
    //.................................................................................	
    if(pDC->IsPrinting()) 
    {
        if(!pWnd) return;

        ClientToScreen(&rec);
        LOGFONT lf = lfntTextGrf;
        lf.lfHeight = int((lf.lfHeight * sclY)/2.0);
        lf.lfWidth = int((lf.lfWidth * sclX)/2.0);
        CFont fnt;
        fnt.CreateFontIndirect(&lf);
        pFont = pDC->SelectObject(&fnt);
        pDC->SetTextColor(RGB(100,100,100));
    }
    else
    {
        pFont = pDC->SelectObject(&fFontTextGrf);
        pDC->SetTextColor(colorTextGrf);
    }

    ASSERT(pFont);

    CSize sz = CScale::GetSizeText(pDC, m_textGrf, &fFontTextGrf, true);
    pDC->SetBkMode(TRANSPARENT);
    pDC->TextOut(rec.right - sz.cx - 2, rec.top + 2, m_textGrf);

    pDC->SelectObject(pFont);
}
CString CFieldGraph::GetStrDataX(F value)
{
	CString str;
	COleDateTime time_val = value;
	if(!m_psclHor->sclStyle.flViewTime)
	{
		F aff = fabs(value);
		str.Format("%.1f", value);
	}
	else
	{
		if (time_val.m_dt < MAX_DATE && time_val.m_dt > MIN_DATE)
			str.Format("%02d-%02d-%02d", time_val.GetDay(), time_val.GetMonth(), time_val.GetYear()); 
	}
	return str;
}
void CFieldGraph::DrawDataOnField(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
	CRect rec;
	GetClientRect(&rec); 
	CFont *pFont = NULL;

	CString str_data_beg, str_data_end;
	str_data_beg = GetStrDataX(m_psclHor->Fmin);
	str_data_end = GetStrDataX(m_psclHor->Fmax);
	//.................................................................................	
	if(pDC->IsPrinting()) 
	{
		if(!pWnd) return;

		ClientToScreen(&rec);
		LOGFONT lf = lfntTextGrf;
		lf.lfHeight = int((lf.lfHeight * sclY)/2.0);
		lf.lfWidth = int((lf.lfWidth * sclX)/2.0);
		CFont fnt;
		fnt.CreateFontIndirect(&lf);
		pFont = pDC->SelectObject(&fnt);
		pDC->SetTextColor(RGB(100,100,100));
	}
	else
	{
		pFont = pDC->SelectObject(&fFontTextGrf);
		pDC->SetTextColor(colorTextGrf);
	}

	ASSERT(pFont);
	pDC->SetBkMode(TRANSPARENT);

	CSize szL = CScale::GetSizeText(pDC, str_data_beg, &fFontTextGrf, true);
	CSize szR = CScale::GetSizeText(pDC, str_data_end, &fFontTextGrf, true);

	if(!fldStyle.bitFlags.flOrient) 
	{
		pDC->TextOut(rec.left + int(2*sclX), rec.bottom - szL.cy - int(2*sclY), str_data_beg);
		pDC->TextOut(rec.right - szR.cx - int(2*sclX), rec.bottom - szR.cy - int(2*sclY), str_data_end);
	}
	else
	{
		pDC->TextOut(rec.left + int(2*sclX), rec.top + int(2*sclY), str_data_beg);
		pDC->TextOut(rec.left + int(2*sclX), rec.bottom - szR.cy - int(2*sclY), str_data_end);
	}

	pDC->SelectObject(pFont);
}
BOOL CFieldGraph::CreateMemoryPaintDC(CDC* pDC)
{
	CPaintDC* pDCPaint = (CPaintDC*)pDC;
	int cxClip = fieldRec.Width();
	int cyClip = fieldRec.Height();
	if(cxClip <= 0 || cyClip <= 0)
        return false;

	if (m_pDCMem == NULL)
	{
		m_pDCMem = new CDC();
		ASSERT(m_pDCMem != NULL);
		VERIFY(m_pDCMem->CreateCompatibleDC(pDC));
	}

	if (m_pBitmap == NULL)
    {
		m_pBitmap = new CBitmap();
        m_sizeBitmap = CSize(0,0);
    }

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
void CFieldGraph::DrawGrf(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
    if(!m_hWnd) return;

    CRect rec = fieldRec, rec1 = fieldRec,  rec2 = slctRectPush, rec3 = slctRect;
    CRect clipRect;

    fieldRecTST = fieldRec;
    GetClientRect(&fieldRec);

    if(pDC->IsPrinting()) 
    {
        if(!pWnd) return;
        GetClientRect(&rec); //??**
        ClientToScreen(&rec);
        		Daraw2Rect(pDC, rec, RGB(0,255,0));

        int svdc = pDC->SaveDC();
        pDC->IntersectClipRect(&rec);

        DrawLinesAlert(pDC, pWnd, sclX, sclY, prnShift);
        DrawGreed(pDC, pWnd, sclX, sclY, prnShift);
        DrawLineScale(pDC, pWnd, sclX, sclY, prnShift);
        DrawGraph(pDC, pWnd, sclX, sclY, prnShift);
        DrawGraphText(pDC, pWnd, sclX, sclY, prnShift);
		if(fldStyle.bitFlags.flDataOnField) DrawDataOnField(pDC, pWnd, sclX, sclY, prnShift);

        pDC->RestoreDC(svdc);
    }
    else 
    {
        rec.InflateRect(2,2);

		if(!CreateMemoryPaintDC(pDC)) return;

        if(flagfragment) 
        {
            if(slctRectPush.Width()) m_pDCMem->FillSolidRect(&slctRectPush, colorFon);
            if(slctRect.Width() != 0) m_pDCMem->FillSolidRect(&slctRect, colorSelectGraph);
        }
        else m_pDCMem->FillSolidRect(&fieldRec, colorFon);
        CPen pen(PS_SOLID,1,RGB(0,0,0));
        CPen *oldpen = m_pDCMem->SelectObject(&pen);
        ASSERT(oldpen);
        m_pDCMem->SelectObject(oldpen);
        DrawGreed(m_pDCMem);
        DrawLineScale(m_pDCMem);
        DrawGraph(m_pDCMem);
        DrawGraphText(m_pDCMem);
		if(fldStyle.bitFlags.flDataOnField) DrawDataOnField(m_pDCMem);
        pDC->BitBlt(rec1.left,rec1.top,rec1.Width()+10,rec1.Height()+1,m_pDCMem,0,0,SRCCOPY);
    }
}

void CFieldGraph::OnPaint() 
{
	paint_lock = true;
    CPaintDC dc(this);
    DrawGrf(&dc);
	paint_lock = false;
}

int CFieldGraph::GetNearestPoint(CPoint& point)
{
	POINT pXY;
	PrepareDataXForDraw();
	PrepareDataYForDraw();
	GetXY(pXY);
	BackConversionXY(pXY);
	int xx = pXY.x, yy = pXY.y;

	return abs(yy-point.y);
}

int CFieldGraph::SetMouseValue(CPoint& point)
{
    VFRingBuf<F>* pMain = DATA.GetBuf(-1);
    if(pMain == NULL) return -1;

    F Fmin, Fmax, value;
    pMain->GetMinMax(Fmin, Fmax);

    CRect rec = fieldRec;
    RotRect90(rec, fldStyle.bitFlags.flOrient);

    F dl, pxy;
    if(!fldStyle.bitFlags.flOrient) 
    {
        dl = fieldRec.Width();
        pxy = point.x;
    }
    else
    {
        dl = fieldRec.Height();
        pxy = point.y;
    }

    value = Fmin + ((Fmax - Fmin)*pxy)/dl;
    if(value < pMain->GetFirstData()) index_x = 0;
    else if(value > pMain->GetLastData()) index_x = pMain->GetSizeData() - 1;
    else index_x = pMain->FindIndexInViewRange(value);
    pMain->mouse_value = value;
    pMain->mouse_index = index_x;

    VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS], *pBuf;
    FRingBuf<F> *p_curRBuf;
    UINT num_buf = DATA.GetTBLPtrBuffers(tblPtrBuffers);
	if(num_buf == 0) return -1;
    index_y = 0;

	nearest_distance = 1.e+20;
	int nd = nearest_distance;
	int nb = -1;
	m_Xdata = value;
    for(int i = 0; i < (int)DATA.GetNumBuffers(); i++)
    {
        pBuf = tblPtrBuffers[i];
        if(pBuf == NULL) continue;
        p_curRBuf = pBuf->GetCurRBuf();
        if(p_curRBuf == NULL) continue;
        if((index_y = pBuf->mouse_index = p_curRBuf->FindeIndexInFrag(index_x)) < 0) continue;
        pBuf->mouse_value = p_curRBuf->GetElement(index_y)*pBuf->maxF;

		m_Ydata = pBuf->mouse_value/pBuf->maxF;
		nd = GetNearestPoint(point);
		if(nd < nearest_distance) 
		{
			nearest_distance = nd;
			nb = pBuf->num_self;
			index_x0 = index_x;
			index_y0 = index_y;
		}
    }
	return nb;
}

void CFieldGraph::OnLButtonDown(UINT nFlags, CPoint point) 
{
	pointEdit = point;
    if(nFlags == 9)
    {
		int nub_buf = SetMouseValue(point);
		pMarker0 = FindeMarker(nub_buf, index_x0);
		if(pMarker0 != NULL)
		{
			CClientDC dc(this);
			pMarker0->flag_select = pMarker0->flag_select ? false : true;
			DrawRepers(&dc, &fieldRec, true);
			if(!pMarker0->flag_select) pMarker0 = NULL;
		}
    }
    else if(nFlags == 5)
    {
        GetParent()->SendMessage(PM_EDITPOINTGRAPH, 150258, NULL);
    }
	else if(nFlags == 13)
	{
		int nub_buf = SetMouseValue(point);
		SetMarker(nub_buf, index_x0, RGB(0,0,0), 'A');
	}
    else if(typeGraph != 0)
    {
		slctRect = CRect(point, CSize(0,0));
		flagtrack = true;

		CClientDC dc(this);
		slctRect.NormalizeRect();
		dc.DrawFocusRect(&slctRect);
    }
    CStatic::OnLButtonDown(nFlags, point);
}

void CFieldGraph::OnLButtonUp(UINT nFlags, CPoint point) 
{
    if((typeGraph != 0) && (nFlags == 0))
    {
        flagtrack = false;
        CClientDC dc(this);
        dc.DrawFocusRect(&slctRect);
		CSize sz = slctRect.Size();
		if(sz.cx > 20 && sz.cy > 20) SetRectRange();
    }
    CStatic::OnLButtonUp(nFlags, point);
}

void CFieldGraph::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    if(!m_hWnd) return;
    pointEdit = point;
    GetParent()->SendMessage(PM_EDITPOINTGRAPH, 150257, NULL);
    CStatic::OnLButtonDblClk(nFlags, point);
}

void CFieldGraph::OnMouseMove(UINT nFlags, CPoint point) 
{
    if(m_hWnd == NULL) return;

    CRect rec;
    GetClientRect(&rec);

	if(pMarker0 && 	flagtrack)
	{
		pMarker0->SetShiftText(point.x - pointEdit.x, point.y - pointEdit.y);
		CClientDC dc(this);
		DrawRepers(&dc, &fieldRec, true);
		Invalidate(true);
		goto EN1;
	}

    if(flagtrack)
    {
        if(typeGraph != 0)
        {
            CClientDC dc(this);

            dc.DrawFocusRect(&slctRect);
			slctRect = CRect(slctRect.TopLeft(), point);
			slctRect.NormalizeRect();

			if(pGraph->GetBitStyle(FLD_SELECT_ONLY_Y))
			{
				if(nFlags == 1)
				{
					if(fldStyle.bitFlags.flOrient)
					{
						slctRect.left = rec.left;
						slctRect.right = rec.right;
					}
					else
					{
						slctRect.top = rec.top;
						slctRect.bottom = rec.bottom;
					}
				}
			}
			else
			{
				if(nFlags == 13)//Ctrl + Shift
				{
					if(fldStyle.bitFlags.flOrient)
					{
						slctRect.left = rec.left;
						slctRect.right = rec.right;
					}
					else
					{
						slctRect.top = rec.top;
						slctRect.bottom = rec.bottom;
					}
				}
			}
            dc.DrawFocusRect(&slctRect);
        }
    }

    if(GetCapture() != this) 
    {
        ::SetCursor(cursorCross);
        SetCapture();
    }
    else
    {
		CPoint screen_point = point;
		ClientToScreen(&screen_point);
		CWnd *wnd = WindowFromPoint(screen_point);
		if (wnd != this || !rec.PtInRect(point))
		{
			::SetCursor(cursorNorm);
			ReleaseCapture();
		}
	}
EN1:;
    //-------------------------------------------------------------
    if(flag_rbtn_down || fldStyle.bitFlags.flFixData) 
    {
        SetMouseValue(point);
        PostMessage(PM_EDITPOINTGRAPH, 150259, NULL);
    }

    m_val.m_bType = fldStyle.bitFlags.flViewTime;
    if(m_val.m_hWnd != NULL) m_val.Invalidate(true);
    //-------------------------------------------------------------

    CStatic::OnMouseMove(nFlags, point);
}

void CFieldGraph::OnRButtonDown(UINT nFlags, CPoint point) 
{
    if(!m_hWnd) return;
    flag_rbtn_down = true;

	if(nFlags == 6)
	{
		GetParent()->SendMessage(PM_SHOW_PANEL, 0, NULL);
	}
 
	SetMouseValue(point);
    PostMessage(PM_EDITPOINTGRAPH, 150259, NULL);

    CRect rec;
    GetClientRect(&rec);
    rec.bottom = rec.top + 15;

    if(m_val.m_hWnd != NULL) m_val.SetWindowPos(&wndBottom,
        rec.left, 
        rec.top, 
        rec.Width() - 2, 
        rec.Height(), 
        SWP_SHOWWINDOW);
    m_val.m_point = point;
    m_val.m_bType = fldStyle.bitFlags.flViewTime;


    if(m_val.m_hWnd != NULL) m_val.Invalidate(true);

    GetParent()->SendMessage(PM_R_BUTTON_DOWN, 0, 0);
    CStatic::OnRButtonDown(nFlags, point);
}

void CFieldGraph::OnRButtonUp(UINT nFlags, CPoint point) 
{
    flag_rbtn_down = false;
    if(m_val.m_hWnd != NULL && !fldStyle.bitFlags.flFixData) m_val.ShowWindow(SW_HIDE);
    CStatic::OnRButtonUp(nFlags, point);
}

void CFieldGraph::OnSize(UINT nType, int cx, int cy) 
{
    CStatic::OnSize(nType, cx, cy);
    GetClientRect(&fieldRec);
}

void CFieldGraph::DoHScroll(int shift)
{
}
UINT CFieldGraph::CreateBufer(UINT sizeBuf, F* pBufY, F* pBufX)
{
    if(sizeBuf <= 0) return -1;
    DATA.DeleteAllBuffers();
    UINT num_buf = DATA.CreateBufferInit(sizeBuf, sizeBuf, 0, colorGraph[0], pBufY, pBufX);

    ASSERT(num_buf == 0);

    VFRingBuf<F>* pMain = DATA.GetBuf(-1);
    if(pMain == NULL) return -1;
    pMain->SetPosUMarker(1, 0);
    DATA.SetPosUMarker23ToEndBuf(-1);

    pMain->SetMinMax(pMain->GetDataUMarker(1), pMain->GetDataUMarker(2));
    pMain->SaveMinMax();
    ClearUnDo();
    if(pvReper) delete[] pvReper;
    pvReper = (ReperParamV*)new ReperParamV[MAX_NUM_REPERS];
    ASSERT(pvReper);

    if(ptsReper) delete[] ptsReper;
    ptsReper = (ReperParam*)new ReperParam[MAX_NUM_REPERS];
    ASSERT(ptsReper);

    numRepers = 0;
    numRepersTS = 0;
    return num_buf;
}
void CFieldGraph::CreateBufX(UINT size_buf, UINT size_data, F* pBufX)
{
    DATA.InitX(size_buf, size_data, 0, pBufX);
    ClearUnDo();
}
void CFieldGraph::CreateBufX(VFRingBuf<F>* pBufX)
{
    DATA.InitX(pBufX);
    ClearUnDo();
}
void CFieldGraph::CreateBufX(FRingBuf<F>* pBufX)
{
    DATA.InitX(pBufX);
    ClearUnDo();
}
void CFieldGraph::SetAdrBufX(UINT base_pos, VFRingBuf<F>& pBufX)
{
    DATA.SetExternalBufX(base_pos, pBufX);
    ClearUnDo();
}
void CFieldGraph::SetAdrBufX(UINT base_pos, FRingBuf<F>& pBufX)
{
    DATA.SetExternalBufX(base_pos, pBufX);
    ClearUnDo();
}
BOOL CFieldGraph::InitDataX(int size_data, F* pBufX)
{
    if(size_data <= 0) return false;
    DATA.InitX(size_data, size_data, 0, pBufX);
    ClearUnDo();
    return TRUE;
}
void CFieldGraph::InitDataX(VFRingBuf<F>& pBufX)
{
    DATA.InitX(pBufX);
    ClearUnDo();
}
void CFieldGraph::InitDataX(FRingBuf<F>& pBufX)
{
    DATA.InitX(pBufX);
    ClearUnDo();
}
UINT CFieldGraph::CreateBufY(UINT size_buf, UINT size_data, F* pBufY)
{
    UINT nn = DATA.GetNumBuffers();
    UINT num_buf = DATA.CreateBuffer(size_buf, size_data, 0, colorGraph[nn%10], pBufY);
    ClearUnDo();
    return num_buf;
}
UINT CFieldGraph::CreateBufY(VFRingBuf<F>* pBufY)
{
    UINT num_buf = DATA.CreateBuffer(pBufY);
    ClearUnDo();
    return num_buf;
}
UINT CFieldGraph::CreateBufY(FRingBuf<F>* pBufY)
{
    UINT nn = DATA.GetNumBuffers();
    UINT num_buf = DATA.CreateBuffer(pBufY);
    DATA.GetBuf(num_buf)->SetBaseColor(colorGraph[nn%10]);
    ClearUnDo();
    return num_buf;
}
void CFieldGraph::SetAdrBufY(UINT num_buf, UINT base_pos, VFRingBuf<F>& pBufY)
{
    DATA.SetExternalBufY(base_pos, num_buf, pBufY);
    ClearUnDo();
}
UINT CFieldGraph::SetAdrBufY(UINT base_pos, FRingBuf<F>& pBufY)
{
    UINT nn = DATA.GetNumBuffers();
    UINT num_buf = DATA.SetExternalBufY(base_pos, pBufY);
    DATA.GetBuf(num_buf)->SetBaseColor(colorGraph[nn%10]);
    return num_buf;
}
void CFieldGraph::SetAdrBufY(UINT num_buf, UINT base_pos, FRingBuf<F>& pBufY)
{
    UINT nn = DATA.GetNumBuffers();
    DATA.SetExternalBufY(base_pos, num_buf, pBufY);
    DATA.GetBuf(num_buf)->SetBaseColor(colorGraph[nn%10]);
}
UINT CFieldGraph::CreateBufYWithExtInpBuf(UINT base_pos, VFRingBuf<F>& pBufY)
{
    UINT num_buf = DATA.CreateBuffer(base_pos);
    DATA.SetExternalBufY(base_pos, num_buf, pBufY);
    ClearUnDo();
    return num_buf;
}
UINT CFieldGraph::CreateBufYWithExtInpBuf(UINT base_pos, FRingBuf<F>& pBufY)
{
    UINT nn = DATA.GetNumBuffers();
    UINT num_buf = DATA.CreateBuffer((FRingBuf<F>*)NULL);
    DATA.SetExternalBufY(base_pos, num_buf, pBufY);
    if(DATA.GetBuf(num_buf) != NULL) DATA.GetBuf(num_buf)->SetBaseColor(colorGraph[nn%10]);
    ClearUnDo();
    return num_buf;
}
void CFieldGraph::InitDataY(UINT num_buf, UINT size_data, F* pBufY)
{
    DATA.InitY(num_buf, size_data, size_data, 0, pBufY);
}
void CFieldGraph::InitDataY0(UINT num_buf, UINT beg_index, UINT size_data, F* pBufY)
{
	DATA.InitY(num_buf, beg_index, size_data, pBufY);
}
void CFieldGraph::InitDataY(UINT num_buf, VFRingBuf<F>& pBufY)
{
    DATA.InitY(num_buf, pBufY);
}
void CFieldGraph::InitDataY(UINT num_buf, FRingBuf<F>& pBufY)
{
    DATA.InitY(num_buf, pBufY);
}
UINT CFieldGraph::InitDataYWithName(CString sName, UINT sizeBuf, F* inBufY)
{
    int num_buf = DATA.GetBufWithName(sName);
    if(num_buf < 0)
    {
        num_buf = CreateBufY(sizeBuf, sizeBuf, inBufY);
        SetNameGrf(num_buf, sName);
    }
    else InitDataY(num_buf, sizeBuf, inBufY);
    return num_buf;
}

void CFieldGraph::AddData(UINT num_buf, UINT size_data, UINT base_pos, F* inBuf)
{
    DATA.AddData(num_buf, size_data, base_pos, inBuf);
}

F CFieldGraph::GetScaleXStart()
{
    VFRingBuf<F>* pMain = DATA.GetBuf(-1);
    if(pMain == NULL) return 0;
    return pMain->GetDataUMarker(1);
}

void CFieldGraph::GetMinMaxDiapazonDef(int num_grf, F& FminDiapazon, F& FmaxDiapazon) 
{
    VFRingBuf<F>* pBuf = DATA.SelectBuf(num_grf);
    if(pBuf == NULL) return;
    pBuf->CalcMinMaxDiapazon(FminDiapazon, FmaxDiapazon);
}
GRF_MINMAX CFieldGraph::SetParMinMax(int num_grf, GRF_MINMAX& par)
{
    GRF_MINMAX old_par;
    VFRingBuf<F>* pBuf = DATA.SelectBuf(num_grf);
    if(pBuf == NULL) return old_par;
    if(par.flags_minmax.bitFlags.fl_min_max) 
    {
        pBuf->GetMinMax(old_par.Fmin, old_par.Fmax);
        pBuf->SetMinMax(par.Fmin, par.Fmax);
    }
    if(par.flags_minmax.bitFlags.fl_diapazon) 
    {
        pBuf->GetMinMaxDiapazon(old_par.FminS, old_par.FmaxS);
        pBuf->SetMinMaxDiapazonEX(par.FminS, par.FmaxS);
    }
    if(par.flags_minmax.bitFlags.fl_text_grf) 
    {
        old_par.textGrf = m_textGrf;
        m_textGrf = par.textGrf;
    }
    if(par.flags_minmax.bitFlags.fl_min_max) DATA.SetPosViewMarkersDef();
    return old_par;
}

GRF_MINMAX CFieldGraph::GetParMinMax(int num_grf)
{
    GRF_MINMAX old_par;
    VFRingBuf<F>* pBuf = DATA.SelectBuf(num_grf);
    if(pBuf == NULL) return old_par;
    pBuf->GetMinMax(old_par.Fmin, old_par.Fmax);
    pBuf->GetMinMaxDiapazon(old_par.FminS, old_par.FmaxS);
    old_par.textGrf = m_textGrf;
    return old_par;
}

void CFieldGraph::ChangeScale(FRingBuf<F>* pBuff, GRF_MINMAX& par)
{
	if(m_hWnd == NULL) return;
    if(par.flags_minmax.bitFlags.fl_buf_x) DATA.SetExternalBufX(0, *pBuff);
	if(par.flags_minmax.bitFlags.fl_buf_x_only) return;
	SetParMinMax(-1, par);
	if(par.flags_minmax.bitFlags.fl_min_max) m_psclHor->SetMinMax(par.Fmin, par.Fmax);
    GetParent()->SendMessage(PM_CHANGERECTRANGE, 0, NULL);
}
void CFieldGraph::DestroyBuf()
{
    if(pvReper) delete[] pvReper;
    pvReper = NULL;
	DATA.DeleteAllBuffers();
}
void CFieldGraph::SetMinMaxT(const F scFmin, const F scFmax)
{
    DATA.SetMinMax(-1, scFmin, scFmax);
    DATA.SetPosViewMarkersDef();
}

void CFieldGraph::SetMinMaxXDiapazon(const F FminX, const F FmaxX)
{
    VFRingBuf<F>* pMain = DATA.GetBuf(-1);
    if(pMain == NULL) return;
    pMain->SetMinMaxDiapazon(FminX, FmaxX);
    int pos_mark = pMain->GetPosUMarker(2);
    if(pos_mark < 0) return;
    UINT nn = pMain->FindIndex(0, pos_mark, pMain->Fmin);
    if(nn < 0) return;

    pMain->SetPosUMarker(1, nn);
}

void CFieldGraph::GetMinMaxT(F& scFmin, F& scFmax)
{
    DATA.GetMinMax(-1, scFmin, scFmax);
}


void CFieldGraph::SetMinMaxF(F scFmin, F scFmax, int num_grf)
{
    if(m_hWnd == NULL) return;
    VFRingBuf<F>* pBuf = DATA.GetBuf(num_grf);
    if(pBuf == NULL) return;
    DATA.GetBuf(num_grf)->SetMinMaxDiapazon(scFmin, scFmax);
    GetParent()->SendMessage(PM_CHANGERECTRANGE, 0, NULL);
}

void CFieldGraph::SetMinMaxY(F scFmin, F scFmax, int num_grf)
{
    if(m_hWnd == NULL) return;
    VFRingBuf<F>* pBuf = DATA.GetBuf(num_grf);
    if(pBuf == NULL) return;
    DATA.GetBuf(num_grf)->SetMinMax(scFmin, scFmax);
    GetParent()->SendMessage(PM_CHANGERECTRANGE, 0, NULL);
}

void CFieldGraph::GetMinMaxF(F& fmin, F& fmax, int num_grf)
{
    DATA.GetMinMax(num_grf, fmin, fmax);
}

void CFieldGraph::GetMinMaxFDiapazon(F& FminX, F& FmaxX, int num_grf)
{
    DATA.GetMinMaxDiapazon(num_grf, FminX, FmaxX);
}

void CFieldGraph::SetMinMaxFDiapazon(F FminX, F FmaxX, int num_grf)
{
    DATA.SetMinMaxDiapazon(num_grf, FminX, FmaxX);
    Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
// FRAGMENTS
/*
void CFieldGraph::RedrawFragment(CPoint point)
{
if(!m_hWnd) return;

CRect rectDraw;
if(XpointOld == point.x) return;

flagselected = !slctRectOld.PtInRect(point);

if(XpointOld < point.x) direct = true;
else direct = false;

if(direct)
{
if(flagselected)
{
if(XpointOld == slctRectOld.right)
{
slctRect.left = slctRectOld.right;
slctRect.right = point.x;
slctRectPush.left = slctRectPush.right;

rectDraw = slctRect;
slctRectOld = CRect(slctRectOld.TopLeft(), 
CPoint(point.x, slctRectOld.bottom));
}
else
{
slctRect.left = slctRectOld.right;
slctRect.right = point.x;
slctRectPush.left = slctRectOld.left;
slctRectPush.right = slctRectOld.right;

rectDraw = CRect(slctRectPush.TopLeft(), slctRect.BottomRight());
slctRectOld = CRect(CPoint(slctRectOld.right, slctRectOld.top),
CPoint(point.x, slctRectOld.bottom));
}
}
else
{
slctRect.left = slctRect.right;
slctRectPush.left = slctRectOld.left;
slctRectPush.right = point.x;

rectDraw = slctRectPush;
slctRectOld = CRect(CPoint(point.x, slctRectOld.top),
slctRectOld.BottomRight());
}
}
else
{
if(flagselected)
{
slctRect.left = point.x;
slctRect.right = slctRectOld.left;

if(XpointOld == slctRectOld.left) 
{
slctRectPush.left = slctRectPush.right;

rectDraw = slctRect;
slctRectOld = CRect(CPoint(point.x, slctRectOld.top), 
slctRectOld.BottomRight());
}
else
{
slctRectPush.left = slctRectOld.left;
slctRectPush.right = slctRectOld.right;

rectDraw = CRect(slctRect.TopLeft(), slctRectPush.BottomRight());
slctRectOld = CRect(CPoint(point.x, slctRectOld.top), 
CPoint(slctRectOld.left, slctRectOld.bottom));
}
}
else
{
slctRect.left = slctRect.right;
slctRectPush.left = point.x;
slctRectPush.right = slctRectOld.right;

rectDraw = slctRectPush;
slctRectOld = CRect(slctRectOld.TopLeft(), 
CPoint(point.x, slctRectOld.bottom));
}
}

slctRectOld.NormalizeRect();
XpointOld = point.x;
InvalidateRect(&rectDraw, false);
}

int CFieldGraph::GetSizeFragment()
{
return sizeFragm;
}

int CFieldGraph::GetStartFragment()
{
return startFragm;
}

int CFieldGraph::GetFragment(BYTE* pBuf)
{
for(int i = 0; i < sizeFragm; i++)
pBuf[i] = (BYTE)(pData[0][i]*FmaxY[0]);
return sizeFragm;
}

int CFieldGraph::GetFragment(WORD* pBuf)
{
for(int i = 0; i < sizeFragm; i++)
pBuf[i] = (WORD)(pData[0][i]*FmaxY[0]);
return sizeFragm;
}

int CFieldGraph::GetFragment(F* pBuf)
{
for(int i = 0; i < sizeFragm; i++)
pBuf[i] = pData[0][startFragm+i]*FmaxY[0];
return sizeFragm;
}

int CFieldGraph::InsertFragment(int uStartInsert, int uszFragment, BYTE* pBuf)
{
if(!pBuf || uStartInsert < 0 || uszFragment < 0) return sizeData;

int i;
F* pp = pData[0];

if(!sizeData) uStartInsert = 0;
else if(uStartInsert > sizeData - 1) uStartInsert = sizeData - 1;

pData[0] = (F*)new F[sizeData + uszFragment];
if (!pData[0])
{
TRACE0("Not enough memory.\n");
return 0;
}
sizeData += uszFragment;

if(pp) for(i = 0; i < uStartInsert; i++) pData[0][i] = pp[i]*FmaxY[0];
for(i = 0; i < uszFragment; i++) pData[0][uStartInsert+i] = pBuf[i];
if(pp) for(i = uStartInsert + uszFragment; i < sizeData; i++) 
pData[0][i] = pp[i-uszFragment]*FmaxY[0];

ScaleFY = 1.0f;
FminXView = FminX;
FmaxXView = FminX + (FmaxX - FminX)/ScaleFY;

if(pp == NULL) 
{
ScaleFX = 1.0f;
startDataView	= 0;
endDataView		= (sizeData - 1)/ScaleFX;
}

if(pp) delete pp;
if(!m_hWnd) Invalidate();
return sizeData;
}

int CFieldGraph::InsertFragment(int uStartInsert, int uszFragment, WORD* pBuf)
{
if(!pBuf || uStartInsert < 0 || uszFragment < 0) return sizeData;

int i;
F* pp = pData[0];

if(!sizeData) uStartInsert = 0;
else if(uStartInsert > sizeData - 1) uStartInsert = sizeData - 1;

pData[0] = (F*)new F[sizeData + uszFragment];
if (!pData[0])
{
TRACE0("Not enough memory.\n");
return 0;
}
sizeData += uszFragment;

if(pp) for(i = 0; i < uStartInsert; i++) pData[0][i] = pp[i]*FmaxY[0];
for(i = 0; i < uszFragment; i++) pData[0][uStartInsert+i] = pBuf[i];
if(pp) for(i = uStartInsert + uszFragment; i < sizeData; i++) 
pData[0][i] = pp[i-uszFragment]*FmaxY[0];

ScaleFY = 1.0f;
FminXView = FminX;
FmaxXView = FminX + (FmaxX - FminX)/ScaleFY;

if(pp == NULL) 
{
ScaleFX = 1.0f;
startDataView	= 0;
endDataView		= (sizeData - 1)/ScaleFX;
}

if(pp) delete pp;
if(!m_hWnd) Invalidate();
return sizeData;
}

int CFieldGraph::InsertFragment(int uStartInsert, int uszFragment, F* pBuf)
{
if(!pBuf || uStartInsert < 0 || uszFragment < 0) return sizeData;

int i;
F* pp = pData[0];

if(!sizeData) uStartInsert = 0;
else if(uStartInsert > sizeData - 1) uStartInsert = sizeData - 1;

pData[0] = (F*)new F[sizeData + uszFragment];
if (!pData[0])
{
TRACE0("Not enough memory.\n");
return 0;
}
sizeData += uszFragment;

if(pp) for(i = 0; i < uStartInsert; i++) pData[0][i] = pp[i]*FmaxY[0];
for(i = 0; i < uszFragment; i++) pData[0][uStartInsert+i] = pBuf[i];
if(pp) for(i = uStartInsert + uszFragment; i < sizeData; i++) 
pData[0][i] = pp[i-uszFragment]*FmaxY[0];

FmaxY[0] = NormData(sizeData, pData[0]);
FminX = -FmaxY[0];
FmaxX = FmaxY[0];

ScaleFY = 1.0f;
FminXView = FminX;
FmaxXView = FminX + (FmaxX - FminX)/ScaleFY;

if(pp == NULL) 
{
ScaleFX = 1.0f;
startDataView	= 0;
endDataView		= (sizeData - 1)/ScaleFX;
}

if(pp) delete pp;
if(!m_hWnd) Invalidate();
return sizeData;
}


int CFieldGraph::DeleteFragment(int uStartDelete, int uszFragment)
{
if(uStartDelete < 0 || uszFragment < 0 || uStartDelete > sizeData - 1) return sizeData;
if(!sizeData || !pData[0]) return sizeData;
if(uStartDelete + uszFragment > sizeData - 1) uszFragment = sizeData - uStartDelete;

int i;
F* pp = pData[0];
pData[0] = (F*)new F[sizeData - uszFragment];
if (!pData[0])
{
TRACE0("Not enough memory.\n");
return 0;
}
sizeData -= uszFragment;

for(i = 0; i < uStartDelete; i++) pData[0][i] = pp[i]*FmaxY[0];
for(i = uStartDelete; i < sizeData; i++) pData[0][i] = pp[i+uszFragment]*FmaxY[0];

if(pp) delete pp;

FmaxY[0] = NormData(sizeData, pData[0]);
FminX = -FmaxY[0];
FmaxX = FmaxY[0];
ScaleFY = 1.0f;
FminXView = FminX;
FmaxXView = FminX + (FmaxX - FminX)/ScaleFY;

if(!m_hWnd) Invalidate();
return sizeData;
}

int CFieldGraph::DeleteFragment()
{
int uStartDelete = startFragm, uszFragment = sizeFragm;

if(uStartDelete < 0 || uszFragment < 0 || uStartDelete > sizeData - 1) return sizeData;
if(!sizeData || !pData[0]) return sizeData;
if(uStartDelete + uszFragment > sizeData - 1) uszFragment = sizeData - uStartDelete;

int i;
F* pp = pData[0];
pData[0] = (F*)new F[sizeData - uszFragment];
if (!pData[0])
{
TRACE0("Not enough memory.\n");
return 0;
}

sizeData -= uszFragment;

for(i = 0; i < uStartDelete; i++) pData[0][i] = pp[i]*FmaxY[0];
for(i = uStartDelete; i < sizeData; i++) pData[0][i] = pp[i+uszFragment]*FmaxY[0];

if(pp) delete pp;

FmaxY[0] = NormData(sizeData, pData[0]);
FminX = -FmaxY[0];
FmaxX = FmaxY[0];
ScaleFY = 1.0f;
FminXView = FminX;
FmaxXView = FminX + (FmaxX - FminX)/ScaleFY;

if(!m_hWnd) Invalidate();
return sizeData;
}



F CFieldGraph::HFullZoom()
{
F hg = ScaleFX;
SavePar();

ScaleFX = 1.0f;
startDataView	= 0;
endDataView		= (sizeData - 1)/ScaleFX;

if(!m_hWnd) Invalidate();
return hg;
}

F CFieldGraph::VFullZoom()
{
F vg = ScaleFY;
SavePar();

ScaleFY = 1.0f;
FminXView = FminX;
FmaxXView = FminX + (FmaxX - FminX)/ScaleFY;

if(!m_hWnd) Invalidate();
return vg;
}
*/

void CFieldGraph::UpdateViewRange()
{
    VFRingBuf<F>* pMain = DATA.GetBuf(-1);
    if(pMain == NULL) return;
    int prev_pos_um1 = pMain->GetPosUMarker(1); 
	pMain->SetPosUMarker23ToEndBuf();
    CClientDC dc(this);
    DrawGraph(&dc, this);

    pMain->SetPosUMarker(1, prev_pos_um1); 
	pMain->SetPosUMarker23ToEndBuf();
}
CSladeBar::CSladeBar()
{
}

CSladeBar::~CSladeBar()
{
}

BEGIN_MESSAGE_MAP(CSladeBar, CStatic)
    //{{AFX_MSG_MAP(CSladeBar)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSladeBar::OnPaint() 
{
    CPaintDC dc(this); 
    CRect slideRec;
    GetClientRect(&slideRec);
    dc.DrawEdge(&slideRec, EDGE_SUNKEN,BF_RECT);
}

IMPLEMENT_DYNCREATE(CGraph, CStatic)

CGraph::CGraph()
{
	m_pDCMem = NULL;
	m_pBitmap = NULL;
	m_pbitmapOld = NULL;

	ResetStyle(0xffffffffffffffff);
    colorFon = GetSysColor(COLOR_MENU);
    colorFontX = RGB(255,0,0);
    colorFontY = RGB(255,0,0);

    sNameX = "Ось X";
    sNameY = "Ось Y";
    szSlotName = CSize(3, 3);

    sclVert = CScale("Axis Y", 5);
    sclVert.SetColorFon(colorFon);

    sclHor  = CScale("Axis X", 5);
    sclHor.SetColorFon(colorFon);

    FieldGraph = CFieldGraph(10, 5);
    FieldGraph.m_psclVert = &sclVert;
    FieldGraph.m_psclHor = &sclHor;

    szHorScroll.cy = szVertScroll.cx = 10;
    szSlot1 = CSize(2,2);
    szSlot2 = CSize(0,3);
    szSlot3 = CSize(2,2);
    szSlot4 = CSize(2,1);

    LOGFONT logfontX = {
        -11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    LOGFONT  logfontY = {
        -11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    LOGFONT  logfontD = {
        -11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    logfontNX  = logfontX;
    logfontNY  = logfontY;
    logfontDgt = logfontD;

    colorHeadX = colorHeadY = RGB(255,255,255);

    flagEdit = false;

	tuning.pGraph = this;
	tuning.pFieldGraph = &FieldGraph;
	tuning.pHScale = &sclHor;
	tuning.pVScale = &sclVert;
	tuning.pScrollBarX = &sbarHor0;
	tuning.pScrollBarY = &sbarVert0;
	size_check = CSize(15, 15);
	rec_tuning = CRect(0,0,0,0);
    loc_data_for_print = 0;
}

CGraph::~CGraph()
{
	if (m_pDCMem != NULL)
	{
		ASSERT(m_pbitmapOld != NULL);
		delete m_pDCMem;
		delete m_pBitmap;
	}
	else
	{
		ASSERT(m_pbitmapOld == NULL);
		ASSERT(m_pBitmap == NULL);
	}
}

CGraph::CGraph(CString NameX, CString NameY, UINT NstrichX, UINT NstrichY, ULONG64 style)
{
	m_pDCMem = NULL;
	m_pBitmap = NULL;
	m_pbitmapOld = NULL;

	int NSX, NSY;
	CString nameX, nameY;
	sNameX = NameX;
	sNameY = NameY;
	nameX = NameX, nameY = NameY;
	NSX = NstrichX, NSY = NstrichY;

	sclVert = CScale(nameY, NSY);
	sclHor  = CScale(nameX, NSX);
	FieldGraph = CFieldGraph(NstrichX, NstrichY);

	ResetStyle(0xffffffffffffffff);
	SetStyle(style);

    szSlotName = CSize(3, 3);
    colorFontX = RGB(0,0,0);
    colorFontY = RGB(0,0,0);

	BOOL flgOrient = GetBitStyle(GRF_ORIENT_FLD);
	SubstituteBitStyle(SCLY_ORIENT, !flgOrient);
	SubstituteBitStyle(SCLY_ORIENT_INVERT, flgOrient);

	SubstituteBitStyle(SCLX_ORIENT, flgOrient);
	SubstituteBitStyle(SCLX_ORIENT_INVERT, false);
	SubstituteBitStyle(SCLX_TYPE_SCALE, true);

	SubstituteBitStyle(FLD_ORIENT, flgOrient);
    FieldGraph.m_psclVert = &sclVert;
    FieldGraph.m_psclHor = &sclHor;

    colorFon = RGB(255,255,255);

    szHorScroll.cy = szVertScroll.cx = 10;
    szSlot1 = CSize(2,2);
    szSlot2 = CSize(0,3);
    szSlot3 = CSize(2,2);
    szSlot4 = CSize(2,1);

    LOGFONT logfontX = {
        -11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    LOGFONT  logfontY = {
        -11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    LOGFONT  logfontD = {
        -11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

    logfontNX  = logfontX;
    logfontNY  = logfontY;
    logfontDgt = logfontD;

    colorHeadX = colorHeadY = RGB(255,255,255);
    flagEdit = false;

	tuning.pGraph = this;
	tuning.pFieldGraph = &FieldGraph;
	tuning.pHScale = &sclHor;
	tuning.pVScale = &sclVert;
	tuning.pScrollBarX = &sbarHor0;
	tuning.pScrollBarY = &sbarVert0;
	size_check = CSize(15, 15);
	rec_tuning = CRect(0,0,0,0);
    loc_data_for_print = 0;
}

CGraph& CGraph::operator=(CGraph& grf)
{
	m_pDCMem = NULL;
	m_pBitmap = NULL;
	m_pbitmapOld = NULL;

	sclVert = grf.sclVert;
	sclHor	= grf.sclHor;
	FieldGraph = grf.FieldGraph;

	ResetStyle(0xffffffffffffffffL);
	SetStyle(grf.graphStyle.dwFlags);

    sNameX = grf.sNameX;
    sNameY = grf.sNameY;

    colorFontX = grf.colorFontX;
    colorFontY = grf.colorFontY;
    colorHeadX = grf.colorHeadX;
    colorHeadY = grf.colorHeadY;

    logfontNX  = grf.logfontNX;
    logfontNY  = grf.logfontNY;
    logfontDgt = grf.logfontDgt;

    flagEdit = grf.flagEdit;
    loc_data_for_print = 0;
    return *this;
}

BEGIN_MESSAGE_MAP(CGraph, CStatic)
    //{{AFX_MSG_MAP(CGraph)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_MESSAGE(PM_CHANGERECTRANGE, OnChangeRange)
    ON_WM_SIZE()
    ON_WM_MOVE()
    ON_MESSAGE(PM_SETSCALETIME, OnSetScaleTime)
    ON_MESSAGE(PM_OUTDIGITGRAPH, OnOutDigData)
    ON_MESSAGE(PM_EDITPOINTGRAPH, OnEditPointGraph)
	ON_MESSAGE(PM_SHOW_PANEL, OnShowPanel)
    ON_WM_DESTROY()
    ON_MESSAGE(PM_R_BUTTON_DOWN, OnRButtonDown1)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CGraph::PreparePrintingParam(CDC* pDC, CWnd *pWnd, BOOL falagSize)
{
    if(!pWnd) return;

    int Lpg, wx, wy; 
    int shift = 20; 

    Lpg = pDC->GetDeviceCaps(LOGPIXELSX);
    wx = pDC->GetDeviceCaps(HORZSIZE) - 2*shift;
    wy = pDC->GetDeviceCaps(VERTSIZE) - 2*shift;

    CRect rec;
    if(falagSize) GetClientRect(&rec);
    else pWnd->GetClientRect(&rec);

    F  vr1 = 25.4 * rec.Width();
    if (vr1 != 0) sclX = (F)((wx * Lpg)/vr1); 

    vr1 = 25.4 * rec.Height();
    if (vr1 != 0) sclY = (F)((wy * Lpg)/vr1);

    if(sclX > sclY) sclX = sclY;
    else sclY = sclX;

    prnShift.cx = long(shift*(Lpg/25.4f));
    prnShift.cy = long(shift*(Lpg/25.4f));
}

int CGraph::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CStatic::OnCreate(lpCreateStruct) == -1)
        return -1;

    ModifyStyle(0, SS_NOTIFY);
    cursorNorm = LoadCursor(NULL, IDC_ARROW);
    cursorHand = LoadCursor(NULL, IDC_SIZEWE);

    CRect recHscl = CRect(0,0,0,0);

    sclVert.Create(NULL, WS_CHILD|SS_NOTIFY, recVertScl, this);
	if(GetBitStyle(SCLY_VIEW)) sclVert.ShowWindow(SW_SHOW);
	else sclVert.ShowWindow(SW_HIDE);

	sclHor.Create(NULL, WS_CHILD|SS_NOTIFY, recHorScl, this);
	if(GetBitStyle(SCLX_VIEW)) sclHor.ShowWindow(SW_SHOW);
	else sclHor.ShowWindow(SW_HIDE);

    if(fFontNameX.m_hObject) fFontNameX.DeleteObject();
    if(fFontNameY.m_hObject) fFontNameY.DeleteObject();
    if(fFontDgt.m_hObject) fFontDgt.DeleteObject();
    fFontNameX.CreateFontIndirect(&logfontNX);
    fFontNameY.CreateFontIndirect(&logfontNY);
    fFontDgt.CreateFontIndirect(&logfontDgt);

    FieldGraph.Create(NULL, WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|SS_NOTIFY, recGraphFld, this);

    FieldGraph.hWndGraph = this->m_hWnd;
    FieldGraph.pGraph = this;

    BOOL flgOrient = GetBitStyle(GRF_ORIENT_FLD);
    recSlideHorBar = recSlideVertBar = recHorScroll = recVertScroll = CRect(0, 0, 0, 0);
    stSlideHorBar.Create(NULL, WS_CHILD, recSlideHorBar, this);
    stSlideVertBar.Create(NULL, WS_CHILD, recSlideVertBar, this);

    sbarHor0.Create(SBS_HORZ|WS_CHILD|SS_NOTIFY, recHorScroll, this, 20000);
	if(GetBitStyle(GRF_X_SCROLL)) sbarHor0.ShowWindow(SW_SHOW);

    sbarVert0.Create(SBS_VERT|WS_CHILD|SS_NOTIFY, recVertScroll, this, 20001);
	if(GetBitStyle(GRF_Y_SCROLL)) sbarVert0.ShowWindow(SW_SHOW);

    SetFlagXScroll(SetFlagYScroll(GetFlagXScroll()));

    if(!flgOrient)
    {
        p_ScrollBarX = &sbarHor0;
        p_ScrollBarY = &sbarVert0;
    }
    else
    {
        p_ScrollBarX = &sbarVert0;
        p_ScrollBarY = &sbarHor0;
    }

    sclVert.m_fldGraph = sclHor.m_fldGraph = &FieldGraph;
//--------------------------------------------------------------------------------
    SubstituteBitStyle(SCLX_ORIENT_FONT_M, GetBitStyle(SCLX_ORIENT_FONT_M));
    SubstituteBitStyle(SCLX_ORIENT_FONT_N, GetBitStyle(SCLX_ORIENT_FONT_N));
    sclHor.UpdateOrientFont();

    SubstituteBitStyle(SCLY_ORIENT_FONT_M, GetBitStyle(SCLY_ORIENT_FONT_M));
    SubstituteBitStyle(SCLY_ORIENT_FONT_N, GetBitStyle(SCLY_ORIENT_FONT_N));
    sclVert.UpdateOrientFont();
//--------------------------------------------------------------------------------

	tuning.Create(NULL, WS_CHILD|SS_NOTIFY, recHscl, this);
    ChangePosition();
    Invalidate(true);
    return 0;
}

void CGraph::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {	// storing code
        ar << sNameX;
        ar << sNameY;
        ar << colorFontX;
        ar << colorFontY;
//        ar << graphStyle.dwFlags;
        ar << colorFon;
//        ar << logfontNX;
  //      ar << logfontNY;
    //    ar << logfontDgt;
        ar << colorHeadX;
        ar << flagEdit;
    }
    else
    {	// loading code
        ar >> sNameX;
        ar >> sNameY;
        ar >> colorFontX;
        ar >> colorFontY;
//        ar >> graphStyle.dwFlags;
        ar >> colorFon;
//        ar >> logfontNX;
  //      ar >> logfontNY;
    //    ar >> logfontDgt;
        ar >> colorHeadX;
        ar >> flagEdit;
    }

    FieldGraph.Serialize(ar);
    sclVert.Serialize(ar);
    sclHor.Serialize(ar);
}

BOOL CGraph::SetFlagXScroll(BOOL flg)
{
    BOOL fl = GetFlagXScroll();
    if(!GetBitStyle(GRF_ORIENT_FLD)) SubstituteBitStyle(GRF_X_SCROLL, flg);
    else SubstituteBitStyle(GRF_Y_SCROLL, flg);
    return fl;
}

BOOL CGraph::SetFlagYScroll(BOOL flg)
{
    BOOL fl = GetFlagYScroll();
	if(!GetBitStyle(GRF_ORIENT_FLD)) SubstituteBitStyle(GRF_Y_SCROLL, flg);
	else SubstituteBitStyle(GRF_X_SCROLL, flg);
    return fl;
}

BOOL CGraph::GetFlagXScroll()
{
    if(!GetBitStyle(GRF_ORIENT_FLD)) return GetBitStyle(GRF_X_SCROLL);
    else return GetBitStyle(GRF_Y_SCROLL);
}

BOOL CGraph::GetFlagYScroll()
{
	if(!GetBitStyle(GRF_ORIENT_FLD)) return GetBitStyle(GRF_Y_SCROLL);
	else return GetBitStyle(GRF_X_SCROLL);
}

LRESULT CGraph::OnChangeRange(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    if(p_ScrollBarX->m_hWnd == NULL || p_ScrollBarY->m_hWnd == NULL) return false;

    int nPos;
    SCROLLINFO sScrollInfo;

    SetRangeA();
    //.............................................................................
    p_ScrollBarX->GetScrollInfo(&sScrollInfo, SIF_ALL);
    nPos = FieldGraph.GetSrollXPos(MAXSCROLLX - sScrollInfo.nPage + 1);
    p_ScrollBarX->SetScrollPos(nPos, true);
    FieldGraph.DATA.GetMinMax(-1, sclHor.Fmin, sclHor.Fmax);
    //.............................................................................
    p_ScrollBarY->GetScrollInfo(&sScrollInfo, SIF_ALL);
    nPos = FieldGraph.GetSrollYPos(MAXSCROLLX - sScrollInfo.nPage + 1);
    p_ScrollBarY->SetScrollPos(nPos);
    FieldGraph.DATA.GetMinMax(0, sclVert.Fmin, sclVert.Fmax);

	if(FlagChangeScaleRedraw)
	{
		sclHor.Invalidate();
		sclVert.Invalidate();
		FieldGraph.Invalidate();
	}

	if(FieldGraph.indexList > 0) 
	{
		if(!GetBitStyle(GRF_ORIENT_FLD)) SubstituteBitStyle(GRF_Y_SCROLL, true);
		else SubstituteBitStyle(GRF_X_SCROLL, true);

		RecalcSize();
		ChangePosition();
	}

    return true;
}

LRESULT CGraph::OnSetScaleTime(WPARAM wParam, LPARAM lParam)
{
    if(!m_hWnd) return false;

    HWND hsclWnd = GetParent()->m_hWnd; 
    ::SendMessage(hsclWnd, PM_SETSCALETIME, (WPARAM)(m_hWnd), lParam);
    return true;
}

LRESULT CGraph::OnOutDigData(WPARAM wParam, LPARAM lParam)
{
    return false;
    if(!GetBitStyle(GRF_ENABLE_DGT)) return false;

    if(wParam < 3) return true;
    DigitParam* pParam = (DigitParam*)lParam;

    F fV = pParam->fValue;
    F dTime = pParam->time;
    BOOL bType = pParam->bflData;
    //-----------------------------------------------------------------------
    CDC* pDC = GetDC();

    CRect grfRec, fieldRec;
    GetClientRect(&grfRec);
    FieldGraph.GetClientRect(&fieldRec);

    FieldGraph.ClientToScreen(&fieldRec);
    ScreenToClient(&fieldRec);

    grfRec.bottom -=5;
    grfRec.top += (grfRec.Height() - 18);

    grfRec.left = fieldRec.left;
    grfRec.right = fieldRec.right;

    pDC->DrawEdge(&grfRec, EDGE_SUNKEN,BF_RECT);

    //---------------------------------------------------------------
    CString str, str1;

    if(bType)
    {
        dleta.m_span = dTime;
        if(dleta.m_span > MAX_DATE || dleta.m_span < MIN_DATE) return false;

        str = dleta.Format("%H:%M:%S"); 
    }
    else str.Format("%.2f", dTime); 
    //	str = dleta.Format("%H:%M"); 
    str1.Format("%0.2f", fV); 

    str += "  ";
    str += str1;

    COLORREF clr = GetColorHaderAlert();
    CFont *pFont = pDC->SelectObject(&fFontDgt);
    ASSERT(pFont);

    grfRec.InflateRect(-2,-2);
    pDC->FillSolidRect(&grfRec, clr);
    pDC->IntersectClipRect(&grfRec);

    pDC->SetTextColor(RGB(0,0,255));
    pDC->SetBkColor(clr);
    pDC->TextOut(grfRec.left + 4, grfRec.top, str);

    pDC->SelectObject(pFont);
    //---------------------------------------------------------------
    ReleaseDC(pDC);
    return true;
}

LRESULT CGraph::OnEditPointGraph(WPARAM wParam, LPARAM lParam)
{
    if(!m_hWnd) return false;

	if(wParam == 150258)
    {
        FieldGraph.flag_scroll = !FieldGraph.flag_scroll;
        if(!GetBitStyle(GRF_ORIENT_FLD)) SubstituteBitStyle(GRF_X_SCROLL, FieldGraph.flag_scroll);
        else SubstituteBitStyle(GRF_Y_SCROLL, FieldGraph.flag_scroll);
        RecalcSize();
        ChangePosition();
        Invalidate(FALSE);
    }

    flagEdit = true;
    GetParent()->PostMessage(PM_EDITPOINT, wParam, UINT(FieldGraph.flag_scroll));
    return true;
}

LRESULT CGraph::OnShowPanel(WPARAM wParam, LPARAM lParam)
{
	SubstituteBitStyle(GRF_PANEL, !GetBitStyle(GRF_PANEL));
	ChangePosition();
	Invalidate(FALSE);
	GetParent()->SendMessage(PM_SHOW_PANEL, num_self, GetBitStyle(GRF_PANEL));
	return true;
}
void CGraph::RecalcSize()
{
    if(!m_hWnd) return;

    CSize szGraphFld, sizeTX, sizeTY;
    CRect graphRec, fieldRec;
    int deflBottom, deflRight, cx, cy;

    GetClientRect(&graphRec);
    CRect inflateSizeH(0,0,0,0);
    CRect inflateSizeV(0,0,0,0);
    if(sclVert.IsShowScl()) inflateSizeV = sclVert.GetInflateSize();
    if(sclHor.IsShowScl()) inflateSizeH = sclHor.GetInflateSize();

    GetInflateSize(sizeTX, sizeTY);
    if(GetBitStyle(GRF_X_SCROLL)) cy = szHorScroll.cy;
    else cy = 0;

    if(GetBitStyle(GRF_Y_SCROLL)) cx = szVertScroll.cx;
    else cx = 0;

    if(GetBitStyle(GRF_X_SCROLL)) 
    {
        deflBottom = cy + szSlot1.cy;
        recHorScroll = CRect(graphRec.left, graphRec.bottom - cy, graphRec.right, graphRec.bottom);
        recHorScroll.DeflateRect(szSlot1.cx, 0, cx + szSlot1.cx, 0);
        recHorScroll.OffsetRect(0, -szSlot1.cy);
    }
    else deflBottom = 0;

    if(GetBitStyle(GRF_Y_SCROLL)) 
    {
        deflRight = szVertScroll.cx + szSlot1.cx;
        recVertScroll = CRect(graphRec.right - szVertScroll.cx, graphRec.top, graphRec.right, graphRec.bottom);
        recVertScroll.DeflateRect(0, szSlot1.cy, 0, cy + szSlot1.cy);
        recVertScroll.OffsetRect(-szSlot1.cx, 0);
    }
    else deflRight = 0;

    if(!GetBitStyle(GRF_ORIENT_FLD))
    {
        CSize inflateField = CSize(sizeTY.cx, sizeTX.cy);

        recGraphFld = CRect(__max(inflateSizeV.left, inflateSizeH.top) + 2 + inflateField.cx, 
            inflateSizeV.right, 
            graphRec.right - __max(inflateSizeH.right, int(size_check.cx * (int)GetBitStyle(GRF_PANEL))),
            graphRec.bottom - __max(inflateSizeH.left, inflateSizeV.top) - 2 - inflateField.cy);

        recGraphFld.DeflateRect(0, 0, deflRight, deflBottom);

        recHorScl  = CRect(recGraphFld.left, recGraphFld.bottom, recGraphFld.right,  
            graphRec.bottom - 2 - inflateField.cy);
        recHorScl.DeflateRect(0, 0, 0, deflBottom);

        recVertScl = CRect(2 + inflateField.cx, recGraphFld.top, recGraphFld.left, recGraphFld.bottom);
    }
    else
    {
        CSize inflateField = CSize(sizeTX.cx, sizeTY.cy);

        recGraphFld = CRect(__max(inflateSizeH.left, inflateSizeV.top) + 2 + inflateField.cx, 
            __max(inflateSizeV.left, inflateSizeH.right) + 2 + inflateField.cy, 
            graphRec.right - 2 - __max(inflateSizeV.right, int(size_check.cx * (int)GetBitStyle(GRF_PANEL))),
            graphRec.bottom - inflateSizeH.top);


        recGraphFld.DeflateRect(0, 0, deflRight, deflBottom);

        recHorScl = CRect(inflateField.cx + 2, recGraphFld.top, recGraphFld.left, recGraphFld.bottom);
        recVertScl  = CRect(recGraphFld.left, graphRec.top + 2 + inflateField.cy, recGraphFld.right, recGraphFld.top);
    }

	rec_tuning = CRect(recGraphFld.right+2, recGraphFld.top, recGraphFld.right+size_check.cx, recGraphFld.bottom);
	tuning.RecalcSize();
}


void CGraph::ChangePosition() 
{
	RecalcSize();
    if(FieldGraph.m_hWnd == NULL) return;
    FieldGraph.SetWindowPos(&wndBottom, recGraphFld.left, recGraphFld.top, recGraphFld.Width(), 
        recGraphFld.Height(), SWP_SHOWWINDOW);
    CRect rec;
    FieldGraph.GetClientRect(&rec);


    if(stSlideVertBar.m_hWnd == NULL) return;
    if(p_ScrollBarY->m_hWnd == NULL) return;

    if(GetBitStyle(GRF_X_SCROLL))
    {
        stSlideHorBar.SetWindowPos(&wndBottom, recSlideHorBar.left, recSlideHorBar.top, 
            recSlideHorBar.Width(), recSlideHorBar.Height(), SWP_SHOWWINDOW);
        stSlideHorBar.ShowWindow(SW_SHOW);

        sbarHor0.SetWindowPos(&wndBottom, recHorScroll.left, recHorScroll.top, recHorScroll.Width(), 
            recHorScroll.Height(), SWP_SHOWWINDOW);
    }	
    else
    {
        stSlideHorBar.ShowWindow(SW_HIDE);
        sbarHor0.ShowWindow(SW_HIDE);
    }

    if(GetBitStyle(GRF_Y_SCROLL))
    {
        stSlideVertBar.SetWindowPos(&wndBottom, recSlideVertBar.left, recSlideVertBar.top, 
            recSlideVertBar.Width(), recSlideVertBar.Height(), SWP_SHOWWINDOW);
        stSlideVertBar.ShowWindow(SW_SHOW);

        sbarVert0.SetWindowPos(&wndBottom, recVertScroll.left, recVertScroll.top, 
            recVertScroll.Width(), recVertScroll.Height(), SWP_SHOWWINDOW);
    }
    else
    {
        stSlideVertBar.ShowWindow(SW_HIDE);
        sbarVert0.ShowWindow(SW_HIDE);
    }

	if(sclHor.m_hWnd != NULL)
	{
		if(GetBitStyle(SCLX_VIEW))
		{
			sclHor.SetWindowPos(&wndBottom, recHorScl.left, recHorScl.top, recHorScl.Width(), 
				recHorScl.Height(), SWP_SHOWWINDOW);
		}
		else sclHor.ShowWindow(SW_HIDE);
	}

    CRect rec2;
    sclHor.GetClientRect(&rec2);

	if(sclVert.m_hWnd != NULL)
	{
		if(GetBitStyle(SCLY_VIEW))
		{
			sclVert.SetWindowPos(&wndBottom, recVertScl.left, recVertScl.top, recVertScl.Width(), 
				recVertScl.Height(), SWP_SHOWWINDOW);
		}
		else sclVert.ShowWindow(SW_HIDE);
	}

    sclHor.CalcScaleStrichX(NULL);
    sclVert.CalcScaleStrichY(NULL);

	if(!GetBitStyle(GRF_PANEL)) tuning.ShowWindow(SW_HIDE);
	else tuning.SetWindowPos(&wndBottom, rec_tuning.left, rec_tuning.top, rec_tuning.Width(), rec_tuning.Height(), SWP_SHOWWINDOW);
}

BOOL CGraph::CreateMemoryPaintDC(CDC* pDC, CDC* pDCMem)
{
	CPaintDC* pDCPaint = (CPaintDC*)pDC;
	rectClip = pDCPaint->m_ps.rcPaint;
	int cxClip = rectClip.Width();
	int cyClip = rectClip.Height();
	if(cxClip <= 0 || cyClip <= 0) return false;

	if (m_pDCMem != NULL)
	{
		ASSERT(m_pbitmapOld != NULL);
		m_pBitmap->DeleteObject();
		delete m_pDCMem;
		delete m_pBitmap;
	}
	else
	{
		ASSERT(m_pbitmapOld == NULL);
		ASSERT(m_pBitmap == NULL);
	}

	m_pDCMem = new CDC();
	ASSERT(m_pDCMem != NULL);
	VERIFY(m_pDCMem->CreateCompatibleDC(pDC));
	m_pBitmap = new CBitmap();

	VERIFY(m_pBitmap->CreateCompatibleBitmap(pDC, cxClip, cyClip));
	m_pbitmapOld  = m_pDCMem->SelectObject(m_pBitmap);
	return true;
}
void CGraph::DrawGrf(CDC* pDC, CWnd *pWnd)
{
    if(!m_hWnd) return;

    CRect fieldRec;
    GetClientRect(&fieldRec);

    if(pDC->IsPrinting()) 
    {
        if(!pWnd) return;
		sclX = prn.k_x;
		sclY = prn.k_y;
        ClientToScreen(&fieldRec);
        Daraw2Rect(pDC, fieldRec);

        FieldGraph.DrawGrf(pDC, pWnd, sclX, sclY, &prnShift);
        sclVert.DrawGrf(pDC, pWnd, sclX, sclY, &prnShift);
        sclHor.DrawGrf(pDC, pWnd, sclX, sclY, &prnShift);
    }
    else
    {
		if(!CreateMemoryPaintDC(pDC, m_pDCMem)) return;
		CRect rec;
		pDC->GetClipBox(&rec);
		m_pDCMem->GetClipBox(&rec);
		m_pDCMem->FillSolidRect(&fieldRec, colorFon);
        if(GetBitStyle(GRF_RAMKA_AR_GRF)) m_pDCMem->DrawFocusRect(&fieldRec);
        OutTextName(m_pDCMem);
        DrawRectsUnDo(m_pDCMem);
        pDC->BitBlt(0,0,fieldRec.Width(),fieldRec.Height(),m_pDCMem,0,0,SRCCOPY);
    }
}

void CGraph::DrawGrf(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift)
{
    if(!m_hWnd) return;
    CRect fieldRec;
    GetClientRect(&fieldRec);
	CRect rec = fieldRec;
	RecalcSize();
	ChangePosition();
	ClientToScreen(&fieldRec);

    if(pDC->IsPrinting()) 
    {
        if(!pWnd) return;
		sclVert.SetPrintFontP(sclX);
		sclHor.SetPrintFontP(sclX);
		ChangePosition();

        Daraw2Rect(pDC, fieldRec);

        FieldGraph.SetNumStrich(61, 11, &fieldRec);
        sclVert.DrawGrf(pDC, pWnd, sclX, sclY, prnShift);
        sclHor.DrawGrf(pDC, pWnd, sclX, sclY, prnShift);
        FieldGraph.DrawGrf(pDC, pWnd, sclX, sclY, prnShift);
        FieldGraph.SetNumStrich(31, 6, &fieldRec);
        sclHor.CalcScaleStrichX(0);
        sclVert.CalcScaleStrichY(0);
    }
    else
    {
        sclHor.CalcScaleStrichX(0);
        sclVert.CalcScaleStrichY(0);

        //		pDC->DrawEdge(&fieldRec, EDGE_SUNKEN,BF_RECT);
        		pDC->FillSolidRect(&fieldRec, colorFon);
    }
}

void CGraph::DrawRectsUnDo(CDC *pDC) 
{
    if(!m_hWnd) return;

    if(FieldGraph.m_hWnd == NULL) return;
    CRect rec, rec1;
    FieldGraph.GetClientRect(&rec);
    FieldGraph.ClientToScreen(&rec);
    ScreenToClient(&rec);

    rec1 = rec;
    rec1.top = rec.bottom - 10;
    rec1.right = rec.left + 20;

    rec1.OffsetRect(-30,15);

    rec = rec1;

    rec.InflateRect(0,0,-11,0);
    rec1.InflateRect(-11,0,0,0);

    RectUnDoF = rec1;
    RectUnDoB = rec;

    pDC->FillSolidRect(&RectUnDoF, RGB(200,200,200));
    pDC->FillSolidRect(&RectUnDoB, RGB(200,200,200));
}

void CGraph::OutTextName(CDC *pDC) 
{
    if(!m_hWnd) return;

    if(sclHor.m_hWnd == NULL || sclVert.m_hWnd == NULL) return;

    CRect grfRec, sclXrec, sclYrec, grfRecClip, textRec;
    GetClientRect(&grfRec);
    grfRecClip = grfRec;
    grfRecClip.DeflateRect(2,2);
    pDC->IntersectClipRect(&grfRecClip);

    CFont *pFont;
    CSize sz, szT;
    int x1, y1;

    if(GetBitStyle(GRF_NAME_X))
    {
        //-------------------------------------------------------------------
        sclHor.GetClientRect(&sclXrec);
        sclHor.ClientToScreen(&sclXrec);
        ScreenToClient(&sclXrec);

        pFont = pDC->SelectObject(&fFontNameX);
        ASSERT(pFont);

        sz = CScale::GetSizeText(pDC, sNameX, &fFontNameX, GetBitStyle(GRF_NAME_X));
        szT = sz;
        GetClientRect(&textRec);

        if(!GetBitStyle(GRF_ORIENT_FLD))
        {
            if(!GetBitStyle(GRF_INV_NAME_X)) sz.cy = 0;
            x1 = sclXrec.left + sclXrec.Width()/2 - sz.cx/2;
            y1 = sclXrec.bottom + szSlotName.cx + sz.cy;
            textRec = CRect(CPoint(textRec.left, y1 - sz.cy), CSize(textRec.Width(), szT.cy));
            textRec.InflateRect(-2, 3);
        }
        else
        {
            if(GetBitStyle(GRF_INV_NAME_X)) sz.cy = -sz.cy;
            x1 = sclXrec.left - sz.cx - szSlotName.cx;
            y1 = sclXrec.top + sclXrec.Height()/2 - sz.cy/2;
            szT = sz;
            textRec = CRect(CPoint(textRec.left, textRec.top),  CSize(szT.cx, textRec.Height()));
            textRec.right += 8;
        }

        if(GetBitStyle(GRF_BTN_X_NAME))
        {
            pDC->FillSolidRect(&textRec, colorHeadX);
            pDC->DrawEdge(&textRec, BDR_RAISEDINNER,BF_RECT);

            pDC->SetBkColor(colorHeadX);
        }
        pDC->SetTextColor(colorFontX);
        pDC->TextOut(x1, y1, sNameX);

        pDC->SelectObject(pFont);
    }

    if(GetBitStyle(GRF_NAME_Y))
    {
        //-------------------------------------------------------------------
        sclVert.GetClientRect(&sclYrec);
        sclVert.ClientToScreen(&sclYrec);
        ScreenToClient(&sclYrec);

        pFont = pDC->SelectObject(&fFontNameY);
        ASSERT(pFont);
        pDC->SetTextColor(colorFontY);

        sz = CScale::GetSizeText(pDC, sNameY, &fFontNameY, GetBitStyle(GRF_NAME_Y));
        szT = sz;
        GetClientRect(&textRec);

        if(!GetBitStyle(GRF_ORIENT_FLD))
        {
            if(GetBitStyle(GRF_INV_NAME_X)) sz.cy = -sz.cy;
            x1 = sclYrec.left - sz.cx - szSlotName.cx;
            y1 = sclYrec.top + sclYrec.Height()/2 + sz.cy/2;
            szT = sz;
            textRec = CRect(CPoint(textRec.left, textRec.top),  CSize(szT.cx, textRec.Height()));
            textRec.right += 8;
        }
        else
        {
            if(GetBitStyle(GRF_INV_NAME_Y)) sz.cy = 0;
            x1 = sclYrec.left + sclYrec.Width()/2 - sz.cx/2;
            y1 = sclYrec.top - szSlotName.cx - sz.cy;
            textRec = CRect(CPoint(textRec.left, y1), CSize(textRec.Width(), szT.cy));
            textRec.InflateRect(-2, 3);
        }

        if(GetBitStyle(GRF_BTN_Y_NAME))
        {
            pDC->FillSolidRect(&textRec, colorHeadY);
            pDC->DrawEdge(&textRec, BDR_RAISEDINNER,BF_RECT);

            pDC->SetBkColor(colorHeadY);
        }

        pDC->SetTextColor(colorFontY);

        if(GetBitStyle(GRF_BTN_Y_NAME))
        {
            grfRecClip.DeflateRect(2,2);
            pDC->IntersectClipRect(&grfRecClip);
            pDC->TextOut(x1, y1, sNameY);
            grfRecClip.InflateRect(2,2);
            pDC->IntersectClipRect(&grfRecClip);
        }
        else pDC->TextOut(x1, y1, sNameY);

        pDC->SelectObject(pFont);
    }
}

void CGraph::OnPaint() 
{
    CPaintDC dc(this); 
    DrawGrf(&dc);
}


void CGraph::OnDestroy() 
{
    CStatic::OnDestroy();

    sclVert.DestroyWindow();
    sclHor.DestroyWindow();

    if(fFontNameX.m_hObject) fFontNameX.DeleteObject();
    if(fFontNameY.m_hObject) fFontNameY.DeleteObject();
    if(fFontDgt.m_hObject) fFontDgt.DeleteObject();

    FieldGraph.DestroyWindow();
    stSlideVertBar.DestroyWindow();
    sbarVert0.DestroyWindow();

    stSlideHorBar.DestroyWindow();
    sbarHor0.DestroyWindow();

    recHorScl = 0;
    recVertScl = 0;
}


void CGraph::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if(!m_hWnd) return;

    SCROLLINFO sScrollInfo;
    int Pos, shift = 0, savPos, deltaPos;

    if(nSBCode == SB_ENDSCROLL) 
    {
        return;
    }
    else
    {
        //		FieldGraph.SetGreed(false);
        pScrollBar->GetScrollInfo(&sScrollInfo, SIF_ALL);
        savPos = pScrollBar->GetScrollPos();
        Pos = nPos;
        pScrollBar->GetScrollInfo(&sScrollInfo, SIF_ALL);
        int nPage = sScrollInfo.nPage;

        int dn = 1;//(sScrollInfo.nMax - nPage + 1)/100;
        switch(nSBCode)
        {
        case SB_THUMBTRACK:
            shift = nPos - Pos;
            Pos += shift;
            break;

        case SB_LINEDOWN:
            shift = dn + pScrollBar->GetScrollPos();
            Pos += shift;
            break;

        case SB_LINEUP:
            shift = - dn + pScrollBar->GetScrollPos();
            Pos += shift;
            break;

        case SB_PAGEDOWN:
            shift = nPage + pScrollBar->GetScrollPos();
            Pos += shift;
            break;

        case SB_PAGEUP:
            shift = - nPage + pScrollBar->GetScrollPos();
            Pos += shift;
            break;
        }

        if(Pos < 0) Pos = 0;

        if(Pos > (sScrollInfo.nMax  - (int)sScrollInfo.nPage + 1)) Pos = sScrollInfo.nMax - (int)sScrollInfo.nPage + 1;
        else if(Pos < sScrollInfo.nMin) Pos = sScrollInfo.nMin;

        deltaPos = Pos - savPos;
        scroll_pos = Pos;
        scroll_max = sScrollInfo.nMax - (int)sScrollInfo.nPage + 1;
        FieldGraph.HSetScroolGraph(Pos, sScrollInfo.nMax - (int)sScrollInfo.nPage + 1);

        pScrollBar->SetScrollPos(Pos, true);
        GetParent()->PostMessage(PM_CHANGESCROLL, num_self, 0);
    }
}

void CGraph::OnHScrollP(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if(pScrollBar == NULL) CStatic::OnHScroll(nSBCode, nPos, pScrollBar);
    else if(nSBCode == SB_ENDSCROLL) return;
    else
    {
        SCROLLINFO sScrollInfo;
        pScrollBar->GetScrollInfo(&sScrollInfo, SIF_ALL);

        int Pos = pScrollBar->GetScrollPos();
        int nPage = sScrollInfo.nPage;
        int shift = 0;
        int dn = (sScrollInfo.nMax - nPage + 1)/100;
        switch(nSBCode)
        {
        case SB_THUMBTRACK:
            shift = nPos - Pos;
            break;

        case SB_LINEDOWN:
            shift = 1;
            break;

        case SB_LINEUP:
            shift = - 1;
            break;

        case SB_PAGEDOWN:
            shift = nPage;
            break;

        case SB_PAGEUP:
            shift = - nPage;
            break;
        }

        Pos += shift;
        if(Pos > sScrollInfo.nMax) Pos = sScrollInfo.nMax;
        else if(Pos < sScrollInfo.nMin) Pos = sScrollInfo.nMin;

        FieldGraph.HSetScroolGraph(Pos, sScrollInfo.nMax);
    }
}

void CGraph::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    SCROLLINFO sScrollInfo;
    int Pos, shift = 0, savPos, deltaPos;

    if(nSBCode == SB_ENDSCROLL) 
    {
        //		FieldGraph.SetGreed(true);
        return;
    }
    else
    {
        pScrollBar->GetScrollInfo(&sScrollInfo, SIF_ALL);
        savPos = pScrollBar->GetScrollPos();
        Pos = nPos;
        pScrollBar->GetScrollInfo(&sScrollInfo, SIF_ALL);
        int nPage = sScrollInfo.nPage;

        int dn = 1;
        switch(nSBCode)
        {
        case SB_THUMBTRACK:
            shift = nPos - Pos;
            Pos += shift;
            break;

        case SB_LINEDOWN:
            shift = dn + pScrollBar->GetScrollPos();
            Pos += shift;
            break;

        case SB_LINEUP:
            shift = - dn + pScrollBar->GetScrollPos();
            Pos += shift;
            break;

        case SB_PAGEDOWN:
            shift = nPage + pScrollBar->GetScrollPos();
            Pos += shift;
            break;

        case SB_PAGEUP:
            shift = - nPage + pScrollBar->GetScrollPos();
            Pos += shift;
            break;
        }

        if(Pos < 0) Pos = 0;

        if(Pos > sScrollInfo.nMax - (int)sScrollInfo.nPage + 1) Pos = sScrollInfo.nMax - (int)sScrollInfo.nPage + 1;
        else if(Pos < sScrollInfo.nMin) Pos = sScrollInfo.nMin;

        deltaPos = Pos - savPos;
        scroll_pos = Pos;
        scroll_max = sScrollInfo.nMax - (int)sScrollInfo.nPage + 1;
        FieldGraph.VSetScroolGraph(Pos, sScrollInfo.nMax - (int)sScrollInfo.nPage + 1);

        pScrollBar->SetScrollPos(Pos, true);
        GetParent()->PostMessage(PM_CHANGESCROLL, num_self, 1);
    }
}

BOOL CGraph::HSetScroolGraphExternal()
{
    BOOL bflOrient = GetBitStyle(GRF_ORIENT_FLD);
    if(!bflOrient) 
    {
        if(p_ScrollBarX == NULL) return false;
        p_ScrollBarX->SetScrollPos(scroll_pos, true);
        return FieldGraph.XSetScrollGraph(scroll_pos, scroll_max);
    }
    else 
    {
        if(p_ScrollBarY == NULL) return false;
        p_ScrollBarY->SetScrollPos(scroll_pos, true);
        return FieldGraph.YSetScrollGraph(scroll_pos, scroll_max);
    }
}
BOOL CGraph::VSetScroolGraphExternal()
{
    BOOL bflOrient = GetBitStyle(GRF_ORIENT_FLD);
    if(!bflOrient) 
    {
        if(p_ScrollBarY == NULL) return false;
        p_ScrollBarY->SetScrollPos(scroll_pos, true);
        return FieldGraph.YSetScrollGraph(scroll_pos, scroll_max);
    }
    else 
    {
        if(p_ScrollBarX == NULL) return false;
        p_ScrollBarX->SetScrollPos(scroll_pos, true);
        return FieldGraph.XSetScrollGraph(scroll_pos, scroll_max);
    }
}
void CGraph::SetScroolGraphExternal(int num_scroll)
{
    if(num_scroll == 0 && GetBitStyle(GRF_X_SCROLL)) HSetScroolGraphExternal();
    else if(num_scroll == 1 && GetBitStyle(GRF_Y_SCROLL)) VSetScroolGraphExternal();
}

void CGraph::OnMouseMove(UINT nFlags, CPoint point) 
{
    if(GetCapture() != this && (RectUnDoF.PtInRect(point) || RectUnDoB.PtInRect(point))) 
    {
        ::SetCursor(cursorHand);
        SetCapture();
    }
    else if(!RectUnDoF.PtInRect(point) && !RectUnDoB.PtInRect(point))
    {
        ::SetCursor(cursorNorm);
        ReleaseCapture();
    }
    CStatic::OnMouseMove(nFlags, point);
}

void CGraph::OnLButtonDown(UINT nFlags, CPoint point) 
{
	BOOL fl1 = RectUnDoB.PtInRect(point);
	BOOL fl2 = RectUnDoF.PtInRect(point);
	if(fl1 || fl2)
	{
		if(fl1) UndoBack();
		else if(fl2) UndoForward();

		if(FieldGraph.indexList > 0) 
		{
			if(!GetBitStyle(GRF_ORIENT_FLD)) SubstituteBitStyle(GRF_Y_SCROLL, true);
			else SubstituteBitStyle(GRF_X_SCROLL, true);
		}
		else
		{
			if(!GetBitStyle(GRF_ORIENT_FLD)) SubstituteBitStyle(GRF_Y_SCROLL, false);
			else SubstituteBitStyle(GRF_X_SCROLL, false);
		}
		RecalcSize();
		ChangePosition();
		Invalidate(FALSE);
	}
    CStatic::OnLButtonDown(nFlags, point);
}
LRESULT CGraph::OnRButtonDown1(WPARAM wParam, LPARAM lParam)
{
    UINT nn = FieldGraph.DATA.GetPosUMarker(-1, 1);
//	TRACE2("\n ------- %d %d", wParam, nn);
    GetParent()->SendMessage(PM_R_BUTTON_DOWN, wParam, nn);
    return true;
}

void CGraph::OnSize(UINT nType, int cx, int cy) 
{
    CStatic::OnSize(nType, cx, cy);

    RecalcSize();
    ChangePosition();

    Invalidate();
}

void CGraph::OnMove(int x, int y) 
{
    CStatic::OnMove(x, y);
    if(m_hWnd) Invalidate(true);
    if(sclVert.m_hWnd) Invalidate(true);
    if(sclHor.m_hWnd) Invalidate(true);
    if(FieldGraph.m_hWnd) Invalidate(true);
}


void CGraph::GetInflateSizeA(CDC* pDC, CSize &sizeTX, CSize &sizeTY)
{
    CFont *pFont;
    sizeTX = sizeTY = CSize(0,0);

    if(GetBitStyle(GRF_NAME_X))
    {
        pFont = pDC->SelectObject(&fFontNameX);
        ASSERT(pFont);
        sizeTX = CScale::GetSizeText(pDC, sNameX, &fFontNameX, GetBitStyle(GRF_NAME_X));
        sizeTX += CSize(2*szSlotName.cx, 2*szSlotName.cy);
        pDC->SelectObject(pFont);
    }

    if(GetBitStyle(GRF_NAME_Y))
    {
        pFont = pDC->SelectObject(&fFontNameY);
        ASSERT(pFont);
        sizeTY = CScale::GetSizeText(pDC, sNameY, &fFontNameY, GetBitStyle(GRF_NAME_Y));
        sizeTY += CSize(2*szSlotName.cx, 2*szSlotName.cy);
        pDC->SelectObject(pFont);
    }
}


void CGraph::GetInflateSize(CSize &sizeTX, CSize &sizeTY)
{
    CClientDC dc(this);
    GetInflateSizeA(&dc, sizeTX, sizeTY);
    return;
}


void CGraph::RotFont(BOOL flg, CFont *pFont)
{
    LOGFONT lf;
    pFont->GetLogFont(&lf);

    lf.lfEscapement = lf.lfOrientation = 900*int(flg);

    if(pFont->m_hObject) pFont->DeleteObject();
    pFont->CreateFontIndirect(&lf);

    return;
}

LOGFONT CGraph::SetFontNameX(LOGFONT& lf)
{
    LOGFONT slf = logfontNX;
    logfontNX = lf;

    if(fFontNameX.m_hObject) fFontNameX.DeleteObject();
    fFontNameX.CreateFontIndirect(&logfontNX);

	SubstituteBitStyle(GRF_INV_NAME_X, lf.lfEscapement);

    RecalcSize();
    ChangePosition();
    if(m_hWnd) Invalidate(true);

    return slf;
}

LOGFONT CGraph::SetFontNameY(LOGFONT& lf)
{
    LOGFONT slf = logfontNY;
    logfontNY = lf;

    if(fFontNameY.m_hObject) fFontNameY.DeleteObject();
    fFontNameY.CreateFontIndirect(&logfontNY);

	SubstituteBitStyle(GRF_INV_NAME_Y, lf.lfEscapement);

    RecalcSize();
    ChangePosition();
    if(m_hWnd) Invalidate(true);

    return slf;
}

BOOL CGraph::SetOrientFontX(BOOL flg)
{
	BOOL sflg = GetBitStyle(GRF_INV_NAME_X);
	SubstituteBitStyle(GRF_INV_NAME_X, flg);

    logfontNX.lfEscapement = logfontNX.lfOrientation = 900*int(flg);

    if(fFontNameX.m_hObject) fFontNameX.DeleteObject();
    fFontNameX.CreateFontIndirect(&logfontNX);

    RecalcSize();
    ChangePosition();
    if(m_hWnd) Invalidate(true);

    return sflg;
}

BOOL CGraph::SetOrientFontY(BOOL flg)
{
	BOOL sflg = GetBitStyle(GRF_INV_NAME_Y);
	SubstituteBitStyle(GRF_INV_NAME_Y, flg);

    logfontNY.lfEscapement = logfontNY.lfOrientation = 900*int(flg);

    if(fFontNameY.m_hObject) fFontNameY.DeleteObject();
    fFontNameY.CreateFontIndirect(&logfontNY);

    RecalcSize();
    ChangePosition();
    if(m_hWnd) Invalidate(true);

    return sflg;
}

int CGraph::SetSizeFontX(int sz)
{
    int ssz = -logfontNX.lfHeight;
    logfontNX.lfHeight = -sz;

    if(fFontNameX.m_hObject) fFontNameX.DeleteObject();
    fFontNameX.CreateFontIndirect(&logfontNX);

    RecalcSize();
    ChangePosition();
    if(m_hWnd) Invalidate(true);

    return ssz;
}

int CGraph::SetSizeFontY(int sz)
{
    int ssz = -logfontNY.lfHeight;
    logfontNY.lfHeight = -sz;

    if(fFontNameY.m_hObject) fFontNameY.DeleteObject();
    fFontNameY.CreateFontIndirect(&logfontNY);

    RecalcSize();
    ChangePosition();
    if(m_hWnd) Invalidate(true);

    return ssz;
}
void CGraph::SetRangeA()
{
    if(p_ScrollBarX->m_hWnd == NULL || p_ScrollBarY->m_hWnd == NULL) return;
    SCROLLINFO sScrollInfo;

    p_ScrollBarX->SetScrollRange(0, MAXSCROLLX, true);
    p_ScrollBarX->GetScrollInfo(&sScrollInfo, SIF_ALL);
    sScrollInfo.nPage = MAXSCROLLX/20;
    p_ScrollBarX->SetScrollInfo(&sScrollInfo);
    //.............................................................................
    p_ScrollBarY->SetScrollRange(0, MAXSCROLLY, true);
    p_ScrollBarY->GetScrollInfo(&sScrollInfo, SIF_ALL);
    sScrollInfo.nPage = MAXSCROLLY/20;
    p_ScrollBarY->SetScrollInfo(&sScrollInfo, SIF_ALL);
}

void CGraph::SetRange(UINT sizeBuf, int num_buf)
{
    VFRingBuf<F>* pMain = FieldGraph.DATA.GetBuf(-1);
    if(pMain == NULL) return;
    if(p_ScrollBarX->m_hWnd == NULL || p_ScrollBarY->m_hWnd == NULL) return;
    SCROLLINFO sScrollInfo;

    int sizeXScroll = FieldGraph.GetSizeXScroll(MAXSCROLLX);

    p_ScrollBarX->SetScrollRange(0, MAXSCROLLX - 1 + MAXSCROLLX/20 - sizeXScroll, true);
    p_ScrollBarX->GetScrollInfo(&sScrollInfo, SIF_ALL);
    sScrollInfo.nPage = MAXSCROLLX/20;
    p_ScrollBarX->SetScrollInfo(&sScrollInfo, SIF_ALL);

    sclHor.Fmin = pMain->Fmin;
    sclHor.Fmax = pMain->Fmax; 
    //.............................................................................

    int sizeVScroll = FieldGraph.GetSizeYScroll(MAXSCROLLY);

    p_ScrollBarY->SetScrollRange(0, MAXSCROLLY - 1 + MAXSCROLLY/20 - sizeVScroll, true);
    p_ScrollBarY->GetScrollInfo(&sScrollInfo, SIF_ALL);
    sScrollInfo.nPage = MAXSCROLLY/20;
    p_ScrollBarY->SetScrollInfo(&sScrollInfo, SIF_ALL);

    if(num_buf < 0) return;
    FieldGraph.DATA.GetMinMax(num_buf, sclVert.Fmin, sclVert.Fmax);
}

//,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
UINT CGraph::CreateBufer(UINT sizeBuf, F* inBuf, F* pBufX)
{
    UINT num_buf = FieldGraph.CreateBufer(sizeBuf, inBuf, pBufX);
    SetRange(sizeBuf, -1);
    return num_buf;
}

UINT CGraph::CreateBufY(UINT size_buf, UINT size_data, F* inBuf)
{
    UINT num_buf = FieldGraph.CreateBufY(size_buf, size_data, inBuf);
    return num_buf;
}

void CGraph::SetAdrBufX(UINT base_pos, VFRingBuf<F>& pBufX)
{
    FieldGraph.SetAdrBufX(base_pos, pBufX);
    SetRange(pBufX.GetSizeData(), -1);
}
void CGraph::SetAdrBufX(UINT base_pos, FRingBuf<F>& pBufX)
{
    FieldGraph.SetAdrBufX(base_pos, pBufX);
    SetRange(pBufX.GetSizeData(), -1);
}

UINT CGraph::CreateBufYWithExtInpBuf(UINT base_pos, VFRingBuf<F>& pBuf)
{
    UINT num_buf = FieldGraph.CreateBufYWithExtInpBuf(base_pos, pBuf);
    return num_buf;
}
UINT CGraph::CreateBufYWithExtInpBuf(UINT base_pos, FRingBuf<F>& pBuf)
{
    UINT num_buf = FieldGraph.CreateBufYWithExtInpBuf(base_pos, pBuf);
    return num_buf;
}
F CGraph::GetScaleXStart()
{
    return FieldGraph.GetScaleXStart();
}

void CGraph::SetViewInitDataX()
{
    FieldGraph.DATA.SetViewInitDataX();
}

//,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
void CGraph::DestroyBuf()
{
    FieldGraph.DestroyBuf();
}

//,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
BOOL CGraph::InitDataX(int maxSize, F* pBufX)
{
    return FieldGraph.InitDataX(maxSize, pBufX);
}
void CGraph::InitDataX(VFRingBuf<F>& pBufX)
{
    FieldGraph.InitDataX(pBufX);
}
void CGraph::InitDataX(FRingBuf<F>& pBufX)
{
    FieldGraph.InitDataX(pBufX);
}

void CGraph::InitDataY(UINT num_buf, UINT sizeBuf, F* inBuf)
{
    FieldGraph.InitDataY(num_buf, sizeBuf, inBuf);
}
void CGraph::InitDataY(UINT num_buf, UINT beg_index, UINT size_data, F* inBuf)
{
	FieldGraph.InitDataY0(num_buf, beg_index, size_data, inBuf);
}

void CGraph::InitDataY(UINT num_buf, VFRingBuf<F>& pBufY)
{
    FieldGraph.InitDataY(num_buf, pBufY);
}
void CGraph::InitDataY(UINT num_buf, FRingBuf<F>& pBufY)
{
    FieldGraph.InitDataY(num_buf, pBufY);
}

UINT CGraph::InitDataYWithName(CString sName, UINT sizeBuf, F* inBuf)
{
    return FieldGraph.InitDataYWithName(sName, sizeBuf, inBuf);
}

void CGraph::AddData(UINT num_buf, UINT size_data, UINT base_pos, F* inBuf)
{
    FieldGraph.AddData(num_buf, size_data, base_pos, inBuf);
}
void CGraph::InitDataXY(UINT num_buf, UINT sizeBuf, F* inBufX, F* inBufY)
{
    InitDataX(sizeBuf, inBufX);
    InitDataY(num_buf, sizeBuf, inBufY);
}

//,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
void CGraph::SetMinMaxT(const F FminX, const F FmaxX)
{
    sclHor.SetMinMax(FminX, FmaxX);
    FieldGraph.SetMinMaxT(FminX, FmaxX);
    OnChangeRange(0, 0);
    if(m_hWnd != NULL) Invalidate(true);
}
void CGraph::SetMinMaxTR(const F FminX, const F FmaxX)
{
    sclHor.SetMinMax(FminX, FmaxX);
    FieldGraph.SetMinMaxT(FminX, FmaxX);
    if(m_hWnd != NULL) Invalidate(true);
}

void CGraph::SetMinMaxXDiapazon(const F FminX, const F FmaxX)
{
    sclHor.SetMinMax(FminX, FmaxX);
    FieldGraph.SetMinMaxXDiapazon(FminX, FmaxX);
    if(m_hWnd != NULL) Invalidate(true);
}

void CGraph::SetMinMaxTScale(const F FminX, const F FmaxX)
{
    sclHor.SetMinMax(FminX, FmaxX);
    FieldGraph.SetMinMaxT(FminX, FmaxX);
    if(sclHor.m_hWnd != NULL) sclHor.Invalidate(true);
    if(FieldGraph.m_hWnd != NULL) FieldGraph.Invalidate(false);
    OnChangeRange(0, 0);
}

void CGraph::GetMinMaxT(F& FminX, F& FmaxX)
{
    FieldGraph.GetMinMaxT(FminX, FmaxX);
}

void CGraph::SetMinMaxF(F Fmin, F Fmax, int num_grf)
{
    if(!m_hWnd) return;

    sclVert.SetMinMax(Fmin, Fmax);
    FieldGraph.SetMinMaxF(Fmin, Fmax, num_grf);
    if(m_hWnd != NULL)Invalidate(true);
}

void CGraph::SetMinMaxY(F Fmin, F Fmax, int num_grf)
{
    if(!m_hWnd) return;

    sclVert.SetMinMax(Fmin, Fmax);
    FieldGraph.SetMinMaxY(Fmin, Fmax, num_grf);
    if(m_hWnd != NULL)Invalidate(true);
}

void CGraph::GetMinMaxF(F& Fmin, F& Fmax, int num_grf)
{
    FieldGraph.GetMinMaxF(Fmin, Fmax, num_grf);
}

void CGraph::GetMinMaxFDiapazon(F& Fmin, F& Fmax, int num_grf)
{
    FieldGraph.GetMinMaxFDiapazon(Fmin, Fmax, num_grf);
}

void CGraph::SetMinMaxFDiapazon(F Fmin, F Fmax, int num_grf)
{
    FieldGraph.SetMinMaxFDiapazon(Fmin, Fmax, num_grf);
}

void CGraph::ChangeHScale(F deltaScale)
{
    if(!FieldGraph.ChangeHScale(deltaScale)) return;
    sclHor.SetMinMax(FieldGraph.DATA.GetPosUMarker(-1, 1), FieldGraph.DATA.GetPosUMarker(-1, 2));
    if(FieldGraph.HTestRange()) p_ScrollBarX->SetScrollPos(0);
    SetRangeA();
}
void CGraph::ChangeVScale(F deltaScale)
{
    /*
    if(!FieldGraph.ChangeVScale(deltaScale)) return;
    sclVert.SetMinMax(FieldGraph.FminXView, FieldGraph.FmaxXView);

    SCROLLINFO sScrollInfo;
    p_ScrollBarY->GetScrollInfo(&sScrollInfo, SIF_ALL);
    if(FieldGraph.VTestRange()) p_ScrollBarY->SetScrollPos(sScrollInfo.nMax);
    */
}


void CGraph::SetGreed(BOOL greed)
{
	SubstituteBitStyle(FLD_GREED, greed);
    FieldGraph.SetGreed(greed);
}

void CGraph::SetScaleLine(BOOL scaleline)
{
    FieldGraph.SetScaleLine(scaleline);
}

void CGraph::SetStyleLineFunc(UINT stypeView)
{
    FieldGraph.SetStyleLineFunc(stypeView);
}

/*
int CGraph::GetSizeFragment()
{
return FieldGraph.GetSizeFragment();
}

int CGraph::GetStartFragment()
{
return FieldGraph.GetStartFragment();
}

//...............................................
int CGraph::GetFragment(BYTE* pBuf)
{
return FieldGraph.GetFragment(pBuf);
}

int CGraph::GetFragment(WORD* pBuf)
{
return FieldGraph.GetFragment(pBuf);
}

int CGraph::GetFragment(F* pBuf)
{
return FieldGraph.GetFragment(pBuf);
}
//...............................................

int CGraph::InsertFragment(int uStartInsert, int uszFragment, BYTE* pBuf)
{
int sz = FieldGraph.InsertFragment(uStartInsert, uszFragment, pBuf);
SetRangeA(sz);
FieldGraph.slctRect = FieldGraph.fieldRec;
FieldGraph.SetRectRange();
return sz;
}

int CGraph::InsertFragment(int uStartInsert, int uszFragment, WORD* pBuf)
{
int sz = FieldGraph.InsertFragment(uStartInsert, uszFragment, pBuf);
SetRangeA(sz);
FieldGraph.slctRect = FieldGraph.fieldRec;
FieldGraph.SetRectRange();
return sz;
}

int CGraph::InsertFragment(int uStartInsert, int uszFragment, F* pBuf)
{
int sz = FieldGraph.InsertFragment(uStartInsert, uszFragment, pBuf);
SetRangeA(sz);
FieldGraph.slctRect = FieldGraph.fieldRec;
FieldGraph.SetRectRange();
return sz;
}

//...............................................
int CGraph::DeleteFragment(int uStartDelete, int uszFragment)
{
int sz = FieldGraph.DeleteFragment(uStartDelete, uszFragment);
SetRangeA(sz);
FieldGraph.slctRect = FieldGraph.fieldRec;
FieldGraph.SetRectRange();
return sz;
}

int CGraph::DeleteFragment()
{
int sz = FieldGraph.DeleteFragment();
SetRangeA(sz);
FieldGraph.slctRect = FieldGraph.fieldRec;
FieldGraph.SetRectRange();
return sz;
}
*/
//...............................................

BOOL CGraph::UndoBack()
{
    return FieldGraph.UndoBack();
}

BOOL CGraph::UndoForward()
{
    return FieldGraph.UndoForward();
}

//...............................................
/*
F CGraph::HFullZoom()
{
F zm = FieldGraph.HFullZoom();
FieldGraph.slctRect = FieldGraph.fieldRec;
FieldGraph.SetRectRange();

return zm;
}

F CGraph::VFullZoom()
{
F zm = FieldGraph.VFullZoom();
FieldGraph.slctRect = FieldGraph.fieldRec;
FieldGraph.SetRectRange();

return zm;
}
*/
void CGraph::SetHorScale(BOOL showScale)
{
    if(!m_hWnd) return;
	SubstituteBitStyle(SCLX_VIEW, showScale);
    RecalcSize();
    ChangePosition();
}

void CGraph::SetVertScale(BOOL showScale)
{
	if(!m_hWnd) return;
	SubstituteBitStyle(SCLY_VIEW, showScale);
    RecalcSize();
    ChangePosition();
}	

void CGraph::SetRot90(BOOL flg)
{
	if(m_hWnd == NULL || FieldGraph.m_hWnd == NULL) return;
	if(GetBitStyle(GRF_ORIENT_FLD) == flg) return;
	SubstituteBitStyle(GRF_ORIENT_FLD, flg);

	SubstituteBitStyle(SCLY_ORIENT, !flg);
	SubstituteBitStyle(SCLY_ORIENT_INVERT, flg);

	SubstituteBitStyle(SCLX_ORIENT, flg);
	SubstituteBitStyle(SCLX_ORIENT_INVERT, false);
	SubstituteBitStyle(SCLX_TYPE_SCALE, true);

	SubstituteBitStyle(FLD_ORIENT, flg);

	BOOL flgI = GetBitStyle(GRF_INV_NAME_X);
	SubstituteBitStyle(GRF_INV_NAME_X, GetBitStyle(GRF_INV_NAME_Y));
	SubstituteBitStyle(GRF_INV_NAME_Y, flgI);

	SubstituteBitStyle(SCLX_ORIENT_FONT_M, !GetBitStyle(SCLX_ORIENT_FONT_M));
	SubstituteBitStyle(SCLX_ORIENT_FONT_N, !GetBitStyle(SCLX_ORIENT_FONT_N));
	sclHor.UpdateOrientFont();

	SubstituteBitStyle(SCLY_ORIENT_FONT_M, !GetBitStyle(SCLY_ORIENT_FONT_M));
	SubstituteBitStyle(SCLY_ORIENT_FONT_N, !GetBitStyle(SCLY_ORIENT_FONT_N));
	sclVert.UpdateOrientFont();

	SetOrientFontNX(GetBitStyle(GRF_INV_NAME_X));
	SetOrientFontNY(GetBitStyle(GRF_INV_NAME_Y));

    RecalcSize();
    ChangePosition(); 
    if(m_hWnd) Invalidate(true);
    FieldGraph.Invalidate(true);
}

void CGraph::AddPoint(UINT num_grf, F mfnc, BOOL flg_scl)
{
    FieldGraph.AddPoint(num_grf, mfnc, flg_scl);
}

void CGraph::Pinok(int sz_pinok)
{
    FieldGraph.Pinok(sz_pinok);
}
BOOL CGraph::DrawLastData(int num_grf, int sz_data)
{
    return FieldGraph.DrawLastData(num_grf, sz_data);
}

int CGraph::GetSizeData()
{
    return FieldGraph.DATA.GetSizeData(-1);
}

void CGraph::AddPointXY(F Yvalue, F Xvalue, UINT num_grf)
{
    FieldGraph.AddPointXY(Yvalue, Xvalue, num_grf);
}

void CGraph::SetViewRangeTime(F startViewTime, F rangeViewTime, F dtime)
{
    FieldGraph.SetViewRangeTime(startViewTime, rangeViewTime, dtime);
}

void CGraph::GetRecFieldF(CRect *rec)
{
    if(FieldGraph.m_hWnd != NULL) FieldGraph.GetClientRect(rec);
}

void CGraph::GetRecFieldHS(CRect *rec)
{
    if(sclHor.m_hWnd != NULL) sclHor.GetClientRect(rec);
}

void CGraph::GetRecFieldVS(CRect *rec)
{
    if(sclVert.m_hWnd != NULL) sclVert.GetClientRect(rec);
}
//-------------------------------------------------------------------------
ULONG64 CGraph::SetStyle(ULONG64 dwStyle, BOOL flRedraw)
{
	graphStyle.dwFlags |=  dwStyle;
	FieldGraph.SetStyle((DWORD)graphStyle.style.styleFieldGrf);
	sclHor.SetStyle((DWORD)graphStyle.style.styleSclX);
	sclVert.SetStyle((DWORD)graphStyle.style.styleSclY);
	if(flRedraw) UpdateGraph();
	return graphStyle.dwFlags;
}
ULONG64 CGraph::GetStyle()
{
	return graphStyle.dwFlags;
}
ULONG64 CGraph::ResetStyle(ULONG64 g_style, BOOL flRedraw)
{
	GraphFlags tmp;
	tmp.dwFlags = g_style;

	g_style = ~g_style;
	graphStyle.dwFlags &=  g_style;

	FieldGraph.ResetStyle((DWORD)tmp.style.styleFieldGrf);
	sclHor.ResetStyle((DWORD)tmp.style.styleSclX);
	sclVert.ResetStyle((DWORD)tmp.style.styleSclY);

	if(flRedraw) UpdateGraph();
	return graphStyle.dwFlags;

	return graphStyle.dwFlags;
}
BOOL CGraph::GetBitStyle(ULONG64 g_style)
{
	ULONG64 bit_style = graphStyle.dwFlags &  g_style;
	return bit_style == 0 ? false : true;
}

/*
void CGraph::SetAlert1Val(F alrtVal)
{
FieldGraph.SetAlert1Val(alrtVal);
if(FieldGraph.m_hWnd != NULL) FieldGraph.Invalidate(true);
}

void CGraph::SetAlert2Val(F alrtVal)
{
FieldGraph.SetAlert2Val(alrtVal);
if(FieldGraph.m_hWnd != NULL) FieldGraph.Invalidate(true);
}

void CGraph::SetAlert3Val(F alrtVal)
{
FieldGraph.SetAlert3Val(alrtVal);
if(FieldGraph.m_hWnd != NULL) FieldGraph.Invalidate(true);
}

void CGraph::SetAlert4Val(F alrtVal)
{
FieldGraph.SetAlert4Val(alrtVal);
if(FieldGraph.m_hWnd != NULL) FieldGraph.Invalidate(true);
}

void CGraph::SetAlert1(BOOL bflAlert)
{
FieldGraph.SetAlert1(bflAlert);
if(FieldGraph.m_hWnd != NULL) FieldGraph.Invalidate(true);
}

void CGraph::SetAlert2(BOOL bflAlert)
{
FieldGraph.SetAlert2(bflAlert);
if(FieldGraph.m_hWnd != NULL) FieldGraph.Invalidate(true);
}

void CGraph::SetAlert3(BOOL bflAlert)
{
FieldGraph.SetAlert3(bflAlert);
if(FieldGraph.m_hWnd != NULL) FieldGraph.Invalidate(true);
}

void CGraph::SetAlert4(BOOL bflAlert)
{
FieldGraph.SetAlert4(bflAlert);
if(FieldGraph.m_hWnd != NULL) FieldGraph.Invalidate(true);
}
*/
void CGraph::SetShowSclH(BOOL bShow)
{
    sclHor.SetShowFlag(bShow);
}

void CGraph::SetShowSclV(BOOL bShow)
{
    sclVert.SetShowFlag(bShow);
}

void CGraph::SetColorHaderAlert(COLORREF color)
{
    sclHor.SetColorHaderAlert(color);
}

COLORREF CGraph::GetColorHaderAlert()
{
    return sclHor.GetColorHaderAlert();
}

void CGraph::RenameVScale(CString sName, BOOL bflgType, LPVOID pBuff)
{
	SubstituteBitStyle(SCLY_TIME, bflgType);
	SubstituteBitStyle(FLD_STYLE_TYME, bflgType);
    sclVert.RenameScale(sName);
}

BOOL CGraph::EnableDgt(BOOL flg)
{
    BOOL fl = GetBitStyle(GRF_ENABLE_DGT);
    SubstituteBitStyle(GRF_ENABLE_DGT, flg);
    return fl;	
}

void CGraph::RenameHScale(CString sName, BOOL bflgType, LPVOID pBuff)
{
	SubstituteBitStyle(SCLX_TIME, bflgType);
	SubstituteBitStyle(FLD_STYLE_TYME, bflgType);
    sclHor.RenameScale(sName);
}

void CGraph::SetAdrBufY(UINT num_buf, UINT base_pos, VFRingBuf<F>& pBuf)
{
    FieldGraph.SetAdrBufY(num_buf, base_pos, pBuf);
    FieldGraph.Invalidate(false);
}
void CGraph::SetAdrBufY(UINT num_buf, UINT base_pos, FRingBuf<F>& pBuf)
{
    FieldGraph.SetAdrBufY(num_buf, base_pos, pBuf);
    FieldGraph.Invalidate(false);
}
UINT CGraph::SetAdrBufY(UINT base_pos, FRingBuf<F>& pBuf)
{
    UINT nn = FieldGraph.SetAdrBufY(base_pos, pBuf);
    UpdateGraph();
    return nn;
}

void CGraph::ChangeScale(CString sName, BOOL bflgType, LPVOID pBuff, GRF_MINMAX& par)
{
    if(pBuff == NULL) return;
	if(par.flag_undo) FieldGraph.UndoStart();//чтоб не накапливалось
	SubstituteBitStyle(SCLX_TIME, bflgType);
	SubstituteBitStyle(FLD_STYLE_TYME, bflgType);
    FieldGraph.ChangeScale((FRingBuf<F>*)pBuff, par);
	if(FieldGraph.indexList == 0) 
	{
		if(!GetBitStyle(GRF_ORIENT_FLD)) ResetStyle(GRF_Y_SCROLL);
		else ResetStyle(GRF_X_SCROLL);
		RecalcSize();
		ChangePosition();
	}
    sclHor.RenameScale(sName);
}
/////////////////////////////////////////////////////////////////////////////////////
void CGraph::UpdateGraph()
{
    RecalcSize();
    ChangePosition();
    if(m_hWnd) Invalidate(true);
}
void CGraph::UpdateRectRangeExternal(BOOL flRedraw)
{
    FieldGraph.SetRectRangeExternal();
	if(flRedraw) FieldGraph.Invalidate(false); 
}

BOOL CGraph::SetViewNX(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(GRF_NAME_X);
	SubstituteBitStyle(GRF_NAME_X, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}

CString CGraph::SetTextNX(CString sname, BOOL flRedraw)
{
    CString snm = sNameX;
    sNameX = sname;
    if(flRedraw) UpdateGraph();
    return  snm;
}

LOGFONT CGraph::SetFontNX(LOGFONT& lf, BOOL flRedraw)
{
    LOGFONT slf = logfontNX;
    logfontNX = lf;

    logfontNX.lfEscapement = slf.lfEscapement;
    logfontNX.lfOrientation = slf.lfOrientation;
    logfontNX.lfOutPrecision = slf.lfOutPrecision;

    if(fFontNameX.m_hObject) fFontNameX.DeleteObject();
    fFontNameX.CreateFontIndirect(&logfontNX);

	SubstituteBitStyle(GRF_INV_NAME_X, slf.lfEscapement);
    if(flRedraw) UpdateGraph();
    return slf;
}
int CGraph::SetSizeFontNX(int sz, BOOL flRedraw)
{
    int ssz = -logfontNX.lfHeight;
    logfontNX.lfHeight = -sz;

    if(fFontNameX.m_hObject) fFontNameX.DeleteObject();
    fFontNameX.CreateFontIndirect(&logfontNX);

    if(flRedraw) UpdateGraph();

    return ssz;
}
BOOL CGraph::SetOrientFontNX(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(GRF_INV_NAME_X);
	SubstituteBitStyle(GRF_INV_NAME_X, flg);

    logfontNX.lfEscapement = logfontNX.lfOrientation = 900*int(flg);

    if(fFontNameX.m_hObject) fFontNameX.DeleteObject();
    fFontNameX.CreateFontIndirect(&logfontNX);

    if(flRedraw) UpdateGraph();

    return sflg;
}
COLORREF CGraph::SetColorFontNX(COLORREF clr, BOOL flRedraw)
{
    COLORREF sclr = colorFontX;
    colorFontX = clr;

    if(flRedraw) UpdateGraph();
    return sclr;
}
//==================================================================================
BOOL CGraph::SetViewNY(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(GRF_NAME_Y);
	SubstituteBitStyle(GRF_NAME_Y, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}
CString CGraph::SetTextNY(CString sname, BOOL flRedraw)
{
    CString snm = sNameY;
    sNameY = sname;
    if(flRedraw) UpdateGraph();
    return  snm;
}
LOGFONT CGraph::SetFontNY(LOGFONT& lf, BOOL flRedraw)
{
    LOGFONT slf = logfontNY;
    logfontNY = lf;

    logfontNY.lfEscapement = slf.lfEscapement;
    logfontNY.lfOrientation = slf.lfOrientation;
    logfontNY.lfOutPrecision = slf.lfOutPrecision;

    if(fFontNameY.m_hObject) fFontNameY.DeleteObject();
    fFontNameY.CreateFontIndirect(&logfontNY);

	SubstituteBitStyle(GRF_INV_NAME_Y, slf.lfEscapement);

    if(flRedraw) UpdateGraph();

    return slf;
}
int CGraph::SetSizeFontNY(int sz, BOOL flRedraw)
{
    int ssz = -logfontNY.lfHeight;
    logfontNY.lfHeight = -sz;

    if(fFontNameY.m_hObject) fFontNameY.DeleteObject();
    fFontNameY.CreateFontIndirect(&logfontNY);

    if(flRedraw) UpdateGraph();

    return ssz;
}
BOOL CGraph::SetOrientFontNY(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(GRF_INV_NAME_Y);
	SubstituteBitStyle(GRF_INV_NAME_Y, flg);

    logfontNY.lfEscapement = logfontNY.lfOrientation = 900*int(flg);

    if(fFontNameY.m_hObject) fFontNameY.DeleteObject();
    fFontNameY.CreateFontIndirect(&logfontNY);

    if(flRedraw) UpdateGraph();

    return sflg;
}

COLORREF CGraph::SetColorFontNY(COLORREF clr, BOOL flRedraw)
{
    COLORREF sclr = colorFontY;
    colorFontY = clr;

    if(flRedraw) UpdateGraph();
    return sclr;
}
//==================================================================================
BOOL CGraph::SetViewSclX(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(SCLX_VIEW);
	SubstituteBitStyle(SCLX_VIEW, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}

BOOL CGraph::SetViewDgtX(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(SCLX_VIEW_DGT);
	SubstituteBitStyle(SCLX_VIEW_DGT, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}

BOOL CGraph::SetViewSclNX(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(SCLX_NAME);
	SubstituteBitStyle(SCLX_NAME, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}

CString CGraph::SetTextSclNX(CString sname, BOOL flRedraw)
{
    CString snm = sclHor.sclName;
    sclHor.sclName = sname;
    if(flRedraw) UpdateGraph();
    return  snm;
}

LOGFONT CGraph::SetFontDgtX(LOGFONT& lf, BOOL flRedraw)
{
    LOGFONT& lfscl = sclHor.m_logfontM; 
    CFont& fntscl = sclHor.fFontMain;

    LOGFONT slf = lfscl;
    lfscl = lf;

    lfscl.lfEscapement = slf.lfEscapement;
    lfscl.lfOrientation = slf.lfOrientation;
    lfscl.lfOutPrecision = slf.lfOutPrecision;

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

	SubstituteBitStyle(SCLX_ORIENT_FONT_M, slf.lfEscapement);
    if(flRedraw) UpdateGraph();

    return slf;
}

int CGraph::SetSizeFontDgtX(int sz, BOOL flRedraw)
{
    LOGFONT& lfscl = sclHor.m_logfontM; 
    CFont& fntscl = sclHor.fFontMain;

    int ssz = -lfscl.lfHeight;
    lfscl.lfHeight = -sz;

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

    if(flRedraw) UpdateGraph();

    return ssz;
}

BOOL CGraph::SetOrientDgtX(BOOL flg, BOOL flRedraw)
{
    LOGFONT& lfscl = sclHor.m_logfontM; 
    CFont& fntscl = sclHor.fFontMain;

	BOOL sflg = GetBitStyle(SCLX_ORIENT_FONT_M);
	SubstituteBitStyle(SCLX_ORIENT_FONT_M, flg);

    lfscl.lfEscapement = lfscl.lfOrientation = 900*int(flg);

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

    if(flRedraw) UpdateGraph();

    return sflg;
}
COLORREF CGraph::SetColorDgtX(COLORREF clr, BOOL flRedraw)
{
    COLORREF sclr = sclHor.colorText;
    sclHor.colorText = clr;
	if(sclHor.m_hWnd != NULL && flRedraw)
	{
		sclHor.Invalidate(true);
		sclHor.UpdateWindow();
	}
    return sclr;
}
LOGFONT CGraph::SetFontSclNX(LOGFONT& lf, BOOL flRedraw)
{
    LOGFONT& lfscl = sclHor.m_logfontN; 
    CFont& fntscl = sclHor.fFontName;

    LOGFONT slf = lfscl;
    lfscl = lf;

    lfscl.lfEscapement = slf.lfEscapement;
    lfscl.lfOrientation = slf.lfOrientation;
    lfscl.lfOutPrecision = slf.lfOutPrecision;

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

	SubstituteBitStyle(SCLX_ORIENT_FONT_N, slf.lfEscapement);
    if(flRedraw) UpdateGraph();

    return slf;
}

int CGraph::SetSizeFontSclNX(int sz, BOOL flRedraw)
{
    LOGFONT& lfscl = sclHor.m_logfontN; 
    CFont& fntscl = sclHor.fFontName;

    int ssz = -lfscl.lfHeight;
    lfscl.lfHeight = -sz;

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

    if(flRedraw) UpdateGraph();

    return ssz;
}

BOOL CGraph::SetOrientSclNX(BOOL flg, BOOL flRedraw)
{
    LOGFONT& lfscl = sclHor.m_logfontN; 
    CFont& fntscl = sclHor.fFontName;

	BOOL sflg = GetBitStyle(SCLX_ORIENT_FONT_N);
	SubstituteBitStyle(SCLX_ORIENT_FONT_N, flg);

    lfscl.lfEscapement = lfscl.lfOrientation = 900*int(flg);

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

    if(flRedraw) UpdateGraph();

    return sflg;
}

COLORREF CGraph::SetColorSclNX(COLORREF clr, BOOL flRedraw)
{
    COLORREF sclr = sclHor.colorName;
    sclHor.colorName = clr;

    if(flRedraw) UpdateGraph();
    return sclr;
}

COLORREF CGraph::SetColorHeadX(COLORREF clr, BOOL flRedraw) 
{
    COLORREF sclr = colorHeadX;
    colorHeadX = clr;

    if(flRedraw) UpdateGraph();
    return sclr;
}

BOOL CGraph::SetButtonNameX(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(GRF_BTN_X_NAME);
	SubstituteBitStyle(GRF_BTN_X_NAME, flg);
	if(flRedraw) UpdateGraph();
    return sflg;
}
//==================================================================================
BOOL CGraph::SetViewSclY(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(SCLY_VIEW);
	SubstituteBitStyle(SCLY_VIEW, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}

BOOL CGraph::SetViewDgtY(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(SCLY_VIEW_DGT);
	SubstituteBitStyle(SCLY_VIEW_DGT, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}

BOOL CGraph::SetViewSclNY(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(SCLY_NAME);
	SubstituteBitStyle(SCLY_NAME, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}

CString CGraph::SetTextSclNY(CString sname, BOOL flRedraw)
{
    CString snm = sclVert.sclName;
    sclVert.sclName = sname;
    if(flRedraw) UpdateGraph();
    return  snm;
}

LOGFONT CGraph::SetFontDgtY(LOGFONT& lf, BOOL flRedraw)
{
    LOGFONT& lfscl = sclVert.m_logfontM; 
    CFont& fntscl = sclVert.fFontMain;

    LOGFONT slf = lfscl;
    lfscl = lf;

    lfscl.lfEscapement = slf.lfEscapement;
    lfscl.lfOrientation = slf.lfOrientation;
    lfscl.lfOutPrecision = slf.lfOutPrecision;

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

	SubstituteBitStyle(SCLY_ORIENT_FONT_M, slf.lfEscapement);
    if(flRedraw) UpdateGraph();

    return slf;
}

int CGraph::SetSizeFontDgtY(int sz, BOOL flRedraw)
{
    LOGFONT& lfscl = sclVert.m_logfontM; 
    CFont& fntscl = sclVert.fFontMain;

    int ssz = -lfscl.lfHeight;
    lfscl.lfHeight = -sz;

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

    if(flRedraw) UpdateGraph();

    return ssz;
}

BOOL CGraph::SetOrientDgtY(BOOL flg, BOOL flRedraw)
{
    LOGFONT& lfscl = sclVert.m_logfontM; 
    CFont& fntscl = sclVert.fFontMain;

	BOOL sflg = GetBitStyle(SCLY_ORIENT_FONT_M);
	SubstituteBitStyle(SCLY_ORIENT_FONT_M, flg);

    lfscl.lfEscapement = lfscl.lfOrientation = 900*int(flg);

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

    if(flRedraw) UpdateGraph();

    return sflg;
}

COLORREF CGraph::SetColorDgtY(COLORREF clr, BOOL flRedraw)
{
    COLORREF sclr = sclVert.colorText;
    sclVert.colorText = clr;
	if(sclHor.m_hWnd != NULL && flRedraw)
	{
		sclVert.Invalidate(true);
		sclVert.UpdateWindow();
	}
    return sclr;
}

LOGFONT CGraph::SetFontSclNY(LOGFONT& lf, BOOL flRedraw)
{
    LOGFONT& lfscl = sclVert.m_logfontN; 
    CFont& fntscl = sclVert.fFontName;

    LOGFONT slf = lfscl;
    lfscl = lf;

    lfscl.lfEscapement = slf.lfEscapement;
    lfscl.lfOrientation = slf.lfOrientation;
    lfscl.lfOutPrecision = slf.lfOutPrecision;

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

	SubstituteBitStyle(SCLY_ORIENT_FONT_N, slf.lfEscapement);
    if(flRedraw) UpdateGraph();

    return slf;
}

int CGraph::SetSizeFontSclNY(int sz, BOOL flRedraw)
{
    LOGFONT& lfscl = sclVert.m_logfontN; 
    CFont& fntscl = sclVert.fFontName;

    int ssz = -lfscl.lfHeight;
    lfscl.lfHeight = -sz;

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

    if(flRedraw) UpdateGraph();

    return ssz;
}

BOOL CGraph::SetOrientSclNY(BOOL flg, BOOL flRedraw)
{
    LOGFONT& lfscl = sclVert.m_logfontN; 
    CFont& fntscl = sclVert.fFontName;

	BOOL sflg = GetBitStyle(SCLY_ORIENT_FONT_N);
	SubstituteBitStyle(SCLY_ORIENT_FONT_N, flg);

    lfscl.lfEscapement = lfscl.lfOrientation = 900*int(flg);

    if(fntscl.m_hObject) fntscl.DeleteObject();
    fntscl.CreateFontIndirect(&lfscl);

    if(flRedraw) UpdateGraph();

    return sflg;
}

COLORREF CGraph::SetColorSclNY(COLORREF clr, BOOL flRedraw)
{
    COLORREF sclr = sclVert.colorName;
    sclVert.colorName = clr;

    if(flRedraw) UpdateGraph();
    return sclr;
}

COLORREF CGraph::SetColorHeadY(COLORREF clr, BOOL flRedraw) 
{
    COLORREF sclr = colorHeadY;
    colorHeadY = clr;

    if(flRedraw) UpdateGraph();
    return sclr;
}

BOOL CGraph::SetButtonNameY(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(GRF_BTN_X_NAME);
	SubstituteBitStyle(GRF_BTN_X_NAME, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}

COLORREF CGraph::SetColorBk(COLORREF clr, BOOL flRedraw)
{
    COLORREF clrs = colorFon;
    colorFon = sclVert.colorFon = sclHor.colorFon = clr;

    if(flRedraw) UpdateGraph();
    return clrs;
}

BOOL CGraph::ShowGreed(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(FLD_GREED);
	SubstituteBitStyle(FLD_GREED, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}

BOOL CGraph::ShowScale(BOOL flg, BOOL flRedraw)
{
	BOOL sflg = GetBitStyle(FLD_SCALE);
	SubstituteBitStyle(FLD_SCALE, flg);
    if(flRedraw) UpdateGraph();
    return sflg;
}

COLORREF CGraph::SetColorFieldGraph(COLORREF color, BOOL flRedraw)
{
    COLORREF clr = FieldGraph.SetColorFon(color);
	if(FieldGraph.m_hWnd != NULL && flRedraw)
	{
		FieldGraph.Invalidate(true);
		FieldGraph.UpdateWindow();
	}
	return clr;
}

COLORREF CGraph::SetColorGreedGraph(COLORREF color, BOOL flRedraw)
{
    COLORREF clr = FieldGraph.SetColorGreed(color);
	if(FieldGraph.m_hWnd != NULL && flRedraw)
	{
		FieldGraph.Invalidate(true);
		FieldGraph.UpdateWindow();
	}
	return clr;
}

void CGraph::SetStyleGreedGraph(int nPenStyle, int nWidth, BOOL flRedraw)
{
    FieldGraph.SetStyleGreed(nPenStyle, nWidth);
	if(FieldGraph.m_hWnd != NULL && flRedraw)
	{
		FieldGraph.Invalidate(true);
		FieldGraph.UpdateWindow();
	}
}

COLORREF CGraph::SetColorGraph(COLORREF color)
{
    return FieldGraph.SetColorGraph(color);
    UpdateGraph();
}

COLORREF CGraph::SetColorGraphEX(UINT NumGrf, COLORREF color)
{
    return FieldGraph.SetColorGraphEX(NumGrf, color);
    UpdateGraph();
}

COLORREF CGraph::SetColorReperGraph(COLORREF color, BOOL flRedraw)
{
    COLORREF clr = FieldGraph.SetColorReper(color);;
    if(flRedraw) UpdateGraph();
    return clr;
}

COLORREF CGraph::SetColorTextReper(COLORREF color, BOOL flRedraw)
{
    COLORREF clr = FieldGraph.SetColorTextReper(color);;
    if(flRedraw) UpdateGraph();
    return clr;
}

COLORREF CGraph::SetColorTextGrf(COLORREF color, BOOL flRedraw)
{
    COLORREF clr = FieldGraph.SetColorTextGrf(color);;
    if(flRedraw) UpdateGraph();
    return clr;
}

LOGFONT CGraph::SetFontReper(LOGFONT& lf, BOOL flRedraw)
{
    LOGFONT slf = FieldGraph.SetFontReper(lf);
    if(flRedraw) UpdateGraph();
    return slf;
}

LOGFONT CGraph::SetFontTextGrf(LOGFONT& lf, BOOL flRedraw)
{
    LOGFONT slf = FieldGraph.SetFontTextGrf(lf);
    if(flRedraw) UpdateGraph();
    return slf;
}

CString CGraph::SetTextGrf(CString sname, BOOL flRedraw)
{
    CString snm = FieldGraph.m_textGrf;
    FieldGraph.m_textGrf = sname;
    if(flRedraw) UpdateGraph();
    return  snm;
}

void CGraph::AddReper(F XR, CString TxtR)
{
}

void CGraph::AddReper(F YR, F XR, CString TxtR)
{
    FieldGraph.AddReper(YR, XR, TxtR);
}
void CGraph::ClearRepers()
{
    FieldGraph.ClearRepers();
}
TMarker<F>* CGraph::GetEditPoint(int& inex_x, int& inex_y, int &num_buf)
{
    ::SetCursor(FieldGraph.cursorNorm); // !!!IVG  19.12.02
    ReleaseCapture();					// !!!IVG  19.12.02
    return FieldGraph.GetEditPoint(inex_x, inex_y, num_buf);
}
void CGraph::SetMarker(int num_buf, int index, COLORREF clr, char type_ch)
{
    FieldGraph.SetMarker(num_buf, index, clr, type_ch);
}
void CGraph::SetEditPoint(F XX, F YY, int indexp)
{
    FieldGraph.SetEditPoint(XX, YY, indexp);
    flagEdit = false;
}
int CGraph::SetTypeGraph(int gtype)
{
    return FieldGraph.SetTypeGraph(gtype);
}
void CGraph::ShowScroll(BOOL flShowScrollH, BOOL flShowScrollV)
{
    if(GetBitStyle(GRF_ORIENT_FLD) == 0)
    {
		SubstituteBitStyle(GRF_X_SCROLL,  flShowScrollV);
		SubstituteBitStyle(GRF_Y_SCROLL,  flShowScrollH);
    }
    else
    {
		SubstituteBitStyle(GRF_X_SCROLL,  flShowScrollH);
		SubstituteBitStyle(GRF_Y_SCROLL,  flShowScrollV);
    }
    UpdateGraph();
}
COLORREF CGraph::SetColorFonSclX(COLORREF clr, BOOL flRedraw)
{
    COLORREF clrs = sclHor.colorFon;
    sclHor.colorFon = clr;

    if(flRedraw) UpdateGraph();
    return clrs;
}

COLORREF CGraph::SetColorFonSclY(COLORREF clr, BOOL flRedraw)
{
    COLORREF clrs = sclVert.colorFon;
    sclVert.colorFon = clr;

    if(flRedraw) UpdateGraph();
    return clrs;
}

void CGraph::SetColorDiagram(int numDiagram, COLORREF clr, BOOL flag_color)
{
    FieldGraph.SetColorDiagram(numDiagram, clr, flag_color);
    UpdateGraph();
}

COLORREF CGraph::GetColorDiagram(int numDiagram)
{
    return FieldGraph.colorGraph[numDiagram];
}

int CGraph::GetThicknessDiagram(int numDiagram)
{
    return FieldGraph.GetThicknessDiagram(numDiagram);
}

void CGraph::SetThicknessDiagram(int numDiagram, int thick, BOOL flRedraw)
{
    if(thick < 0 || thick > 10) return;
    FieldGraph.SetThicknessDiagram(numDiagram, thick);

    if(flRedraw) UpdateGraph();
    return;
}

CRect CGraph::GetRectFldGrf()
{
    CRect rec;
    if(!FieldGraph.m_hWnd) return CRect(0,0,0,0);

    FieldGraph.GetClientRect(&rec);
    FieldGraph.ClientToScreen(&rec);

    return rec;
}

void CGraph::UpdateViewRange()
{
    FieldGraph.UpdateViewRange();
    if(p_ScrollBarX->m_hWnd != NULL) p_ScrollBarX->SetScrollPos(0);
    if(p_ScrollBarY->m_hWnd != NULL) p_ScrollBarY->SetScrollPos(0);
}

void CGraph::Test()
{
    VFRingBuf<F> *tblPtrBuffers[MAX_BUFFERS];
    FieldGraph.DATA.GetTBLPtrBuffers(tblPtrBuffers);
}

void CGraph::SetViewRepers(BOOL flg_view_reper, int num_buf)
{
    FieldGraph.DATA.SetViewRepers(flg_view_reper, num_buf);
    FieldGraph.Invalidate(false);
}

void CGraph::SetViewData(BOOL flg_view_data, int num_buf)
{
    FieldGraph.DATA.SetViewData(flg_view_data, num_buf);
    UpdateGraph();
}

void CGraph::UpdateReper(BOOL flg)
{
    if(flg) FieldGraph.UpdateReper();
    else FieldGraph.Invalidate(true);
}

void CGraph::SynchronizeGraph(double min_x, double max_x)
{
    FieldGraph.SynchronizeGraph(min_x, max_x);
}

void CGraph::SetNumStrich(int nStrichX, int nStrichY)
{
    FieldGraph.SetNumStrich(nStrichX, nStrichY, NULL);
    sclHor.SetNumStich(nStrichX, NULL);
    sclVert.SetNumStich(nStrichY, NULL);
    UpdateGraph();
}

void CGraph::SetNameGrf(int num_grf, CString sname)
{
    FieldGraph.SetNameGrf(num_grf, sname);
}
VFRingBuf<F>* CGraph::GetVXBuffer()
{
	return FieldGraph.DATA.GetBuf(-1);
}

BOOL CFieldGraph::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
    // TODO: Add your message handler code here and/or call default

    return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CGraph::GetRectFieldGrf(CRect& rec)
{
	if(FieldGraph.m_hWnd != NULL)
	{
		FieldGraph.GetClientRect(&rec);
		return true;
	}
		
	return false;
}

int CGraph::ClearBeginDataY(int num_point_for_delete)
{
	return FieldGraph.DATA.ClearBeginDataY(num_point_for_delete);
}
int CGraph::ClearBeginDataX(int num_point_for_delete)
{
	return FieldGraph.DATA.ClearBeginDataX(num_point_for_delete);
}

void CGraph::ResetFlagsClearedY()
{
	FieldGraph.DATA.ResetFlagsClearedY();
	return;
}

int CGraph::ClearBeginDataXY(int num_point_for_delete)
{
	F minX, maxX;
	F diapazon = FieldGraph.DATA.GetMinMaxDiapazon(-1, minX, maxX);
	int nn = FieldGraph.DATA.ClearBeginDataXY(num_point_for_delete);
	FieldGraph.DATA.GetElement(-1, 0, minX);
	SetMinMaxXDiapazon(minX, minX + diapazon);
	if(m_hWnd) Invalidate(true);
	return nn;
}

double CGraph::GetLifeTime(int num_buf, double start_time, double end_time)
{
	return FieldGraph.DATA.GetLifeTime(num_buf, start_time, end_time);
}

double CGraph::GetDrv(int num_buf, double dX)
{
	return FieldGraph.DATA.GetDrv(num_buf, dX);
}

