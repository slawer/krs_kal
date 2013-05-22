#ifndef __SB_H_
#define __SB_H_

struct SB_ScrollBar: CStatic
{
	SB_ScrollBar();
	//{{AFX_DATA(SB_ScrollBar)
	//}}AFX_DATA

	//{{AFX_VIRTUAL(SB_ScrollBar)
	//}}AFX_VIRTUAL

	void SetTotalElements(int total, bool redraw = false);
	int GetTotalElements() { return m_total_elements; }
	void SetVisibleElements(int visible, bool redraw = false);
	int GetVisibleElements() { return m_visible_elements; }
	void SetFirstVisibleElement(int first, bool redraw = false);
	int GetFirstVisibleElement() { return m_first_visible_element; }
	void SetReaction(HWND hwnd, UINT msg) { m_reaction_hwnd = hwnd; m_reaction_msg = msg; }
	void GetReaction(HWND &hwnd, UINT &msg) { hwnd = m_reaction_hwnd; msg = m_reaction_msg; }

	void SetColors(DWORD col1, DWORD col2, DWORD col3, DWORD col4, DWORD col5);
protected:
	DWORD m_color[5];

	bool Create(bool horizontal, int x, int y, int w, int h, CWnd* parent);
	void DrawRect(CRect rect, bool downed, CDC*dc);
	void OnSetParams(bool redraw);
	void DoReaction();

	virtual int GetLength() = 0;
	virtual int GetWidth() = 0;
	virtual void CalculateRects();
	void DrawArrows(CDC*dc);

	HWND m_reaction_hwnd;
	UINT m_reaction_msg;

	int m_total_elements, m_visible_elements, m_first_visible_element, m_d_pos;

	int m_w, m_h, m_downed;
	CRect m_lo_arrow_rect, m_hi_arrow_rect, m_pos_rect, m_lo_pos_rect, m_hi_pos_rect;
	CPoint m_prev_pt, m_mouse_pt;

	CDC m_mem_dc;
	CBitmap m_bitmap;
	bool  m_mem_dc_available;
	CPen m_pen;
	CBrush m_brush;

	virtual void GetRect(int pos, int len, CRect &rect);
	virtual void PosToXY(int along, int across, LONG &x, LONG &y) = 0;
	virtual int PointToPos(CPoint point) = 0;

	//{{AFX_MSG(SB_ScrollBar)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

struct SB_HScrollBar: SB_ScrollBar
{
	bool Create(int x, int y, int w, int h, CWnd* parent);

	virtual void PosToXY(int along, int across, LONG &x, LONG &y);
	virtual int PointToPos(CPoint point);

	virtual int GetLength() { return m_w; }
	virtual int GetWidth() { return m_h; }
};

struct SB_VScrollBar: SB_ScrollBar
{
	bool Create(int x, int y, int w, int h, CWnd* parent);

	virtual void PosToXY(int along, int across, LONG &x, LONG &y);
	virtual int PointToPos(CPoint point);

	virtual int GetLength() { return m_h; }
	virtual int GetWidth() { return m_w; }
};

#endif
////////////////////////////////////////////////////////////////////////////////
// end
