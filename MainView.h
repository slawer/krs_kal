// MainView.h : interface of the CMainView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Param.h"
#include "DB.h"
#include "src/SERV.h"
#include "PanelArray.h"

class CMainView : public CFormView
{
protected: // create from serialization only
    DECLARE_DYNCREATE(CMainView)


public:
    CMainView();
	CMainView(UINT nIDTemplate, int n_lists, int n_grf_panel, int n_dgt_panel);
	BOOL m_flag_init;

public:
	//{{AFX_DATA(CMainView)
//	enum{ IDD = IDD_KRS_FORM };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Attributes
public:
	//----------------------------------------------------------------------------
	CParamList		m_lMainLst;
	CDataView*		m_parOfView;
	typeView		type_view;
	//----------------------------------------------------------------------------
	DWORD m_startTick;
	COleDateTime m_startTime;
	CRect m_scaleRect;
	float m_printScaleX, m_printScaleY;
	CSize m_printShift;

	int m_flagPrint;
	COleDateTime tm_print;
	GRF_MINMAX par_print;
	double minf_print, maxf_print;
	BOOL flg_stretch_print;

	FRingBuf<double> *m_ptmData, *m_pcurGlub;
	LPVOID pBufX;
	int num_lists, num_grf_panel, num_dgt_panel;

	int m_numContextMenu;
	BOOL flag_scroll;
	BOOL m_flag_orient;
	BOOL m_flag_activity_db;
	PanelDescription *m_panel_descrG, *m_panel_descrD;
	GRF_MINMAX par;
	bool m_setup_grafs_diapazon;

	// Operations
public:
    void InitDataView(int n_lists, int n_grf_panel, int n_dgt_panel);
	CDataView* GetActiveData();
	ParamOfView* GetActiveList();
	LPVOID Get_pBufX();
	//void ChangeActiveList(int act_lst);
	void SetTextGrfPanel();
	void SelectRegime();
	void SetScaleT();
	void SetScaleG();
	double GetLenX(int num_list);
	void RecalcLenX(CDC *pDC, int num_lists, float scale);
	BOOL RecalcLenX(int cy, bool flg);
    void RecalcScaleFactor(int num_list);
	void ChangePos(BOOL flg);
	void ReconnectGraph(CWnd* pParentWnd);
	void InitActiveParamGraphV();
	double GetCurTime();
	void OnRemoveGraph(); 
	void DestroyParamList(CParamList* lsList);

	CString GetStrScale(int nn);
	void SetParXScale(int num_list, BOOL flg = false);
	BOOL GetMinMaxScale(GRF_MINMAX &par, int num_list);

	bool GetParamFromRestore(int num, PanelDescription *plstParam, ParamInPanel& param_descr);

	float GetScaleFactorT(CDC* pDC, float scale);
	float GetScaleFactorG(CDC* pDC, float scale);
	int GetScaleT(CDC* pDC, ParamOfView *pList, int time_sekund);
	int GetScaleG(CDC* pDC, ParamOfView *pList, int glub_santimetr);
	void SetScaleFactor(int num_lists, float fscale, BOOL flRedraw = true);
    void SetScaleFactorActiveList(float fscale, BOOL flRedraw = true);
	void UpdateActiveView(BOOL flRedraw = true);
	void SetParamPrinting(CDC* pDC);
	BOOL GetOrient();
    void LockGraphData(int n_lock);
	void CheckGrf();
	void CheckRegim();
	void RedrawActiveView();
	void UpdateActiveList();
	void SaveAllLists();
	void SetOrientView();
	void GetInfoAllLists();
	void RecalcActiveLilst();
    void PrintHeaderPage(CDC* pDC, int n_list = 0);
    void PrepareListToPrint(CDC* pDC, int n_list);
    void RestoreParamList(CDC* pDC, int n_list);

	void DeleteView();
	void InitList(int num_list, CWnd* pParentForDgtPanel, LPVOID pBufX, CParamMap* pmapPrm); 
    void InitViews(CWnd* pParentForDgtPanel, CParamMap* m_mapPrm);
	void SetInfoAllLists();
	void RestoreList(int num_list);
    void RestoreAllLists();
	void ClearAllParamsBuffers(CParamMap *mapParam, FRingBuf<double> *ptmData, FRingBuf<double> *pcurGlub);
	void SetColorInterface();

	int AddTimeGlub(double dTime, double dGlub);
	void UpdateListInputDataDB(int list_num);
	virtual void UpdateInputDataDB() { UpdateListInputDataDB(-1); }

	void SetModulesExtern(bool anyway = false);
	void RenameMenuItem(UINT ID_menu_item, CString new_name);

// Overrides
public:
	virtual void ChangeaActiveListA(WPARAM wParam, LPARAM lParam){};
	virtual void RButtonDown1A(WPARAM wParam, LPARAM  lParam){};
	virtual double GetStartX(int num_list){return 0;};
	virtual void SetStartX(double start_value, int num_list, BOOL flg){};
    virtual void RecalcStagesList(){}
	//virtual BOOL AddSubMenuItem(HMENU hMenu, int num_menu_item, UINT ID_submenu_item, CString new_name, HICON hIcon = 0){return false;};
	virtual void ReloadDataView() {}
	virtual void OnScrollGraphs() {}

// ClassWizard generated virtual function overrides
//{{AFX_VIRTUAL(CMainView)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnDraw(CDC* pDC);
//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CMainView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMainView)
	afx_msg LRESULT OnChangeTypeScale(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeaActiveList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeaNumGrf(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeParamProperty_1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeParamProperty(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddParam(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddParam2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecalcSize(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecalcSizeDgtPanels(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnChangeNamePanel(WPARAM, LPARAM);
	afx_msg LRESULT OnSaveAllList(WPARAM, LPARAM);
	afx_msg LRESULT OnUpdateView(WPARAM, LPARAM);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnContMenuMarker();
	afx_msg LRESULT OnRButtonDown1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteExtParam(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteParamFromList(WPARAM wParam, LPARAM lParam);
	afx_msg void OnContMenuScale5();
	afx_msg void OnContMenuScale10();
	afx_msg void OnContMenuScale120();
	afx_msg void OnContMenuScale1440();
	afx_msg void OnContMenuScale20();
	afx_msg void OnContMenuScale240();
	afx_msg void OnContMenuScale2880();
	afx_msg void OnContMenuScale360();
	afx_msg void OnContMenuScale60();
	afx_msg void OnContMenuScale720();
	afx_msg void OnMenuScaleTest();
	afx_msg void OnContMenuScaleG10();
	afx_msg void OnContMenuScaleG100();
	afx_msg void OnContMenuScaleG20();
	afx_msg void OnContMenuScaleG50();
	afx_msg void OnContMenuScaleG200();
	afx_msg void OnContMenuScaleG500();
	afx_msg void OnContMenuScaleG1000();
	afx_msg void OnContMenuScaleG2000();
	afx_msg void OnContMenuScaleG5000();
	afx_msg void OnMenuScaleG10000();
	afx_msg LRESULT OnSetStyleAllGrf(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResetStyleAllGrf(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSetStyleActiveList(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnResetStyleActiveList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPanelGrf(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeRangeGrf(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeScrollGrf(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecalcActiveLilst(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnModulesChanges(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual void Serialize(CArchive& ar);
public:
//public:
//	afx_msg void OnSetFocus(CWnd* pOldWnd);
//public:
//	afx_msg void OnKillFocus(CWnd* pNewWnd);
public:
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
};

