#if !defined(AFX_DLGEDITMARKERTYPE_H__53C16F91_5A28_4AF1_9BEC_FCA06C29F94B__INCLUDED_)
#define AFX_DLGEDITMARKERTYPE_H__53C16F91_5A28_4AF1_9BEC_FCA06C29F94B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <DB.h>
#include "DlgWaiting.h"

/////////////////////////////////////////////////////////////////////////////
// DlgEditMarkerType dialog

class DlgEditMarkerType : public CDialog
{
// Construction
public:
	DlgEditMarkerType(BS_MarkerType* marker, CWnd* pParent = NULL);   // standard constructor

	BS_MarkerType* m_marker;
	DlgWaiting m_dlg_waiting;

// Dialog Data
	//{{AFX_DATA(DlgEditMarkerType)
	enum { IDD = IDD_DIALOG_EDIT_MARKER_TYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgEditMarkerType)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgEditMarkerType)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG

	afx_msg LRESULT OnDataDB(WPARAM, LPARAM);
	afx_msg LRESULT OnErrorDB(WPARAM code, LPARAM string);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITMARKERTYPE_H__53C16F91_5A28_4AF1_9BEC_FCA06C29F94B__INCLUDED_)
