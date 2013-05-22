#include "stdafx.h"
#include "krs.h"
#include "DlgStageSettings.h"
#include "DlgEditCalculation.h"

#include "STAGES.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgStageSettings::DlgStageSettings(STAGES_StageTemplate* stage_template, CWnd* pParent /*=NULL*/):
	CDialog(DlgStageSettings::IDD, pParent), m_stage_template(stage_template)
{
	//{{AFX_DATA_INIT(DlgStageSettings)
	//}}AFX_DATA_INIT
}

void DlgStageSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgStageSettings)
	DDX_Control(pDX, IDC_LIST_STAGES, m_list_stages);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgStageSettings, CDialog)
	//{{AFX_MSG_MAP(DlgStageSettings)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_LBN_SELCHANGE(IDC_LIST_STAGES, OnSelchangeListStages)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &DlgStageSettings::OnBnClickedOk)
END_MESSAGE_MAP()

CString GetStrOfCalcTemplate(STAGES_CalculationTemplate* ct)
{
	CString str;
	str.Format("%s = %s", ct->m_name, (ct->m_formula)?ct->m_formula->GetText(FORMULA_MODE_RESULT):"- не определено -");
	return str;
}

BOOL DlgStageSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_EDIT_NAME, m_stage_template->m_name);
	
	CString str;
	STAGES_CalculationTemplates::iterator current = m_stage_template->m_calculation_templates.begin();
	while (current != m_stage_template->m_calculation_templates.end())
	{
		m_list_stages.AddString(GetStrOfCalcTemplate(*current));
		current++;
	}

	return TRUE;
}

void DlgStageSettings::OnSelchangeListStages() 
{
	BOOL ena = TRUE;
	int ind = m_list_stages.GetCurSel();
	if (ind == -1)
		ena = FALSE;
	GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(ena);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(ena);
}

void DlgStageSettings::OnButtonAdd() 
{
	STAGES_CalculationTemplate* ct = new STAGES_CalculationTemplate;
	DlgEditCalculation dlg(ct);
	if (dlg.DoModal() == IDCANCEL)
	{
		delete ct;
		return;
	}
	m_stage_template->m_calculation_templates.push_back(ct);
	m_list_stages.AddString(GetStrOfCalcTemplate(ct));
}

void DlgStageSettings::OnButtonEdit() 
{
	int ind = m_list_stages.GetCurSel(), counter = 0;
	if (ind == -1)
		return;
	STAGES_CalculationTemplates::iterator current = m_stage_template->m_calculation_templates.begin();
	while (current != m_stage_template->m_calculation_templates.end() && counter < ind)
	{
		current++;
		counter++;
	}
	if (current == m_stage_template->m_calculation_templates.end())
		return;
	STAGES_CalculationTemplate* ct = *current;
	DlgEditCalculation dlg(ct);
	if (dlg.DoModal() == IDOK)
	{
		m_list_stages.InsertString(ind, GetStrOfCalcTemplate(ct));
		m_list_stages.DeleteString(ind + 1);
		m_list_stages.SetCurSel(ind);
	}
}

void DlgStageSettings::OnButtonDelete() 
{
	int ind = m_list_stages.GetCurSel(), counter = 0;
	if (ind == -1)
		return;
	if (MessageBox("Удалить выделенное вычисление?", "Удаление вычисления", MB_YESNO) == IDNO)
		return;
	STAGES_CalculationTemplates::iterator current = m_stage_template->m_calculation_templates.begin();
	while (current != m_stage_template->m_calculation_templates.end() && counter < ind)
	{
		current++;
		counter++;
	}
	if (current == m_stage_template->m_calculation_templates.end())
		return;
	delete *current;
	m_stage_template->m_calculation_templates.erase(current);
	m_list_stages.DeleteString(ind);
	m_list_stages.SetCurSel(-1);
}

void DlgStageSettings::OnBnClickedOk()
{
	GetDlgItemText(IDC_EDIT_NAME, m_stage_template->m_name);
	OnOK();
}
