#pragma once

#include <InterfaceObject.h>
struct PUMENU_Menu: InterfaceObject
{
	PUMENU_Menu(int x_count, int y_count, int cell_w, int cell_h, DWORD bk_color = 0xFFFFFF, bool with_shadow = true);

	void Show(CPoint point, int horizontal_alignment, int vertical_alignment, CWnd* wnd); 
	void SetFont(CFont* font);

	int GetBorderWidth() { return m_border_width; }
	int GetWindowWidth() { return m_x_cells_num*m_cell_w + 2*m_border_width; }
	int GetWindowHeight() { return m_y_cells_num*m_cell_h + 2*m_border_width; }

	virtual void BeforeDrawAll(CDC*dc) {}
	virtual void AfterDrawAll(CDC*dc) {}
	virtual void OnSelectionChanged() {}

friend class PopupMenuWindow;

protected:
	virtual void BeforeShow() {};
	virtual void PrepareDC(CDC* dc) {};
	virtual void DrawCell(CDC* dc, int x_pos, int y_pos, int x, int y) {};
	virtual void OnClose() {};

	void UpdateDesktopSize();

	int m_x_cells_num, m_y_cells_num, m_cell_w, m_cell_h;
	int m_selected_cell_x_pos, m_selected_cell_y_pos;
	int m_desktop_w, m_desktop_h;

	void CalculateXY(int x_pos, int y_pos, int &x, int &y);
	virtual void OnMouseMove() {}

private:
	void PrivateOnMouseMove();
	void PrivateOnClose();
	void Draw(CDC* dc);
	void DrawOneCell(CDC* dc, int x_pos, int y_pos, bool alone = true);

	bool m_with_shadow, m_closing;
	int m_border_width;
};

////////////////////////////////////////////////////////////////////////////////
// end