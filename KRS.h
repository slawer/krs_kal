#if !defined(AFX_KRS_H__847BC9AF_EE72_437E_AA18_DD951511ACA4__INCLUDED_)
#define AFX_KRS_H__847BC9AF_EE72_437E_AA18_DD951511ACA4__INCLUDED_

#define KRS_DDX_TextCommon(pDX, id, var, format) { if (pDX->m_bSaveAndValidate) { CString str; DDX_Text(pDX, id, str); str.Replace(',', '.'); sscanf(str, format, &var); } else DDX_Text(pDX, id, var); }
#define KRS_DDX_TextFloat(pDX, id, var) KRS_DDX_TextCommon(pDX, id, var, "%f")
#define KRS_DDX_TextDouble(pDX, id, var) KRS_DDX_TextCommon(pDX, id, var, "%lf")

#include "src/cfgfiles.h"

#define KRS_PRJ_FLAG_BURENIE 1
#define KRS_PRJ_FLAG_STAGES 2
extern int KRS_programm_flag; 
extern int KRS_project_flag;
extern int KRS_send_to_bo_interval;

extern CString KRS_root_path, KRS_program_name;
extern CCfgFiles KRS_config_file;
extern bool KRS_running, KRS_need_send_to_db_additional_to;
extern double KRS_time_circ_lifetime, KRS_time_bur_lifetime;

#if !defined _PARAMH
#define _PARAMH
#include "Param.h"
#include "DlgBar1.h"
#include "DlgBarH.h"
#include "DlgLineParam.h"
#include "DialogBarDB.h"
#include "DialogBarCM.h"
#include "DlgPanelCement.h"
#include "ChangeParam.h"
#include "src/SERV.h"

void KRS_ClearMap(CParamMap* map, bool delete_params = true);
void KRS_CheckParamForAlwaysActive(CParam* param);
void KRS_MakeupFormulas();
void KRS_CheckFinishInitialization();
void KRS_DeleteAllViews();
void KRS_SendTOsToDB();
void KSR_StartAllConnections();
void KSR_StopAllConnections();
void KRS_SendCurrentParamsToBD();

typedef struct {
	CString sNameElement;
	CString sName2Element;
	float dlinaElement;
} TElement;

typedef struct {
	COLORREF color_panel_grf_bk;
	COLORREF color_panel_dgt_bk;
	COLORREF color_graph_bk;
	COLORREF color_graph_dgt;
	COLORREF color_graph_dgt_bk;
	COLORREF color_graph_greed;
} MColorInterface;

typedef CMap<int, int, TElement*, TElement*&> MapTElements;

class CTBLElement
{
	MapTElements tblTElements;
	int curent_element;
	float save_len;
public:
	CTBLElement()
	{
		tblTElements.RemoveAll();
		curent_element = 0;
		save_len = 0;
	}
	void Init()
	{
		DeleteAllElements();
		curent_element = 0;
		save_len = 0;
	}

	void Add(CString name, float length)
	{
		TElement *te = new TElement;
		te->sNameElement = name;
		te->dlinaElement = length;
		tblTElements[tblTElements.GetCount()] = te;
	}
	void Add(CString name, CString name2, float length)
	{
		TElement *te = new TElement;
		te->sNameElement = name;
		te->sName2Element = name2;
		te->dlinaElement = length;
		tblTElements[tblTElements.GetCount()] = te;
	}
    void Add(TElement *te)
    {
        Add(te->sNameElement, te->sName2Element, te->dlinaElement);
    }

    void RemoveElement(int num)
    {
        TElement *te;
        if(!tblTElements.Lookup(num, te)) return;
        delete te;
        tblTElements.RemoveKey(num);
    }

	void Set(int num, CString name, float length)
	{
		TElement *te;
		if(!tblTElements.Lookup(num, te)) return;
		te->sNameElement = name;
		te->dlinaElement = length;
	}
	void SetName(int num, CString name)
	{
		TElement *te;
		if(!tblTElements.Lookup(num, te)) return;
		te->sNameElement = name;
	}
    void SetName2(int num, CString name)
    {
        TElement *te;
        if(!tblTElements.Lookup(num, te)) return;
        te->sName2Element = name;
    }
	void SetNameCE(CString name)
	{
		TElement *te;
		if(!tblTElements.Lookup(curent_element, te)) return;
		te->sNameElement = name;
	}
	void SetLen(int num, float length)
	{
		TElement *te;
		if(!tblTElements.Lookup(num, te)) return;
		te->dlinaElement = length;
	}
	void SetLenToLastElm(float length)
	{
		TElement *te;
		int num = tblTElements.GetCount() - 1;
		if(num < 0) return;
		if(!tblTElements.Lookup(num, te)) return;
		te->dlinaElement = length;
	}
	void SetLenCE(float length)
	{
		TElement *te;
		if(!tblTElements.Lookup(curent_element, te)) return;
		te->dlinaElement = length;
	}

	float GetLen(int num)
	{
        if(tblTElements.GetCount() == 0) return 0;
		TElement *te;
		if(!tblTElements.Lookup(num, te)) return 0;
		return te->dlinaElement;
	}
	float GetLenCE()
	{
        if(tblTElements.GetCount() == 0) return 0;
		TElement *te;
		if(!tblTElements.Lookup(curent_element, te)) return 0;
		return te->dlinaElement;
	}
	float GetLenNextElm()
	{
        if(tblTElements.GetCount() == 0) return 0;
		if(curent_element >= tblTElements.GetCount() - 1) return 0.0f;
		TElement *te;
		if(!tblTElements.Lookup(curent_element + 1, te)) return 0.0f;
		return te->dlinaElement;
	}

	float GetLenPrevElm()
	{
        if(tblTElements.GetCount() == 0) return 0;
		if(curent_element <= 0) return 0.0f;
		TElement *te;
		if(!tblTElements.Lookup(curent_element - 1, te)) return 0.0f;
		return te->dlinaElement;
	}

	CString GetName(int num)
	{
        if(tblTElements.GetCount() == 0) return "";
		TElement *te;
		if(!tblTElements.Lookup(num, te)) return "";
		return te->sNameElement;
	}
	CString GetName2(int num)
	{
		if(tblTElements.GetCount() == 0) return "";
		TElement *te;
		if(!tblTElements.Lookup(num, te)) return "";
		return te->sName2Element;
	}
	CString GetName2txe(int num)
	{
		if(tblTElements.GetCount() == 0) return "";
		TElement *te;
		if(!tblTElements.Lookup(num, te)) return "";
		CString cs = te->sName2Element;
		int nn1 = cs.Find(".exe", 0);
		if(nn1 < 0) return "";
		nn1 += 4; 
		cs.Delete(nn1, cs.GetLength() - (nn1 - 1));

		return cs;
	}


	CString GetNameCE()
	{
        if(tblTElements.GetCount() == 0) return "";
		TElement *te;
		if(!tblTElements.Lookup(curent_element, te)) return "";
		return te->sNameElement;
	}
	int GetCount()
	{
		return tblTElements.GetCount();
	}

	void SetNumCurElement(int num)
	{
		if(num < tblTElements.GetCount() - 1 && num >= 0 ) curent_element = num;
	}
	int GetNumCurElement()
	{
		return curent_element;
	}
	int IncCurElement()
	{
		if(curent_element < tblTElements.GetCount() - 2) curent_element++;
		return curent_element;
	}
	int DecrCurElement()
	{
        if(tblTElements.GetCount() == 0) return 0;
		if(curent_element > 0 ) curent_element--;
		return curent_element;
	}
	float GetLenFromBegToCurElem()
	{
		if(tblTElements.GetCount() == 0) return 0; 
		TElement *te;
		save_len = 0.0f;
		for(int i = 0; i <= curent_element; i++)
		{
			if(tblTElements.Lookup(i, te)) save_len += te->dlinaElement;
		}
		return save_len;
	}
	float GetSaveCurLen()
	{
		return save_len;
	}

	void SetSaveCurLen(float cur_save_len)
	{
		save_len = cur_save_len;
	}

	float FindComponovka(float cur_L)
	{
		int nn = GetCount();
		if(nn == 0) return 0;
		if(GetNumCurElement() == nn - 1) return save_len;
		float m_sign = cur_L - save_len;
		if(m_sign > 0)
		{
			while(1) 
			{
				if(GetNumCurElement() == nn - 1) break;
				m_sign -= GetLenNextElm();
				if(abs(m_sign) < 0.1)//щшибка определения
				{
					IncCurElement();
					break;
				}
				if(m_sign < 0) break;
				IncCurElement();
			} 
		}
		else
		{
			while(1) 
			{
				if(GetNumCurElement() == 0) break;
				m_sign += GetLenCE();
				if(abs(m_sign) < 0.1)//щшибка определения
				{
					DecrCurElement();
					break;
				}
				if(m_sign > 0) break;
				DecrCurElement();
			} 
		}
		return GetLenFromBegToCurElem();
	}

	float GetAllLen()
	{
		if(tblTElements.GetCount() == 0) return 0; 
		int mKey;
		TElement *te;
		POSITION pos = tblTElements.GetStartPosition();
		float s0 = 0.0f;
		while (pos != NULL)
		{
			tblTElements.GetNextAssoc(pos, mKey, te);
			if(te != NULL) s0 += te->dlinaElement;
		}
		return s0;
	}

	void DeleteAllElements()
	{
		if(tblTElements.GetCount() == 0) return; 
		int mKey;
		TElement *te;
		POSITION pos = tblTElements.GetStartPosition();
		while (pos != NULL)
		{
			tblTElements.GetNextAssoc(pos, mKey, te);
			if(te != NULL) delete te;
		}
		tblTElements.RemoveAll();
	}

    CTBLElement& operator=(CTBLElement& tbl)
    {
        Init();
        if(tbl.tblTElements.GetCount() == 0) return  *this; 
        int mKey;
        TElement *te;
        POSITION pos = tbl.tblTElements.GetStartPosition();
        while (pos != NULL)
        {
            tbl.tblTElements.GetNextAssoc(pos, mKey, te);
            if(te != NULL) Add(te);
        }
        return *this;
    }
    CTBLElement(CTBLElement& tbl)
    {
        operator=(tbl);
    }
    CTBLElement(CTBLElement* tbl)
    {
        operator=(*tbl);
    }

    void Rebuild()
    {
        CTBLElement* p_tmp = new CTBLElement(this);
        *this = *p_tmp;
        delete p_tmp;
    }

	~CTBLElement()
	{
		DeleteAllElements();
	}
};

class DataList
{
public:
    CParamMap           map_param;

    DataList()
    {
        map_param.RemoveAll();
    }
    DataList(CParamMap& map_prm)
    {
		int nn = map_prm.GetCount();
        SetMap(map_prm);
    }
    ~DataList()
    {
        DeleteAllElements();
    }

    DataList& operator=(DataList& td_list)
    {
        DeleteAllElements();
        SetMap(td_list.map_param);
    }

    void SetMap(CParamMap& map_prm)
    {
        CopyMap(map_prm, map_param);
    }
    void GetMap(CParamMap& map_prm)
    {
        CopyMap(map_param, map_prm);
    }
    static void CopyMap(CParamMap& map_src, CParamMap& map_dst)
    {
        if(map_src.GetCount() > 0) 
        {
            map_dst.RemoveAll();
            int mKey;
            CParam *pPrm;
            POSITION pos = map_src.GetStartPosition();
            while (pos != NULL)
            {
                map_src.GetNextAssoc(pos, mKey, pPrm);
                map_dst[mKey] = pPrm;
            }
        }
    }

    void DeleteAllElements()
    {
        map_param.RemoveAll();
    }

    BOOL AddParamToMap(CParam* pPrm)
    {
        if(pPrm == NULL || pPrm->m_num_par < 0) return false;
        map_param[pPrm->m_num_par] = pPrm; 
        return true;
    }
    BOOL RemoveParamFromMap(int num_par)
    {
        CParam *pPrm;
        if(!map_param.Lookup(num_par, pPrm)) return false; 
        map_param.RemoveKey(num_par);
        return true;
    }
    BOOL ReplaceParamInMap(CParam* pPrm)
    {
        if(pPrm == NULL || pPrm->m_num_par < 0) return false;
        map_param[pPrm->m_num_par] = pPrm;
        return true;
    }
};

typedef CMap<int, int, DataList*, DataList*&> DataListsMap;

class ListsData
{
public:
    DataListsMap data_all_lists;

    ListsData()
    {
    }
    ~ListsData()
    {
        DeleteAllElements();
    }

    void DeleteAllElements()
    {
        if(data_all_lists.GetCount() == 0) return; 
        POSITION pos = data_all_lists.GetStartPosition();
        while (pos != NULL)
        {
            int mKey;
            DataList *data_single_list;
            data_all_lists.GetNextAssoc(pos, mKey, data_single_list);
            if(data_single_list != NULL) delete data_single_list;
        }
        data_all_lists.RemoveAll();
    }
    BOOL RemoveDataList(int num)
    {
        DataList *data_single_list;
        if(!data_all_lists.Lookup(num, data_single_list)) return false;
        data_all_lists.RemoveKey(num);
        return true;
    }
    void AddDataList(CParamMap& map_prm)
    {
        DataList *mp =  new DataList(map_prm);
        data_all_lists[data_all_lists.GetCount()] = mp;
    }
    DataList* GetDataList(int num_list)
    {
        DataList *data_single_list;
        if(!data_all_lists.Lookup(num_list, data_single_list)) return NULL;
        return data_single_list;
    }
	BOOL GetDataList(int num_list, CParamMap& map_prm)
	{
		DataList *data_single_list;
		if(!data_all_lists.Lookup(num_list, data_single_list)) return false;

		if(data_single_list != NULL)
		{
			DataList::CopyMap(data_single_list->map_param, map_prm);
			return true;
		}
		return false;
	}

    BOOL ReplaceMapInList(int num_list, CParamMap& map_param)
    {
        DataList *data_single_list;
        if(!data_all_lists.Lookup(num_list, data_single_list)) return false;
        data_single_list->SetMap(map_param);
        return true;
    }
    BOOL AddParamToMapParam(int num_list, CParam* pPrm)
    {
        DataList *data_single_list;
        if(!data_all_lists.Lookup(num_list, data_single_list)) return false;
        return data_single_list->AddParamToMap(pPrm);
    }
    BOOL RemoveParamFromMapParam(int num_list, int num_par)
    {
        DataList *data_single_list;
        if(!data_all_lists.Lookup(num_list, data_single_list)) return false;
        return data_single_list->RemoveParamFromMap(num_par);
    }
    BOOL ReplaceParamInMapParam(int num_list, int num_par, CParam* pPrm)
    {
        DataList *data_single_list;
        if(!data_all_lists.Lookup(num_list, data_single_list)) return false;
        return data_single_list->ReplaceParamInMap(pPrm);
    }
};

#endif //_PARAMH

typedef CParam* LPPARAM; 

extern COleDateTime KRS_syncro_ole_time;
extern DWORD KRS_syncro_tick;

class CKRSApp;
class CMainFrame;
class CKRSView;
class CDBView;
class CCementView;
class CDlgPanelCement;

#if defined(_PARAM)
#undef _PARAM
CKRSApp     *pApp           = NULL;
CMainFrame  *pFrm           = NULL;
CKRSView    *pRealV         = NULL;
CDBView     *pDBV           = NULL;
CCementView *pCementV       = NULL;

typeView t_view             = t_real_time;

short iZaboi				= 0;
short iTalblock				= 0;
short iGlubinaZaboia		= 0;	
short iLinstrumenta			= 0;
short iNknbk				= 0;
short iZahodKvadrata		= 0;
short iVisotaElevatora		= 0;
short iPologenieKnbk		= 0;
short iKruk					= 0;
short iDoloto				= 0;	
short iKluch				= 0;	
short iRotorMoment			= 0;
short iDavlVhoda			= 0;		
short iMechanikSkorost		= 0;	
short iSkorostSPO			= 0;		
short iVremiaCirkuliacii	= 0;		
short iTimeBurenia			= 0;		
short iTime					= 0;		
short iShurf				= 0;		
short iState				= 0;
short iGlubina				= 0;
short iRotoBur				= 0;
short iTest1				= 0;
short iTest2				= 0;
CParam *pZaboi					= NULL;
CParam *pTalblock				= NULL;
CParam *pGlubinaZaboia			= NULL;
CParam *pLinstrumenta			= NULL;
CParam *pNkbk					= NULL;	
CParam *pZahodKvadrata			= NULL;	
CParam *pVisotaElevatora		= NULL;
CParam *pPologenieKnbk			= NULL;	
CParam *pKruk					= NULL;	
CParam *pDoloto					= NULL;	
CParam *pKluch					= NULL;
CParam *pRotorMoment			= NULL;
CParam *pDavlVhoda				= NULL;
CParam *pMechanikSkorost		= NULL;
CParam *pSkorostSPO				= NULL;
CParam *pVremiaCirkuliacii		= NULL;
CParam *pTimeBurenia			= NULL;
CParam *pTime					= NULL;
CParam *pShurf					= NULL;
CParam *pState					= NULL;
CParam *pGlubina				= NULL;
CParam *pRotorBur				= NULL;
CParam *pTest1					= NULL;
CParam *pTest2					= NULL;
CParam *pImpl					= NULL;
list<int> KRS_always_active_params;
list<int> KRS_always_not_timeout_params;

int iNumActiveList = 0;
int iMaxLists = 0;

CDlgBar1		m_wndDialogBar;
CDLgBarH		m_wndDialogBarH;
CDlgLineParam	m_wndDaligLineParam;
CDialogBarDB	m_wndDialogBarDB;
CDialogBarCM	m_wndDialogBarCM;
CDlgPanelCement	m_wndDialogPanelCement;

CMutex			m_params_map_mutex; 
CMutex			m_params_db_map_mutex;
CParamMap		m_MapParam;
SERV_DeviceCfg	m_convertor_cfg;
SERV_Device		m_convertor_device("Конвертер", 0, 1000, &m_convertor_cfg);

CParamMap		m_MapParamDB;
CParamMap		m_MapParamDefault;
CParamMap*		m_MapParamCurent;
CParamMap		m_MapParamCM, m_MapParamCM_additional_only;
//ListsData       gListData;

double	gDolBlok = 10.0f;
double	gDolMax = 20.0f;
double	gIZaboiValue = 0.0f;
double	gDavlenieAvaria = 100.0f;
double	gDavlenieBlok = 5.0f;
double	gDavlenieMin = 10.0f;
double	gGlubinaZaboia = 0.0f;
double	gGlubina = 0.0f;

CTBLElement gtblElement;

double	gMomentMaxLevel = 100.0f;
int gnFlag1 = -1, gnFlag2 = 0, gnFlag3 = 0;
double gMomentDiapazon = 30.0f, gMomentPB = 30.0f;

double gKvadratDlina = 15.0f, gfKvadratPerehodnik = 2.5f, gfKvadratElevator = 2.5f;
double	gVesAvaria = 300.0f, gVesBlok = 5.0f, gfVesDiapazon = 100.0f;

double gfTalblokDiapazon = 20.0f, gTalblokAvaria = 35.0f;
double gPorogTime = 1.0f, gPorogVisota = 0.1f;

int gKNBKList = 1;
CString gKNBKPath = "C:\\KBI.xls";

double gWk = 2.0f;
double gfTalblokDeltaH = 0.0f;

double gMexInterval = 3.0f, gSPOInterval = 10.0f;
double  gStartTimeCirc = 0.0f, gStartTimeBur = 0.0f;

BOOL gflagButtonNull = 0;

int gnTalblok = 0;

double gPorogBur = 1.0f;

BOOL gflZaboiIsSetted = 0;

CString	gMestorogdenie = "Новое";
CString	gKust = "Кустистый";
CString	gRabota = "Тяжелая";
CString	gSkvagina = "Перспективная";
double gDeltaTimeAlert = 0;
int gflgViewMarkers = 0;
int gflgViewPanelBur = 0;
int gflgViewDiag = 0;
int gflgViewDgtRT = 0;
int gflgViewDgtBD = 0;
int gflgViewRealArch = 0;
int gflgViewTimeGlub = 0;
int gflgDataBegEnd = 0;
int gflgGrfStupenki = 0;
int gflgScrollAll = 0;
int	gflgIndParBur = 0;
int	gflgIndHighPanel = 0;
int	gflgDgtGraphPanel = 0;
int	gflgIndDataTime = 0;
int gflgIndConverter = 0;
int gflgIndRegimeBur = 0;
int gflgSelectOnlyY = 0;

int	giMinut = 0;
int	giHour = 0;
int	giDay = 1;
int	giMonth = 1;
int	giYear = 07;
int	giTimeLength = 1;
float gfTimeLengthData = 1;
float gfGlubLengthData = 10;
int	giTimeLengthMinut = 0;
int gfRequestGlubStart = 0;
int gfRequestGlubLen = 100;
int gflgCheckTimeGlub = 1;

int KRS_start_counter = 0;

double gPorogRotorBur = 50.0f;
double gVesRotorBur = 50.0f;
int gControlRotorBur = 0;
BOOL gflgRabotaBezShurfa = 0;

FRingBuf<double> *pgtmData = NULL, *pgcurGlub = NULL;
FRingBuf<double> *pgtmDataDB = NULL, *pgcurGlubDB = NULL;
FRingBuf<double> *pgtmDataCM = NULL, *pgcurGlubCM = NULL;
LPVOID pgBufX = NULL, pgBufXDB = NULL, pgBufXCM = NULL;

BOOL gflPassWd = false;
BOOL gflPassWdUser = false;
BOOL gflDisablePassWd = false;

typeView gt_cement = t_cement_rt;
int gflgDataReaded = 0;
MColorInterface g_color_interface;
int g_StyleGreedGraph = PS_SOLID;

int G_HORZRES = 100;
int G_VERTSIZE = 100;
double gfDLi = 0.0f;
double gfLinstrumenta = 0.0f;

#else
extern CKRSApp     *pApp;
extern CKRSApp KRS_app;

extern CMainFrame  *pFrm;
extern CKRSView    *pRealV;
extern CDBView     *pDBV;
extern CCementView *pCementV;

extern typeView t_view;

extern bool KRS_shutdown;
extern short iZaboi;
extern short iTalblock;
extern short iGlubinaZaboia;	
extern short iLinstrumenta;
extern short iNknbk;
extern short iZahodKvadrata;
extern short iVisotaElevatora;
extern short iPologenieKnbk;
extern short iKruk;
extern short iDoloto;	
extern short iKluch;	
extern short iRotorMoment;
extern short iDavlVhoda;		
extern short iMechanikSkorost;	
extern short iSkorostSPO;		
extern short iVremiaCirkuliacii;		
extern short iTimeBurenia;
extern short iTime;
extern short iShurf;
extern short iState;
extern short iGlubina;
extern short iRotoBur;
extern short iTest1;
extern short iTest2;
extern CParam *pZaboi;
extern CParam *pTalblock;
extern CParam *pGlubinaZaboia;
extern CParam *pLinstrumenta;
extern CParam *pNkbk;
extern CParam *pZahodKvadrata;
extern CParam *pVisotaElevatora;
extern CParam *pPologenieKnbk;
extern CParam *pKruk;
extern CParam *pDoloto;
extern CParam *pKluch;
extern CParam *pRotorMoment;
extern CParam *pDavlVhoda;
extern CParam *pMechanikSkorost;
extern CParam *pSkorostSPO;
extern CParam *pVremiaCirkuliacii;
extern CParam *pTimeBurenia;
extern CParam *pTime;
extern CParam *pShurf;
extern CParam *pState;
extern CParam *pGlubina;
extern CParam *pRotorBur;
extern CParam *pTest1;
extern CParam *pTest2;
extern CParam *pImpl;
extern list<int> KRS_always_active_params;
extern list<int> KRS_always_not_timeout_params;

extern int iNumActiveList;
extern int iMaxLists;

extern CDlgBar1			m_wndDialogBar;
extern CDLgBarH			m_wndDialogBarH;
extern CDlgLineParam	m_wndDaligLineParam;
extern CDialogBarDB		m_wndDialogBarDB;
extern CDialogBarCM		m_wndDialogBarCM;
extern CDlgPanelCement	m_wndDialogPanelCement;

extern CMutex			m_params_map_mutex; 
extern CMutex			m_params_db_map_mutex;
extern CParamMap		m_MapParam;
extern SERV_DeviceCfg	m_convertor_cfg;
extern SERV_Device		m_convertor_device;

extern CParamMap		m_MapParamDB;
extern CParamMap		m_MapParamDefault;
extern CParamMap*		m_MapParamCurent;
extern CParamMap		m_MapParamCM, m_MapParamCM_additional_only;
//extern ListsData        gListData;

extern double	gDolBlok;
extern double	gDolMax;
extern double	gIZaboiValue;
extern double	gDavlenieAvaria;
extern double	gDavlenieBlok;
extern double	gDavlenieMin;
extern double	gGlubinaZaboia;
extern double	gGlubina;

extern CTBLElement gtblElement;

extern double	gMomentMaxLevel;
extern int gnFlag1, gnFlag2, gnFlag3;
extern double gMomentDiapazon, gMomentPB;

extern double gKvadratDlina, gfKvadratPerehodnik, gfKvadratElevator;
extern double	gVesAvaria, gVesBlok, gfVesDiapazon;

extern double gfTalblokDiapazon, gTalblokAvaria;
extern double gPorogTime, gPorogVisota;

extern int gKNBKList;
extern CString gKNBKPath;

extern double gWk;
extern double gfTalblokDeltaH;

extern double gMexInterval, gSPOInterval;
extern double  gStartTimeCirc, gStartTimeBur;

extern BOOL gflagButtonNull;

extern int gnTalblok;

extern double gPorogBur;

extern BOOL gflZaboiIsSetted;


extern CString	gMestorogdenie;
extern CString	gKust;
extern CString	gRabota;
extern CString	gSkvagina;

//extern double gTimeScale, gGlubScale, gTimeScaleDB, gGlubScaleDB;
//extern double gDeltaTime, gDeltaTimeAlert, gDeltaH, gDeltaTimeDB, gDeltaHDB;
extern double gDeltaTimeAlert;

extern int gflgViewMarkers;
extern int gflgViewPanelBur;
extern int gflgViewDiag;
extern int gflgViewDgtRT;
extern int gflgViewDgtBD;
extern int gflgViewRealArch;
extern int gflgViewTimeGlub;
extern int gflgDataBegEnd;
extern int gflgGrfStupenki;
extern int gflgScrollAll;
extern int gflgIndParBur;
extern int gflgIndHighPanel;
extern int gflgDgtGraphPanel;
extern int gflgIndDataTime;
extern int gflgIndConverter;
extern int gflgIndRegimeBur;
extern int gflgSelectOnlyY;

extern int	giMinut;
extern int	giHour;
extern int	giDay;
extern int	giMonth;
extern int	giYear;
extern int	giTimeLength;
extern float gfTimeLengthData;
extern float gfGlubLengthData;
extern int	giTimeLengthMinut;
extern int gfRequestGlubStart;
extern int gfRequestGlubLen;
extern int gflgCheckTimeGlub;

extern int KRS_start_counter;

extern double gPorogRotorBur;
extern double gVesRotorBur;
extern int gControlRotorBur;

extern BOOL gflgRabotaBezShurfa;

extern FRingBuf<double> *pgtmData, *pgcurGlub;
extern FRingBuf<double> *pgtmDataDB, *pgcurGlubDB;
extern FRingBuf<double> *pgtmDataCM, *pgcurGlubCM;
extern LPVOID pgBufX, pgBufXDB, pgBufXCM;

extern BOOL gflPassWd;
extern BOOL gflPassWdUser;
extern BOOL gflDisablePassWd;

extern typeView gt_cement;
extern int gflgDataReaded;
extern MColorInterface g_color_interface;
extern int g_StyleGreedGraph;

extern int G_HORZRES;
extern int G_VERTSIZE;
extern double gfDLi;
extern float gGlI, gGlX;

extern double gfLinstrumenta;

#endif //_SPPARAM


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       

#define MAX_BUF_RTIME	1000000  
#define MAX_BUF_DB	1000000  
#define MAX_PARAM 80

#define PM_DATAACP				WM_USER + 127
#define PM_DATAACPFRM			WM_USER + 136

#define PM_MESSAGETODLGKOFF		WM_USER + 100
#define PM_MESSAGETODLGKALIBR	WM_USER + 104
#define PM_CHANGESCALE			WM_USER + 108
#define PM_FIILLBUFFDB			WM_USER + 112
#define PM_CHANGE_ACTIVE_LIST	WM_USER + 118
#define PM_CHANGE_NUM_GRF		WM_USER + 120
#define PM_CHANGE_TIME_GLUB		WM_USER + 122
#define PM_CHANGE_POSITION		WM_USER + 124
#define PM_DATA_CHANGED			WM_USER + 126
#define PM_RECALC_ACTIVE_LIST	WM_USER + 128
#define PM_CHANGE_PARAM_PROPERT_1	WM_USER + 129
#define PM_CHANGE_PARAM_PROPERT	WM_USER + 130
#define PM_DELETE_PARAM			WM_USER + 131
#define PM_ADD_PARAM			WM_USER + 132
#define PM_ADD_PARAM2			WM_USER + 133
#define PM_RECALC_SIZE			WM_USER + 134
#define PM_RECALC_SIZE_DGT_PANELS			WM_USER + 136
#define PM_CHANGE_TYPE_VIEW		WM_USER + 138
#define PM_GET_DATA_DB			WM_USER + 140
#define PM_DATA_DB_CHANGED		WM_USER + 142
#define PM_R_BUTTON_DOWN		WM_USER + 144
#define PM_SHOW_PANEL		   	WM_USER + 146
#define PM_CHANGE_NAME_PANEL	WM_USER + 147
#define PM_SAVE_ALL_LIST		WM_USER + 148
#define PM_SET_LIST_DB			WM_USER + 150
#define PM_UPDATE_VIEW			WM_USER + 152
#define PM_STYLE_GRF_CHANGED	WM_USER + 154
#define PM_SET_STYLE	       	WM_USER + 156
#define PM_RESET_STYLE	     	WM_USER + 158
#define PM_CHANGE_RECT_RANGE   	WM_USER + 160
#define PM_DELETE_EXT_PARAM		WM_USER + 162
#define PM_REDRAW_ACTIVE_LIST	WM_USER + 164
#define PM_REQUEST_DB_FINISHED	WM_USER + 166
#define PM_MAXIMIZE_FRAME   	WM_USER + 168
#define PM_DELETE_PARAM_FROM_LIST   	WM_USER + 170
#define PM_SYNCHRONIZE_GRAPH   	WM_USER + 172
#define PM_SET_STYLE_ACTYVE_LIST      	WM_USER + 174
#define PM_RESET_STYLE_ACTYVE_LIST     	WM_USER + 176

#define WM_DATA_DB		((WM_USER + 0) + 1)
#define WM_INITED_DB	((WM_USER + 0) + 2)
#define WM_ERROR_DB		((WM_USER + 0) + 3)
#define WM_DATA_CONV	((WM_USER + 3) + 1)
#define WM_ERROR_CONV	((WM_USER + 3) + 2)
#define WM_INITED_CONV	((WM_USER + 3) + 3)
#define WM_DATA_SERV	((WM_USER + 6) + 1)
#define WM_ERROR_SERV	((WM_USER + 6) + 2)
#define WM_INITED_SERV	((WM_USER + 6) + 3)
#define WM_ON_CONV_LIST_READY (WM_USER + 101)

#define PM_MENU_EXTERNAL   	    WM_USER + 2000
#define PM_MENU_LIST_EXTERNAL  	WM_USER + 2002
#define PM_MODULES_CHANGED   	WM_USER + 2004

#define PM_MENU_STARTUP   	    WM_USER + 3000


#define G_MAX_VIEWS 500			
#define G_MAX_FIELDS_GRAPHS 3
#define G_MAX_FIELDS_PANELS 3


class CKRSApp : public CWinApp
{
public:

	FILE* m_params_file;
	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);

	CKRSApp();
	~CKRSApp();

	void SetIniFileMode(CString file_path);
	void SetRegistryMode(bool only_registry = false);

	void GetSection_NET();
	void WriteSection_NET();
	void GetSection_DB();
	void WriteSection_DB();

	void ReadParamFromRegistry(CParam *Param);
	void WriteParamToRegistry(CParam *pParam, bool write_formula_text = false);
	void ReadAllRegistriData();
	void WriteAllRegistryData();
	void WriteRegistriCheckParam(LPARAM lParam);
	void WriteRegistriSelectParam(LPARAM lParam);
	BOOL SaveParamFromList(int numList, int numPanel, ParamInPanel param_descr, int numPositionInPanel, typeView t_v);
	void DeleteParamFromList(int numList, int numPanel, int pos, typeView t_v);
	BOOL RestoreDataList(int numList, int numPanel, LPVOID plstParam, typeView t_v);
	BOOL ReadPanelList(int numList, int numPanel, LPVOID plstParam, typeView t_v);
	BOOL WritePanelList(int numList, int numPanel,  LPVOID plstParam, typeView t_v);
	void WriteFloatValueToRegistry(CString strSection, CString strEntry, double value);
	double ReadFloatValueFromRegistry(CString strSection, CString strEntry, double default_value = 0.0);
	void WriteIntegerValueToRegistry(CString strSection, CString strEntry, int iValue);
	int ReadIntegerValueFromRegistry(CString strSection, CString strEntry, int default_val = 0);
	void GetDataFromXLS();
    CDocument* NewDoc(typeView t_v);
    CDocument* GetDoc(typeView t_v);
    CView* SetActiveView(typeView t_v);
    CView* GetActiveView();
    BOOL IsActiveView(typeView t_v);
    CView* GetView(typeView t_v);
    void SendMessageToView(typeView t_v, UINT Msg, WPARAM wParam, LPARAM lParam);
    void SendMessageToActiveView(UINT Msg, WPARAM wParam, LPARAM lParam);
    void PostMessageToView(typeView t_v, UINT Msg, WPARAM wParam, LPARAM lParam);
    void PostMessageToActiveView(UINT Msg, WPARAM wParam, LPARAM lParam);
    BOOL GetPassword(int tp_access);
	void ChangeStyleView(typeView t_v, ULONG64 style, BOOL flg_set);
	void ChangeStyleActiveView(ULONG64 style, BOOL flg_set);
	void ChangeStyleAllView(ULONG64 style, BOOL flg_set);
    void ChangeStyleActiveList(ULONG64 style, BOOL flg_set);
	void NewProject();
	bool CreateArchieve(bool read, CString filename, CArchive** ar, CFile**file);
	void CloseArchieve(CArchive* ar, CFile* file);
	CView* LoadDocArchive(typeView t_v, CString work_path);
	CView* SaveDocArchive(typeView t_v, CString work_path);

	bool m_KRS_ini_exists;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKRSApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CKRSApp)
	afx_msg void OnAppAbout();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnViewPasswd();
    afx_msg void OnDisablePasswd();
    afx_msg void OnUpdateViewPasswd(CCmdUI *pCmdUI);
	afx_msg void OnMenuInterface();
	afx_msg void OnUpdateMenuInterface(CCmdUI *pCmdUI);
	afx_msg void OnNastroikaModules();
	void OnModule(int i);
    afx_msg void OnModule0();
    afx_msg void OnModule1();
    afx_msg void OnModule2();
    afx_msg void OnModule3();
    afx_msg void OnModule4();
    afx_msg void OnModule5();
    afx_msg void OnModule6();
    afx_msg void OnModule7();
    afx_msg void OnModule8();
    afx_msg void OnModule9();
	afx_msg void OnOrientView();
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
public:
	afx_msg void OnAcceleratorColorInterface();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KRS_H__847BC9AF_EE72_437E_AA18_DD951511ACA4__INCLUDED_)
#pragma once


class CDlgPassWord : public CDialog
{
	DECLARE_DYNAMIC(CDlgPassWord)

public:
	CDlgPassWord(CWnd* pParent = NULL);   
	virtual ~CDlgPassWord();

	enum { IDD = IDD_DLGPASSWORD };
	int type_access;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:
    CString str_password;
    afx_msg void OnEnChangePassword();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
};
