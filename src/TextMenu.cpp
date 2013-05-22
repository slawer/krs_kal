#include <StdAfx.h>
#include "TextMenu.h"
#ifdef _MNP
	#include "CLR.h"
#else
	#define CLR_GetSysColor(x) GetSysColor(x)
#endif

CFont* TextMenu::m_default_font = NULL;
TextMenu::TextMenu(int max_h_count, int increased_w, TextMenu_Alignment text_alignment, int text_offset):
	PUMENU_Menu(0, 0, 10, 10, 0x000000),
	m_max_h_count(max_h_count), m_increased_w(increased_w), m_text_alignment(text_alignment), m_text_offset(text_offset),
	m_index_offset(0), m_timer1_set(false),
	m_on_select_wnd(NULL), m_on_select_msg(0)
{
	SetWindowName("TextMenu");
	if (m_default_font == NULL)
	{
		LOGFONT lf;
		lf.lfHeight = 120;
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfWeight = FW_BOLD;
		lf.lfItalic = FALSE;
		lf.lfUnderline = FALSE;
		lf.lfStrikeOut = FALSE;
		lf.lfCharSet = RUSSIAN_CHARSET;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = ANTIALIASED_QUALITY;
		lf.lfPitchAndFamily = FF_DONTCARE;
		sprintf(lf.lfFaceName, "MS Shell Dlg");
		m_font = new CFont();
		m_font->CreatePointFontIndirect(&lf);
		m_internal_font = true;
	}
	else
	{
		m_font = m_default_font;
		m_internal_font = false;
	}
}

TextMenu::~TextMenu()
{
	Clear();
	if (m_font && m_internal_font)
	{
		m_font->DeleteObject();
		delete m_font;
	}
}

void TextMenu::OnKey()
{
	if (m_key_pressed.find(27) != m_key_pressed.end())
	{
		m_selected_cell_x_pos = m_selected_cell_y_pos = -1;
		GetWnd()->PostMessage(WM_LBUTTONUP, 0, 0);
		GetWnd()->PostMessage(WM_RBUTTONUP, 0, 0);
	}
}

void TextMenu::BeforeShow()
{
	m_timer1_set = false;

	SetMessageHandler(WM_KEYDOWN, (InterfaceObject_OwnFunction)&TextMenu::OnKey);

	CClientDC dc(GetWnd());
	PrepareDC(&dc);
	CSize sz;
	m_cell_w = 0;
	int i, i_max = m_elements.size();
	for (i = 0; i < i_max; i++)
	{
		sz = dc.GetTextExtent(m_elements[i]->GetStr());
		if (sz.cx > m_cell_w)
			m_cell_w = sz.cx;
	}
	m_cell_w += 2*GetBorderWidth() + m_increased_w;
	m_cell_h = sz.cy + 2;

	int limit_h_count = (m_desktop_h - 2 * GetBorderWidth())/m_cell_h;
	int limit_w_count = (m_desktop_w - 2 * GetBorderWidth())/m_cell_w;
	int size = m_elements.size();
	if (size > limit_h_count * limit_w_count)
	{
		m_index_offset = min(m_index_offset, size - limit_h_count * limit_w_count);
		m_x_cells_num = limit_w_count;
		m_y_cells_num = limit_h_count;
	}
	else
	{
		m_index_offset = 0;

		int max_h = m_max_h_count;
		if (max_h == -1)
			max_h = limit_h_count;
		if (size > max_h * limit_w_count)
		{
			m_x_cells_num = limit_w_count;
			m_y_cells_num = 1 + (size-1) / m_x_cells_num;
		}
		else
		{
			m_y_cells_num = max_h;
			m_x_cells_num = 1 + (size-1) / max(1, m_y_cells_num);
			m_y_cells_num = 1 + (size-1) / max(1, m_x_cells_num);
		}
	}
}

void TextMenu::PrepareDC(CDC* dc)
{
	if (m_font)
		dc->SelectObject(m_font);
	dc->SetBkMode(TRANSPARENT);
}

void TextMenu::DrawCell(CDC* dc, int x_pos, int y_pos, int x, int y)
{
	int index = m_index_offset + x_pos * m_y_cells_num + y_pos;
	int size = m_elements.size();
	bool empty = (index >= size);
	bool selected = !empty && (x_pos == m_selected_cell_x_pos && y_pos == m_selected_cell_y_pos);
	DWORD back_color = (selected)?CLR_GetSysColor(COLOR_HIGHLIGHT):CLR_GetSysColor(COLOR_INFOBK);//0xFFFFFFCLR_GetSysColor(COLOR_MENU);
	DWORD text_color = (selected)?CLR_GetSysColor(COLOR_HIGHLIGHTTEXT):CLR_GetSysColor(COLOR_INFOTEXT);
	dc->FillSolidRect(x, y, m_cell_w, m_cell_h, back_color);
	if (!empty)
	{
		CString str = m_elements[index]->GetStr();
		if (m_index_offset > 0 && x_pos == 0 && y_pos == 0)
			str = "... << ...";
		else
		if (index < size-1 && x_pos == m_x_cells_num-1 && y_pos == m_y_cells_num-1)
			str = "... >> ...";
		CSize sz = dc->GetTextExtent(str);
		dc->SetTextColor(text_color);
		int text_x = x + m_text_offset;
		if (m_text_alignment == TextMenu_Center)
			text_x += (m_cell_w - sz.cx)/2;
		else
		if (m_text_alignment == TextMenu_Right)
			text_x += m_cell_w - sz.cx;
		dc->TextOut(text_x, y + (m_cell_h - sz.cy)/2, str);
	}
}

void TextMenu::OnMouseMove()
{
	if (m_timer1_set || m_selected_cell_x_pos == -1 || m_selected_cell_y_pos == -1)
		return;
		
	int index = m_index_offset + m_selected_cell_x_pos * m_y_cells_num + m_selected_cell_y_pos;
	int size = m_elements.size();
	if (
		(m_selected_cell_x_pos == 0 && m_selected_cell_y_pos == 0 && m_index_offset > 0)
		||
		(m_selected_cell_x_pos == m_x_cells_num-1 && m_selected_cell_y_pos == m_y_cells_num-1 && index < size-1)
		)
	{
		SetTimer(1, 20);
		m_timer1_set = true;
	}
}

void TextMenu::OnClose()
{
	if (m_selected_cell_x_pos == -1 || m_selected_cell_y_pos == -1)
		return;

	int index = m_index_offset + m_selected_cell_x_pos * m_y_cells_num + m_selected_cell_y_pos;
	int size = m_elements.size();

	if (
		index > size - 1
		||
		(m_selected_cell_x_pos == 0 && m_selected_cell_y_pos == 0 && m_index_offset > 0)
		||
		(m_selected_cell_x_pos == m_x_cells_num-1 && m_selected_cell_y_pos == m_y_cells_num-1 && index < size-1)
		)
		return;

	Element* element = m_elements[index];
	OnSelect(index, element->GetData(), element->GetStr());
}

void TextMenu::OnSelect(int pos, DWORD data, CString str)
{
	if (m_on_select_wnd == NULL || !::IsWindow(m_on_select_wnd->m_hWnd) || m_on_select_msg == 0)
		return;
	m_on_select_wnd->PostMessage(m_on_select_msg, pos, data);
}

void TextMenu::OnTimer(UINT timer_id)
{
	if (timer_id == 1)
	{
		if (m_selected_cell_x_pos != -1 && m_selected_cell_y_pos != -1)
		{
			int index = m_index_offset + m_selected_cell_x_pos * m_y_cells_num + m_selected_cell_y_pos;
			int size = m_elements.size();
			int rest = size - m_y_cells_num * m_x_cells_num;
			int add = 1 + rest/30;
			if (m_selected_cell_x_pos == 0 && m_selected_cell_y_pos == 0 && m_index_offset > 0)
			{
				m_index_offset = max(0, m_index_offset - add);
				GetWnd()->Invalidate(FALSE);
				return;
			}
			if (m_selected_cell_x_pos == m_x_cells_num-1 && m_selected_cell_y_pos == m_y_cells_num-1 && index < size-1)
			{
				m_index_offset = min(rest, m_index_offset + add);
				GetWnd()->Invalidate(FALSE);
				return;
			}
		}
		KillTimer(1);
		m_timer1_set = false;
		return;
	}
}

void TextMenu::SetFont(CFont* font)
{
	if (m_font && m_internal_font)
	{
		m_font->DeleteObject();
		delete m_font;
		m_internal_font = false;
	}
	m_font = font;
}

void TextMenu::Add(CString str, DWORD data)
{
	int position = m_elements.size();
	m_elements[position] = new Element(str, data, position);
}

void TextMenu::Clear()
{
	int i, i_max = m_elements.size();
	for (i = 0; i < i_max; i++)
		delete m_elements[i];
	m_elements.clear();
}

int TextMenu::GetCount()
{
	return m_elements.size();
}

////////////////////////////////////////////////////////////////////////////////
// end  