#if !defined(AFX_GRAPH_H__09F2D8E3_D1BD_11D2_8632_0040C7952451__INCLUDED_)
#define AFX_GRAPH_H__09F2D8E3_D1BD_11D2_8632_0040C7952451__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Graph.h : header file
//
#include <afxcmn.h>
#include <afxdisp.h>
#include "RingBuff.h"

#define PM_CHANGERECTRANGE		WM_USER + 100
#define PM_SETSCALETIME			WM_USER + 104
#define PM_EDITPOINTGRAPH		WM_USER + 108
#define PM_EDITPOINT			WM_USER + 112
#define PM_CHANGESCROLL			WM_USER + 116
#define PM_R_BUTTON_DOWN		WM_USER + 144
#define PM_SHOW_PANEL			WM_USER + 146
#define PM_STYLE_GRF_CHANGED	WM_USER + 154
#define PM_CHANGE_RECT_RANGE	WM_USER + 160
#define PM_SYNCHRONIZE_GRAPH   	WM_USER + 172
#define PM_EDIT_MARKER		   	WM_USER + 174

#define MAXSCROLLY 10000
#define MAXSCROLLX 10000

class PrinterParam
{
public:
	int prn_npix_x_p;
	int prn_npix_y_p;
	int LpgX;
	int LpgY;

	int w_x_mm;
	int w_y_mm;
	int w_x_p;
	int w_y_p;
	int sh_x_p0;
	int sh_y_p0;
	int sh_x_p;
	int sh_y_p;

	float sh_x_mm;
	float sh_y_mm;

	CRect rec_screen;
	float k_x;
	float k_y;
	CRect rec_prn;

	CDC *pDC;


	PrinterParam()
	{
		prn_npix_x_p = 4676;
		prn_npix_y_p = 6814;
		LpgX = 600;
		LpgY = 600;

		w_x_mm = 197;
		w_y_mm = 288;

		w_x_p = 4653;
		w_y_p = 6803;
		sh_x_p0 = 11;
		sh_y_p0 = 5;
		sh_x_p = sh_x_p0;
		sh_y_p = sh_y_p0;

		sh_x_mm = 0.46566668f;
		sh_y_mm = 0.21166667f;

		rec_prn = GetRectPrn();
		float k_x = prn_npix_x_p/rec_screen.Width();
		float k_y = prn_npix_y_p/rec_screen.Height();

		rec_prn = CRect(sh_x_p0, sh_y_p0, prn_npix_x_p - 1, prn_npix_y_p - 1);
		pDC = NULL;
	}

	PrinterParam(CDC* pDC)
	{
		GetParam(pDC);
	}

	void GetParam(CDC* pDC)
	{
		prn_npix_x_p = pDC->GetDeviceCaps(HORZRES);
		prn_npix_y_p = pDC->GetDeviceCaps(VERTRES);
		LpgX = pDC->GetDeviceCaps(LOGPIXELSX);
		LpgY = pDC->GetDeviceCaps(LOGPIXELSX);

		w_x_mm = pDC->GetDeviceCaps(HORZSIZE);
		w_y_mm = pDC->GetDeviceCaps(VERTSIZE);
		w_x_p = w_x_mm*(LpgX/25.4);
		w_y_p = w_y_mm*(LpgY/25.4);
		sh_x_p0 = (prn_npix_x_p - w_x_p)/2;
		sh_y_p0 = (prn_npix_y_p - w_y_p)/2;
		sh_x_p = sh_x_p0;
		sh_y_p = sh_y_p0;

		sh_x_mm = ((prn_npix_x_p - w_x_p)/2)/(LpgX/25.4);
		sh_y_mm = ((prn_npix_y_p - w_y_p)/2)/(LpgX/25.4);

		rec_screen = GetRectPrn();

		k_x = prn_npix_x_p/rec_screen.Width();
		k_y = prn_npix_y_p/rec_screen.Height();
		rec_prn = CRect(sh_x_p0, sh_y_p0, prn_npix_x_p - 1, prn_npix_y_p - 1);
	}
	CRect GetRectPrn()
	{
		CWnd* pWnd = CWnd::FromHandle(GetDesktopWindow());
		pWnd->GetClientRect(rec_screen);
		return rec_screen;
	}

	CRect GetDrawRectPrn(CDC* pDC, float shxmm_l, float shxmm_r, float shymm_t, float shymm_b)
	{
		GetParam(pDC);
		sh_x_p = (shxmm_l + sh_x_mm)*LpgX/25.4;
		rec_prn.left = sh_x_p;
		sh_y_p = (shymm_t + sh_y_mm)*LpgY/25.4;
		rec_prn.top = sh_y_p;
		rec_prn.right = (prn_npix_x_p - 1) - shxmm_r*LpgX/25.4;
		rec_prn.bottom = (prn_npix_y_p - 1) - shymm_b*LpgX/25.4;;
		return rec_prn;
	}
	void CalcSizeFontPrn(CDC* pDC, int &f_szx, int &f_szy)
	{
		GetParam(pDC);
		f_szx *= k_x;
		f_szy *= k_y;
	}
	CRect OffsetLeftTopInScreen(int sh_l, int sh_t)
	{
		sh_x_p += sh_l;// * k_x;
		rec_prn.left = sh_x_p;
		sh_y_p += sh_t;// * k_y;;
		rec_prn.top = sh_y_p;
		return rec_prn;
	}
};

enum typeGraphView
{
    gpoint	= 0,
    gpoint1	= 1,
    gpoint2	= 2,
    gline	= 3,
    gline1	= 4,
    gstick	= 5
};
#define GRF_EXTERN_SYNCHRO	0x0000000000000001L 
#define FLD_SELECT_ONLY_Y	0x0000000000000002L	
#define GRF_X_SCROLL		0x0000000000000004L	
#define GRF_Y_SCROLL		0x0000000000000008L	

#define GRF_BTN_X_NAME		0x0000000000000010L	
#define GRF_BTN_Y_NAME		0x0000000000000020L	
#define GRF_ORIENT_X_SCL	0x0000000000000040L	
#define GRF_ORIENT_Y_SCL	0x0000000000000080L	

#define GRF_RAMKA_AR_GRF	0x0000000000000100L	
#define GRF_PANEL			0x0000000000000200L	
#define GRF_ORIENT_FLD		0x0000000000000400L	
#define GRF_NAME_X			0x0000000000000800L	

#define GRF_NAME_Y			0x0000000000001000L	
#define GRF_INV_NAME_X		0x0000000000002000L	
#define GRF_INV_NAME_Y		0x0000000000004000L	
#define GRF_ENABLE_DGT		0x0000000000008000L	

#define FLD_STYLE_TYME		0x0000000000010000L	
#define FLD_SCALE			0x0000000000020000L	
#define FLD_GREED			0x0000000000040000L	
#define FLD_ORIENT			0x0000000000080000L	

#define FLD_ALERT1			0x0000000000100000L	
#define	FLD_ALERT2			0x0000000000200000L	
#define FLD_ALERT3			0x0000000000400000L	
#define FLD_ALERT4			0x0000000000800000L	

#define	FLD_TYPE_POINT		0x0000000001000000L	
#define FLD_CLR_FROM_MARKER	0x0000000002000000L	
#define FLD_TYPE_SCALE		0x0000000004000000L	
#define FLD_SIGN_DATA		0x0000000008000000L	

#define FLD_DATA_FIX		0x0000000010000000L	
#define FLD_DATA_ON_FIELD	0x0000000020000000L	
#define FLD_LINE_JAGGIES	0x0000000040000000L	
#define GRF_RESERV31		0x0000000080000000L	

#define SCLX_ORIENT			0x0000000100000000L	
#define SCLX_ORIENT_INVERT	0x0000000200000000L	
#define SCLX_BASE_LINE		0x0000000400000000L	
#define SCLX_MAIN_STRICH	0x0000000800000000L	

#define SCLX_SLAVE_STRICH	0x0000001000000000L	
#define SCLX_COLOR_FON		0x0000002000000000L	
#define SCLX_NAME			0x0000004000000000L	
#define SCLX_VIEW			0x0000008000000000L	

#define SCLX_VIEW_DGT		0x0000010000000000L	
#define SCLX_BUTTON_NAME	0x0000020000000000L	
#define SCLX_TIME			0x0000040000000000L	
#define SCLX_ORIENT_FONT_M	0x0000080000000000L	
#define SCLX_ORIENT_FONT_N	0x0000100000000000L	
#define SCLX_TYPE_SCALE		0x0000200000000000L	
#define SCLX_SIGN_DATA		0x0000400000000000L	
#define SCLX_DATA_ON_FIELD	0x0000800000000000L	

#define SCLY_ORIENT			0x0001000000000000L	
#define SCLY_ORIENT_INVERT	0x0002000000000000L	
#define SCLY_BASE_LINE		0x0004000000000000L	
#define SCLY_MAIN_STRICH	0x0008000000000000L	

#define SCLY_SLAVE_STRICH	0x0010000000000000L	
#define SCLY_COLOR_FON		0x0020000000000000L	
#define SCLY_NAME			0x0040000000000000L	
#define SCLY_VIEW			0x0080000000000000L	

#define SCLY_VIEW_DGT		0x0100000000000000L	
#define SCLY_BUTTON_NAME	0x0200000000000000L	
#define SCLY_TIME			0x0400000000000000L	
#define SCLY_ORIENT_FONT_M	0x0800000000000000L	

#define SCLY_ORIENT_FONT_N	0x1000000000000000L	
#define SCLY_TYPE_SCALE		0x2000000000000000L	
#define SCLY_SIGN_DATA		0x4000000000000000L	
#define SCLY_DATA_ON_FIELD	0x8000000000000000L	
#define MAX_NUM_REPERS 10000

typedef struct {
    DWORD	
flOrient		:1,
flOrientInvert	:1,
flBaseLine		:1,
flMainStrich	:1,

flSlaveStrich	:1,
flColorFon		:1,
flName			:1,
flView			:1,

flViewDgt		:1,
flButtonName	:1,
flViewTime		:1,
flOrientFontM	:1,

flOrientFontN	:1,
flTypeScale		:1, 
flSignData		:1,	
flDataOnField	:1; 
} ScaleFlags;

union dwordScaleFlags {
    DWORD dwFlags;
    ScaleFlags bitFlags;
};

typedef double F;
typedef int I;

class CValue : public CToolTipCtrl
{
    // Construction
public:
    CValue();

    // Attributes
public:

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CValue)
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CValue();

    // Generated message map functions
protected:
    //{{AFX_MSG(CValue)
    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// COuot window
class COuot : public CStatic
{
public:
    COuot();
protected:
    LOGFONT m_logfontN;
    CFont	fFontName;
    COleDateTimeSpan dleta;

    CString GetMonth(WORD& wmonth);

public:
    LPVOID m_pFld;
    CPoint m_point;
    BOOL m_bType;
public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(COuot)
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~COuot();
    // Generated message map functions
protected:
    //{{AFX_MSG(COuot)
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
class CFieldGraph;
class CGraph;
class CScale;

class CPanel : public CStatic
{
	DECLARE_DYNAMIC(CPanel)
	CGraph *pGraph;
	CFieldGraph *pFieldGraph;
	CScale *pHScale, *pVScale;
	CScrollBar *pScrollBarX, *pScrollBarY;

	CButton m_point_view, m_greed_view, m_fix_data;
	CRect rec_ch;
	int size_check, index_check;
	COLORREF colorText;

	void RecalcSize();
	void FindePointerGrf(CPoint point);
	void DrawPointerGrf(CDC* pDC);

protected:
	friend class CGraph;
	friend class CScale;
	friend class CFieldGraph;

public:
	CPanel();
	virtual ~CPanel();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCheckPoint();
	afx_msg void OnBnClickedCheckGreed();
	afx_msg void OnBnClickedCheckFixData();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

class CScale : public CStatic
{
    DECLARE_DYNCREATE(CScale)
public:
    CScale();
protected:
    CScale(CString sName, UINT Nstrich);
    friend class CFieldGraph;
    friend class CGraph;
	friend class CPanel;
protected:
    ScaleFlags sclStyle;
    int sclNstrich;
    CString sclName;
    CPoint basePoint;
    CSize bazeSize;
    CSize sizeMainStrich, sizeSlaveStrich;
    COLORREF	colorBaseLine, colorMainStrich, colorSlaveStrich, colorFon, colorValue, 
        colorName, colorText, colorHeadAlert, colorHead;
    CSize uScale;
    UINT typeSlideScale;
    CPen pnBaseLine, pnMainStrich, pnSlaveStrich;
    LOGFONT m_logfontM, m_logfontN;
    CFont	fFontMain, fFontName;
    int shiftStrichText;
    int strichScaleMain[200];
protected:
    CFieldGraph *m_fldGraph;
    F Fmin, Fmax;
    COleDateTime dleta;
protected:
    LOGFONT SetFontM(LOGFONT& lf);
    LOGFONT SetFontN(LOGFONT& lf);
    int SetSizeFontM(int sz);
    int SetSizeFontN(int sz);
    BOOL SetOrientFontM(BOOL flg);
    BOOL SetOrientFontN(BOOL flg);
	void UpdateOrientFont();

    void SetSize(CSize sclSize){bazeSize = sclSize;};
    void SetPos(CPoint sclPos){basePoint = sclPos;};
    CRect GetInflateSizeA(CDC* pDC);
    CRect GetInflateSize();
    void SetColorBaseLine(COLORREF color) {colorBaseLine = color;};
    void SetColorMainStrich(COLORREF color){colorMainStrich = color;};
    void SetColorSlaveStrich(COLORREF color){colorSlaveStrich = color;};
    void SetColorFon(COLORREF color);
    void SetColorValue(COLORREF color){colorValue = color;};
    void SetColorName(COLORREF color){colorName = color;};
    void SetColorText(COLORREF color){colorText = color;};
    void SetMinMax(F FminX, F FmaxX);
    void ChangeString();
    void SetShowFlag(BOOL flag) {sclStyle.flView = flag;};
    void SetOrientS(BOOL flag);
    BOOL IsShowScl() {return sclStyle.flView;};
    void SetBtnName(BOOL flag) {sclStyle.flButtonName = flag;};
    void SetViewTime(BOOL flag) {sclStyle.flViewTime = flag;};
    void SetStyle(DWORD dwStyle);
	void ResetStyle(DWORD dwStyle);
    DWORD GetStyle();

    void SetNumStich(UINT nStrich, CRect* p_rec);
    void SetNameScl(CString sName) {sclName = sName;};

    int GetNumStich() {return sclNstrich;};
    CString GetNameScl() {return sclName;};
    void SetColorHaderAlert(COLORREF color);

    COLORREF GetColorHaderAlert(){return colorHead;};

    void DrawXScl(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift);
    void DrawYScl(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift);
    void DrawGrf(CDC* pDC, CWnd *pWnd = NULL, F sclX = 1.0, F sclY = 1.0, CSize *prnShift = NULL);
    void SetOrientScale(BOOL flag) {sclStyle.flOrient = flag;};
    void SetInvertScale(BOOL flag) {sclStyle.flOrientInvert = flag;};
    void SetViewNameD(BOOL flg);


    void SetPrintFont(F scl, bool flOrientN, bool flOrientM);
    void SetPrintFontP(F scl);
    void SetNormalFont();
    void RenameScale(CString sName);
    COLORREF GetColorFon() {return colorFon;};
    BOOL GetOrientFontM(){ return sclStyle.flOrientFontM;};
    BOOL GetOrientFontN(){ return sclStyle.flOrientFontN;};

    void CalcScaleStrichX(CRect *rec = 0);
    void CalcScaleStrichY(CRect* p_rec);


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CScale)
    //}}AFX_VIRTUAL

protected:
    CScale& operator=(CScale& scl);
public:
    static CSize GetSizeText(CDC *pDC, const CString& str, CFont *pFont, BOOL flgView);
    virtual ~CScale();
    // Generated message map functions
protected:
    //{{AFX_MSG(CScale)
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    virtual void Serialize(CArchive& ar);
};
#define MAX_BUFFERS 100
#define MAX_MARKERS 10000

typedef struct {
    BOOL bflData;
    F fValue, time;
} DigitParam;

typedef struct {
    int startDataView, endDataView, sizeViewX;
    F FminY[MAX_BUFFERS], FmaxY[MAX_BUFFERS];
    F FminX, FmaxX;
} ParamForSavesAndRestores;

typedef struct {
    F YR;
    F XR;
    CString TxtR;
} ReperParam;

typedef struct {
    F XR;
    CString TxtR;
} ReperParamV;

typedef struct {
    DWORD	
flViewTime		:1,
flScale			:1,
flGreed			:1,
flOrient		:1,

flAlert1		:1,
flAlert2		:1,
flAlert3		:1,
flAlert4		:1,

flTypePoint		:1,
flColorFromMark	:1,
flTypeScale		:1,
flSignData		:1,

flFixData       :1,
flDataOnField	:1;
} GraphFieldStyleFlags;

union GraphFieldFlags {
    DWORD dwFlags;
    GraphFieldStyleFlags bitFlags;
};
#include <afxtempl.h>

class CDataGraph : public CObject
{
    CDataGraph();
    CString sName;
    int sizeBuf, sizeData;
    F* pData;
    virtual ~CDataGraph();
    CDataGraph& operator=(CDataGraph& pdata);
};
typedef CTypedPtrList<CObList, CDataGraph*> CDataGrf;
class CGraph;

//------------------------------------------------------------------------
#define FL_GRF_MINMAX	0x00000001  
#define FL_GRF_DIAPAZON	0x00000002  
#define FL_GRF_TEXT		0x00000004  
#define FL_GRF_BUF_X	0x00000010  
#define FL_GRF_ALL		0x00000017  

typedef struct {
    DWORD	
fl_min_max		:1,
fl_diapazon		:1,
fl_text_grf		:1,
fl_buf_x		:1,
fl_buf_x_only	:1;
} GRF_FLAGS_SET_MINMAX;

union GRF_FLAGS_MINMAX {
    DWORD dwFlags;
    GRF_FLAGS_SET_MINMAX bitFlags;
};

typedef struct{
    GRF_FLAGS_MINMAX flags_minmax;
    F Fmin, Fmax, FminS, FmaxS;
    CString textGrf;
	BOOL flag_undo;
} GRF_MINMAX;
//------------------------------------------------------------------------

class CFieldGraph : public CStatic
{
    DECLARE_DYNCREATE(CFieldGraph)
	GRData<F> DATA;
public:
    CFieldGraph();
    friend class COuot;
    void static ConvToScale(CRect& rec, F sclX, F sclY);

    static CSize *prnShiftXY;
    static CRect slctRect;

    void RedrawGraph();

protected:
    CFieldGraph(UINT NstrichX, UINT NstrichY);
    friend class CGraph;
    friend class CScale;
	friend class CPanel;
    CDataGrf lData;
    BOOL m_flgExtBuf;

protected:
    COuot m_val;
    HWND hWndGraph;
    CGraph *pGraph;
    DigitParam dgParam;
    GraphFieldFlags fldStyle;
    typeGraphView typeView;
    CScale *m_psclHor, *m_psclVert;

    LOGFONT lfntTextReper;
    CFont	fFontTextReper;
    LOGFONT lfntTextGrf;
    CFont	fFontTextGrf;

    ParamForSavesAndRestores *saveList;

    int indexList, maxCountList;
    int indexListMax;
    CRect fieldRec;
    int nMax, nPage, nPos;
    HCURSOR cursorNorm, cursorCross;
    ReperParam *ptsReper;
    ReperParamV *pvReper;
    F dAlert1, dAlert2, dAlert3, dAlert4;

    int sizeFragm, startFragm;
    int numRepers, numRepersTS;

    int sclNstrichX, sclNstrichY;
    CSize sizeMainStrich, sizeSlaveStrich;

    COLORREF  colorScale, colorMainStrich, colorSlaveStrich, colorSelectGraph, 
        colorFon, colorGreed, colorSelectFon,
        colorAlert1, colorAlert2, colorAlert3, colorAlert4, 
        colorReper, colorTextReper, colorTextGrf;

    COLORREF colorGraph[MAX_BUFFERS];

    CPen pnScaleLine, pnMainStrich, pnSlaveStrich, pnSelectGraph, pnGreed;
    CPen pnAlert1, pnAlert2, pnAlert3, pnAlert4, pnReper;

    F ScaleFX, ScaleFY;
    BOOL flagtrack;
    CRect slctRectPush, slctRectOld;
    BOOL flagfragment, flagselected, direct;
    int XpointOld;

    COleDateTimeSpan dleta;
    COleDateTime olTime;

    CRect fieldRecTST;
    BOOL bflgTest;
    F saveEditX;
    F saveEditY;
    int	saveEditIndex;	
    int typeGraph;
    CString m_textGrf;
    F m_fkofScroll;

    int x_save, y_save, i_min, i_max;
    int xx, xx0, xx1, xx2, yy, yy0;
    int i_count;
    CPoint pointEdit;
    BOOL flag_scroll;
    BOOL flag_rbtn_down;
//...........................................
    CRect m_rec;
    F m_Xdata, m_Ydata, m_FminX, m_FminY, m_FmaxY, m_maxF, m_stepX, m_kof;
    VFRingBuf<F> *m_pBuffer;
    CDC *m_pDCMem, *m_pDCC;
	CBitmap *m_pBitmap, *m_pbitmapOld;
	CRect rectClip;
	CSize m_sizeBitmap;
	BOOL paint_lock;
    BOOL m_flOrient, m_flSignData;
	DWORD time_paint;
	F nearest_distance;
	int index_x, index_y, index_x0, index_y0;
protected:
    F GetMax(int N, F *V);
    F NormData(int N, F *V);

protected:
    void SavePar();
    BOOL UndoBack();
    BOOL UndoForward();
	void UndoStart();
    void ClearUnDo();

    int SetTypeGraph(int gtype);

    BOOL TestSize(F stepX);

    void inline GetClientRectMy(CRect *rec);
    void DrawLineScale(CDC* pDC, CWnd *pWnd = NULL, F sclX = 1.0, F sclY = 1.0, CSize *prnShift = NULL);
	void DrawLineScaleA(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift, F Fmin, F Fmax);
    void DrawGreed(CDC* pDC, CWnd *pWnd = NULL, F sclX = 1.0, F sclY = 1.0, CSize *prnShift = NULL);
    void DrawGraph(CDC* pDC, CWnd *pWnd = NULL, F sclX = 1.0, F sclY = 1.0, CSize *prnShift = NULL);
    void DrawLinesAlert(CDC* pDC, CWnd *pWnd = NULL, F sclX = 1.0, F sclY = 1.0, CSize *prnShift = NULL);

    UINT CreateBufer(UINT sizeBuf, F* pBufY, F* pBufX);
    void CreateBufX(UINT size_buf, UINT size_data, F* pBufX);
    void CreateBufX(VFRingBuf<F>* pBufX);
    void CreateBufX(FRingBuf<F>* pBufX);
    void SetAdrBufX(UINT base_pos, VFRingBuf<F>& pBufX);
    void SetAdrBufX(UINT base_pos, FRingBuf<F>& pBufX);
    BOOL InitDataX(int size_data, F* pBufX);
    void InitDataX(VFRingBuf<F>& pBufX);
    void InitDataX(FRingBuf<F>& pBufX);

    UINT CreateBufY(UINT size_buf, UINT size_data, F* pBufY);
    UINT CreateBufY(VFRingBuf<F>* pBufY);
    UINT CreateBufY(FRingBuf<F>* pBufY);
    void SetAdrBufY(UINT num_buf, UINT base_pos, VFRingBuf<F>& pBufY);
    void SetAdrBufY(UINT num_buf, UINT base_pos, FRingBuf<F>& pBufY);
    UINT SetAdrBufY(UINT base_pos, FRingBuf<F>& pBufY);
    UINT CreateBufYWithExtInpBuf(UINT base_pos, VFRingBuf<F>& pBufY);
    UINT CreateBufYWithExtInpBuf(UINT base_pos, FRingBuf<F>& pBufY);
    void InitDataY(UINT num_buf, UINT size_data, F* pBufY);
	void InitDataY0(UINT num_buf, UINT beg_index, UINT size_data, F* pBufY);
    void InitDataY(UINT num_buf, VFRingBuf<F>& pBufY);
    void InitDataY(UINT num_buf, FRingBuf<F>& pBufY);
    UINT InitDataYWithName(CString sName, UINT sizeBuf, F* inBufY);

    F GetScaleXStart();
    void DestroyBuf();
    void AddData(UINT num_buf, UINT size_data, UINT base_pos, F* inBuf);
    void SetMinMaxT(const F scFmin, const F scFmax);
    void SetMinMaxXDiapazon(const F FminX, const F FmaxX);
    void SetMinMaxF(F scFmin, F scFmax, int num_grf = 0);

    void GetMinMaxT(F& FminX, F& FmaxX);
    void GetMinMaxF(F& Fmin, F& Fmax, int num_grf = 0);
    void GetMinMaxFDiapazon(F& Fmin, F& Fmax, int num_grf = 0);
    void SetMinMaxFDiapazon(F Fmin, F Fmax, int num_grf = 0);

    BOOL HSetScroolGraph(int Pos, int nMax);
    BOOL VSetScroolGraph(int Pos, int nMax);
    BOOL XSetScrollGraph(int Pos, int nMax);
    BOOL YSetScrollGraph(int Pos, int nMax);
    BOOL inline SetSizeVievX();

    BOOL ChangeHScale(F deltaScale);
    BOOL ChangeVScale(F deltaScale);
    int GetHPos(int nIndex, int nPage, int nMax);
    int GetVPos(F fValue, int nPage, int nMax);

	BOOL DrawMarker(CDC* pDC, TMarker<F> *pMarker, CFont& fnt);
	void DrawRepers(CDC* pDC, CRect rec, BOOL flg_redraw = false);

    BOOL HTestRange();
    BOOL VTestRange();
    void SetGreed(BOOL greed);
    BOOL TestGreed();
    void SetScaleLine(BOOL scaleline);
    void SetStyleLineFunc(UINT stypeView);
    void SetRectRangeA();
	void SetRectRange();
	void SetRectRangeExternal();
    int IndexTime(CPoint point, F& dTime, F& fVal);
    void IndexTime(CPoint point, F& dTime, F& fVal, int &indexp);
    int GetIndex(CPoint point);
    int GetIndex(F tim);
    void DoHScroll(int shift);
    BOOL inline PrepareDataXForDraw();
    BOOL inline PrepareDataYForDraw();
    void inline GetXY(POINT& p);
    void inline BackConversionXY(POINT& p);
    void inline BackConversionXY(int& x, int& y);
    POINT inline MoveToA();
    void inline DrawPointA();
    void inline DrawPointB();
    void inline DrawLoosePoint();
    void inline DrawLineA();
    POINT inline DrawLineB();
    BOOL DrawLastData(int num_grf, int sz_data);
    void DrawLastDataN(int num_grf, int sz_data);
    void AddPoint(UINT num_grf, F mfnc, BOOL flg_scl = false);
    void Pinok(int sz_pinok);
    void AddPointXY(F Yvalue, F Xvalue, UINT num_grf = 0);
    void SetViewRangeTime(F startViewTime, F rangeViewTime, F dtime);
    void SetInvert90(BOOL flag) {fldStyle.bitFlags.flOrient = flag;};
    void SetViewTime(BOOL flag) {fldStyle.bitFlags.flViewTime = flag;};
    void SetStyle(DWORD dwStyle);
	void ResetStyle(DWORD dwStyle);
    DWORD GetStyle() {return fldStyle.dwFlags;};

	BOOL CreateMemoryPaintDC(CDC* pDC);
    void DrawGrf(CDC* pDC, CWnd *pWnd = NULL, F sclX = 1.0, F sclY = 1.0, CSize *prnShift = NULL);
    void SetTP();
    void ChangeScale(FRingBuf<F>* pBuff, GRF_MINMAX& par);
    void ClearBufer();

    COLORREF SetColorGreed(COLORREF clr);
	void SetStyleGreed(int nPenStyle, int nWidth);
    COLORREF SetColorFon(COLORREF clr);
    COLORREF SetColorScale(COLORREF clr);
    COLORREF SetColorGraph(COLORREF clr);
    COLORREF SetColorGraphEX(UINT NumGrf, COLORREF color);
    BOOL SetViewGreed(BOOL flg);
    BOOL SetViewGreedScale(BOOL flg);
    BOOL SetScaleLeft(BOOL flg);
    BOOL SetScaleCentr(BOOL flg);

    void SetNumStrich(int nStrichX, int nStrichY, CRect* p_rec);
    void SetNameGrf(int numDiagram, CString sname);

    COLORREF SetColorReper(COLORREF color);
    COLORREF SetColorTextReper(COLORREF color);
    COLORREF SetColorTextGrf(COLORREF color);
    void AddReper(F XR, CString TxtR);
    void AddReper(F YR, F XR, CString TxtR);
    void ClearRepers();
    LOGFONT SetFontReper(LOGFONT& lf);
    LOGFONT SetFontTextGrf(LOGFONT& lf);

    TMarker<F>* GetEditPoint(int& inex_x, int& inex_y, int& num_buf);
    TMarker<F>* FindeMarker(int num_buf, int index_point);
    TMarker<F>* SetMarker(int num_buf, int index, COLORREF clr, char type_ch);
    void SetEditPoint(F XX, F YY, int indexp);

    int inline GetSizeYScroll(int maxVertScrollSize);
    int inline GetSrollYPos(int maxVertScrollSize);
    int inline GetSizeXScroll(int maxXScrollSize);
    int inline GetSrollXPos(int maxXScrollSize);

    void SetColorDiagram(int numDiagram, COLORREF clr, BOOL flag_color);
    int GetThicknessDiagram(int numDiagram);
    void SetThicknessDiagram(int numDiagram, int thick);
    void DrawGraphText(CDC* pDC, CWnd *pWnd = NULL, F sclX = 1.0, F sclY = 1.0, CSize *prnShift = NULL);
	CString GetStrDataX(F value);
	void DrawDataOnField(CDC* pDC, CWnd *pWnd = NULL, F sclX = 1.0, F sclY = 1.0, CSize *prnShift = NULL);
    void UpdateViewRange();
    F GetDistancePoint(CPoint last_point, CPoint pp);
    void UpdateReper();
    void SynchronizeGraph(double min_x, double max_x);
    void SetMinMaxY(F Fmin, F Fmax, int num_grf);
    int SetMouseValue(CPoint& point);
	int CFieldGraph::GetNearestPoint(CPoint& point);

    GRF_MINMAX SetParMinMax(int num_grf, GRF_MINMAX& par);
    GRF_MINMAX GetParMinMax(int num_grf);
    void GetMinMaxDiapazonDef(int num_grf, F& Fmin, F& Fmax);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFieldGraph)
    //}}AFX_VIRTUAL

protected:
    CFieldGraph& operator=(CFieldGraph& fld);

public:
    virtual ~CFieldGraph();

    // Generated message map functions
protected:
    //{{AFX_MSG(CFieldGraph)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnEditMarker(WPARAM wParam, LPARAM lParam);
   //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
public:
    virtual void Serialize(CArchive& ar);
};
/////////////////////////////////////////////////////////////////////////////
// CSladeBar window
class CSladeBar : public CStatic
{
public:
    CSladeBar();
public:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSladeBar)
    //}}AFX_VIRTUAL

public:
    virtual ~CSladeBar();
protected:
    //{{AFX_MSG(CSladeBar)
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
typedef struct {
    ULONG64	
flViewHorScl	:1,		
flViewVertScl	:1,		
flHScroll		:1,		
flVScroll		:1,		
flButtonXName	:1,		
flButtonYName	:1,		
flOrientSH		:1,		
flOrientSV		:1,		
flRamkaArGrf	:1,		
flPanel			:1,		
flOrientGRF		:1,		
flViewNameX		:1,		
flViewNameY		:1,		
flNameInvX		:1,		
flNameInvY		:1,		
flEnableDgt		:1,		
flViewTime		:1,		
flScale			:1,		
flGreed			:1,		
flOrientFLD		:1,		
flAlert1		:1,		
flAlert2		:1,		
flAlert3		:1,		
flAlert4		:1,		
flTypePoint		:1,		
flColorFromMark	:1,		
flTypeScale		:1,		
flSignData		:1,		
flFixData		:1,		
flDataOnField	:1,		
flReserv30		:1,		
flReserv31		:1,		
flOrientX		:1,		
flOrientInvertX	:1,		
flBaseLineX		:1,		
flMainStrichX	:1,		
flSlaveStrichX	:1,		
flColorFonX		:1,		
flNameX			:1,		
flViewX			:1,		
flViewDgtX		:1,		
flButtonNameX	:1,		
flViewTimeX		:1,		
flOrientFontMX	:1,		
flOrientFontNX	:1,		
flTypeScaleX	:1,		
flSignDataX		:1,		
flDataOnFieldX	:1,		
flOrientY		:1,		
flOrientInvertY	:1,		
flBaseLineY		:1,		
flMainStrichY	:1,		
flSlaveStrichY	:1,		
flColorFonY		:1,		
flNameY			:1,		
flViewY			:1,		
flViewDgtY		:1,		
flButtonNameY	:1,		
flViewTimeY		:1,		
flOrientFontMY	:1,		
flOrientFontNY	:1,		
flTypeScaleY	:1,		
flSignDataY		:1,		
flDataOnFieldY	:1;		
} GraphStyleFlags;

typedef struct {
	ULONG64	
styleGrf		:16,
styleFieldGrf	:16,
styleSclX		:16,
styleSclY		:16;
} GraphStyleComponent;

union GraphFlags {
    ULONG64 dwFlags;
    GraphStyleFlags bitFlags;
	GraphStyleComponent style;
};

class CGraph : public CStatic
{
    DECLARE_DYNCREATE(CGraph)
public:
    CGraph();
    CGraph(CString sNameX, CString sNameY, UINT NstrichX, UINT NstrichY, ULONG64 style = 0x3);

    CFieldGraph FieldGraph;
    CScale sclHor;
    CScale sclVert;
	int num_self;
    int loc_data_for_print;
    static int scroll_pos, scroll_max;
protected:
	friend class CPanel;
    GraphFlags graphStyle;
    CString sNameX, sNameY;
    CSladeBar stSlideHorBar, stSlideVertBar;
    CScrollBar sbarHor0, sbarVert0, *p_ScrollBarX, *p_ScrollBarY;
	CPanel tuning;

    F sclX, sclY;
    CSize prnShift;

    HCURSOR cursorNorm, cursorHand;

    COLORREF colorFon, colorGraph, colorGreed, colorFragment, colorScale, colorFontX, colorFontY, colorHeadX, colorHeadY;

    CSize szSlot1, szSlot2, szSlot3, szSlot4, szSlotName, szHorScroll, szVertScroll, size_check;
    CRect	recHorScl, recVertScl, recGraphFld, recHorScroll, recVertScroll,
        recSlideHorBar, recSlideVertBar, RectUnDoF, RectUnDoB, rec_tuning;

    LOGFONT logfontNX, logfontNY, logfontDgt;
    CFont	fFontNameX, fFontNameY, fFontDgt;

    COleDateTimeSpan dleta;
    BOOL flagEdit;

	CDC *m_pDCMem;
	CBitmap *m_pBitmap, *m_pbitmapOld;
	CRect rectClip;
	CSize m_sizeBitmap;
    void GetInflateSizeA(CDC* pDC, CSize &sizeTX, CSize &sizeTY);
    void GetInflateSize(CSize &sizeTX, CSize &sizeTY);
    void OutTextName(CDC *pDC);
    void SetRangeA();
    void SetRange(UINT sizeBuf, int num_buf);
    void OnHScrollP(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) ;
    BOOL SetFlagXScroll(BOOL flg);
    BOOL SetFlagYScroll(BOOL flg);
    BOOL GetFlagXScroll();
    BOOL GetFlagYScroll();

public:
    void RecalcSize();
    void ChangePosition();
	BOOL CreateMemoryPaintDC(CDC* pDC, CDC* pDCMem);
    void DrawGrf(CDC* pDC, CWnd *pWnd = NULL);
    void DrawGrf(CDC* pDC, CWnd *pWnd, F sclX, F sclY, CSize *prnShift);
    void ShowScroll(BOOL flShowScrollH, BOOL flShowScrollV);
    ULONG64 SetStyle(ULONG64 dwStyle, BOOL flRedraw = false);

	template <class type>
	void SubstituteBitStyle(ULONG64 dwStyle, type value, BOOL flRedraw = false)
	{
		if(value == 0) ResetStyle(dwStyle, flRedraw);
		else SetStyle(dwStyle, flRedraw);
	}

	ULONG64 ResetStyle(ULONG64 g_style, BOOL flRedraw = false);
    ULONG64 GetStyle();
	BOOL GetBitStyle(ULONG64 g_style);

    void PreparePrintingParam(CDC* pDC, CWnd *pWnd, BOOL falagSize = true);
    void UpdateGraph();
    void UpdateRectRangeExternal(BOOL flRedraw);

    void SetScaleLine(BOOL scaleline);
    BOOL EnableDgt(BOOL flg = true);
    void SetColorHaderAlert(COLORREF color);
    COLORREF GetColorHaderAlert();
    COLORREF GetColorHeadAvaria() {return FieldGraph.colorAlert4;};
    COLORREF GetColorHeadMax() {return FieldGraph.colorAlert1;};
    COLORREF GetColorHeadMin() {return FieldGraph.colorAlert2;};
    COLORREF GetColorHeadNorm() {return sclHor.GetColorFon();};
    COLORREF SetColorHeadX(COLORREF clr, BOOL flRedraw = true);
    BOOL SetButtonNameX(BOOL btn, BOOL flRedraw);
    COLORREF SetColorHeadY(COLORREF clr, BOOL flRedraw = true);
    BOOL SetButtonNameY(BOOL btn, BOOL flRedraw);
    void RotFont(BOOL flg, CFont *pFont);
    void SetRot90(BOOL flg);

    void DrawRectsUnDo(CDC *pDC); 
    BOOL ShowGreed(BOOL bFlag, BOOL flRedraw = TRUE);
    void SetStyleLineFunc(UINT stypeView);
    void SetViewRangeTime(F startViewTime, F rangeViewTime, F dtime);
    void GetRecFieldF(CRect *rec);

    void SetMinMaxF(F Fmin, F Fmax, int num_grf = 0);
    void GetMinMaxF(F& FminX, F& FmaxX, int num_grf = 0);
    void GetMinMaxFDiapazon(F& Fmin, F& Fmax, int num_grf = 0);
    void SetMinMaxFDiapazon(F Fmin, F Fmax, int num_grf = 0);

    void ChangeHScale(F deltaScale);
    void ChangeVScale(F deltaScale);

    UINT CreateBufer(UINT sizeBuf, F* pBufY, F* pBufX);
    void CreateBufX(UINT size_buf, UINT size_data, F* pBufX);
    void CreateBufX(VFRingBuf<F>* pBufX);
    void CreateBufX(FRingBuf<F>* pBufX);
    void SetAdrBufX(UINT base_pos, VFRingBuf<F>& pBufX);
    void SetAdrBufX(UINT base_pos, FRingBuf<F>& pBufX);
    BOOL InitDataX(int size_data, F* pBufX);
    void InitDataX(VFRingBuf<F>& pBufX);
    void InitDataX(FRingBuf<F>& pBufX);

    UINT CreateBufY(UINT size_buf, UINT size_data, F* pBufY);
    UINT CreateBufY(VFRingBuf<F>* pBufY);
    UINT CreateBufY(FRingBuf<F>* pBufY);
    void SetAdrBufY(UINT num_buf, UINT base_pos, VFRingBuf<F>& pBufY);
    void SetAdrBufY(UINT num_buf, UINT base_pos, FRingBuf<F>& pBufY);
    UINT SetAdrBufY(UINT base_pos, FRingBuf<F>& pBufY);
    UINT CreateBufYWithExtInpBuf(UINT base_pos, VFRingBuf<F>& pBufY);
    UINT CreateBufYWithExtInpBuf(UINT base_pos, FRingBuf<F>& pBufY);
    void InitDataY(UINT num_buf, UINT size_data, F* pBufY);
	void InitDataY(UINT num_buf, UINT beg_index, UINT size_data, F* inBuf);
    void InitDataY(UINT num_buf, VFRingBuf<F>& pBufY);
    void InitDataY(UINT num_buf, FRingBuf<F>& pBufY);
    UINT InitDataYWithName(CString sName, UINT sizeBuf, F* inBuf);

    F GetScaleXStart();
    void AddData(UINT num_buf, UINT size_data, UINT base_pos, F* inBuf);
    void InitDataXY(UINT num_buf, UINT sizeBuf, F* inBufX, F* inBufY);
    int SetTypeGraph(int gtype);

    void SetGreed(BOOL greed);
    DWORD SetStyleGraph(DWORD dwStyle);
    DWORD GetStyleGraph();
    void DestroyBuf();
    void AddPoint(UINT num_grf, F mfnc, BOOL flg_scl = false);
    void Pinok(int sz_pinok);
    BOOL DrawLastData(int num_grf, int sz_data);
    int GetSizeData();
    void AddPointXY(F Yvalue, F Xvalue, UINT num_grf = 0);
    BOOL UndoBack();
    BOOL UndoForward();
    COLORREF SetColorGreedGraph(COLORREF color, BOOL flRedraw = false);
	void SetStyleGreedGraph(int nPenStyle, int nWidth, BOOL flRedraw);
    COLORREF SetColorFieldGraph(COLORREF color, BOOL flRedraw = false);
    COLORREF SetColorGraph(COLORREF color);
    COLORREF SetColorGraphEX(UINT NumGrf, COLORREF color);
    COLORREF SetColorReperGraph(COLORREF color, BOOL flRedraw = false);
    COLORREF SetColorTextReper(COLORREF clr, BOOL flRedraw = false);
    COLORREF SetColorTextGrf(COLORREF clr, BOOL flRedraw = false);
    void AddReper(F XR, CString TxtR);
    void AddReper(F YR, F XR, CString TxtR);
    void ClearRepers();
    LOGFONT SetFontReper(LOGFONT& lf, BOOL flRedraw = false);
    LOGFONT SetFontTextGrf(LOGFONT& lf, BOOL flRedraw = false);
    CString SetTextGrf(CString sname, BOOL flRedraw = true);

    TMarker<F>* GetEditPoint(int& inex_x, int& inex_y, int& num_buf);
    void SetMarker(int num_buf, int index, COLORREF clr, char type_ch);
    void SetEditPoint(F XX, F YY, int indexp);
    BOOL ShowScale(BOOL bFlag, BOOL flRedraw = TRUE);
    void GetRecFieldHS(CRect *rec);
    void GetRecFieldVS(CRect *rec);
    void RenameVScale(CString sName, BOOL bflgType, LPVOID pBuff);
    void RenameHScale(CString sName, BOOL bflgType, LPVOID pBuff);

    void ChangeScale(CString sName, BOOL bflgType, LPVOID  pBuff, GRF_MINMAX& par);
    COLORREF SetColorBk(COLORREF clr, BOOL flRedraw = true);

    LOGFONT SetFontNameX(LOGFONT& lf);
    LOGFONT SetFontNameY(LOGFONT& lf);

    void SetShowSclH(BOOL bShow);
    void SetShowSclV(BOOL bShow);
    void SetHorScale(BOOL showScale);
    void SetVertScale(BOOL showScale);
    void SetMinMaxT(const F FminX, const F FmaxX);
    void SetMinMaxTR(const F FminX, const F FmaxX);
    void SetMinMaxXDiapazon(const F FminX, const F FmaxX);
    void SetMinMaxTScale(const F FminX, const F FmaxX);
    void GetMinMaxT(F& FminX, F& FmaxX);

    BOOL GetOrientFontX(){ return GetBitStyle(GRF_INV_NAME_X);};
    BOOL GetOrientFontY(){ return GetBitStyle(GRF_INV_NAME_Y);};
    BOOL SetOrientFontX(BOOL flg);
    BOOL SetOrientFontY(BOOL flg);
    int SetSizeFontX(int sz);
    int SetSizeFontY(int sz);

    BOOL GetViewNX() {return GetBitStyle(GRF_NAME_X);};
    BOOL SetViewNX(BOOL flg, BOOL flRedraw = true);
    CString SetTextNX(CString sname, BOOL flRedraw = true);
    CString GetTextNX() {return  sNameX;};
    LOGFONT SetFontNX(LOGFONT& lf, BOOL flRedraw = true);
    LOGFONT GetFontNX() {return logfontNX;};
    int GetSizeFontNX() {return -logfontNX.lfHeight;};
    int SetSizeFontNX(int sz, BOOL flRedraw = true);
    BOOL GetOrientFontNX() {return GetBitStyle(GRF_INV_NAME_X);};
    BOOL SetOrientFontNX(BOOL flg, BOOL flRedraw = true);
    COLORREF GetColorFontNX() {return colorFontX;};
    COLORREF SetColorFontNX(COLORREF clr, BOOL flRedraw = true);

    BOOL GetViewNY() {return GetBitStyle(GRF_NAME_Y);};
    BOOL SetViewNY(BOOL flg, BOOL flRedraw = true);
    CString SetTextNY(CString sname, BOOL flRedraw = true);
    CString GetTextNY() {return  sNameY;};
    LOGFONT SetFontNY(LOGFONT& lf, BOOL flRedraw = true);
    LOGFONT GetFontNY() {return logfontNY;};
    int GetSizeFontNY() {return -logfontNY.lfHeight;};
    int SetSizeFontNY(int sz, BOOL flRedraw = true);
    BOOL GetOrientFontNY() {return GetBitStyle(GRF_INV_NAME_Y);};
    BOOL SetOrientFontNY(BOOL flg, BOOL flRedraw = true);
    COLORREF GetColorFontNY() {return colorFontY;};
    COLORREF SetColorFontNY(COLORREF clr, BOOL flRedraw = true);

    BOOL GetViewSclX() {return GetBitStyle(SCLX_VIEW);};
    BOOL SetViewSclX(BOOL flg, BOOL flRedraw = true);
    BOOL GetViewDgtX() {return GetBitStyle(SCLX_VIEW_DGT);};
    BOOL SetViewDgtX(BOOL flg, BOOL flRedraw = true);
    BOOL GetViewSclNX() {return GetBitStyle(SCLX_NAME);};
    BOOL SetViewSclNX(BOOL flg, BOOL flRedraw = true);
    CString GetTextSclNX() {return sclHor.sclName;};
    CString SetTextSclNX(CString sname, BOOL flRedraw = true);
    LOGFONT SetFontDgtX(LOGFONT& lf, BOOL flRedraw = true);
    int GetSizeFontDgtX() const {return -sclHor.m_logfontM.lfHeight;};
    int SetSizeFontDgtX(int sz, BOOL flRedraw);
    BOOL GetOrientDgtX()  {return GetBitStyle(SCLX_ORIENT_FONT_M);};
    BOOL SetOrientDgtX(BOOL flg, BOOL flRedraw = true);
    COLORREF GetColorDgtX() {return sclHor.colorText;};
    COLORREF SetColorDgtX(COLORREF clr, BOOL flRedraw = true);
    LOGFONT SetFontSclNX(LOGFONT& lf, BOOL flRedraw = true);
    int GetSizeFontSclNX() {return -sclHor.m_logfontN.lfHeight;};
    int SetSizeFontSclNX(int sz, BOOL flRedraw);
    BOOL GetOrientSclNX()  {return GetBitStyle(SCLX_ORIENT_FONT_N);};
    BOOL SetOrientSclNX(BOOL flg, BOOL flRedraw = true);
    COLORREF GetColorSclNX() {return sclHor.colorName;};
    COLORREF SetColorSclNX(COLORREF clr, BOOL flRedraw = true);
    COLORREF SetColorFonSclX(COLORREF clr, BOOL flRedraw = true);

    BOOL GetViewSclY() {return GetBitStyle(SCLY_VIEW);};
    BOOL SetViewSclY(BOOL flg, BOOL flRedraw = true);
    BOOL GetViewDgtY() {return GetBitStyle(SCLY_VIEW_DGT);};
    BOOL SetViewDgtY(BOOL flg, BOOL flRedraw = true);
    BOOL GetViewSclNY() {return GetBitStyle(SCLY_NAME);};
    BOOL SetViewSclNY(BOOL flg, BOOL flRedraw = true);
    CString GetTextSclNY() {return sclVert.sclName;};
    CString SetTextSclNY(CString sname, BOOL flRedraw = true);
    LOGFONT SetFontDgtY(LOGFONT& lf, BOOL flRedraw = true);
    int GetSizeFontDgtY() {return -sclVert.m_logfontM.lfHeight;};
    int SetSizeFontDgtY(int sz, BOOL flRedraw);
    BOOL GetOrientDgtY()  {return GetBitStyle(SCLY_ORIENT_FONT_M);};
    BOOL SetOrientDgtY(BOOL flg, BOOL flRedraw = true);
    COLORREF GetColorDgtY() {return sclVert.colorText;};
    COLORREF SetColorDgtY(COLORREF clr, BOOL flRedraw = true);
    LOGFONT SetFontSclNY(LOGFONT& lf, BOOL flRedraw = true);
    int GetSizeFontSclNY() {return -sclVert.m_logfontN.lfHeight;};
    int SetSizeFontSclNY(int sz, BOOL flRedraw);
    BOOL GetOrientSclNY()  {return GetBitStyle(SCLY_ORIENT_FONT_N);};
    BOOL SetOrientSclNY(BOOL flg, BOOL flRedraw = true);
    COLORREF GetColorSclNY() {return sclVert.colorName;};
    COLORREF SetColorSclNY(COLORREF clr, BOOL flRedraw = true);
    COLORREF SetColorFonSclY(COLORREF clr, BOOL flRedraw = true);

    void SetColorDiagram(int numDiagram, COLORREF clr, BOOL flag_color = true);
    COLORREF GetColorDiagram(int numDiagram);
    int GetThicknessDiagram(int numDiagram);
    void SetThicknessDiagram(int numDiagram, int thick, BOOL flRedraw = true);

    CRect GetRectFldGrf();
    void UpdateViewRange();
    void SetViewInitDataX();

    void Test();
    void SetViewRepers(BOOL flg_view_reper, int num_buf = -1);
    void SetViewData(BOOL flg_view_data, int num_buf);
    void UpdateReper(BOOL flg = false);
    void SynchronizeGraph(double min_x, double max_x);
    void SetNumStrich(int nStrichX, int nStrichY);
    void SetMinMaxY(F Fmin, F Fmax, int num_grf = 0);
    void SetNameGrf(int num_grf, CString sname);
	VFRingBuf<F>* GetVXBuffer();
    UINT GetStartMarkerX(){return FieldGraph.DATA.GetPosUMarker(-1, 1);};


    GRF_MINMAX SetParMinMax(int num_grf, GRF_MINMAX& par){return FieldGraph.SetParMinMax(num_grf, par);};
    GRF_MINMAX GetParMinMax(int num_grf){return FieldGraph.GetParMinMax(num_grf);};

    void RemoveBufer(int num_buf) {FieldGraph.DATA.RemoveBuffer(num_buf);}
    void GetMinMaxDiapazonDef(int num_grf, F& Fmin, F& Fmax) 
    {FieldGraph.GetMinMaxDiapazonDef(num_grf, Fmin, Fmax);}

    FRingBuf<F>* GetBuf(int num_buf) { return FieldGraph.DATA.GetCurRBuf(num_buf); }
    BOOL HSetScroolGraphExternal();
    BOOL VSetScroolGraphExternal();
    void SetScroolGraphExternal(int num_scroll);
	BOOL GetRectFieldGrf(CRect& rec);
	int ClearBeginDataY(int num_point_for_delete);
	int ClearBeginDataX(int num_point_for_delete);
	void ResetFlagsClearedY();
	int ClearBeginDataXY(int num_point_for_delete);
	double GetLifeTime(int num_grf, double start_time, double end_time);
	double GetDrv(int num_buf, double dX = 0);

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGraph)
public:
    //}}AFX_VIRTUAL

public:
    CGraph& operator=(CGraph& fld);
    virtual ~CGraph();

protected:
    //{{AFX_MSG(CGraph)
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg LRESULT OnChangeRange(WPARAM /*wParam*/, LPARAM /*lParam*/);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int x, int y);
    afx_msg LRESULT OnSetScaleTime(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnOutDigData(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEditPointGraph(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDestroy();
    afx_msg LRESULT OnRButtonDown1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowPanel(WPARAM wParam, LPARAM lParam);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    virtual void Serialize(CArchive& ar);
    long time_graph;
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPH_H__09F2D8E3_D1BD_11D2_8632_0040C7952451__INCLUDED_)


