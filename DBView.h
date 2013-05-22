#pragma once

#include "Param.h"
#include "DB.h"
#include "src/SERV.h"
#include "PanelArray.h"
#include "MainView.h"

class CDBView : public CMainView
{
protected: // create from serialization only
    CDBView();
    DECLARE_DYNCREATE(CDBView)
    BOOL m_flag_init;
	BOOL m_bflag_begin_read_DB;
	
public:
	//{{AFX_DATA(CDBView)
	enum{ IDD = IDD_DBVIEW };
	//}}AFX_DATA

// Attributes
public:
//----------------------------------------------------------------------------
// Operations
public:
    //void ResetIndex();
    void ResetIndexParamDB();
    void SetScaleDB();
	VFRingBuf<double>* GetVXBuffer(ParamOfView* pList, int n_grf_panel);
	void ResetViewIndexList(int num_list);
	void ResetViewIndexForAllLists();
    bool GetDataFromDB(int type_request_db);
    //void UpdateInputDataDB();
    int GetActiveParamDB(CParamMap *pActive);
    void AnalyzePacketDB(DB_Packet *packet);
    void GetBegEndDBFromRequest();
	void RequestDBFinished(int flag_time_glub);
 
// Overrides
public:
	virtual void ChangeaActiveListA(WPARAM wParam, LPARAM lParam);
	virtual void RButtonDown1A(WPARAM wParam, LPARAM  lParam);
	virtual double GetStartX(int num_list);
	virtual void SetStartX(double start_value, int num_list, BOOL flg);
	void ReloadDataView();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnInitialUpdate(); // called first time after construct
 
// Implementation
public:
	void SetActivityDB();
	void RemoveActivityDB();

    virtual ~CDBView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// Generated message map functions
protected:
	afx_msg LRESULT OnDataDB(WPARAM, LPARAM);
    afx_msg LRESULT OnErrorDB(WPARAM code, LPARAM string);
    afx_msg LRESULT OnInitedDB(WPARAM, LPARAM);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg LRESULT OnSetListDB(WPARAM, LPARAM);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

