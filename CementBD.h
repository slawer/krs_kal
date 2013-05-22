#pragma once

#include "CementView.h"

// CCementBD form view

class CCementBD : public CCementView
{
	DECLARE_DYNCREATE(CCementBD)

public:
	CCementBD();           // protected constructor used by dynamic creation
	virtual ~CCementBD();

	virtual CMenu* SetModulesExtern();
	virtual void RenameMenuItem(UINT ID_menu_item, CString new_name);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


