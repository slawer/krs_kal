#include "stdafx.h"
#include "krs.h"
#include "DlgEditBO.h"
#include "DlgEditDE.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgEditBO::DlgEditBO(SERV_BO*bo, SERV_BO*exist, CWnd* pParent /*=NULL*/):
	CDialog(DlgEditBO::IDD, pParent), m_bo(bo), m_exist(exist)
{
	//{{AFX_DATA_INIT(DlgEditBO)
	//}}AFX_DATA_INIT
}

void DlgEditBO::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEditBO)
	DDX_Control(pDX, IDC_LIST_DE, m_list_de);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgEditBO, CDialog)
	//{{AFX_MSG_MAP(DlgEditBO)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_LBN_SELCHANGE(IDC_LIST_DE, OnSelchangeListDe)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void DlgEditBO::CheckComplete()
{
	BOOL ena = !m_bo->m_data_elements.empty();
	GetDlgItem(IDOK)->EnableWindow(ena);
}

void DlgEditBO::RebuildListDE()
{
	int offset = 0, sel_index = m_list_de.GetCurSel();
	m_list_de.ResetContent();
	SERV_DEs::iterator current = m_bo->m_data_elements.begin();
	while (current != m_bo->m_data_elements.end())
	{
		m_list_de.AddString((*current)->GetInfo(offset));
		current++;
	}
	m_list_de.SetCurSel(sel_index);
}

BOOL DlgEditBO::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_EDIT_DEV_ADDR, CString(TXT("%02X") << m_bo->m_device_addr));
	SetDlgItemText(IDC_EDIT_DEV_ADDR2, CString(TXT("%02X") << m_bo->m_device_addr_ust));

	RebuildListDE();
	CheckComplete();

	return TRUE;
}

void DlgEditBO::OnOK() 
{
	char dev_addr_str[3];
	GetDlgItemText(IDC_EDIT_DEV_ADDR, dev_addr_str, 3);
	int dev_addr = -1, dev_addr_ust = -1;
	if (sscanf(dev_addr_str, "%X", &dev_addr) != 1 || dev_addr <= 0)
	{
		MessageBox("Адрес устройства для вывода задан неверно", "Ошибка в адресе устройства");
		return;
	}
	GetDlgItemText(IDC_EDIT_DEV_ADDR2, dev_addr_str, 3);
	if (sscanf(dev_addr_str, "%X", &dev_addr_ust) != 1 || dev_addr_ust <= 0)
	{
		MessageBox("Адрес устройства для уставок задан неверно", "Ошибка в адресе устройства");
		return;
	}
	SERV_BOs::iterator current = SERV_tmp_indication_blocks.begin();
	while (current != SERV_tmp_indication_blocks.end())
	{
		if ((*current)->m_device_addr == dev_addr && m_exist != *current)
		{
			MessageBox("Устройство с таким адресом уже существует", "Ошибка в адресе устройства");
			return;
		}
		current++;
	}
	m_bo->m_device_addr = dev_addr;
	m_bo->m_device_addr_ust = dev_addr_ust;
	CDialog::OnOK();
}

void DlgEditBO::OnSelchangeListDe() 
{
	BOOL ena = TRUE;
	int ind = m_list_de.GetCurSel();
	if (ind == -1)
		ena = FALSE;
	GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(ena);
}

void DlgEditBO::OnButtonAdd() 
{
	SERV_DE* new_de = new SERV_DE(2, FORMULA_all_formulas[0]->MakeTemplate());
	DlgEditDE dlg(new_de);
	if (dlg.DoModal() != IDOK)
	{
		delete new_de;
		return;
	}
	m_bo->m_data_elements.push_back(new_de);
	RebuildListDE();
	CheckComplete();
}

void DlgEditBO::OnButtonEdit() 
{
	int ind = m_list_de.GetCurSel(), ind_tmp = 0;
	SERV_DEs::iterator current = m_bo->m_data_elements.begin();
	while (current != m_bo->m_data_elements.end())
	{
		if (ind_tmp == ind)
		{
			DlgEditDE dlg(*current);
			if (dlg.DoModal() == IDOK)
				RebuildListDE();
			break;
		}
		ind_tmp++;
		current++;
	}	
}

void DlgEditBO::OnButtonDelete()
{
	if (MessageBox("Удалить выделенный элемент массива данных?", "Удаление из массива данных", MB_YESNO) == IDNO)
		return;

	int ind = m_list_de.GetCurSel(), ind_tmp = 0;
	SERV_DEs::iterator current = m_bo->m_data_elements.begin();
	while (current != m_bo->m_data_elements.end())
	{
		if (ind_tmp == ind)
		{
			m_bo->m_data_elements.erase(current);
			m_list_de.DeleteString(ind);
			RebuildListDE();
			CheckComplete();
			break;
		}
		ind_tmp++;
		current++;
	}
}
