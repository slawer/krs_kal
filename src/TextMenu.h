//#pragma once

#include <map>
using namespace std;

#include "PUMENU.h"

enum TextMenu_Alignment
{
	TextMenu_Left = 0,
	TextMenu_Center,
	TextMenu_Right
};

struct TextMenu: PUMENU_Menu
{	
	TextMenu(int max_h_count, int increased_w = 10, TextMenu_Alignment text_alignment = TextMenu_Center, int text_offset = 0);
	virtual ~TextMenu();

	virtual void OnSelect(int pos, DWORD data, CString str); // вызывается при удачном выборе элемента при закрытии меню

	virtual void BeforeShow();
	virtual void PrepareDC(CDC* dc);
	virtual void DrawCell(CDC* dc, int x_pos, int y_pos, int x, int y);
	virtual void OnMouseMove();
	virtual void OnClose();
	virtual void OnTimer(UINT timer_id);

	void Add(CString str, DWORD data = 0);
	void Clear();
	int GetCount();

	void OnKey();

	void SetFont(CFont* font);

	TextMenu_Alignment GetTextAlignment() { return m_text_alignment; }
	void SetTextAlignment(TextMenu_Alignment alignment) { m_text_alignment = alignment; }
	int GetTextOffset() { return m_text_offset; }
	void SetTextOffset(int offset) { m_text_offset = offset; }

	void SetOnSelectMessage(CWnd* wnd, UINT msg)
	{
		m_on_select_wnd = wnd;
		m_on_select_msg = msg;
	}

	static CFont *m_default_font;

protected:
	CWnd* m_on_select_wnd;
	UINT m_on_select_msg;

	int m_max_h_count, m_increased_w;
	CFont *m_font;
	
	int m_index_offset;
	bool m_timer1_set, m_internal_font;
	TextMenu_Alignment m_text_alignment;
	int m_text_offset;
	
	struct Element
	{
		Element(CString str = "", DWORD data = 0, int position = 0):
			m_str(str), m_data(data), m_position(position)
		{
		}
		CString GetStr() { return m_str; }
		DWORD GetData() { return m_data; }
	private:
		CString m_str;
		DWORD m_data;
		int m_position;
	};

	map<int, Element*> m_elements;
};

////////////////////////////////////////////////////////////////////////////////
// end