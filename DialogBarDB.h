#if !defined(AFX_DIALOGBARDB_H__7BAEFA7E_4C47_4A7A_A020_87E26C5CC82C__INCLUDED_)
#define AFX_DIALOGBARDB_H__7BAEFA7E_4C47_4A7A_A020_87E26C5CC82C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DlgEventJrn.h"
#include "PanelKnopki.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectData dialog

class CDlgSelectData : public CDialog
{
// Construction
public:
	CDlgSelectData(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelectData)
	enum { IDD = IDD_DIALOG_SELECTDATA };
	COleDateTime	m_DataBeg;
	COleDateTime	m_DataEnd;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectData)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDialogBarDB dialog

class CDialogBarDB : public CDialogBar
{
// Construction
public:
	CDialogBarDB();   // standard constructor
	CDlgEventJrn m_dlgEventJrn;


	void IncrementBytes(int bytes);
	void IncrementTime(DWORD ticks);

// Dialog Data
	//{{AFX_DATA(CDialogBarDB)
	enum { IDD = IDD_DIALOG_BAR_DB };
	CStatic	m_static_db;
	CProgressCtrl	m_progress_db;
	int		m_iStartTime;
	int		m_iDiapazonTime;
    CString	m_Minut;
    CString	m_Hour;
    CString	m_Day;
    CString	m_Month;
    CString	m_Year;
    CString	m_TimeLength;
    CString	m_TimeLengthMinut;
    CString	m_GlubStart;
    CString	m_GlubLength;
    int	m_CheckTimeGlub;
	CPanelKnopki	m_PanelKnopki;
	//}}AFX_DATA

	void SetCheckControl(int bflag_chek);
	void CheckControl(int flg_callback = 0);
    void StartRequestDB();
	void FinishRequestDB();

	bool CheckTimeInterval();
    void SaveControl();
	void GetDataFromEdit(CString& inp_str, int& value, int max_len, int min_value, int max_value, 
		DWORD cur_control, DWORD next_control);
	void TestKillFocus(CString& inp_str, int& value, int max_len, int min_value, int max_value, 
		DWORD cur_control, DWORD next_control);
	BOOL TestFocusItem(DWORD next_control);
	void SetValidData(CString& inp_str, int& value, int max_len, DWORD next_control);
	void SetInValidData(CString& inp_str, int& value, int max_len, DWORD cur_control);
    void RecalcPosIndicators();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogBarDB)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
    virtual void Serialize(CArchive& ar);
	virtual ~CDialogBarDB();
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogBarDB)
	afx_msg void OnButtonSelectDay();
	afx_msg void OnButtonSelectFree2();
	afx_msg void OnButtonOtchet4();
	afx_msg void OnButtonOtchetFree2();
	afx_msg void OnUpdateControl(CCmdUI* pCmdUi);
	afx_msg void OnUpdateEnable(CCmdUI* pCmdUi);
	afx_msg void OnMyInitDialog();
   	afx_msg LRESULT OnMyInitDialog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonBreakDbRequest();
    afx_msg void OnChangeEditMinut();
    afx_msg void OnChangeEditHour();
    afx_msg void OnChangeEditDay();
    afx_msg void OnChangeEditYear();
    afx_msg void OnChangeEditMonth();
    afx_msg void OnChangeEditTimeLen();
    afx_msg void OnChangeEditTimeLenMinut();
    afx_msg void OnChangeEditGlubStart();
    afx_msg void OnChangeEditGlubLen();
    afx_msg void OnButtonDbRequest();
    afx_msg void OnRadioCheckTimeGlub();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnEnKillfocusEditMinut();
	afx_msg void OnEnKillfocusEditHour();
	afx_msg void OnEnKillfocusEditDay();
	afx_msg void OnEnKillfocusEditMonth();
	afx_msg void OnEnKillfocusEditYear();
	afx_msg void OnEnKillfocusEditIntervalTime();
    afx_msg void OnEnKillfocusEditIntervalTimeMinut();
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectDataDay dialog

class CDlgSelectDataDay : public CDialog
{
// Construction
public:
	CDlgSelectDataDay(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelectDataDay)
	enum { IDD = IDD_DIALOG_SELECTDATADAY };
	COleDateTime	m_DataDay;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectDataDay)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectDataDay)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGBARDB_H__7BAEFA7E_4C47_4A7A_A020_87E26C5CC82C__INCLUDED_)
