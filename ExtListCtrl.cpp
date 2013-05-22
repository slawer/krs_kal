#include "stdafx.h"
#include "ExtListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_EDITITEM                    20001
#define IDC_CBOXITEM                    20002

CExtListCtrl::CExtListCtrl():
	m_mouse_in_item(-1), m_mouse_in_subitem(-1),
	m_show_selection(true), m_allow_move_selection_by_keys(true),
	m_lb_click_msg(0), m_lb_dbl_click_msg(0),
	m_rb_click_msg(0), m_rb_dbl_click_msg(0),
	m_key_down_msg(0),
	m_messages_wnd(NULL),
	m_mem_dc_available(false)
{
	_bInvisible = FALSE;

	EditItem = NULL;
	CBoxItem = NULL;
	ItemFont = NULL;

	OldnItem = 0;
	OldnSubItem = 0;

	curItem = 0;
	curSubItem = 0;

	OldStr = "";
	nCaretColor = GetSysColor(COLOR_HIGHLIGHT);
	nCaretColorInactive = GetSysColor(COLOR_INACTIVECAPTIONTEXT/*COLOR_INACTIVECAPTION*/);
	nCaretTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);

	nStyle = 0;			//equal to 0, when no edit, button and combobox (it is default)

	bIsItemChanging = false;
	bIsListFocused = false;
	
	nCharBuf = -1;
	
	fChar = -1;

	VERIFY(m_pFont.CreateFont(
			14,                        // nHeight
			7,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_THIN,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			DEFAULT_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily   
//			"Times New Roman"));                 
			"Arial"));                 
//			"Arial Narrow"));

}

CExtListCtrl::~CExtListCtrl()
{
	if (EditItem != NULL) {
		delete EditItem;
	}

	if (CBoxItem != NULL) {
		delete CBoxItem;
	}
}


BEGIN_MESSAGE_MAP(CExtListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CExtListCtrl)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtListCtrl::OnSize(UINT nType, int cx, int cy)
{
	if (cx > 0 && cy > 0)
	{
		if (m_mem_dc_available)
		{
			m_bitmap.DeleteObject();
			m_mem_dc.DeleteDC();
		}
		m_mem_dc_available = false;
		CClientDC dc(this);
		if (m_mem_dc.CreateCompatibleDC(&dc))
		{
			if (m_bitmap.CreateCompatibleBitmap(&dc, cx, cy))
			{
				if (m_mem_dc.SelectObject(&m_bitmap) != NULL)
                {
				    //m_mem_dc.SelectObject(&m_pen);
				    //m_mem_dc.SelectObject(&m_brush);
				    m_mem_dc_available = true;
                }
			}
			else
				m_mem_dc.DeleteDC();
		}
	}
}

void CExtListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	int top_index = GetTopIndex();
	int bottom_index = top_index + GetCountPerPage();
	if ((int)lpDIS->itemID < top_index || (int)lpDIS->itemID > bottom_index)
		return;

	LVITEM lvItem;

	char str[256];

	int SubItem = 0;

	DWORD list_style = GetExtendedStyle();
	bool selecting = m_show_selection && ((list_style | LVS_SHOWSELALWAYS) || (CWnd *)this == GetFocus());

	CDC* screen_dc = CDC::FromHandle(lpDIS->hDC);
	CDC* pDC = (m_mem_dc_available)?(&m_mem_dc):screen_dc;

    CRect list_rect;
    GetClientRect(&list_rect);

	int scrl_pos = GetScrollPos(SB_HORZ);

	int wAll = -scrl_pos;

	while (lpDIS->rcItem.right > wAll)	
	{
		lvItem.iItem = lpDIS->itemID;
		lvItem.iSubItem = SubItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.cchTextMax = sizeof (str);
		lvItem.pszText = str;

		GetItem(&lvItem);
		int w0 = GetColumnWidth(SubItem);
		
		wAll += w0;

		CRect Rect;
		Rect = lpDIS->rcItem;
		Rect.left = wAll - w0;
		Rect.right = wAll;

		COLORREF BKCOLOR = GetBkColor();
		COLORREF TEXTCOLOR = GetTextColor();
		CFont *FONT = GetFont();
		ASSERT(FONT);
		UINT ALIGNMENT = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
		UINT STYLE;
		ExListRedrawItemFunction _myfunc = NULL;
			
		int element = FindElement(lpDIS->itemID,SubItem);
		if (element != -1)
		{
			BKCOLOR = (itemslist.begin() + element)->BkColor;
			TEXTCOLOR = (itemslist.begin() + element)->TextColor;
			FONT = (itemslist.begin() + element)->Font;
			ALIGNMENT = (itemslist.begin() + element)->alignment;
			STYLE = (itemslist.begin() + element)->style;
			_myfunc = (itemslist.begin() + element)->m_pf;
		}
		else
		{
			element = FindRow(lpDIS->itemID);
			if (element != -1)
			{
				BKCOLOR = (rowslist.begin() + element)->BkColor;
				TEXTCOLOR = (rowslist.begin() + element)->TextColor;
				FONT = (rowslist.begin() + element)->Font;
				ALIGNMENT = (rowslist.begin() + element)->alignment;
				STYLE = (rowslist.begin() + element)->style;
				_myfunc = (rowslist.begin() + element)->m_pf;
			}
			else
			{
				element = FindColumn(SubItem);
				if (element != -1)
				{
					BKCOLOR = (colslist.begin() + element)->BkColor;
					TEXTCOLOR = (colslist.begin() + element)->TextColor;
					FONT = (colslist.begin() + element)->Font;
					ALIGNMENT = (colslist.begin() + element)->alignment;
					STYLE = (colslist.begin() + element)->style;
					_myfunc = (colslist.begin() + element)->m_pf;
				}
			}

		}

		if (m_mem_dc_available)
        	m_mem_dc.SelectObject(FONT);            

		if (_myfunc != NULL)
			_myfunc(pDC, lpDIS->itemID, SubItem, &Rect);
		else
		{
			CRgn Rgn;
			CRect OldRect;
			pDC->GetClipBox(&OldRect);
			Rgn.CreateRectRgn(Rect.left, Rect.top, Rect.right, Rect.bottom);
			pDC->SelectClipRgn(&Rgn);
			pDC->FillSolidRect(&Rect,BKCOLOR);
			pDC->SetTextColor(TEXTCOLOR);
			pDC->SelectObject(FONT);
			Rect.left+=3;
			pDC->DrawText(str,&Rect,ALIGNMENT);
			CRgn Rgn1;
			Rgn1.CreateRectRgn(OldRect.left, OldRect.top, OldRect.right, OldRect.bottom);
			pDC->SelectClipRgn(&Rgn1);
		}
		if (selecting && lpDIS->itemID == curItem)
		{
			if ((list_style | LVS_EX_FULLROWSELECT) || SubItem == curSubItem)
				DrawCaret(pDC, lpDIS->itemID, SubItem);
		}
        if (m_mem_dc_available)
            screen_dc->BitBlt(0, 0, Rect.right, Rect.bottom, &m_mem_dc, 0, 0, SRCCOPY);
		SubItem++;
	}

//	CRect tRect;
//	GetSubItemRect(curItem, curSubItem, LVIR_LABEL, tRect);
}

void CExtListCtrl::SetItemTextColor(int nItem, int nSubItem, COLORREF Color)
{
	int element = FindElement(nItem, nSubItem);
	if (element != -1)
		(itemslist.begin() + element)->TextColor = Color;
	else
	{
		ItemProps.nItem = nItem;
		ItemProps.nSubItem = nSubItem;

		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.m_pf = NULL;
		
		int element0 = FindRow(nItem);
		if (element0 != -1)
		{
			ItemProps.BkColor = (rowslist.begin() + element0)->BkColor;
			ItemProps.Font = (rowslist.begin() + element0)->Font;
			ItemProps.alignment = (rowslist.begin() + element0)->alignment;
		}
		else
		{
			int element0 = FindColumn(nSubItem);
			if (element0 != -1)
			{
				ItemProps.BkColor = (colslist.begin() + element0)->BkColor;
				ItemProps.Font = (colslist.begin() + element0)->Font;
				ItemProps.alignment = (colslist.begin() + element0)->alignment;
			}
			else
			{
				ItemProps.BkColor = GetBkColor();//RGB(255,255,255);
				ItemProps.Font = GetFont();
				ItemProps.style = nStyle;
			}
		}
		
		ItemProps.FontType = FW_THIN;
		ItemProps.TextColor = Color;
		
		itemslist.insert(itemslist.begin(),ItemProps);
	}
	Invalidate();
}

void CExtListCtrl::SetItemBkColor(int nItem, int nSubItem, COLORREF Color)
{
	int element = FindElement(nItem, nSubItem);
	if (element != -1)
		(itemslist.begin() + element)->BkColor = Color;
	else
	{
		ItemProps.nItem = nItem;
		ItemProps.nSubItem = nSubItem;
		ItemProps.BkColor = Color;
		ItemProps.FontType = FW_THIN;
		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.m_pf = NULL;

		int element0 = FindRow(nItem);
		if (element0 != -1)
		{
			ItemProps.TextColor = (rowslist.begin() + element0)->TextColor;
			ItemProps.Font = (rowslist.begin() + element0)->Font;
			ItemProps.alignment = (rowslist.begin() + element0)->alignment;
		}
		else
		{
			int element0 = FindColumn(nSubItem);
			if (element0 != -1)
			{
				ItemProps.TextColor = (colslist.begin() + element0)->TextColor;
				ItemProps.Font = (colslist.begin() + element0)->Font;
				ItemProps.alignment = (colslist.begin() + element0)->alignment;
			}
			else
			{
				ItemProps.TextColor = GetTextColor();//RGB(255,255,255);
				ItemProps.Font = GetFont();
				ItemProps.style = nStyle;
			}
		}
		
		itemslist.insert(itemslist.begin(),ItemProps);
	}
	Invalidate(FALSE);
}

void CExtListCtrl::SetRowTextColor(int nItem, COLORREF Color)
{
	int element = FindRow(nItem);
	if (element != -1)
		(rowslist.begin() + element)->TextColor = Color;
	else
	{
		ItemProps.nItem = nItem;
		ItemProps.BkColor = GetBkColor();//RGB(255,255,255);
		ItemProps.FontType = FW_THIN;
		ItemProps.TextColor = Color;
		ItemProps.Font = GetFont();
		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.style = nStyle;
		ItemProps.m_pf = NULL;

		rowslist.insert(rowslist.begin(),ItemProps);
	}
}

void CExtListCtrl::SetRowBkColor(int nItem, COLORREF Color)
{
	int element = FindRow(nItem);
	if (element != -1)
		(rowslist.begin() + element)->BkColor = Color;
	else
	{
		ItemProps.nItem = nItem;
		ItemProps.BkColor = Color;
		ItemProps.FontType = FW_THIN;
		ItemProps.TextColor = GetTextColor();
		ItemProps.Font = GetFont();
		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.style = nStyle;
		ItemProps.m_pf = NULL;

		rowslist.insert(rowslist.begin(),ItemProps);
	}
}

void CExtListCtrl::SetColTextColor(int nSubItem, COLORREF Color)
{
	int element = FindColumn(nSubItem);
	if (element != -1)
		(colslist.begin() + element)->TextColor = Color;
	else
	{
		ItemProps.nSubItem = nSubItem;
		ItemProps.BkColor = GetBkColor();
		ItemProps.FontType = FW_THIN;
		ItemProps.TextColor = Color;
		ItemProps.Font = GetFont();
		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.style = nStyle;
		ItemProps.m_pf = NULL;

		colslist.insert(colslist.begin(),ItemProps);
	}
}

void CExtListCtrl::SetColBkColor(int nSubItem, COLORREF Color)
{
	int element = FindColumn(nSubItem);
	if (element != -1)
		(colslist.begin() + element)->BkColor = Color;
	else
	{
		ItemProps.nSubItem = nSubItem;
		ItemProps.BkColor = Color;
		ItemProps.FontType = FW_THIN;
		ItemProps.TextColor = GetTextColor();
		ItemProps.Font = GetFont();
		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.style = nStyle;
		ItemProps.m_pf = NULL;

		colslist.insert(colslist.begin(),ItemProps);
	}
	Invalidate(FALSE);
}

void CExtListCtrl::SetItemFont(int nItem, int nSubItem, CFont *NewFont)
{
	CFont *pFont = GetFont();

	LOGFONT pLogListFont;
	pFont->GetLogFont(&pLogListFont);

	LOGFONT pLogNewFont;
	NewFont->GetLogFont(&pLogNewFont);

	if (pLogNewFont.lfHeight > pLogListFont.lfHeight)
		FromHandle(m_hWnd)->ModifyStyle(LVS_OWNERDRAWFIXED,NULL);

	int element = FindElement(nItem,nSubItem);
	if (element != -1)
		(itemslist.begin() + element)->Font = NewFont;
	else
	{
		ItemProps.nItem = nItem;
		ItemProps.nSubItem = nSubItem;
		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.m_pf = NULL;

		int element0 = FindRow(nItem);
		if (element0 != -1)
		{
			ItemProps.BkColor = (rowslist.begin() + element0)->BkColor;
			ItemProps.TextColor = (rowslist.begin() + element0)->TextColor;
			ItemProps.alignment = (rowslist.begin() + element0)->alignment;
		}
		else
		{
			int element0 = FindColumn(nSubItem);
			if (element0 != -1)
			{
				ItemProps.BkColor = (colslist.begin() + element0)->BkColor;
				ItemProps.TextColor = (colslist.begin() + element0)->TextColor;
				ItemProps.alignment = (colslist.begin() + element0)->alignment;
			}
			else
			{
				ItemProps.BkColor = GetBkColor();
				ItemProps.TextColor = GetTextColor();
				ItemProps.style = nStyle;
			}
		}
		
		ItemProps.FontType = FW_THIN;
		ItemProps.Font = NewFont;
		itemslist.insert(itemslist.begin(),ItemProps);
	}

	if (pLogNewFont.lfHeight > pLogListFont.lfHeight)
	{
		SetFont(NewFont,false);
		FromHandle(m_hWnd)->ModifyStyle(NULL,LVS_OWNERDRAWFIXED);

		SetFont(pFont,false);
	}
	
	Invalidate(FALSE);
}

void CExtListCtrl::SetRowFont(int nItem, CFont *NewFont)
{
	CFont *pFont = GetFont();

	LOGFONT pLogListFont;
	pFont->GetLogFont(&pLogListFont);

	LOGFONT pLogNewFont;
	NewFont->GetLogFont(&pLogNewFont);

	if (pLogNewFont.lfHeight > pLogListFont.lfHeight)
		FromHandle(m_hWnd)->ModifyStyle(LVS_OWNERDRAWFIXED,NULL);

	int element = FindRow(nItem);
	if (element != -1)
		(rowslist.begin() + element)->Font = NewFont;
	else
	{
		ItemProps.nItem = nItem;
		ItemProps.BkColor = GetBkColor();
		ItemProps.FontType = FW_THIN;
		ItemProps.TextColor = GetTextColor();
		ItemProps.Font = NewFont;
		
		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.style = nStyle;
		ItemProps.m_pf = NULL;
		
		
		rowslist.insert(rowslist.begin(),ItemProps);
	}

	if (pLogNewFont.lfHeight > pLogListFont.lfHeight)
	{
		SetFont(NewFont,false);
		FromHandle(m_hWnd)->ModifyStyle(NULL,LVS_OWNERDRAWFIXED);
		SetFont(pFont,false);
	}

	Invalidate(FALSE);
}

void CExtListCtrl::SetColFont(int nSubItem, CFont *NewFont)
{
	CFont *pFont = GetFont();

	LOGFONT pLogListFont;
	pFont->GetLogFont(&pLogListFont);

	LOGFONT pLogNewFont;
	NewFont->GetLogFont(&pLogNewFont);

	if (pLogNewFont.lfHeight > pLogListFont.lfHeight)
		FromHandle(m_hWnd)->ModifyStyle(LVS_OWNERDRAWFIXED,NULL);
	
	int element = FindColumn(nSubItem);
	if (element != -1)
		(colslist.begin() + element)->Font = NewFont;
	else
	{
		ItemProps.nSubItem = nSubItem;
		ItemProps.BkColor = GetBkColor();
		ItemProps.FontType = FW_THIN;
		ItemProps.TextColor = GetTextColor();
		ItemProps.Font = NewFont;
		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.style = nStyle;
		ItemProps.m_pf = NULL;

		colslist.insert(colslist.begin(),ItemProps);
	}

	if (pLogNewFont.lfHeight > pLogListFont.lfHeight)
	{
		SetFont(NewFont,false);
		FromHandle(m_hWnd)->ModifyStyle(NULL,LVS_OWNERDRAWFIXED);
		SetFont(pFont,false);
	}
	
	Invalidate(FALSE);
}

int CExtListCtrl::FindElement(int nItem, int nSubItem)
{
	int i = 0;
	int element = -1;

	if (itemslist.capacity()!=0)
	{
		while(itemslist.begin() + i != itemslist.end())
		{
			if ((itemslist.begin()+i)->nItem == nItem &&
				(itemslist.begin()+i)->nSubItem == nSubItem)
			{
				element = i;
			}
			i++;
		}
	}
	return element;
}

int CExtListCtrl::FindRow(int nItem)
{
	int i = 0;
	int element = -1;
	
	if (rowslist.capacity()!=0)
	{
		while(rowslist.begin() + i != rowslist.end())
		{
			if ((rowslist.begin()+i)->nItem == nItem)
			{
				element = i;
			}
			i++;
		}
	}
	return element;
}

int CExtListCtrl::FindColumn(int nSubItem)
{
	int i = 0;
	int element = -1;
	
	if (colslist.capacity()!=0)
	{
		while(colslist.begin() + i != colslist.end())
		{
			if ((colslist.begin()+i)->nSubItem == nSubItem)
			{
				element = i;
			}
			i++;
		}
	}
	return element;
}

void CExtListCtrl::SetColTextAlign(int nSubItem, UINT nFormat)
{
	int element = FindColumn(nSubItem);
	if (element != -1)
		(colslist.begin() + element)->alignment = nFormat | DT_SINGLELINE | DT_BOTTOM;
	else
	{
		ItemProps.nSubItem = nSubItem;
		ItemProps.BkColor = GetBkColor();
		ItemProps.FontType = FW_THIN;
		ItemProps.TextColor = GetTextColor();
		ItemProps.Font = GetFont();
		ItemProps.alignment = nFormat | DT_SINGLELINE | DT_VCENTER;
		ItemProps.m_pf = NULL;

		colslist.insert(colslist.begin(),ItemProps);
	}
	
	Invalidate();
}

void CExtListCtrl::SetItemTextAlign(int nItem, int nSubItem, UINT nFormat)
{
	int element = FindElement(nItem,nSubItem);
	if (element != -1)
		(itemslist.begin() + element)->alignment = nFormat;
	else
	{
		ItemProps.nItem = nItem;
		ItemProps.nSubItem = nSubItem;
		
		int element0 = FindRow(nItem);
		if (element0 != -1)
		{
			ItemProps.BkColor = (rowslist.begin() + element0)->BkColor;
			ItemProps.TextColor = (rowslist.begin() + element0)->TextColor;
			ItemProps.Font = (rowslist.begin() + element0)->Font;
		}
		else
		{
			int element0 = FindColumn(nSubItem);
			if (element0 != -1)
			{
				ItemProps.BkColor = (colslist.begin() + element0)->BkColor;
				ItemProps.TextColor = (colslist.begin() + element0)->TextColor;
				ItemProps.Font = (colslist.begin() + element0)->Font;
			}
			else
			{
				ItemProps.BkColor = GetBkColor();
				ItemProps.TextColor = GetTextColor();
				ItemProps.Font = GetFont();
			}
		}
		
		ItemProps.FontType = FW_THIN;
		ItemProps.alignment = nFormat;
		ItemProps.m_pf = NULL;

		itemslist.insert(itemslist.begin(),ItemProps);
	}
	
	Invalidate();
}

void CExtListCtrl::SetListFont(CFont *pFont, bool bRedraw)
{
	FromHandle(m_hWnd)->ModifyStyle(LVS_OWNERDRAWFIXED,NULL);

	SetFont(pFont,false);
	int i = 0;
	if (itemslist.capacity() != 0)
	{
		while(itemslist.begin() + i != itemslist.end())
		{
			(itemslist.begin()+i)->Font = pFont;
			i++;
		}
	}
	
	FromHandle(m_hWnd)->ModifyStyle(NULL,LVS_OWNERDRAWFIXED);

	if (bRedraw)
		Invalidate(FALSE);
}

COLORREF CExtListCtrl::GetItemTextColor(int nItem, int nSubItem)
{
	COLORREF Color = GetTextColor();
	
	int element = FindElement(nItem,nSubItem);
	if (element != -1)
		Color = (itemslist.begin() + element)->TextColor;
	else
	{
		element = FindRow(nItem);
		if (element != -1)
		{
			Color = (rowslist.begin() + element)->TextColor;
		}
		else
		{
			element = FindColumn(nSubItem);
			if (element != -1)
			{
				Color = (colslist.begin() + element)->TextColor;
			}
		}
	}

	return Color;
}

COLORREF CExtListCtrl::GetItemBkColor(int nItem, int nSubItem)
{
	COLORREF Color = GetBkColor();

	int element = FindElement(nItem,nSubItem);
	if (element != -1)
		Color = (itemslist.begin() + element)->BkColor;
	else
	{
		element = FindRow(nItem);
		if (element != -1)
		{
			Color = (rowslist.begin() + element)->BkColor;
		}
		else
		{
			element = FindColumn(nSubItem);
			if (element != -1)
			{
				Color = (colslist.begin() + element)->BkColor;
			}
		}
	}
	return Color;
}

CFont* CExtListCtrl::GetItemFont(int nItem, int nSubItem)
{
	CFont *pFont = GetFont();
	int element = FindElement(nItem,nSubItem);
	if (element != -1) pFont = (itemslist.begin() + element)->Font;
	else {
		element = FindRow(nItem);
		if (element != -1) pFont = (rowslist.begin() + element)->Font;
		else {
			element = FindColumn(nSubItem);
			if (element != -1) pFont = (colslist.begin() + element)->Font;
		}
	}

	return pFont;
}

UINT CExtListCtrl::GetItemTextAlignment(int nItem, int nSubItem)
{
	UINT ALIGNMENT = DT_SINGLELINE | DT_VCENTER;

	int element = FindElement(nItem,nSubItem);
	if (element != -1) ALIGNMENT = (itemslist.begin() + element)->alignment;
	else {
		element = FindRow(nItem);
		if (element != -1) ALIGNMENT = (rowslist.begin() + element)->alignment;
		else {
			element = FindColumn(nSubItem);
			if (element != -1) ALIGNMENT = (colslist.begin() + element)->alignment;
		}
	}

	return ALIGNMENT;
}

void CExtListCtrl::SetItemStyle(int nItem, int nSubItem, UINT style)
{
	int element = FindElement(nItem,nSubItem);
	if (element != -1)
		(itemslist.begin() + element)->style = style;
	else
	{
		ItemProps.nItem = nItem;
		ItemProps.nSubItem = nSubItem;
		
		int element0 = FindRow(nItem);
		if (element0 != -1)
		{
			ItemProps.BkColor = (rowslist.begin() + element0)->BkColor;
			ItemProps.TextColor = (rowslist.begin() + element0)->TextColor;
			ItemProps.Font = (rowslist.begin() + element0)->Font;
			ItemProps.alignment = (rowslist.begin() + element0)->alignment;
		}
		else
		{
			int element0 = FindColumn(nSubItem);
			if (element0 != -1)
			{
				ItemProps.BkColor = (colslist.begin() + element0)->BkColor;
				ItemProps.TextColor = (colslist.begin() + element0)->TextColor;
				ItemProps.Font = (colslist.begin() + element0)->Font;
				ItemProps.alignment = (colslist.begin() + element0)->alignment;
			}
			else
			{
				ItemProps.BkColor = GetBkColor();
				ItemProps.TextColor = GetTextColor();
				ItemProps.Font = GetFont();
				ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
			}
		}
		
		ItemProps.FontType = FW_THIN;
		ItemProps.style = style;
		ItemProps.m_pf = NULL;

		itemslist.insert(itemslist.begin(),ItemProps);
	}
}

void CExtListCtrl::SetRowStyle(int nItem, UINT style)
{
	int element0 = FindRow(nItem);
	if (element0 != -1)
		(rowslist.begin() + element0)->style = style;
	else
	{
		ItemProps.nItem = nItem;

		ItemProps.BkColor = GetBkColor();
		ItemProps.TextColor = GetTextColor();
		ItemProps.Font = GetFont();
		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.FontType = FW_THIN;
		ItemProps.style = style;
		ItemProps.m_pf = NULL;

		rowslist.insert(rowslist.begin(),ItemProps);
	}
}

void CExtListCtrl::SetColStyle(int nSubItem, UINT style)
{
	int element0 = FindColumn(nSubItem);
	if (element0 != -1)
		(colslist.begin() + element0)->style = style;
	else
	{
		ItemProps.nSubItem = nSubItem;

		ItemProps.BkColor = GetBkColor();
		ItemProps.TextColor = GetTextColor();
		ItemProps.Font = GetFont();
		ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;
		ItemProps.FontType = FW_THIN;
		ItemProps.style = style;
		ItemProps.m_pf = NULL;

		colslist.insert(colslist.begin(),ItemProps);
	}
}

void CExtListCtrl::SetListStyle(UINT style, bool bChangeAllItems)
{
	nStyle = style;
	if (bChangeAllItems == true) {
		int i = 0;
		if (itemslist.capacity() != 0) {
			while(itemslist.begin() + i != itemslist.end()) {
				(itemslist.begin()+i)->style = style;
				i++;
			}
		}

		i = 0;
		
		if (rowslist.capacity() !=0 ) {
			while(rowslist.begin() + i != rowslist.end()) {
				(rowslist.begin()+i)->style = style;
				i++;
			}
		}

		i = 0;

		if (colslist.capacity() !=0 ) {
			while(colslist.begin() + i != colslist.end()) {
				(colslist.begin()+i)->style = style;
				i++;
			}
		}
	}
}

UINT CExtListCtrl::GetItemStyle(int nItem, int nSubItem)
{
	UINT style = nStyle;
	int element = FindElement(nItem,nSubItem);

	if (element != -1)
		style = (itemslist.begin() + element)->style;
	else {
		element = FindRow(nItem);
		if (element != -1)
			style = (rowslist.begin() + element)->style;
		else {
			element = FindColumn(nSubItem);
			if (element != -1)
				style = (colslist.begin() + element)->style;
		}
	}

	return style;
}

void CExtListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_rb_click_msg != 0 && m_messages_wnd != NULL)
	{
		LVHITTESTINFO pInfo;
		pInfo.pt = point;
		int nItem = SubItemHitTest(&pInfo);
		int nSubItem = pInfo.iSubItem;	
		m_messages_wnd->SendMessage(m_rb_click_msg, (nItem << 8) | (nSubItem & 0xFF), (point.x << 16) | (point.y & 0xFFFF) );
	}
}

void CExtListCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	DWORD msg = (m_rb_dbl_click_msg != 0) ? m_rb_dbl_click_msg : m_rb_click_msg;
	if (msg != 0 && m_messages_wnd != NULL)
	{
		LVHITTESTINFO pInfo;
		pInfo.pt = point;
		int nItem = SubItemHitTest(&pInfo);
		int nSubItem = pInfo.iSubItem;	
		m_messages_wnd->SendMessage(msg, (nItem << 8) | (nSubItem & 0xFF), (point.x << 16) | (point.y & 0xFFFF) );
	}
}
void CExtListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	LVHITTESTINFO pInfo;
	pInfo.pt = point;
	int nItem = SubItemHitTest(&pInfo);
	int nSubItem = pInfo.iSubItem;

	if (nItem >= 0 && nSubItem >= 0)
	{
		OldnItem = curItem;
		OldnSubItem = curSubItem;
		UINT style = GetItemStyle(nItem, nSubItem);	
		if (style == 0)
			CListCtrl::OnLButtonDown(nFlags, point);
		if (nItem == curItem && nSubItem == curSubItem)
		{	
			if (style == 1 && EditItem == NULL)
			{
				bIsItemChanging = true;
				CreateEdit(nItem, nSubItem);
			}
		}
		else
		{	
			if (EditItem != NULL)
			{
				OldStr = "";
				SetFocus();
				bIsItemChanging = false;
			}
			curItem = nItem;
			curSubItem = nSubItem;
			if ((CWnd *)this == GetFocus())
				ScrollCaret(nItem,nSubItem);
			else
				SetFocus();
		}
		curItem = nItem;
		curSubItem = nSubItem;
	}
	if (m_lb_click_msg && m_messages_wnd)
		m_messages_wnd->SendMessage(m_lb_click_msg, (nItem << 8) | (nSubItem & 0xFF), (point.x << 16) | (point.y & 0xFFFF) );
}

void CExtListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SetFocus();
	bIsItemChanging = false;
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	Invalidate(FALSE);
}

void CExtListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SetFocus();
	bIsItemChanging = false;
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
	Invalidate(FALSE);
}

BOOL CExtListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{	
	SetFocus();
	bIsItemChanging = false;
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CExtListCtrl::ChangeSelItemText()
{
	CString str;
	if (OldStr != "")
		str = OldStr;
	else
		EditItem->GetWindowText(str);
	SetItemText(OldnItem,OldnSubItem,str);
}

void CExtListCtrl::CreateEdit(int nItem, int nSubItem)
{
	GetSubItemRect(nItem,nSubItem,LVIR_LABEL,itemRect);

	itemRect.bottom -= 1;
	if (nSubItem > 0)
		itemRect.left += 1;

	EditItem = new CExtEdit(this,nItem,nSubItem,GetItemCount(),GetSubItemCount());

	EditItem->Create(WS_VISIBLE | ES_AUTOHSCROLL, itemRect, this, IDC_EDITITEM);
	EditItem->SetWindowPos(&wndTop, itemRect.left, itemRect.top, itemRect.Width(), itemRect.Height(), SWP_SHOWWINDOW);
	EditItem->SetFont(GetItemFont(nItem,nSubItem));	
	OldStr = GetItemText(nItem,nSubItem);
	EditItem->SetWindowText(OldStr);
	EditItem->SetActiveWindow();
	EditItem->SetFocus();
}

void CExtListCtrl::CreateComboBox(int nItem, int nSubItem)
{
		CBoxItem = new CComboBox();
		CBoxItem->Create(WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_VSCROLL,itemRect,this,IDC_CBOXITEM);
		CBoxItem->SetWindowPos(&wndTop, itemRect.left, itemRect.top, itemRect.Width(), itemRect.Height(), SWP_SHOWWINDOW);
		LOGFONT pLogFont;
		GetFont()->GetLogFont(&pLogFont);
		pLogFont.lfHeight -= 8;
		if (ItemFont != NULL)
			delete ItemFont;
		ItemFont = new CFont;
		if(ItemFont->m_hObject) ItemFont->DeleteObject();
		ItemFont->CreateFontIndirect(&pLogFont);
		CBoxItem->SetFont(ItemFont);
		CBoxItem->AddString(GetItemText(nItem,nSubItem));
		CBoxItem->AddString("str_1");
		CBoxItem->AddString("str_2");
		CBoxItem->AddString("str_3");
		CBoxItem->AddString("str_4");
		CBoxItem->ShowDropDown();
}

int CExtListCtrl::GetSubItemCount()
{
	CHeaderCtrl *hdrCtrl = GetHeaderCtrl();
	if (hdrCtrl != NULL) return hdrCtrl->GetItemCount();
	else return -1;
}

void CExtListCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CListCtrl::OnSetFocus(pOldWnd);
	
	bIsListFocused = true;
		
	if (EditItem != NULL) 
	{
		ChangeSelItemText();
		delete EditItem;
		EditItem = NULL;
	}

	//uic tmp ScrollCaret(curItem,curSubItem);
}

BOOL CExtListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYUP)
	{
		int nChar = (int)pMsg->wParam;
		if (nChar == VK_CONTROL || nChar == VK_SHIFT)
			nCharBuf = -1;
	}

	if (pMsg->message == WM_KEYDOWN)
	{
		int nChar = (int)pMsg->wParam;
		if (nChar == VK_CONTROL || nChar == VK_SHIFT)
			nCharBuf = nChar;

		OldnItem = curItem;
		OldnSubItem = curSubItem;

		int ItemCount, SubItemCount;
		bool unhandled_key = false;
		switch (nChar)
		{
			case VK_HOME:	if (EditItem == NULL && m_allow_move_selection_by_keys)
							{
								if (nCharBuf == VK_CONTROL)
									curItem = 0;
								else
									curSubItem = 0;
								if (m_key_down_msg != 0 && m_messages_wnd != NULL)
									m_messages_wnd->SendMessage(m_key_down_msg, nChar, 0);									
								ScrollCaret(curItem,curSubItem);
								return true;
							}
							break;
			case VK_END:	if (EditItem == NULL && m_allow_move_selection_by_keys)
							{
								if (nCharBuf == VK_CONTROL)
									curItem = GetItemCount() - 1;
								else
									curSubItem = GetSubItemCount() - 1;
								if (m_key_down_msg != 0 && m_messages_wnd != NULL)
									m_messages_wnd->SendMessage(m_key_down_msg, nChar, 0);									
								ScrollCaret(curItem,curSubItem);
								return true;
							}
							break;
			case /*VK_PAGE_UP*/33:
							if (EditItem == NULL && m_allow_move_selection_by_keys)
							{
								curItem -= GetCountPerPage();
								if (curItem < 0)
									curItem = 0;
								if (m_key_down_msg != 0 && m_messages_wnd != NULL)
									m_messages_wnd->SendMessage(m_key_down_msg, nChar, 0);									
								ScrollCaret(curItem,curSubItem);
								return true;
							}
							break;
			case /*VK_PAGE_DOWN*/34:
							if (EditItem == NULL && m_allow_move_selection_by_keys)
							{
								curItem += GetCountPerPage();
								ItemCount = GetItemCount() - 1;
								if (curItem > ItemCount)
									curItem = ItemCount;
								if (m_key_down_msg != 0 && m_messages_wnd != NULL)
									m_messages_wnd->SendMessage(m_key_down_msg, nChar, 0);									
								ScrollCaret(curItem,curSubItem);
								return true;
							}
							break;
			case VK_UP:		if (EditItem == NULL && m_allow_move_selection_by_keys)
							{
								curItem--;
								if (curItem < 0)
									curItem = 0;
								if (m_key_down_msg != 0 && m_messages_wnd != NULL)
									m_messages_wnd->SendMessage(m_key_down_msg, nChar, 0);									
								ScrollCaret(curItem,curSubItem);
								return true;
							}
							break;
			case VK_DOWN:	if (EditItem == NULL && m_allow_move_selection_by_keys)
							{
								curItem++;
								ItemCount = GetItemCount() - 1;
								if (curItem > ItemCount)
									curItem = ItemCount;
								if (m_key_down_msg != 0 && m_messages_wnd != NULL)
									m_messages_wnd->SendMessage(m_key_down_msg, nChar, 0);									
								ScrollCaret(curItem,curSubItem);
								return true;
							}
							break;
			case VK_LEFT:	if (EditItem == NULL && m_allow_move_selection_by_keys)
							{
								curSubItem--;
								if (curSubItem < 0)
									curSubItem = 0;
								if (m_key_down_msg != 0 && m_messages_wnd != NULL)
									m_messages_wnd->SendMessage(m_key_down_msg, nChar, 0);									
								ScrollCaret(curItem,curSubItem);
								return true;
							}
							break;
			case VK_RIGHT:	if (EditItem == NULL && m_allow_move_selection_by_keys)
							{
								curSubItem++;
								SubItemCount = GetSubItemCount() - 1;
								if (curSubItem > SubItemCount)
									curSubItem = SubItemCount;
								if (m_key_down_msg != 0 && m_messages_wnd != NULL)
									m_messages_wnd->SendMessage(m_key_down_msg, nChar, 0);									
								ScrollCaret(curItem,curSubItem);
								return true;
							}
							break;
			case VK_RETURN:	OldStr = "";
							ScrollCaret(curItem,curSubItem);
							SelectCtrl(GetItemStyle(curItem,curSubItem));
							bIsItemChanging = !bIsItemChanging;
							break;
			case VK_ESCAPE:	SetFocus();
							bIsItemChanging = false;
							break;
			case VK_TAB:	OldStr = "";
							if (EditItem != NULL)
								bIsItemChanging = !bIsItemChanging;
							break;
			default:		unhandled_key = true;
							if (EditItem != NULL)
								EditItem->Invalidate();
							break;
		}
		if (m_key_down_msg != 0 && m_messages_wnd != NULL)
			m_messages_wnd->SendMessage(m_key_down_msg, nChar, 0);
		if (EditItem == NULL && !unhandled_key)
			return true;
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}

void CExtListCtrl::ScrollCaret(int nItem, int nSubItem)
{
	DWORD list_style = GetExtendedStyle();

	GetSubItemRect(nItem,nSubItem,LVIR_LABEL,itemRect);

	if (IsRectEmpty(&itemRect)) return;

	itemRect.bottom -= 1;
	if (nSubItem > 0)
		itemRect.left += 1;

	CRect rect = itemRect;
	ClientToScreen(&rect);
	CRect wndRect;
	GetWindowRect(&wndRect);

	CHeaderCtrl *hdrCtrl = GetHeaderCtrl();

	CRect hdrRect;
	hdrCtrl->GetItemRect(0,&hdrRect);

	wndRect.top += hdrRect.Height();

	SCROLLINFO ScrlInf;
	GetScrollInfo(SB_HORZ,&ScrlInf,SIF_ALL);
	
	int hs_height = GetSystemMetrics(SM_CYHSCROLL);

	if ((int)ScrlInf.nPage < ScrlInf.nMax)
		wndRect.bottom -= hs_height;

	GetScrollInfo(SB_VERT,&ScrlInf,SIF_ALL);
	
	int vs_width = GetSystemMetrics(SM_CXVSCROLL);

	if ((int)ScrlInf.nPage < ScrlInf.nMax)
		wndRect.right -= vs_width;

	int vScrl = 0;
	int hScrl = 0;
	bool b_vScrolled = false;
	bool b_hScrolled = false;

	int vDdown = rect.bottom - wndRect.bottom;
	int vDup = wndRect.top - rect.top;

	if (vDdown > itemRect.Height())
	{
		vScrl = (int(vDdown / itemRect.Height()) + 1) * itemRect.Height();
		b_vScrolled = true;
	}

	if (vDdown <= itemRect.Height() && vDdown > 0)
	{
		vScrl = itemRect.Height() + 1;
		b_vScrolled = true;
	}

	if (vDup > itemRect.Height())
	{
		int HH = itemRect.Height();
		float num = float(vDup) / float(HH);
		if (num - int(num) > 0.5 && num - int(num) <= 1)
			num = (float)(int(num) + 1);
		vScrl = (int)(-num * itemRect.Height());
		b_vScrolled = true;
	}

	if (vDup <= itemRect.Height() && vDup > 0)
	{
		vScrl = -(itemRect.Height() + 1);
		b_vScrolled = true;
	}

	int hDright = rect.right - wndRect.right;
	int hDleft =  wndRect.left - rect.left;

	CRect OldItemRect;
	if (list_style | LVS_EX_FULLROWSELECT)
		GetItemRect(OldnItem,OldItemRect,LVIR_BOUNDS);
	else
		GetSubItemRect(OldnItem,OldnSubItem,LVIR_LABEL,OldItemRect);

	if ( ((list_style | LVS_EX_FULLROWSELECT) && nItem != OldnItem) || nItem != OldnItem || nSubItem != OldnSubItem)
		InvalidateRect(&OldItemRect);
	
	if (hDright > itemRect.Width())
	{
		for (int i = 0; i < nSubItem; i++)
			hScrl += GetColumnWidth(i);
		b_hScrolled = true;
	}

	int si = GetSubItemCountPerPage();

	int pfit = 3;					//pixels fit added by rvk 030707

	if (hDright <= OldItemRect.Width() && hDright > 0)
	{
		hScrl = hDright + 2;
		if (itemRect.Width() > wndRect.Width())
			hScrl = -(hDleft + pfit);
		b_hScrolled = true;
	}

	if (OldItemRect.Width() <= itemRect.Width() && hDright > 0)
	{
		hScrl = hDright + 2;
		if (itemRect.Width() > wndRect.Width())
			hScrl = -(hDleft + pfit);
		b_hScrolled = true;
	}

	if (hDleft > itemRect.Width())
	{
		hScrl = -(int(hDleft / itemRect.Width()) + 1) * itemRect.Width();
		b_hScrolled = true;
	}

	if (hDleft <= itemRect.Width() && hDleft > 0)
	{
		if (nSubItem == 0)
			pfit = 4;
		hScrl = -(hDleft + pfit);
		b_hScrolled = true;
	}

	if (b_vScrolled)
	{
		itemRect.top -= vScrl;
		itemRect.bottom -= vScrl;
	}

	if (b_hScrolled)
	{
		itemRect.left -= hScrl;
		itemRect.right -= hScrl;
	}

	if (b_vScrolled || b_hScrolled)
	{
		CSize size;
		size.cx = hScrl;
		size.cy = vScrl;
		Scroll(size);
		Update(nItem);
	}

	if (nItem != OldnItem || nSubItem != OldnSubItem ||
		bIsListFocused == true || (list_style | LVS_SHOWSELALWAYS) ||
		((list_style | LVS_EX_FULLROWSELECT) && nItem != OldnItem)
		)
	{
		if (list_style | LVS_EX_FULLROWSELECT)
			GetItemRect(nItem,itemRect,LVIR_BOUNDS);
		InvalidateRect(&itemRect);
		bIsListFocused = false;
	}

	if (m_selection_changed_msg > 0)
		::PostMessage(GetParent()->m_hWnd, m_selection_changed_msg, curItem, curSubItem);
}

void CExtListCtrl::DrawCaret(CDC *pDC, int nItem, int nSubItem)
{
	CString str = GetItemText(nItem, nSubItem);

	CRect tRect;
	GetSubItemRect(nItem,nSubItem,LVIR_BOUNDS,tRect);

	bool focused = (GetFocus() == this);
	pDC->FillSolidRect(&tRect, focused?nCaretColor:nCaretColorInactive);
	tRect.left += 3;
	CFont *pOldItemFont = pDC->SelectObject(GetItemFont(nItem, nSubItem));
	ASSERT(pOldItemFont);

	DWORD text_col = GetItemTextColor(nItem, nSubItem);
	if (focused && (text_col & 0xFFFFFF) == 0x000000)
		text_col = nCaretTextColor;
	pDC->SetTextColor(text_col);
	pDC->DrawText(str,&tRect,DT_SINGLELINE | DT_VCENTER);
	pDC->SelectObject(pOldItemFont);
}

void CExtListCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);
	
	bIsListFocused = false;
	CRect rect;
	GetSubItemRect(curItem,curSubItem,LVIR_LABEL,rect);
	InvalidateRect(&rect);
}

void CExtListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    DWORD msg = (m_lb_dbl_click_msg != 0)?m_lb_dbl_click_msg:m_lb_click_msg;

	LVHITTESTINFO pInfo;

	pInfo.pt = point;

	int nItem = SubItemHitTest(&pInfo);
	int nSubItem = pInfo.iSubItem;

	if (nItem >= 0 && nSubItem >= 0)
	{

		OldnItem = curItem;
		OldnSubItem = curSubItem;

		if (this != CWnd::GetFocus())
		{
			curItem = nItem;
			curSubItem = nSubItem;
			SetFocus();
		}
		
        if (!msg) {
		    UINT style = GetItemStyle(nItem,nSubItem);
		    switch(style)
		    {
			    case 0:
					    CListCtrl::OnLButtonDblClk(nFlags, point);
					    break;
			    case 1:
					    if (EditItem != NULL)
						    SetFocus();
					    bIsItemChanging = !bIsItemChanging;
					    CreateEdit(nItem,nSubItem);
					    break;
		    }
        }

		curItem = nItem;
		curSubItem = nSubItem;
	}

	if (msg && m_messages_wnd)
		m_messages_wnd->SendMessage(msg, (nItem << 8) | (nSubItem & 0xFF), (point.x << 16) | (point.y & 0xFFFF) );
}

void CExtListCtrl::SelectCtrl(UINT style)
{
	switch(style)
	{
		case 0:	break;
		case 1:	if (EditItem != NULL)
					SetFocus();
				else
					CreateEdit(curItem,curSubItem);
				break;
	}
}

int CExtListCtrl::GetSubItemCountPerPage()
{
	int i = 0;
	int WidthString = 0;
	
	CRect wndRect;
	GetWindowRect(&wndRect);

	CRect rect;
	bool bPassLeft = false;
	bool bPassRight = false;
	int SUBITEMCOUNT = GetSubItemCount();

	if (SUBITEMCOUNT > 0)
	{
		for(i = 0; i < SUBITEMCOUNT; i++)
		{
			GetSubItemRect(curItem, i, LVIR_LABEL, rect);
			ClientToScreen(&rect);
			
			WidthString += rect.Width();

			if (WidthString > wndRect.Width())
			{
				if (WidthString == wndRect.Width()) i++; break;
				break;
			}

			if (rect.left < wndRect.left)
			{
				WidthString -= rect.Width();
				bPassLeft = true;
			}
			
			if (rect.right > wndRect.right)
			{
				WidthString -= rect.Width();
				bPassRight = true;
			}

			if (bPassRight == true && bPassLeft == true)
			{
				i--;
				break;
			}
		}
	}

	if (i > 0 && i <= GetSubItemCount())
		return i;
	else
		return -1;
}

CFont* CExtListCtrl::SetHeaderFont(CFont *font)
{
	CHeaderCtrl *hdrCtrl = GetHeaderCtrl();
	CFont *oldFont;
	if (hdrCtrl != NULL)
	{
		oldFont = hdrCtrl->GetFont();
		hdrCtrl->SetFont(font);
		return oldFont;
	}
	else
		return NULL;
}

void CExtListCtrl::SetCursorInvisible(BOOL bInvisible)
{
	_bInvisible = bInvisible;
}

BOOL CExtListCtrl::EnableHeaders(BOOL bEnable)
{
	CHeaderCtrl *hdrCtrl = GetHeaderCtrl();
	if (hdrCtrl == NULL) return FALSE;
	return hdrCtrl->EnableWindow(bEnable);
}

void CExtListCtrl::SetDefaultElement(int nItem, int nSubItem)
{
	ItemProps.nItem = nItem;
	ItemProps.nSubItem = nSubItem;
	ItemProps.m_pf = NULL;
	ItemProps.alignment = DT_SINGLELINE | DT_VCENTER;

	int element0 = FindRow(nItem);
	if (element0 != -1)
	{
		ItemProps.BkColor = (rowslist.begin() + element0)->BkColor;
		ItemProps.Font = (rowslist.begin() + element0)->Font;
		ItemProps.alignment = (rowslist.begin() + element0)->alignment;
	}
	else
	{
		int element0 = FindColumn(nSubItem);
		if (element0 != -1)
		{
			ItemProps.BkColor = (colslist.begin() + element0)->BkColor;
			ItemProps.Font = (colslist.begin() + element0)->Font;
			ItemProps.alignment = (colslist.begin() + element0)->alignment;
		}
		else
		{
			ItemProps.BkColor = GetBkColor();
			ItemProps.Font = GetFont();
			ItemProps.style = nStyle;
		}
	}
	itemslist.insert(itemslist.begin(), ItemProps);	
}

TOOLINFO g_ti;

void CExtListCtrl::SetToolTip(CString tip_str, int nItem, int nSubItem)
{
	int element = FindElement(nItem, nSubItem);
	if (element == -1)
	{
		SetDefaultElement(nItem, nSubItem);
		element = FindElement(nItem, nSubItem);
		if (element == -1)
			return;
	}
	(itemslist.begin() + element)->m_tip = tip_str;

	{
		g_ti.cbSize = sizeof(TOOLINFO);
		g_ti.uFlags = TTF_IDISHWND;
		g_ti.hwnd = GetParent()->m_hWnd;
		g_ti.uId = (UINT)m_hWnd;
		g_ti.rect = CRect(0,0,0,0);
		g_ti.hinst = 0;
		g_ti.lpszText = NULL;
		g_ti.lParam = 0;

		EnableToolTips(TRUE);
	}
}

void CExtListCtrl::SetRedrawFunction(ExListRedrawItemFunction f, int nItem, int nSubItem)
{
	int element = FindElement(nItem, nSubItem);
	if (element == -1)
	{
		SetDefaultElement(nItem, nSubItem);
		element = FindElement(nItem, nSubItem);
		if (element == -1)
			return;
	}
	(itemslist.begin() + element)->m_pf = f;
	Invalidate();
}

int CExtListCtrl::OnToolHitTest(CPoint point, TOOLINFO* ti) const
{
	g_ti.cbSize = sizeof(TOOLINFO);
	g_ti.uFlags = TTF_IDISHWND;
	g_ti.hwnd = GetParent()->m_hWnd;
	g_ti.uId = (UINT)m_hWnd;
	//g_ti.rect = CRect(0,0,0,0);
	g_ti.hinst = 0;
	g_ti.lpszText = NULL;
	//g_ti.lParam = 0;

	*ti = g_ti;
	return 1;
}

void CExtListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	LVHITTESTINFO pInfo;
	pInfo.pt = point;
	int new_item = SubItemHitTest(&pInfo);
	if (new_item != -1)
	{
		SetCapture();
		m_mouse_in_item = new_item;
		m_mouse_in_subitem  = pInfo.iSubItem;
		OnMouseMoveIn(point);
        AFX_MODULE_THREAD_STATE* pThreadState = AfxGetModuleThreadState();
        CToolTipCtrl* pToolTip = pThreadState->m_pToolTip;
		if (pToolTip != NULL)
		{
			int element = FindElement(m_mouse_in_item, m_mouse_in_subitem);
			if (element != -1)
			{
				g_ti.lpszText = (itemslist.begin() + element)->m_tip.GetBuffer(0);
				pToolTip->SetToolInfo(&g_ti);

				pToolTip->Update();
			}
			else
				pToolTip->Pop();
		}
	}
	else
	{
		ReleaseCapture();
		m_mouse_in_item = -1;
		m_mouse_in_subitem = -1;
	}
	CListCtrl::OnMouseMove(nFlags, point);
}