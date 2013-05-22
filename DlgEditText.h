#if !defined(AFX_DLGEDITPANELNAME_H__7532C41B_65F7_41C6_81B5_569E14CF35C2__INCLUDED_)
#define AFX_DLGEDITPANELNAME_H__7532C41B_65F7_41C6_81B5_569E14CF35C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DlgEditText : public CDialog
{
public:
	DlgEditText(CString text, CRect rect, bool bold, int alignment, CWnd* pParent = NULL);   // standard constructor

	CFont* m_main_font;
	CFont* m_bold_font;

	CRect m_rect;
	int m_res;
	bool m_bold;
	int m_alignment;
	CString GetText();

	//{{AFX_DATA(DlgEditText)
	enum { IDD = IDD_DIALOG_EDIT_TEXT };
	CString	m_text_centered;
	CString	m_text_left;
	CString	m_text_right;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(DlgEditText)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(DlgEditText)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
    afx_msg LRESULT OnEnforceFlose(WPARAM, LPARAM);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	void OnCancel();
	void OnOK();
	void OnCloseDialog();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITPANELNAME_H__7532C41B_65F7_41C6_81B5_569E14CF35C2__INCLUDED_)
