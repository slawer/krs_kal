#include <StdAfx.h>
#include "TableList.h"
#include "TXT.h"
#include "BS.h"
#include "MutexWrap.h"
#include "..\resource.h"
#include "..\DlgEditText.h"

BEGIN_MESSAGE_MAP(TableList, SimpleList)
	//{{AFX_MSG_MAP(TableList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int TableList_Num = 14;

static DWORD g_colors[6];
static int g_col1_x = 0, g_col2_x = 45, g_col3_x = 143;

static HICON g_icon_remove = NULL, g_icon_remove_menu, g_icon_remove_inactive;

struct GreaterPredicate : greater<SERV_CalibrationPoint>
{
	bool operator()(const SERV_CalibrationPoint& x, const SERV_CalibrationPoint& y) const
	{
		return x.m_dsp_val < y.m_dsp_val;
	}
};

GreaterPredicate g_greater_predicate;
TableList::TableList():
	SimpleList(200, TableList_H, TableList_Num, 1, 0), m_col1_mode(TableList_MODE_PERCENT),
	m_calc_outer_points(false), m_scaling(true)
{
	m_scroll_h_reduce = 0;

	g_colors[0] = GetSysColor(COLOR_MENU);
	int r = GetRValue(g_colors[0]), g = GetGValue(g_colors[0]), b = GetBValue(g_colors[0]);
	double k1 = 1.1, k2 = 0.95, k3 = 0.9, k4 = 0.6, k5 = 0.4;
	g_colors[1] = RGB(min(255,r*k1), min(255,g*k1), min(255,b*k1));
	g_colors[2] = RGB(r*k2, g*k2, b*k2);
	g_colors[3] = RGB(r*k3, g*k3, b*k3);
	g_colors[4] = RGB(r*k4, g*k4, b*k4);
	g_colors[5] = RGB(r*k5, g*k5, b*k5);
}

CString TableList::GetTextFirstCol(WORD dsp_val)
{
	if (m_col1_mode == TableList_MODE_VOLTAGE)
	{
		if (!m_use_voltage)
			return "не заданo";
		float show_val = m_voltage_k * dsp_val + m_v0;
		show_val = BS_ROUND( pow(10, 3.0f) * show_val) * pow(10, -3.0f);
		return BS_FloatWOZeros(show_val, 3);
	}
	if (m_col1_mode == TableList_MODE_PERCENT)
		return BS_FloatWOZeros(dsp_val*100.0/0xFFFF, 3);
	return TXT("%d")<<dsp_val;
}

CString TableList::GetTextSecondCol(WORD phys_val)
{
	return BS_FloatWOZeros(float(phys_val) / m_k, 3);
}

WORD TableList::GetDSPByFloat(float f)
{
	if (m_col1_mode == TableList_MODE_VOLTAGE)
	{
		if (!m_use_voltage)
			return 0;
		return BS_ROUND((f - m_v0)/m_voltage_k);
	}
	if (m_col1_mode == TableList_MODE_PERCENT)
		return (WORD)(f * 0.01 * 0xFFFF);
	return (WORD)f;	
}

CString TableList::GetSignalTxt()
{
	return TXT("Сигнал (%s)")<<(m_col1_mode == TableList_MODE_VOLTAGE ? "Вольт" : (m_col1_mode == TableList_MODE_PERCENT ? "%" : "ацп"));
}

void TableList::DrawCell(CDC* dc, int cell_pos, int cell_num, DWORD cell_data, int x, int y)
{
	int w = m_cell_w, h = m_cell_h;
	bool first_or_last = (cell_num != 0 && (cell_num == 1 || cell_num == m_data.size() - 1));
	
	if (g_icon_remove == NULL)
	{
		g_icon_remove = AfxGetApp()->LoadIcon(IDI_ICON_REMOVE);
		g_icon_remove_menu = AfxGetApp()->LoadIcon(IDI_ICON_REMOVE_MENU);
		g_icon_remove_inactive = AfxGetApp()->LoadIcon(IDI_ICON_REMOVE_INACTIVE);
	}

	dc->SetBkMode(TRANSPARENT);

	dc->FillSolidRect(x, y, w, h, g_colors[(cell_pos == 0)?2:((cell_num%2==1)?0:2)]);
	dc->FillSolidRect(x, y, 1, h, 0x000000);
	dc->FillSolidRect(x + w - 1, y, 1, h, 0x000000);
	dc->FillSolidRect(x + g_col2_x, y, 1, h, 0x000000);
	dc->FillSolidRect(x + g_col3_x, y, 1, h, 0x000000);
	if (cell_pos == 0)
		dc->FillSolidRect(x + 1, y, w - 2, 1, 0x000000);
	if (cell_pos == m_num_visible_cells - 1 || cell_pos == 0)
		dc->FillSolidRect(x + 1, y + h - 1, w - 2, 1, 0x000000);

	if (cell_data == 0xFFFFFFFF && cell_pos != 0)
		return;

	HICON* icon;
	CString str1, str2, str3;
	if (cell_pos == 0)
	{
		str1 = "№";
		str2 = GetSignalTxt();
		str3 = "Значение";
		icon = &g_icon_remove_menu;
	}
	else
	{
		int counter = 0;
		SERV_CalibrationTable::iterator current = m_table->begin();
		while (current != m_table->end() && counter < (int)cell_data)
		{
			current++;
			counter++;
		}
		if (current == m_table->end())
		{
			str2 = "err";
			str3 = "err";
		}
		else
		{
			str2 = GetTextFirstCol(current->m_dsp_val);
			str3 = GetTextSecondCol(current->m_phys_val);
		}
		str1.Format("%d", counter + 1);
		icon = (first_or_last)?(&g_icon_remove_inactive):(&g_icon_remove);
	}
	dc->DrawIcon(x + 2, y + 2, *icon);
	dc->SelectObject(m_font);
	dc->SetTextColor(first_or_last?0x666666:0x000000);
	dc->TextOut(x + 10 + 16, y + 3, str1);
	dc->TextOut(x + 10 + g_col2_x, y + 3, str2);
	dc->TextOut(x + 10 + g_col3_x, y + 3, str3);
}

void TableList::Rebuild()
{
	m_data.clear();
	InsertCell(-1, 0xFFFFFFFF);
	if (!m_table)
		return;
	SERV_CalibrationPoint first, last;
	if (m_table->size() > 2)
	{
		first = m_table->front(); m_table->pop_front();
		last = m_table->back(); m_table->pop_back();
		m_table->sort(g_greater_predicate);
		m_table->push_front(first);
		m_table->push_back(last);
	}
	if (m_calc_outer_points)
	{
		if (m_table->size() > 3)
		{	
			float dsp, phys;
			float a, b;

			SERV_CalibrationTable::iterator i1 = m_table->begin(), i2;
			i1++; i2 = i1; i2++; // второй и третий элемент списка
			a = float((int)i2->m_phys_val - i1->m_phys_val) / ((int)i2->m_dsp_val - i1->m_dsp_val);
			b = (int)i1->m_phys_val - a * i1->m_dsp_val;
			if (b < 0)
			{
				dsp = -b / a;
				phys = 0;
			}
			else
			if (b > 0xFFFF)
			{
				dsp = (0xFFFF - b) / a;
				phys = 0xFFFF;
			}
			else
			{
				dsp = 0;
				phys = b;
			}
			m_table->begin()->m_dsp_val = BS_ROUND(dsp);
			m_table->begin()->m_phys_val = BS_ROUND(phys);
			
			SERV_CalibrationTable::reverse_iterator r1 = m_table->rbegin(), r2;
			r1++; r2 = r1; r2++; 
			
			a = float((int)r2->m_phys_val - r1->m_phys_val) / ((int)r2->m_dsp_val - r1->m_dsp_val);
			b = (int)r1->m_phys_val - a * r1->m_dsp_val;
			
			dsp = 0xFFFF;
			phys = a*dsp + b;
			
			if (phys < 0)
			{
				dsp = -b / a;
				phys = 0;
			}
			else
			if (phys > 0xFFFF)
			{
				dsp = (0xFFFF - b) / a;
				phys = 0xFFFF;
			}
			
			m_table->rbegin()->m_dsp_val = BS_ROUND(dsp);
			m_table->rbegin()->m_phys_val = BS_ROUND(phys);
		}
	}
	else
	if (m_table->size() > 2)
	{
		m_table->begin()->m_dsp_val = 0;
		m_table->begin()->m_phys_val = 0;
		
		m_table->rbegin()->m_dsp_val = 0xFFFF;
		m_table->rbegin()->m_phys_val = 0xFFFF;
	}
	int counter = 0;
	SERV_CalibrationTable::iterator current = m_table->begin();
	while (current != m_table->end())
	{
		InsertCell(-1, counter);
		current++;
		counter++;
	}
	CWnd *parent = GetParent();
	if (parent)
	parent->PostMessage(m_rebuilt_message, 0, 0);
}

void TableList::ClearTable()
{
	if (MessageBox("Хотите очистить таблицу?", " Сброс таблицы", MB_YESNO) == IDYES)
	{
		m_table->clear();
		m_table->push_back(SERV_CalibrationPoint(0, 0));
		m_table->push_back(SERV_CalibrationPoint(0xFFFF, 0xFFFF));
		Rebuild();
	}
}

void TableList::OnLeftDown(int cell_pos, int cell_num, DWORD cell_data, int x, int y)
{
	bool remove = (x > 2 && x < 18 && y > 2 && y < 18);
	bool first_or_last = (cell_num != 0 && (cell_num == 1 || cell_num == m_data.size() - 1));
	SERV_CalibrationTable::iterator current;

	if (cell_data == 0xFFFFFFFF)
	{
		if (remove)
		{
			ClearTable();
			return;
		}
		if (x >= g_col2_x && x < g_col3_x)
		{
			m_col1_mode++;
			if (!m_use_voltage && m_col1_mode == TableList_MODE_VOLTAGE)
				m_col1_mode = TableList_MODE_DSP;
			if (m_col1_mode > TableList_MODE_VOLTAGE)
				m_col1_mode = TableList_MODE_DSP;
			Invalidate(FALSE);
		}
		return;
	}

	if (x < g_col2_x && !remove) 
		return;

	if (first_or_last) 
		return;

	int counter = 0;
	current = m_table->begin();
	while (current != m_table->end() && counter < (int)cell_data)
	{
		current++;
		counter++;
	}
	if (current == m_table->end())
		return;

	if (remove)
	{
		if (MessageBox("Хотите удалить точку?", " Удаление точки", MB_YESNO) == IDYES)
		{
			m_table->erase(current);
			Rebuild();
		}
		return;
	}

	bool first_col = x < g_col3_x;
	
	if (first_col && m_col1_mode == TableList_MODE_VOLTAGE && !m_use_voltage)
		return;
		
	CString str = first_col ? GetTextFirstCol(current->m_dsp_val) : GetTextSecondCol(current->m_phys_val);
	int yy = cell_pos*TableList_H;
	int xx = first_col?g_col2_x:g_col3_x;
	CRect r(xx + 2, yy + 1, xx + (first_col?(g_col3_x - g_col2_x):(m_cell_w - g_col3_x - 1)) - 1, yy + TableList_H - 1);
	ClientToScreen(&r);
	DlgEditText dlg(str, r, false, 0);
	if (dlg.DoModal() == IDOK)
	{
		str = dlg.GetText();
		str.Replace(",", ".");
		float fl;
		if (sscanf(str, "%f", &fl) != 1)
			MessageBox("Неправильный формат надписи", "Точка не будет изменена");
		else
		{
			CString sss = BS_FloatWOZeros(fl, 3);
			char *ccc = sss.GetBuffer(1024);
			sscanf(ccc, "%f", &fl);
			if (first_col)
			{
				WORD dsp = GetDSPByFloat(fl);
				SERV_CalibrationTable::iterator test_current = m_table->begin();
				int counter = 1;
				while (test_current != m_table->end())
				{
					if (test_current != current && test_current->m_dsp_val == dsp)
					{
						MessageBox(CString(TXT("Сигнал совпадает с сигналом в точке №%d")<<counter), "Точка не будет изменена");
						return;
					}
					counter++;
					test_current++;
				}
				current->m_dsp_val = dsp;
			}
			else
				current->m_phys_val = BS_ROUND(fl * m_k);
			Rebuild();
		}
	}
}

void TableList::OnLeftDouble(int cell_pos, int cell_num, DWORD cell_data, int x, int y)
{
	OnLeftDown(cell_pos, cell_num, cell_data, x, y);
}
////////////////////////////////////////////////////////////////////////////////
// end
