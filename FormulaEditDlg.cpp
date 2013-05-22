#include "stdafx.h"
#include "krs.h"
#include <CHECK.h>
#include "FormulaEditDlg.h"
#include "ArgConstDlg.h"
#include "DlgFormulasList.h"
#include "DlgSelectChannel.h"
#include "MainFrm.h"
#include "TXT.h"
#include "ChangeParam.h"
#include "DlgArgPiecewise.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

FormulaEditDlg::FormulaEditDlg(FORMULA_Formula* root, CWnd* pParent /*=NULL*/)
	: CDialog(FormulaEditDlg::IDD, pParent), m_root(root)
{
	//{{AFX_DATA_INIT(FormulaEditDlg)
	//}}AFX_DATA_INIT
}

void FormulaEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FormulaEditDlg)
	DDX_Control(pDX, IDC_LIST_ARGS, m_list_args);
	DDX_Control(pDX, IDC_COMBO_ARG_TYPE, m_combo_arg_type);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormulaEditDlg, CDialog)
	//{{AFX_MSG_MAP(FormulaEditDlg)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE, OnButtonChoose)
	ON_CBN_SELCHANGE(IDC_COMBO_ARG_TYPE, OnSelchangeComboArgType)
	ON_LBN_SELCHANGE(IDC_LIST_ARGS, OnSelchangeListArgs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

struct ArgInfo
{
	ArgInfo(FORMULA_Formula* f = NULL, FORMULA_Argument* a = NULL, int an = 0):
		m_formula(f), m_argument(a), m_argument_num(an)
	{}
	FORMULA_Formula* m_formula;
	FORMULA_Argument* m_argument;
	int m_argument_num;
};

map<int, ArgInfo> g_agrument_map;

void FormulaEditDlg::RebuildFormula(FORMULA_Formula* formula, int &layer)
{
	CString str;
	FORMULA_Argument* arg;
	int index, info_pos, this_formula_layer = layer, layer2;
	int num = formula->GetArgumentsNum();
	for (int i = 1; i <= num; i++)
	{
		arg = formula->GetArgument(i);
		layer2 = layer;
		str = TXT("%c%d = %s") << 'A' + this_formula_layer << i << ((arg == NULL) ? "- не определено -" : arg->GetText(FORMULA_MODE_ARG_TEMPLATE, layer2));
		index = m_list_args.AddString(str);
		info_pos = m_list_args.GetItemData(index);
		info_pos = (this_formula_layer << 8) | i;
		m_list_args.SetItemData(index, info_pos);
		g_agrument_map[info_pos] = ArgInfo(formula, arg, i);
		if (arg != NULL && arg->GetType() == FORMULA_ARG_FORMULA)
		{
			layer++;
			RebuildFormula(((FORMULA_ArgumentFormula*)arg)->GetFormula(), layer);
		}
	}
}

void FormulaEditDlg::RebuildArgumentsList()
{
	g_agrument_map.clear();
	m_list_args.ResetContent();
	m_max_layer = 0;
	RebuildFormula(m_root, m_max_layer);
	CheckComplete();
	OnSelchangeListArgs();
}

void FormulaEditDlg::OnChooseArgument()
{
	int idx = m_list_args.GetCurSel();
	int info_pos = (idx == -1) ? -1 : m_list_args.GetItemData(idx);
	m_combo_arg_type.EnableWindow(idx != -1);
	FORMULA_Argument* argument = (idx == -1)? NULL : g_agrument_map[info_pos].m_argument;
	m_combo_arg_type.SetCurSel( (argument == 0)?0:argument->GetType() );
	OnSelchangeComboArgType();
}

void FormulaEditDlg::CheckComplete()
{
	BOOL complete = TRUE;
	map<int, ArgInfo>::iterator current = g_agrument_map.begin();
	while (current != g_agrument_map.end())
	{
		if (current->second.m_argument == NULL)
		{
			complete = FALSE;
			break;
		}
		current++;
	}
	GetDlgItem(IDOK)->EnableWindow(complete);

	int n = 0;
	SetDlgItemText(IDC_EDIT_TEMPLATE_VIEW, m_root->GetText(FORMULA_MODE_TEMPLATE, n));
	n = 0;
	SetDlgItemText(IDC_EDIT_RESULT_VIEW, m_root->GetText(FORMULA_MODE_RESULT, n));
}

BOOL FormulaEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RebuildArgumentsList();
	return TRUE;
}

void FormulaEditDlg::OnSelchangeComboArgType() 
{
	GetDlgItem(IDC_BUTTON_CHOOSE)->EnableWindow(m_combo_arg_type.GetCurSel() != 0);
}

void FormulaEditDlg::OnButtonChoose()
{
	int type = m_combo_arg_type.GetCurSel();
	CHECK(type != 0);
	int idx = m_list_args.GetCurSel();
	CHECK(idx != -1);
	int info_pos = m_list_args.GetItemData(idx);
	ArgInfo ai = g_agrument_map[info_pos];
	int num = ai.m_argument_num;
	FORMULA_Formula* formula = ai.m_formula;
	FORMULA_Argument* argument = ai.m_argument;
	if (type == FORMULA_ARG_CONST)
	{
		float init_value = 0;
		FORMULA_ArgumentConstantValue *const_argument = NULL;
		if (argument != NULL && argument->GetType() == FORMULA_ARG_CONST)
		{
			const_argument = (FORMULA_ArgumentConstantValue *)argument;
			init_value = const_argument->GetConstantValue();
		}
		ArgConstDlg dlg(init_value);
		if (dlg.DoModal() == IDOK)
		{
			if (const_argument != NULL)
				const_argument->SetConstantValue(dlg.m_constant);
			else
			{
				const_argument = new FORMULA_ArgumentConstantValue(num, dlg.m_constant);
				formula->SetArgument(const_argument);
				if (argument)
					delete argument;
			}
			RebuildArgumentsList();
		}
	}
	else
	if (type == FORMULA_ARG_FORMULA)
	{
		int init = -1;
		FORMULA_ArgumentFormula *arg_formula = NULL;
		if (argument != NULL && argument->GetType() == FORMULA_ARG_FORMULA)
		{
			arg_formula = (FORMULA_ArgumentFormula *)argument;
			init = arg_formula->GetFormula()->m_num_in_map;
		}
		DlgFormulasList dlg(init, m_max_layer + 1);
		if (dlg.DoModal() != IDOK || dlg.m_selected_formula_num == -1 || init == dlg.m_selected_formula_num)
			return;
		FORMULA_Formula* new_formula = FORMULA_all_formulas[dlg.m_selected_formula_num]->MakeTemplate();
		if (arg_formula != NULL)
		{
			FORMULA_Formula* old_formula = arg_formula->GetFormula();
			new_formula->TryCaptureArguments(old_formula);
			arg_formula->SetFormula(new_formula);
			delete old_formula;
		}
		else
		{
			if (argument)
				delete argument;
			arg_formula = new FORMULA_ArgumentFormula(num, new_formula);
			formula->SetArgument(arg_formula);
		}
		RebuildArgumentsList();
	}
	else
	if (type == FORMULA_ARG_CHANNEL)
	{
		SERV_Channel* channel = NULL;
		FORMULA_ArgumentChannelValue *channel_argument = NULL;
		if (argument != NULL && argument->GetType() == FORMULA_ARG_CHANNEL)
		{
			channel_argument = (FORMULA_ArgumentChannelValue *)argument;
			channel = channel_argument->GetChannel();
		}
		DlgSelectChannel dlg("Выберите канал", false);
		if (channel != NULL)
		{
			dlg.m_convertor_id = channel->GetConvNumber();
			dlg.m_channel_id = channel->GetUniqueNumber();
		}

		if (dlg.DoModal() == IDCANCEL)
			return;

		if (channel_argument == NULL)
		{
			channel_argument = new FORMULA_ArgumentChannelValue(num, dlg.m_convertor_id, dlg.m_channel_id);
			formula->SetArgument(channel_argument);
			if (argument != NULL)
				delete argument;
		}
		else
			channel_argument->SetChannel(dlg.m_convertor_id, dlg.m_channel_id);
		RebuildArgumentsList();
	}
	else
	if (type == FORMULA_ARG_PARAM)
	{
		CParam* param = NULL;
		FORMULA_ArgumentParamValue *param_argument = NULL;
		if (argument != NULL && argument->GetType() == FORMULA_ARG_PARAM)
		{
			param_argument = (FORMULA_ArgumentParamValue *)argument;
			param = param_argument->GetParam();
		}
		CDlgAddParam dlg(-1);
		dlg.m_show_additional_cement_params = false;
		if (param != NULL)
			dlg.m_key = param->m_num_par;
		if (dlg.DoModal() == IDCANCEL)
			return;
		if (!m_MapParam.Lookup(dlg.m_key, param) || param == NULL)
			return;
		if (param_argument == NULL)
		{
			param_argument = new FORMULA_ArgumentParamValue(num, param->m_num_par);
			formula->SetArgument(param_argument);
			if (argument != NULL)
				delete argument;
		}
		else
			param_argument->SetParam(param->m_num_par);
		RebuildArgumentsList();
	}
	else
	if (type == FORMULA_ARG_PIECEWISE)
	{
		FORMULA_ArgumentPiecewise *piecewise_argument = NULL;
		bool need_new = argument == NULL || argument->GetType() != FORMULA_ARG_PIECEWISE;
		if (!need_new)
			piecewise_argument = (FORMULA_ArgumentPiecewise *)argument;
		else
			piecewise_argument = new FORMULA_ArgumentPiecewise(num);
		DlgArgPiecewise dlg(piecewise_argument);
		if (dlg.DoModal() == IDCANCEL)
		{
			if (need_new)
				delete piecewise_argument;
			return;
		}
		if (argument != NULL && argument->GetType() != FORMULA_ARG_PIECEWISE)
			delete argument;
		formula->SetArgument(piecewise_argument);
		RebuildArgumentsList();
		return;
	}
	else
	if (type == FORMULA_ARG_NULL)
	{
		FORMULA_ArgumentNULL *null_argument = NULL;
		bool need_new = argument == NULL || argument->GetType() != FORMULA_ARG_NULL;
		if (!need_new)
			null_argument = (FORMULA_ArgumentNULL *)argument;
		else
			null_argument = new FORMULA_ArgumentNULL(num);
		if (argument != NULL && argument->GetType() != FORMULA_ARG_NULL)
			delete argument;
		formula->SetArgument(null_argument);
		RebuildArgumentsList();
		return;
	}
}

void FormulaEditDlg::OnSelchangeListArgs() 
{
	OnChooseArgument();
}
