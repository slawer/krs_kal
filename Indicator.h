#ifndef __INDICATOR_H_
#define __INDICATOR_H_

//#pragma warning(disable:4786)

#include <MutexWrap.h>

#include <map>
using namespace std;

#define STATE_OFF 0
#define STATE_ON 1
#define STATE_UNKNOWN 2
#define STATE_ERROR 3

#define NAME_BK_COLOR 0x888888
#define NAME_BLINK_BK_COLOR 0xAAAAAA
#define NAME_TEXT_COLOR 0xFFFFFF

struct Indicator_StateInfo
{
	Indicator_StateInfo():
		m_blinking(false)
	{
	}

	CString m_text;
	DWORD m_text_color, m_bk_color;
	bool m_blinking;
	CString m_blink_text;
	DWORD m_blink_text_color, m_blink_bk_color, m_blink_interval;
};

struct Indicator: public CStatic
{
	Indicator();

	void SetState(byte state);
	byte GetState();

	void SetStateBlinking(byte state, bool blinking, DWORD interval, DWORD bk_color, DWORD text_color, CString text = "");

	void SetStateText(byte state, CString text, bool invalidate = true);
	void SetStateTextColor(byte state, DWORD text_color);
	void SetStateBkColor(byte state, DWORD bk_color);
	CString GetCurrentStateText();

	byte m_state;

protected:
	map<byte, Indicator_StateInfo> m_states_info;

	CMutex m_acces_mutex;
	bool m_blinked;
	//{{AFX_MSG(Indicator)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
public:
	DECLARE_MESSAGE_MAP()
};

#endif
