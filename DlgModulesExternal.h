#pragma once
#include "afxcmn.h"
#include "ExtListCtrl.h"


// CDlgModulesExternal dialog

class CDlgModulesExternal : public CDialog
{
	DECLARE_DYNAMIC(CDlgModulesExternal)

public:
	CDlgModulesExternal(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgModulesExternal();

    void RebuildList();
	void ChangeList();

// Dialog Data
	enum { IDD = IDD_DIALOG_MODULES_EXTERNAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CExtListCtrl m_list_modules;
	afx_msg void OnBnClickedButtonModuleDelete();
    afx_msg LRESULT OnMenuListExternal(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
public:
    afx_msg void OnBnClickedOk();
};
