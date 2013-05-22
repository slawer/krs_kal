#if !defined(AFX_DLGSETTINGSBO_H__6073632D_179C_40FE_9240_3352E5C13BDC__INCLUDED_)
#define AFX_DLGSETTINGSBO_H__6073632D_179C_40FE_9240_3352E5C13BDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DlgSettingsBO : public CDialog
{
public:
	DlgSettingsBO(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(DlgSettingsBO)
	enum { IDD = IDD_DIALOG_SETTINGS_BO };
	CListBox	m_list_bo;
	//}}AFX_DATA

	int m_bo_time;
	void OutputBOTime();

	//{{AFX_VIRTUAL(DlgSettingsBO)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(DlgSettingsBO)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonDelete();
	afx_msg void OnSelchangeListBo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPlusBoTime();
public:
	afx_msg void OnBnClickedMinusBoTime();
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETTINGSBO_H__6073632D_179C_40FE_9240_3352E5C13BDC__INCLUDED_)
