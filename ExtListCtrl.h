#if !defined(AFX_EXTLISTCTRL_H__69E8AAFE_BDBE_11D6_9098_AE62F8AA7077__INCLUDED_)
#define AFX_EXTLISTCTRL_H__69E8AAFE_BDBE_11D6_9098_AE62F8AA7077__INCLUDED_

#include <vector>
#include "ExtEdit.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtListCtrl.h : header file
//

using namespace std;

typedef void (*ExListRedrawItemFunction)(CDC* , int, int, CRect*);

struct ItemProperties
{
	int nItem;
	int nSubItem;
	COLORREF TextColor;
	COLORREF BkColor;
	int FontType;			//	FW_BOLD, ...
	CFont *Font;
	UINT alignment;
	UINT style;
	ExListRedrawItemFunction m_pf;
	CString m_tip;
};

typedef vector<ItemProperties> VALLIST;
/////////////////////////////////////////////////////////////////////////////
// CExtListCtrl window

class CExtListCtrl : public CListCtrl
{
// Construction
public:
	CExtListCtrl();
	virtual ~CExtListCtrl();

	//UINT m_sel_changed_msg, m_sel_changed_msg_dclc;
	//CWnd* m_sel_changed_wnd;
	UINT m_lb_click_msg, m_lb_dbl_click_msg, m_rb_click_msg, m_rb_dbl_click_msg, m_key_down_msg;
	CWnd* m_messages_wnd;

	bool m_show_selection;
	bool m_allow_move_selection_by_keys;

	//{{AFX_VIRTUAL(CExtListCtrl)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// TOOLTIPS >>
	void SetToolTip(CString tip_str, int nItem, int nSubItem);
	virtual int OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;
	CToolTipCtrl m_tool_tip;
	int m_mouse_in_item, m_mouse_in_subitem;
	// TOOLTIPS <<
	virtual void OnMouseMoveIn(CPoint point) {}

	CFont* SetHeaderFont(CFont *font);
	void SetListFont(CFont* pFont, bool bRedraw = true);
	void SetColFont(int nSubItem, CFont *NewFont);
	void SetRowFont(int nItem, CFont *NewFont);
	void SetItemFont(int nItem, int nSubItem, CFont *NewFont);
	CFont* GetItemFont(int nItem, int nSubItem);

	void SetColTextAlign(int nSubItem, UINT nFormat);
	void SetItemTextAlign(int nItem, int nSubItem, UINT nFormat);
	UINT GetItemTextAlignment(int nItem, int nSubItem);

	void SetColTextColor(int nSubItem, COLORREF Color);				//Sets text color in selected column
	void SetRowTextColor(int nItem, COLORREF Color);				//Sets text color in selected row
	void SetItemTextColor(int nItem, int nSubItem, COLORREF Color);	//Sets text color in selected item
	COLORREF GetItemTextColor(int nItem, int nSubItem);

	void SetColBkColor(int nSubItem, COLORREF Color);				//Sets background color in selected column
	void SetRowBkColor(int nItem, COLORREF Color);					//Sets background color in selected row
	void SetItemBkColor(int nItem, int nSubItem, COLORREF Color);	//Sets background color in selected item
	COLORREF GetItemBkColor(int nItem, int nSubItem);

	void SetListStyle(UINT style, bool bChangeAllItems = false);	//Sets style in all not special list items
	void SetColStyle(int nSubItem, UINT style);						//Sets style in selected column
	void SetRowStyle(int nItem, UINT style);						//Sets style in selected row
	void SetItemStyle(int nItem, int nSubItem, UINT style);			//Sets style in selected item
	
	void SetCaretColor(COLORREF Color) { nCaretColor = Color; }
	void SetCaretTextColor(COLORREF Color) { nCaretTextColor = Color; }

	void SetCursorInvisible(BOOL bInvisible = TRUE);

	UINT GetItemStyle(int nItem, int nSubItem);

	BOOL EnableHeaders(BOOL bEnable);

	void SetRedrawFunction(ExListRedrawItemFunction f, int nItem, int nSubItem);
	
	int GetCurItem() { return curItem; }
	void SetCurItem(int item) { curItem = item; }
	int GetCurSubItem() { return curSubItem; }
	int fChar;
	
	void SetSelectionChangedMessage(DWORD selection_changed_msg) { m_selection_changed_msg = selection_changed_msg; }

private:
	DWORD m_selection_changed_msg;

	BOOL _bInvisible;

	int nCharBuf;			//keep the code of SHIFT and CONTROL code if one of keys are pressed
							//( CTRL->HOME:END for caret dispacement and
							//	SHIFT->"default text symbol" for edit activation )
	int curItem;
	int curSubItem;

	int OldnItem;
	int OldnSubItem;

	int nCaretColor, nCaretColorInactive;
	int nCaretTextColor;

	bool bIsItemChanging;
	bool bIsListFocused;

	UINT nStyle;			//Other items style in list

	CString OldStr;			//Old string value in selected item ( for ESCAPE key )

	CRect itemRect;
	ItemProperties ItemProps;

	VALLIST itemslist;
	VALLIST rowslist;
	VALLIST colslist;

	CFont *m_pOldFont;
	CFont m_pFont;
	CFont *ItemFont;

	CExtEdit *EditItem;
	CComboBox *CBoxItem;

	int FindElement(int nItem, int nSubItem);
	void SetDefaultElement(int nItem, int nSubItem);
	int FindRow(int nItem);
	int FindColumn(int nSubItem);

	void DrawItem(LPDRAWITEMSTRUCT lpDIS);

	void ChangeSelItemText();
	void CreateEdit(int nItem, int nSubItem);							//creating edit in selected item 
	void CreateComboBox(int nItem, int nSubItem);
	void ScrollCaret(int nItem, int nSubItem);							//scrolling caret in fixed pos
	void DrawCaret(CDC *pDC, int nItem, int nSubItem);					//drawing caret
	void SelectCtrl(UINT style);										//if style = 1 then CreateEdit

	int GetSubItemCount();												//Returns number of columns in the List Ctrl
	int GetSubItemCountPerPage();

protected:
	bool m_mem_dc_available;
	CDC m_mem_dc;
	CBitmap m_bitmap;

protected:
	//{{AFX_MSG(CExtListCtrl)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTLISTCTRL_H__69E8AAFE_BDBE_11D6_9098_AE62F8AA7077__INCLUDED_)