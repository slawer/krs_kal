// NewCorrespondenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "BS.h"
#include "SERV.h"
#include "MainFrm.h"
#include "NewCorrespondenceDlg.h"
#include "DlgSelectChannel.h"
#include "FormulaEditDlg.h"
#include <check.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

NewCorrespondenceDlg::NewCorrespondenceDlg(CParam* param, bool new_param, CWnd* pParent /*=NULL*/)
	: CDialog(NewCorrespondenceDlg::IDD, pParent), m_param(param), m_new_param(new_param)
{
	m_param_type = m_param->m_nTypePar;
	m_frame = (CMainFrame*)AfxGetMainWnd();
	m_num_par = m_param->m_num_par;
	m_name = _T(m_param->sName);
	m_mu = _T(m_param->sRazmernPar);
	m_min_val = 0.0f;
	m_max_val = 0.0f;
	m_min_val_gr = 0.0f;
	m_max_val_gr = 0.0f;
	m_block_val = 0.0f;
	m_crush_val = 0.0f;
	m_file1_path = _T(m_param->sFile1);
	m_file2_path = _T(m_param->sFile2);
	m_file3_path = _T(m_param->sFile3);
	m_name_grf = _T(m_param->sGraphName);
	m_store_to_db = m_param->flDB;
	m_active = m_param->bParamActive;
	m_interval_average = m_param->timeAvrg * 24 * 3600;

	m_min_val = m_param->fMin;
	m_max_val = m_param->fMax;
	m_block_val = m_param->fBlock;
	m_crush_val = m_param->fAvaria;
	if (m_param->m_shown_digits == 0xFF)
	{
		COleDateTimeSpan odts_min((double)m_param->fGraphMIN);
		m_min_val_gr = odts_min.GetTotalMinutes();
		COleDateTimeSpan odts_max((double)m_param->fGraphMAX);
		m_max_val_gr = odts_max.GetTotalMinutes();
	}
	else
	{
		m_min_val_gr = m_param->fGraphMIN;
		m_max_val_gr = m_param->fGraphMAX;
	}

	m_channel_num = m_param->m_channel_num;
	m_channel_conv = m_param->m_channel_conv_num;

	m_attr_channel_num = m_param->m_attr_channel_num;
	m_attr_channel_conv = m_param->m_attr_channel_conv_num;

	m_db_time_interval = m_param->m_db_time_interval;
	m_db_data_interval = m_param->m_db_data_interval;

	if (m_param->m_formula)
		m_formula = m_param->m_formula->MakeCopy();
	else
		m_formula = NULL;

	m_line_width = m_param->m_line_width;
	m_digits = m_param->m_shown_digits;

	m_line_color = m_param->m_color;
}

void NewCorrespondenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_PARAM_TYPE, m_combo_param_type);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_ind_color);
	DDX_Control(pDX, IDC_COMBO_CONTROL_TYPE, m_combo_control_type);
	DDX_Text(pDX, IDC_EDIT_PARAM_NUM, m_num_par);
	DDX_Text(pDX, IDC_EDIT_PARAM_NAME, m_name);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MIN_VAL, m_min_val);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MAX_VAL, m_max_val);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MIN_GR_VAL, m_min_val_gr);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MAX_GR_VAL, m_max_val_gr);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_BLOCK_VAL, m_block_val);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_CRUSH_VAL, m_crush_val);
	DDX_Text(pDX, IDC_EDIT_FILE1, m_file1_path);
	DDX_Text(pDX, IDC_EDIT_FILE2, m_file2_path);
	DDX_Text(pDX, IDC_EDIT_FILE3, m_file3_path);
	DDX_Text(pDX, IDC_EDIT_PARAM_NAME_GRF, m_name_grf);
	DDX_Check(pDX, IDC_CHECK_ACTIVE, m_active);
	DDX_Check(pDX, IDC_CHECK_DB, m_store_to_db);
	DDX_Text(pDX, IDC_EDIT_MU, m_mu);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_TIME_AVERAGE, m_interval_average);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_DB_VAL_INTERVAL, m_db_data_interval);
	CString db_time_interval;
	double sec;
	if (!pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_EDIT_DB_TIME_INTERVAL, BS_GetTimeString(m_db_time_interval));
	}
	else
	{
		DDX_Text(pDX, IDC_EDIT_DB_TIME_INTERVAL, db_time_interval);
		db_time_interval.Replace(',', '.');
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
}

BEGIN_MESSAGE_MAP(NewCorrespondenceDlg, CDialog)
	//{{AFX_MSG_MAP(NewCorrespondenceDlg)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE, OnButtonChoose)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_NOTHING, OnButtonChooseNothing)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE1, OnButtonBrowse1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE2, OnButtonBrowse2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE3, OnButtonBrowse3)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_ATTR, OnButtonChooseAttr)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_NOTHING_ATTR, OnButtonChooseNothingAttr)
	ON_BN_CLICKED(IDC_BUTTON_PLUS, OnButtonPlus)
	ON_BN_CLICKED(IDC_BUTTON_MINUS, OnButtonMinus)
	ON_BN_CLICKED(IDC_STATIC_COLOR, OnStaticColor)
	ON_CBN_SELCHANGE(IDC_COMBO_PARAM_TYPE, OnComboParamType)
	ON_BN_CLICKED(IDC_BUTTON_PLUS2, OnButtonPlus2)
	ON_BN_CLICKED(IDC_BUTTON_MINUS2, OnButtonMinus2)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_CHECK_ACTIVE, &NewCorrespondenceDlg::OnBnClickedCheckActive)
	ON_BN_CLICKED(IDC_CHECK_DB, &NewCorrespondenceDlg::OnBnClickedCheckDb)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR1, &NewCorrespondenceDlg::OnBnClickedButtonClear1)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR2, &NewCorrespondenceDlg::OnBnClickedButtonClear2)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR3, &NewCorrespondenceDlg::OnBnClickedButtonClear3)
END_MESSAGE_MAP()

void NewCorrespondenceDlg::SetEditReadonly(UINT id, bool readonly)
{
	CEdit* edit = (CEdit*)GetDlgItem(id);
	edit->SetReadOnly(readonly);
}

BOOL NewCorrespondenceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateParamInfo();

	SetDlgItemText(IDC_EDIT_MU, m_param->sRazmernPar);
	SetDlgItemText(IDC_STATIC_DB_MU, m_param->sRazmernPar);

	m_combo_control_type.SetCurSel(m_param->nKodControl);

	if (!m_new_param) 
		SetEditReadonly(IDC_EDIT_PARAM_NUM, true); 

	HICON icon;
	CButton* b;
	icon = KRS_app.LoadIcon(IDI_ICON_MINUS);
	b = (CButton*)GetDlgItem(IDC_BUTTON_MINUS);
	b->SetIcon(icon);
	b = (CButton*)GetDlgItem(IDC_BUTTON_MINUS2);
	if (m_param->m_shown_digits == 0xFF)
		b->ShowWindow(SW_HIDE);
	else
		b->SetIcon(icon);
	icon = KRS_app.LoadIcon(IDI_ICON_PLUS);
	b = (CButton*)GetDlgItem(IDC_BUTTON_PLUS);
	b->SetIcon(icon);
	b = (CButton*)GetDlgItem(IDC_BUTTON_PLUS2);
	if (m_param->m_shown_digits == 0xFF)
		b->ShowWindow(SW_HIDE);
	else
		b->SetIcon(icon);
	if (m_digits == 0xFF)
	{
		GetDlgItem(IDC_STATIC_DIGITS)->ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_STATIC_DIGITS_TEXT, "Тип параметра - время");
	}

	m_ind_color.SetStateBkColor(STATE_UNKNOWN, m_line_color);
	m_ind_color.SetStateText(STATE_UNKNOWN, "");
	m_ind_color.SetState(STATE_UNKNOWN);
	OutputThick();
	OutputDigits();

	UpdateData(FALSE);
	
	OnBnClickedCheckDb();

	return TRUE;
}

void NewCorrespondenceDlg::OnOK() 
{
	UpdateData();

	if (m_new_param)
	{
		if (m_num_par == 0)
		{
			CString str;
			str.Format("Введите число больше %d", MAX_PARAM);
			MessageBox(str, "Параметр не может быть создан");
			return;
		}

		if (m_num_par <= MAX_PARAM)
		{
			CString str;
			str.Format("Номера параметров от 1 до %d зарезервированы", MAX_PARAM);
			MessageBox(str, "Параметр не может быть создан");
			return;
		}

		CParam* param;
		if (m_MapParamDefault.Lookup(m_num_par, param))
		{
			MessageBox("Номер параметра не уникален", "Параметр не может быть создан");
			return;
		}
	}
	m_param->sName = m_name;
	m_param->sRazmernPar = m_mu;
	m_param->sGraphName = m_name_grf;
	m_param->sNickName = m_name_grf;
	m_param->m_num_par = m_num_par;
	m_param->timeAvrg = m_interval_average / (24*3600);

	m_param->m_db_time_interval = m_db_time_interval;
	m_param->m_db_data_interval = m_db_data_interval;
	
	m_param->m_nTypePar = m_param_type;
	if (m_param_type == PARAM_TYPE_FORMULA)
	{
		FORMULA_Formula* tmp = m_param->m_formula;
		if (m_formula)
		{
			m_param->m_formula = m_formula->MakeCopy();
			delete m_formula;
		}
		else
			m_param->m_formula = NULL;
		if (tmp)
			delete tmp;

		m_param->m_channel_num = -1;
		m_param->m_channel_conv_num = 0;
		m_param->m_attr_channel_num = -1;
		m_param->m_attr_channel_conv_num = 0;
	}
	else
	{
		bool eq = (m_formula == m_param->m_formula);
		if (m_formula)
			delete m_formula;
		if (m_param->m_formula && !eq)
			delete m_param->m_formula;
		m_param->m_formula = NULL;

		m_param->m_channel_num = m_channel_num;
		m_param->m_channel_conv_num = m_channel_conv;
		m_param->m_attr_channel_num = m_attr_channel_num;
		m_param->m_attr_channel_conv_num = m_attr_channel_conv;
	}
	m_param->bParamActive = m_active;
	m_param->nKodControl = m_combo_control_type.GetCurSel();
	m_param->fMin = m_min_val;
	m_param->fMax = m_max_val;
	m_param->fBlock = m_block_val;
	m_param->fAvaria = m_crush_val;
	if (m_param->m_shown_digits == 0xFF)
	{
		COleDateTimeSpan odts_min(0, 0, m_min_val_gr, 0);
		m_param->fGraphMIN = odts_min.m_span;
		COleDateTimeSpan odts_max(0, 0, m_max_val_gr, 0);
		m_param->fGraphMAX = odts_max.m_span;
	}
	else
	{
		m_param->fGraphMIN = m_min_val_gr;
		m_param->fGraphMAX = m_max_val_gr;
	}
	m_param->sFile1 = m_file1_path;
	m_param->sFile2 = m_file2_path;
	m_param->sFile3 = m_file3_path;
	m_param->flDB = m_store_to_db;
	m_param->m_color = m_line_color;
	m_param->m_line_width = m_line_width;
	m_param->m_shown_digits = m_digits;

	KRS_app.SetIniFileMode(PRJ_GetWorkPath() + "\\Параметры.ini");
	KRS_app.WriteParamToRegistry(m_param);
	KRS_app.SetRegistryMode(true);

	CDialog::OnOK();
}

void NewCorrespondenceDlg::OnButtonChoose() 
{
	if (m_param_type == PARAM_TYPE_FORMULA)
	{
		bool created = false;
		if (m_formula == NULL)
		{
			m_formula = FORMULA_all_formulas[0]->MakeTemplate();
			created = true;
		}
		FormulaEditDlg dlg(m_formula);
		if (dlg.DoModal() == IDCANCEL)
		{
			if (created)
			{
				delete m_formula;
				m_formula = NULL;
			}
			return;
		}
	}
	else
	{
		DlgSelectChannel dlg("Выберите канал", true);

		dlg.m_convertor_id = m_channel_conv;
		dlg.m_channel_id = m_channel_num;

		if (dlg.DoModal() == IDCANCEL)
			return;

		m_channel_num = dlg.m_channel_id;
		m_channel_conv = dlg.m_convertor_id;
	}
	UpdateParamInfo();
}

void NewCorrespondenceDlg::OnButtonChooseNothing()
{
	if (m_param_type == PARAM_TYPE_FORMULA)
	{
		if (!m_formula)
			return;
		FORMULA_Formula* tmp = m_formula;
		m_formula = NULL;
		delete tmp;
	}
	else
	{
		m_channel_num = -1;
		m_channel_conv = 0;
	}
	UpdateParamInfo();
}

void NewCorrespondenceDlg::OnButtonChooseAttr() 
{
	DlgSelectChannel dlg("Выберите канал атрибута", true);

	dlg.m_convertor_id = m_attr_channel_conv;
	dlg.m_channel_id = m_attr_channel_num;

	if (dlg.DoModal() == IDCANCEL)
		return;

	SERV_DeviceCfg* cfg = (dlg.m_convertor_id == 0)?&m_convertor_cfg:SERV_current_device_cfg;
	if (cfg != NULL)
	{
		SERV_Channel* channel = cfg->GetChannel(dlg.m_channel_id);
		if (channel == NULL)
			return;
		SetDlgItemText(IDC_EDIT_CHANNEL_ATTR, channel->GetInfo());
	}
	m_attr_channel_num = dlg.m_channel_id;
	m_attr_channel_conv = dlg.m_convertor_id;
}

void NewCorrespondenceDlg::OnButtonChooseNothingAttr() 
{
	m_attr_channel_num = -1;
	m_attr_channel_conv = 0;
	SetDlgItemText(IDC_EDIT_CHANNEL_ATTR, BS_GetChannelName(NULL));
}

void NewCorrespondenceDlg::BrowseFile(CString &path)
{
	CFileDialog dlg(TRUE, NULL, path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Все файлы (*.*)|*.*||");
	if (dlg.DoModal() != IDOK)
		return;
	path = dlg.GetPathName();
	UpdateData(FALSE);
}

void NewCorrespondenceDlg::OnButtonBrowse1() 
{
	BrowseFile(m_file1_path);
}

void NewCorrespondenceDlg::OnButtonBrowse2() 
{
	BrowseFile(m_file2_path);
}

void NewCorrespondenceDlg::OnButtonBrowse3() 
{
	BrowseFile(m_file3_path);
}

void NewCorrespondenceDlg::OnButtonPlus() 
{
	if (m_line_width >= 10)
		return;
	m_line_width++;
	OutputThick();
}

void NewCorrespondenceDlg::OnButtonMinus() 
{
	if (m_line_width <= 1)
		return;
	m_line_width--;
	OutputThick();
}

void NewCorrespondenceDlg::OutputThick()
{
	SetDlgItemInt(IDC_STATIC_THICK_LINE, m_line_width);
}

void NewCorrespondenceDlg::OnButtonPlus2() 
{
	if (m_digits >= 6)
		return;
	m_digits++;
	OutputDigits();	
}

void NewCorrespondenceDlg::OnButtonMinus2() 
{
	if (m_digits <= 0)
		return;
	m_digits--;
	OutputDigits();
}

void NewCorrespondenceDlg::OutputDigits()
{
	SetDlgItemInt(IDC_STATIC_DIGITS, m_digits);
}

void NewCorrespondenceDlg::OnStaticColor() 
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_line_color = dlg.GetColor();
		m_ind_color.SetStateBkColor(STATE_UNKNOWN, m_line_color);
	}	
}

void NewCorrespondenceDlg::SetChannelText(UINT id, int conv_num, int channel_num)
{
	SERV_DeviceCfg* cfg = (conv_num == 0)?&m_convertor_cfg:SERV_current_device_cfg;
	if (cfg == NULL)
	{
		SetDlgItemText(id, "cfg == NULL");
		return;
	}
	SERV_Channel* channel = cfg->GetChannel(channel_num);
	SetDlgItemText(id, (channel == NULL) ? BS_GetChannelName(NULL) : channel->GetInfo());
}

void NewCorrespondenceDlg::UpdateParamInfo()
{
	BOOL on, add_on;
	if (m_param_type == PARAM_TYPE_CALCULATED)
	{
		m_combo_param_type.ResetContent();
		m_combo_param_type.AddString("Внутренние вычисления");
		m_combo_param_type.SetCurSel(0);
		on = FALSE; // SW_HIDE
		add_on = FALSE;
	}
	else
	{
		m_combo_param_type.ResetContent();
		m_combo_param_type.AddString("Каналы");
		m_combo_param_type.AddString("Формула");
		m_combo_param_type.SetCurSel(m_param_type - 1);
		SetDlgItemText(IDC_STATIC_CHANNEL, (m_param_type == PARAM_TYPE_CHANNEL) ? "Основной канал:" : "Шаблон формулы:");
		SetDlgItemText(IDC_STATIC_CHANNEL_ATTR, (m_param_type == PARAM_TYPE_CHANNEL) ? "Канал атрибута:" : "Формула:");
		if (m_param_type == PARAM_TYPE_CHANNEL)
			SetChannelText(IDC_EDIT_CHANNEL, m_channel_conv, m_channel_num);
		else
		if (m_param_type == PARAM_TYPE_FORMULA)
			SetDlgItemText(IDC_EDIT_CHANNEL, (m_formula==NULL) ? "- формула не указана - " : m_formula->GetText(FORMULA_MODE_TEMPLATE));
		if (m_param_type == PARAM_TYPE_CHANNEL)
			SetChannelText(IDC_EDIT_CHANNEL_ATTR, m_attr_channel_conv, m_attr_channel_num);
		else
		if (m_param_type == PARAM_TYPE_FORMULA)
			SetDlgItemText(IDC_EDIT_CHANNEL_ATTR, (m_formula==NULL) ? "- формула не указана - " : m_formula->GetText(FORMULA_MODE_RESULT));
		on = TRUE; // SW_NORMAL
		add_on = (m_param_type == PARAM_TYPE_FORMULA)?FALSE:TRUE;
	}
	m_combo_param_type.EnableWindow(on);
	GetDlgItem(IDC_STATIC_CHANNEL)->ShowWindow(on);
	GetDlgItem(IDC_STATIC_CHANNEL_ATTR)->ShowWindow(on);
	GetDlgItem(IDC_EDIT_CHANNEL)->ShowWindow(on);
	GetDlgItem(IDC_EDIT_CHANNEL_ATTR)->ShowWindow(on);
	GetDlgItem(IDC_BUTTON_CHOOSE)->ShowWindow(on);
	GetDlgItem(IDC_BUTTON_CHOOSE_NOTHING)->ShowWindow(on);
	GetDlgItem(IDC_BUTTON_CHOOSE_ATTR)->ShowWindow(add_on);
	GetDlgItem(IDC_BUTTON_CHOOSE_NOTHING_ATTR)->ShowWindow(add_on);

	bool already_active = false;
	list<int>::iterator current = KRS_always_active_params.begin();
	while (current != KRS_always_active_params.end())
	{
		if (*current == m_param->m_num_par)
		{
			already_active = true;
			break;
		}
		current++;
	}
	on = (	already_active ||
			(m_param_type == PARAM_TYPE_CHANNEL && m_channel_num == -1) ||
			(m_param_type == PARAM_TYPE_FORMULA && m_formula == NULL));
	GetDlgItem(IDC_CHECK_ACTIVE)->EnableWindow(!on);
//	if (on)
//		CheckDlgButton(IDC_CHECK_ACTIVE, FALSE);
	CheckDlgButton(IDC_CHECK_ACTIVE, !on);
}

void NewCorrespondenceDlg::OnComboParamType() 
{
	m_param_type = m_combo_param_type.GetCurSel() + 1;
	UpdateParamInfo();
}
//---------------------------------------------------------
//EVG 23-01-08
extern int g_pos_in_time_buffer;

void NewCorrespondenceDlg::OnBnClickedCheckActive()
{
    UpdateData(true);
    if(m_param->bParamActive == m_active) return;
    m_param->bParamActive = m_active;

    if(m_active) m_param->FinishTimeOut(g_pos_in_time_buffer);
    else m_param->StartTimeOut();
}

void NewCorrespondenceDlg::OnBnClickedCheckDb()
{
	BOOL ena = IsDlgButtonChecked(IDC_CHECK_DB);
	GetDlgItem(IDC_STATIC_DB_TIME_INTERVAL)->EnableWindow(ena);
	GetDlgItem(IDC_EDIT_DB_TIME_INTERVAL)->EnableWindow(ena);
	GetDlgItem(IDC_STATIC_DB_VAL_INTERVAL)->EnableWindow(ena);
	GetDlgItem(IDC_EDIT_DB_VAL_INTERVAL)->EnableWindow(ena);
	GetDlgItem(IDC_STATIC_DB_MU)->EnableWindow(ena);
}

void NewCorrespondenceDlg::ClearFile(CString &path, CString name)
{
	if (MessageBox("Сбросить оповещение \"" + name + "\"?", "Сброс файла звукового оповещения", MB_YESNO) != IDYES)
		return;
	path = "";
	UpdateData(FALSE);
}

void NewCorrespondenceDlg::OnBnClickedButtonClear1()
{
	ClearFile(m_file1_path, "Больше Аварийного");
}

void NewCorrespondenceDlg::OnBnClickedButtonClear2()
{
	ClearFile(m_file2_path, "Больше Максимального");
}

void NewCorrespondenceDlg::OnBnClickedButtonClear3()
{
	ClearFile(m_file3_path, "Меньше Минимального");
}
