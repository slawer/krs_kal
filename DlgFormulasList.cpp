#include "stdafx.h"
#include "krs.h"
#include "DlgFormulasList.h"
#include "FORMULA.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgFormulasList::DlgFormulasList(int initial, int layer, CWnd* pParent /*=NULL*/)
	: CDialog(DlgFormulasList::IDD, pParent), m_init_num(initial), m_layer(layer)
{
	//{{AFX_DATA_INIT(DlgFormulasList)
	//}}AFX_DATA_INIT
}

void DlgFormulasList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgFormulasList)
	DDX_Control(pDX, IDC_LIST_FORMULAS, m_list_formulas);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgFormulasList, CDialog)
	//{{AFX_MSG_MAP(DlgFormulasList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void DlgFormulasList::OnOK() 
{
	int item = m_list_formulas.GetCurSel();
	if (item != LB_ERR)
		m_selected_formula_num = m_list_formulas.GetItemData(item);
	CDialog::OnOK();
}

BOOL DlgFormulasList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int item, n;
	FORMULA_Formulas::iterator current = FORMULA_all_formulas.begin();
	while (current != FORMULA_all_formulas.end())
	{
		n = m_layer;
		item = m_list_formulas.AddString(current->second->GetText(FORMULA_MODE_TEMPLATE, n));
		m_list_formulas.SetItemData(item, current->second->m_num_in_map);
		if (m_init_num == current->second->m_num_in_map)
			m_list_formulas.SetCurSel(item);
		current++;
	}
	m_selected_formula_num = -1;
	return TRUE;
}
