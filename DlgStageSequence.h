#pragma once

#include "ExtListCtrl.h"

class DlgStageSequence : public CDialog
{
	DECLARE_DYNAMIC(DlgStageSequence)

public:
	DlgStageSequence(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgStageSequence();

	enum { IDD = IDD_DIALOG_STAGES_SEQ };
	CExtListCtrl m_list_seq;

	void RebuildTable();
	void EnableEditDelete(BOOL ena);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnLvnItemchangedListSeq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedOk();
};