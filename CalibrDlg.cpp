// CalibrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "CalibrDlg.h"
#include "MainFrm.h"

#include <check.h>
#include "SERV.h"
#include <math.h>

#define USE_DEVICE true

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_READY (WM_USER + 1)

static CParam* g_param = NULL;
static SERV_Channel* g_channel = NULL;
static int g_param_k = 1;
static bool g_timed_out;

#define STATE_TIMEOUT (STATE_ERROR + 1)

static CDC g_mem_dc;
static CBitmap g_mem_bitmap;
static bool g_mem_inited = false;
static CPen g_red_pen(PS_SOLID, 1, 0x0044FF);
static CPen g_coord_pen(PS_DOT, 1, 0xDDDDDD);
static CPen g_black_pen(PS_SOLID, 1, DWORD(0x000000));

static float g_voltage_k;

#define MODE_VALUE 1
#define MODE_VOLTAGE_OR_PERCENTAGE 0

CString GetValStr(float val, int mode)
{
	if (mode == MODE_VALUE)
		return BS_FloatWOZeros(val / g_param_k, 3);

	CHECK(mode == MODE_VOLTAGE_OR_PERCENTAGE);

	if (!g_channel->m_use_voltage)
		return BS_FloatWOZeros((float)BS_ROUND(val*100 / 0xFFFF), 0);

	// Voltage
	float show_val = g_voltage_k * val + g_channel->m_v0;
	int prec = 3;
//--------------------------------------------------
//EVS 13.09.07
//    show_val = BS_ROUND( pow(10, prec) * show_val) * pow(10, -prec);
	show_val = BS_ROUND( pow(10, (float)prec) * show_val) * pow(10, (float)-prec);
	return BS_FloatWOZeros(show_val, prec);
}

CalibrDlg::CalibrDlg(HWND parent_hwnd, CParam* param, CWnd* pParent /*=NULL*/)
	: CDialog(CalibrDlg::IDD, pParent), m_parent_hwnd(parent_hwnd),
	m_calibration_enabled(false)
{
	g_param = param;
	m_current_point_color = 0x009900;
	//{{AFX_DATA_INIT(CalibrDlg)
	m_combo_point_num = 0;
	//}}AFX_DATA_INIT
}

void CalibrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CalibrDlg)
	DDX_Control(pDX, IDC_LIST_TABLE, m_table_list);
	DDX_Control(pDX, IDC_STATIC_WAIT_MSG, m_indicator_wait);
	DDX_CBIndex(pDX, IDC_COMBO_POINT, m_combo_point_num);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CalibrDlg, CDialog)
	//{{AFX_MSG_MAP(CalibrDlg)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, OnButtonReload)
	ON_BN_CLICKED(IDC_SAVE_TABLE, OnSaveTable)
	ON_BN_CLICKED(IDC_BUTTON_ZERO, OnButtonZero)
	ON_MESSAGE(WM_READY, OnReady)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CalibrDlg message handlers

static int g_w = 250, g_h = 250, g_x0 = -1, g_y0 = -1;

WORD tmp_phys = 1;

SERV_CalibrationTable::iterator CalibrDlg::CalcCurrentADSP()
{
	int channel_phys = (int)g_channel->GetLastValue();

	// по таблице, сохранённой в устройстве, определяем текущее значение АЦП
	m_current_dsp = 0xFFFF;
	SERV_CalibrationTable::iterator current;
	SERV_CalibrationPoint curr_p, prev_p;
	if (m_saved_cal_table.size() > 0)
	{
		current = m_saved_cal_table.begin();
		while (current != m_saved_cal_table.end())
		{
			curr_p = *current;
			if (current != m_saved_cal_table.begin() && 
				(
					(prev_p.m_phys_val >= channel_phys && channel_phys >= curr_p.m_phys_val)
					||
					(curr_p.m_phys_val >= channel_phys && channel_phys >= prev_p.m_phys_val))
				)
			{
				m_current_dsp = prev_p.m_dsp_val + BS_ROUND(float(channel_phys - prev_p.m_phys_val)*(curr_p.m_dsp_val - prev_p.m_dsp_val)/(curr_p.m_phys_val - prev_p.m_phys_val));
				// по новой таблице определяем новое физическое значение
				current = m_current_cal_table.begin();
				while (current != m_current_cal_table.end())
				{
					curr_p = *current;
					if (current != m_current_cal_table.begin() && 
						(
							(prev_p.m_dsp_val >= m_current_dsp && m_current_dsp >= curr_p.m_dsp_val)
							||
							(curr_p.m_dsp_val >= m_current_dsp && m_current_dsp >= prev_p.m_dsp_val))
						)
					{
						m_current_phys = (prev_p.m_phys_val + BS_ROUND(float(m_current_dsp - prev_p.m_dsp_val)*(curr_p.m_phys_val - prev_p.m_phys_val)/(curr_p.m_dsp_val - prev_p.m_dsp_val)));
						return current;
					}
					prev_p = curr_p;
					current++;
				}
				return m_current_cal_table.end();
			}
			prev_p = curr_p;
			current++;
		}
	}
	return m_current_cal_table.end();
}

void CalculateLinearSensor(WORD adsp1, WORD phys1, WORD adsp2, WORD phys2, WORD adsp_limit, WORD phys_limit, WORD &adsp_res, WORD &phys_res)
{
	float k = float(phys2 - phys1)/float(adsp2 - adsp1);
	float ph1 = phys_limit + k * (adsp1 - adsp_limit);
	int dir = (adsp1 < adsp2) ? +1 : -1;
	if (phys1 * dir < ph1 * dir)
	{
		phys_res = phys_limit;
		adsp_res = BS_ROUND(adsp2 - (phys2 - phys_limit)/k);
	}
	else
	{
		adsp_res = adsp_limit;
		phys_res = BS_ROUND(phys2 - (adsp2 - adsp_limit)*k);
	}
}

void CalibrDlg::OnButtonSet()
{
	if (m_current_cal_table.size() < 2 || m_saved_cal_table.size() < 2)
	{
		MessageBox("Ошибка программы", "Точка не будет установлена");
		return;
	}

	float desired_val_float;
	WORD desired_val;
	char str[1024];
	GetDlgItemText(IDC_EDIT_PHYS, str, 1024);

	if (sscanf(str, "%f", &desired_val_float) != 1)
	{
		MessageBox("Введено значение неизвестного формата", "Точка не будет установлена");
		return;
	}

	if (desired_val_float < g_param->fMin)
	{
		MessageBox("Требуемое значение меньше минимального", "Точка не будет установлена");
		return;
	}

	if (desired_val_float > g_param->fMax)
	{
		MessageBox("Требуемое значение больше максимального", "Точка не будет установлена");
		return;
	}

	SERV_CalibrationTable::iterator next = CalcCurrentADSP();
	if (next == m_current_cal_table.end())
	{
		MessageBox("Не определен текущий уровень сигнала", "Ошибка при калибровке");
		return;
	}

	if (m_current_cal_table.size() != 2)
	{
		MessageBox("Калибровка невозможна, таблица содержит более 2 точек", "Точка не будет установлена");
		return;
	}

	desired_val = BS_ROUND(desired_val_float * g_param_k);

	UpdateData();
	SERV_CalibrationTable::iterator ipoint = m_current_cal_table.begin();
	SERV_CalibrationPoint p1, p2;
	WORD desired_adsp = m_current_dsp, desired_phys = desired_val, adsp_res, phys_res;
	int res = m_combo_point_num + 1;
	if (res == 2)
	{
		p1 = *ipoint;
		CalculateLinearSensor(desired_adsp, desired_phys, p1.m_dsp_val, p1.m_phys_val, 0xFFFF, (WORD)(g_param->fMax * g_param_k), adsp_res, phys_res);
		m_current_cal_table.pop_back();
		m_current_cal_table.push_back(SERV_CalibrationPoint(adsp_res, phys_res));
	}
	else
	if (res == 1)
	{
		ipoint++;
		p2 = *ipoint;
		CalculateLinearSensor(desired_adsp, desired_phys, p2.m_dsp_val, p2.m_phys_val, 0, (WORD)(g_param->fMin * g_param_k), adsp_res, phys_res);
		m_current_cal_table.pop_front();
		m_current_cal_table.push_front(SERV_CalibrationPoint(adsp_res, phys_res));
	}
	m_current_phys = desired_val;

	RebuildTable();
	Invalidate(FALSE);
}

void CalibrDlg::RebuildTable()
{
	char buf[100];
	m_table_list.DeleteAllItems();
	WORD i = 0, col;
	SERV_CalibrationPoint cp;
	SERV_CalibrationTable::iterator current = m_current_cal_table.begin();
	while (current != m_current_cal_table.end())
	{
		cp = *current;
		sprintf(buf, "%d", i + 1);
		m_table_list.InsertItem(i, buf);
		col = 1;
		m_table_list.SetItemText(i, col++, GetValStr(cp.m_dsp_val, MODE_VOLTAGE_OR_PERCENTAGE) + " " + (g_channel->m_use_voltage ? "V" : "%"));
		m_table_list.SetItemText(i, col++, GetValStr(cp.m_phys_val, MODE_VALUE));
		current++;
		i++;
	}
}

void CalibrDlg::ResetToDevice()
{
	g_channel->GetCalibrationTable(m_saved_cal_table);
	if (m_saved_cal_table.size() == 0)
	{
		m_saved_cal_table.push_back(SERV_CalibrationPoint(0, (WORD)(g_param->fMin * g_param_k)));
		m_saved_cal_table.push_back(SERV_CalibrationPoint(0xFFFF, (WORD)(g_param->fMax * g_param_k)));
	}
	else
	{
		while (m_saved_cal_table.size() > 2)
		{
			SERV_CalibrationTable::iterator second = m_saved_cal_table.begin();
			second++;
			m_saved_cal_table.erase(second);
		}
	}
	m_current_cal_table = m_saved_cal_table;
	RebuildTable();
	Invalidate(FALSE);
}

LRESULT CalibrDlg::OnReady(WPARAM, LPARAM)
{
	KillTimer(1);
	m_indicator_wait.SetState(STATE_ON);

	SetDlgItemText(IDCANCEL, "Закрыть");
	GetDlgItem(IDC_SAVE_TABLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PHYS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RELOAD)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ZERO)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_POINT)->EnableWindow(TRUE);

	ResetToDevice();
	CalcCurrentADSP();
	EnableCalibration(true);
	return 0;
}

BOOL CalibrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SERV_calibration_indicator = &m_indicator_wait;

	// ПРОВЕРКИ >>
	g_channel = NULL;
	if (pFrm == NULL)
	{
		MessageBox("frm == NULL", "Калибровка невозможна");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	if (g_param == NULL)
	{
		MessageBox("g_param == NULL", "Калибровка невозможна");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	if (g_param->m_channel_num == -1)
	{
		MessageBox("калибруется неактивный или вычисляемый параметр", "Калибровка невозможна");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	if (g_param->fMin >= g_param->fMax)
	{
		MessageBox("минимальное значение параметра больше, либо равно максимальному", "Калибровка невозможна");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	SERV_DeviceCfg* cfg = (g_param->m_channel_conv_num == 0) ? &m_convertor_cfg : SERV_current_device_cfg;
	if (cfg == NULL)
	{
		MessageBox("cfg == NULL", "Калибровка невозможна");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	SERV_Channel* channel = cfg->GetChannel(g_param->m_channel_num);
	if (channel == NULL)
	{
		MessageBox("channel == NULL", "Калибровка невозможна");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	if (channel->GetDevice() == NULL)
	{
		MessageBox("channel->m_device == NULL", "Калибровка невозможна");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	if (channel->IsTimedOut())
	{
		MessageBox("Канал молчит", "Калибровка невозможна");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	g_channel = channel;

	g_param_k = 1;
	if (g_param->m_attr_channel_num != -1)
	{
		cfg = (g_param->m_attr_channel_conv_num == 0) ? &m_convertor_cfg : SERV_current_device_cfg;
		if (cfg == NULL)
		{
			MessageBox("cfg == NULL", "Калибровка невозможна");
			EndDialog(IDCANCEL);
			return FALSE;
		}
		channel = cfg->GetChannel(g_param->m_attr_channel_num);
		if (channel == NULL)
		{
			MessageBox("channel == NULL", "Калибровка невозможна");
			EndDialog(IDCANCEL);
			return FALSE;
		}
		if (channel->GetDevice() == NULL)
		{
			MessageBox("channel->m_device == NULL", "Калибровка невозможна");
			EndDialog(IDCANCEL);
			return FALSE;
		}
		if (channel->IsTimedOut())
		{
			MessageBox("Канал атрибут молчит", "Калибровка невозможна");
			EndDialog(IDCANCEL);
			return FALSE;
		}
		int pw = 0x03 & (WORD)channel->GetLastValue();
//----------------------------------------------
//EVS 13.09.07
		g_param_k = (int)pow(10, (float)pw);
//		g_param_k = pow(10, pw);
//----------------------------------------------
	}
	// ПРОВЕРКИ <<

	if (g_channel->m_use_voltage)
		g_voltage_k = (g_channel->m_v1 - g_channel->m_v0)/g_channel->m_dsp1;

/*
	g_time_start = (COleDateTime::GetCurrentTime()).m_dt;
	g_time_end	= g_time_start + 1.0/24;
	GetDlgItem(IDC_STATIC_GRAPH_AREA)->GetWindowRect(rect_graph);
*/

	SetDlgItemText(IDC_STATIC_MU, g_param->sRazmernPar);
	SetDlgItemText(IDC_STATIC_MU2, g_param->sRazmernPar);

	m_indicator_wait.SetState(STATE_ON);
	CString wait_text = "Идёт запрос данных от устройства...";
	m_indicator_wait.SetStateText(STATE_UNKNOWN, wait_text);
	m_indicator_wait.SetStateBlinking(STATE_UNKNOWN, true, 250, 0x0088FF, 0xFFFFFF, wait_text);
	m_indicator_wait.SetStateTextColor(STATE_UNKNOWN, 0x000000);
	m_indicator_wait.SetStateBkColor(STATE_UNKNOWN, 0x00FFFF);
#ifdef USE_DEVICE
	m_indicator_wait.SetState(STATE_UNKNOWN);
#else
	srand(GetTickCount());
	m_indicator_wait.SetState(STATE_ON);
#endif

	CString title = CString("Калибровка параметра \"") + g_param->sName + "\"";
	SetWindowText(title);
	m_indicator_wait.SetStateText(STATE_ON, "Гтовность");
	m_indicator_wait.SetStateTextColor(STATE_ON, 0x000000);
	m_indicator_wait.SetStateBkColor(STATE_ON, 0x00FF00);

	m_indicator_wait.SetStateText(STATE_ERROR, "ОШИБКА");
	m_indicator_wait.SetStateTextColor(STATE_ERROR, 0x00FFFF);
	m_indicator_wait.SetStateBkColor(STATE_ERROR, 0x0000FF);

	m_indicator_wait.SetStateText(STATE_TIMEOUT, "ОШИБКА: Устройство не отвечает");
	m_indicator_wait.SetStateTextColor(STATE_TIMEOUT, 0x00FFFF);
	m_indicator_wait.SetStateBkColor(STATE_TIMEOUT, 0x0000FF);

	SERV_connection_info.m_wnd = m_hWnd;
	SERV_connection_info.m_calibration_table_ready_msg = WM_READY;

#ifdef USE_DEVICE
	SERV_RequestCalibrationTable(g_channel, true);
	SetTimer(1, 60*1000, NULL);
#endif

	CFont *font = GetFont();
	LOGFONT log;
	font->GetLogFont(&log);
	m_fx.CreateFontIndirect(&log);
	log.lfEscapement = 900;
	log.lfOrientation = 900;
	m_fy.CreateFontIndirect(&log);

	m_table_list.SetExtendedStyle(LVS_EX_GRIDLINES);
	RECT r;
	m_table_list.GetClientRect(&r);
	int len_N = 20, len_perc = 40, len_other = (r.right - r.left - len_N - 0*len_perc) / 2;
	int col = 0;
	m_table_list.InsertColumn(col++, "N", LVCFMT_LEFT, len_N);
    m_table_list.InsertColumn(col++, (g_channel->m_use_voltage)?"Сигнал":"АЦП", LVCFMT_LEFT, len_other);
	//m_table_list.InsertColumn(col++, "%", LVCFMT_LEFT, len_perc);
	m_table_list.InsertColumn(col++, "Значение", LVCFMT_LEFT, len_other);
#ifndef USE_DEVICE
	m_current_phys = g_param->fMax * g_param_k;
	OnReady(0, 0);
#endif
	return TRUE;
}

BOOL CalibrDlg::DestroyWindow() 
{
	SERV_calibration_indicator = NULL;
	SERV_StopCalibration();
	SERV_connection_info.m_wnd = m_parent_hwnd;

	return CDialog::DestroyWindow();
}

byte g_prev_state = STATE_UNKNOWN;
void CalibrDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		KillTimer(1);
		m_indicator_wait.SetState(STATE_TIMEOUT);
		MessageBox("Устройство не отвечает. Калибровка невозможна", "Превышено время ожидания");
		EndDialog(IDCANCEL);
	}
	if (nIDEvent == 2)
	{
		CString dsp_val, phys_val;
		if (g_param->IsTimedOut())
		{
			dsp_val = "ОШИБКА";
			phys_val = "ОТКЛ";
			byte state = m_indicator_wait.GetState();
			if (state != STATE_TIMEOUT)
			{
				g_prev_state = state;
				m_indicator_wait.SetState(STATE_TIMEOUT);
			}
		}
		else
		{
			if (m_indicator_wait.GetState() == STATE_TIMEOUT)
				m_indicator_wait.SetState(g_prev_state);

			if (CalcCurrentADSP() == m_current_cal_table.end())
				dsp_val = "ОШИБКА";
			else
				dsp_val = GetValStr(m_current_dsp, MODE_VOLTAGE_OR_PERCENTAGE) + CString(" ") + CString(g_channel->m_use_voltage?"V":"%");
			phys_val = GetValStr(m_current_phys, MODE_VALUE);
		}
#ifndef USE_DEVICE
		SetDlgItemInt(IDC_STATIC_STAGE, tmp_phys);
#endif

		SetDlgItemText(IDC_STATIC_CURR_ADSP, dsp_val);
		SetDlgItemText(IDC_STATIC_CURR_VAL, phys_val);
//??		m_graf->AddPoint(m_current_phys);
		Invalidate(FALSE);
	}
	CDialog::OnTimer(nIDEvent);
}

int CalibrDlg::GraphX(int adsp)
{
	return g_x0 + g_w*adsp/0xFFFF;
}

int CalibrDlg::GraphY(int val)
{
	return (int)(g_y0 + g_h - g_h*(val - g_param->fMin * g_param_k) / ((g_param->fMax - g_param->fMin)* g_param_k));
}

list<WORD> g_x_labels, g_y_labels;

void CalibrDlg::DrawLabel(WORD val, CDC& dc, byte mode, int level, bool last_label)
{
	CString str = GetValStr(val, mode);
	if (last_label)
	{
		str += " ";
		if (mode == MODE_VOLTAGE_OR_PERCENTAGE)
		{
			str += (g_channel->m_use_voltage)?"V":"%";
		}
		else
			str += g_param->sRazmernPar;
	}
	int size = 2;
	bool curr_color;
	if (mode == MODE_VOLTAGE_OR_PERCENTAGE)
	{
		curr_color = (val == m_current_dsp);
		if (curr_color)
			dc.SetTextColor(m_current_point_color);
		dc.TextOut(GraphX(val), GraphY((int)g_param->fMin) + 3 + level, str);
		dc.FillSolidRect(GraphX(val), GraphY((int)g_param->fMin) + 1, 1, size, curr_color?m_current_point_color:0x000000);
		if (curr_color)
			dc.SetTextColor(0x000000);
	}
	else
	{
		curr_color = (val == m_current_phys);
		if (curr_color)
			dc.SetTextColor(m_current_point_color);
		dc.TextOut(GraphX(0) - 15 - level, GraphY(val), str);
		dc.FillSolidRect(GraphX(0) - size, GraphY(val), size, 1, curr_color?m_current_point_color:0x000000);
		if (curr_color)
			dc.SetTextColor(0x000000);
	}
}

struct LabStruct
{
	WORD m_value;
	int m_pos;
	byte m_level;
};

void CalibrDlg::DrawLabels(byte mode, CDC& dc)
{
	list<WORD>* label_list = &((mode == MODE_VOLTAGE_OR_PERCENTAGE)?g_x_labels:g_y_labels);
	map<byte, list<LabStruct> > lab_map;
	LabStruct lab;
	label_list->sort();
	list<WORD>::iterator current, prev;
	current = label_list->begin();
	while (current != label_list->end())
	{
		if (current != label_list->begin())
		{
//----------------------------------------------
//EVS 13.09.07
//			if (fabs(*prev - *current) < 1e-3)
			if (abs(*prev - *current) == 0)
//----------------------------------------------
				label_list->erase(prev);
		}
		prev = current;
		current++;
	}
	dc.SelectObject((mode == MODE_VOLTAGE_OR_PERCENTAGE)?m_fx:m_fy);

	CString str;
	CSize sz;

	current = label_list->begin();
	int number = 1;
	while (current != label_list->end())
	{
		lab.m_value = *current;
		str = GetValStr(lab.m_value, mode);
		sz = dc.GetTextExtent(str);
		lab.m_pos = (mode == MODE_VOLTAGE_OR_PERCENTAGE)?GraphX(lab.m_value):(g_y0 + g_h - GraphY(lab.m_value));
		lab.m_level = 0xFF;
		if (lab_map.size() > 0)
		{
			list<LabStruct>::reverse_iterator last_lab;
			map<byte, list<LabStruct> >::iterator curr_map_el = lab_map.begin();
			while (curr_map_el != lab_map.end())
			{
				if (curr_map_el->second.size() > 0)
				{
					last_lab = curr_map_el->second.rbegin();
					if (lab.m_pos > last_lab->m_pos)
					{
						lab.m_level = last_lab->m_level;
						break;
					}
				}
				curr_map_el++;
			}
		}
		if (lab.m_level == 0xFF)
			lab.m_level = lab_map.size();
		lab.m_pos += sz.cx + 5;

		lab_map[lab.m_level].push_back(lab);
		DrawLabel(lab.m_value, dc, mode, lab.m_level*11, number == label_list->size());
		current++;
		number++;
	}
}

void CalibrDlg::OnPaint() 
{
	CPaintDC pdc(this); // device context for painting

	if (g_param == NULL || g_channel == NULL)
		return;

	if (g_x0 == -1 && g_y0 == -1)
	{
		g_x0 = 50 + 8;
		g_y0 = 50 + 34;
	}

	if (!g_mem_inited)
	{
		g_mem_dc.CreateCompatibleDC(&pdc);
		g_mem_bitmap.CreateCompatibleBitmap(&pdc, g_x0 + g_w + 51, g_y0 + g_h + 51);
		g_mem_dc.SelectObject(&g_mem_bitmap);
		g_mem_dc.SetBkMode(TRANSPARENT);
		SetGraphicsMode(g_mem_dc.m_hDC, GM_ADVANCED);
		g_mem_inited = true;
	}


	g_x_labels.clear();
	g_y_labels.clear();
	g_x_labels.push_back(0);
	g_y_labels.push_back((WORD)(g_param->fMin * g_param_k));

	g_mem_dc.SelectObject(g_black_pen);
	g_mem_dc.Rectangle(g_x0 - 50, g_y0 - 50, g_x0 + g_w + 50, g_y0 + g_h + 50);

	g_mem_dc.FillSolidRect(g_x0, GraphY((int)(g_param->fMin * g_param_k)), g_w, 1, 0x000000);
	g_mem_dc.FillSolidRect(g_x0, GraphY((int)(g_param->fMax * g_param_k)), 1, g_h, 0x000000);

	SERV_CalibrationTable::iterator current;
	CPen current_pen(PS_DOT, 1, m_current_point_color);
	SERV_CalibrationPoint cp;

	if (m_current_cal_table.size() > 0)
	{
		// рисуем серые линии до осей
		g_mem_dc.SelectObject(g_coord_pen);
		current = m_current_cal_table.begin();
		while (current != m_current_cal_table.end())
		{
			cp = *current;
			if (cp.m_phys_val != g_param->fMin * g_param_k)
			{
				g_mem_dc.MoveTo(GraphX(0) + 1, GraphY(cp.m_phys_val));
				g_mem_dc.LineTo(GraphX(cp.m_dsp_val) - 2, GraphY(cp.m_phys_val));
			}
			if (cp.m_dsp_val != 0)
			{
				g_mem_dc.MoveTo(GraphX(cp.m_dsp_val), GraphY(cp.m_phys_val) + 2);
				g_mem_dc.LineTo(GraphX(cp.m_dsp_val), GraphY((int)(g_param->fMin * g_param_k)));
			}
			g_x_labels.push_back(cp.m_dsp_val);
			g_y_labels.push_back(cp.m_phys_val);
			current++;
		}
		// текущая точка
		g_mem_dc.SelectObject(current_pen);
		g_y_labels.push_back(m_current_phys);
		g_x_labels.push_back(m_current_dsp);
		if (m_current_phys > g_param->fMin * g_param_k)
		{
			g_mem_dc.MoveTo(GraphX(0) + 1, GraphY(m_current_phys));
			g_mem_dc.LineTo(GraphX(m_current_dsp) - 2, GraphY(m_current_phys));
		}
		if (m_current_dsp > 0)
		{
			g_mem_dc.MoveTo(GraphX(m_current_dsp), GraphY(m_current_phys) + 2);
			g_mem_dc.LineTo(GraphX(m_current_dsp), GraphY((int)(g_param->fMin * g_param_k)));
		}
	}
	g_x_labels.push_back(0xFFFF);
	g_y_labels.push_back((WORD)(g_param->fMax * g_param_k));
	// рисуем метки на осях
	DrawLabels(MODE_VOLTAGE_OR_PERCENTAGE, g_mem_dc);
	DrawLabels(MODE_VALUE, g_mem_dc);
	if (m_current_cal_table.size() > 0)
	{
		// рисуем ломанную
		g_mem_dc.SelectObject(g_red_pen);
		current = m_current_cal_table.begin();
		while (current != m_current_cal_table.end())
		{
			cp = *current;
			if (current == m_current_cal_table.begin())
				g_mem_dc.MoveTo(GraphX(cp.m_dsp_val), GraphY(cp.m_phys_val));
			else
				g_mem_dc.LineTo(GraphX(cp.m_dsp_val), GraphY(cp.m_phys_val));
			current++;
		}
		// рисуем точки
		current = m_current_cal_table.begin();
		while (current != m_current_cal_table.end())
		{
			cp = *current;
			g_mem_dc.FillSolidRect(GraphX(cp.m_dsp_val) - 1, GraphY(cp.m_phys_val) - 1, 3, 3, 0x0000CC);
			g_mem_dc.FillSolidRect(GraphX(cp.m_dsp_val), GraphY(cp.m_phys_val), 1, 1, 0x000099);
			current++;
		}
		// текущая точка
		g_mem_dc.FillSolidRect(GraphX(m_current_dsp) - 1, GraphY(m_current_phys) - 1, 3, 3, m_current_point_color);
		g_mem_dc.FillSolidRect(GraphX(m_current_dsp), GraphY(m_current_phys), 1, 1, 0x007700);
	}
	pdc.BitBlt(g_x0 - 50, g_y0 - 50, g_w + 101, g_h + 101, &g_mem_dc, g_x0 - 50, g_y0 - 50, SRCCOPY);
}

void CalibrDlg::EnableCalibration(bool ena)
{
	if (g_param == NULL || g_channel == NULL)
		return;

	if (ena == m_calibration_enabled)
		return;

	m_calibration_enabled = ena;
	GetDlgItem(IDC_BUTTON_SET)->EnableWindow(m_calibration_enabled);
	if (m_calibration_enabled)
	{
#ifdef USE_DEVICE
		SetTimer(2, 100, NULL);
#else
		SetTimer(2, 100, NULL);
#endif
	}
	else
	{
		KillTimer(2);
	}
}

void CalibrDlg::OnButtonReload() 
{
	if (g_param == NULL || g_channel == NULL)
		return;

#ifndef USE_DEVICE
	OnTimer(2);
	return;
#endif

	ResetToDevice();
	CalcCurrentADSP();
	Invalidate(FALSE);
	EnableCalibration(true);
}

void CalibrDlg::OnSaveTable() 
{
	if (g_param == NULL || g_channel == NULL)
		return;

#ifndef USE_DEVICE
	tmp_phys = BS_ROUND((g_param->fMax - g_param->fMin) * rand()/RAND_MAX);
	OnTimer(2);
	return;
#endif

	if (MessageBox("Вы уверены?", "Записать таблицу в устройство?", MB_YESNO) != IDYES)
		return;
	g_channel->ResetCalibrationTable();
	SERV_CalibrationPoint cp;
	SERV_CalibrationTable::iterator current = m_current_cal_table.begin();
	while (current != m_current_cal_table.end())
	{
		cp = *current;
		g_channel->AddCalibrationPoint(SERV_CalibrationPoint(cp.m_dsp_val, cp.m_phys_val));
		current++;
	}
	SERV_WriteCalibrationTable(g_channel);
	m_saved_cal_table = m_current_cal_table;
}

void CalibrDlg::OnButtonZero() 
{
	if (g_param == NULL || g_channel == NULL)
		return;

	m_saved_cal_table.clear();
	m_saved_cal_table.push_back(SERV_CalibrationPoint(0, (WORD)g_param->fMin));
	m_saved_cal_table.push_back(SERV_CalibrationPoint(0xFFFF, (WORD)g_param->fMax));
	m_current_cal_table = m_saved_cal_table;
	RebuildTable();
	Invalidate(FALSE);
	EnableCalibration(true);
}
