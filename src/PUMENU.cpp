#include <StdAfx.h>
#include "PUMENU.h"
PUMENU_Menu::PUMENU_Menu(int x_count, int y_count, int cell_w, int cell_h, DWORD bk_color, bool with_shadow):
	m_x_cells_num(x_count), m_y_cells_num(y_count), m_cell_w(cell_w), m_cell_h(cell_h),
	m_border_width(1), m_with_shadow(with_shadow), m_closing(false)
{
	m_can_loose_capture = false;
	m_always_capture = true;

	m_bk_color = bk_color;
	SetWindowName("PUMENU_Menu");

	SetMessageHandler(WM_LBUTTONDOWN, (InterfaceObject_OwnFunction)(&PUMENU_Menu::PrivateOnMouseMove));
	SetMessageHandler(WM_LBUTTONUP, (InterfaceObject_OwnFunction)(&PUMENU_Menu::PrivateOnClose));
	SetMessageHandler(WM_RBUTTONDOWN, (InterfaceObject_OwnFunction)(&PUMENU_Menu::PrivateOnClose));
	SetMessageHandler(WM_RBUTTONUP, (InterfaceObject_OwnFunction)(&PUMENU_Menu::PrivateOnClose));
	SetMessageHandler(WM_MOUSEMOVE, (InterfaceObject_OwnFunction)(&PUMENU_Menu::PrivateOnMouseMove));
}

void PUMENU_Menu::Show(CPoint point, int horizontal_alignment, int vertical_alignment, CWnd* parent_wnd)
{
	m_closing = false;
	m_selected_cell_x_pos = m_selected_cell_y_pos = -1;

	CWnd* wnd = GetWnd();
	if (wnd->m_hWnd == 0)
	{
		InterfaceObject::Create(parent_wnd, 0, 0, 100, 100, false, false, m_bk_color, true, m_with_shadow?CS_DROPSHADOW:0, 0);
		wnd->ModifyStyle(WS_CAPTION | WS_DLGFRAME, WS_POPUP, 0);
	}
	UpdateDesktopSize();
	BeforeShow();

	int w = min(m_desktop_w, GetWindowWidth());
	int h = min(m_desktop_h, GetWindowHeight());
	int x = point.x - w*(1 - horizontal_alignment)/2;
	x = min(max(0, x), m_desktop_w - w);
	int y = point.y - h*(1 - vertical_alignment)/2;
	y = min(max(0, y), m_desktop_h - h);
	ChangePosition(x, y, w, h);
	wnd->ShowWindow(SW_NORMAL);

	//parent_wnd->ClientToScreen(&point);
	wnd->ScreenToClient(&point);
	m_winmessage_point = m_prev_winmessage_point = point;
	PrivateOnMouseMove();

	AdjustStartMouseStatus();

	//SetCapture();
}

void PUMENU_Menu::UpdateDesktopSize()
{
	CWnd* desktop_wnd = m_wnd->GetDesktopWindow();
	CRect desktop_rect;
	desktop_wnd->GetClientRect(&desktop_rect);
	m_desktop_w = desktop_rect.Width();
	m_desktop_h = desktop_rect.Height();
}

void PUMENU_Menu::Draw(CDC* dc)
{
	PrepareDC(dc);
	BeforeDrawAll(dc);

	int i, j;
	for (i = 0; i < m_x_cells_num; i++)
		for (j = 0; j < m_y_cells_num; j++)
			DrawOneCell(dc, i, j, false);

	AfterDrawAll(dc);
}

void PUMENU_Menu::CalculateXY(int x_pos, int y_pos, int &x, int &y)
{
	x = m_border_width + x_pos*m_cell_w;
	y = m_border_width + y_pos*m_cell_h;
}

void PUMENU_Menu::DrawOneCell(CDC* dc, int x_pos, int y_pos, bool alone)
{
	CDC* cdc = (alone && IsAvailableMemDC())? GetMemDC() : dc;

	int x, y;
	CalculateXY(x_pos, y_pos, x, y);
	DrawCell(cdc, x_pos, y_pos, x, y);

	if (alone && IsAvailableMemDC())
		dc->BitBlt(x, y, m_cell_w, m_cell_h, GetMemDC(), x, y, SRCCOPY);
}

void PUMENU_Menu::PrivateOnClose()
{
	if (m_closing)
		return;
	m_closing = true;
	CWnd *wnd = GetWnd();
	if (wnd->GetCapture() == wnd)
		ReleaseCapture();
	wnd->SendMessage(WM_CLOSE, 0, 0);
	OnClose();
}

void PUMENU_Menu::PrivateOnMouseMove()
{
	if (m_wnd == NULL || !IsWindow(m_wnd->m_hWnd))
		return;
	CClientDC dc(m_wnd);
	PrepareDC(&dc);
	int i, j, x, y, prev_x_pos, prev_y_pos;
	for (i = 0; i < m_x_cells_num; i++)
		for (j = 0; j < m_y_cells_num; j++)
		{
			CalculateXY(i, j, x, y);
			if (m_winmessage_point.x >= x && m_winmessage_point.x < x + m_cell_w && m_winmessage_point.y >= y && m_winmessage_point.y < y + m_cell_h)
			{
				if (i != m_selected_cell_x_pos || j != m_selected_cell_y_pos)
				{
					prev_x_pos = m_selected_cell_x_pos;
					prev_y_pos = m_selected_cell_y_pos;
					m_selected_cell_x_pos = i;
					m_selected_cell_y_pos = j;
					if (prev_x_pos != -1 && prev_y_pos != -1) 
						DrawOneCell(&dc, prev_x_pos, prev_y_pos);
					DrawOneCell(&dc, m_selected_cell_x_pos, m_selected_cell_y_pos);
					OnSelectionChanged();
				}
				OnMouseMove();
				return;
			}
		}
	if (m_selected_cell_x_pos != -1 && m_selected_cell_y_pos != -1)
	{
		prev_x_pos = m_selected_cell_x_pos;
		prev_y_pos = m_selected_cell_y_pos;
		m_selected_cell_x_pos = m_selected_cell_y_pos = -1;
		DrawOneCell(&dc, prev_x_pos, prev_y_pos);
		OnSelectionChanged();
	}
	OnMouseMove();
}

////////////////////////////////////////////////////////////////////////////////
// end