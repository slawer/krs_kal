#include "stdafx.h"
#include "KRS.h"
#include "STAGES.h"
#include "DlgAddStage.h"
#include "DlgStageSequence.h"

static int WIDTH_LAST_COL = 0;

IMPLEMENT_DYNAMIC(DlgStageSequence, CDialog)

DlgStageSequence::DlgStageSequence(CWnd* pParent /*=NULL*/):
	CDialog(DlgStageSequence::IDD, pParent)
{
}

DlgStageSequence::~DlgStageSequence()
{
}

void DlgStageSequence::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SEQ, m_list_seq);
}

BEGIN_MESSAGE_MAP(DlgStageSequence, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &DlgStageSequence::OnBnClickedButtonAdd)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SEQ, &DlgStageSequence::OnLvnItemchangedListSeq)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &DlgStageSequence::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &DlgStageSequence::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDOK, &DlgStageSequence::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL DlgStageSequence::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_list_seq.SetListFont(GetFont());
	m_list_seq.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	//m_list_seq.m_show_selection = false;
	RECT r;
	m_list_seq.GetClientRect(&r);
	int len_N = 55, len_name = 150;
	WIDTH_LAST_COL = (r.right - r.left) - len_N - 16 - len_name;
	m_list_seq.InsertColumn(0, "N этапа", LVCFMT_LEFT, len_N);
	m_list_seq.InsertColumn(1, "Название", LVCFMT_LEFT, len_name);
	m_list_seq.InsertColumn(2, "Шаблон", LVCFMT_LEFT, WIDTH_LAST_COL);
	RebuildTable();
	m_list_seq.SetCurItem(STAGES_working_sequence.size()-1);
	return TRUE;
}

void DlgStageSequence::RebuildTable()
{	
	m_list_seq.DeleteAllItems();

	int list_line = 0, index;
	CString str;
	STAGES_WorkingSequence::iterator current = STAGES_working_sequence.begin();
	STAGES_StageTemplate* stage_template;
	while (current != STAGES_working_sequence.end())
	{
		str = TXT("%d")<<(list_line+1);
		index = m_list_seq.InsertItem(list_line, str);
		stage_template = STAGES_GetStageTemplate(current->m_stage_template_num);
		if (stage_template == NULL)
			str = TXT("Несуществующий шаблон №%d")<<*current;
		else
			//str = TXT(" %s (%d)") << stage_template->m_name << stage_template->m_stage_template_num;
			str = TXT(" %s") << stage_template->m_name;
		m_list_seq.SetItemText(index, 1, current->m_stage_name);
		m_list_seq.SetItemText(index, 2, str);
		m_list_seq.SetItemData(index, list_line+1);
		m_list_seq.SetItemStyle(index, 1, 1);
		list_line++;
		current++;
	}
	m_list_seq.SetColumnWidth(2, WIDTH_LAST_COL + ((m_list_seq.GetCountPerPage() < m_list_seq.GetItemCount()) ? 0 : 16));
};

void DlgStageSequence::EnableEditDelete(BOOL ena)
{
	GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(ena);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(ena);
}

void DlgStageSequence::OnLvnItemchangedListSeq(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	BOOL ena = m_list_seq.GetCurItem() >= 0;
	EnableEditDelete(ena);
	*pResult = 0;
}

void DlgStageSequence::OnBnClickedButtonAdd()
{
	if (STAGES_stage_templates.size() == 0)
	{
		MessageBox("Библиотека этапов пуста", "Добавление невозможно", MB_OK);
		return;
	}
		
	DlgAddStage dlg;
	if (dlg.DoModal() == IDCANCEL || dlg.m_selected_stage_template <= 0)
		return;

	int selected = m_list_seq.GetCurItem();
	if (selected == -1 || selected > (int)STAGES_working_sequence.size() - 1)
		selected = STAGES_working_sequence.size() - 1;

	STAGES_WorkingSequence::iterator current = STAGES_working_sequence.begin();
	for (int i = 0; i < selected + 1; i++)
		current++;

	STAGES_working_sequence.insert(current, STAGES_StageInfoForSequence(dlg.m_selected_stage_template));

	RebuildTable();
	if (STAGES_working_sequence.size() == 1)
	{
		EnableEditDelete(TRUE);
		m_list_seq.SetCurItem(0);
	}
	else
		m_list_seq.SetCurItem(selected+1);
}

void DlgStageSequence::OnBnClickedButtonEdit()
{
	int selected = m_list_seq.GetCurItem();
	if (selected != -1)
	{
		if (selected >= (int)STAGES_working_sequence.size())
		{
			MessageBox("selected >= STAGES_working_sequence.size()", "Ошибка", MB_OK);
			return;
		}
		STAGES_WorkingSequence::iterator desired_num = STAGES_working_sequence.begin();
		for (int i = 0; i < selected; i++)
			desired_num++;
		DlgAddStage dlg;
		dlg.m_selected_stage_template = desired_num->m_stage_template_num;
		if (dlg.DoModal() == IDCANCEL || dlg.m_selected_stage_template <= 0)
			return;
		desired_num->m_stage_template_num = dlg.m_selected_stage_template;
		desired_num->m_stage_name = STAGE_GetNameOfTemplate(dlg.m_selected_stage_template);
		RebuildTable();
	}
}

void DlgStageSequence::OnBnClickedButtonDelete()
{
	int selected = m_list_seq.GetCurItem();
	if (selected != -1)
	{
		if (selected >= (int)STAGES_working_sequence.size())
		{
			MessageBox("selected >= STAGES_working_sequence.size()", "Ошибка", MB_OK);
			return;
		}
		STAGES_WorkingSequence::iterator desired_info = STAGES_working_sequence.begin();
		for (int i = 0; i < selected; i++)
			desired_info++;
		bool err = false;
		STAGES_StageTemplates::iterator desired = STAGES_stage_templates.find(desired_info->m_stage_template_num);
		if (desired == STAGES_stage_templates.end())
		{
			MessageBox("desired == STAGES_stage_templates.end()", "Ошибка", MB_OK);
			err = true;
		}
		if (err || MessageBox(CString(TXT("Удалить этап №%d по шаблону «%s»") << (selected + 1) << desired->second->m_name), "Удаление этапа", MB_YESNO) == IDYES)
		{
			STAGES_working_sequence.erase(desired_info);
			int num = selected - 1;
			if (num == -1)
			{
				if (STAGES_working_sequence.size() > 0)
					num = 0;
				else
					EnableEditDelete(FALSE);
			}
			m_list_seq.SetCurItem(num);
			RebuildTable();
		}
	}
}

void DlgStageSequence::OnBnClickedOk()
{
	// берём из списка новые имена этапов >>
	int list_line = 0;
	STAGES_WorkingSequence::iterator current = STAGES_working_sequence.begin();
	while (current != STAGES_working_sequence.end())
	{
		current->m_stage_name = m_list_seq.GetItemText(list_line, 1);
		list_line++;
		current++;
	}
	// берём из списка новые имена этапов <<
	STAGES_SaveWorkingSequence();
	OnOK();
}