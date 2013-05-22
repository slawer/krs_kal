#include <StdAfx.h>
#include "SB.h"

BEGIN_MESSAGE_MAP(SB_ScrollBar, CStatic)
	//{{AFX_MSG_MAP(SB_ScrollBar)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
SB_ScrollBar::SB_ScrollBar():
	m_downed(-1), m_mem_dc_available(false),
	m_reaction_hwnd(NULL), m_reaction_msg(0)
{
	SetColors(0xEEEEEE, 0xAAAAAA, 0x888888, 0x666666, 0x333333);
}

void SB_ScrollBar::SetColors(DWORD col1, DWORD col2, DWORD col3, DWORD col4, DWORD col5)
{
	m_color[0] = col1;
	m_color[1] = col2;
	m_color[2] = col3;
	m_color[3] = col4;
	m_color[4] = col5;
	if (m_mem_dc_available)
	{
		m_pen.DeleteObject();
		m_brush.DeleteObject();
	}
	m_pen.CreatePen(PS_SOLID, 1, m_color[4]);
	m_brush.CreateSolidBrush(m_color[4]);
	if (m_mem_dc_available)
	{
		m_mem_dc.SelectObject(&m_pen);
		m_mem_dc.SelectObject(&m_brush);
	}
}

bool SB_ScrollBar::Create(bool horizontal, int x, int y, int w, int h, CWnd* parent)
{
	m_w = w;
	m_h = h;
	CRect rect(x, y, x + m_w, y + m_h);
	return CStatic::Create("", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rect, parent, IDC_STATIC) == TRUE;
}
void SB_ScrollBar::OnSetParams(bool redraw)
{
	if (m_visible_elements > m_total_elements)
		m_visible_elements = m_total_elements;
	if (m_first_visible_element + m_visible_elements > m_total_elements)
		m_first_visible_element = m_total_elements - m_visible_elements;
	CalculateRects();
	if (redraw)
		Invalidate(FALSE);
}

void SB_ScrollBar::SetTotalElements(int total, bool redraw)
{
	m_total_elements = max(1, total);
	OnSetParams(redraw);
}

void SB_ScrollBar::SetVisibleElements(int visible, bool redraw)
{
	m_visible_elements = max(1, visible);
	OnSetParams(redraw);
}

void SB_ScrollBar::SetFirstVisibleElement(int first, bool redraw)
{
	m_first_visible_element = max(0, first);
	OnSetParams(redraw);
}
void SB_ScrollBar::DrawArrows(CDC* dc)
{
	int all_pos = GetLength() - 3;
	int sz = GetWidth() - 2;
	int across_sz = sz - 6;
	int pos_sz = across_sz/2;
	CPoint pt1, pt2, pt3;

	int pos = sz/2-pos_sz/2;
	PosToXY(pos, sz/2, pt1.x, pt1.y);
	PosToXY(pos+pos_sz, sz/2-across_sz/2, pt2.x, pt2.y);
	PosToXY(pos+pos_sz, sz/2+across_sz/2, pt3.x, pt3.y);
	dc->BeginPath();
	dc->MoveTo(pt1);
	dc->LineTo(pt2);
	dc->LineTo(pt3);
	dc->LineTo(pt1);
	dc->EndPath();
	dc->StrokeAndFillPath();

	PosToXY(all_pos - (pos), sz/2, pt1.x, pt1.y);
	PosToXY(all_pos - (pos+pos_sz), sz/2-across_sz/2, pt2.x, pt2.y);
	PosToXY(all_pos - (pos+pos_sz), sz/2+across_sz/2, pt3.x, pt3.y);
	dc->BeginPath();
	dc->MoveTo(pt1);
	dc->LineTo(pt2);
	dc->LineTo(pt3);
	dc->LineTo(pt1);
	dc->EndPath();
	dc->StrokeAndFillPath();
}

void SB_ScrollBar::DrawRect(CRect rect, bool downed, CDC*dc)
{
	dc->FillSolidRect(rect, m_color[0]);
	InflateRect(rect, -1, -1);
	dc->FillSolidRect(rect, downed?m_color[3]:m_color[1]);
}

void SB_ScrollBar::OnPaint()
{
	CPaintDC pdc(this);

	CDC* dc = (m_mem_dc_available)?(&m_mem_dc):(&pdc);

	CPen* old_pen;
	CBrush* old_brush;
	if (!m_mem_dc_available)
	{
		old_pen = dc->SelectObject(&m_pen);
		old_brush = dc->SelectObject(&m_brush);
	}

	dc->FillSolidRect(0, 0, m_w, m_h, m_color[3]);

	dc->FillSolidRect(m_lo_pos_rect, (m_downed == 3 && m_lo_pos_rect.PtInRect(m_mouse_pt))?m_color[3]:m_color[2]);
	dc->FillSolidRect(m_hi_pos_rect, (m_downed == 4 && m_hi_pos_rect.PtInRect(m_mouse_pt))?m_color[3]:m_color[2]);

	DrawRect(m_lo_arrow_rect, m_downed == 0, dc);
	DrawRect(m_hi_arrow_rect, m_downed == 1, dc);
	DrawArrows(dc);
	DrawRect(m_pos_rect, m_downed == 2, dc);

	if (m_mem_dc_available)
	{
		pdc.BitBlt(0, 0, m_w, m_h, &m_mem_dc, 0, 0, SRCCOPY);
	}
	else
	{
		dc->SelectObject(old_pen);
		dc->SelectObject(old_brush);
	}
}
void SB_ScrollBar::OnSize(UINT nType, int cx, int cy) 
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
			m_mem_dc.SelectObject(&m_bitmap);
			m_mem_dc.SelectObject(&m_pen);
			m_mem_dc.SelectObject(&m_brush);
			m_mem_dc_available = true;
		}
	}
	m_w = cx;
	m_h = cy;
	CalculateRects();
}

void SB_ScrollBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
}

void SB_ScrollBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	if (m_lo_arrow_rect.PtInRect(point))
		m_downed = 0;
	else
	if (m_hi_arrow_rect.PtInRect(point))
		m_downed = 1;
	else
	if (m_pos_rect.PtInRect(point))
	{
		m_downed = 2;
		m_prev_pt = point;
		m_d_pos = PointToPos(point) - m_first_visible_element;
	}
	else
	if (m_lo_pos_rect.PtInRect(point))
		m_downed = 3;
	else
	if (m_hi_pos_rect.PtInRect(point))
		m_downed = 4;

	if (m_downed != 2)
		SetTimer(1, 20, NULL);

	Invalidate(FALSE);
}

void SB_ScrollBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_downed != -1)
	{
		m_downed = -1;
		Invalidate(FALSE);
	}
	ReleaseCapture();
}

void SB_ScrollBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_mouse_pt = point;
	if (m_downed == 2 && m_prev_pt != point)
	{		
		int pos = PointToPos(point);
		int el = max(0, min(m_total_elements - m_visible_elements, pos - m_d_pos));
		if (el != m_first_visible_element)
		{
			m_first_visible_element = el;
			m_first_visible_element = max(0, min(m_total_elements - m_visible_elements, m_first_visible_element));
			CalculateRects();
			Invalidate(FALSE);
			DoReaction();
		}
		m_prev_pt = point;
	}
}

void SB_ScrollBar::DoReaction()
{
	if (m_reaction_hwnd)
		::SendMessage(m_reaction_hwnd, m_reaction_msg, m_first_visible_element, m_downed);
}

void SB_ScrollBar::OnTimer(UINT nIDEvent) 
{
	if (m_downed == -1)
	{
		KillTimer(nIDEvent);
		return;
	}

	if (nIDEvent == 1)
	{
		KillTimer(1);
		SetTimer(2, 300, NULL);
	}

	if (nIDEvent == 2)
	{
		KillTimer(2);
		SetTimer(3, 50, NULL);
	}

	int el = m_first_visible_element;

	if (m_downed == 0)
	{
		if (m_first_visible_element > 0)
			el = m_first_visible_element - 1;
	}
	else
	if (m_downed == 1)
	{
		if (m_first_visible_element + m_visible_elements < m_total_elements)
			el = m_first_visible_element + 1;
	}
	else
	if (m_downed == 3 && m_lo_pos_rect.PtInRect(m_mouse_pt))
	{
		el = max(0, m_first_visible_element - m_visible_elements);
	}
	else
	if (m_downed == 4 && m_hi_pos_rect.PtInRect(m_mouse_pt))
	{
		el = min(m_total_elements - m_visible_elements, m_first_visible_element + m_visible_elements);
	}

	if (el != m_first_visible_element)
	{
		m_first_visible_element = el;
		CalculateRects();
		Invalidate(FALSE);
		DoReaction();
	}
}
void SB_ScrollBar::CalculateRects()
{
	int free_len = GetLength() - 2*GetWidth();
	double perc = double(m_visible_elements)/m_total_elements;
	int dragger_len = max(5, int(perc*free_len));
	int offset = (m_total_elements == m_visible_elements)?0:((free_len - dragger_len)*m_first_visible_element/(m_total_elements - m_visible_elements));

	int sz = GetWidth() - 2;
	GetRect(0, sz, m_lo_arrow_rect);
	GetRect(GetLength() - GetWidth(), sz, m_hi_arrow_rect);
	GetRect(GetWidth() - 1 + offset, dragger_len, m_pos_rect);

	GetRect(sz, offset+1, m_lo_pos_rect);
	GetRect(GetWidth() - 1 + offset + dragger_len, free_len - dragger_len - offset + 1, m_hi_pos_rect);
}

void SB_ScrollBar::GetRect(int pos, int len, CRect &rect)
{
	PosToXY(pos, 0, rect.left, rect.top);
	PosToXY(pos + len, GetWidth() - 2, rect.right, rect.bottom);
}
bool SB_HScrollBar::Create(int x, int y, int w, int h, CWnd* parent)
{
	m_total_elements = 200;
	m_visible_elements = 20;
	m_first_visible_element = 2;

	return SB_ScrollBar::Create(true, x, y, w, h, parent);
}

void SB_HScrollBar::PosToXY(int along, int across, LONG &x, LONG &y)
{
	x = 1 + along;
	y = 1 + across;
}

int SB_HScrollBar::PointToPos(CPoint point)
{
	int pos = point.x;
	if (pos < GetWidth())
		return 0;
	if (pos > GetLength() - GetWidth())
		return m_total_elements;
	int free_len = GetLength() - 2*GetWidth();
	pos -= GetWidth();
	return m_total_elements*pos/free_len;
}
bool SB_VScrollBar::Create(int x, int y, int w, int h, CWnd* parent)
{
	m_total_elements = 200;
	m_visible_elements = 10;
	m_first_visible_element = 10;

	return SB_ScrollBar::Create(false, x, y, w, h, parent);
}

void SB_VScrollBar::PosToXY(int along, int across, LONG &x, LONG &y)
{
	x = 1 + across;
	y = 1 + along;
}

int SB_VScrollBar::PointToPos(CPoint point)
{
	int pos = point.y;
	if (pos < GetWidth())
		return 0;
	if (pos > GetLength() - GetWidth())
		return m_total_elements;
	int free_len = GetLength() - 2*GetWidth();
	pos -= GetWidth();
	return m_total_elements*pos/free_len;
}

////////////////////////////////////////////////////////////////////////////////
// end