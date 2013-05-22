#include "stdafx.h"
#include "krs.h"
#include "BS.h"
#include "DlgArgPiecewise.h"
#include "FormulaEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgArgPiecewise* DlgArgPiecewise_dlg = NULL;

static CRect g_rect;
static int g_0_x = -1, g_0_y;
static float g_min_x, g_min_y, g_max_x, g_max_y;
static CPen g_red_pen(PS_SOLID, 1, 0x0044FF);
static CPen g_axes_pen(PS_SOLID, 1, 0x777777);
static CPen g_project_pen(PS_DOT, 1, 0xAAAAAA);
static CPen g_project_pen2(PS_DOT, 1, 0x00AA00);
static CFont g_font_x, g_font_y, *g_font = NULL;
static float g_in_x, g_in_y;

DlgArgPiecewise::DlgArgPiecewise(FORMULA_ArgumentPiecewise *arg, CWnd* pParent /*=NULL*/):
	CDialog(DlgArgPiecewise::IDD, pParent),
	m_arg(arg), m_formula(NULL)
{
	if (m_arg)
	{
		if (m_arg->m_formula)
		{
			m_formula = m_arg->m_formula->MakeCopy();
			m_table = m_arg->m_table;
		}
	}
	//{{AFX_DATA_INIT(DlgArgPiecewise)
	//}}AFX_DATA_INIT
}

void DlgArgPiecewise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgArgPiecewise)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgArgPiecewise, CDialog)
	//{{AFX_MSG_MAP(DlgArgPiecewise)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_EDIT_FORMULA, OnButtonEditFormula)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL DlgArgPiecewise::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DlgArgPiecewise_dlg = this;

	CWnd* static_rect = GetDlgItem(IDC_STATIC_RECT);
	static_rect->GetClientRect(&g_rect);
	static_rect->ClientToScreen(&g_rect);
	ScreenToClient(&g_rect);

	if (g_0_x == -1)
	{
		g_0_x = 40;
		g_0_y = 40;
		g_min_x = 15;
		g_max_x = 250;
		g_min_y = 10;
		g_max_y = 120;
	}
	if (m_table.m_table.size() == 0)
	{
		m_table.m_table[g_min_x] = 10;
		m_table.m_table[g_max_x] = 100;
	}

	if (g_font == NULL)
	
	{
		g_font = GetFont();
		LOGFONT log;
		g_font->GetLogFont(&log);
		g_font_x.CreateFontIndirect(&log);
		log.lfEscapement = 900;
		log.lfOrientation = 900;
		g_font_y.CreateFontIndirect(&log);
	}

	SetDlgItemText(IDC_EDIT_IN_FORMULA, ((m_formula == NULL) ? "- не задано -" : m_formula->GetText(FORMULA_MODE_RESULT)));
	//SetTimer(1, 100, NULL);

	m_points_list.Create(g_rect.right + 5, g_rect.top, this);
	m_points_list.m_table = &m_table;
	m_points_list.SetFont(&g_font_x);
	m_points_list.Rebuild();

	CheckComplete();

	return TRUE;
}

int GetX(float x)
{
	return (int)(g_rect.left + g_0_x + (g_rect.Width() - 2*g_0_x)*(x - g_min_x)/(g_max_x - g_min_x));
}

int GetY(float y)
{
	return (int)(g_rect.bottom - g_0_y - (g_rect.Height() - 2*g_0_y)*(y - g_min_y)/(g_max_y - g_min_y));
}

void Draw(CDC* dc)
{
	SetGraphicsMode(dc->m_hDC, GM_ADVANCED);

	map<float, float>* pmap = &(DlgArgPiecewise_dlg->m_table.m_table);
	map<float, float>::iterator current;

	g_min_x = (float)INT_MAX;
	g_max_x = (float)INT_MIN;
	g_min_y = (float)INT_MAX;
	g_max_y = (float)INT_MIN;
	current = pmap->begin();
	while(current != pmap->end())
	{
		if (current->first > g_max_x)
			g_max_x = current->first;
		if (current->first < g_min_x)
			g_min_x = current->first;
		if (current->second > g_max_y)
			g_max_y = current->second;
		if (current->second < g_min_y)
			g_min_y = current->second;
		current++;
	}

	dc->FillSolidRect(g_rect, 0xFFFFFF); 

	dc->SelectObject(g_project_pen);
	dc->MoveTo(GetX(g_min_x), GetY(g_max_y));
	dc->LineTo(GetX(g_max_x), GetY(g_max_y));
	dc->MoveTo(GetX(g_max_x), GetY(g_min_y));
	dc->LineTo(GetX(g_max_x), GetY(g_max_y));
	if (DlgArgPiecewise_dlg->m_formula != NULL)
	{
		dc->SelectObject(g_project_pen2);
		dc->MoveTo(GetX(g_in_x), GetY(g_min_y));
		dc->LineTo(GetX(g_in_x), GetY(g_in_y));
		dc->LineTo(GetX(g_min_x), GetY(g_in_y));
	}
	dc->SelectObject(g_axes_pen);
	dc->MoveTo(GetX(g_min_x), GetY(g_min_y));
	dc->LineTo(GetX(g_max_x), GetY(g_min_y));
	dc->MoveTo(GetX(g_min_x), GetY(g_min_y));
	dc->LineTo(GetX(g_min_x), GetY(g_max_y));
	dc->SelectObject(g_red_pen);
	current = pmap->begin();
	dc->MoveTo(GetX(current->first), GetY(current->second));
	current++;
	while(current != pmap->end())
	{
		dc->LineTo(GetX(current->first), GetY(current->second));
		current++;
	}
	current = pmap->begin();
	while(current != pmap->end())
	{
		dc->FillSolidRect(GetX(current->first) - 1, GetY(current->second) - 1, 3, 3, 0x0000FF);
		current++;
	}
	if (DlgArgPiecewise_dlg->m_formula != NULL)
		dc->FillSolidRect(GetX(g_in_x) - 1, GetY(g_in_y) - 1, 3, 3, 0x008800);
	dc->SetBkMode(TRANSPARENT);
	CSize sz;
	CString str;
	dc->SelectObject(&g_font_x);
	dc->SetTextColor(0x888888);
	str = BS_FloatWOZeros(g_min_x, 3);
	dc->TextOut(GetX(g_min_x), GetY(g_min_y) + 5, str);
	str = BS_FloatWOZeros(g_max_x, 3);
	sz = dc->GetTextExtent(str);
	dc->TextOut(GetX(g_max_x) - sz.cx, GetY(g_min_y) + 5, str);
	if (DlgArgPiecewise_dlg->m_formula != NULL)
	{
		dc->SetTextColor(0x008800);
		str = BS_FloatWOZeros(g_in_x, 3);
		sz = dc->GetTextExtent(str);
		dc->TextOut(GetX(g_in_x) - sz.cx/2, GetY(g_min_y) + 15, str);
		dc->SetTextColor(0x888888);
	}
	dc->SelectObject(&g_font_y);
	str = BS_FloatWOZeros(g_max_y, 3);
	sz = dc->GetTextExtent(str);
	dc->TextOut(GetX(g_min_x) - 17, GetY(g_max_y) + sz.cx, str);
	str = BS_FloatWOZeros(g_min_y, 3);
	dc->TextOut(GetX(g_min_x) - 17, GetY(g_min_y), str);
	if (DlgArgPiecewise_dlg->m_formula != NULL)
	{
		dc->SetTextColor(0x008800);
		str = BS_FloatWOZeros(g_in_y, 3);
		sz = dc->GetTextExtent(str);
		dc->TextOut(GetX(g_min_x) - 27, GetY(g_in_y) + sz.cx/2, str);
	}
}

void DlgArgPiecewise::OnPaint() 
{
	CPaintDC dc(this);
}

void DlgArgPiecewise::OnButtonEditFormula() 
{
	bool new_created = false;
	if (m_formula == NULL)
	{
		m_formula = FORMULA_all_formulas[0]->MakeTemplate();
		if (m_formula == NULL)
		{
			SetTimer(1, 100, NULL);
			return;
		}
		new_created = true;
	}
	FormulaEditDlg dlg(m_formula);
	if (dlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_IN_FORMULA, m_formula->GetText(FORMULA_MODE_RESULT));
	}
	else
	{
		if (new_created)
		{
			delete m_formula;
			m_formula = NULL;
		}
	}
	CheckComplete();
}

void DlgArgPiecewise::OnTimer(UINT nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}

bool DlgArgPiecewise::GetValue(float &fl, UINT id)
{
	CString str;
	GetDlgItemText(id, str);
	str.Replace(",", ".");
	if (sscanf(str, "%f", &fl) != 1)
	{
		MessageBox("Неправильный формат надписи", "Точка не будет изменёна");
		return false;
	}
	else
	{
		CString sss = BS_FloatWOZeros(fl, 3);
		char *ccc = sss.GetBuffer(1024);
		sscanf(ccc, "%f", &fl);
		return true;
	}
}

void DlgArgPiecewise::OnButtonAdd() 
{
	float x, y;
	if (!GetValue(x, IDC_EDIT_IN) || !GetValue(y, IDC_EDIT_OUT))
		return;
	m_table.m_table[x] = y;
	m_points_list.Rebuild();
	CheckComplete();
}

void DlgArgPiecewise::CheckComplete()
{
	bool complete = m_table.m_table.size() >= 2;
	if (m_formula == NULL)
		complete = false;
	GetDlgItem(IDOK)->EnableWindow(complete);
}

void DlgArgPiecewise::OnCancel() 
{
	if (m_formula)
	{
		delete m_formula;
		m_formula = NULL;
	}
	CDialog::OnCancel();
}

void DlgArgPiecewise::OnOK() 
{
	if (m_arg)
	{
		if (m_arg->m_formula)
			delete m_arg->m_formula;
		m_arg->m_formula = m_formula;
		m_arg->m_table = m_table;
	}
	CDialog::OnOK();
}