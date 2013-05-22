#pragma once

#include "CementView.h"

// CCementRT form view

class CCementRT : public CCementView
{
	DECLARE_DYNCREATE(CCementRT)

protected:
	CCementRT();           // protected constructor used by dynamic creation
	virtual ~CCementRT();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    afx_msg LRESULT OnReadDataMessage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSynchronizeGraph(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLButtonDown1(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


