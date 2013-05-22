#if !defined(AFX_DLGWAITING_H__1DF27928_F4A2_4B2C_A248_AC61D5C1EF06__INCLUDED_)
#define AFX_DLGWAITING_H__1DF27928_F4A2_4B2C_A248_AC61D5C1EF06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWaiting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgWaiting dialog

class DlgWaiting : public CDialog
{
// Construction
public:
	DlgWaiting(bool available_to_cancel, CWnd* pParent = NULL);   // standard constructor

	bool m_available_to_cancel, m_inited;

	void Start(int total_time_in_millisec);
	void Pause();
	void Resume();
	void Stop();

	DWORD GetTotalWitingTime() { return m_total_tick_count; }
	DWORD GetTotalWitedTime() { return m_passed_tick_count + GetTickCount() - m_start_tick_count; }

	void SignalToClose(int id);

	//{{AFX_DATA(DlgWaiting)
	enum { IDD = IDD_DIALOG_WAITING };
	CProgressCtrl	m_progress_bar;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgWaiting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	DWORD m_total_tick_count, m_passed_tick_count, m_start_tick_count;

	// Generated message map functions
	//{{AFX_MSG(DlgWaiting)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnButtonStop();
	afx_msg LRESULT OnEnforceClose(WPARAM id, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWAITING_H__1DF27928_F4A2_4B2C_A248_AC61D5C1EF06__INCLUDED_)
