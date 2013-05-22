#pragma once

#include "KRS.h"

#define WM_DlgWaitDB_END_DIALOG (WM_USER + 1)

class DlgWaitDB : public CDialog
{
	DECLARE_DYNAMIC(DlgWaitDB)

public:
	DlgWaitDB(CWnd* pParent = NULL);
	virtual ~DlgWaitDB();

	enum { IDD = IDD_DIALOG_WAIT_DB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();
	BOOL OnInitDialog();
	void SetupText();
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonChange();
	afx_msg LRESULT OnEnforceClose(WPARAM id, LPARAM);
};

extern DlgWaitDB DlgWaitDB_dlg;