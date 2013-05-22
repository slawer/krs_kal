// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

//#define _DBG

#if !defined(AFX_MAINFRM_H__D265F5FB_C837_4634_84C1_724934350C65__INCLUDED_)
#define AFX_MAINFRM_H__D265F5FB_C837_4634_84C1_724934350C65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DlgBar1.h"
#include "DlgBarH.h"
#include "DlgLineParam.h"
#include "DialogBarDB.h"

#define WM_PARAM_LIST_READY (WM_USER + 2)

#include "ChangeParam.h"

typedef struct {
	int numRegim;
	CString sNameRegim;
	BOOL flShurf;
	BOOL flWes; 
	BOOL flDavl; 
	BOOL flDol; 
	int  nTalblok;
	int flGlub;
} REGIM;


#define MAX_REGIM 100
class RegimBur
{
public:
	RegimBur()
	{
		numRegimCur = 0;
		max_regime = 0;

		regimBur[max_regime].numRegim = 0;
		regimBur[max_regime].sNameRegim = "Õ≈“ –≈∆»Ã¿";

		max_regime++;
	};

	~RegimBur(){};

	int numRegimCur;
	int max_regime;
	REGIM regimBur[MAX_REGIM];

	void AddRegim(CString nameReg0, BOOL flShurf0, BOOL flWes0, 
		BOOL flDavl0, BOOL flDol0, int nTalblok0, int flGlub0)
	{
		regimBur[max_regime].numRegim = max_regime + 1;
		regimBur[max_regime].sNameRegim = nameReg0;
		regimBur[max_regime].flShurf = flShurf0;
		regimBur[max_regime].flWes = flWes0; 
		regimBur[max_regime].flDavl = flDavl0; 
		regimBur[max_regime].flDol = flDol0; 
		regimBur[max_regime].nTalblok = nTalblok0;
		regimBur[max_regime].flGlub = flGlub0;

		max_regime++;
	};

	CString TestCurRegime(BOOL flShurf0, BOOL flWes0, 
		BOOL flDavl0, BOOL flDol0, int nTalblok0, int flGlub0)
	{
		for(int i = 1; i < max_regime; i++)
		{
			BOOL flShurf	= regimBur[i].flShurf;
			BOOL flWes		= regimBur[i].flWes;
			BOOL flDavl		= regimBur[i].flDavl;
			BOOL flDol		= regimBur[i].flDol;
			int  nTalblok	= regimBur[i].nTalblok;
			int flGlub		= regimBur[i].flGlub;
			
			if(flGlub < 0) flGlub = flGlub0;
			if((flShurf == flShurf0) && (flWes == flWes0) && (flDavl == flDavl0) && 
				(flDol == flDol0) && (nTalblok == nTalblok0) && (flGlub == flGlub0)) 
			{
				numRegimCur = i;
				break;
			}
			else numRegimCur = 0;
		}

		return regimBur[numRegimCur].sNameRegim;
	};

	int GetCurRegime()
	{
		return numRegimCur;
	};


};


//#include "RingBuff.h"

class CKRSView;

class CMainFrame : public CMDIFrameWnd
{

protected: // create from serialization only
	DECLARE_DYNCREATE(CMainFrame)
//	DECLARE_SERIAL(CMainFrame);

// Attributes
public:
    CMainFrame();

	RegimBur		m_regimBurenia;
	int numPAPAM;

	BOOL m_flaChangeList, m_flagChangeTimeGlub, m_flagTimeGlub;
	BOOL m_flaChangeListDB;

	int		m_fRaportHours;
	int		m_fRaportMinut;
	int		m_fRaportSecond;

	COleDateTime m_tTimeRaport;

	int m_iStartTime;
	int m_iDiapazonTime;

	COleDateTime m_ScaleTimeStart, m_curTime;
	double m_lasTime;

	int m_iperiodWDB, m_eDBWritePeriod;

	BOOL m_bFlagTimeAlert;
	double m_prev_time;
	
	double m_start_time, m_start_glub, m_start_timeDB, m_start_glubDB;

	FRingBuf<double> *ptmData, *pcurGlub;
	FRingBuf<double> *ptmDataDB, *pcurGlubDB;
	LPVOID pBufX, pBufXDB;

//----------------------------------------------------------------------------
	int				m_active_list;
	CParamList		m_lMainLst;
	int				m_type_view;

    HCURSOR m_CurNorm, m_CurHSpl, m_CurVSpl, m_CurCursor;
    BOOL m_bFlagDown;
    BOOL m_CusorSplit;
    CRect	m_SplitBar;

// Operations
public:
	void BreakDBConnection(bool send_msg);

	int CreateMapParam();
	void SetInitialData();
	void DeleteList(CParamList* lsList);
	int InitParamGraph();

	double CreateTestData();

	void ParamCalculation();

	BOOL TestTimedOutPar(CParam *pPrm, BOOL flg_timedout, int pos);
	float CalcMainParamA(int mKey, double curTime);

	//void TryToCompareParamLists();
//	void RefreshMapParam();

	bool UpdateParamListInDB();
    void ChangeTypeView(WPARAM wParam, LPARAM lParam);
	void SetXBufToParam(CParam *prm);

//#ifndef _DBG
	BOOL GetPickAlertion(CParam* pPrm);
	BOOL GetPickDFAlertion(CParam* pPrm);
	CString GetFileAlertion(CParam* pPrm);
	void TestPickAlertParam(CParam* pPrm);
	void TestPickAlertAllParam();
	void TestPickDFAlertParam();
	void TestAlertParam();
    void SetViewPanelBur(BOOL flg);
    void SetViewDgtRT();
    void SetViewDgtBD();
	void SetHighPanel();
	void SetGraphDigit();
	BOOL GetFlgAccess();
	void SetParValue(CParam *pPar, float value);
	void ChangeflgRabotaBezShurfa();
	void SetGlX(); 
	void SetGlI();



//#endif _DBG

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void Serialize(CArchive& ar);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

friend UINT TimerThreadProcedure(LPVOID param);
// Generated message map functions

	afx_msg void OnRadioScaleTim();
	afx_msg void OnRadioScaleGlub();
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnReadDataMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnListReadyCONV(WPARAM, LPARAM);
	afx_msg LRESULT OnErrorCONV(WPARAM code, LPARAM string);
	afx_msg LRESULT OnInitedCONV(WPARAM, LPARAM);
	afx_msg LRESULT OnDataSERV(WPARAM, LPARAM);
	afx_msg LRESULT OnErrorSERV(WPARAM code, LPARAM string);
	afx_msg LRESULT OnInitedSERV(WPARAM, LPARAM);
    afx_msg LRESULT OnMaximizeFrame(WPARAM, LPARAM);
	afx_msg void OnButtonGetParam();
	afx_msg LRESULT OnChangeParamProperty(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddParam(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeTypeView(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStartProperty();
	afx_msg void OnEditProject();
	afx_msg void OnOpenWork();
	afx_msg void OnNewWork();
	afx_msg void OnImportWork();
	afx_msg void OnUpdateStartProperty(CCmdUI* pCmdUI);
	afx_msg void OnMenuPropDb();
    afx_msg void OnUpdateMenuPropDb(CCmdUI *pCmdUI);
	afx_msg void OnMenuTcpit();
	afx_msg void OnUpdateMenuTcpit(CCmdUI* pCmdUI);
	afx_msg void OnKalibrovka();
    afx_msg void OnUpdateMenuKalibrovka(CCmdUI* pCmdUI);
	afx_msg void OnKalibrovkaM();
    afx_msg void OnUpdateMenuKalibrovkaM(CCmdUI* pCmdUI);
	afx_msg void OnParamatry();
    afx_msg void OnUpdateMenuParamatry(CCmdUI* pCmdUI);
	afx_msg void OnUploadMem();
	afx_msg void OnDownloadMem();
    afx_msg void OnUpdateMenuDownloadMem(CCmdUI* pCmdUI);

	afx_msg void OnStagesLib();
	afx_msg void OnStagesLoad();
	afx_msg void OnStagesSave();
	afx_msg void OnStagesChooseFont();
	afx_msg void OnStagesChooseFontSel();

	afx_msg void OnStagesGetDB();
	
	afx_msg void OnWk();
	afx_msg void OnMarkerTypes();
	afx_msg void OnCheckNgr1();
	afx_msg void OnCheckNgr2();
	afx_msg void OnCheckNgr3();
	afx_msg void OnConfigDevices();
    afx_msg void OnUpdateMenuConfigDevices(CCmdUI* pCmdUI);
	afx_msg void OnDebugInfo();
	afx_msg void OnSettingsBO();
    afx_msg void OnUpdateMenuSettingsBO(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnStartMessage(WPARAM, LPARAM);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnListReadyDB(WPARAM, LPARAM);
	afx_msg void OnUpdateMarkerTypes(CCmdUI *pCmdUI);
	afx_msg void OnMenuOpisanieMestorogdenija();
	afx_msg void OnClose();
	afx_msg void OnUpdateDebugInfo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditProject(CCmdUI *pCmdUI);
	afx_msg void OnUpdateOpenWork(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNewWork(CCmdUI *pCmdUI);
	afx_msg void OnUpdateImportWork(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMenuOpisanieMestorogdenija(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUploadMem(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNastroikaModules(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__D265F5FB_C837_4634_84C1_724934350C65__INCLUDED_)
