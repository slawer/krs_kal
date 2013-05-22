#if !defined(AFX_DLGSTAGESETTINGS_H__B7998D34_EF43_4269_BFCA_66F16DC8D738__INCLUDED_)
#define AFX_DLGSTAGESETTINGS_H__B7998D34_EF43_4269_BFCA_66F16DC8D738__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "STAGES.h"

class DlgStageSettings : public CDialog
{
public:
	DlgStageSettings(STAGES_StageTemplate* stage_template, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(DlgStageSettings)
	enum { IDD = IDD_DIALOG_STAGE_SETTINGS };
	CListBox	m_list_stages;
	//}}AFX_DATA

	STAGES_StageTemplate* m_stage_template;

protected:
	//{{AFX_VIRTUAL(DlgStageSettings)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//{{AFX_MSG(DlgStageSettings)
	afx_msg void OnButtonAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListStages();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGSTAGESETTINGS_H__B7998D34_EF43_4269_BFCA_66F16DC8D738__INCLUDED_)