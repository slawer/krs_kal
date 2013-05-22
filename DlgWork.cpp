// uic 19.08.2008

#include "stdafx.h"
#include "DlgWork.h"
#include "DlgEditText.h"
#include "MainFrm.h"
#include ".\src\DB.h"

////////////////////////////////////////////////////////////////////////////////
// internal data
////////////////////////////////////////////////////////////////////////////////
static PRJ_Project* g_project = NULL;
static PRJ_Deposit* g_deposit = NULL;
static PRJ_Cluster* g_cluster = NULL;
static PRJ_BoreHole* g_bore_hole = NULL;
static PRJ_Work* g_work = NULL;
static CString g_importing_work_path;

////////////////////////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////////////////////////
void MemorizeSelectedBranch()
{
	PRJ_Object *obj = g_project;
	while (obj != NULL)
	{
		obj->m_prev_selected_child_id = obj->m_selected_child_id;
		obj = obj->GetSelectedChild();
	}
}

void RestoreSelectedBranch()
{
	PRJ_Object *obj = g_project;
	while (obj != NULL)
	{
		obj->m_selected_child_id = obj->m_prev_selected_child_id;
		obj = obj->GetSelectedChild();
	}
}

int FillComboWithChilds(CComboBox& combo, PRJ_Object* object)
{
	int i, id = 0, id2;
	if (object == NULL)
		return id;
	for (i = 1; i <= (int)object->m_max_id; i++)
	{
		if (object->m_childs.find(i) != object->m_childs.end())
		{
			id2 = combo.AddString(object->m_childs[i]->m_name);
			combo.SetItemData(id2, object->m_childs[i]->m_id);
			if (i == object->m_selected_child_id)
				id = id2;
		}
	}
	return id;
}

bool RemoveDir(CString full_path)
{
	bool result = false;

	CString folder_path = full_path + "\\";
	CString path;

	WIN32_FIND_DATA fd = {};
	HANDLE hFind = FindFirstFile(folder_path + "*", &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		result = TRUE;
		do {
			if (!(strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..")))
				continue;
			path = folder_path + fd.cFileName;

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				result = RemoveDir(path); 
			else
			{
				if( fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					SetFileAttributes(path, FILE_ATTRIBUTE_NORMAL);
				result = TRUE == DeleteFile(path);
			}

		} while(result && FindNextFile(hFind, &fd));
		FindClose( hFind);

		if (result)
			result = TRUE == RemoveDirectory(full_path);
	}
	return result;
}

IMPLEMENT_DYNAMIC(DlgWork, CDialog)

DlgWork::DlgWork(CString title, byte mode, CWnd* pParent): CDialog(DlgWork::IDD, pParent),
	m_title(title), m_mode(mode)
{
}

DlgWork::~DlgWork()
{
}

void DlgWork::AdjustCombos()
{
	m_combo_m.ResetContent();
	m_combo_k.ResetContent();
	m_combo_s.ResetContent();
	m_combo_r.ResetContent();

	CString first_text;

	bool not_open = (m_mode & DlgWork_OPEN)==0;
	bool not_new = (m_mode & DlgWork_NEW)==0;

	bool enable_add = not_open;
	bool enable_edit = not_open & not_new;
	bool enable_delete = not_open & not_new;

	if (g_project->m_childs.size() == 0)
		first_text = "- список месторождений пуст -";
	else
		first_text = "- месторождение не выбрано -";
	m_combo_m.AddString(first_text);
	m_combo_m.SetCurSel(FillComboWithChilds(m_combo_m, g_project));
	m_combo_m.EnableWindow(g_project->m_childs.size() > 0);
	g_deposit = (PRJ_Deposit*)g_project->GetSelectedChild();
	GetDlgItem(IDC_BUTTON_ADD_M)->EnableWindow(enable_add);
	GetDlgItem(IDC_BUTTON_EDIT_M)->EnableWindow(enable_edit && g_deposit != NULL);
	GetDlgItem(IDC_BUTTON_DELETE_M)->EnableWindow(enable_delete && g_deposit != NULL && g_project->m_prev_selected_child_id != g_deposit->m_id);

	if (g_deposit == NULL || g_deposit->m_childs.size() == 0)
		first_text = "- список кустов пуст -";
	else
		first_text = "- куст не выбран -";
	m_combo_k.AddString(first_text);
	m_combo_k.SetCurSel(FillComboWithChilds(m_combo_k, g_deposit));
	m_combo_k.EnableWindow(g_deposit != NULL && g_deposit->m_childs.size() > 0);
	g_cluster = (g_deposit == NULL) ? NULL : (PRJ_Cluster*)g_deposit->GetSelectedChild();
	GetDlgItem(IDC_BUTTON_ADD_K)->EnableWindow(enable_add && g_deposit != NULL);
	GetDlgItem(IDC_BUTTON_EDIT_K)->EnableWindow(enable_edit && g_cluster != NULL);
	GetDlgItem(IDC_BUTTON_DELETE_K)->EnableWindow(enable_delete && g_cluster != NULL && g_deposit->m_prev_selected_child_id != g_cluster->m_id);

	if (g_cluster == NULL || g_cluster->m_childs.size() == 0)
		first_text = "- список скважин пуст -";
	else
		first_text = "- скважина не выбрана -";
	m_combo_s.AddString(first_text);
	m_combo_s.SetCurSel(FillComboWithChilds(m_combo_s, g_cluster));
	m_combo_s.EnableWindow(g_cluster != NULL && g_cluster->m_childs.size() > 0);
	g_bore_hole = (g_cluster == NULL) ? NULL : (PRJ_BoreHole*)g_cluster->GetSelectedChild();
	GetDlgItem(IDC_BUTTON_ADD_S)->EnableWindow(enable_add && g_cluster != NULL);
	GetDlgItem(IDC_BUTTON_EDIT_S)->EnableWindow(enable_edit && g_bore_hole != NULL);
	GetDlgItem(IDC_BUTTON_DELETE_S)->EnableWindow(enable_delete && g_bore_hole != NULL && g_cluster->m_prev_selected_child_id != g_bore_hole->m_id);

	if (g_bore_hole == NULL || g_bore_hole->m_childs.size() == 0)
		first_text = "- список работ пуст -";
	else
		first_text = "- работа не выбрана -";
	m_combo_r.AddString(first_text);
	m_combo_r.SetCurSel(FillComboWithChilds(m_combo_r, g_bore_hole));
	m_combo_r.EnableWindow(g_bore_hole != NULL && g_bore_hole->m_childs.size() > 0);
	g_work = (g_bore_hole == NULL) ? NULL : (PRJ_Work*)g_bore_hole->GetSelectedChild();
	GetDlgItem(IDC_BUTTON_ADD_R)->EnableWindow(enable_add && g_bore_hole != NULL);
	GetDlgItem(IDC_BUTTON_EDIT_R)->EnableWindow(enable_edit && g_work != NULL);
	GetDlgItem(IDC_BUTTON_DELETE_R)->EnableWindow(enable_delete && g_work != NULL && g_bore_hole->m_prev_selected_child_id != g_work->m_id);
	
	GetDlgItem(IDOK)->EnableWindow(g_work != NULL);
}

BOOL DlgWork::OnInitDialog() 
{
	PRJ_timer_saving_available = false;

	CDialog::OnInitDialog();

	m_old_project_saved = false;
	m_new_project_saved = false;
	m_failed_load = false;

	g_project = PRJ_GetProject();

	if (m_mode == DlgWork_IMPORT)
	{
		CFileDialog dlg(TRUE, NULL, STAGES_last_file_path, OFN_HIDEREADONLY, "Файлы конфигурации(*.cfg)|*.cfg||");
		if (dlg.DoModal() != IDOK)
		{
			EndDialog(IDCANCEL);
			return FALSE;
		}
		g_importing_work_path = STAGES_last_file_path = dlg.GetPathName();
	}
	else
	if (g_project->m_ok)
		g_importing_work_path = PRJ_GetWorkPath();
	else
		g_importing_work_path = "";

	SetWindowText(" " + m_title);

	int show_mode = (m_mode & DlgWork_OPEN) > 0 ? SW_NORMAL : SW_HIDE;
	GetDlgItem(IDOK)->ShowWindow(show_mode);
	show_mode = (m_mode & DlgWork_NEW) > 0 ? SW_NORMAL : SW_HIDE;
	m_combo_template.ShowWindow(show_mode);
	GetDlgItem(IDC_STATIC_TEMPLATE)->ShowWindow(show_mode);
	if (show_mode == SW_NORMAL)
	{
		m_combo_template.AddString("последний созданный");
		m_combo_template.AddString("текущий");
		m_combo_template.AddString("пустой");
		m_combo_template.SetCurSel(0);
	}

	if (g_project->m_name.IsEmpty())
		g_project->CreateDefault();
	MemorizeSelectedBranch();
	AdjustCombos();
	return TRUE;
}

void DlgWork::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_M, m_combo_m);
	DDX_Control(pDX, IDC_COMBO_K, m_combo_k);
	DDX_Control(pDX, IDC_COMBO_S, m_combo_s);
	DDX_Control(pDX, IDC_COMBO_R, m_combo_r);

	DDX_Control(pDX, IDC_COMBO_TEMPLATE, m_combo_template);
}

BEGIN_MESSAGE_MAP(DlgWork, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD_M, &DlgWork::OnBnClickedButtonAddM)
	ON_CBN_SELCHANGE(IDC_COMBO_M, &DlgWork::OnCbnSelchangeComboM)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_M, &DlgWork::OnBnClickedButtonEditM)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_M, &DlgWork::OnBnClickedButtonDeleteM)
	ON_CBN_SELCHANGE(IDC_COMBO_K, &DlgWork::OnCbnSelchangeComboK)
	ON_BN_CLICKED(IDC_BUTTON_ADD_K, &DlgWork::OnBnClickedButtonAddK)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_K, &DlgWork::OnBnClickedButtonEditK)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_K, &DlgWork::OnBnClickedButtonDeleteK)
	ON_CBN_SELCHANGE(IDC_COMBO_S, &DlgWork::OnCbnSelchangeComboS)
	ON_BN_CLICKED(IDC_BUTTON_ADD_S, &DlgWork::OnBnClickedButtonAddS)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_S, &DlgWork::OnBnClickedButtonEditS)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_S, &DlgWork::OnBnClickedButtonDeleteS)
	ON_CBN_SELCHANGE(IDC_COMBO_R, &DlgWork::OnCbnSelchangeComboR)
	ON_BN_CLICKED(IDC_BUTTON_ADD_R, &DlgWork::OnBnClickedButtonAddR)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_R, &DlgWork::OnBnClickedButtonEditR)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_R, &DlgWork::OnBnClickedButtonDeleteR)
	ON_BN_CLICKED(IDOK, &DlgWork::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &DlgWork::OnBnClickedCancel)
END_MESSAGE_MAP()

void DlgWork::SelectChild(PRJ_Object* obj, CComboBox& combo)
{
	ASSERT(obj != NULL);
	int id = combo.GetCurSel();
	obj->m_selected_child_id = (id == -1) ? 0 : combo.GetItemData(id);
	AdjustCombos();
}

void CopyParam(int num, CParam *param, CString file_name, CString* total)
{
	CString number_str;
	number_str.Format(" %d", num);
	*total += number_str;
	param->m_num_par = num;

	KRS_app.SetRegistryMode();
	KRS_app.ReadParamFromRegistry(param);
	KRS_app.SetIniFileMode(file_name);
	KRS_app.WriteParamToRegistry(param, true);
}

void FillList(CString prefix, list<CString> *lst)
{
	lst->clear();
	int counter = 1;
	char val[2048];
	while (true)
	{
		if (KRS_config_file.ReadPar(prefix + (TXT("_%d")<<counter), val, "*") != CFGF_OK)
			break;
		if (val[0] == '*')
			break;
		lst->push_back(CString(val));
		counter++;
	}
}

void CopyAdditionalFiles()
{
	CString new_work_path = g_work->GetFullPath(), old_work_file = g_importing_work_path;
	list<CString>::iterator current_file_name;
	list<CString> file_list;
	FillList("COPY_FILE", &file_list);
	current_file_name = file_list.begin();
	while (current_file_name != file_list.end())
	{
		CopyFile(old_work_file + "\\" + *current_file_name, new_work_path + "\\" + *current_file_name, TRUE);
		current_file_name++;
	}

	FillList("COPY_FILE_FROM_ROOT", &file_list);
	current_file_name = file_list.begin();
	while (current_file_name != file_list.end())
	{
		CopyFile(KRS_root_path + "\\" + *current_file_name, new_work_path + "\\" + *current_file_name, TRUE);
		current_file_name++;
	}
}

void DlgWork::CopyFilesFromRoot()
{
	KRS_app.SetRegistryMode();
	CString devices_cfg = KRS_app.GetProfileString("DEVICES", "Config file name", KRS_root_path + "\\default.cfg");
	CString new_work_path = g_work->GetFullPath();
	CopyFile(KRS_root_path + "\\main.sv", new_work_path + "\\main.sv", TRUE);
	CopyFile(KRS_root_path + "\\main1.sv", new_work_path + "\\main1.sv", TRUE);
	CopyFile(KRS_root_path + "\\main2.sv", new_work_path + "\\main2.sv", TRUE);
	CopyFile(KRS_root_path + "\\bo.cfg", new_work_path + "\\БО.cfg", TRUE);
	CopyFile(devices_cfg, new_work_path + "\\Устройства.cfg", TRUE);
	if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
	{
		if (m_mode == DlgWork_IMPORT)
			CopyFile(g_importing_work_path, new_work_path + "\\stages.cfg", TRUE);
		else
			CopyFile(KRS_root_path + "\\stages.cfg", new_work_path + "\\stages.cfg", TRUE);
	}

	PRJ_ReadWorkFile(true); 
	PRJ_WriteWorkFile(); 

	CString section, num_list_prefix = "List", entry, text, file_path = new_work_path + "\\Панели.ini";
	int view_num, i, j, max_j = 6;
	for (view_num = 0; view_num < 2; view_num++)
	{
		for (i = 0; i < 5; i++)
		{
			section.Format("%d", i);
			section = num_list_prefix + section;
			for (j = 0; j < max_j; j++)
			{
				entry.Format("Panel%d", j);
				KRS_app.SetRegistryMode();
				text = KRS_app.GetProfileString(section, entry, "$ 0:");
				KRS_app.SetIniFileMode(file_path);
				KRS_app.WriteProfileString(section, entry, text);
			}
		}
		num_list_prefix += "DB";
		max_j = 3;
	}
	CString total;
	int count = 0;
	file_path = new_work_path + "\\Параметры.ini";
	CParam param;
	for (i = 1; i < 24; i++)
	{
		CopyParam(i, &param, file_path, &total);
		count++;
	}
	KRS_app.SetRegistryMode();
	CString additional_param_nums = KRS_app.GetProfileString("Settings", "дополнительные параметры", "");
	if (additional_param_nums.GetLength() > 0)
	{
		char *str = additional_param_nums.GetBuffer(1024);
		char *str_ptr = str;
		int num, index = 0;
		while (true)
		{
			if (sscanf(str_ptr, "%d", &num) != 1)
				break;
			CopyParam(num, &param, file_path, &total);
			count++;
			index = additional_param_nums.Find(",", index);
			if (index == -1)
				break;
			index++;
			str_ptr = str + index;
		}
	}
	text.Format("%d:", count);
	total = text + total;
	KRS_app.WriteProfileString("total", "params", total);
	KRS_app.SetRegistryMode();
}

BOOL CopyFilesFromFolder(CString from_path, CString to_path)
{
	BOOL res = TRUE;

	CString new_work_path = to_path, old_work_file = from_path;
	list<CString>::iterator current_file_name;
	list<CString> file_list;

	// копирование доп.файлов + основных из исходной папки
	FillList("COPY_FILE", &file_list);

	file_list.push_back("main.sv");
	file_list.push_back("main1.sv");
	file_list.push_back("main2.sv");
	file_list.push_back("БО.cfg");
	file_list.push_back("stages.cfg");
	file_list.push_back("Устройства.cfg");
	file_list.push_back("Панели.ini");
	file_list.push_back("Параметры.ini");
	file_list.push_back("Работа.ini");

	current_file_name = file_list.begin();
	while (current_file_name != file_list.end())
	{
		if (CopyFile(old_work_file + "\\" + *current_file_name, new_work_path + "\\" + *current_file_name, TRUE) == FALSE)
			res = FALSE;
		current_file_name++;
	}

	// копирование доп.файлов + из корневой папки
	FillList("COPY_FILE_FROM_ROOT", &file_list);
	current_file_name = file_list.begin();
	while (current_file_name != file_list.end())
	{
		if (CopyFile(KRS_root_path + "\\" + *current_file_name, new_work_path + "\\" + *current_file_name, TRUE) == FALSE)
			res = FALSE;
		current_file_name++;
	}
	
	return res;
}

BOOL CopyFolder(CString from_path, CString to_path)
{
	CString src_path, dst_path;
	BOOL result = FALSE;

	WIN32_FIND_DATA fd = {};
	HANDLE hFind = FindFirstFile(from_path + "\\*", &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		result = TRUE;
		do
		{
			if (!(strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..")))
				continue;
			src_path = from_path + "\\" + fd.cFileName;
			dst_path = to_path + "\\" + fd.cFileName;

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				result = CopyFolder(src_path, dst_path); 
			else
			{
				result = TRUE == CopyFile(src_path, dst_path, TRUE);
			}

		} while(result && FindNextFile(hFind, &fd));
		FindClose( hFind);
	}
	return result;
}

void NullifyProject()
{
	gfDLi = gGlI = gGlX = 0;
	gfTalblokDeltaH = gGlubinaZaboia = gGlubina = 0;
	if (pGlubinaZaboia != NULL)
	{
		pGlubinaZaboia->fCur = pGlubinaZaboia->fCur = pGlubinaZaboia->fCurAvg = pGlubinaZaboia->fCurPrevious = 0;
		pZaboi->fCur = pZaboi->fCur = pZaboi->fCurAvg = pZaboi->fCurPrevious = gIZaboiValue = 0;
		pGlubina->fCur = pGlubina->fCur = pGlubina->fCurAvg = pGlubina->fCurPrevious = 0;
	}
	gtblElement.SetNumCurElement(0); 
}

void DlgWork::AddChild(PRJ_Object* parent, PRJ_Object* obj, CComboBox& combo, CString initial_text)
{
	ASSERT(parent != NULL && obj != NULL);
	CRect r;
	combo.GetWindowRect(&r);
	r.DeflateRect(2, 2);
	int template_num = m_combo_template.GetCurSel();

	DlgEditText dlg(initial_text, r, false, 0);
	if (dlg.DoModal() != IDOK)
	{
		delete obj;
		return;
	}
	
	obj->m_name = dlg.GetText();

	obj->m_name.TrimLeft();
	obj->m_name.TrimRight();
	if (obj->m_name.GetLength() == 0)
	{
		MessageBox("Недопустимое имя работы", "Имя не может быть пустым, или содержать одни пробелы");
		delete obj;
		return;
	}

	parent->AddChild(obj);
	parent->m_selected_child_id = obj->m_id;
	AdjustCombos();
	if (obj == g_work)
	{
		if (m_mode != DlgWork_IMPORT && g_project->m_ok) 
		{
			PRJ_SaveProject(PRJ_PrevSelectedBranch, PRJ_SaveAllProject); 
			m_old_project_saved = true;
		}

		if (template_num == 2) // пустой
		{
			KRS_ClearMap(&m_MapParam, false);
			KRS_ClearMap(&m_MapParamDefault);
			KRS_ClearMap(&m_MapParamDB);
			pFrm->SetInitialData();
			SERV_current_device_cfg->Clear();
			SERV_ClearListBO(SERV_indication_blocks);
			if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
				STAGES_ClearAllStages();
		}

		if (g_project->m_error_on_start || m_mode == DlgWork_IMPORT)
		{
			if (g_project->m_error_on_start) 
			{
				g_project->m_ok = true;
				if (KRS_project_flag == 0)
					KRS_project_flag = KRS_programm_flag;
			}
			CopyFilesFromRoot();
			PRJ_SaveProject(PRJ_NowSelectedBranch, PRJ_SaveTreeOnly); 
			CopyAdditionalFiles();
		}
		else
		{
			if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
				STAGES_SetCurrentMode(STAGES_MODE_EDIT);

			if (template_num == 0) // последний
			{
				KRS_app.SetRegistryMode(true);
				CString str = KRS_app.GetProfileString("info", "last_created_work", "");

				if (str.GetLength() == 0)
				{
					MessageBox("Не удалось найти последний созданный проект\nШаблон изменен на \"текущий\"", "Проблема при создании нового проекта");
					template_num = 1;
				}
				if (CopyFilesFromFolder(str, g_work->GetFullPath()) == FALSE)
				{
					MessageBox("При копировании файлов из папки последнего созданного проекта не все файлы были скопированы успешно", "Проблема при создании нового проекта");
				}
				
				// затираем моду в 0
				if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
				{
					STAGES_ChangeModeToEdit(g_work->GetFullPath());
				}
			}

			NullifyProject();
			if (template_num == 1) // текущий
				PRJ_SaveProject(PRJ_NowSelectedBranch, PRJ_SaveAllProject); 
			DB_connection_info.m_db_name = PRJ_CreateDBName(PRJ_NowSelectedBranch);
			PRJ_SetCreatedDB(false);
			CopyAdditionalFiles();
			m_new_project_saved = true;

			KRS_app.SetRegistryMode(true);
			KRS_app.WriteProfileString("info", "last_created_work", g_work->GetFullPath());
		}
		OnBnClickedOk();
	}
}

void DlgWork::RenameObject(PRJ_Object* obj, CComboBox& combo)
{
	ASSERT(obj != NULL);
	CRect r;
	combo.GetWindowRect(&r);
	r.DeflateRect(2, 2);

	DlgEditText dlg(obj->m_name, r, false, 0);
	if (dlg.DoModal() == IDOK)
	{
		CString old_path = obj->GetFullPath();
		obj->m_name = dlg.GetText();
		CString new_path = obj->GetFullPath();
		MoveFile(old_path, new_path);
		AdjustCombos();
	}
	g_project->SaveProject(PRJ_NowSelectedBranch, PRJ_SaveTreeOnly); 
}

void DlgWork::DeleteObject(PRJ_Object* object, CString class_name)
{
	ASSERT(object != NULL);
	PRJ_Object* parent = object->m_parent;
	ASSERT(parent != NULL);
	if (parent->m_prev_selected_child_id == object->m_id)
	{
		MessageBox("Удаление объектов в ветви текущей работы запрещено.\nДля получения возможности удаления откройте другую работу.", " Невозможно выполнить операцию", MB_OK);
		return;
	}
	if (MessageBox("Удалить " + class_name + " \"" + object->m_name + "\"?", "Удалить " + class_name + "?", MB_YESNO) == IDYES)
	{
		parent->m_selected_child_id = 0;
		if (!RemoveDir(object->GetFullPath()))
			MessageBox("Ошибка при удалении файлов", " Ошибка при удалении файлов", MB_OK);
		parent->RemoveChild(object, true);
		AdjustCombos();
		g_project->SaveProject(PRJ_NowSelectedBranch, PRJ_SaveTreeOnly); 
	}
}

void DlgWork::OnCbnSelchangeComboM()
{
	SelectChild(g_project, m_combo_m);
}

void DlgWork::OnBnClickedButtonAddM()
{
	g_deposit = new PRJ_Deposit();
	AddChild(g_project, g_deposit, m_combo_m, "<название месторождения>");
}

void DlgWork::OnBnClickedButtonEditM()
{
	RenameObject(g_deposit, m_combo_m);
}

void DlgWork::OnBnClickedButtonDeleteM()
{
	DeleteObject(g_deposit, "месторождение");
}

void DlgWork::OnCbnSelchangeComboK()
{
	SelectChild(g_deposit, m_combo_k);
}

void DlgWork::OnBnClickedButtonAddK()
{
	g_cluster = new PRJ_Cluster();
	AddChild(g_deposit, g_cluster, m_combo_k, "<название куста>");
}

void DlgWork::OnBnClickedButtonEditK()
{
	RenameObject(g_cluster, m_combo_k);
}

void DlgWork::OnBnClickedButtonDeleteK()
{
	DeleteObject(g_cluster, "куст");
}

void DlgWork::OnCbnSelchangeComboS()
{
	SelectChild(g_cluster, m_combo_s);
}

void DlgWork::OnBnClickedButtonAddS()
{
	g_bore_hole = new PRJ_BoreHole();
	AddChild(g_cluster, g_bore_hole, m_combo_s, "<название скважины>");
}

void DlgWork::OnBnClickedButtonEditS()
{
	RenameObject(g_bore_hole, m_combo_s);
}

void DlgWork::OnBnClickedButtonDeleteS()
{
	DeleteObject(g_bore_hole, "скважину");
}

void DlgWork::OnCbnSelchangeComboR()
{
	SelectChild(g_bore_hole, m_combo_r);
}

void DlgWork::OnBnClickedButtonAddR()
{
	g_work = new PRJ_Work();
	AddChild(g_bore_hole, g_work, m_combo_r, "<название работы>");
}

void DlgWork::OnBnClickedButtonEditR()
{
	RenameObject(g_work, m_combo_r);
}

void DlgWork::OnBnClickedButtonDeleteR()
{
	DeleteObject(g_work, "работу");
}

void DlgWork::OnBnClickedOk()
{
	if (m_mode == DlgWork_IMPORT)
	{
		RestoreSelectedBranch();
		PRJ_ReadWorkFile(false); 
	}
	else
	{
		if (m_mode | DlgWork_OPEN)
		{
			if (!m_old_project_saved && g_project->m_ok)
				PRJ_SaveProject(PRJ_PrevSelectedBranch, PRJ_SaveAllProject); 
			if (!m_new_project_saved)
				PRJ_SaveProject(PRJ_NowSelectedBranch, PRJ_SaveTreeOnly); 
		}
		if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
			STAGES_StopGettingFromDB();
		KSR_StopAllConnections();
		PRJ_LoadProjectResult res = PRJ_LoadProject(PRJ_GetProjectPath());
		if (res != PRJ_LoadProjectOK) 
		{
			m_failed_load = true;
			g_project->m_ok = false;
			MessageBox(" Не удалось открыть работу", CString(TXT("Код ошибки %d") << res), MB_OK);
			MemorizeSelectedBranch();
			AdjustCombos();
			return;
		}
		if (m_new_project_saved)
		{
			NullifyProject();

/*			if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
			{
				STAGES_SetCurrentMode(STAGES_MODE_EDIT);
				PRJ_SaveProject(PRJ_PrevSelectedBranch, PRJ_SaveAllProject); 
				PRJ_LoadProjectResult res = PRJ_LoadProject(PRJ_GetProjectPath());
			}
*/
		}
		KSR_StartAllConnections();
	}
	g_project->ResetPrevSelected();
	OnOK(); 
	PRJ_timer_saving_available = true;
}

void DlgWork::OnBnClickedCancel()
{
	RestoreSelectedBranch();
	g_project->ResetPrevSelected();
	if (m_failed_load)
	{
		KSR_StopAllConnections();
		PRJ_LoadProject(PRJ_GetProjectPath());
		KSR_StartAllConnections();
	}
	PRJ_SaveInfoIntoRegistry(); 
	OnCancel();
	PRJ_timer_saving_available = true;
}

////////////////////////////////////////////////////////////////////////////////
// end