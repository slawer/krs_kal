#if !defined(AFX_DLGEDITSENSOR_H__BCE46868_8525_4E13_A598_D4F488BA6D31__INCLUDED_)
#define AFX_DLGEDITSENSOR_H__BCE46868_8525_4E13_A598_D4F488BA6D31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SERV.h>

/////////////////////////////////////////////////////////////////////////////
// DlgEditSensor dialog

class DlgEditSensor : public CDialog
{
public:
	DlgEditSensor(SERV_Channel* channel, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(DlgEditSensor)
	enum { IDD = IDD_DIALOG_EDIT_SENSOR };
	Indicator	m_ind_value;
	CString	m_channel_name;
	int		m_data_offset;
	int		m_data_len;
	BOOL	m_use_voltage;
	int		m_dsp0;
	int		m_dsp1;
	float	m_v0;
	float	m_v1;
	BOOL	m_check_jumps;
	int		m_jump_up;
	int		m_jump_down;
	//}}AFX_DATA

	SERV_Channel *m_channel, *m_channel_original;
	DWORD m_last_value;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgEditSensor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgEditSensor)
	virtual void OnOK();
	afx_msg void OnCheckUseVoltage();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITSENSOR_H__BCE46868_8525_4E13_A598_D4F488BA6D31__INCLUDED_)
