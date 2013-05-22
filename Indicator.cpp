#include "stdafx.h"
#include "Indicator.h"

Indicator::Indicator():
	m_state(STATE_UNKNOWN), m_blinked(false)
{
	m_states_info[STATE_UNKNOWN].m_text = "?";
	m_states_info[STATE_UNKNOWN].m_text_color = 0x000000;
	m_states_info[STATE_UNKNOWN].m_bk_color = 0x00FFFF;
	m_states_info[STATE_ON].m_text = "ON";
	m_states_info[STATE_ON].m_text_color = 0x000000;
	m_states_info[STATE_ON].m_bk_color = 0x00FF00;
	m_states_info[STATE_OFF].m_text = "OFF";
	m_states_info[STATE_OFF].m_text_color = 0xFFFFFF;
	m_states_info[STATE_OFF].m_bk_color = 0x0000FF;
}

void Indicator::SetState(byte state)
{
	MutexWrap access(m_acces_mutex);
	if (state == m_state)
		return;
	m_blinked = false;
	if (IsWindow(m_hWnd) && m_states_info[m_state].m_blinking)
		KillTimer(1);
	m_state = state;
	if (IsWindow(m_hWnd) && m_states_info[m_state].m_blinking)
		SetTimer(1, m_states_info[m_state].m_blink_interval, NULL);
	if (IsWindow(m_hWnd))
		Invalidate(FALSE);
}

byte Indicator::GetState()
{
	MutexWrap access(m_acces_mutex);
	return m_state;
}

CString Indicator::GetCurrentStateText()
{
	MutexWrap access(m_acces_mutex);
	return m_states_info[m_state].m_text;
}

void Indicator::OnTimer(UINT nIDEvent)
{
	MutexWrap access(m_acces_mutex);
	if (m_states_info[m_state].m_blinking)
	{
		m_blinked = !m_blinked;
		Invalidate(FALSE);
	}
}

void Indicator::SetStateBlinking(byte state, bool blinking, DWORD interval, DWORD bk_color, DWORD text_color, CString text)
{
	if (m_states_info[state].m_blinking)
		KillTimer(1);

	if (!blinking)
	{
		m_states_info[state].m_blinking = false;
	}
	else
	{
		m_states_info[state].m_blinking = true;
		m_states_info[state].m_blink_bk_color = bk_color;
		m_states_info[state].m_blink_text_color = text_color;
		m_states_info[state].m_blink_interval = interval;
		if (text == "")
			text = m_states_info[state].m_text;
		m_states_info[state].m_blink_text = text;
		if (m_state == state)
		{
			m_blinked = false;
			SetTimer(1, interval, NULL);
		}
	}
}

void Indicator::SetStateText(byte state, CString text, bool invalidate)
{
	MutexWrap access(m_acces_mutex);
	m_states_info[state].m_text = text;
	if (invalidate && m_hWnd != 0 && ::IsWindow(m_hWnd))
		Invalidate(FALSE);
}

void Indicator::SetStateTextColor(byte state, DWORD text_color)
{
	MutexWrap access(m_acces_mutex);
	m_states_info[state].m_text_color = text_color;
	if(m_hWnd == 0) return;
	if (::IsWindow(m_hWnd))
		Invalidate(FALSE);
}

void Indicator::SetStateBkColor(byte state, DWORD bk_color)
{
	MutexWrap access(m_acces_mutex);
	m_states_info[state].m_bk_color = bk_color;
	if(m_hWnd == 0) return;
	if (::IsWindow(m_hWnd))
		Invalidate(FALSE);
}

BEGIN_MESSAGE_MAP(Indicator, CStatic)
	//{{AFX_MSG_MAP(Indicator)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void Indicator::OnPaint()
{
	CPaintDC dc(this);

	CFont *font = GetFont();
	CRect rect;
	GetClientRect(&rect);

	CString text;
	DWORD bk_col, txt_col;
	{
		MutexWrap access(m_acces_mutex);
		if (m_blinked)
		{
			text = m_states_info[m_state].m_blink_text;
			bk_col = m_states_info[m_state].m_blink_bk_color;
			txt_col = m_states_info[m_state].m_blink_text_color;
		}
		else
		{
			text = m_states_info[m_state].m_text;
			bk_col = m_states_info[m_state].m_bk_color;
			txt_col = m_states_info[m_state].m_text_color;
		}
	}

	dc.FillSolidRect(rect, 0x000000);
	rect.DeflateRect(1, 1);
	dc.FillSolidRect(rect, bk_col);
	CFont *OldFont = dc.SelectObject(font);
	ASSERT(OldFont);
	dc.SetBkMode(TRANSPARENT);
	CSize sz = dc.GetTextExtent(text);
	dc.SetTextColor(txt_col);
	dc.TextOut((rect.Width() - sz.cx)/2 + 1, (rect.Height() - sz.cy)/2 + 1, text);
	dc.SelectObject(OldFont);

	CStatic::OnPaint();
}