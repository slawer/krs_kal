#include "stdafx.h"
#include "KRS.h"
#include "DlgCalibrNew.h"
#include "src\TableList.h"
#include "DlgWaiting.h"
#include "DlgEditSensor.h"

#define WM_READY (WM_USER + 1)
#define WM_REBUILT (WM_USER + 2)

IMPLEMENT_DYNAMIC(DlgCalibrNew, CDialog)

BEGIN_MESSAGE_MAP(DlgCalibrNew, CDialog)
ON_WM_PAINT()
ON_WM_TIMER()
ON_MESSAGE(WM_READY, OnReady)
ON_MESSAGE(WM_REBUILT, OnRebuilt)
ON_BN_CLICKED(IDC_BUTTON_ADD, &DlgCalibrNew::OnBnClickedButtonAdd)
ON_BN_CLICKED(IDC_BUTTON_RESET, &DlgCalibrNew::OnBnClickedButtonReset)
ON_BN_CLICKED(IDC_BUTTON_SAVE, &DlgCalibrNew::OnBnClickedButtonSave)
ON_BN_CLICKED(IDC_RADIO1, &DlgCalibrNew::OnBnClickedRadio1)
ON_BN_CLICKED(IDC_RADIO2, &DlgCalibrNew::OnBnClickedRadio2)
ON_BN_CLICKED(IDC_RADIO3, &DlgCalibrNew::OnBnClickedRadio3)
ON_BN_CLICKED(IDC_BUTTON_CLEAR, &DlgCalibrNew::OnBnClickedButtonClear)
ON_BN_CLICKED(IDC_BUTTON_KEEP, &DlgCalibrNew::OnBnClickedButtonKeep)
ON_BN_CLICKED(IDC_CHECK_CALC_OUTER, &DlgCalibrNew::OnBnClickedCheckCalcOuter)
ON_BN_CLICKED(IDC_BUTTON_CFG, &DlgCalibrNew::OnBnClickedButtonCfg)
ON_BN_CLICKED(IDC_CHECK_SCALE, &DlgCalibrNew::OnBnClickedCheckScale)
END_MESSAGE_MAP()

static DlgWaiting *g_dlg_waiting = NULL;
static TableList g_table_list;
static CParam* g_param = NULL;
static SERV_Channel* g_channel = NULL;
static int g_phys_val_interval = 0xFFFF, g_min_phys_val = 0;
static int g_param_k = 1;
static SERV_CalibrationTable g_current_cal_table, g_saved_cal_table;
static CFont g_horizontal_font, g_vertical_font;
static bool g_table_ready = false, g_fonts_created = false, g_need_to_read_table = false, g_read_or_write, g_first_time, g_timeout, g_error_on_calculation;
static WORD g_current_phys, g_current_dsp;

struct MemDc
{
	MemDc():
		m_inited(false)
	{}

	void Init(CPaintDC& pdc, int x, int y, int w, int h)
	{
		if (m_inited)
			return;

		if (!m_mem_dc.CreateCompatibleDC(&pdc))
			return;

		if (!m_mem_bitmap.CreateCompatibleBitmap(&pdc, x + w, y + h))
		{
			m_mem_dc.DeleteDC();
			return;
		}

		m_mem_dc.SelectObject(&m_mem_bitmap);
		m_mem_dc.SetBkMode(TRANSPARENT);
		SetGraphicsMode(m_mem_dc.m_hDC, GM_ADVANCED);
		m_inited = true;
	}

	void Blt(CPaintDC& pdc, int x, int y, int w, int h)
	{
		pdc.BitBlt(x, y, w, h, &m_mem_dc, x, y, SRCCOPY);
	}

	CDC m_mem_dc;
	CBitmap m_mem_bitmap;
	bool m_inited;
};

static MemDc g_mem_dc;

static int g_x, g_y, g_w, g_h;

DlgCalibrNew::DlgCalibrNew(CParam *param, CWnd* pParent /*=NULL*/)
	: CDialog(DlgCalibrNew::IDD, pParent)
{
	g_param = param;
	SERV_DeviceCfg* cfg = (g_param->m_channel_conv_num == 0) ? &m_convertor_cfg : SERV_current_device_cfg;
	g_channel = cfg->GetChannel(g_param->m_channel_num);
	
	if (g_param->m_attr_channel_num != -1)
	{
		SERV_Channel* channel;
		cfg = (g_param->m_attr_channel_conv_num == 0) ? &m_convertor_cfg : SERV_current_device_cfg;
		if (cfg == NULL)
		{
			MessageBox("cfg == NULL", " Калибровка невозможна");
//			EndDialog(IDCANCEL);
//			return FALSE;
		}
		channel = cfg->GetChannel(g_param->m_attr_channel_num);
		if (channel == NULL)
		{
			MessageBox("channel == NULL", " Калибровка невозможна");
//			EndDialog(IDCANCEL);
//			return FALSE;
		}
		if (channel->GetDevice() == NULL)
		{
			MessageBox("channel->m_device == NULL", " Калибровка невозможна");
//			EndDialog(IDCANCEL);
//			return FALSE;
		}
		if (channel->IsTimedOut())
		{
			MessageBox("Канал атрибут молчит", " Калибровка невозможна");
//			EndDialog(IDCANCEL);
//			return FALSE;
		}
		int pw = 0x03 & (WORD)channel->GetLastValue();
		g_param_k = (int)pow(10, (float)pw);
	}
	else
		g_param_k = 1;
}

DlgCalibrNew::~DlgCalibrNew()
{
}

void DlgCalibrNew::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void DlgCalibrNew::OnCancel()
{
	CKRSApp* app = (CKRSApp*)AfxGetApp();
	app->WriteIntegerValueToRegistry("Settings","калибровка.масштабирование", g_table_list.m_scaling);
	app->WriteIntegerValueToRegistry("Settings","калибровка.рассчёт", g_table_list.m_calc_outer_points);
	CDialog::OnCancel(); 
}

void DlgCalibrNew::OnOK()
{
}

void CalcCurrentDSP()
{
	g_error_on_calculation = true;
	int channel_phys = (int)g_channel->GetLastValue();

	g_current_dsp = 0xFFFF;
	SERV_CalibrationTable::iterator current;
	SERV_CalibrationPoint curr_p, prev_p;
	if (g_saved_cal_table.size() > 0)
	{
		current = g_saved_cal_table.begin();
		while (current != g_saved_cal_table.end())
		{
			curr_p = *current;
			if (current != g_saved_cal_table.begin() && 
				(
				(prev_p.m_phys_val >= channel_phys && channel_phys >= curr_p.m_phys_val)
				||
				(curr_p.m_phys_val >= channel_phys && channel_phys >= prev_p.m_phys_val))
				)
			{
				g_current_dsp = prev_p.m_dsp_val + BS_ROUND(float(channel_phys - prev_p.m_phys_val)*(curr_p.m_dsp_val - prev_p.m_dsp_val)/(curr_p.m_phys_val - prev_p.m_phys_val));
				current = g_current_cal_table.begin();
				while (current != g_current_cal_table.end())
				{
					curr_p = *current;
					if (current != g_current_cal_table.begin() && 
						(
						(prev_p.m_dsp_val >= g_current_dsp && g_current_dsp >= curr_p.m_dsp_val)
						||
						(curr_p.m_dsp_val >= g_current_dsp && g_current_dsp >= prev_p.m_dsp_val))
						)
					{
						g_current_phys = (prev_p.m_phys_val + BS_ROUND(float(g_current_dsp - prev_p.m_dsp_val)*(curr_p.m_phys_val - prev_p.m_phys_val)/(curr_p.m_dsp_val - prev_p.m_dsp_val)));
						g_error_on_calculation = false;
						return;
					}
					prev_p = curr_p;
					current++;
				}
				return;
			}
			prev_p = curr_p;
			current++;
		}
	}
}

void DlgCalibrNew::OnTimer(UINT nIDEvent) 
{
	static int counter = 0;
	static bool set_range = false;

	if (g_first_time)
	{
		g_first_time = false;
		counter = 0;
		set_range = false;
	}
	
	if (g_need_to_read_table)
	{
		g_need_to_read_table = false;
		ReadTable();
		return;
	}

	if (nIDEvent == 1)
	{
		counter++;
		if (g_dlg_waiting != NULL && ::IsWindow(g_dlg_waiting->m_hWnd))
		{
			if (!set_range)
			{
				g_dlg_waiting->SetWindowText(g_read_or_write?" Идёт чтение данных из устройства...":" Идёт запись данных в устройство");
				g_dlg_waiting->m_progress_bar.SetRange(0, 600);
				set_range = true;
				if (g_read_or_write)
				{
					if (!SERV_RequestCalibrationTable(g_channel, true))
					{
						if (g_dlg_waiting !=0 && ::IsWindow(g_dlg_waiting->m_hWnd))
							g_dlg_waiting->SignalToClose(IDABORT);
						SERV_StopCalibration();
						EndDialog(IDCANCEL);
					}
				}
				else
				{
					if (!SERV_WriteCalibrationTable(g_channel))
					{
						if (g_dlg_waiting !=0 && ::IsWindow(g_dlg_waiting->m_hWnd))
							g_dlg_waiting->SignalToClose(IDABORT);
						SERV_StopCalibration();
						MessageBox("Ошибка при сохранении таблицы", " Таблица не будет сохранена в устройство");
					}
				}
			}
			g_dlg_waiting->m_progress_bar.SetPos(counter);
		}
			
		if (counter >= 600)
		{
			KillTimer(1);
			counter = 0;
			set_range = false;
			if (g_dlg_waiting !=0 && ::IsWindow(g_dlg_waiting->m_hWnd))
				g_dlg_waiting->SignalToClose(IDABORT);
			SERV_StopCalibration();
			if (g_read_or_write)
				EndDialog(IDABORT);
			else
				MessageBox("Ошибка при сохранении таблицы", " Таблица не была сохранена в устройство");
		}
		
		return;
	}
	if (nIDEvent == 2)
	{
		if (g_param)
			g_timeout = g_param->IsTimedOut();
		else
			g_timeout = g_channel->IsTimedOut();
		if (!g_timeout)
			CalcCurrentDSP();
		Invalidate(FALSE);
		if (g_table_ready)
		{
			if (g_timeout)
			{
				SetDlgItemText(IDC_EDIT_VALUE_NOW, "ОТКЛ");
				SetDlgItemText(IDC_EDIT_SIGNAL_NOW, "ОТКЛ");
			}
			else
			{
				SetDlgItemText(IDC_EDIT_VALUE_NOW, g_table_list.GetTextSecondCol(g_current_phys));
				if (g_error_on_calculation)
					SetDlgItemText(IDC_EDIT_SIGNAL_NOW, "ОШИБКА");
				else
					SetDlgItemText(IDC_EDIT_SIGNAL_NOW, g_table_list.GetTextFirstCol(g_current_dsp));
			}
		}
		if (!IsDlgButtonChecked(IDC_RADIO1 + g_table_list.m_col1_mode))
		{
			CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO1 + g_table_list.m_col1_mode);
		}
	}
}

void DlgCalibrNew::ReadTable()
{
	g_first_time = true;
	g_read_or_write = true;
	DlgWaiting dlg(true, this);
	g_dlg_waiting = &dlg;
	SetTimer(1, 100, NULL);
	if (dlg.DoModal() == IDCANCEL)
	{
		SERV_StopCalibration();
		EndDialog(IDIGNORE);
	}
	g_dlg_waiting = NULL;
}

void DlgCalibrNew::WriteTable()
{
	g_first_time = true;
	g_read_or_write = false;
	DlgWaiting dlg(true);
	g_dlg_waiting = &dlg;
	SetTimer(1, 100, NULL);
	if (dlg.DoModal() == IDCANCEL)
	{
		SERV_StopCalibration();
		MessageBox("Сохранение таблицы прервано пользователем", " Таблица не была сохранена в устройство");
	}
	g_dlg_waiting = NULL;
}

BOOL DlgCalibrNew::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString title;
	if (g_param)
		title = TXT("Калибровка параметра «%s»") << g_param->sName;
	else
		title = TXT("Калибровка канала «%s»") << g_channel->GetInfo();
	SetWindowText(" " + title);

	SERV_connection_info.m_calibration_table_ready_wnd = m_hWnd;
	SERV_connection_info.m_calibration_table_ready_msg = WM_READY;
	g_saved_cal_table.clear();
	g_current_cal_table.clear();

	CButton *btn = (CButton*)GetDlgItem(IDC_BUTTON_KEEP);
	HICON icon = KRS_app.LoadIcon(IDI_ICON_KEEP);
	btn->SetIcon(icon);
	
	if (!g_fonts_created)
	{
		g_fonts_created = true;
		CFont *font = GetFont();
		LOGFONT log;
		font->GetLogFont(&log);
		g_horizontal_font.CreateFontIndirect(&log);
		log.lfEscapement = 900;
		log.lfOrientation = 900;
		g_vertical_font.CreateFontIndirect(&log);
	}
	
	CRect rect;
	CWnd *w = GetDlgItem(IDC_STATIC_TABLE_POS);
	w->GetWindowRect(&rect);
	ScreenToClient(&rect);
	g_table_list.SetCellWidth(rect.right - rect.left);
	g_table_list.SetNumVisibleCells(11);
	g_table_list.Create(rect.left, rect.top, this);
	w->SetWindowPos(NULL, rect.left, rect.top, g_table_list.GetCellWidth(), g_table_list.GetNumVisibleCells() * g_table_list.GetCellHeigh(), SWP_NOZORDER);
	g_table_list.SetFont(GetFont());
	g_table_list.m_rebuilt_message = WM_REBUILT;
	g_table_list.m_col1_mode = TableList_MODE_DSP;
	g_table_list.Rebuild();

	w = GetDlgItem(IDC_STATIC_GRAPH_POS);
	w->GetWindowRect(&rect);
	ScreenToClient(&rect);
	g_x = rect.left;
	g_y = rect.top;
	g_w = rect.right - rect.left;
	g_h = g_table_list.GetNumVisibleCells() * g_table_list.GetCellHeigh();
	w->SetWindowPos(NULL, g_x, g_y, g_w, g_h, SWP_NOZORDER);
	
	g_table_list.m_use_voltage = g_channel->m_use_voltage;
	g_table_list.m_v0 = g_channel->m_v0;
	g_table_list.m_voltage_k = (g_channel->m_v1 - g_channel->m_v0)/g_channel->m_dsp1;

	g_need_to_read_table = true;
	g_first_time = true;
	g_table_ready = false;

	CKRSApp* app = (CKRSApp*)AfxGetApp();
	g_table_list.m_scaling = (app->ReadIntegerValueFromRegistry("Settings","калибровка.масштабирование", g_table_list.m_scaling?1:0) == 1);
	CheckDlgButton(IDC_CHECK_SCALE, g_table_list.m_scaling);
	g_table_list.m_calc_outer_points = (app->ReadIntegerValueFromRegistry("Settings","калибровка.рассчёт", g_table_list.m_calc_outer_points?1:0) == 1);
	CheckDlgButton(IDC_CHECK_CALC_OUTER, g_table_list.m_calc_outer_points);
	
	SetTimer(2, 100, NULL);
	return TRUE;
}

void CalcPhysInterval()
{
	if (g_current_cal_table.size() == 0)
		return;
		
	SERV_CalibrationTable::iterator current = g_current_cal_table.begin();
	g_min_phys_val = 0xFFFF;
	g_phys_val_interval = 0;
	WORD curr_val;
	while (current != g_current_cal_table.end())
	{
		curr_val = current->m_phys_val;
		current++;
		if (g_table_list.m_scaling && current == g_current_cal_table.end() && g_current_cal_table.size() > 2)
			break;
		if (curr_val < g_min_phys_val)
			g_min_phys_val = curr_val;
		if (curr_val > g_phys_val_interval)
			g_phys_val_interval = curr_val;
	}
	g_min_phys_val = 0; 
	g_phys_val_interval -= g_min_phys_val;
}

static int w, h, x, y_top, y_bottom;

void GetXY(SERV_CalibrationPoint& p, int&gx, int&gy)
{
	gx = x + BS_ROUND(float(p.m_dsp_val) * w / 0xFFFF);
	gy = y_bottom - BS_ROUND(float(p.m_phys_val - g_min_phys_val) * h / g_phys_val_interval);
}

void DlgCalibrNew::Draw(CDC& dc)
{
	CalcPhysInterval();
	w = g_w - 20*2, h = g_h - 20*2, x = g_x + 20, y_top = g_y + 20, y_bottom = g_y + g_h - 20;
	
	dc.FillSolidRect(g_x, g_y, g_w, g_h, 0x000000);
	dc.FillSolidRect(g_x+1, g_y+1, g_w-2, g_h-2, 0xFFFFFF);
	
	if (!g_table_ready)
		return;
		
	DWORD axes_color = 0x555555;
	DWORD cal_line_color = 0x000088;
	DWORD cal_point_color = 0x0000FF;
	DWORD cal_point_proj_color = 0xDDDDDD;
	CString str;
			
	int px, py;
	SERV_CalibrationTable::iterator current;

	current = g_current_cal_table.begin();
	while (current != g_current_cal_table.end())
	{
		GetXY(*current, px, py);
		dc.FillSolidRect(px, y_top, 1, h, cal_point_proj_color);
		dc.FillSolidRect(x+1, py, w, 1, cal_point_proj_color);
		current++;
	}
	if (!g_timeout)
	{
		SERV_CalibrationPoint p;
		p.m_phys_val = g_current_phys;
		p.m_dsp_val = g_current_dsp;
		GetXY(p, px, py);
		dc.FillSolidRect(x + 1, py, w, 1, 0x008800);
		if (!g_error_on_calculation)
			dc.FillSolidRect(px, y_top, 1, h, 0x008800);
	}
	dc.FillSolidRect(x, y_bottom, w+1, 1, axes_color);
	dc.FillSolidRect(x, y_bottom+1, 1, 2, axes_color);
	dc.FillSolidRect(x + w, y_bottom+1, 1, 2, axes_color);	
	dc.FillSolidRect(x, y_top, 1, h, axes_color);
	dc.FillSolidRect(x-2, y_top, 2, 1, axes_color);
	dc.FillSolidRect(x-2, y_bottom, 2, 1, axes_color);

	dc.FillSolidRect(x + w, y_top, 1, h, cal_point_proj_color);
	dc.FillSolidRect(x + 1, y_top, w, 1, cal_point_proj_color);

	dc.SelectObject(&g_horizontal_font);
	dc.SetTextColor(axes_color);
	str = g_table_list.GetSignalTxt();
	dc.TextOut(x + 1 + (w - dc.GetTextExtent(str).cx)/2, y_bottom + 1, str);
	dc.SetTextColor(0x000000);
	str = g_table_list.GetTextFirstCol(0);
	dc.TextOut(x + 1, y_bottom + 1, str);
	str = g_table_list.GetTextFirstCol(0xFFFF);
	dc.TextOut(x + w - dc.GetTextExtent(str).cx, y_bottom + 1, str);

	dc.SelectObject(&g_vertical_font);
	dc.SetTextColor(axes_color);
	str = "Значение (" + g_param->sRazmernPar + ")";
	dc.TextOut(x - 15, y_bottom - (h - dc.GetTextExtent(str).cx)/2 , str);
	dc.SetTextColor(0x000000);
	str = g_table_list.GetTextSecondCol(g_min_phys_val);
	dc.TextOut(x - 15, y_bottom, str);
	str = g_table_list.GetTextSecondCol(g_min_phys_val + g_phys_val_interval);
	dc.TextOut(x - 15, y_top + dc.GetTextExtent(str).cx, str);
	CPen cal_line_pen(PS_SOLID, 1, cal_line_color);
	CPen *prev_pen = dc.SelectObject(&cal_line_pen);	
	current = g_current_cal_table.begin();
	while (current != g_current_cal_table.end())
	{
		GetXY(*current, px, py);
		if (current == g_current_cal_table.begin())
			dc.MoveTo(px, py);
		else
			dc.LineTo(px, py);
		current++;
	}
	dc.SelectObject(prev_pen);	
	current = g_current_cal_table.begin();
	while (current != g_current_cal_table.end())
	{
		GetXY(*current, px, py);
		dc.FillSolidRect(px - 1, py - 1, 3, 3, cal_point_color);
		current++;
	}
	if (!g_timeout && !g_error_on_calculation)
	{
		SERV_CalibrationPoint p;
		p.m_phys_val = g_current_phys;
		p.m_dsp_val = g_current_dsp;
		GetXY(p, px, py);
		dc.FillSolidRect(px - 1, py - 1, 3, 3, 0x008800);
	}
}

void DlgCalibrNew::OnPaint() 
{
	CPaintDC dc(this);
	g_mem_dc.Init(dc, g_x, g_y, g_w, g_h);
	if (g_mem_dc.m_inited)
	{
		Draw(g_mem_dc.m_mem_dc);
		g_mem_dc.Blt(dc, g_x, g_y, g_w, g_h);
	}
}

LRESULT DlgCalibrNew::OnRebuilt(WPARAM, LPARAM)
{
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(g_current_cal_table.size() < 10);
	
	CListBox *list_box = (CListBox*)GetDlgItem(IDC_LIST_LOG);
	list_box->ResetContent();
	if (g_saved_cal_table.size() < 2)
	{
		list_box->AddString("Таблица содержит менее 2 точек");
	}
	else
	{
		SERV_CalibrationTable::iterator current = g_current_cal_table.begin(), next = current;
		next++;
		int num = 1;
		while (true)
		{
			if (current->m_phys_val > next->m_phys_val)
				list_box->AddString(CString(TXT("Значение в точке №%d меньше, чем в предыдущей") << num + 1));
			next++;
			if (next == g_current_cal_table.end())
				break;
			num++;
			current++;
		}	
	}
	if(list_box->GetCount() == 0)
		list_box->AddString("Таблица в порядке");

	return 0;
}

LRESULT DlgCalibrNew::OnReady(WPARAM, LPARAM)
{
	if (g_read_or_write)
		g_table_ready = true;
		
	if (g_dlg_waiting !=0 && ::IsWindow(g_dlg_waiting->m_hWnd))
		g_dlg_waiting->SignalToClose(IDOK);	
	KillTimer(1);
	ResetToDevice();
	//g_saved_cal_table = g_current_cal_table;
	return 0;
}

void DlgCalibrNew::ResetToDevice()
{
	g_channel->GetCalibrationTable(g_saved_cal_table);
	/*
	if (g_saved_cal_table.size() == 2)
	{
		SERV_CalibrationPoint p1 = g_saved_cal_table.front(), p2 = g_saved_cal_table.back();
		int d_dsp = p2.m_dsp_val - p1.m_dsp_val;
		int d_phys = p2.m_phys_val - p1.m_phys_val;
		//if (d_dsp != 0xFFFF || d_phys != 0xFFFF)
		{
			if (d_dsp > 3)
			{
				g_saved_cal_table.pop_back();
				int phys, dsp;
				dsp = BS_ROUND(p1.m_dsp_val + 0.333 * d_dsp);
				phys = BS_ROUND(p1.m_phys_val + 0.333 * d_phys);
				g_saved_cal_table.push_back(SERV_CalibrationPoint(dsp, phys));
				dsp = BS_ROUND(p1.m_dsp_val + 0.666 * d_dsp);
				phys = BS_ROUND(p1.m_phys_val + 0.666 * d_phys);
				g_saved_cal_table.push_back(SERV_CalibrationPoint(dsp, phys));
				g_saved_cal_table.push_back(p2);
			}
		}
	}
	else
	*/
	if (g_saved_cal_table.size() == 0)
	{
		g_saved_cal_table.push_back(SERV_CalibrationPoint(0, 0));
		g_saved_cal_table.push_back(SERV_CalibrationPoint(0xFFFF, 0xFFFF));
	}
	g_current_cal_table = g_saved_cal_table;
	RebuildTable();
}

void DlgCalibrNew::RebuildTable()
{
	g_table_list.m_k = g_param_k;
	g_table_list.m_table = &g_current_cal_table;
	g_table_list.Rebuild();
	CalcPhysInterval();
}

void DlgCalibrNew::OnBnClickedButtonAdd()
{
	CString str;
	SERV_CalibrationPoint p;
	float fl;

	if (GetDlgItemText(IDC_EDIT_SIGNAL, str) == 0 || sscanf(str, "%f", &fl) != 1)
	{
		MessageBox("Неправильный формат в поле \"Сигнал\"", " Точка не будет добавлена");
		return;
	}
	CString sss = BS_FloatWOZeros(fl, 3);
	char *ccc = sss.GetBuffer(1024);
	sscanf(ccc, "%f", &fl);
	p.m_dsp_val = g_table_list.GetDSPByFloat(fl);
	if (GetDlgItemText(IDC_EDIT_VALUE, str) == 0 || sscanf(str, "%f", &fl) != 1)
	{
		MessageBox("Неправильный формат в поле \"Значение\"", " Точка не будет добавлена");
		return;
	}
	p.m_phys_val = BS_ROUND(fl * g_param_k);
	SERV_CalibrationTable::iterator test_current = g_current_cal_table.begin();
	int counter = 1;
	while (test_current != g_current_cal_table.end())
	{
		if (test_current->m_dsp_val == p.m_dsp_val)
		{
			MessageBox(CString(TXT("Сигнал совпадает с сигналом в точке №%d")<<counter), " Точка не будет изменена");
			return;
		}
		counter++;
		test_current++;
	}

	SERV_CalibrationPoint first = g_current_cal_table.front();
	g_current_cal_table.pop_front();
	g_current_cal_table.push_front(p);
	g_current_cal_table.push_front(first);
	RebuildTable();
}

void DlgCalibrNew::OnBnClickedButtonReset()
{
	if (MessageBox("Хотите вернуться к таблице устройства? Все изменения будут потеряны.", " Восстановление таблицы устройства", MB_YESNO) == IDYES)
		ResetToDevice();
}

void DlgCalibrNew::OnBnClickedButtonSave()
{
	if (MessageBox("Хотите записать таблицу в устройство?", " Запись калибровочной таблицы", MB_YESNO) == IDYES)
	{
		g_channel->ResetCalibrationTable();
		SERV_CalibrationPoint cp;
		SERV_CalibrationTable::iterator current = g_current_cal_table.begin();
		while (current != g_current_cal_table.end())
		{
			cp = *current;
			g_channel->AddCalibrationPoint(SERV_CalibrationPoint(cp.m_dsp_val, cp.m_phys_val));
			current++;
		}
		g_saved_cal_table = g_current_cal_table;
		WriteTable();
	}
}

void DlgCalibrNew::OnBnClickedRadio1()
{
	g_table_list.m_col1_mode = TableList_MODE_DSP;
}

void DlgCalibrNew::OnBnClickedRadio2()
{
	g_table_list.m_col1_mode = TableList_MODE_PERCENT;
}

void DlgCalibrNew::OnBnClickedRadio3()
{
	g_table_list.m_col1_mode = TableList_MODE_VOLTAGE;
}

void DlgCalibrNew::OnBnClickedButtonClear()
{
    g_error_on_calculation = false;
	g_table_list.ClearTable();
}

void DlgCalibrNew::OnBnClickedButtonKeep()
{
	if (g_error_on_calculation)
		MessageBox("Ошибка в вычислении текущего Сигнала", " Текущие значения не могут быть сохранены");
	else
	if (g_timeout)
		MessageBox("Устройство не отвечает, данных нет", " Текущие значения не могут быть сохранены");
	else
	{
		CString str;
		GetDlgItemText(IDC_EDIT_VALUE_NOW, str);
		SetDlgItemText(IDC_EDIT_VALUE, str);
		GetDlgItemText(IDC_EDIT_SIGNAL_NOW, str);
		SetDlgItemText(IDC_EDIT_SIGNAL, str);
	}
}

void DlgCalibrNew::OnBnClickedButtonCfg()
{
	DlgEditSensor dlg(g_channel);
	if (dlg.DoModal() == IDOK)
	{
		SERV_current_device_cfg->Save();
		g_table_list.m_use_voltage = g_channel->m_use_voltage;
		g_table_list.m_v0 = g_channel->m_v0;
		g_table_list.m_voltage_k = (g_channel->m_v1 - g_channel->m_v0)/g_channel->m_dsp1;
	}
}

void DlgCalibrNew::OnBnClickedCheckCalcOuter()
{
	g_table_list.m_calc_outer_points = (bool)(IsDlgButtonChecked(IDC_CHECK_CALC_OUTER) == TRUE);
//	GetDlgItem(IDC_CHECK_SCALE)->EnableWindow(!g_table_list.m_calc_outer_points);
	g_table_list.Rebuild();
}

void DlgCalibrNew::OnBnClickedCheckScale()
{
	g_table_list.m_scaling = (bool)(IsDlgButtonChecked(IDC_CHECK_SCALE) == TRUE);
	g_table_list.Rebuild();
}
