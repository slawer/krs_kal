#if !defined(AFX_DLGEVENTJRN_H__9D4F3E57_A7A5_4898_82BD_2EDA39A47758__INCLUDED_)
#define AFX_DLGEVENTJRN_H__9D4F3E57_A7A5_4898_82BD_2EDA39A47758__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEventJrn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEventJrn dialog

class CDlgEventJrn : public CDialog
{
// Construction
public:
	CDlgEventJrn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEventJrn)
	enum { IDD = IDD_DIALOG_JOURNAL };
	CListCtrl	m_LstJourn;
	//}}AFX_DATA

	CMutex m_list_mutex;

	COleDateTime tmEvnt;
	SYSTEMTIME sytime;
	LV_ITEM item;
	CString cs1;

	double enTime, bgTime;
	int typeTime;

//	void FillList(CDaoRecordset *recSet, int nCase);
	void AddCaseToList(CString& sName, float fVal, CString& cs, double dTime);
	void FillListEvent(int nCase);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEventJrn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEventJrn)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAlertAvaria();
	afx_msg void OnButtonAlertaMax();
	afx_msg void OnButtonAlertMin();
	afx_msg void OnButtonAlertAll();
	afx_msg void OnButtonJrnlHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEVENTJRN_H__9D4F3E57_A7A5_4898_82BD_2EDA39A47758__INCLUDED_)
