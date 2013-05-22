#pragma once

#include "DialogBarMain.h"
#include "ExtListCtrl.h"
#include "STAGES.h"

// CDlgPanelCement

class CDlgPanelCement : public CDialogBarMain
{
public:
	CDlgPanelCement();
	virtual ~CDlgPanelCement();
    //{{AFX_DATA(CDlgPanelCement)
    enum { IDD = IDD_DIALOG_PANEL_CEMENT };
    STAGES_List m_list_stages;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CDlgPanelCement)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
    virtual void ResizeOtherBars();

public:
    void RecalcListWidth();
   
	DECLARE_MESSAGE_MAP()
public:
    afx_msg LRESULT OnMyInitDialog(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();

    virtual void Serialize(CArchive& ar);
};


