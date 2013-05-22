#pragma once

#include "Param.h"
#include "src/SERV.h"
#include "PanelArray.h"
#include "MainView.h"

#define CM_NUM_LISTS 5				
#define CM_NUM_FIELDS_GRAPHS 3
#define CM_NUM_FIELDS_PANELS 0

class CCementView : public CMainView
{
protected: 
    CCementView();
    DECLARE_DYNCREATE(CCementView)

public:
    enum{ IDD = IDD_CEMENTVIEW };

	void SetActivityDB();
	void RemoveActivityDB();
	virtual void UpdateInputDataDB();

	void SetScaleDB(int num_list);
	void RequestDataFromDB();
	bool m_need_update_from_db;

    void SetVisibilityPanelEtap();
	void SetValidIndexBeginData(int num_list);
	VFRingBuf<double>* GetVXBuffer(ParamOfView* pList, int n_grf_panel);

    virtual void ChangeaActiveListA(WPARAM wParam, LPARAM lParam);
    virtual void RButtonDown1A(WPARAM wParam, LPARAM  lParam);
    virtual double GetStartX(int num_list);
    virtual void SetStartX(double start_value, int num_list, BOOL flg);
    virtual void RecalcStagesList();

	virtual void DeleteList(int num_list);
	virtual void SwapLists(int num_list1, int num_list2);
	virtual void AddList();
	void ReloadDataView();

    void HideAllGraphsInStages();
    void HideAllGraphsInStage(int n_list);

	virtual void OnScrollGraphs();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnInitialUpdate(); // called first time after construct

public:
    virtual ~CCementView();

protected:
    afx_msg LRESULT OnStagesLBD(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSelectedStageLBD(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStagesRBD(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectedStageRBD(WPARAM wParam, LPARAM lParam);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()
public:
    virtual void Serialize(CArchive& ar);
};

