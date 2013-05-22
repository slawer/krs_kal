#include "stdafx.h"
#include "KRS.h"
#include "DlgHardCfg.h"

IMPLEMENT_DYNAMIC(DlgHardCfg, CDialog)

static CString g_cfg_path;

DlgHardCfg::DlgHardCfg(bool save, CWnd* pParent):
	CDialog(DlgHardCfg::IDD, pParent), m_save(save)
{
}

DlgHardCfg::~DlgHardCfg()
{
}

void DlgHardCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgHardCfg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_CFGS, &DlgHardCfg::OnLbnSelchangeListCfgs)
END_MESSAGE_MAP()

BOOL DlgHardCfg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	if (m_save)
		SetWindowText(" Сохранение конфигурации");
	else
		SetWindowText(" Выберите конфигурацию для восстановления");
	
	CListBox *list_box = (CListBox *)GetDlgItem(IDC_LIST_CFGS);
	char str[1024];
	GetCurrentDirectory(1024, str);
	CString prev_dir = str;
	g_cfg_path = KRS_root_path + "\\CFG";
	SetCurrentDirectory(g_cfg_path);

	WIN32_FIND_DATA file_data;
	HANDLE file_handle = FindFirstFile("*.*", &file_data);
	while (file_handle != INVALID_HANDLE_VALUE)
	{
		if ((file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			list_box->AddString(file_data.cFileName);
		if (FindNextFile(file_handle, &file_data) == 0)
			file_handle = INVALID_HANDLE_VALUE;
	}
	
	SetCurrentDirectory(prev_dir);
	
	if (m_save)
	{
		SetDlgItemText(IDOK, "Сохранить");
		
		SYSTEMTIME st;
		GetLocalTime(&st);
		CString str_date, str_time;
		str_date = TXT("%d.%02d.%02d")<<st.wYear<<st.wMonth<<st.wDay;
		str_time = TXT("%02d-%02d-%02d")<<st.wHour<<st.wMinute<<st.wSecond;

		SetDlgItemText(IDC_EDIT_NAME, "Конфигурация от " + str_date + " " + str_time);
	}
	else
	{
		GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);
	}
	
	return TRUE;
}

void DlgHardCfg::OnOK()
{
	CListBox *list_box = (CListBox *)GetDlgItem(IDC_LIST_CFGS);
	if (m_save)
	{
		if (GetDlgItemText(IDC_EDIT_NAME, m_cfg_name) <= 0)
		{
			MessageBox("Имя конфигурации задано неверно", "Недопустимое имя", MB_OK);
			return;
		}
		CString existing_name;
		for (int i = 0; i < list_box->GetCount(); i++)
		{
			list_box->GetText(i, existing_name);
			if (m_cfg_name.CompareNoCase(existing_name) == 0)
			{
				MessageBox("Такое имя уже существует", "Недопустимое имя", MB_OK);
				return;
			}
		}
		FILE* f = fopen(m_cfg_name, "wb");
		if (!f)
		{
			MessageBox("Невозможно создать файл с таким именем", "Недопустимое имя", MB_OK);
			return;
		}
		fclose(f);
		remove(m_cfg_name);
	}
	else
	{
		int selected_index = list_box->GetCurSel();
		if (selected_index == -1)
		{
			MessageBox("Файл конфигурации не выбран", "Выберите файл", MB_OK);
			return;
		}
		list_box->GetText(selected_index, m_cfg_name);
	}
	CDialog::OnOK();
}

void DlgHardCfg::OnLbnSelchangeListCfgs()
{
	CListBox *list_box = (CListBox *)GetDlgItem(IDC_LIST_CFGS);
	int selected_index = list_box->GetCurSel();
	if (selected_index == -1)
		return;
	list_box->GetText(selected_index, m_cfg_name);
	SetDlgItemText(IDC_EDIT_NAME, m_cfg_name);
}
