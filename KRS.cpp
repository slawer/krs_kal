// KRS.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#define _PARAM
#include "KRS.h"
#include "excel.h"

#include "MainFrm.h"
#include "KRSDoc.h"
#include "KRSView.h"
#include "ChildFrame.h"

#include "DBDoc.h"
#include "DBView.h"
#include "DBChildFrm.h"

#include "CementDoc.h"
#include "CementView.h"
#include "CementChildWnd.h"

#include "CementRT.h"
//#include "CementBD.h"

#include "DlgInterface.h"
#include "DlgModulesExternal.h"

#include "src\MutexWrap.h"
#include "src\DB.h"
#include "src\CONV.h"
#include "src\SERV.h"
#include "src\PRJ.h"
#include "src\LOG_V0.h"
#include "src\FORMULA.h"

#include "DlgWork.h"
#include "DlgColorInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double dv;
float fv;
int iv;
char bfv[1024];

bool KRS_shutdown = false;
bool g_second_instance = false;

COleDateTime KRS_syncro_ole_time;
DWORD KRS_syncro_tick;

CString KRS_root_path, KRS_program_name;
CCfgFiles KRS_config_file;
int KRS_programm_flag = 0;
int KRS_project_flag = 0;
int KRS_send_to_bo_interval = 1000;
bool KRS_running = false, KRS_need_send_to_db_additional_to = false;
double KRS_time_circ_lifetime = 0, KRS_time_bur_lifetime = 0;
CKRSApp KRS_app;

extern CString GetStrData(COleDateTime tm);
extern CString GetStrTime(COleDateTime tm);

void KRS_DeleteAllViews()
{
	CMainView *pViev;
	pViev = (CMainView*)KRS_app.GetView(t_real_time);
	if (pViev != NULL)
		pViev->DeleteView();
	pViev = (CMainView*)KRS_app.GetView(t_archive);
	if (pViev != NULL)
		pViev->DeleteView();
	pViev = (CMainView*)KRS_app.GetView(t_cement_rt);
	if (pViev != NULL)
		pViev->DeleteView();
}

void KRS_ClearMap(CParamMap* map, bool delete_params)
{
	if (delete_params)
	{
		int mKey;
		CParam *pPar;
		POSITION pos = map->GetStartPosition();
		while (pos != NULL)
		{
			map->GetNextAssoc(pos, mKey, pPar);
			if (pPar != NULL)
			{
				if (pPar->pBufParam != NULL) 
				{
					delete pPar->pBufParam; 
					pPar->pBufParam = NULL;
				}
				delete pPar;
				pPar = NULL;
			}
		}
	}
	map->RemoveAll();
}

void KRS_CheckParamForAlwaysActive(CParam* param)
{
	if ((KRS_project_flag & KRS_PRJ_FLAG_BURENIE) == 0)
		return;
	list<int>::iterator current = KRS_always_active_params.begin();
	while (current != KRS_always_active_params.end())
	{
		if (*current == param->m_num_par)
		{
			param->bParamActive = true;
			break;
		}
		current++;
	}
}

void KRS_MakeupFormulas()
{
	int key;
	CParam* param;
	POSITION pos = m_MapParamDefault.GetStartPosition();
	while (pos != NULL)
	{
		m_MapParamDefault.GetNextAssoc(pos, key, param);
		if (param->m_nTypePar == PARAM_TYPE_FORMULA)
			param->m_formula = FORMULA_Formula::CreateFromText(param->m_text_formula_for_registry);
	}
	pos = m_MapParamDefault.GetStartPosition();
	while (pos != NULL)
	{
		m_MapParamDefault.GetNextAssoc(pos, key, param);
		if (param->bParamActive || param->m_num_par <= MAX_PARAM)
		{
			param->Init(true);

			if (param->bParamActive)
				m_MapParam[key] = param;

			list<int>::iterator current = KRS_always_not_timeout_params.begin();
			while (current != KRS_always_not_timeout_params.end())
			{
				if (*current == param->m_num_par)
				{
					param->m_always_not_timed_out = true;
					if (!param->CanBeTimedOut())
						param->FinishTimeOut(0);
					break;
				}
				current++;
			}
		}
	}
}

bool LoadProgramConfig()
{
	char str[1024];

	KRS_config_file.ReadPar("PROJECT_FLAG", KRS_programm_flag, KRS_PRJ_FLAG_BURENIE | KRS_PRJ_FLAG_STAGES);
	if (KRS_programm_flag & KRS_PRJ_FLAG_BURENIE)
	{
	}

	KRS_config_file.ReadPar("LAST_PROJECT", str);
	if (PRJ_LoadProject(str) != PRJ_LoadProjectOK)
	{
		DlgWork dlg("Открыть работу / Создать новую работу", DlgWork_OPEN | DlgWork_NEW);
		if (dlg.DoModal() != IDOK)
			return false;
	}
	else
		KSR_StartAllConnections();

	return true;
}

void KRS_CheckFinishInitialization()
{
	if (KRS_start_counter == 100)
		return;
	KRS_start_counter++;
	if (KRS_start_counter == 4)
	{
		KRS_start_counter = 100;
		if (!LoadProgramConfig())
			pFrm->PostMessage(WM_CLOSE, 0, 0);//DestroyWindow();
	}
}

////////////////////////////////////////////////////////////////////////////////
// CKRSApp
////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CKRSApp, CWinApp)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
    ON_COMMAND(ID_VIEW_PASSWD, &CKRSApp::OnViewPasswd)
    ON_COMMAND(ID_DISABLE_PASSWD, &CKRSApp::OnDisablePasswd)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PASSWD, &CKRSApp::OnUpdateViewPasswd)
	ON_COMMAND(ID_MENU_INTERFACE, &CKRSApp::OnMenuInterface)
	ON_UPDATE_COMMAND_UI(ID_MENU_INTERFACE, &CKRSApp::OnUpdateMenuInterface)
	ON_COMMAND(ID_NASTROIKA_MODULES, &CKRSApp::OnNastroikaModules)
    ON_COMMAND(ID_MENU_MODULE_EXTERNAL + 0, &CKRSApp::OnModule0)
    ON_COMMAND(ID_MENU_MODULE_EXTERNAL + 1, &CKRSApp::OnModule1)
    ON_COMMAND(ID_MENU_MODULE_EXTERNAL + 2, &CKRSApp::OnModule2)
    ON_COMMAND(ID_MENU_MODULE_EXTERNAL + 3, &CKRSApp::OnModule3)
    ON_COMMAND(ID_MENU_MODULE_EXTERNAL + 4, &CKRSApp::OnModule4)
    ON_COMMAND(ID_MENU_MODULE_EXTERNAL + 5, &CKRSApp::OnModule5)
    ON_COMMAND(ID_MENU_MODULE_EXTERNAL + 6, &CKRSApp::OnModule6)
    ON_COMMAND(ID_MENU_MODULE_EXTERNAL + 7, &CKRSApp::OnModule7)
    ON_COMMAND(ID_MENU_MODULE_EXTERNAL + 8, &CKRSApp::OnModule8)
    ON_COMMAND(ID_MENU_MODULE_EXTERNAL + 9, &CKRSApp::OnModule9)
	ON_COMMAND(ID_ORIENT_VIEW, &CKRSApp::OnOrientView)
	ON_COMMAND(ID_ACCELERATOR_COLOR_INTERFACE, &CKRSApp::OnAcceleratorColorInterface)
END_MESSAGE_MAP()

void SaveProgramConfig()
{
	KRS_config_file.WritePar("PROJECT_FLAG", KRS_programm_flag);
	KRS_config_file.WritePar("LAST_PROJECT", PRJ_GetProjectPath());
}

CKRSApp::CKRSApp()
{
	m_params_file = NULL;

	char buff[1024];
	DWORD res = GetCurrentDirectory(1024, buff);
	KRS_root_path = buff;

    pApp = (CKRSApp*)AfxGetApp();

    pgtmData = new FRingBuf<double>(MAX_SIIZE_BUF, 0, 0, 0, NULL);
    pgtmData->AddFragment(0);
    pgcurGlub = new FRingBuf<double>(MAX_SIIZE_BUF, 0, 0, 0, NULL);
    pgcurGlub->AddFragment(0);
    pgBufX = (LPVOID)pgtmData; 

    pgtmDataDB = new FRingBuf<double>(MAX_SIIZE_BUF, 0, 0, 0, NULL);
    pgtmDataDB->AddFragment(0);
    pgcurGlubDB = new FRingBuf<double>(MAX_SIIZE_BUF, 0, 0, 0, NULL);
    pgcurGlubDB->AddFragment(0);
    pgBufXDB = (LPVOID)pgtmDataDB; 

	MutexWrap_Init();

	FORMULA_Init();

	KRS_config_file.AssignTo("KRS.cfg");
}

int CKRSApp::ExitInstance() 
{
	WSACleanup();

	SERV_ClearListBO(SERV_indication_blocks);
	SERV_ClearListBO(SERV_tmp_indication_blocks);
	
	return CWinApp::ExitInstance();
}

CKRSApp::~CKRSApp()
{
	if (!g_second_instance)
	{
		SaveProgramConfig();
		LOG_V0_AddMessage(LOG_V0_MESSAGE, "## Конец работы программы   ##");
		LOG_V0_AddMessage(LOG_V0_MESSAGE, "           ##############################", false, false);
		SERV_DeviceCfgShutDown();
		LOG_V0_ShutDown();
	}
	MutexWrap_ShutDown();
}

void CKRSApp::SetIniFileMode(CString file_path)
{
	if (m_pszRegistryKey)
	{
		free((void*)m_pszRegistryKey);
		m_pszRegistryKey = NULL;
	}
	if (m_pszProfileName)
		free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(LPTSTR(LPCTSTR(file_path)));
}

void CKRSApp::SetRegistryMode(bool only_registry)
{
	if (m_KRS_ini_exists && !only_registry)
	{
		SetIniFileMode(KRS_root_path + "\\KRS.ini");
		return;
	}	
	if (m_pszRegistryKey)
	{
		free((void*)m_pszRegistryKey);
		m_pszRegistryKey = NULL;
	}
	if (m_pszProfileName)
		free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(LPTSTR(LPCTSTR("KRS")));
	SetRegistryKey(_T("SKB OREOL"));
}

BOOL CKRSApp::InitInstance()
{
	STAGES_PreInit();

	KRS_always_active_params.push_back(3);
	KRS_always_active_params.push_back(21);
	KRS_always_not_timeout_params.push_back(3);
	KRS_always_not_timeout_params.push_back(4);
	KRS_always_not_timeout_params.push_back(5);
	KRS_always_not_timeout_params.push_back(21);

	HANDLE Test_Present = CreateMutex(NULL,false,"KRS_LoiS_alreaDy_StarTed");
	DWORD ret = GetLastError();
	if (ret == ERROR_ALREADY_EXISTS || ret == ERROR_ACCESS_DENIED)
	{ 
		g_second_instance = true;
		MessageBox(0, "Программа уже запущена", "KRS", MB_OK);
		CloseHandle(Test_Present); 
		return FALSE;
	}
	
	LOG_V0_Init();
	bool log_debug;
	KRS_config_file.ReadPar("LOG_Debug", log_debug, false);
	if (!log_debug)
		LOG_V0_SetMask(LOG_V0_MESSAGE);
	LOG_V0_AddMessage(LOG_V0_MESSAGE, "           ##############################", false, false);
	LOG_V0_AddMessage(LOG_V0_MESSAGE, "## Начало работы программы  ##");
	
	{
		#define MAX_STR 1024
		char str[MAX_STR];
		LoadString(AfxGetResourceHandle(), IDR_MAINFRAME, str, MAX_STR);
		KRS_program_name = str;
		LOG_V0_AddMessage(LOG_V0_MESSAGE, KRS_program_name);
		#undef MAX_STR
	}

	WSADATA ws;
	if (WSAStartup(0x0202, &ws) != 0)
	{
		AfxMessageBox("WinSock2 initialization failed");
		return FALSE;
	}

    if(!AfxOleInit())
     {
        AfxMessageBox("Could not initialize COM dll");
        return FALSE;
     }

 	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	SetRegistryMode(true);
	WriteProfileString("info", "root", KRS_root_path); // ЭТО ДОЛЖНО БЫТЬ В РЕЕСТРЕ
	gfDLi = ReadFloatValueFromRegistry("InitData","DLi");

	SERV_DeviceCfgInit(GetProfileString("DEVICES", "Config file name", ""));

	FILE* f = fopen(KRS_root_path + "\\KRS.ini", "rb");
	m_KRS_ini_exists = (f != NULL);
	if (m_KRS_ini_exists)
		fclose(f);
	SetRegistryMode();

	LoadStdProfileSettings();  

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_KRSREALTIMETYPE,
		RUNTIME_CLASS(CKRSDoc),
		RUNTIME_CLASS(CChildFrame),       
		RUNTIME_CLASS(CKRSView));
    if (!pDocTemplate)
      return FALSE;
	AddDocTemplate(pDocTemplate);

    pDocTemplate = new CMultiDocTemplate(
        IDR_KRSRARCHIVETYPE,
        RUNTIME_CLASS(CDBDoc),
        RUNTIME_CLASS(CDBChildFrm),       
        RUNTIME_CLASS(CDBView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_KRSCEMENTTYPE,
		RUNTIME_CLASS(CCementDoc),
		RUNTIME_CLASS(CCementChildWnd),
		RUNTIME_CLASS(CCementRT));
    if (!pDocTemplate)
        return FALSE;
	AddDocTemplate(pDocTemplate);
   	CMainFrame* pMainFrame = new CMainFrame;
	m_pMainWnd = pMainFrame;
    if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
    {
        delete pMainFrame;
        return FALSE;
    }
    m_pMainWnd = pMainFrame;

	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

    m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}



class CAboutDlg : public CDialog
{
public:
	CAboutDlg():CDialog(CAboutDlg::IDD)
	{}

	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual BOOL OnInitDialog()
	{
		SetDlgItemText(IDC_STATIC_VER, KRS_program_name);
		return TRUE;
	}
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
	}
protected:
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CKRSApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


void CKRSApp::ReadAllRegistriData()
{
	CParam *pPrm, *pPrmTmp;
	int mKey;

	pPrmTmp = new CParam;
	POSITION posPrm = m_MapParamDefault.GetStartPosition();
	while (posPrm != NULL)
	{
		m_MapParamDefault.GetNextAssoc(posPrm, mKey, pPrm);
		pPrmTmp->m_num_par = pPrm->m_num_par;
		pPrmTmp->m_shown_digits = pPrm->m_shown_digits;
		ReadParamFromRegistry(pPrmTmp);
		if (pPrm->m_nTypePar == PARAM_TYPE_CALCULATED)
			pPrmTmp->m_nTypePar = PARAM_TYPE_CALCULATED;
		if (pPrmTmp->m_nTypePar < PARAM_TYPE_CALCULATED || pPrmTmp->m_nTypePar > PARAM_TYPE_FORMULA) // ошибка какая-то
			pPrmTmp->m_nTypePar = PARAM_TYPE_CHANNEL;
		if (pPrmTmp->sName.GetLength() != 0)
			*pPrm = *pPrmTmp;
	}
	delete pPrmTmp;
	CString additional_param_nums = GetProfileString("Settings", "дополнительные параметры", "");
	if (additional_param_nums.GetLength() > 0)
	{
		char *str = additional_param_nums.GetBuffer(1024);
		char *str_ptr = str;
		int num, index = 0;
		while (true)
		{
			if (sscanf(str_ptr, "%d", &num) != 1)
				break;
			pPrmTmp = new CParam;
			pPrmTmp->m_num_par = num;
			ReadParamFromRegistry(pPrmTmp);
			if (pPrmTmp->m_nTypePar <= PARAM_TYPE_CALCULATED || pPrmTmp->m_nTypePar > PARAM_TYPE_FORMULA) // ошибка какая-то
				pPrmTmp->m_nTypePar = PARAM_TYPE_CHANNEL;
			m_MapParamDefault[pPrmTmp->m_num_par] = pPrmTmp;
			index = additional_param_nums.Find(",", index);
			if (index == -1)
				break;
			index++;
			str_ptr = str + index;
		}
	}

	GetSection_NET();
	GetSection_DB();

	STAGES_last_file_path = GetProfileString("STAGES", "STAGES_last_file_path", "");
	if (STAGES_last_file_path.GetLength() == 0)
		STAGES_last_file_path = KRS_root_path + "\\stages.cfg";

	gPorogRotorBur = ReadFloatValueFromRegistry("InitData","PorogRotorBur", 50);
	gVesRotorBur = ReadFloatValueFromRegistry("InitData","VesRotorBur", 50);

    ReadIntegerValueFromRegistry("InitData", "KNBKList");
    gflgRabotaBezShurfa = ReadIntegerValueFromRegistry("InitData","RabotaBezShurfa");

    gflDisablePassWd = ReadIntegerValueFromRegistry("InitData","12345");
//    gflPassWd = false;//gflDisablePassWd;
	gfDLi = ReadFloatValueFromRegistry("InitData","DLi");

	KRS_MakeupFormulas();
}

void CKRSApp::WriteSection_DB()
{
	WriteProfileString("DB","DB_connection_info.m_db_server", DB_connection_info.m_db_server);
	WriteProfileString("DB","DB_connection_info.m_db_name",	DB_connection_info.m_db_name);
	WriteProfileString("DB","DB_connection_info.m_login", DB_connection_info.m_login);
	WriteProfileString("DB","DB_connection_info.m_password", DB_connection_info.m_password);
}

void CKRSApp::GetSection_DB()
{
	DB_connection_info.m_db_server = GetProfileString("DB","DB_connection_info.m_db_server", "localhost");
	DB_connection_info.m_db_name = GetProfileString("DB","DB_connection_info.m_db_name", "oreol");
	DB_connection_info.m_login = GetProfileString("DB","DB_connection_info.m_login", "login");
	DB_connection_info.m_password = GetProfileString("DB","DB_connection_info.m_password", "password");
}

void CKRSApp::WriteSection_NET()
{
	WriteProfileString("NET","DB_connection_info.m_server_ip", DB_connection_info.m_server_ip);
	WriteProfileString("NET","CONV_connection_info.m_server_ip", CONV_connection_info.m_server_ip);
	WriteProfileString("NET","SERV_connection_info.m_server_ip", SERV_connection_info.m_server_ip);

	WriteIntegerValueToRegistry("NET","CONV_connection_info.m_server_port", CONV_connection_info.m_server_port);
	WriteIntegerValueToRegistry("NET","DB_connection_info.m_server_port", DB_connection_info.m_server_port);
	WriteIntegerValueToRegistry("NET","SERV_connection_info.m_server_port", SERV_connection_info.m_server_port);
}

void CKRSApp::GetSection_NET()
{
	DB_connection_info.m_server_ip = GetProfileString("NET","DB_connection_info.m_server_ip", "localhost");
	CONV_connection_info.m_server_ip = GetProfileString("NET","CONV_connection_info.m_server_ip", "localhost");
	SERV_connection_info.m_server_ip = GetProfileString("NET","SERV_connection_info.m_server_ip", "localhost");

	CONV_connection_info.m_server_port = ReadIntegerValueFromRegistry("NET","CONV_connection_info.m_server_port", 58000);
	DB_connection_info.m_server_port = ReadIntegerValueFromRegistry("NET","DB_connection_info.m_server_port", 50);
	SERV_connection_info.m_server_port = ReadIntegerValueFromRegistry("NET","SERV_connection_info.m_server_port", 56000);
}

void CKRSApp::WriteAllRegistryData()
{
	CParam* pPrm;
	int mKey;

	CString additional_param_nums = "", one_num;

	POSITION posPrm = m_MapParamDefault.GetStartPosition();
	while (posPrm != NULL)
	{
		m_MapParamDefault.GetNextAssoc(posPrm, mKey, pPrm);
		if (mKey > MAX_PARAM)
		{
			if (additional_param_nums.GetLength() > 0)	
				additional_param_nums += ",";
			one_num.Format("%d", mKey);
			additional_param_nums += one_num;
		}
		WriteParamToRegistry(pPrm);
	}
	WriteProfileString("Settings", "дополнительные параметры", additional_param_nums);
    WriteIntegerValueToRegistry("InitData","12345", gflDisablePassWd);
	WriteFloatValueToRegistry("InitData","DLi", gfDLi);
}

void CKRSApp::WriteFloatValueToRegistry(CString strSection, CString strEntry, double value)
{
	WriteProfileString(strSection, strEntry, BS_FloatWOZeros(value, 30));
}

double CKRSApp::ReadFloatValueFromRegistry(CString strSection, CString strEntry, double default_value)
{
	sprintf(bfv, "%f", default_value);
	double val;
	if (sscanf(GetProfileString(strSection, strEntry, bfv), "%lf", &val) != 1)
		val = default_value;
	return val;
}

void CKRSApp::WriteIntegerValueToRegistry(CString strSection, CString strEntry, int iValue)
{
	sprintf(bfv, "%d", iValue);
	WriteProfileString(strSection, strEntry, bfv);
}

int CKRSApp::ReadIntegerValueFromRegistry(CString strSection, CString strEntry, int default_value)
{
	sprintf(bfv, "%d", default_value);
	if (sscanf(GetProfileString(strSection, strEntry, bfv), "%d", &iv) != 1)
		iv = default_value;
	return iv;
}

void CKRSApp::WriteRegistriCheckParam(LPARAM lParam)
{
	CParam *pParam = (CParam*)lParam;
	CString sn = pParam->sName;

	int idv;
	char bf[1024];

	idv = (int)pParam->flControl1;
	sprintf(bf,"%d", idv);
	WriteProfileString(sn,"Control1", bf);
}

void CKRSApp::WriteRegistriSelectParam(LPARAM lParam)
{
	CParam *pParam = (CParam*)lParam;
	CString sn = pParam->sName;

	int idv;
	char bf[1024];

	idv = (int)pParam->bMain;
	sprintf(bf,"%d", idv);
	WriteProfileString(sn,"Main", bf);
}

void CKRSApp::ReadParamFromRegistry(CParam *pParam)
{
	CString sn;
	sn.Format("параметр %03d", pParam->m_num_par);

	pParam->sName = GetProfileString(sn, "!имя", "");
	if (pParam->sName == "" && pParam->m_num_par <= MAX_PARAM)
		return;
	pParam->sRazmernPar = GetProfileString(sn, "размерность", "?");
	pParam->sGraphName = GetProfileString(sn, "короткое название", pParam->sName.Left(4));
	pParam->sDiscript = GetProfileString(sn, "описание", "");

	pParam->fCur = ReadFloatValueFromRegistry(sn, "fCur");
	pParam->fMin = ReadFloatValueFromRegistry(sn, "Min");
	pParam->fMax = ReadFloatValueFromRegistry(sn, "Max");
	pParam->fDiapazon = ReadFloatValueFromRegistry(sn, "Diapazon");
	pParam->fDiapazon1 = ReadFloatValueFromRegistry(sn, "Diapazon1");
	pParam->fBlock = ReadFloatValueFromRegistry(sn, "Block");
	pParam->fAvaria = ReadFloatValueFromRegistry(sn, "Avaria");
	pParam->nKodControl = ReadIntegerValueFromRegistry(sn, "KodControl");
	pParam->fPodklinkaKof = ReadFloatValueFromRegistry(sn, "Podklinka");
	pParam->fObiom = ReadFloatValueFromRegistry(sn, "Obiom");
	pParam->fNapolnenie = ReadFloatValueFromRegistry(sn, "Napolnenie");
	pParam->fRsahodIzm = ReadFloatValueFromRegistry(sn, "RsahodIzm");
	pParam->fRsahodRasch = ReadFloatValueFromRegistry(sn, "RsahodRasch");
	pParam->fPodveskaDatchika = ReadFloatValueFromRegistry(sn, "PodveskaDatchika");
	pParam->fPloschad = ReadFloatValueFromRegistry(sn, "Ploschad");
	pParam->fIzmObioma = ReadFloatValueFromRegistry(sn, "IzmObioma");
	pParam->fVObioma = ReadFloatValueFromRegistry(sn, "VObioma");
	pParam->fNizIzmer = ReadFloatValueFromRegistry(sn, "NizIzmer");
	pParam->fNizIstina = ReadFloatValueFromRegistry(sn, "NizIstina");
	pParam->fVerhIzmer = ReadFloatValueFromRegistry(sn, "VerhIzmer");
	pParam->fVerhIstina = ReadFloatValueFromRegistry(sn, "VerhIstina");
	pParam->flGraph = (BOOL)ReadIntegerValueFromRegistry(sn, "flGraph", FALSE);
	pParam->flControl1 = (BOOL)ReadIntegerValueFromRegistry(sn, "Control1", FALSE);
	pParam->flControl2 = (BOOL)ReadIntegerValueFromRegistry(sn, "Control2", FALSE);
	pParam->nScale = ReadIntegerValueFromRegistry(sn, "Scale");
	pParam->flagDiapazon = (BOOL)ReadIntegerValueFromRegistry(sn, "flDiapazon", FALSE);
	pParam->nFlag1 = ReadIntegerValueFromRegistry(sn, "nFlag1");
	pParam->nFlag2 = ReadIntegerValueFromRegistry(sn, "nFlag2");
	pParam->fKalibr = ReadFloatValueFromRegistry(sn, "Kalibr", 1);
	pParam->fKalibrA = ReadFloatValueFromRegistry(sn, "KalibrA");
	pParam->fKalibrK = ReadFloatValueFromRegistry(sn, "KalibrK", 1);
	pParam->nMainList = ReadIntegerValueFromRegistry(sn, "MainList");
	pParam->nViewList = ReadIntegerValueFromRegistry(sn, "ViewList");
	pParam->bMain = (BOOL)ReadIntegerValueFromRegistry(sn, "Main", FALSE);
	pParam->bView = (BOOL)ReadIntegerValueFromRegistry(sn, "View", FALSE);
	pParam->fGraphMIN = ReadFloatValueFromRegistry(sn, "fGraphMin");
	pParam->fGraphMAX = ReadFloatValueFromRegistry(sn, "fGraphMax", 1);
	pParam->nViewList = ReadIntegerValueFromRegistry(sn, "nViewList", 255);
///	pParam->m_num_par = ReadIntegerValueFromRegistry(sn, "num_par");
	pParam->m_nTypePar = ReadIntegerValueFromRegistry(sn, "m_nTypePar");
	pParam->m_text_formula_for_registry = GetProfileString(sn, "m_formula", "");
	pParam->bParamActive = (BOOL)ReadIntegerValueFromRegistry(sn, "bParamActive", FALSE);
	pParam->m_channel_num = ReadIntegerValueFromRegistry(sn, "m_channel_num", -1);
	if (pParam->m_channel_num < 1)
		pParam->m_channel_num = -1;
	pParam->m_channel_conv_num = ReadIntegerValueFromRegistry(sn, "m_channel_conv_num", 0);
	if (pParam->m_channel_conv_num != 1)
		pParam->m_channel_conv_num = 0;
	pParam->m_attr_channel_num = ReadIntegerValueFromRegistry(sn, "m_attr_channel_num", -1);
	if (pParam->m_attr_channel_num < 1)
		pParam->m_attr_channel_num = -1;
	pParam->m_attr_channel_conv_num = ReadIntegerValueFromRegistry(sn, "m_attr_channel_conv_num", 0);
	if (pParam->m_attr_channel_conv_num != 1)
		pParam->m_attr_channel_conv_num = 0;
	pParam->m_shown_digits = ReadIntegerValueFromRegistry(sn, "m_shown_digits", pParam->m_shown_digits);
	pParam->m_shown_digits = max(0, min(6, pParam->m_shown_digits));
	pParam->sFile1 = GetProfileString(sn, "File1", "");
	pParam->sFile2 = GetProfileString(sn, "File2", "");
	pParam->sFile3 = GetProfileString(sn, "File3", "");
	pParam->sFile4 = GetProfileString(sn, "File4", "");
	pParam->flPodacha = (BOOL)ReadIntegerValueFromRegistry(sn, "flPodacha", FALSE);
	pParam->flDB = (BOOL)ReadIntegerValueFromRegistry(sn, "flDB", FALSE);
	pParam->m_color = ReadIntegerValueFromRegistry(sn, "m_color", 0);
	pParam->m_line_width = ReadIntegerValueFromRegistry(sn, "m_line_width", 2);
	pParam->m_db_data_interval = ReadFloatValueFromRegistry(sn, "m_db_data_interval", 1.0);
	int ver = GetProfileInt(sn, "VER", 0);
	double time_k = 1.0 / (24*60*60);
	pParam->m_db_time_interval = ReadFloatValueFromRegistry(sn, "m_db_time_interval", (ver == 1) ? 1 : time_k);
	if (ver >= 1)
		pParam->m_db_time_interval *= time_k;
	if (pParam->m_db_time_interval < 0.1 * time_k || pParam->m_db_time_interval > 1 * 60 * 60 * time_k)
		pParam->m_db_time_interval = 1.0 * time_k;
	pParam->timeAvrg = ReadFloatValueFromRegistry(sn, "timeAvrg", 0);
	KRS_CheckParamForAlwaysActive(pParam);
}

BOOL CKRSApp::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
	if (m_params_file)
		return fputs(lpszEntry + CString("=") + lpszValue + "\n", m_params_file);
	else
		return CWinApp::WriteProfileString(lpszSection, lpszEntry, lpszValue);
}

void CKRSApp::WriteParamToRegistry(CParam *pParam, bool write_formula_text)
{
	CString sn;
	sn.Format("параметр %03d", pParam->m_num_par);

	if (m_params_file)
		fputs("[" + sn + "]\n", m_params_file);

	WriteProfileString(sn, "!имя", pParam->sName);
	WriteProfileString(sn, "размерность", pParam->sRazmernPar);
	WriteProfileString(sn, "короткое название", pParam->sGraphName);
	WriteProfileString(sn, "описание", pParam->sDiscript);

	WriteFloatValueToRegistry(sn, "fCur", pParam->fCur);
	WriteFloatValueToRegistry(sn, "Min", pParam->fMin);
	WriteFloatValueToRegistry(sn, "Max", pParam->fMax);
	WriteFloatValueToRegistry(sn, "Diapazon", pParam->fDiapazon);
	WriteFloatValueToRegistry(sn, "Diapazon1", pParam->fDiapazon1);
	WriteFloatValueToRegistry(sn, "Block", pParam->fBlock);
	WriteFloatValueToRegistry(sn, "Avaria", pParam->fAvaria);
	WriteIntegerValueToRegistry(sn, "KodControl", pParam->nKodControl);
	WriteFloatValueToRegistry(sn, "Podklinka", pParam->fPodklinkaKof);
	WriteFloatValueToRegistry(sn, "Obiom", pParam->fObiom);
	WriteFloatValueToRegistry(sn, "Napolnenie", pParam->fNapolnenie);
	WriteFloatValueToRegistry(sn, "RsahodIzm", pParam->fRsahodIzm);
	WriteFloatValueToRegistry(sn, "RsahodRasch", pParam->fRsahodRasch);
	WriteFloatValueToRegistry(sn, "PodveskaDatchika", pParam->fPodveskaDatchika);
	WriteFloatValueToRegistry(sn, "Ploschad", pParam->fPloschad);
	WriteFloatValueToRegistry(sn, "IzmObioma", pParam->fIzmObioma);
	WriteFloatValueToRegistry(sn, "VObioma", pParam->fVObioma);
	WriteFloatValueToRegistry(sn, "NizIzmer", pParam->fNizIzmer);
	WriteFloatValueToRegistry(sn, "NizIstina", pParam->fNizIstina);
	WriteFloatValueToRegistry(sn, "VerhIzmer", pParam->fVerhIzmer);
	WriteFloatValueToRegistry(sn, "VerhIstina", pParam->fVerhIstina);
	WriteIntegerValueToRegistry(sn, "flGraph", pParam->flGraph);
	WriteIntegerValueToRegistry(sn, "Control1", pParam->flControl1);
	WriteIntegerValueToRegistry(sn, "Control2", pParam->flControl2);
	WriteIntegerValueToRegistry(sn, "Scale", pParam->nScale);
	WriteIntegerValueToRegistry(sn, "flDiapazon", pParam->flagDiapazon);
	WriteIntegerValueToRegistry(sn, "nFlag1", pParam->nFlag1);
	WriteIntegerValueToRegistry(sn, "nFlag2", pParam->nFlag2);
	WriteFloatValueToRegistry(sn, "Kalibr", pParam->fKalibr);
	WriteFloatValueToRegistry(sn, "KalibrA", pParam->fKalibrA);
	WriteFloatValueToRegistry(sn, "KalibrK", pParam->fKalibrK);
	WriteIntegerValueToRegistry(sn, "MainList", pParam->nMainList);
	WriteIntegerValueToRegistry(sn, "ViewList", pParam->nViewList);
	WriteIntegerValueToRegistry(sn, "Main", pParam->bMain);
	WriteIntegerValueToRegistry(sn, "View", pParam->bView);
	WriteFloatValueToRegistry(sn, "fGraphMin", pParam->fGraphMIN);
	WriteFloatValueToRegistry(sn, "fGraphMax", pParam->fGraphMAX);
	WriteIntegerValueToRegistry(sn, "nViewList", pParam->nViewList);
//	WriteIntegerValueToRegistry(sn, "num_par", pParam->m_num_par);
	WriteIntegerValueToRegistry(sn, "m_nTypePar", pParam->m_nTypePar);
	if (!write_formula_text)
		pParam->m_text_formula_for_registry = (pParam->m_formula == NULL)?"":pParam->m_formula->GetTextForSave();
	WriteProfileString(sn, "m_formula", pParam->m_text_formula_for_registry);
	WriteIntegerValueToRegistry(sn, "bParamActive", pParam->bParamActive);
	WriteIntegerValueToRegistry(sn, "m_channel_num", pParam->m_channel_num);
	WriteIntegerValueToRegistry(sn, "m_channel_conv_num", pParam->m_channel_conv_num);
	WriteIntegerValueToRegistry(sn, "m_attr_channel_num", pParam->m_attr_channel_num);
	WriteIntegerValueToRegistry(sn, "m_attr_channel_conv_num", pParam->m_attr_channel_conv_num);
	WriteIntegerValueToRegistry(sn, "m_shown_digits", pParam->m_shown_digits);

	WriteProfileString(sn, "File1", pParam->sFile1);
	WriteProfileString(sn, "File2", pParam->sFile2);
	WriteProfileString(sn, "File3", pParam->sFile3);
	WriteProfileString(sn, "File4", pParam->sFile4);
	WriteIntegerValueToRegistry(sn, "flPodacha", pParam->flPodacha);
	WriteIntegerValueToRegistry(sn, "flDB", pParam->flDB);
	WriteIntegerValueToRegistry(sn, "m_color", pParam->m_color);
	WriteIntegerValueToRegistry(sn, "m_line_width", pParam->m_line_width);
	
	WriteFloatValueToRegistry(sn, "m_db_data_interval", pParam->m_db_data_interval);
	WriteFloatValueToRegistry(sn, "m_db_time_interval", pParam->m_db_time_interval * (24*60*60));
	WriteIntegerValueToRegistry(sn, "VER", 1);
	WriteFloatValueToRegistry(sn, "timeAvrg", pParam->timeAvrg);
}

void CKRSApp::DeleteParamFromList(int numList, int numPanel, int pos, typeView t_v)
{
	PanelDescription panel_desrc;
	ReadPanelList(numList, numPanel, &panel_desrc, t_v);
	if (pos >= (int)panel_desrc.m_params.size())
		return;
	list<ParamInPanel>::iterator doomed = panel_desrc.m_params.begin();
	for (int i = 0; i < pos; i++)
		doomed++;
	panel_desrc.m_params.erase(doomed);
	WritePanelList(numList, numPanel, &panel_desrc, t_v);
}

BOOL CKRSApp::SaveParamFromList(int numList, int numPanel, ParamInPanel param_descr, int pos_in_panel, typeView t_v)
{
	PanelDescription panel_desrc;
	ReadPanelList(numList, numPanel,  &panel_desrc, t_v);

	if (pos_in_panel < (int)panel_desrc.m_params.size())
	{
		list<ParamInPanel>::iterator current = panel_desrc.m_params.begin();
		for (int i = 0; i < pos_in_panel; i++)
			current++;
		panel_desrc.m_params.insert(current, param_descr);
		panel_desrc.m_params.erase(current);
	}
	else
		panel_desrc.m_params.push_back(param_descr);

	WritePanelList(numList, numPanel,  &panel_desrc, t_v);
	return true;
}

BOOL CKRSApp::RestoreDataList(int numList, int numPanel,  LPVOID plstParam, typeView t_v)
{
	CString snumList, snumPanel, cs;
	PanelDescription *plParam = (PanelDescription*)plstParam;

	if(t_v == t_archive) snumList = "ListDB";
    else if(t_v == gt_cement) snumList = "ListCM";
	else snumList = "List";

	cs.Format("%d", numList); 
	snumList += cs;

	snumPanel = "Panel";
	cs.Format("%d", numPanel); 
	snumPanel += cs;
	
	return true;
}

BOOL CKRSApp::ReadPanelList(int numList, int numPanel,  LPVOID plstParam, typeView t_v)
{
	if(t_v == t_cement_rt || t_v == t_cement_db)
		return TRUE;

	CString num_list_str, num_panel_str, format_str;
	format_str = "List";
    if (t_v == t_archive) format_str += "DB";
    else if (t_v == gt_cement) format_str += "CM";
	format_str += "%d";
	num_list_str.Format(format_str, numList);
	format_str = "Panel%d";
	num_panel_str.Format(format_str, numPanel);
	CString text = GetProfileString(num_list_str, num_panel_str, "");

	PanelDescription *panel_descr = (PanelDescription*)plstParam;
	return panel_descr->FillFromText(text)?TRUE:FALSE;
}

BOOL CKRSApp::WritePanelList(int numList, int numPanel,  LPVOID plstParam, typeView t_v)
{
	if(t_v == t_cement_rt || t_v == t_cement_db)
		return TRUE;
		
	PanelDescription *panel_descr = (PanelDescription*)plstParam;
	CString num_list_str, num_panel_str, format_str;
	format_str = "List";
    if(t_v == t_archive) format_str += "DB";
    else if(t_v == gt_cement) format_str += "CM";
	format_str += "%d";
	num_list_str.Format(format_str, numList);
	format_str = "Panel%d";
	num_panel_str.Format(format_str, numPanel);
	CString text = panel_descr->GetTextForSave();
	WriteProfileString(num_list_str, num_panel_str, text);
	return TRUE;	
}

BOOL bbb = true;
void CKRSApp::OnButton2() 
{
	m_wndDialogBar.m_Regim.SetBlink(bbb);
	bbb = !bbb;
}
void CKRSApp::GetDataFromXLS()
{
	COleVariant VOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	CFileFind fndFile;
	if(!fndFile.FindFile(gKNBKPath)) return;

	_Application objApp;
	_Workbook objBook;
	Workbooks objBooks;
	Worksheets objSheets;
	_Worksheet objSheet;
	Range objRange;
	VARIANT ret;

	if(!objApp.CreateDispatch("Excel.Application"))
	{
		AfxMessageBox("Couldn't CreateDispatch on Excel");
		return;
	}
	objBooks = objApp.GetWorkbooks();
	objBook = objBooks.Open(gKNBKPath,
		VOptional, VOptional, VOptional, VOptional,
		VOptional, VOptional, VOptional, VOptional,
		VOptional, VOptional, VOptional, VOptional, VOptional, VOptional);
	objSheets = objBook.GetWorksheets();
	objSheet = objSheets.GetItem(COleVariant((short)1));

	objRange = objSheet.GetRange(COleVariant("C3"), COleVariant("F1000"));
	ret = objRange.GetValue2();

	COleSafeArray sa(ret);

	gtblElement.Init();

	long lNumRows;
	long lNumCols;
	sa.GetUBound(1, &lNumRows);
	sa.GetUBound(2, &lNumCols);

	long index[2];
	VARIANT val;
	int r;

	char buf[1024];
	for(r = 1; r <= lNumRows; r++)
	{
		index[0] = r;
		index[1] = 1;
		sa.GetElement(index, &val);
		if(val.vt == VT_BSTR) 
			gtblElement.Add((CString)val.bstrVal, 0);
		else if(val.vt == VT_EMPTY) break;
		else
		{
			sprintf(buf, "%ld Неправильный формат ячейки(текстовый)", r);
			::MessageBox(NULL, buf, "Sheet Count", MB_OK | MB_SETFOREGROUND);

			break;
		}

		index[0] = r;
		index[1] = 4;
		sa.GetElement(index, &val);
		if(val.vt == VT_BSTR) 
		{
			float ff;
			CString cs = (CString)val.bstrVal;
			cs.Replace(",", ".");
			ff = (float)atof(cs.GetBuffer(cs.GetLength()));
			gtblElement.SetLenToLastElm(ff);
		}
		else if(val.vt == VT_R8)
		{
			gtblElement.SetLenToLastElm((float)val.dblVal);
		}
		else if(val.vt == VT_EMPTY) break;
		else
		{
			sprintf(buf, "%ld Неправильный формат ячейки(числовой)", r);
			::MessageBox(NULL, buf, "Sheet Count", MB_OK | MB_SETFOREGROUND);

			break;
		}
	}
	objBook.Close(COleVariant((short)FALSE), VOptional, VOptional);
	objApp.Quit();
}

CDocument* CKRSApp::NewDoc(typeView t_v)
{
    if(m_pDocManager != NULL)
    {
        POSITION pos = GetFirstDocTemplatePosition();
        CDocTemplate* pTtemplate;

        switch(t_v)
        {
        case t_real_time:
            pTtemplate = GetNextDocTemplate(pos);
        	break;
        case t_archive:
            GetNextDocTemplate(pos);
            pTtemplate = GetNextDocTemplate(pos);
        	break;
        case t_cement_rt:
        case t_cement_db:
            GetNextDocTemplate(pos);
            GetNextDocTemplate(pos);
            pTtemplate = GetNextDocTemplate(pos);
            break;
        }

        pos = pTtemplate->GetFirstDocPosition();
        if(pos == NULL) 
			return pTtemplate->OpenDocumentFile(NULL);
        else 
			return pTtemplate->GetNextDoc(pos);
    }
    return NULL;
}

CDocument* CKRSApp::GetDoc(typeView t_v)
{
    if(m_pDocManager != NULL)
    {
        POSITION pos = GetFirstDocTemplatePosition();
        CDocTemplate* pTtemplate;

        switch(t_v)
        {
        case t_real_time:
            pTtemplate = GetNextDocTemplate(pos);
            break;
        case t_archive:
            GetNextDocTemplate(pos);
            pTtemplate = GetNextDocTemplate(pos);
            break;
        case t_cement_rt:
        case t_cement_db:
            GetNextDocTemplate(pos);
            GetNextDocTemplate(pos);
            pTtemplate = GetNextDocTemplate(pos);
            break;
        }
        pos = pTtemplate->GetFirstDocPosition();
        if(pos != NULL) return pTtemplate->GetNextDoc(pos);
    }

    return NULL;
}

CView* CKRSApp::SetActiveView(typeView t_v)
{
    CView* pView = NULL;
    CDocument* pDoc = NewDoc(t_v);
    if(pDoc != NULL)
    {
        POSITION pos = pDoc->GetFirstViewPosition();
        if(pos != NULL)	
        {
            pView = pDoc->GetNextView(pos);
            CFrameWnd* pFrame = pView->GetParentFrame();
            ASSERT(pFrame != NULL);
//            pFrame->ActivateFrame(SW_MAXIMIZE);

            pFrame->SetActiveView(pView, true);
            pFrame->BringWindowToTop();
//            pFrame->ShowWindow(SW_SHOWMAXIMIZED);

        }
    }
    return pView;
}

CView* CKRSApp::GetActiveView()
{
    CMDIFrameWnd *pFrame = (CMDIFrameWnd*)m_pMainWnd;
    if(pFrame != NULL)
    {
        CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
        return (CView*) pChild->GetActiveView();
    }
    else
    {
        CView* pView = NULL;
        if(m_pDocManager != NULL)
        {
            POSITION posDocTampl = GetFirstDocTemplatePosition();
            CDocTemplate* pTtemplate;

            while(posDocTampl != NULL)
            {
                pTtemplate = GetNextDocTemplate(posDocTampl);
                POSITION posDoc = pTtemplate->GetFirstDocPosition();
                if(posDoc == NULL) continue;

                CDocument* pDoc = pTtemplate->GetNextDoc(posDoc);
                if(pDoc == NULL) continue;

                POSITION posView = pDoc->GetFirstViewPosition();
                if(posView == NULL)	continue;

                pView = pDoc->GetNextView(posView);
                if(pView == NULL)	continue;

                CFrameWnd* pFrame = pView->GetParentFrame();
                if(pFrame == NULL)	continue;
                return pFrame->GetActiveView();
            }
        }
    }
    return NULL;
}
BOOL CKRSApp::IsActiveView(typeView t_v)
{
    CView* pView = GetActiveView();
    if(pView == NULL) return false;
    switch(t_v)
    {
    case t_real_time: 
        if(pView->IsKindOf(RUNTIME_CLASS(CKRSView))) return true;
    	break;
    case t_archive:
        if(pView->IsKindOf(RUNTIME_CLASS(CDBView))) return true;
        break;
    case t_cement_rt:
    case t_cement_db:
        if(pView->IsKindOf(RUNTIME_CLASS(CCementRT))) return true;
        break;
    }
    return false;
}
CView* CKRSApp::GetView(typeView t_v)
{
    CDocument* pDoc = GetDoc(t_v);
    if(pDoc == NULL) return NULL;

    POSITION posView = pDoc->GetFirstViewPosition();
    if(posView == NULL)	return NULL;

    return pDoc->GetNextView(posView);
}

void CKRSApp::SendMessageToView(typeView t_v, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    CView* pView = GetView(t_v);
	if(pView) ::SendMessage(pView->m_hWnd, Msg, wParam, lParam);

}
void CKRSApp::SendMessageToActiveView(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    CView* pView = GetActiveView();
	if(pView) ::SendMessage(pView->m_hWnd, Msg, wParam, lParam);
}

void CKRSApp::PostMessageToView(typeView t_v, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    CView* pView = GetView(t_v);
	if(pView) ::PostMessage(pView->m_hWnd, Msg, wParam, lParam);

}
void CKRSApp::PostMessageToActiveView(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    CView* pView = GetActiveView();
	if(pView) ::PostMessage(pView->m_hWnd, Msg, wParam, lParam);

}
void CKRSApp::ChangeStyleView(typeView t_v, ULONG64 style, BOOL flg_set)
{
	CView* pView = GetView(t_v);
	if(pView == NULL) return;
	if(flg_set) ::PostMessage(pView->m_hWnd, PM_SET_STYLE, DWORD(style>>32), DWORD(style));
	else ::PostMessage(pView->m_hWnd, PM_RESET_STYLE, DWORD(style>>32), DWORD(style));
}
void CKRSApp::ChangeStyleActiveView(ULONG64 style, BOOL flg_set)
{
	CView* pView = GetActiveView();
	if(pView == NULL) return;
	if(flg_set) ::PostMessage(pView->m_hWnd, PM_SET_STYLE, DWORD(style>>32), DWORD(style));
	else ::PostMessage(pView->m_hWnd, PM_RESET_STYLE, DWORD(style>>32), DWORD(style));
}
void CKRSApp::ChangeStyleActiveList(ULONG64 style, BOOL flg_set)
{
    CView* pView = GetActiveView();
    if(pView == NULL) return;
    if(flg_set) 
        ::PostMessage(pView->m_hWnd, PM_SET_STYLE_ACTYVE_LIST, DWORD(style>>32), DWORD(style));
    else 
        ::PostMessage(pView->m_hWnd, PM_RESET_STYLE_ACTYVE_LIST, DWORD(style>>32), DWORD(style));
}

void CKRSApp::ChangeStyleAllView(ULONG64 style, BOOL flg_set)
{
	CView* pView = NULL;
	if(m_pDocManager != NULL)
	{
		POSITION posDocTampl = GetFirstDocTemplatePosition();
		CDocTemplate* pTtemplate;

		while(posDocTampl != NULL)
		{
			pTtemplate = GetNextDocTemplate(posDocTampl);
			POSITION posDoc = pTtemplate->GetFirstDocPosition();
			if(posDoc == NULL) continue;

			CDocument* pDoc = pTtemplate->GetNextDoc(posDoc);
			if(pDoc == NULL) continue;

			POSITION posView = pDoc->GetFirstViewPosition();
			if(posView == NULL)	continue;

			pView = pDoc->GetNextView(posView);
			if(pView == NULL)	continue;

			if(flg_set) ::PostMessage(pView->m_hWnd, PM_SET_STYLE, DWORD(style>>32), DWORD(style));
			else ::PostMessage(pView->m_hWnd, PM_RESET_STYLE, DWORD(style>>32), DWORD(style));
		}
	}
}

BOOL CKRSApp::GetPassword(int tp_access)
{
    if(gflDisablePassWd) return true;
    CDlgPassWord dlg;
	dlg.type_access = tp_access;
    if(dlg.DoModal() == IDOK) return true;
    return false;
}

#include "stdafx.h"
#include "KRS.h"
#include "CementRT.h"

IMPLEMENT_DYNAMIC(CDlgPassWord, CDialog)

CDlgPassWord::CDlgPassWord(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPassWord::IDD, pParent)
    , str_password(_T(""))
{

}

CDlgPassWord::~CDlgPassWord()
{
}

void CDlgPassWord::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_PASSWORD, str_password);
}


BEGIN_MESSAGE_MAP(CDlgPassWord, CDialog)
    ON_EN_CHANGE(IDC_PASSWORD, &CDlgPassWord::OnEnChangePassword)
    ON_BN_CLICKED(IDOK, &CDlgPassWord::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CDlgPassWord::OnBnClickedCancel)
END_MESSAGE_MAP()



void CDlgPassWord::OnEnChangePassword()
{
}

void CDlgPassWord::OnBnClickedOk()
{
    UpdateData(true);
    BOOL flg = false;
	if(type_access == 1)
	{
		if(str_password != "SKBOREOL_ADMIN") OnCancel();
		else OnOK();
	}
	else if(type_access == 2)
	{
		if(str_password != "SKBOREOL_USER") OnCancel();
		else OnOK();
	}
}

void CDlgPassWord::OnBnClickedCancel()
{
    OnCancel();
}

void CKRSApp::OnViewPasswd()
{
    if(!gflPassWd) gflPassWd = GetPassword(1);
    else gflPassWd = false;
}
void CKRSApp::OnUpdateViewPasswd(CCmdUI *pCmdUI)
{
    gflPassWd = gflPassWd && !gflDisablePassWd;
}


void CKRSApp::OnDisablePasswd()
{
    gflDisablePassWd = !gflDisablePassWd;
    gflPassWd = gflDisablePassWd;
}


void CKRSApp::OnMenuInterface()
{
	CDlgInterface dlg;
	dlg.DoModal();
}

void CKRSApp::OnUpdateMenuInterface(CCmdUI *pCmdUI)
{
//	pCmdUI->Enable(gflPassWd);
	pCmdUI->Enable(pFrm->GetFlgAccess());
}

void CKRSApp::OnNastroikaModules()
{
	CDlgModulesExternal dlg;
	if (dlg.DoModal() == IDOK)
		::PostMessage(GetActiveView()->m_hWnd, PM_MODULES_CHANGED, 0, 0);
}

void CKRSApp::OnModule(int i)
{
	BaseDoc* pDoc = (BaseDoc*)(pApp->GetActiveView()->GetDocument());
    WinExec(pDoc->m_ListModuleExternal.GetName2(i), SW_SHOW);
}

void CKRSApp::OnModule0()
{
    OnModule(0);
}

void CKRSApp::OnModule1()
{
    OnModule(1);
}

void CKRSApp::OnModule2()
{
    OnModule(2);
}

void CKRSApp::OnModule3()
{
    OnModule(3);
}
void CKRSApp::OnModule4()
{
    OnModule(4);
}

void CKRSApp::OnModule5()
{
    OnModule(5);
}
void CKRSApp::OnModule6()
{
    OnModule(6);
}

void CKRSApp::OnModule7()
{
    OnModule(7);
}
void CKRSApp::OnModule8()
{
    OnModule(8);
}

void CKRSApp::OnModule9()
{
    OnModule(9);
}

void CKRSApp::OnOrientView()
{
	CView* pView1 = GetActiveView();
	if(!pView1->IsKindOf(RUNTIME_CLASS(CMainView)))
		return;
	CMainView* pView = (CMainView*)pView1;
	if(pView == NULL) return;
	pView->SetOrientView();
}

bool CKRSApp::CreateArchieve(bool read, CString filename, CArchive** ar, CFile**file)
{
	CFileException fe;
	*file = new CFile;
	if (*file == NULL)
	{
		LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "CKRSApp::CreateArchieve: *file == NULL");
		return false;
	}
	if (!(*file)->Open(filename, read ? (CFile::modeRead|CFile::shareDenyWrite) : 
		(CFile::modeCreate | CFile::modeWrite | CFile::shareDenyRead), &fe))
	{
		LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "CKRSApp::CreateArchieve: !(*file)->Open(" + filename + ")");
		delete *file;
		return false;
	}
	*ar = new CArchive(*file, (read ? CArchive::load : CArchive::store) | CArchive::bNoFlushOnDelete);
	(*ar)->m_bForceFlat = FALSE;
	return true;
}

void CKRSApp::CloseArchieve(CArchive* ar, CFile* file)
{
	ar->Close();
	file->Close();
	delete file;
	delete ar;
}

CView* CKRSApp::LoadDocArchive(typeView t_v, CString work_path)
{
	CArchive *ar;
	CFile* file;
	CString archieve_path;
	CView *pView = NULL;
	CDocument *pDoc;
	pView = KRS_app.GetView(t_v);
	if (pView != NULL)
	{
		pDoc = pView->GetDocument();
		if (pDoc != NULL)
		{
			switch (t_v)
			{
				case t_real_time: archieve_path = "Main.sv"; break;
				case t_archive: archieve_path = "Main1.sv"; break;
				case t_cement_rt: archieve_path = "Main2.sv"; break;
			}
			if (CreateArchieve(true, work_path + "\\" + archieve_path, &ar, &file))
			{	
				pDoc->Serialize(*ar);
				CloseArchieve(ar, file);
			}
			else
				pFrm->MessageBox("Не удалось прочитать архив", work_path + "\\" + archieve_path);
		}
	}
	return pView;
}

CView* CKRSApp::SaveDocArchive(typeView t_v, CString work_path)
{
	CArchive *ar;
	CFile* file;
	CString archieve_path;
	CView *pView;
	CDocument *pDoc;
	pView = KRS_app.GetView(t_v);
	if (pView != NULL)
	{
		pDoc = pView->GetDocument();
		if (pDoc != NULL)
		{
			switch (t_v)
			{
				case t_real_time: archieve_path = "Main.sv"; break;
				case t_archive: archieve_path = "Main1.sv"; break;
				case t_cement_rt: archieve_path = "Main2.sv"; break;
			}
			CString str = work_path + "\\" + archieve_path;
			if (CreateArchieve(false, str, &ar, &file))
			{	
				pDoc->Serialize(*ar);
				CloseArchieve(ar, file);
			}
			else
			{
				LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "Не удалось сохранить архив:" + str);
				pFrm->MessageBox("Не удалось сохранить архив", str);
			}
		}
	}
	return pView;
}

BOOL CKRSApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CKRSApp::OnAcceleratorColorInterface()
{
	CDlgColorInterface dlg;
	dlg.DoModal();
}

void KRS_SendTOsToDB()
{
	BS_Measurement measurement_to_send;
	measurement_to_send.m_time = COleDateTime::GetCurrentTime().m_dt;
	measurement_to_send.m_deepness = (KRS_project_flag & KRS_PRJ_FLAG_BURENIE) ? pGlubina->fCurAvg : 0;
	measurement_to_send.m_param_values_size = m_MapParamDB.GetCount();
	measurement_to_send.m_param_values = new BS_OneParamValue[m_MapParamDB.GetCount()];
	BS_OneParamValue* ov = measurement_to_send.m_param_values;
	POSITION pos = m_MapParamDB.GetStartPosition();
	int index;
	CParam* param;
	while (pos != NULL)
	{
		m_MapParamDB.GetNextAssoc(pos, index, param);
		ov->m_param_index = index;
		ov->m_param_value = PARAM_TO_VAL;
		ov++;
	}
	DB_SendParamValues(&measurement_to_send);
}

void KSR_StartAllConnections()
{
	DB_connection_info.m_wnd = KRS_app.GetView(t_archive)->m_hWnd;
	DB_connection_info.m_data_received_msg = NULL; // WM_DATA_DB;
	DB_connection_info.m_initialization_complete_msg = WM_INITED_DB;
	DB_connection_info.m_error_occur_msg = WM_ERROR_DB;
	DB_InitConnection();

	SERV_connection_info.m_wnd = pFrm->m_hWnd;
	SERV_connection_info.m_data_received_msg = NULL;//WM_DATA_SERV;
	SERV_connection_info.m_initialization_complete_msg = WM_INITED_SERV;
	SERV_connection_info.m_error_occur_msg = WM_ERROR_SERV;
	SERV_InitConnection();

	CONV_connection_info.m_wnd = pFrm->m_hWnd;
	CONV_connection_info.m_data_received_msg = PM_DATAACPFRM;
	CONV_connection_info.m_initialization_complete_msg = WM_INITED_CONV;
	CONV_connection_info.m_error_occur_msg = WM_ERROR_CONV;
	CONV_connection_info.m_params_info_ready_msg = WM_ON_CONV_LIST_READY;
	CONV_InitConnection();
}

void KSR_StopAllConnections()
{
	DB_ShutDownConnection(true);
	SERV_ShutDownConnection(true);
	CONV_ShutDownConnection(true);
}