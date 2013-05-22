#ifndef __SimpleList_H_
#define __SimpleList_H_

#include "SB.h"
#include <list>
using namespace std;

struct SimpleList: CStatic
{
	//{{AFX_DATA(SimpleList)
	//}}AFX_DATA

	//{{AFX_VIRTUAL(SimpleList)
	//}}AFX_VIRTUAL

	SimpleList(int cell_w, int cell_h, int num_cells_visible, int scroll_spacing, int scroll_wideness);

	int InsertCell(int pos, DWORD data);
	void DeleteCell(int pos);

	virtual void DrawCell(CDC* dc, int cell_pos, int cell_num, DWORD cell_data, int x, int y) = 0;
	virtual void OnLeftDown(int cell_pos, int cell_num, DWORD cell_data, int x, int y) {}
	virtual void OnLeftUp(int cell_pos, int cell_num, DWORD cell_data, int x, int y) {}
	virtual void OnLeftDouble(int cell_pos, int cell_num, DWORD cell_data, int x, int y) {}
	virtual void OnRightDown(int cell_pos, int cell_num, DWORD cell_data, int x, int y) {}
	virtual void OnRightUp(int cell_pos, int cell_num, DWORD cell_data, int x, int y) {}

	list<DWORD>* GetData() { return &m_data; }

	BOOL Create(int x, int y, CWnd* parent);

	void SetNumVisibleCells(int num) { m_v_scroll_bar.SetVisibleElements(num); m_num_visible_cells = num; }
	int GetNumVisibleCells() { return m_num_visible_cells; }

	void SetCellWidth(int w) { m_cell_w = w; }
	int GetCellWidth() { return m_cell_w; }

	void SetCellHeigh(int h) { m_cell_h = h; }
	int GetCellHeigh() { return m_cell_h; }

	int GetScrollWideness() { return m_scroll_wideness; }
	int GetScrollSpaceing() { return m_scroll_spaceing; }

protected:
	bool CalcParamsForClicks(CPoint point, int &cell_pos, DWORD &cell_data);

	SB_VScrollBar m_v_scroll_bar;

	int m_total_num_cells;
	int m_num_visible_cells;
	int m_first_visible_cell;
	int m_scroll_h_reduce;

	list<DWORD> m_data;

	int m_cell_w, m_cell_h, m_scroll_spaceing, m_scroll_wideness;

	CDC m_mem_dc;
	CBitmap m_bitmap;
	bool  m_mem_dc_available;
	int m_total_h;

	//{{AFX_MSG(SimpleList)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnScroll(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
////////////////////////////////////////////////////////////////////////////////
// end