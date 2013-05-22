// uic 10.07.2008

#pragma once

#include <map>
using namespace std;

enum PRJ_LoadProjectResult
{
	PRJ_LoadProjectOK = 0,
	PRJ_LoadProjectFileError,
	PRJ_LoadWorkFileError,
	PRJ_LoadProjectFailed
};

enum PRJ_SaveProjectMode
{
	PRJ_SaveAllProject = 0,
	PRJ_SaveTreeOnly
};

enum PRJ_Branch
{
	PRJ_NowSelectedBranch = 0,
	PRJ_PrevSelectedBranch
};

extern bool PRJ_timer_saving_available;

PRJ_LoadProjectResult PRJ_LoadProject(CString path);
bool PRJ_SaveProject(PRJ_Branch branch, PRJ_SaveProjectMode mode, CProgressCtrl* progress = NULL);
void PRJ_CreateDefaultProject();
CString PRJ_GetProjectPath();
CString PRJ_GetWorkPath(PRJ_Branch branch = PRJ_NowSelectedBranch);
void PRJ_CleanUp();
void PRJ_SaveInfoIntoRegistry();

void PRJ_WritePanels(PRJ_Branch branch = PRJ_NowSelectedBranch, bool close_file = true);
void PRJ_WriteWorkFile(PRJ_Branch branch = PRJ_NowSelectedBranch);
bool PRJ_ReadWorkFile(bool from_registry);

CString PRJ_CreateDBName(PRJ_Branch branch);
bool PRJ_IsCreatedDB();
void PRJ_SetCreatedDB(bool created);

void PRJ_OnStagesReadyAfterLoad();

void PRJ_RestoreProjectByDirectories(CString str);

struct PRJ_Project;
PRJ_Project* PRJ_GetProject();
struct PRJ_Object;
typedef map<UINT, PRJ_Object*> PRJ_Objects;

struct PRJ_Object
{
	PRJ_Object();
	virtual ~PRJ_Object();

	void DeleteChilds();

	virtual bool IgnoreIDs() { return false; } 
	virtual PRJ_Object* CreateChild() = 0;

	UINT m_id, m_max_id, m_selected_child_id, m_prev_selected_child_id;
	CString m_name;
	PRJ_Objects m_childs;
	PRJ_Object* m_parent;

	void AddChild(PRJ_Object* child);
	bool RemoveChild(PRJ_Object* child, bool remove_and_delete);
	PRJ_Object* GetSelectedChild(PRJ_Branch branch = PRJ_NowSelectedBranch);

	void ResetPrevSelected();
	void SetSelected();

	virtual CString GetFullPath();
	virtual CString GetClassDir() = 0;
	CString GetOwnDir();
	virtual CString GetClassName() = 0;
	virtual void SaveInfo(PRJ_Branch branch);

	virtual bool Load(CArchive& archive);
	virtual bool Save(CArchive& archive);
	virtual bool OwnLoad(CArchive& archive) = 0;
	virtual bool OwnSave(CArchive& archive) = 0;

	virtual bool RestoreByDir(CString path, int deepness);
};
struct PRJ_Work: PRJ_Object
{
	virtual PRJ_Object* CreateChild() { return NULL; }
	virtual CString GetClassDir() { return "Работы"; }
	virtual CString GetClassName() { return "Работа"; }
	virtual bool OwnLoad(CArchive& archive) { return true; };
	virtual bool OwnSave(CArchive& archive) { return true; };
};
struct PRJ_BoreHole: PRJ_Object
{
	virtual PRJ_Object* CreateChild() { return new PRJ_Work; }
	virtual CString GetClassDir() { return "Скважины"; }
	virtual CString GetClassName() { return "Скважина"; }
	virtual bool OwnLoad(CArchive& archive) { return true; };
	virtual bool OwnSave(CArchive& archive) { return true; };
};
struct PRJ_Cluster: PRJ_Object
{
	virtual PRJ_Object* CreateChild() { return new PRJ_BoreHole; }
	virtual CString GetClassDir() { return "Кусты"; }
	virtual CString GetClassName() { return "Куст"; }
	virtual bool OwnLoad(CArchive& archive) { return true; };
	virtual bool OwnSave(CArchive& archive) { return true; };
};
struct PRJ_Deposit: PRJ_Object
{
	virtual PRJ_Object* CreateChild() { return new PRJ_Cluster; }
	virtual CString GetClassDir() { return "Месторождения"; }
	virtual CString GetClassName() { return "Месторождение"; }
	virtual bool OwnLoad(CArchive& archive) { return true; };
	virtual bool OwnSave(CArchive& archive) { return true; };
};
struct PRJ_Project: PRJ_Object
{
	PRJ_Project();

	CString m_path;
	bool m_ok, m_error_on_start, m_error_in_tree_reported, m_now_saving, m_stages_loaded;

	virtual bool IgnoreIDs() { return true; }
	virtual PRJ_Object* CreateChild() { return new PRJ_Deposit; }
	virtual CString GetClassDir() { return ""; }
	virtual CString GetClassName() { return "Проект"; }
	virtual bool OwnLoad(CArchive& archive) { return true; };
	virtual bool OwnSave(CArchive& archive) { return true; };

	PRJ_LoadProjectResult LoadProject(CString path);
	bool SaveProject(PRJ_Branch branch, PRJ_SaveProjectMode mode, CProgressCtrl* progress = NULL);
	bool CreateDefault();

	virtual CString GetFullPath();
	virtual CString GetProjectFilePath();

	void ReportError(CString err);
};

////////////////////////////////////////////////////////////////////////////////
// end