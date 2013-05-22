#if !defined(AFX_EXTEDIT_H__BF357DE7_1334_11D7_9527_00308488508C__INCLUDED_)
#define AFX_EXTEDIT_H__BF357DE7_1334_11D7_9527_00308488508C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtEdit window

class CExtListCtrl;

class CExtEdit : public CEdit
{
	CExtListCtrl *m_pExtListCtrl;
	int m_nItem, m_nSubItem;
	int m_nItemCount, m_nSubItemCount;
	COLORREF BkColor, TextColor;
	UINT Alignment;
//	CFont *Font;
// Construction
public:
	CExtEdit();
	CExtEdit(	CExtListCtrl *ListCtrl,
				int &nItem,
				int &nSubItem,
				int nItemCount,
				int nSubItemCount);

// Attributes
public:
//	bool bCaretIsShown;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtEdit();

private:
	CBrush br;

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTEDIT_H__BF357DE7_1334_11D7_9527_00308488508C__INCLUDED_)
