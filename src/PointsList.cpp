// uic 19.07.2007

#include <StdAfx.h>
#include "PointsList.h"
#include "TXT.h"
#include "BS.h"
#include "MutexWrap.h"
#include "..\resource.h"
#include "..\DlgEditText.h"

#include "..\DlgArgPiecewise.h"

BEGIN_MESSAGE_MAP(PointsList, SimpleList)
	//{{AFX_MSG_MAP(PointsList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int PointsList_Num = 14;

static DWORD g_colors[6];
static int g_col1_x = 0, g_col2_x = 45, g_col3_x = 123;

static HICON g_icon_remove = NULL, g_icon_remove_menu;
PointsList::PointsList(int mode):
	SimpleList(200, PointsList_H, PointsList_Num, 1, (mode == 0)?15:0)
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

void PointsList::DrawCell(CDC* dc, int cell_pos, int cell_num, DWORD cell_data, int x, int y)
{
	int w = m_cell_w, h = m_cell_h;

	if (g_icon_remove == NULL)
	{
		g_icon_remove = AfxGetApp()->LoadIcon(IDI_ICON_REMOVE);
		g_icon_remove_menu = AfxGetApp()->LoadIcon(IDI_ICON_REMOVE_MENU);
	}

	dc->SetBkMode(TRANSPARENT);

	dc->FillSolidRect(x, y, w, h, g_colors[(cell_pos == 0)?2:((cell_num%2==1)?0:2)]);
	dc->FillSolidRect(x, y, 1, h, 0x000000);
	dc->FillSolidRect(x + w - 1, y, 1, h, 0x000000);
	dc->FillSolidRect(x + g_col2_x, y, 1, h, 0x000000);
	dc->FillSolidRect(x + g_col3_x, y, 1, h, 0x000000);
	if (cell_pos == 0)
		dc->FillSolidRect(x + 1, y, w - 2, 1, 0x000000);
	if (cell_pos == /*PointsList_Num*/m_num_visible_cells - 1 || cell_pos == 0)
		dc->FillSolidRect(x + 1, y + h - 1, w - 2, 1, 0x000000);

	if (cell_data == 0xFFFFFFFF && cell_pos != 0)
		return;

	HICON* icon;
	CString str1, str2, str3;
	if (cell_pos == 0)
	{
		str1 = "№";
		str2 = "Вход";
		str3 = "Выход";
		icon = &g_icon_remove_menu;
	}
	else
	{
		int counter = 0;
		map<float, float>::iterator current = m_table->m_table.begin();
		while (current != m_table->m_table.end() && counter < (int)cell_data)
		{
			current++;
			counter++;
		}
		if (current == m_table->m_table.end())
			return;
		str1.Format("%d", counter + 1);
//		str2.Format("%.1f", current->first);
		str2 = BS_FloatWOZeros(current->first, 3);
//		str3.Format("%.1f", current->second);
		str3 = BS_FloatWOZeros(current->second, 3);
		icon = &g_icon_remove;
	}
	dc->DrawIcon(x + 2, y + 2, *icon);
	CFont *prev_font = dc->SelectObject(m_font);
	dc->TextOut(x + 10 + 16, y + 3, str1);
	dc->TextOut(x + 10 + g_col2_x, y + 3, str2);
	dc->TextOut(x + 10 + g_col3_x, y + 3, str3);
	dc->SelectObject(prev_font);
}

void PointsList::Rebuild()
{
	m_data.clear();
	InsertCell(-1, 0xFFFFFFFF);
	int counter = 0;
	map<float, float>::iterator current = m_table->m_table.begin();
	while (current != m_table->m_table.end())
	{
		InsertCell(-1, counter);
		current++;
		counter++;
	}
}

void PointsList::OnLeftDown(int cell_pos, int cell_num, DWORD cell_data, int x, int y)
{
	bool remove = (x > 2 && x < 18 && y > 2 && y < 18);

	if (cell_data == 0xFFFFFFFF)
	{
		if (remove)
		{
			m_table->m_table.clear();
			Rebuild();
			DlgArgPiecewise_dlg->CheckComplete();
		}
		return;
	}

	if (x < g_col2_x && !remove)
		return;

	int counter = 0;
	map<float, float>::iterator current = m_table->m_table.begin();
	while (current != m_table->m_table.end() && counter < (int)cell_data)
	{
		current++;
		counter++;
	}
	if (current == m_table->m_table.end())
		return;

	if (remove)
	{
		m_table->m_table.erase(current);
		Rebuild();
		DlgArgPiecewise_dlg->CheckComplete();
		return;
	}

	CString str = BS_FloatWOZeros(x < g_col3_x ? current->first : current->second, 3);
	//str.Format("%.3f", x < g_col3_x ? current->first : current->second);
	int yy = cell_pos*PointsList_H;
	int xx = (x < g_col3_x)?g_col2_x:g_col3_x;
	CRect r(xx + 2, yy + 1, xx + (g_col3_x - g_col2_x) - 1, yy + PointsList_H - 1);
	ClientToScreen(&r);
	DlgEditText dlg(str, r, false, 0);
	if (dlg.DoModal() == IDOK)
	{
		str = dlg.GetText();
		str.Replace(",", ".");
		float fl;
		if (sscanf(str, "%f", &fl) != 1)
			MessageBox("Неправильный формат надписи", "Точка не будет изменёна");
		else
		{
			CString sss = BS_FloatWOZeros(fl, 3);
			char *ccc = sss.GetBuffer(1024);
			sscanf(ccc, "%f", &fl);

			if (x < g_col3_x)
			{
				float val = current->second;
				m_table->m_table.erase(current);
				m_table->m_table[fl] = val;
			}
			else
				current->second = fl;
			Rebuild();
		}
			
	}
}

void PointsList::OnLeftDouble(int cell_pos, int cell_num, DWORD cell_data, int x, int y)
{
	OnLeftDown(cell_pos, cell_num, cell_data, x, y);
}
////////////////////////////////////////////////////////////////////////////////
// end
