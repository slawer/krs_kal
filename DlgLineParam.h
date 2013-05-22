#if !defined(AFX_DLGLINEPARAM_H__62CAED94_F4D6_4510_9BA7_6D3CF3FF9120__INCLUDED_)
#define AFX_DLGLINEPARAM_H__62CAED94_F4D6_4510_9BA7_6D3CF3FF9120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLineParam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgLineParam dialog

class CDlgLineParam : public CDialogBar
{
// Construction
public:
	CDlgLineParam();   // standard constructor

	void ChangePosition(CWnd *pWnd, int wl, int wt, int ww, int wh);

// Dialog Data
	//{{AFX_DATA(CDlgLineParam)
	enum { IDD = IDD_DIALOG_LINE_PARAM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLineParam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDlgLineParam();
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLineParam)
	afx_msg void OnPaint();
	afx_msg LRESULT OnChangePosition(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLINEPARAM_H__62CAED94_F4D6_4510_9BA7_6D3CF3FF9120__INCLUDED_)
