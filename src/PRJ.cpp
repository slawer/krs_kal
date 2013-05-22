#include <StdAfx.h>

#include <math.h>
#include <io.h>

#include "PRJ.h"
#include "CONV.h"
#include "SERV.h"
#include "DB.h"
#include "LOG_V0.h"
#include "..\KRS.h"
#include "..\MainFrm.h"
#include "..\MainView.h"
#include "..\DlgWork.h"
#include "..\CementView.h"

static PRJ_Project g_project;

extern typeView gt_cement;
extern CCementView* g_cement_view;

static bool g_is_db_for_current_project_created = true;

bool PRJ_timer_saving_available = true;

PRJ_Project* PRJ_GetProject()
{
	return &g_project;
}

PRJ_LoadProjectResult PRJ_LoadProject(CString path)
{
	PRJ_LoadProjectResult res = g_project.LoadProject(path);
	if (res == PRJ_LoadProjectOK)
		PRJ_SaveInfoIntoRegistry();
	return res;
}

void PRJ_CreateDefaultProject()
{
	g_project.CreateDefault();
}

bool PRJ_SaveProject(PRJ_Branch branch, PRJ_SaveProjectMode mode, CProgressCtrl* progress)
{
	return g_project.SaveProject(branch, mode, progress);
}

CString PRJ_GetProjectPath()
{
	return g_project.m_path;
}

static CString g_importing_work_path;

CString PRJ_GetWorkPath(PRJ_Branch branch)
{
	CString res = g_project.m_path;
	PRJ_Object *obj = &g_project, *obj2;
	int count = 0;
	while (true)
	{
		obj2 = obj->GetSelectedChild(branch);
		count++;
		if (obj2 == NULL)
			break;
		obj = obj2;
	}
	if (count != 5)
	{
		if (!g_project.m_error_in_tree_reported)
		{
			g_project.m_error_in_tree_reported = true;
		}
		return "";
	}
	return obj->GetFullPath();
}

void PRJ_CleanUp()
{
	g_project.DeleteChilds();
}
PRJ_Object::PRJ_Object():
	m_max_id(0), m_id(0), m_selected_child_id(0), m_prev_selected_child_id(0), m_parent(NULL)
{
}

PRJ_Object::~PRJ_Object()
{
	DeleteChilds();
}

void PRJ_Object::DeleteChilds()
{
	PRJ_Objects::iterator current = m_childs.begin();
	while (current != m_childs.end())
	{
		current->second->DeleteChilds();
		delete current->second;
		current++;
	}
	m_childs.clear();
	m_max_id = 0;
}

void PRJ_Object::ResetPrevSelected()
{
	m_prev_selected_child_id = 0;
	PRJ_Objects::iterator current = m_childs.begin();
	while (current != m_childs.end())
	{
		current->second->ResetPrevSelected();
		current++;
	}
}

void PRJ_Object::AddChild(PRJ_Object* child)
{
	m_max_id++;
	child->m_id = m_max_id;
	child->m_parent = this;
	ASSERT(m_childs.find(child->m_id) == m_childs.end());
	m_childs[child->m_id] = child;
	CString str = GetFullPath() + "\\" + child->GetClassDir();
	if (CreateDirectory(str, NULL) != TRUE)
		LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "PRJ_Object::AddChild - Не создалась директория \"" + str + "\"");
	str = child->GetFullPath();
	if (CreateDirectory(str, NULL) != TRUE)
		LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "PRJ_Object::AddChild - Не создалась директория \"" + str + "\"");
}

bool PRJ_Object::RemoveChild(PRJ_Object* child, bool remove_and_delete)
{
	PRJ_Objects::iterator desired = m_childs.find(child->m_id);
	if (desired == m_childs.end())
		return false;
	m_childs.erase(child->m_id);
	if (remove_and_delete)
		delete child;
	else
		child->m_parent = NULL;
	return true;
}

PRJ_Object* PRJ_Object::GetSelectedChild(PRJ_Branch branch)
{
	int id = branch == PRJ_PrevSelectedBranch ? m_prev_selected_child_id : m_selected_child_id;
	if (id == 0 || m_childs.find(id) == m_childs.end())
		return NULL;
	return m_childs[id];
}

CString PRJ_Object::GetFullPath()
{
	return m_parent->GetFullPath() + "\\" + GetClassDir() + "\\" + GetOwnDir();
}

CString PRJ_Object::GetOwnDir()
{
	CString res1 = m_name;
	res1.Replace('\\', '-');
	res1.Replace('/', '-');
	res1.Replace(':', ';');
	res1.Replace('*', '+');
	res1.Replace('?', '!');
	res1.Replace('"', '\'');
	res1.Replace('<', '(');
	res1.Replace('>', ')');
	res1.Replace('|', '!');
	res1.TrimLeft();
	res1.TrimRight();
	CString res2;
	res2.Format("%04d ", m_id);
	return res2 + res1;
}

void PRJ_Object::SaveInfo(PRJ_Branch branch)
{
	KRS_app.WriteProfileString("INFO", GetClassName(), m_name);
	int id = branch == PRJ_PrevSelectedBranch ? m_prev_selected_child_id : m_selected_child_id;
	if (id == 0 || m_childs.find(id) == m_childs.end())
		return;
	m_childs[id]->SaveInfo(branch);
}

bool PRJ_Object::Load(CArchive& archive)
{
	DeleteChilds();

	if (!IgnoreIDs())
	{
		archive>>m_id;
		//archive>>m_max_id;
		archive>>m_name;
	}

	if (!OwnLoad(archive))
		return false;

	UINT number_of_childs, i;
	archive >> number_of_childs;

	PRJ_Object* child;
	for (i = 0; i < number_of_childs; i++)
	{
		child = CreateChild();
		if (child == NULL)
			return false;
		if (!child->Load(archive))
		{
			delete child;
			return false;
		}
		m_childs[child->m_id] = child;
		child->m_parent = this;
		if (m_max_id < child->m_id)
			m_max_id = child->m_id;
	}

	archive >> m_selected_child_id;

	return true;
}

bool PRJ_Object::Save(CArchive& archive)
{
	if (!IgnoreIDs())
	{
		archive<<m_id;
		//archive<<m_max_id;
		archive<<m_name;
	}

	if (!OwnSave(archive))
		return false;

	archive << UINT(m_childs.size());

	PRJ_Objects::iterator current = m_childs.begin();
	while (current != m_childs.end())
	{
		if (!current->second->Save(archive))
			return false;
		current++;
	}

	archive << m_selected_child_id;

	return true;
}
PRJ_Project::PRJ_Project():
	m_now_saving(false),
	m_ok(false), m_error_on_start(true), m_error_in_tree_reported(false)
{
}

CString PRJ_Project::GetFullPath()
{
	return m_path;
}

CString PRJ_Project::GetProjectFilePath()
{
	return m_path + "\\" + m_name + ".project";
}

bool PRJ_Project::CreateDefault()
{
	DeleteChilds();

	m_name = "Проект";
	m_path = KRS_root_path + "\\" + m_name;
	KRS_project_flag = KRS_programm_flag;

	if (CreateDirectory(m_path, NULL) != TRUE)
		return false;

	if (CreateDirectory(m_path + "\\Месторождения", NULL) != TRUE)
		return false;

	return true;
}

void PRJ_Project::ReportError(CString err)
{
	MessageBox(NULL, err, "ОШИБКА", MB_OK);
}

bool GetNum(CString* str, int& num)
{
	if (sscanf(*str, "%d", &num) < 1)
		return false;
	int signs = 1;
	while (num >= pow(10, float(signs)))
		signs++;
	str->Delete(0, signs);
	return true;
}

void PRJ_OnStagesReadyAfterLoad()
{
	m_wndDialogBarCM.AdjustButtonNames();

	CCementView* view = (CCementView*)KRS_app.GetView(t_cement_rt);
	view->SetColorInterface();

	if (STAGES_GetCurrentMode() == STAGES_MODE_ARCHIVE)
		view->RequestDataFromDB();
	else
		view->m_need_update_from_db = false;
}

int g_loading_recursion = 0;
struct Recurser
{
	Recurser()
	{
		g_loading_recursion++;
	}
	~Recurser()
	{
		g_loading_recursion--;
	}
};

PRJ_LoadProjectResult PRJ_Project::LoadProject(CString path)
{
	Recurser recurser;
	m_ok = false;
	m_stages_loaded = false;

	{
		CFileException fe;
		CFile* file = new CFile;
		if (file == NULL)
			return PRJ_LoadProjectFileError;
		CString prev_name = m_name, prev_path = m_path;
		int index = path.ReverseFind('\\');
		if (index == -1)
		{
			delete file;
			return PRJ_LoadProjectFileError;
		}
		m_name = path.Right(path.GetLength() - index - 1);
		m_path = path;
		
		CString str = GetProjectFilePath();
		str.TrimLeft();
		str.TrimRight();

		if (!file->Open(str, CFile::modeRead|CFile::shareDenyWrite, &fe))
		{
			delete file;
			if (g_loading_recursion == 2)
			{
				ReportError("файл ПРОЕКТА отсутствует, или повреждён");
				m_name = prev_name;
				m_path = prev_path;
				return PRJ_LoadProjectFileError;
			}
			PRJ_RestoreProjectByDirectories(path);
			return LoadProject(path);
		}
		CArchive load_archive(file, CArchive::load | CArchive::bNoFlushOnDelete);
		load_archive.m_bForceFlat = FALSE;
		bool ok = Load(load_archive);
		load_archive.Close();
		file->Close();
		delete file;
		if (!ok)
		{
			if (g_loading_recursion == 2)
			{
				ReportError("файл РАБОТЫ отсутствует, или повреждён");
				return PRJ_LoadWorkFileError;
			}
			PRJ_RestoreProjectByDirectories(path);
			return LoadProject(path);
		}
	}

	CString work_path = PRJ_GetWorkPath();
	if (work_path.IsEmpty())
		return PRJ_LoadProjectFailed;
	if (!PRJ_ReadWorkFile(false))
		return PRJ_LoadProjectFailed;
	pFrm->KillTimer(3); 
	pFrm->KillTimer(4); 

	KRS_DeleteAllViews();

	KRS_ClearMap(&m_MapParam, false);
	KRS_ClearMap(&m_MapParamDefault);
	KRS_ClearMap(&m_MapParamDB);

	pFrm->SetInitialData();
	CParam* param;
	KRS_app.SetIniFileMode(work_path + "\\Параметры.ini");
	CString all_numbers_str = KRS_app.GetProfileString("total", "params", "0:");
	int i, max_i, num;
	bool is_time;
	if (GetNum(&all_numbers_str, max_i))
	{
		all_numbers_str.Delete(0, 1); 
		for (i = 0; i < max_i; i++)
		{
			all_numbers_str.Delete(0, 1); 
			if (!GetNum(&all_numbers_str, num))
			{
				pFrm->MessageBox("[total] params x: ... %%d", "ОШИБКА чтения файла Параметры.ini", MB_OK);
				break;
			}
			if (!m_MapParamDefault.Lookup(num, param))
			{
				param = new CParam();
				param->m_num_par = num;
				m_MapParamDefault[param->m_num_par] = param;
				is_time = false;
			}
			else
				is_time = param->m_shown_digits == 0xFF;
			KRS_app.ReadParamFromRegistry(param);
			if (is_time)
				param->m_shown_digits = 0xFF;
		}
	}
	else
	{
		pFrm->MessageBox("[total] params %%d:", "ОШИБКА чтения файла Параметры.ini", MB_OK);
		return PRJ_LoadProjectFailed;
	}
	KRS_MakeupFormulas();
	SERV_DeviceCfgInit(work_path + "\\Устройства.cfg");
	SERV_LoadConfigBO(work_path);
	CMainView* pView;
	pView = (CMainView*)KRS_app.LoadDocArchive(t_real_time, work_path);
	if (pView != NULL)
	{
		pView->ReloadDataView();
		pView->PostMessage(PM_CHANGE_ACTIVE_LIST, -1, NULL);
	}
	pView = (CMainView*)KRS_app.LoadDocArchive(t_archive, work_path);
	if (pView != NULL)
	{
		pView->ReloadDataView();
		pView->PostMessage(PM_CHANGE_ACTIVE_LIST, -1, NULL);
	}
	
	if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
		pView = (CMainView*)KRS_app.LoadDocArchive(t_cement_rt, work_path);
	KRS_app.SetIniFileMode(work_path + "\\Панели.ini");
	int type_of_view;
	for (type_of_view = t_real_time; type_of_view < t_cement_rt; type_of_view++)
	{
		pView = (CMainView*)(KRS_app.GetView((typeView)type_of_view));
		if (pView != NULL)
		{
			pView->RestoreAllLists();
			pView->SetColorInterface();
		}
	}
	KRS_app.SetRegistryMode();
	m_wndDialogBarH.GetDlgItem(IDC_RADIO_ARCHIVE)->EnableWindow(FALSE);

	if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
	{
		STAGES_LoadResult res = STAGES_LoadStages(false, false);

		if (res == STAGES_LoadCancelled)
			return PRJ_LoadProjectFailed;

		if (res == STAGES_LoadFailed)
			res = STAGES_LoadStages(false, true);

		if (res == STAGES_LoadFailed)
		{
			if (pFrm->MessageBox("Сбой при загрузке этапов. Продолжить работать с пустым списком этапов?", " Ошибка при загрузке работы", MB_YESNO) != IDYES)
				return PRJ_LoadProjectFailed; 
			res = STAGES_LoadOK;
		}
		if (res == STAGES_LoadOK)
		{
			m_stages_loaded = true;
			PRJ_OnStagesReadyAfterLoad();
		}
		m_wndDialogBarH.GetDlgItem(IDC_RADIO_CEMENT)->EnableWindow(res == STAGES_LoadOK);
	}

	/**/
    pFrm->SetViewDgtRT();
    pFrm->SetViewPanelBur(true);

    pFrm->SetHighPanel();
    m_wndDialogBarH.SetViewRealArch();
    m_wndDialogBarH.SetViewTimeGlub();
    m_wndDialogBarH.OutputNKBK(0);
    m_wndDialogBarH.OutputWk(0, false);

    KRS_app.ChangeStyleAllView(FLD_DATA_ON_FIELD, gflgDataBegEnd);
	KRS_app.ChangeStyleAllView(FLD_LINE_JAGGIES, gflgGrfStupenki);
	KRS_app.ChangeStyleAllView(FLD_SELECT_ONLY_Y, gflgSelectOnlyY);
    pFrm->SetGraphDigit();

    m_wndDialogBar.SetViewDiag();
    m_wndDialogBar.SetIndParamBur();
    m_wndDialogBar.SetIndDataTime();
    
    pFrm->SetTimer(3, 1000, NULL); 
	/**/
	
	m_path = path;
	m_ok = true;
	m_error_in_tree_reported = false;
	m_error_on_start = false;

	return PRJ_LoadProjectOK;
}

void PRJ_SaveInfoIntoRegistry()
{
	if (!g_project.m_ok)
		return;

	gMestorogdenie = "";
	gKust = "";
	gSkvagina = "";
	gRabota = "";

	PRJ_Object* deposit = g_project.GetSelectedChild();
	if (deposit)
	{
		gMestorogdenie = deposit->m_name;
		PRJ_Object* cluster = deposit->GetSelectedChild();
		if (cluster)
		{
			gKust = cluster->m_name;
			PRJ_Object* bore_hole = cluster->GetSelectedChild();
			if (bore_hole)
			{
				gSkvagina = bore_hole->m_name;
				PRJ_Object* work = bore_hole->GetSelectedChild();
				if (work)
				{
					gRabota = work->m_name;
				}
			}
		}
	}
	KRS_app.SetRegistryMode(true);
	KRS_app.WriteProfileString("info", "project_path", g_project.m_path);
	KRS_app.WriteProfileString("info", "work_path", PRJ_GetWorkPath());
	KRS_app.WriteProfileString("info", "deposit", gMestorogdenie);
	KRS_app.WriteProfileString("info", "cluster", gKust);
	KRS_app.WriteProfileString("info", "borehole", gSkvagina);
	KRS_app.WriteProfileString("info", "work", gRabota);
}

void PRJ_WritePanels(PRJ_Branch branch, bool close_file)
{
	CString str = PRJ_GetWorkPath(branch);
	if (str.IsEmpty())
		return;
	KRS_app.SetIniFileMode(str + "\\Панели.ini");
	int type_of_view;
	CMainView *pView;
	for (type_of_view = t_real_time; type_of_view < t_cement_rt; type_of_view++)
	{
		pView = (CMainView*)(KRS_app.GetView((typeView)type_of_view));
		if(pView != NULL)
		{
			pView->SaveAllLists();
		}
	}
	if (close_file)
		KRS_app.SetRegistryMode();
}

void PRJ_WriteWorkFile(PRJ_Branch branch)
{
	CString str = PRJ_GetWorkPath(branch);
	if (str.IsEmpty())
		return;
	KRS_app.SetIniFileMode(str + "\\Работа.ini");
	KRS_app.WriteSection_NET();
	KRS_app.WriteSection_DB();
	KRS_app.WriteProfileInt("DB", "DB_CREATED", g_is_db_for_current_project_created?1:0);
	KRS_app.WriteProfileInt("INFO", "FLAG", KRS_project_flag);
	KRS_app.WriteProfileInt("INFO", "BO_INTERVAL", KRS_send_to_bo_interval);
	g_project.SaveInfo(branch);
	KRS_app.WriteFloatValueToRegistry("InitData", "PorogRotorBur", gPorogRotorBur);
	KRS_app.WriteFloatValueToRegistry("InitData", "VesRotorBur", gVesRotorBur);
    KRS_app.WriteIntegerValueToRegistry("InitData", "RabotaBezShurfa", gflgRabotaBezShurfa);
    KRS_app.WriteIntegerValueToRegistry("InitData", "12345", gflDisablePassWd);
	KRS_app.WriteFloatValueToRegistry("InitData", "gfLinstrumenta", gfLinstrumenta);
	KRS_app.WriteFloatValueToRegistry("InitData", "gfDLi", gfDLi);

	KRS_app.WriteIntegerValueToRegistry("ColorInterface", "color_graph_bk", g_color_interface.color_graph_bk);
	KRS_app.WriteIntegerValueToRegistry("ColorInterface", "color_graph_dgt", g_color_interface.color_graph_dgt);
	KRS_app.WriteIntegerValueToRegistry("ColorInterface", "color_graph_greed", g_color_interface.color_graph_greed);
	KRS_app.WriteIntegerValueToRegistry("ColorInterface", "color_panel_dgt_bk", g_color_interface.color_panel_dgt_bk);
	KRS_app.WriteIntegerValueToRegistry("ColorInterface", "color_panel_grf_bk", g_color_interface.color_panel_grf_bk);
	KRS_app.WriteIntegerValueToRegistry("ColorInterface", "color_graph_dgt_bk", g_color_interface.color_graph_dgt_bk);
	KRS_app.WriteIntegerValueToRegistry("ColorInterface", "StyleGreedGraph", g_StyleGreedGraph);

	if (KRS_project_flag & KRS_PRJ_FLAG_BURENIE)
	{
		KRS_app.WriteFloatValueToRegistry("InitData", "KRS_time_circ_lifetime", KRS_time_circ_lifetime);
		KRS_app.WriteFloatValueToRegistry("InitData", "KRS_time_bur_lifetime", KRS_time_bur_lifetime);
	}

	pApp->WriteFloatValueToRegistry("InitData","MexInterval", (float)gMexInterval);
	pApp->WriteFloatValueToRegistry("InitData","SPOInterval", (float)gSPOInterval);
	pApp->WriteFloatValueToRegistry("InitData","PorogRotorBur", (float)gPorogRotorBur);
	pApp->WriteFloatValueToRegistry("InitData","VesRotorBur", (float)gVesRotorBur);

    KRS_app.SetRegistryMode();
}

bool PRJ_ReadWorkFile(bool from_registry)
{
	if (from_registry)
		KRS_app.SetRegistryMode();
	else
	{
		CString str = PRJ_GetWorkPath();
		if (str.IsEmpty())
			return false;
		KRS_app.SetIniFileMode(str + "\\Работа.ini");
	}
	KRS_app.GetSection_NET();
	KRS_app.GetSection_DB();
	g_is_db_for_current_project_created = (0 != KRS_app.ReadIntegerValueFromRegistry("DB", "DB_CREATED", 1));
	if (from_registry)
	{
		KRS_project_flag = KRS_programm_flag;
	}
	else
	{
		KRS_project_flag = KRS_app.GetProfileInt("INFO", "FLAG", KRS_PRJ_FLAG_BURENIE | KRS_PRJ_FLAG_STAGES);
		if (KRS_project_flag == 0 || (KRS_project_flag & KRS_programm_flag) == 0)
		{
			g_project.ReportError("тип проекта не соответствует программе");
			return false;
		}
	}
	KRS_send_to_bo_interval = KRS_app.GetProfileInt("INFO", "BO_INTERVAL", 1000);

	gPorogRotorBur = KRS_app.ReadFloatValueFromRegistry("InitData", "PorogRotorBur", 50);
	gVesRotorBur = KRS_app.ReadFloatValueFromRegistry("InitData", "VesRotorBur", 50);
    gflgRabotaBezShurfa = KRS_app.ReadIntegerValueFromRegistry("InitData", "RabotaBezShurfa", 0);
    gflDisablePassWd = KRS_app.ReadIntegerValueFromRegistry("InitData", "12345", 0);
	gfLinstrumenta = KRS_app.ReadFloatValueFromRegistry("InitData", "gfLinstrumenta", 0);
	gfDLi = KRS_app.ReadFloatValueFromRegistry("InitData", "gfDLi", 0);

//    gflPassWd = gflDisablePassWd;
	g_color_interface.color_graph_bk = KRS_app.ReadIntegerValueFromRegistry("ColorInterface", "color_graph_bk", RGB(204,236,255));
	g_color_interface.color_graph_dgt = KRS_app.ReadIntegerValueFromRegistry("ColorInterface", "color_graph_dgt", RGB(0,0,255));
	g_color_interface.color_graph_greed = KRS_app.ReadIntegerValueFromRegistry("ColorInterface", "color_graph_greed", RGB(166,202,240));
	g_color_interface.color_panel_dgt_bk = KRS_app.ReadIntegerValueFromRegistry("ColorInterface", "color_panel_dgt_bk", GetSysColor(COLOR_MENU));
	g_color_interface.color_panel_grf_bk = KRS_app.ReadIntegerValueFromRegistry("ColorInterface", "color_panel_grf_bk", GetSysColor(COLOR_MENU));
	g_color_interface.color_graph_dgt_bk = KRS_app.ReadIntegerValueFromRegistry("ColorInterface", "color_graph_dgt_bk", GetSysColor(COLOR_MENU));
	g_StyleGreedGraph = KRS_app.ReadIntegerValueFromRegistry("ColorInterface", "StyleGreedGraph", 0);

	if (KRS_project_flag & KRS_PRJ_FLAG_BURENIE)
	{
		KRS_time_circ_lifetime = KRS_app.ReadFloatValueFromRegistry("InitData", "KRS_time_circ_lifetime", 0);
		KRS_time_bur_lifetime = KRS_app.ReadFloatValueFromRegistry("InitData", "KRS_time_bur_lifetime", 0);
	}

	gMexInterval = pApp->ReadFloatValueFromRegistry("InitData","MexInterval", 50);
	gSPOInterval = pApp->ReadFloatValueFromRegistry("InitData","SPOInterval", 50);
	gPorogRotorBur = pApp->ReadFloatValueFromRegistry("InitData","PorogRotorBur", 50);
	gVesRotorBur = pApp->ReadFloatValueFromRegistry("InitData","VesRotorBur", 50);

    if (!from_registry)
		KRS_app.SetRegistryMode();
	return true;
}

bool PRJ_Project::SaveProject(PRJ_Branch branch, PRJ_SaveProjectMode mode, CProgressCtrl* progress)
{
	struct SaveControl
	{
		SaveControl(PRJ_Project* prj): m_prj(prj)
		{
			if (m_prj->m_now_saving)
			{
				LOG_V0_AddMessage(LOG_V0_MESSAGE, "!!! Попытка сохранить сохраняемый проект");
				m_prj->m_now_saving = false; 
			}
			else
				m_prj->m_now_saving = true;
		}
		~SaveControl()
		{
			if (m_prj->m_now_saving == false)
				m_prj->m_now_saving = true; 
			else
				m_prj->m_now_saving = false;
		}
		PRJ_Project* m_prj;
	};

	SaveControl save_control(this);

	if (m_now_saving == false) 
		return false;

	if (branch == PRJ_PrevSelectedBranch && m_error_on_start)
		return true;

	{
		CFileException fe;
		CFile* file = new CFile;
		if (file == NULL)
			return false;
		if (!file->Open(GetProjectFilePath(), CFile::modeCreate | CFile::modeWrite | CFile::shareDenyRead, &fe))
		{
			LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "PRJ_Project::SaveProject - не открылся файл \"" + GetProjectFilePath() + "\"");
			delete file;
			return false;
		}
		CArchive save_archive(file, CArchive::store | CArchive::bNoFlushOnDelete);
		save_archive.m_bForceFlat = FALSE;
		bool res = Save(save_archive);
		save_archive.Close();
		file->Close();
		delete file;
		if (!res)
		{
			ReportError("сохранение файла проекта невозможно");
			return false;
		}
		if (progress)
			progress->SetPos(10);
	}

	if (mode == PRJ_SaveTreeOnly)
	{
		m_ok = true;
		return true;
	}

	CString work_path = PRJ_GetWorkPath(branch);
	if (work_path.IsEmpty())
	{
		LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "PRJ_Project::SaveProject - work_path.IsEmpty()");
		return false;
	}

	PRJ_WriteWorkFile(branch);
	if (progress)
		progress->SetPos(15);
	KRS_app.m_params_file = fopen(work_path + "\\Параметры.ini", "wt");
	if (KRS_app.m_params_file)
	{
		int key;
		CParam* param;
		POSITION pos = m_MapParamDefault.GetStartPosition();
		CString all_numbers_str, number_str;
		int params_count = 0, total_params = m_MapParamDefault.GetCount();
		all_numbers_str.Format("%d:", total_params);
		while (pos != NULL)
		{
			m_MapParamDefault.GetNextAssoc(pos, key, param);
			params_count++;
			KRS_app.WriteParamToRegistry(param);
			number_str.Format(" %d", key);
			if (progress)
				progress->SetPos(15 + 30 * params_count / total_params);
			all_numbers_str += number_str;
		}
		if (KRS_app.m_params_file)
			fputs("[total]\n", KRS_app.m_params_file);	
		KRS_app.WriteProfileString("total", "params", all_numbers_str);
		fclose(KRS_app.m_params_file);
		KRS_app.m_params_file = NULL;
	}
	SERV_current_device_cfg->Save(work_path + "\\Устройства.cfg");
	if (progress)
		progress->SetPos(50);
	SERV_SaveConfigBO(work_path);
	if (progress)
		progress->SetPos(55);
	if (m_error_on_start)
	{
		CopyFile(KRS_root_path + "\\main.sv", work_path + "\\main.sv", TRUE);
		CopyFile(KRS_root_path + "\\main1.sv", work_path + "\\main1.sv", TRUE);
		CopyFile(KRS_root_path + "\\main2.sv", work_path + "\\main2.sv", TRUE);
		if (progress)
			progress->SetPos(80);
	}
	else
	{
		KRS_app.SaveDocArchive(t_real_time, work_path);
		if (progress)
			progress->SetPos(60);

		KRS_app.SaveDocArchive(t_archive, work_path);
		if (progress)
			progress->SetPos(70);

		if (KRS_project_flag & KRS_PRJ_FLAG_STAGES)
		{
			KRS_app.SaveDocArchive(t_cement_rt, work_path);
			if (progress)
				progress->SetPos(80);
		}
	}
	PRJ_WritePanels(branch);
	if (progress)
		progress->SetPos(90);

	if (m_stages_loaded)
	{
		STAGES_SaveStages(false, branch);
		if (progress)
			progress->SetPos(100);
	}
		
	return true;
}

CString g_path_of_work;

void PRJ_Object::SetSelected()
{
	if (m_parent == NULL)
		return;
	m_parent->m_selected_child_id = m_id;
	m_parent->SetSelected();
}

bool PRJ_Object::RestoreByDir(CString path, int deepness)
{
	PRJ_Object* child = CreateChild();
	CString childs_path = path + "\\" + child->GetClassDir(), child_path_tmp = childs_path + "\\", child_path;
	delete child;
	char name[2000];
	child = NULL;
	UINT id;
	PRJ_Objects::iterator doomed;

	WIN32_FIND_DATA fd = {};
	HANDLE hFind = FindFirstFile(child_path_tmp + "*", &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..")))
				continue;
			child_path = child_path_tmp + fd.cFileName;

			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				continue;

			if (sscanf(fd.cFileName, "%d %[^\0]s", &id, name) != 2)
				continue;

			child = CreateChild();

			child->m_id = id;
			child->m_name = name;
			child->m_parent = this;

			if (deepness < 3)
			{
				if (!child->RestoreByDir(child_path, deepness + 1))
				{
					delete child;
					continue;
				}
			}

			doomed = m_childs.find(id);
			if (doomed != m_childs.end())
			{
				if (child->m_childs.size() < doomed->second->m_childs.size()) 
				{
					delete child;
					continue;
				}
				delete doomed->second;
			}
			m_childs[id] = child;

			if (deepness == 3 && childs_path + "\\" + fd.cFileName == g_path_of_work)
				child->SetSelected();
			if (m_max_id < id)
				m_max_id = id;
		} while(FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
	return true;
}

void PRJ_RestoreProjectByDirectories(CString str)
{
	KRS_app.SetRegistryMode(true);
	g_path_of_work = KRS_app.GetProfileString("info", "work_path");

	PRJ_Project* new_project = new PRJ_Project;
	new_project->m_path = str;
	int index = str.ReverseFind('\\');
	if (index == -1)
	{
		delete new_project;
		return;
	}
	new_project->m_name = str.Right(str.GetLength() - index - 1);
	if (!new_project->RestoreByDir(new_project->m_path, 0))
	{
		delete new_project;
		return;
	}
	CString new_name;
	for (int i = 1; i < 1000; i++)
	{
		new_name.Format("%s (%d).backup", new_project->m_name, i);
		FILE* f = fopen(new_project->m_path + "\\" + new_name, "rb");
		if (f == NULL)
			break;
		fclose(f);
	}
	MoveFile(new_project->GetProjectFilePath(), new_project->m_path + "\\" + new_name);
	new_project->SaveProject(PRJ_NowSelectedBranch, PRJ_SaveTreeOnly);
	delete new_project;
}

CString PRJ_CreateDBName(PRJ_Branch branch)
{
	char str[1024];
	KRS_config_file.ReadPar("DB_PREFIX", str, "db");

	CString str_d, str_t;
	SYSTEMTIME st;
	GetLocalTime(&st);
	str_d.Format("%d_%02d_%02d", st.wYear, st.wMonth, st.wDay);
	str_t.Format("%02d_%02d_%02d", st.wHour, st.wMinute, st.wSecond);
	return str + str_d + "_" + str_t;
}

bool PRJ_IsCreatedDB()
{
	return g_is_db_for_current_project_created;
}

void PRJ_SetCreatedDB(bool created)
{
	g_is_db_for_current_project_created = created;
	PRJ_WriteWorkFile(PRJ_NowSelectedBranch);
}

////////////////////////////////////////////////////////////////////////////////
// end