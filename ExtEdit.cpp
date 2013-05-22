// ExtEdit.cpp : implementation file
//

#include "stdafx.h"
//#include "cartruck.h"
#include "ExtEdit.h"
#include "ExtListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtEdit

CExtEdit::CExtEdit()
{
//	bCaretIsShown = false;
	
//	br.CreateSolidBrush(RGB(0,255,0));
}

CExtEdit::CExtEdit(CExtListCtrl *ListCtrl,
				   int &nItem,
				   int &nSubItem,
				   int nItemCount,
				   int nSubItemCount)
{
//	bCaretIsShown = false;
	m_pExtListCtrl = ListCtrl;
	m_nItem = nItem;
	m_nSubItem = nSubItem;
	m_nItemCount = nItemCount;
	m_nSubItemCount = nSubItemCount;

	if(m_pExtListCtrl != NULL) {
		TextColor = m_pExtListCtrl->GetItemTextColor(m_nItem, m_nSubItem);
		BkColor = m_pExtListCtrl->GetItemBkColor(m_nItem, m_nSubItem);
//		Font = m_pExtListCtrl->GetItemFont(m_nItem, m_nSubItem);
		Alignment = m_pExtListCtrl->GetItemTextAlignment(m_nItem, m_nSubItem);
	}

//	SetFont(Font);
	br.CreateSolidBrush(BkColor);
}

CExtEdit::~CExtEdit()
{
	br.DeleteObject();
}


BEGIN_MESSAGE_MAP(CExtEdit, CEdit)
	//{{AFX_MSG_MAP(CExtEdit)
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtEdit message handlers

void CExtEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	UP		=	38
//	DOWN	=	40
/*
	if(bCaretIsShown == false) {
		CString str;
		switch(nChar) {
			case VK_UP :	GetWindowText(str);
							m_pExtListCtrl->SetItemText(m_nItem,m_nSubItem,str);
							m_nItem--;
							if(m_nItem < 0) m_nItem = 0;
							m_pExtListCtrl->CreateEdit(m_nItem,m_nSubItem);
							break;
			case VK_DOWN :	GetWindowText(str);
							m_pExtListCtrl->SetItemText(m_nItem,m_nSubItem,str);
							m_nItem++;
							if(m_nItem > m_nItemCount - 1) m_nItem = m_nItemCount - 1;
							m_pExtListCtrl->CreateEdit(m_nItem,m_nSubItem);
							break;
			case VK_LEFT :	GetWindowText(str);
							m_pExtListCtrl->SetItemText(m_nItem,m_nSubItem,str);
							m_nSubItem--;
							if(m_nSubItem < 0) m_nSubItem = 0;
							m_pExtListCtrl->CreateEdit(m_nItem,m_nSubItem);
							break;
			case VK_RIGHT :	GetWindowText(str);
							m_pExtListCtrl->SetItemText(m_nItem,m_nSubItem,str);
							m_nSubItem++;
							if(m_nSubItem > m_nSubItemCount - 1) m_nSubItem = m_nSubItemCount - 1;
							m_pExtListCtrl->CreateEdit(m_nItem,m_nSubItem);					
							break;
			default : break;
		}
	}
*/
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CExtEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	m_pExtListCtrl->fChar = -1;
	CString str;
	GetWindowText(str);
	m_pExtListCtrl->SetItemText(m_nItem,m_nSubItem,str);
	ShowWindow(SW_HIDE);
//	PostMessage(WM_CLOSE);
}

BOOL CExtEdit::PreTranslateMessage(MSG* pMsg) 
{
/*
	if(pMsg->message == WM_KEYDOWN) {
		int nChar = (int)pMsg->wParam;
		if(nChar == VK_RETURN) {
			if(!bCaretIsShown) {
				SetReadOnly(false);
				SetSel(0,-1);
				ShowCaret();
				bCaretIsShown = true;
			} else {
				SetSel(0,0);
				HideCaret();
				SetReadOnly(true);
				bCaretIsShown = false;
			}
			
			return true;
		}
	}
*/	
	return CEdit::PreTranslateMessage(pMsg);
}

void CExtEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
/*
	SetReadOnly(false);
	SetSel(0,-1);
	ShowCaret();
	bCaretIsShown = true;
*/	
	CEdit::OnLButtonDown(nFlags, point);
}

/*
void CExtEdit::OnPaint() 
{

	CPaintDC dc(this); // device context for painting

	CRect rect;
	//GetWindowRect(&rect);
	GetClientRect(&rect);
	//LPTSTR str;
	//GetLine(0,str);
	CString str;
	GetWindowText(str);

//	CPen Pen(PS_SOLID,1,RGB(0,0,0));
	
//	CPen *OldPen = dc.SelectObject(&Pen);
	
//	dc.SelectObject(OldPen);

	
	
	CFont *ItemFont = m_pExtListCtrl->GetItemFont(m_nItem,m_nSubItem);

	CPoint pos = GetCaretPos();

	LOGFONT LogFont;
	ItemFont->GetLogFont(&LogFont);

	CreateSolidCaret(1,LogFont.lfHeight);

	pos.y += (rect.Height() - LogFont.lfHeight) / 2;

	SetCaretPos(pos);

	CRect txtrect;
	GetRect(&txtrect);

//	dc.FillSolidRect(&rect, m_pExtListCtrl->GetItemBkColor(m_nItem,m_nSubItem));
	
	CFont *OldFont = dc.SelectObject(ItemFont);

//	CPen Pen(PS_SOLID,1,RGB(0,0,0));
	
//	CPen *OldPen = dc.SelectObject(&Pen);
	
//	dc.SelectObject(OldPen);	

	dc.SetBkMode(OPAQUE);
	dc.SetBkColor(m_pExtListCtrl->GetItemBkColor(m_nItem, m_nSubItem));

//	dc.DrawText(str,&rect,DT_SINGLELINE | DT_VCENTER);

	dc.SelectObject(OldFont);

	// Do not call CEdit::OnPaint() for painting messages
}
*/

void CExtEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);

	if(m_pExtListCtrl->fChar != -1) {
//		CString str = (char)(m_pExtListCtrl->fChar);
//		SetWindowText(str);
		SetWindowText("");
		OnKeyDown(m_pExtListCtrl->fChar,1,2078);
//		PostMessage(WM_KEYDOWN,NULL,NULL);
		SetSel(1,-1);
	}
	else SetSel(0,-1);
}

HBRUSH CExtEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	br.DeleteObject();
//	pDC->SetTextAlign(DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(TextColor);
	pDC->SetBkColor(BkColor);
	br.CreateSolidBrush(BkColor);

	return br;
}
