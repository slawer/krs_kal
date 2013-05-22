#if !defined(AFX_PARAM_H__03E05A81_A5A8_11D3_9DB9_D7AC05780F19__INCLUDED_)
#define AFX_PARAM_H__03E05A81_A5A8_11D3_9DB9_D7AC05780F19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Param.h : header file
//

#include "Graph.h"
#include "EditClr.h"
#include "HeaderGrf.h"
#include "FORMULA.h"

#define MAX_SIIZE_BUF 900000

extern DWORD Param_clr_min, Param_clr_norm, Param_clr_max, Param_clr_avaria;
extern int Param_blink_counter;

enum typeView
{
	t_real_time	= 0,
	t_archive	= 1,
	t_cement_rt	= 2,
	t_cement_db	= 3
};

enum typePanel
{
	tp_grf	= 0,
	tp_dgt	= 1
};

class CPanelWnd;
class CParam;
class ParView;

#include <list>
using namespace std;

typedef CTypedPtrArray<CObArray, ParView*> CParamArray;
typedef CTypedPtrList<CObList, CParam*> CParamList;
typedef CMap<int, int, CParam*, CParam*&> CParamMap;

struct ParamInPanel
{
	int m_param_num;
	double m_min_val, m_max_val;
	COLORREF m_color, text_color;
	int m_line_width;
	int text_size;
	int font_weight;
	
	CString GetTextForSave();
	bool FillFromText(CString text);
};

struct PanelDescription
{
	CString GetTextForSave();
	bool FillFromText(CString text);
	
	CString snamePanel;
	list<ParamInPanel> m_params;
};

#define PARAM_TYPE_CALCULATED 0
#define PARAM_TYPE_CHANNEL 1
#define PARAM_TYPE_FORMULA 2

#define PARAM_DB_STATUS_NORMAL 0
#define PARAM_DB_STATUS_BEFORE_TO 1
#define PARAM_DB_STATUS_TO 2

#define PARAM_TO_VAL 2e20
#define PARAM_TO_VAL_CHECK 1e20

class CParam : public CObject
{
	RBuf<float> *pData;
	RBuf<F> *ptmData;
	int sizeBuf, indexWirite, indexRead, sizeAverage;
	double lastTimeAverage;
	BOOL flagLastAverage;
    bool m_timed_out_db;

public:
	bool CanBeTimedOut();
	byte m_calc_status, m_prev_calc_status;
    bool m_always_not_timed_out;

	float GetCurrentValue(byte &status);
	FORMULA_Formula* m_formula;
	CString m_text_formula_for_registry;

	int m_nTypePar;	

	int m_index_of_last_measurement; 
	
	byte m_shown_digits; 

	void StartTimeOut();
	void FinishTimeOut(int pos);
	bool IsTimedOut();
	bool IsTimedOutDB() { return m_timed_out_db; }
    void StartTimeOutBD();
    void FinishTimeOutBD(int pos);


	int m_line_width;
	COLORREF m_color;

	int m_num_par; 
	CParamMap*		m_MapParam;


	CParam();
	CParam(CString ValName, float Val, float ValMin, float ValMax, 
				float ValBlock, float ValAvaria);
	CParam(CParam& prm);
	CParam(CParam* prm);

// Attributes
public:
	FRingBuf<F>* pBufParam; 
	CItemClr	iItemParam;
	CItemClr2	iItemParamA;

	float m_last_value_send_to_db; 
	byte m_send_to_db_status; 
	double timeAvrg,
		m_db_time_interval, 
		m_last_time_send_to_db, 
		m_db_data_interval; 
	float curGlubZab;
	float lastAvrg;
	float fCurAvg, fCurAvgPrevious, m_last_good_val;
	double fCurSave;

	short nPar;

	int	  iCmd2;	
	float fCur;
	float fCurPrevious;
	float fMin, fMax;
	float fDiapazon;
	float fDiapazon1;
	float fBlock;
	float fAvaria;
	float fPodklinka;
	float fObiom;
	float fNapolnenie;
	float fRsahodIzm;
	float fRsahodRasch;
	float fPodveskaDatchika;
	float fPloschad;
	float fIzmObioma;
	float fVObioma;
	float fNizIzmer;
	float fNizIstina;
	float fVerhIzmer;
	float fVerhIstina;
	float previousAvrg;

	float fPodklinkaKof;
	float fGraphMIN, fGraphMAX;


	float fPeriod;
	float fImplLebedki, saveImplLebedki;

	float fVerhZnach, fNizZmach;  

	BOOL  flGraph, flControl1, flControl2, 	flPodacha, flDB, fl_db;

	int	  nScale;	
	BOOL  flagDiapazon;
	int	  nFlag1, nFlag2;	

	float fKalibr;
	float fKalibrA;
	float fKalibrK;
    int	  nKodControl; 	
	int numAlert;

	int  nMainList;
	int  nViewList;
	BOOL  bMain;
	BOOL  bView;
	CString sRazmernPar;
	CString sName;
	CString sGraphName;
	CString sNickName;
	CString sDiscript;
	CString sFile1;
	CString sFile2;
	CString sFile3;
	CString sFile4;

	int prevNumTimeInterval;
	double prevTimeInterval;
	float prevValue;
	double prevTime;

	double lastTime;

	BOOL bParamActive;
	int m_channel_num, m_channel_conv_num,
		m_attr_channel_num, m_attr_channel_conv_num;

	CString GetNameForShow();
// Operations
public:

	void Init(int base_pos = 0);

	void DestroyParam();
	BOOL IsActive() {return flGraph;};

	void SetTimeAvrg(float tAvrg){timeAvrg = tAvrg;};

	int AddTail(double dTime);
	float GetAvrg();
	void SaveAvrg();
	float GetAvrgInterval(int dTime);
	float GetDiff();
	float GetDrv(float dTime = 0.0f);
	float GetDiffValue(float dTime);
	float GetLastAvrg(){return lastAvrg;};
	float GetPrevAvrg(){return previousAvrg;};
	float GetAvrgP();
	float GetIndexValue(double& dtm, int nShift);
	int GetIndexDTime(double dTime);
	float GetAvrgShift(int nShift);
	float GetAvrgPrevious(double dTime);
	double GetLastTime();
	void SaveDOL();
	float GetDfDOL();
	float GetDTime();
	float GetSPO();
	float GetDfPar();
	CString TestCase(int nCase, float fValue);
	void SetAlertLine();

	float GetChangeSpeed(double, float);
	void SetScaleTime(LPVOID pBufX, double m_ScaleXStart, double m_ScaleXEnd, BOOL m_flagTimeGlub);
	int AddData(float ff);
	int GetSizeData();
	void ResetIndex();

	//{{AFX_VIRTUAL(CParam)
	//}}AFX_VIRTUAL

public:
	virtual ~CParam();
	CParam& operator=(CParam& prm);
};

/////////////////////////////////////////////////////////////////////////////

class CStrPrn : public CWnd
{
protected: // create from serialization only
	DECLARE_SERIAL(CStrPrn);
	CStrPrn();

// Construction
public:
	CStrPrn(CString sName, CString sText, int nStr, float kIntrv, int intrvNameText, int iOtstup, int iBold,
			BOOL bItalic, BOOL bUnderline, int szFnt);

// Attributes
public:
	CEdit	m_cEdit;

	CString strName, strText;
	int numStr;	
	float kInterval; 
	int intervalNameText; 
	int iOtstup; 
	int iBold;  
	BOOL bItalic;
	BOOL bUnderline;  
	int sizeFnt;


	LOGFONT m_lfntT, m_lfntN;
	CFont	fFontText, fFontName;

// Operations
public:
	void setParam2T(int iBld, BOOL bItal, BOOL bUnderl, int szFnt);
	void SetStrText(CString str);
	void GetStrText(CString& str, int& otst);
	void CreatePrnStr(CWnd *pWnd);
	void SetPosPrnStr(CWnd *pWnd, CDC* pDC);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStrPrn)
	//}}AFX_VIRTUAL

// Implementation
public:
	CStrPrn& operator=(CStrPrn& strP);
	virtual ~CStrPrn();


// Generated message map functions
protected:
	//{{AFX_MSG(CStrPrn)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

typedef CTypedPtrList<CObList, CStrPrn*> CCStrPrnList;

/////////////////////////////////////////////////////////////////////////////

class CBurDevice : public CObject
{
// Construction
public:
	CBurDevice();
	CBurDevice(CString Name, float Val, float ValMin, float ValMax, 
				float ValBlock, float ValAvaria);

// Attributes
public:
	float fVal;
	float fValMin, fValMax;
	float fValDiapazon;
	float fValBlock;
	float fValAvaria;
	UINT  nValMainList;
	UINT  nValViewList;
	BOOL  bValMain;
	BOOL  bValView;
	CString sValName;
	CString sValDiscript;
	CString sValFile1;
	CString sValFile2;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBurDevice)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBurDevice();

	// Generated message map functions
protected:

};

class ParView : public CObject
{
public:
	CParam* pPrm;
	CDgtHeadParam* m_dgtLabel;
	int m_num_par_local;
	int num_grf;
	float f_minGraph, f_maxGraph;
	int line_width;
	COLORREF colorGraph;
	COLORREF color_font_dgt;
	LOGFONT  font_dgt;
    BOOL m_flag_orient;

	ParView()
	{
		pPrm = NULL;
		m_dgtLabel = NULL;
		m_num_par_local = -1;
		num_grf = -1;
		f_minGraph = 0;
		f_maxGraph = 50;
		line_width = 2;
		colorGraph = RGB(0,0,0);
		color_font_dgt = RGB(0,0,255);

		LOGFONT lf = {
			-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
			OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
			VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
		};

		font_dgt = lf;
        m_flag_orient = true;
	}

	ParView(CParam* par, CDgtHeadParam* label, int num_local, int n_grf)
	{
		pPrm = par;
		if(m_dgtLabel != NULL) delete m_dgtLabel;
		m_dgtLabel = label;
		m_num_par_local = num_local;
		num_grf = n_grf;

		f_minGraph = 0;
		f_maxGraph = 50;
		line_width = 2;
		colorGraph = RGB(0,0,0);
		color_font_dgt = RGB(0,0,255);

		LOGFONT lf = {
			-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
			OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
			VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
		};

		font_dgt = lf;
        m_flag_orient = true;
	}

	~ParView()
	{
		if(pPrm != NULL) pPrm = NULL;
		if(m_dgtLabel != NULL) {delete m_dgtLabel; m_dgtLabel = NULL;}
		m_num_par_local = -1;
		num_grf = -1;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CPanelWnd window
#include "resource.h"       // main symbols
#include "ChangeParam.h"

#define CR(rgb) GetRValue(rgb)
#define CG(rgb) GetGValue(rgb)
#define CB(rgb) GetBValue(rgb)

typedef struct {
	int nSize;
	double scl_time[MAX_SIIZE_BUF];
	double scl_glub[MAX_SIIZE_BUF];
	float pData[MAX_SIIZE_BUF];
} ParamDB_P;

class CPanelWnd : public CStatic
{
public:
	CGraph m_parGraph;
	CParamArray m_ParamArray;

	CWnd* m_pParentForGraph;
	CWnd* m_pParentForPanel;
	CParamMap*	m_MapParam;

	LPVOID m_pBufX;
	BOOL m_flag_visible_field;
	int num_panel;
	BOOL type_panel, m_flagTimeGlub;
	double m_ScaleXStart, m_deltaX;

	CFont fFontName;
	LOGFONT  m_logfontN;

	int m_splitH;

	CString sNamePanel;

	CRect rect_header, rect_add;
	BOOL m_flag_size;

	typeView t_view;
	typePanel tp_panel;
    BOOL m_flag_orient;

	void DeleteParam(int num_in_list);
	void DeleteParamByNum(int num_param);

	ULONG64 GetStyleGrf();
	ULONG64 GetBitStyle(ULONG64 g_style);

	template <class type>
	void SubstituteBitStyleGrf(ULONG64 dwStyle, type value, BOOL flRedraw)
	{
		m_parGraph.SubstituteBitStyle(dwStyle, value, flRedraw);
	}

public:

	CChangeParam	m_dlgChangeParam;


// Construction
public:
	CPanelWnd();

// Attributes
public:

// Operations
public:
	
	BOOL GetInfoPanel(PanelDescription *plstParam);
	int GetMaxSize();
	void ShowPanel(BOOL fl_visible);
	void SetVisibilityFlag(BOOL fl_visible);
	BOOL GetVisible();
	BOOL GetRectFieldGrf(CRect& rec);
    UINT GetStartMarkerX();
	void RedrawPanel(typePanel tp_p);
	void UpdateVisible(BOOL fl_visible);
    void UpdateRectRange(int num_grf_panel, BOOL flRedraw);
    void UpdateScrollRange(int num_grf_panel, int num_scroll);
	void ChangeGraphColor();
	void SetScaleTime(LPVOID pBufX, double ScaleXStart, double ScaleXEnd, BOOL flagTimeGlub, CString textGrf);
	void SetScaleXDiapazon(LPVOID pBufX, double ScaleXStart, double ScaleXEnd, BOOL flagTimeGlub, CString textGrf);
	void SetTextGrfPanel(CString textGrf, BOOL flRedraw = false);
	void UpdateReper(BOOL flg = false);
    void SynchronizeGraph(double min_x, double max_x);
	VFRingBuf<double>* GetVXBuffer();
	void ChangeScale(LPVOID pBufX, GRF_MINMAX& par, BOOL flagTimeGlub);
    void GetMinMaxX(F& min, F& max);
	int AddParam(ParamInPanel* pd, BOOL flg_redraw);
	void ClearParams();
	void RequestData(COleDateTime begTime, COleDateTime endTime);
	void InitPanelForDataDB(COleDateTime begTime, COleDateTime endTime, CString textGrf);
	int AddActiveParam(CParamMap *pActive);
	void RecalcFieldSize();
	CSize GetSizeDgtPanel();
	void PrintGrf(CDC *pDC, CWnd *pWnd, float printScaleX, float printScaleY, 
		CSize *printShift);
    void LockDataForPrint(int lock_value);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanelWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPanelWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPanelWnd)
	afx_msg LRESULT OnDataChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDataDBChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeParamProperty(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddParam(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeStyleGrf(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnDeleteParam(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnRButtonDown1(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSetStyleGrf(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResetStyleGrf(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAM_H__03E05A81_A5A8_11D3_9DB9_D7AC05780F19__INCLUDED_)


