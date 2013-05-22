#include "stdafx.h"
#include "krs.h"
#include "DlgEditDE.h"
#include "FormulaEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgEditDE::DlgEditDE(SERV_DE* de, CWnd* pParent /*=NULL*/):
	CDialog(DlgEditDE::IDD, pParent), m_de(de)
{
	//{{AFX_DATA_INIT(DlgEditDE)
	//}}AFX_DATA_INIT
}

void DlgEditDE::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEditDE)
	DDX_Control(pDX, IDC_COMBO_LENGTH, m_combo_length);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgEditDE, CDialog)
	//{{AFX_MSG_MAP(DlgEditDE)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_CHECK_USTAVKA, OnCheckUstavka)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void DlgEditDE::OutputFormiula()
{
	SetDlgItemText(IDC_EDIT_FORMULA, m_formula->GetText(FORMULA_MODE_RESULT));
	SetDlgItemText(IDC_EDIT_TEMPLATE, m_formula->GetText(FORMULA_MODE_TEMPLATE));
}

void DlgEditDE::CheckComplete()
{
	BOOL ena = (m_formula->GetArgument(1) != NULL);
	GetDlgItem(IDOK)->EnableWindow(ena);
}

BOOL DlgEditDE::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_combo_length.SetCurSel(m_de->m_length - 1);

	m_formula = m_de->m_formula->MakeCopy();

	CheckDlgButton(IDC_CHECK_USTAVKA, m_de->m_has_ustavka_and_limit);
	OnCheckUstavka();
	SetDlgItemText(IDC_EDIT_USTAVKA_ADDR, CString(TXT("%04X") << m_de->m_ustavka_addr));
	SetDlgItemText(IDC_EDIT_LIMIT_ADDR, CString(TXT("%04X") << m_de->m_limit_addr));

	OutputFormiula();
	CheckComplete();
	return TRUE;
}

void DlgEditDE::OnButtonEdit() 
{
	FormulaEditDlg dlg(m_formula);
	if (dlg.DoModal() != IDOK)
		return;

	OutputFormiula();
	CheckComplete();
}

void DlgEditDE::OnOK() 
{
	bool has = IsDlgButtonChecked(IDC_CHECK_USTAVKA) == TRUE;
	if (has)
	{
		char addr_str[5];
		int ustavka_addr = -1;
		int limit_addr = -1;
		GetDlgItemText(IDC_EDIT_USTAVKA_ADDR, addr_str, 5);
		if (sscanf(addr_str, "%X", &ustavka_addr) != 1 || ustavka_addr <= 0)
		{
			MessageBox("Адрес уставки задан неверно", "Ошибка в адресе уставки");
			return;
		}
		GetDlgItemText(IDC_EDIT_LIMIT_ADDR, addr_str, 5);
		if (sscanf(addr_str, "%X", &limit_addr) != 1 || limit_addr <= 0)
		{
			MessageBox("Адрес предела задан неверно", "Ошибка в адресе предела");
			return;
		}
		m_de->m_ustavka_addr = ustavka_addr;
		m_de->m_limit_addr = limit_addr;
	}
	else
	{
		m_de->m_ustavka_addr = 0;
		m_de->m_limit_addr = 0;
	}
	m_de->m_has_ustavka_and_limit = has;

	delete m_de->m_formula;
	m_de->m_formula = m_formula;

	m_de->m_length = m_combo_length.GetCurSel() + 1;
	CDialog::OnOK();
}

void DlgEditDE::OnCheckUstavka()
{
	BOOL ena = IsDlgButtonChecked(IDC_CHECK_USTAVKA);
	GetDlgItem(IDC_EDIT_USTAVKA_ADDR)->EnableWindow(ena);
	GetDlgItem(IDC_EDIT_LIMIT_ADDR)->EnableWindow(ena);
}

void DlgEditDE::OnCancel() 
{
	if (m_formula)
		delete m_formula;
	CDialog::OnCancel();
}
