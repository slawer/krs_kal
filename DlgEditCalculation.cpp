#include "stdafx.h"
#include "krs.h"
#include "DlgEditCalculation.h"
#include "FormulaEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgEditCalculation::DlgEditCalculation(STAGES_CalculationTemplate* ct, CWnd* pParent /*=NULL*/):
	CDialog(DlgEditCalculation::IDD, pParent), m_calc_template(NULL), m_name("err"), m_formula(NULL), m_param(NULL), m_param_num(-1), m_db_time_interval(0), m_db_data_interval(0)
{
	SetupForCalculation(ct);
	//{{AFX_DATA_INIT(DlgEditCalculation)
	//}}AFX_DATA_INIT
}

DlgEditCalculation::~DlgEditCalculation()
{
}

void DlgEditCalculation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEditCalculation)
	DDX_Control(pDX, IDC_STATIC_COLOR, m_ind_color);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MIN_GR_VAL, m_min_graph);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MAX_GR_VAL, m_max_graph);
	//}}AFX_DATA_MAP

	// настройки БД >>
	if (m_param || m_has_graph || pDX->m_bSaveAndValidate)
	{
		if (GetDlgItem(IDC_EDIT_DB_VAL_INTERVAL)->IsWindowEnabled())
			KRS_DDX_TextDouble(pDX, IDC_EDIT_DB_VAL_INTERVAL, m_db_data_interval);
	}
	else
		DDX_Text(pDX, IDC_EDIT_DB_VAL_INTERVAL, CString(" - НЕТ ПАРАМЕТРА - "));

	double sec;
	if (!pDX->m_bSaveAndValidate)
	{
		if (m_param || m_has_graph)
			DDX_Text(pDX, IDC_EDIT_DB_TIME_INTERVAL, BS_GetTimeString(m_db_time_interval));
		else
			DDX_Text(pDX, IDC_EDIT_DB_TIME_INTERVAL, CString(" - НЕТ ПАРАМЕТРА - "));
	}
	else
	if (m_param || m_has_graph)
	{
		CString db_time_interval;
		DDX_Text(pDX, IDC_EDIT_DB_TIME_INTERVAL, db_time_interval);
		int h, m;
		if (sscanf(db_time_interval, "%d:%d:%lf", &h, &m, &sec) != 3)
		{
			MessageBox("Неверный формат интервала времени", "Ошибка в настройках БД", MB_OK);
			return;
		}
		if (h < 0 || h > 23)
		{
			MessageBox("Неверный формат интервала времени", "час < 0 или час > 23", MB_OK);
			return;
		}
		if (m < 0 || m > 59)
		{
			MessageBox("Неверный формат интервала времени", "мин < 0 или мин > 59", MB_OK);
			return;
		}
		if (sec < 0 || sec >= 60)
		{
			MessageBox("Неверный формат интервала времени", "сек < 0 или сек >= 60", MB_OK);
			return;
		}
		COleDateTimeSpan ts(0, h, m, int(sec));
		m_db_time_interval = ts.m_span + (sec - int(sec))/(24*60*60);
	}
	// настройки БД <<
}

BEGIN_MESSAGE_MAP(DlgEditCalculation, CDialog)
	//{{AFX_MSG_MAP(DlgEditCalculation)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE, OnButtonChoose)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_GRAPH, &DlgEditCalculation::OnBnClickedCheckGraph)
	ON_BN_CLICKED(IDC_CHECK_PRESET, &DlgEditCalculation::OnBnClickedCheckPreset)
	ON_BN_CLICKED(IDC_CHECK_MAX, &DlgEditCalculation::OnBnClickedCheckMax)
	ON_BN_CLICKED(IDC_CHECK_MIN, &DlgEditCalculation::OnBnClickedCheckMin)
	ON_BN_CLICKED(IDC_CHECK_MID, &DlgEditCalculation::OnBnClickedCheckMid)
	ON_BN_CLICKED(IDC_BUTTON_PLUS, &DlgEditCalculation::OnBnClickedButtonPlus)
	ON_BN_CLICKED(IDC_BUTTON_MINUS, &DlgEditCalculation::OnBnClickedButtonMinus)
	ON_STN_CLICKED(IDC_STATIC_COLOR, &DlgEditCalculation::OnStnClickedStaticColor)
END_MESSAGE_MAP()

void DlgEditCalculation::SetupForCalculation(STAGES_CalculationTemplate* ct)
{
	if (m_calc_template)
		delete m_calc_template;
	m_calc_template = ct;
	if (m_calc_template)
	{
		m_name = m_calc_template->m_name;
		if (m_calc_template->m_formula)
			m_formula = m_calc_template->m_formula->MakeCopy();
		m_has_graph = m_calc_template->m_has_graph;
		memcpy(m_show_column, m_calc_template->m_show_column, sizeof(m_calc_template->m_show_column));
		m_auto_graph_mode = m_has_graph ? (m_calc_template->m_auto_graph_mode%10) : 0;
		m_preset_value = m_calc_template->m_preset_value;
		m_line_width = m_calc_template->m_graph_line_width;
		m_min_graph = m_calc_template->m_graph_min_val;
		m_max_graph = m_calc_template->m_graph_max_val;
		m_line_color = m_calc_template->m_graph_color;
	}
}

void DlgEditCalculation::OnButtonChoose() 
{
	if (m_formula == NULL)
		m_formula = FORMULA_all_formulas[0]->MakeTemplate();

	FormulaEditDlg dlg(m_formula);
	if (dlg.DoModal() == IDOK)
		SetDlgItemText(IDC_EDIT_FORMULA, m_formula->GetText(FORMULA_MODE_RESULT));
	if (m_formula)
		GetDlgItem(IDOK)->EnableWindow(TRUE);

	UpdateBlockDB();
}

void DlgEditCalculation::UpdateBlockDB()
{
	m_param_num = m_formula ? m_formula->CorrespondingParamNum() : -1;
	m_MapParamCurent->Lookup(m_param_num, m_param);

	BOOL ena = m_param_num == -1 && m_has_graph;
	GetDlgItem(IDC_EDIT_DB_TIME_INTERVAL)->EnableWindow(ena);
	GetDlgItem(IDC_EDIT_DB_VAL_INTERVAL)->EnableWindow(ena);
	BOOL save_db = ((m_param != NULL && m_param->flDB) || m_has_graph)?TRUE:FALSE;
	CheckDlgButton(IDC_CHECK_DB, save_db);

	if (m_param)
	{
		m_db_time_interval = m_param->m_db_time_interval;
		m_db_data_interval = m_param->m_db_data_interval;
	}
	else
	{
		m_db_time_interval = m_calc_template->m_db_time_interval;
		m_db_data_interval = m_calc_template->m_db_data_interval;
	}
	UpdateData(FALSE);
}

BOOL DlgEditCalculation::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_EDIT_NAME, m_name);
	SetDlgItemText(IDC_EDIT_FORMULA, m_formula?m_formula->GetText(FORMULA_MODE_RESULT):"- не задана -");
	if (m_formula)
		GetDlgItem(IDOK)->EnableWindow(TRUE);

	UpdateBlockDB();

	CheckDlgButton(IDC_CHECK_GRAPH, m_has_graph);

	CheckDlgButton(IDC_CHECK_MAX, m_show_column[STAGES_COLUMN_MAX]);
	CheckDlgButton(IDC_CHECK_MIN, m_show_column[STAGES_COLUMN_MIN]);
	CheckDlgButton(IDC_CHECK_MID, m_show_column[STAGES_COLUMN_MID]);
	CheckDlgButton(IDC_CHECK_PRESET, m_show_column[STAGES_COLUMN_PRESET]);

	SetDlgItemText(IDC_EDIT_PRESET, CString(TXT("%.3f")<<m_preset_value));

	CButton* b;
	b = (CButton*)GetDlgItem(IDC_BUTTON_MINUS);
	b->SetIcon(KRS_app.LoadIcon(IDI_ICON_MINUS));
	b = (CButton*)GetDlgItem(IDC_BUTTON_PLUS);
	b->SetIcon(KRS_app.LoadIcon(IDI_ICON_PLUS));
	m_ind_color.SetStateBkColor(STATE_UNKNOWN, m_line_color);
	m_ind_color.SetStateText(STATE_UNKNOWN, "");
	m_ind_color.SetState(STATE_UNKNOWN);
	OutputThick();
	
	CheckDlgButton(IDC_CHECK_USE_PARAM_SETTINGS, m_calc_template->m_auto_graph_mode >= 10);

	return TRUE;
}

void DlgEditCalculation::OnCancel() 
{
	delete m_formula;	
	CDialog::OnCancel();
}

void DlgEditCalculation::OnOK() 
{
	CString str; 
	GetDlgItemText(IDC_EDIT_PRESET, str);
	str.Replace(",", ".");
	if (sscanf(str, "%f", &m_preset_value) != 1)
	{
		MessageBox("Неверный формат данных в поле \"Плановое значение\"", "Ошибка ввода");
		return;
	}

	UpdateData(TRUE);
	
	if (m_calc_template)
	{
		FORMULA_Formula* tmp = m_calc_template->m_formula;
		m_calc_template->m_formula = m_formula;
		delete tmp;
		m_calc_template->m_has_graph = m_has_graph;
		m_calc_template->m_auto_graph_mode = m_has_graph ? m_auto_graph_mode : 0;
		if (IsDlgButtonChecked(IDC_CHECK_USE_PARAM_SETTINGS))
			m_calc_template->m_auto_graph_mode += 10;
		memcpy(m_calc_template->m_show_column, m_show_column, sizeof(m_show_column));
		m_calc_template->m_preset_value = m_preset_value;
		m_calc_template->m_graph_line_width = m_line_width;
		m_calc_template->m_graph_min_val = m_min_graph;
		m_calc_template->m_graph_max_val = m_max_graph;
		m_calc_template->m_graph_color = m_line_color;
		m_calc_template->m_db_time_interval = m_db_time_interval;
		m_calc_template->m_db_data_interval = m_db_data_interval;
		GetDlgItemText(IDC_EDIT_NAME, m_calc_template->m_name);
	}
	
	CDialog::OnOK();
}

void DlgEditCalculation::OnBnClickedCheckGraph()
{
	m_has_graph = (IsDlgButtonChecked(IDC_CHECK_GRAPH) == TRUE);
	UpdateBlockDB();
//	m_combo_graph.EnableWindow(m_has_graph);
}

void DlgEditCalculation::OnBnClickedCheckPreset()
{
	m_show_column[STAGES_COLUMN_PRESET] = m_show_column[STAGES_COLUMN_DELTA] = (IsDlgButtonChecked(IDC_CHECK_PRESET) == TRUE);
}

void DlgEditCalculation::OnBnClickedCheckMax()
{
	m_show_column[STAGES_COLUMN_MAX] = (IsDlgButtonChecked(IDC_CHECK_MAX) == TRUE);
}

void DlgEditCalculation::OnBnClickedCheckMin()
{
	m_show_column[STAGES_COLUMN_MIN] = (IsDlgButtonChecked(IDC_CHECK_MIN) == TRUE);
}

void DlgEditCalculation::OnBnClickedCheckMid()
{
	m_show_column[STAGES_COLUMN_MID] = (IsDlgButtonChecked(IDC_CHECK_MID) == TRUE);
}

void DlgEditCalculation::OutputThick()
{
	SetDlgItemInt(IDC_STATIC_THICK_LINE, m_line_width);
}

void DlgEditCalculation::OnBnClickedButtonPlus()
{
	if (m_line_width >= 10)
		return;
	m_line_width++;
	OutputThick();
}

void DlgEditCalculation::OnBnClickedButtonMinus()
{
	if (m_line_width <= 1)
		return;
	m_line_width--;
	OutputThick();
}

void DlgEditCalculation::OnStnClickedStaticColor()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_line_color = dlg.GetColor();
		m_ind_color.SetStateBkColor(STATE_UNKNOWN, m_line_color);
	}
}