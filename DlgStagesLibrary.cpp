#include "stdafx.h"
#include "KRS.h"
#include "STAGES.h"
#include "DlgStagesLibrary.h"
#include "DlgStageSettings.h"

IMPLEMENT_DYNAMIC(DlgStagesLibrary, CDialog)

DlgStagesLibrary::DlgStagesLibrary(CWnd* pParent /*=NULL*/)
	: CDialog(DlgStagesLibrary::IDD, pParent), m_prev_map(NULL)
{
}

DlgStagesLibrary::~DlgStagesLibrary()
{
}

void DlgStagesLibrary::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LIB, m_list_lib);
}

BEGIN_MESSAGE_MAP(DlgStagesLibrary, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LIB, &DlgStagesLibrary::OnLvnItemchangedListLib)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &DlgStagesLibrary::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &DlgStagesLibrary::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &DlgStagesLibrary::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDOK, &DlgStagesLibrary::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &DlgStagesLibrary::OnBnClickedButtonCopy)
	ON_WM_DESTROY()	
END_MESSAGE_MAP()

static int WIDTH_LAST_COL = 0;

void DlgStagesLibrary::RebuildTable()
{	
	m_list_lib.DeleteAllItems();

	int list_line = 0, index;
	CString str;
	STAGES_StageTemplates::iterator current = STAGES_stage_templates.begin();
	while (current != STAGES_stage_templates.end())
	{
		if (current->first != 0)
		{
			str = TXT("%d")<<current->first;
			index = m_list_lib.InsertItem(list_line, str);
			m_list_lib.SetItemText(index, 1, current->second->m_name);
			m_list_lib.SetItemData(index, current->first);
			//SetupLine(index, param);
			list_line++;
		}
		current++;
	}
	m_list_lib.SetColumnWidth(1, WIDTH_LAST_COL + ((m_list_lib.GetCountPerPage() < m_list_lib.GetItemCount()) ? 0 : 16));
};

void DlgStagesLibrary::EnableEditDelete(BOOL ena)
{
	GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(ena);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(ena);
	GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(ena);
}

BOOL DlgStagesLibrary::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_prev_map = m_MapParamCurent;
	m_MapParamCurent = &m_MapParam;

	m_list_lib.SetListFont(GetFont());
	m_list_lib.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	//m_list_lib.m_show_selection = false;
	RECT r;
	m_list_lib.GetClientRect(&r);
	int len_N = 25;
	WIDTH_LAST_COL = (r.right - r.left) - len_N - 16;
	m_list_lib.InsertColumn(0, "N", LVCFMT_LEFT, len_N);
	m_list_lib.InsertColumn(1, "Название шаблона этапа", LVCFMT_LEFT, WIDTH_LAST_COL);
	RebuildTable();
	return TRUE;
}

void DlgStagesLibrary::OnLvnItemchangedListLib(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	BOOL ena = m_list_lib.GetCurItem() >= 0;
	EnableEditDelete(ena);
	*pResult = 0;
}

void DlgStagesLibrary::OnBnClickedButtonDelete()
{
	int selected = m_list_lib.GetCurItem();
	if (selected != -1)
	{
		int num = m_list_lib.GetItemData(selected);
		STAGES_StageTemplates::iterator desired = STAGES_stage_templates.find(num);
		if (desired != STAGES_stage_templates.end())
		{
			if (MessageBox(CString(TXT("Удалить шаблон «%s»") << desired->second->m_name), "Удаление шаблона", MB_YESNO) == IDYES)
			{
				delete desired->second;
				STAGES_stage_templates.erase(desired);
				num = selected - 1;
				if (num == -1)
				{
					if (STAGES_stage_templates.size() > 1)
						num = 0;
					else
						EnableEditDelete(FALSE);
				}
				m_list_lib.SetCurItem(num);
				RebuildTable();
			}
		}
		else
			MessageBox("selected != -1 && find == end", "Ошибка", MB_OK);
	}
}

void DlgStagesLibrary::OnBnClickedButtonAdd()
{
	STAGES_StageTemplate* stage_template = new STAGES_StageTemplate;
	stage_template->m_name = TXT("Этап №%d")<<STAGES_stage_templates.size();
	DlgStageSettings dlg(stage_template);
	if (dlg.DoModal() == IDCANCEL)
	{
		delete stage_template;
		return;
	}
	stage_template->m_stage_template_num = STAGES_stage_templates.size();
	STAGES_stage_templates[stage_template->m_stage_template_num] = stage_template;
	if (STAGES_stage_templates.size() == 2)
	{
		EnableEditDelete(TRUE);
		m_list_lib.SetCurItem(0);
	}
	RebuildTable();
}

void DlgStagesLibrary::OnBnClickedButtonEdit()
{
	int selected = m_list_lib.GetCurItem();
	if (selected != -1)
	{
		int num = m_list_lib.GetItemData(selected);
		STAGES_StageTemplates::iterator desired = STAGES_stage_templates.find(num);
		if (desired != STAGES_stage_templates.end())
		{
			DlgStageSettings dlg(desired->second);
			if (dlg.DoModal() == IDCANCEL)
				return;
			RebuildTable();
		}
		else
			MessageBox("selected != -1 && find == end", "Ошибка", MB_OK);
	}
}

void DlgStagesLibrary::OnBnClickedOk()
{
	STAGES_SaveTemplates();
	OnOK();
}

void DlgStagesLibrary::OnBnClickedButtonCopy()
{
	int selected = m_list_lib.GetCurItem();
	if (selected != -1)
	{
		int num = m_list_lib.GetItemData(selected);
		STAGES_StageTemplates::iterator desired = STAGES_stage_templates.find(num);
		if (desired != STAGES_stage_templates.end())
		{
			STAGES_StageTemplate* clone = new STAGES_StageTemplate(desired->second);
			DlgStageSettings dlg(clone);
			if (dlg.DoModal() == IDCANCEL)
			{
				delete clone;
				return;
			}
			clone->m_stage_template_num = STAGES_stage_templates.size();
			STAGES_stage_templates[clone->m_stage_template_num] = clone;
			RebuildTable();
		}
		else
			MessageBox("selected != -1 && find == end", "Ошибка", MB_OK);
	}
}

void DlgStagesLibrary::OnDestroy()
{
	m_MapParamCurent = m_prev_map;
}