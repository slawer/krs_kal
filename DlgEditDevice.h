#if !defined(AFX_DLGEDITDEVICE_H__BCC4E829_4775_4DBE_A125_9D394B50978D__INCLUDED_)
#define AFX_DLGEDITDEVICE_H__BCC4E829_4775_4DBE_A125_9D394B50978D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SERV.h>

/////////////////////////////////////////////////////////////////////////////
// DlgEditDevice dialog

class DlgEditDevice : public CDialog
{
// Construction
public:
	DlgEditDevice(SERV_Device* device, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(DlgEditDevice)
	enum { IDD = IDD_DIALOG_EDIT_DEVICE };
	CComboBox m_combo_addr;
	CString	m_string_name;
	int		m_to_interval;
	//}}AFX_DATA

	SERV_Device* m_device;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgEditDevice)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgEditDevice)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITDEVICE_H__BCC4E829_4775_4DBE_A125_9D394B50978D__INCLUDED_)
