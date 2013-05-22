#pragma once

#include "ExtListCtrl.h"

class DlgStagesLibrary : public CDialog
{
	DECLARE_DYNAMIC(DlgStagesLibrary)

public:
	DlgStagesLibrary(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgStagesLibrary();

	enum { IDD = IDD_DIALOG_STAGES_LIB };
	CExtListCtrl m_list_lib;

	void RebuildTable();
	void EnableEditDelete(BOOL ena);
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	CParamMap* m_prev_map;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLvnItemchangedListLib(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnDestroy();	
};
#pragma once