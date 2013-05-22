#include <StdAfx.h>
#include "SimpleList.h"
#include "LOG_V0.h"

#define WM_SCROLL_MESSAGE (WM_USER+1)

BEGIN_MESSAGE_MAP(SimpleList, CStatic)
	//{{AFX_MSG_MAP(SimpleList)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_MESSAGE(WM_SCROLL_MESSAGE, OnScroll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
SimpleList::SimpleList(int cell_w, int cell_h, int num_cells_visible, int scroll_spacing, int scroll_wideness):
	m_cell_w(cell_w), m_cell_h(cell_h), m_num_visible_cells(num_cells_visible), m_scroll_spaceing(scroll_spacing), m_scroll_wideness(scroll_wideness),
	m_total_num_cells(0), m_scroll_h_reduce(0),
	m_mem_dc_available(false)
{
}

BOOL SimpleList::Create(int x, int y, CWnd* parent)
{
	CRect rect(x, y, x + m_cell_w + m_scroll_spaceing + m_scroll_wideness, y + m_cell_h * m_num_visible_cells);
	if (!m_v_scroll_bar.Create(x + m_cell_w + m_scroll_spaceing, y, m_scroll_wideness, m_cell_h * m_num_visible_cells, parent))
		return FALSE;
	if (!CStatic::Create("", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rect, parent, IDC_STATIC))
	{
		m_v_scroll_bar.DestroyWindow();
		return FALSE;
	}
	m_v_scroll_bar.SetReaction(m_hWnd, WM_SCROLL_MESSAGE);
	m_v_scroll_bar.SetTotalElements(m_num_visible_cells);
	m_v_scroll_bar.SetVisibleElements(m_num_visible_cells);
	m_v_scroll_bar.SetFirstVisibleElement(1);
	return TRUE;
}

LRESULT SimpleList::OnScroll(WPARAM, LPARAM)
{
	Invalidate(FALSE);
	return 0;
}

int SimpleList::InsertCell(int pos, DWORD data)
{
	list<DWORD>::iterator c = m_data.begin();
	if (pos == -1 || m_data.empty())
	{
		pos = m_data.size();
		c = m_data.end();
	}
	else
	{
		int sz = 0;
		while (sz < pos && c != m_data.end())
		{
			sz++; c++;
		}
		pos = sz;
	}
	m_data.insert(c, data);
	m_v_scroll_bar.SetTotalElements(max(m_num_visible_cells, (int)m_data.size()));
	if (m_v_scroll_bar.GetFirstVisibleElement() > pos + 1)
	{
		m_v_scroll_bar.SetFirstVisibleElement(pos + 1);
	}
	else
	if (m_v_scroll_bar.GetFirstVisibleElement() + m_num_visible_cells < pos + 1)
	{
		m_v_scroll_bar.SetFirstVisibleElement(pos + 1 - m_num_visible_cells);
	}
	if (m_hWnd)
		Invalidate(FALSE);
	return pos;
}

void SimpleList::DeleteCell(int pos)
{
	list<DWORD>::iterator c = m_data.begin();
	if (pos == -1 || m_data.empty())
		return;
	int sz = 0;
	while (pos > 0)
	{
		pos--; c++;
		if (c == m_data.end())
			return;
	}
	m_data.erase(c);
	m_v_scroll_bar.SetTotalElements(max(m_num_visible_cells, (int)m_data.size()));
	Invalidate(FALSE);
}

void SimpleList::OnPaint()
{
	m_v_scroll_bar.Invalidate(FALSE);

	CPaintDC pdc(this);
	CDC* dc = (m_mem_dc_available)?(&m_mem_dc):(&pdc);

	int i;
	DWORD data;
	list<DWORD>::iterator c = m_data.begin();
	for (i = 0; i < m_v_scroll_bar.GetFirstVisibleElement(); i++)
		if (c != m_data.end())
			c ++;
	for (i = 0; i < m_num_visible_cells; i++)
	{
		if (c == m_data.end())
			data = 0xFFFFFFFF;
		else
		{
			data = *c;
			c++;
		}
		DrawCell(dc, i, i + m_v_scroll_bar.GetFirstVisibleElement(), data, 0, i*m_cell_h);
	}

	if (m_mem_dc_available)
		pdc.BitBlt(0, 0, m_cell_w, m_total_h, &m_mem_dc, 0, 0, SRCCOPY);
}

void SimpleList::OnSize(UINT nType, int cx, int cy)
{
	m_cell_w = cx - m_scroll_wideness - m_scroll_spaceing;
	m_cell_h = cy/m_num_visible_cells;

	//if (m_mem_dc_available)
	m_mem_dc_available = false;
	if (m_bitmap.m_hObject != NULL)
	{
		if (!m_bitmap.DeleteObject())
		{
			LOG_V0_AddMessage(LOG_V0_MESSAGE, "Ошибка графического модуля, сообщите разработчикам: m_bitmap.DeleteObject()", true);
			return;
		}
	}
	if (m_mem_dc.m_hDC != NULL)
	{
		if (!m_mem_dc.DeleteDC())
		{
			LOG_V0_AddMessage(LOG_V0_MESSAGE, "Ошибка графического модуля, сообщите разработчикам: m_mem_dc.DeleteDC()", true);
			return;
		}
	}
	CClientDC dc(this);
	if (m_mem_dc.CreateCompatibleDC(&dc))
	{
		if (m_bitmap.CreateCompatibleBitmap(&dc, m_cell_w, cy))
		{
			m_mem_dc.SelectObject(&m_bitmap);
			m_mem_dc_available = true;
		}
		else
			m_mem_dc.DeleteDC();
	}

	m_total_h = cy;

	CRect rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);
	GetParent()->ScreenToClient(&rect);
	m_v_scroll_bar.SetWindowPos(NULL, rect.left + cx - m_scroll_wideness, rect.top, m_scroll_wideness, cy - m_scroll_h_reduce, SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER);
}

bool SimpleList::CalcParamsForClicks(CPoint point, int &cell_pos, DWORD &cell_data)
{
	int i;
	list<DWORD>::iterator c = m_data.begin();
	for (i = 0; i < m_v_scroll_bar.GetFirstVisibleElement(); i++)
		if (c != m_data.end())
			c ++;
	for (i = 0; i < m_num_visible_cells; i++)
	{
		if (c == m_data.end())
			cell_data = 0xFFFFFFFF;
		else
		{
			cell_data = *c;
			c++;
		}
		if (point.x >= 0 && point.x < m_cell_w && point.y >= i*m_cell_h && point.y < (i+1)*m_cell_h)
		{
			cell_pos = i;
			return true;
		}
	}
	return false;
}

void SimpleList::OnLButtonDown(UINT nFlags, CPoint point)
{
	int cell_pos;
	DWORD cell_data;
	if (CalcParamsForClicks(point, cell_pos, cell_data))
		OnLeftDown(cell_pos, cell_pos + m_v_scroll_bar.GetFirstVisibleElement(), cell_data, point.x - 0, point.y - cell_pos*m_cell_h);
}

void SimpleList::OnLButtonUp(UINT nFlags, CPoint point)
{
	int cell_pos;
	DWORD cell_data;
	if (CalcParamsForClicks(point, cell_pos, cell_data))
		OnLeftUp(cell_pos, cell_pos + m_v_scroll_bar.GetFirstVisibleElement(), cell_data, point.x - 0, point.y - cell_pos*m_cell_h);
}

void SimpleList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int cell_pos;
	DWORD cell_data;
	if (CalcParamsForClicks(point, cell_pos, cell_data))
		OnLeftDouble(cell_pos, cell_pos + m_v_scroll_bar.GetFirstVisibleElement(), cell_data, point.x - 0, point.y - cell_pos*m_cell_h);
}

void SimpleList::OnRButtonDown(UINT nFlags, CPoint point)
{
	int cell_pos;
	DWORD cell_data;
	if (CalcParamsForClicks(point, cell_pos, cell_data))
		OnRightDown(cell_pos, cell_pos + m_v_scroll_bar.GetFirstVisibleElement(), cell_data, point.x - 0, point.y - cell_pos*m_cell_h);
}

void SimpleList::OnRButtonUp(UINT nFlags, CPoint point)
{
	int cell_pos;
	DWORD cell_data;
	if (CalcParamsForClicks(point, cell_pos, cell_data))
		OnRightUp(cell_pos, cell_pos + m_v_scroll_bar.GetFirstVisibleElement(), cell_data, point.x - 0, point.y - cell_pos*m_cell_h);
}

////////////////////////////////////////////////////////////////////////////////
// end