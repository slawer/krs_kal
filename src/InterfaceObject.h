#pragma once

#include <map>
using namespace std;

struct InterfaceObject;

typedef void (InterfaceObject::*InterfaceObject_OwnFunction)(void);
typedef map<UINT, InterfaceObject_OwnFunction> InterfaceObject_OwnFunctions;

typedef void (*InterfaceObject_GlobalFunction)(void);
typedef map<UINT, InterfaceObject_GlobalFunction> InterfaceObject_GlobalFunctions;

struct InterfaceObject
{
	InterfaceObject();
	virtual ~InterfaceObject();

	friend struct InterfaceObject_Window;

	void SetMessageHandler(UINT msg, InterfaceObject_OwnFunction handler);
	void SetMessageHandler(UINT msg, InterfaceObject_GlobalFunction handler);
	void Create(CWnd *parent, CRect &rect, bool child, bool border, DWORD bk_color = 0, bool with_brush = false, DWORD additional_class_style = 0, DWORD window_style = WS_VISIBLE);
	void Create(CWnd *parent, int x, int y, int w, int h, bool child, bool border, DWORD bk_color = 0, bool with_brush = false, DWORD additional_class_style = 0, DWORD window_style = WS_VISIBLE);
	virtual void OnCreate() {}

	void ChangePosition(int x, int y, int new_w, int new_h, int add_swp = SWP_NOREDRAW);

	void SetTimer(UINT timer_id, UINT millisec);
	void KillTimer(UINT timer_id);
	virtual void OnTimer(UINT timer_id) {}

	virtual void Invalidate(BOOL erase = FALSE);

	CWnd* GetWnd() { return m_wnd; }
	CRect GetWindowRect() { return m_window_rect; }
	CPoint GetWindowPos() { return m_window_pos; }
	CPoint GetMousePos() { return m_winmessage_point; }

	DWORD GetFlag() { return m_winmessage_flag; }

	void SetWindowName(CString str);

	bool IsAvailableMemDC() { return m_mem_dc_available; }
	CDC* GetMemDC() { return &m_mem_dc; }
	CBitmap* GetMemBitmap() { return &m_bitmap; }

	bool IsLeftButtonDown() { return m_left_button_down; }
	bool IsRightButtonDown() { return m_right_button_down; }
	bool IsMiddleButtonDown() { return m_middle_button_down; }

	bool IsMouseIn();

	DWORD m_bk_color;
	bool m_report_mouse_move_when_leave;

protected:
	void CreateMemDC();
	void DeleteMemDC();

	virtual void Draw(CDC* dc) {}
	virtual void OnDestroyWindow() {}
	virtual void OnChangeSize() {}

	bool m_use_mem_dc, m_capture_focus;

	CPoint m_window_pos;
	CRect m_window_rect;	
	CWnd* m_wnd;
	CPoint m_winmessage_point, m_prev_winmessage_point;
	DWORD m_winmessage_flag;
	WPARAM m_winmessage_wparam;
	LPARAM m_winmessage_lparam;
	InterfaceObject_OwnFunctions m_message_own_handlers;
	InterfaceObject_GlobalFunctions m_message_global_handlers;
	map<UINT, UINT> m_key_pressed;
	int m_wheel_direction;
	bool m_left_button_down, m_right_button_down, m_middle_button_down;

	bool m_can_loose_capture, m_always_capture;
	void AdjustStartMouseStatus(); 

private:
	bool m_need_to_set_downs;

	CDC m_mem_dc;
	CBitmap m_bitmap;
	bool m_mem_dc_available;
	bool m_has_borders;

	void PrivateOnChangeSize();
	void PrivateDraw(CDC* dc);
	void ExecuteHandler(UINT msg);
	void OnWinMessage(UINT msg, UINT flags, CPoint point);	
};
////////////////////////////////////////////////////////////////////////////////
// end