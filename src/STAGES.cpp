#include <StdAfx.h>
#include "STAGES.h"
#include "BS.h"
#include "DBGET.h"
#include "TXT.h"
#include "SERV.h"
#include "PRJ.h"
#include "..\DlgStageSettings.h"
#include "..\DlgEditText.h"
#include "..\KRS.h"
#include "..\CementView.h"
#include "..\DlgAddStage.h"
#include "..\DlgEditCalculation.h"
#include "..\DlgWaitDB.h"
#include "..\DlgWork.h"
#include "..\MainFrm.h"

#include "TextMenu.h"

#include <math.h>

#define ID_COLLAPSE 0
#define ID_MENU 1

#define ID_DELETE 2
#define ID_UP 3
#define ID_DOWN 4
#define ID_ADD 5
#define ID_EDIT 6

#define WM_MENU (WM_USER + 1)
STAGES_StageTemplates STAGES_stage_templates;
int STAGES_zazor = 3;
CString STAGES_last_file_path;
//bool STAGES_next_open_stages_make_editable = false;

static CString g_title_selected_stage = "Выбранный этап";
static CString g_title_stages_format = "Задание на работу   >   %s";
static CString g_title_stages = "ошибка";
static CString g_title_start_time = "Начало: ";
static CString g_title_duration = "Длительность: ";
static CString g_title_columns[7] = {"Вычисление", "Значение", "Плановое", "Отклонение", "Минимум", "Максимум", "Среднее"};
static CString g_delete_button_title = "[Удалить] ";
static CString g_move_up_button_title = "[Вверх] ";
static CString g_move_down_button_title = "[Вниз] ";
static CString g_title_request_action = " Подтвердите действие";
static CString g_title_wrong_format = " Неверный формат данных";
static CString g_ensure_saved_text = "\nТекущее задание является выполненым. При загрузке шаблона данные буду утеряны.";

static DWORD g_current_db_stage_num = 0;
static HANDLE g_stage_finished_from_db_event = CreateEvent(NULL, FALSE, FALSE, NULL);
static HANDLE g_stop_getting_from_db_event = CreateEvent(NULL, FALSE, FALSE, NULL);
static HANDLE g_getting_from_db_finished_event = CreateEvent(NULL, FALSE, FALSE, NULL);

static bool g_inited = false;
static bool g_header_icon_down = true;
static byte g_arrow_counter = 0, g_max_arrow_counter = 0;

static STAGES_Stages g_stages;
static STAGES_List *g_list_stages = NULL, *g_list_selected_stage = NULL, *g_current_list = NULL;
static DWORD g_colors[6];

#define FILE_STR_LEN 50000
static char g_file_str[FILE_STR_LEN];

#define STAGES_FILE_VERSION 11
static int g_stages_file_version = -1;

#define STAGE_TEMPLATES_FILE_VERSION 3
static int g_stages_templates_file_version = -1;

#define STAGE_SEQUENCE_FILE_VERSION 2
static int g_stages_sequence_file_version = -1;

static DWORD g_selected_stage_num = 0, g_executing_stage_num = 0;

static CFont
	*g_sel_stage_main_font = NULL, *g_sel_stage_header_font = NULL,
	*g_all_stages_main_font = NULL, *g_all_stages_header_font = NULL;
static LOGFONT g_sel_stage_lf, g_all_stages_lf;

static STAGES_Stage *g_executing_stage = NULL;

CCementView* g_cement_view = NULL;

static STAGES_Mode g_current_mode = STAGES_MODE_EDIT;

static STAGES_TextInfo* g_selected_text_info = NULL;

static bool g_drawing_selected_text_info = false;

static int g_prev_column_stages_width = -1, g_prev_column_selected_width = -1;

struct StagesMenu : TextMenu
{
	StagesMenu():
		TextMenu(25, 6, TextMenu_Right, -3)
	{
	}
	virtual void OnClose()
	{
		TextMenu::OnClose();
		g_list_stages->m_mouse_in_item = -1;
	}
	virtual void OnSelect(int pos, DWORD data, CString str)
	{
		g_list_stages->PostMessage(WM_MENU, data, 0);
	}
	STAGES_Stage *m_stage;
	STAGES_Calculation *m_calculation;
};

static StagesMenu g_stages_menu;
void SetAdditionalParamsTimeout()
{
	CParam *param;
	STAGES_Calculation *calculation;
	STAGES_Calculations::iterator current_calculation;
	STAGES_Stage *stage;
	STAGES_Stages::iterator current_stage = g_stages.begin();
	while (current_stage != g_stages.end())
	{
		stage = *current_stage;
		if (stage->m_stage_num == g_executing_stage_num)
		{
			current_calculation = stage->m_calculations.begin();
			while (current_calculation != stage->m_calculations.end())
			{
				calculation = *current_calculation;
				if (calculation->m_has_graph &&
					m_MapParamCM_additional_only.Lookup(calculation->m_corresponding_param_num, param)
					)
				{
					param->StartTimeOut();
					param->m_send_to_db_status = PARAM_DB_STATUS_TO;
					calculation->m_status = FORMULA_CALC_STATUS_TIMEOUT;
				}
				current_calculation++;
			}
			break;
		}
		current_stage++;
	}
}

bool SetExecutingStagePointer(bool collaps_all)
{
	STAGES_Stages::iterator current = g_stages.begin();
	while (current != g_stages.end() && (*current)->m_stage_num < g_executing_stage_num)
	{
		if (collaps_all)
			(*current)->m_collapsed_in_list = true;
		current++;
	}
	if (current == g_stages.end())
	{
		if (g_stages.size() > 0)
			g_list_stages->MessageBox("Внутренняя ошибка программы (current == g_stages.end())", " Ошибка", MB_OK);
		g_executing_stage_num = 0;
		g_executing_stage = NULL;
		return false;
	}
	g_executing_stage = (*current);
	if (collaps_all)
	{
		while (current != g_stages.end())
		{
			(*current)->m_collapsed_in_list = true;
			current++;
		}	
	}
	return true;
}

void StagesSelectionChanged(bool stages_num_changed)
{
	if (stages_num_changed)
	{
		m_wndDialogBarCM.m_PanelKnopki.Init(STAGES_GetStagesNumber(), CSize(20,20), g_cement_view);
		m_wndDialogBarCM.Invalidate(TRUE);
	}
	m_wndDialogBarCM.m_PanelKnopki.PushBtn(g_selected_stage_num - 1);
	g_cement_view->ChangeaActiveListA(WPARAM(g_selected_stage_num - 1), 0);
}

void ListSizeChanged()
{
	g_cement_view->RecalcStagesList();
	m_wndDialogPanelCement.Invalidate(FALSE);
	m_wndDialogBarCM.Invalidate(FALSE);
}

void DeleteOneParam(STAGES_Stage* stage, int param_num)
{
	CParam *param;
	if (!m_MapParamCM_additional_only.Lookup(param_num, param))
		return;
	ParamOfView* pov = g_cement_view->m_parOfView->GetList(stage->m_stage_num - 1);
	if (pov)
		pov->DeleteExtParam(param_num);
	m_MapParamCM_additional_only.RemoveKey(param_num);
	if (param)
		delete param;
}

void DeleteParamOfOneCalculation(STAGES_Stage* stage, STAGES_Calculation *calculation)
{
	if (!calculation->m_has_graph)
		return;
	DeleteOneParam(stage, calculation->m_corresponding_param_num);
}

void DeleteParamsOfStage(STAGES_Stage* stage)
{
	STAGES_Calculations::iterator current_calculation = stage->m_calculations.begin();
	while (current_calculation != stage->m_calculations.end())
	{
		DeleteParamOfOneCalculation(stage, *current_calculation);
		current_calculation++;
	}
}

void SetupParamForCalculation(CParam* param, STAGES_Calculation *calculation)
{
	param->sName = "Вычисление " + calculation->m_name;
	param->sGraphName = calculation->m_name;
	param->sRazmernPar = "ед";
	param->m_nTypePar = PARAM_TYPE_FORMULA;
	param->m_formula = calculation->m_formula->MakeCopy();
	param->m_shown_digits = 3;
	param->bParamActive = true;
	param->fl_db = 0;
	param->flDB = FALSE;

	param->fMin = 0;
	param->fMax = 0;
	param->fBlock = 0;
	param->fAvaria = 0;
	param->nKodControl = 0;

	param->fGraphMIN = calculation->m_graph_min_val;
	param->fGraphMAX = calculation->m_graph_max_val;
	param->m_color = calculation->m_graph_color;
	param->m_line_width = calculation->m_graph_line_width;
	param->m_db_time_interval = calculation->m_db_time_interval;
	param->m_db_data_interval = calculation->m_db_data_interval;
}

void CreateParamForOneCalculation(STAGES_Calculation *calculation)
{
	if (!calculation->m_has_graph)
		return;

	CParam *param;
	int param_num = calculation->CorrespondingParamNum();

	if (!m_MapParamDefault.Lookup(param_num, param) && param_num >= STAGES_START_ADDITIONAL_PARAM_NUM) // вычисление нуждается в параметре
	{
		param = new CParam();
		param->m_num_par = param_num;
		SetupParamForCalculation(param, calculation);
		param->Init(pgtmData->GetCur());
		m_MapParamCM_additional_only[param_num] = param;
	}
}

void CreateParamsForOneStage(STAGES_Stage *stage)
{
	STAGES_Calculations::iterator current_calculation = stage->m_calculations.begin();
	while (current_calculation != stage->m_calculations.end())
	{
		CreateParamForOneCalculation(*current_calculation);
		current_calculation++;
	}
}

bool AddStage()
{
	CPoint p = g_list_stages->m_mouse_point;
	g_list_stages->ClientToScreen(&p);
	LPARAM lp = (p.x << 16) | (p.y & 0xFFFF);
	DlgAddStage dlg(lp);

	if (dlg.DoModal() != IDOK || dlg.m_selected_stage_template == -1)
	{
		g_list_stages->m_mouse_in_item = -1;
		return false;
	}
	g_list_stages->m_mouse_in_item = -1;
	STAGES_Stage* stage = new STAGES_Stage(STAGES_stage_templates[dlg.m_selected_stage_template]);
	stage->m_stage_num = g_stages.size() + 1;
	g_stages.push_back(stage);
	CreateParamsForOneStage(stage);
	g_cement_view->AddList();

	m_wndDialogBarCM.m_PanelKnopki.Init(STAGES_GetStagesNumber(), CSize(20,20), g_cement_view);
	m_wndDialogBarCM.m_PanelKnopki.PushBtn(g_selected_stage_num - 1);
	g_cement_view->RecalcStagesList();
	m_wndDialogBarCM.Invalidate(TRUE);

	if (g_stages.size() == 1)
		g_cement_view->ChangeaActiveListA(0, 0);
	return true;
}

bool AddCalculation(STAGES_Stage* stage)
{
	STAGES_CalculationTemplate* ct = new STAGES_CalculationTemplate;
	DlgEditCalculation dlg(ct);
	if (dlg.DoModal() == IDCANCEL)
	{
		g_list_stages->m_mouse_in_item = -1;
		delete ct;
		return false;
	}
	g_list_stages->m_mouse_in_item = -1;
	STAGES_Calculation* calculation = new STAGES_Calculation(ct);
	delete ct;
	stage->m_calculations.push_back(calculation);
	CreateParamForOneCalculation(calculation);
	if (stage->m_stage_num == g_selected_stage_num)
		STAGES_UpdateMapParam();	
	return true;
}

bool DeleteStage(STAGES_Stage* stage)
{
	if (g_list_stages->MessageBox("Удалить этап?", g_title_request_action, MB_YESNO) != IDYES)
	{
		g_list_stages->m_mouse_in_item = -1;
		return false;
	}
	g_list_stages->m_mouse_in_item = -1;

	int new_sel_stage = g_selected_stage_num;
	bool decrease_num = false;
	STAGES_Stages::iterator current_stage = g_stages.begin(), doomed_stage;
	while (current_stage != g_stages.end())
	{
		if (decrease_num)
			(*current_stage)->m_stage_num--;

		if (*current_stage == stage)
		{
			if (g_selected_stage_num > stage->m_stage_num || g_selected_stage_num == g_stages.size()/*в случае равенства только в последнем элементе, иначе можно селекшын не сдвигать*/)
				new_sel_stage--;
			doomed_stage = current_stage;
			current_stage++;
			g_stages.erase(doomed_stage);
			g_cement_view->DeleteList(stage->m_stage_num - 1);
			decrease_num = true;
			DeleteParamsOfStage(stage);
			delete stage;
			stage = NULL;
			STAGES_SetSelectedStage(new_sel_stage);
		}
		else
			current_stage++;
	}
	return true;
}

bool DeleteAllStages1()
{
	if (g_list_stages->MessageBox("Удалить все этапы?", g_title_request_action, MB_YESNO) != IDYES)
	{
		g_list_stages->m_mouse_in_item = -1;
		return false;
	}
	g_list_stages->m_mouse_in_item = -1;

	int new_sel_stage = g_selected_stage_num;
	bool decrease_num = false;
	STAGES_Stages::iterator current_stage = g_stages.begin(), doomed_stage;
	while (current_stage != g_stages.end())
	{
		g_cement_view->DeleteList(0);
		DeleteParamsOfStage(*current_stage);
		delete *current_stage;
		current_stage++;
	}
	g_stages.clear();
	STAGES_SetSelectedStage(0);
	return true;
}

bool DeleteCalculation(STAGES_Stage* stage, STAGES_Calculation* calculation)
{
	if (g_list_stages->MessageBox("Удалить вычисление?", g_title_request_action, MB_YESNO) != IDYES)
	{
		g_list_stages->m_mouse_in_item = -1;
		return false;
	}
	g_list_stages->m_mouse_in_item = -1;

	STAGES_Calculations::iterator current_calculation = stage->m_calculations.begin();
	while (current_calculation != stage->m_calculations.end())
	{
		if (*current_calculation == calculation)
		{
			stage->m_calculations.erase(current_calculation);
			DeleteParamOfOneCalculation(stage, calculation);
			delete calculation;
			if (stage->m_stage_num == g_selected_stage_num)
				STAGES_UpdateMapParam();			
			return true;
		}
		current_calculation++;
	}
	return false;
}

bool EditCalculation(STAGES_Stage* stage, STAGES_Calculation* calculation)
{
	bool prev_has_graph = calculation->m_has_graph;
	int prev_param_num = calculation->m_corresponding_param_num;

	DlgEditCalculation dlg(calculation);
	if (dlg.DoModal() == IDCANCEL)
	{
		g_list_stages->m_mouse_in_item = -1;
		return false;
	}
	g_list_stages->m_mouse_in_item = -1;

	calculation->m_value_column[STAGES_COLUMN_PRESET] = calculation->m_preset_value;
	calculation->m_value_column[STAGES_COLUMN_DELTA] = calculation->m_value_column[STAGES_COLUMN_VALUE] - calculation->m_value_column[STAGES_COLUMN_PRESET];
	calculation->m_corresponding_param_num = -1; 
	calculation->CorrespondingParamNum();
	if (calculation->m_corresponding_param_num >= STAGES_START_ADDITIONAL_PARAM_NUM)
	{
		if (prev_param_num >= STAGES_START_ADDITIONAL_PARAM_NUM)
		{
			calculation->m_corresponding_param_num = prev_param_num; 
			CParam *param = m_MapParamCM_additional_only[calculation->m_corresponding_param_num];
			SetupParamForCalculation(param, calculation);
		}
		else
		{
			CreateParamForOneCalculation(calculation);
			if (stage->m_stage_num == g_selected_stage_num)
				STAGES_UpdateMapParam();
		}
	}
	else
	if (prev_param_num >= STAGES_START_ADDITIONAL_PARAM_NUM)
	{
		DeleteOneParam(stage, prev_param_num);
		if (stage->m_stage_num == g_selected_stage_num)
			STAGES_UpdateMapParam();
	}
		return true;
}

void RebuildStagesLists()
{
	if (g_list_stages == NULL)
		return;
	g_list_stages->Rebuild();
	g_list_selected_stage->Rebuild();
}

bool MoveUp(STAGES_Stage* stage, STAGES_Calculation* calculation)
{
	if (calculation == NULL)
	{
		if (g_stages.size() < 2)
			return false;

		STAGES_Stages::iterator current_stage, other_stage;
		int new_sel_stage;

		current_stage = g_stages.begin();
		other_stage = current_stage;
		current_stage++;
		while (current_stage != g_stages.end())
		{
			if (*current_stage == stage)
			{
				new_sel_stage = g_selected_stage_num;
				if (g_selected_stage_num == (*other_stage)->m_stage_num)
					new_sel_stage++;
				else
					if (g_selected_stage_num == (*current_stage)->m_stage_num)
						new_sel_stage--;
				(*other_stage)->m_stage_num++;
				(*current_stage)->m_stage_num--;
				g_stages.insert(other_stage, *current_stage);
				g_stages.erase(current_stage);
				g_cement_view->SwapLists((*other_stage)->m_stage_num-1, (*other_stage)->m_stage_num-2);
				STAGES_SetSelectedStage(new_sel_stage);
				return true;
			}
			current_stage++;
			other_stage++;
		}
	}
	else
	{
		if (stage->m_calculations.size() < 2)
			return false;
		
		STAGES_Calculations::iterator current_calculation, other_calculation;

		current_calculation = stage->m_calculations.begin();
		other_calculation = current_calculation;
		current_calculation++;
		while (current_calculation != stage->m_calculations.end())
		{
			if (*current_calculation == calculation)
			{
				stage->m_calculations.insert(other_calculation, *current_calculation);
				stage->m_calculations.erase(current_calculation);
				return true;
			}
			current_calculation++;
			other_calculation++;
		}
	}
	return false;
}

bool MoveDown(STAGES_Stage* stage, STAGES_Calculation* calculation)
{
	if (calculation == NULL)
	{
		if (g_stages.size() < 2)
			return false;

		STAGES_Stages::iterator current_stage, other_stage;
		int new_sel_stage;

		current_stage = g_stages.begin();
		other_stage = current_stage;
		other_stage++;
		while (other_stage != g_stages.end())
		{
			if (*current_stage == stage)
			{
				new_sel_stage = g_selected_stage_num;
				if (g_selected_stage_num == (*other_stage)->m_stage_num)
					new_sel_stage--;
				else
					if (g_selected_stage_num == (*current_stage)->m_stage_num)
						new_sel_stage++;
				(*other_stage)->m_stage_num--;
				(*current_stage)->m_stage_num++;
				g_stages.insert(current_stage, *other_stage);
				g_stages.erase(other_stage);
				g_cement_view->SwapLists((*current_stage)->m_stage_num-1, (*current_stage)->m_stage_num-2);
				STAGES_SetSelectedStage(new_sel_stage);
				return true;
			}
			current_stage++;
			other_stage++;
		}
	}
	else
	{
		if (stage->m_calculations.size() < 2)
			return false;
		
		STAGES_Calculations::iterator current_calculation, other_calculation;
		
		current_calculation = stage->m_calculations.begin();
		other_calculation = current_calculation;
		other_calculation++;
		while (other_calculation != stage->m_calculations.end())
		{
			if (*current_calculation == calculation)
			{
				stage->m_calculations.insert(current_calculation, *other_calculation);
				stage->m_calculations.erase(other_calculation);
				return true;
			}
			current_calculation++;
			other_calculation++;
		}
	}
	return false;
}

void ShowMenu(STAGES_Stage* stage, STAGES_Calculation* calculation, int x, int y)
{
	g_stages_menu.Clear();
	g_stages_menu.m_stage = stage;
	g_stages_menu.m_calculation = calculation;
	if (stage == NULL)
	{
		g_stages_menu.Add("Удалить все этапы", ID_DELETE);
		g_stages_menu.Add("Добавить этап", ID_ADD);
	}
	else
		if (calculation == NULL)
		{
			g_stages_menu.Add("Удалить этап", ID_DELETE);
			if (stage->m_stage_num > 1)
				g_stages_menu.Add("Переместить этап вверх", ID_UP);
			if (stage->m_stage_num < g_stages.size())
				g_stages_menu.Add("Переместить этап вниз", ID_DOWN);
			g_stages_menu.Add("Добавить вычисление", ID_ADD);
		}
		else
		{
			g_stages_menu.Add("Удалить вычисление", ID_DELETE);
			if (calculation != stage->m_calculations.front())
				g_stages_menu.Add("Переместить вычисление вверх", ID_UP);
			if (calculation != stage->m_calculations.back())
				g_stages_menu.Add("Переместить вычисление вниз", ID_DOWN);
			if (stage->m_stage_num == g_selected_stage_num)
				g_stages_menu.Add("Редактировать вычисление", ID_EDIT);
		}
		CRect list_rect;
		g_list_stages->GetWindowRect(&list_rect);
		CPoint pt(x, y);
		pt.Offset(list_rect.left, list_rect.top);
		g_stages_menu.Show(pt, -1, -1, g_list_stages);
}

bool CreateStagesFont(CFont** fnt, LOGFONT* lf)
{
	if (*fnt)
	{
		(*fnt)->DeleteObject();
		delete *fnt;
	}
	*fnt = new CFont();

	return (*fnt)->CreatePointFontIndirect(lf) == TRUE;
}

void CreateCalcHeaderFont(CFont** fnt, LOGFONT* lf)
{
	lf->lfItalic = !lf->lfItalic;
	lf->lfUnderline = !lf->lfUnderline;
	CreateStagesFont(fnt, lf);
	lf->lfItalic = !lf->lfItalic;
	lf->lfUnderline = !lf->lfUnderline;
}

void DeleteAllStages()
{
	STAGES_Stages::iterator current_stage = g_stages.begin();
	while (current_stage != g_stages.end())
	{
		delete *current_stage;
		current_stage++;
	}
	g_stages.clear();
}

struct FileCloser
{
	FileCloser(FILE*f): m_file(f)
	{}
	~FileCloser()
	{
		if (m_file)
			fclose(m_file);
	}
	FILE* m_file;
};

DWORD MixColors(DWORD one, DWORD two, float part_of_one)
{
	float part_two = 1-part_of_one;
	int r_one = one & 0xFF, r_two = two & 0xFF;
	int g_one = (one & 0xFF00)>>8, g_two = (two & 0xFF00)>>8;
	int b_one = (one & 0xFF0000)>>16, b_two = (two & 0xFF0000)>>16;
	return	(int(part_of_one * r_one + part_two * r_two) & 0xFF) |
		((int(part_of_one * g_one + part_two * g_two) & 0xFF)<<8) |
		((int(part_of_one * b_one + part_two * b_two) & 0xFF)<<16);
}

CString ModeName(STAGES_Mode mode)
{
	switch (mode)
	{
		case STAGES_MODE_EDIT: return "составление задания";
		case STAGES_MODE_EXECUTION: return "выполнение работы";
		case STAGES_MODE_ARCHIVE: return "обработка результатов";
		case STAGES_MODE_PAUSE: return "выполнение работы - пауза";
		case STAGES_MODE_BREAK: return "выполнение работы - перерыв";
	}
	return "ошибка";
}

void STAGES_SetCurrentMode(STAGES_Mode new_mode)
{
	g_current_mode = new_mode;
	g_title_stages.Format(g_title_stages_format, ModeName(new_mode));

	if (new_mode == STAGES_MODE_EDIT || new_mode == STAGES_MODE_ARCHIVE)
	{
		g_executing_stage_num = 0;
		g_executing_stage = NULL;
	}

	if (new_mode == STAGES_MODE_EDIT)
	{
        g_cement_view->HideAllGraphsInStages();
		STAGES_Stages::iterator current = g_stages.begin();
		while (current != g_stages.end())
		{
			(*current)->ResetCalculations();
			current++;
		}
	}
}

CString GetDurationString(STAGES_Stage* stage)
{
	COleDateTime start;
	start.m_dt = stage->m_duration;
	int hour = start.GetHour();
	int minute = start.GetMinute();
	int second = start.GetSecond();
	start.m_dt -= hour*1.0/24 + minute * 1.0/(24*60) + second * 1.0/(24*60*60);
	return TXT("%02d:%02d:%02d") << int(start.m_dt) * 24 + hour << minute << second;
}

void GetStageDurationFromString(STAGES_Stage* stage, CString str)
{
	int hour, minute, second, day = 0;
	if (sscanf(str.GetBuffer(0), "%d:%d:%d", &hour, &minute, &second) != 3)
	{
		g_list_stages->MessageBox("Часы:ММ:СС", g_title_wrong_format, MB_OK);
		return;
	}
	if (hour < 0)
	{
		g_list_stages->MessageBox("часы", g_title_wrong_format, MB_OK);
		return;
	}
	if (hour > 23)
	{
		day = hour / 24;
		hour -= day * 24;
	}
	if (minute < 0 || minute > 59)
	{
		g_list_stages->MessageBox("минуты", g_title_wrong_format, MB_OK);
		return;
	}
	if (second < 0 || second > 59)
	{
		g_list_stages->MessageBox("секунды", g_title_wrong_format, MB_OK);
		return;
	}
	COleDateTime duration;
	if (duration.SetTime(hour, minute, second) != 0)
	{
		g_list_stages->MessageBox("SetTime", " Ошибка", MB_OK);
		return;	
	}
	stage->m_duration = duration.m_dt + day;
}

CString GetStartString(STAGES_Stage* stage)
{
	COleDateTime start;
	start.m_dt = stage->m_start_time;
	if (stage->m_start_time == 0)
		return "Не начат";
	int year = start.GetYear();
	int month = start.GetMonth();
	int day = start.GetDay();
	int hour = start.GetHour();
	int minute = start.GetMinute();
	int second = start.GetSecond();
	return TXT("%d.%02d.%02d %02d:%02d:%02d") << year << month << day << hour << minute << second;
}

void GetStageStartFromString(STAGES_Stage* stage, CString str)
{
	int year, month, day, hour, minute, second;
	if (sscanf(str.GetBuffer(0), "%d.%d.%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) != 6)
	{
		g_list_stages->MessageBox("ГГ.ММ.ДД ЧЧ:ММ:СС", g_title_wrong_format, MB_OK);
		return;
	}
	if (year < 0)
	{
		g_list_stages->MessageBox("год", g_title_wrong_format, MB_OK);
		return;
	}
	if (month < 0 || month > 11)
	{
		g_list_stages->MessageBox("месяц", g_title_wrong_format, MB_OK);
		return;
	}
	if (day < 0 || day > 31)
	{
		g_list_stages->MessageBox("день", g_title_wrong_format, MB_OK);
		return;
	}
	if (hour < 0 || hour > 23)
	{
		g_list_stages->MessageBox("часы", g_title_wrong_format, MB_OK);
		return;
	}
	if (minute < 0 || minute > 59)
	{
		g_list_stages->MessageBox("минуты", g_title_wrong_format, MB_OK);
		return;
	}
	if (second < 0 || second > 59)
	{
		g_list_stages->MessageBox("секунды", g_title_wrong_format, MB_OK);
		return;
	}
	COleDateTime start;
	if (start.SetDateTime(year, month, day, hour, minute, second) != 0)
	{
		g_list_stages->MessageBox("SetDateTime", " Ошибка", MB_OK);
		return;	
	}
	stage->m_start_time = start.m_dt;
}

void CheckTotalStatusForHeader()
{
	if (g_stages.size() > 0)
	{
		bool common_status = g_stages.front()->m_collapsed_in_list;
		STAGES_Stages::iterator current_stage = g_stages.begin();
		current_stage++;
		while (current_stage != g_stages.end())
		{
			if ((*current_stage)->m_collapsed_in_list != common_status)
				break;
			current_stage++;
		}
		if (current_stage == g_stages.end()) 
			g_header_icon_down = common_status;
	}
}

void RebuildParamsAndMapInfo()
{
	g_cement_view->InitDataView(g_stages.size(), 3, 0);
	g_cement_view->InitViews(&m_wndDialogBarCM, &m_MapParamCM);
}

void DeleteAdditionalParams()
{
	CParam *param;
	int key;
	POSITION pos = m_MapParamCM_additional_only.GetStartPosition();
	while (pos != NULL)
	{
		m_MapParamCM_additional_only.GetNextAssoc(pos, key, param);
		delete param;
	}
	m_MapParamCM_additional_only.RemoveAll();
}

STAGES_StageTemplate* GetStageTemplate(int stage_template_num)
{
	STAGES_StageTemplates::iterator desired = STAGES_stage_templates.find(stage_template_num);
	if (desired == STAGES_stage_templates.end())
		return NULL;
	return desired->second;
}

CString GetNameOfTemplate(int template_num)
{
	STAGES_StageTemplate* desired = GetStageTemplate(template_num);
	if (desired == NULL)
		return "ERROR (NULL)";
	return desired->m_name;
}
STAGES_CalculationTemplate::STAGES_CalculationTemplate():
	m_formula(NULL), m_name("- имя -"),
	m_preset_value(0),
	m_has_graph(false), m_auto_graph_mode(10),
	m_graph_color(0x000000), m_graph_line_width(1), m_graph_min_val(-100), m_graph_max_val(100),
	m_corresponding_param_num(-1),
	m_db_time_interval(10.0/(24*60*60)), m_db_data_interval(1)
{
	memset(m_show_column, 1, sizeof(m_show_column));
}

STAGES_CalculationTemplate::~STAGES_CalculationTemplate()
{
	if (m_formula)
		delete m_formula;
}

void STAGES_CalculationTemplate::Save(FILE*f, bool info_for_template_only)
{
	fputs(CString(TXT("NAM %s\n") << m_name), f);
	fputs(CString(TXT("FRM %s\n") << (m_formula == NULL ? "NULL" : m_formula->GetTextForSave())), f);
	DWORD show_mask = 0;
	if (m_has_graph)
		show_mask |= 1 << 0;
	for (int i = 0; i < 6; i++)
		if (m_show_column[i])
			show_mask |= 1 << (i+1);
	fputs(CString(TXT("INF %X %f %d\n") << show_mask << m_preset_value << (info_for_template_only?-1:m_corresponding_param_num)), f);
	//if (info_for_template_only)
	fputs(CString(TXT("GRA %d %06X %d %lf %lf\n") << m_auto_graph_mode << m_graph_color << m_graph_line_width << m_graph_min_val << m_graph_max_val), f);
	fputs(CString(TXT("DB %lf %lf\n") << (m_db_time_interval * (24*60*60)) << m_db_data_interval), f);
}

bool STAGES_CalculationTemplate::LoadTemplateData(FILE*f, bool info_for_template_only)
{
	if (!fgets(g_file_str, FILE_STR_LEN, f) || memcmp(g_file_str, "NAM ", 4) != 0)
		return false;
	m_name = g_file_str + 4;
	m_name = m_name.Left(m_name.GetLength() - 1);
	if (!fgets(g_file_str, FILE_STR_LEN, f) || memcmp(g_file_str, "FRM ", 4) != 0)
		return false;
	m_formula = FORMULA_Formula::CreateFromText(g_file_str + 4);
	if (m_formula == NULL)
		return false;
	if (g_stages_file_version >= 6 || g_stages_templates_file_version >= 2)
	{
		DWORD auto_graph_mode, show_mask;
		if (!fgets(g_file_str, FILE_STR_LEN, f) ||
			sscanf(g_file_str, "INF %X %f %d", &show_mask, &m_preset_value, &m_corresponding_param_num) < ((info_for_template_only || g_stages_file_version < 8)?2:3))
			return false;
		for (int i = 0; i < 6; i++)
			m_show_column[i] = (show_mask & (1 << (i+1))) != 0;
		if (g_stages_file_version >= 8 || info_for_template_only)
		{
			if (!fgets(g_file_str, FILE_STR_LEN, f) ||
				sscanf(g_file_str, "GRA %d %X %d %lf %lf", &auto_graph_mode, &m_graph_color, &m_graph_line_width, &m_graph_min_val, &m_graph_max_val) != 5)
				return false;
			m_auto_graph_mode = auto_graph_mode;
			m_has_graph = (show_mask & (1 << 0)) != 0;
			if (!m_has_graph)
				m_auto_graph_mode = 10;
			if (g_stages_file_version >= 10 || g_stages_templates_file_version >= 2)
			{
				if (!fgets(g_file_str, FILE_STR_LEN, f) ||
					sscanf(g_file_str, "DB %lf %lf", &m_db_time_interval, &m_db_data_interval) != 2)
					return false;
				if (g_stages_file_version >= 11 || g_stages_templates_file_version >= 3)
					m_db_time_interval /= (24*60*60);
			}
			else
			{
				m_db_time_interval = 1.0/(24*60*60);
				m_db_data_interval = 1.0;
			}
		}
	}
	return true;
}

STAGES_CalculationTemplate* STAGES_CalculationTemplate::Load(FILE*f)
{
	STAGES_CalculationTemplate* res = new STAGES_CalculationTemplate();
	if (!res->LoadTemplateData(f, true))
	{
		delete res;
		res = NULL;
	}
	return res;
}

int STAGES_CalculationTemplate::CorrespondingParamNum()
{
	if (m_corresponding_param_num != -1)
		return m_corresponding_param_num;

	if (!m_has_graph)
	{
		m_corresponding_param_num = -1;
		return m_corresponding_param_num;
	}
	m_corresponding_param_num = m_formula->CorrespondingParamNum();
	if (m_corresponding_param_num == -1) 
	{
		CParam *param;
		m_corresponding_param_num = STAGES_START_ADDITIONAL_PARAM_NUM;
		while (true)
		{
			if (!m_MapParamCM_additional_only.Lookup(m_corresponding_param_num, param))
				break;
			m_corresponding_param_num++;
		}
	}
	return m_corresponding_param_num;
}
STAGES_Calculation::STAGES_Calculation(STAGES_CalculationTemplate *calc_template):
	STAGES_CalculationTemplate()
{
	if (calc_template)
	{
		m_name = calc_template->m_name;
		m_formula = calc_template->m_formula->MakeCopy();

		m_has_graph = calc_template->m_has_graph;
		m_auto_graph_mode = calc_template->m_auto_graph_mode;
		m_graph_color = calc_template->m_graph_color;
		m_graph_line_width = calc_template->m_graph_line_width;
		m_graph_min_val = calc_template->m_graph_min_val;
		m_db_data_interval = calc_template->m_db_data_interval;
		m_db_time_interval = calc_template->m_db_time_interval;

		memcpy(m_show_column, calc_template->m_show_column, sizeof(calc_template->m_show_column));
		m_value_column[STAGES_COLUMN_PRESET] = calc_template->m_preset_value;
	}
	else
		m_value_column[STAGES_COLUMN_PRESET] = 0;
	Reset();
}

STAGES_Calculation::~STAGES_Calculation()
{
}

void STAGES_Calculation::Reset()
{
	if (m_formula)
		m_formula->Reset();
	if (m_has_graph)
	{
		CParam *param;
		if (m_MapParamCM_additional_only.Lookup(m_corresponding_param_num, param))
			param->m_formula->Reset();
	}
	m_value_column[STAGES_COLUMN_VALUE] = 0;
	m_value_column[STAGES_COLUMN_DELTA] = -m_value_column[STAGES_COLUMN_PRESET];
	m_value_column[STAGES_COLUMN_MIN] = (float)INT_MAX;
	m_value_column[STAGES_COLUMN_MAX] = (float)INT_MIN;
	m_value_column[STAGES_COLUMN_MID] = 0;
	m_mid_counter = 0;
	m_status = FORMULA_CALC_STATUS_NOT_READY;
	m_was_ok = false;
}

void STAGES_Calculation::Calculate()
{
	if (m_formula == NULL)
		return;
	m_status = FORMULA_CALC_STATUS_NOT_READY;
	float val = m_formula->Calculate(m_status);
	if (m_status == FORMULA_CALC_STATUS_COMPLETE)
	{
		m_value_column[STAGES_COLUMN_VALUE] = val;
		if (m_value_column[STAGES_COLUMN_VALUE] < m_value_column[STAGES_COLUMN_MIN])
			m_value_column[STAGES_COLUMN_MIN] = m_value_column[STAGES_COLUMN_VALUE];
		if (m_value_column[STAGES_COLUMN_VALUE] > m_value_column[STAGES_COLUMN_MAX])
			m_value_column[STAGES_COLUMN_MAX] = m_value_column[STAGES_COLUMN_VALUE];

		double k = 1.0/(m_mid_counter + 1);
		m_value_column[STAGES_COLUMN_MID] = (m_value_column[STAGES_COLUMN_MID] * k * m_mid_counter) + (m_value_column[STAGES_COLUMN_VALUE] * k);
		m_mid_counter++;

		m_value_column[STAGES_COLUMN_DELTA] = m_value_column[STAGES_COLUMN_VALUE] - m_value_column[STAGES_COLUMN_PRESET];
		m_was_ok = true;
	}
}

void STAGES_Calculation::Save(FILE*f)
{
	m_preset_value = m_value_column[STAGES_COLUMN_PRESET];
	STAGES_CalculationTemplate::Save(f, false);
	fputs(CString(TXT("VAL %f (%f %f %f %d) %d %d\n")
		<< m_value_column[STAGES_COLUMN_VALUE]
		<< m_value_column[STAGES_COLUMN_MIN] << m_value_column[STAGES_COLUMN_MAX] << m_value_column[STAGES_COLUMN_MID] << m_mid_counter
		<< m_status << (m_was_ok?1:0)), f);
}

STAGES_Calculation* STAGES_Calculation::Load(FILE*f)
{
	STAGES_Calculation* res = new STAGES_Calculation();
	if (!res->LoadTemplateData(f, g_stages_file_version < 7))
	{
		delete res;
		return NULL;
	}
	int was_ok;
	if (!fgets(g_file_str, FILE_STR_LEN, f) ||
		sscanf(g_file_str, "VAL %f (%f %f %f %d) %d %d",
				&res->m_value_column[STAGES_COLUMN_VALUE],
				&res->m_value_column[STAGES_COLUMN_MIN], &res->m_value_column[STAGES_COLUMN_MAX], &res->m_value_column[STAGES_COLUMN_MID], &res->m_mid_counter,
				&res->m_status, &was_ok) != 7)
	{
		delete res;
		return NULL;
	}
	res->m_was_ok = (was_ok != 0);
	res->m_value_column[STAGES_COLUMN_PRESET] = res->m_preset_value;
	res->m_value_column[STAGES_COLUMN_DELTA] = res->m_value_column[STAGES_COLUMN_VALUE] - res->m_value_column[STAGES_COLUMN_PRESET];
	return res;
}
STAGES_Stage::STAGES_Stage(STAGES_StageTemplate* stage_template):
	m_collapsed_in_list(false), m_stage_num(0), m_start_time(0), m_duration(0)
{
	if (stage_template != NULL)
	{
		m_name = stage_template->m_name;
		STAGES_Calculation *sc;
		STAGES_CalculationTemplates::iterator current_calc_template = stage_template->m_calculation_templates.begin();
		while (current_calc_template != stage_template->m_calculation_templates.end())
		{
			sc = new STAGES_Calculation(*current_calc_template);
			m_calculations.push_back(sc);
			current_calc_template++;
		}
	}
}

STAGES_Stage::~STAGES_Stage()
{
	STAGES_Calculations::iterator current = m_calculations.begin();
	while (current != m_calculations.end())
	{
		delete *current;
		current++;
	}
}

void STAGES_Stage::ResetCalculations()
{
	STAGES_Calculations::iterator current_calculation = m_calculations.begin();
	while (current_calculation != m_calculations.end())
	{
		(*current_calculation)->Reset();
		current_calculation++;
	}
	m_start_time = 0;
	m_duration = 0;
}

void STAGES_Stage::Calculate()
{
	STAGES_Calculation* calculation;
	STAGES_Calculations::iterator current = m_calculations.begin();
	while (current != m_calculations.end())
	{
		calculation = (*current);
		calculation->Calculate();
		current++;
	}
}

void STAGES_Stage::Save(FILE*f)
{
	fputs(CString(TXT("NAM %s\n") << m_name), f);
	fputs(CString(TXT("NUM %d\n") << m_stage_num), f);
	fputs(CString(TXT("BEG %lf\n") << m_start_time), f);
	fputs(CString(TXT("DUR %lf\n") << m_duration), f);
	fputs(CString(TXT("N_C %d\n") << m_calculations.size()), f);
	STAGES_Calculations::iterator current = m_calculations.begin();
	while (current != m_calculations.end())
	{
		(*current)->Save(f);
		current++;
	}
	ParamOfView *pList = g_cement_view->m_parOfView->GetList(m_stage_num - 1);
	PanelDescription panel_descr;
	CString line;
	int show_mask = 0;
	for (int i = 0; i < 3; i++)
	{
		line.Format("GP%d ", i+1);
		if (pList->GetInfoPanel(i, &panel_descr))
		{
			line += panel_descr.GetTextForSave();
		}
		line += '\n';
		fputs(line, f);

		if (pList->m_active_grf[i])
			show_mask |= 1<<i;
	}
	line.Format("SDO %f %f %lf %lf %d %d\n", pList->gTimeScale, pList->gGlubScale, pList->gDeltaTime, pList->gDeltaH, pList->m_flag_orient, show_mask);
	fputs(line, f);
}
void AdjustAdditionalParamsDB()
{
	int mKey;
	CParam *db_param, *cement_param;
	POSITION pos = m_MapParamDB.GetStartPosition();
	while (pos != NULL)
	{
		m_MapParamDB.GetNextAssoc(pos, mKey, db_param);
		if (db_param == NULL)
		{
			m_MapParamDB.RemoveKey(mKey);
			continue;
		}
		if (mKey < STAGES_START_ADDITIONAL_PARAM_NUM)
			continue;
		if (m_MapParamCM_additional_only.Lookup(mKey, cement_param))
		{
			if (cement_param == NULL)
			{
				KRS_app.SendMessageToView(t_archive, PM_DELETE_EXT_PARAM, mKey, 0);
				KRS_app.SendMessageToView(t_cement_rt, PM_DELETE_EXT_PARAM, mKey, 0);
				m_MapParamCM_additional_only.RemoveKey(mKey);
			}
			else
			{
				db_param->m_color = cement_param->m_color;
				db_param->m_line_width = cement_param->m_line_width;
				db_param->sGraphName = cement_param->sGraphName;
				db_param->sName = cement_param->sName;
			}
		}
		else
		{
			KRS_app.SendMessageToView(t_archive, PM_DELETE_EXT_PARAM, mKey, 0);
			KRS_app.SendMessageToView(t_cement_rt, PM_DELETE_EXT_PARAM, mKey, 0);
		}
	}
}

STAGES_Stage* STAGES_Stage::Load(FILE*f, byte mode)
{
	int num, num_calc;
	double beg, dur;
	CString name;
	if (g_stages_file_version >= 2)
	{
		if (!fgets(g_file_str, FILE_STR_LEN, f) || memcmp(g_file_str, "NAM ", 4) != 0)
			return NULL;
		name = g_file_str + 4;
		name = name.Left(name.GetLength() - 1);
	}
	if (!fgets(g_file_str, FILE_STR_LEN, f) || sscanf(g_file_str, "NUM %d", &num) != 1)
		return NULL;
	if (!fgets(g_file_str, FILE_STR_LEN, f) || sscanf(g_file_str, "BEG %lf", &beg) != 1)
		return NULL;
	if (!fgets(g_file_str, FILE_STR_LEN, f) || sscanf(g_file_str, "DUR %lf", &dur) != 1)
		return NULL;
	if (!fgets(g_file_str, FILE_STR_LEN, f) || sscanf(g_file_str, "N_C %d", &num_calc) != 1)
		return NULL;
	STAGES_Stage* res = new STAGES_Stage(NULL);
	res->m_name = name;
	res->m_stage_num = num;
	res->m_start_time = beg;
	res->m_duration = dur;
	res->m_collapsed_in_list = false; 
	STAGES_Calculation* calc;
	while (num_calc > 0)
	{
		num_calc--;
		calc = STAGES_Calculation::Load(f);
		if (calc == NULL)
		{
			delete res;
			return NULL;
		}
		res->m_calculations.push_back(calc);
	}
	g_stages.push_back(res);
	CreateParamsForOneStage(res);
	STAGES_UpdateMapParam(res->m_stage_num);

	if (mode == STAGES_MODE_ARCHIVE)
		AdjustAdditionalParamsDB();
	if (g_stages_file_version >= 7)
	{
		int num_list = res->m_stage_num - 1;
		ParamOfView *pList = g_cement_view->m_parOfView->GetList(num_list);
		PanelDescription panel_descr;
		CString line;
		int i;
		for (i = 0; i < 3; i++)
		{
			if (!fgets(g_file_str, FILE_STR_LEN, f) || sscanf(g_file_str, "GP%d ", &num) != 1)
				return NULL;
			if (!panel_descr.FillFromText(g_file_str + 4))
			{
				delete res;
				return NULL;
			}
			pList->SetupPanel(i, &panel_descr);
		}
		if (g_stages_file_version >= 9)
		{
			float s1, s2;
			double d1, d2;
			int o, show_mask = 1;
			if (!fgets(g_file_str, FILE_STR_LEN, f) || sscanf(g_file_str, "SDO %f %f %lf %lf %d %d", &s1, &s2, &d1, &d2, &o, &show_mask) < 5)
				return NULL;
			pList->gTimeScale = s1;
			pList->gGlubScale = s2;
			pList->gDeltaTime = d1;
			pList->gDeltaH = d2;
			pList->m_flag_orient = o;
			double start_time = (res->m_start_time != 0) ? res->m_start_time : COleDateTime::GetCurrentTime().m_dt;
			g_cement_view->SetStartX(/*g_cement_view->GetStartX(num_list)*/start_time, num_list, true);
			pList->SetStyleGrfInstant(GRF_ORIENT_FLD, pList->m_flag_orient == FALSE);
			pList->SetStyleGrfInstant(GRF_Y_SCROLL, mode != STAGES_MODE_ARCHIVE);

			pList->ShowActiveList(false);
			for (i = 0; i < 3; i++)
				pList->m_active_grf[i] = show_mask & (1<<i);
			pList->SetVisibilityFlagAll();
		}
	}
	return res;
}
STAGES_StageTemplate::STAGES_StageTemplate():
	m_stage_template_num(0)
{
}

STAGES_StageTemplate::STAGES_StageTemplate(STAGES_StageTemplate* etalon)
{
	if (etalon != NULL)
	{
		m_name = etalon->m_name + " (копия)";
		STAGES_CalculationTemplate *sc;
		STAGES_CalculationTemplates::iterator current_calc_template = etalon->m_calculation_templates.begin();
		while (current_calc_template != etalon->m_calculation_templates.end())
		{
			sc = new STAGES_CalculationTemplate();
			sc->m_name = (*current_calc_template)->m_name;
			sc->m_formula = (*current_calc_template)->m_formula->MakeCopy();
			m_calculation_templates.push_back(sc);
			current_calc_template++;
		}
	}
}

STAGES_StageTemplate::~STAGES_StageTemplate()
{
	STAGES_CalculationTemplates::iterator current = m_calculation_templates.begin();
	while (current != m_calculation_templates.end())
	{
		delete *current;
		current++;
	}
	m_calculation_templates.clear();
}

void STAGES_StageTemplate::Save(FILE*f)
{
	fputs(CString(TXT("NAM %s\n") << m_name), f);
	fputs(CString(TXT("NUM %d\n") << m_stage_template_num), f);
	fputs(CString(TXT("N_C %d\n") << m_calculation_templates.size()), f);
	STAGES_CalculationTemplates::iterator current = m_calculation_templates.begin();
	while (current != m_calculation_templates.end())
	{
		(*current)->Save(f, true);
		current++;
	}
}

STAGES_StageTemplate* STAGES_StageTemplate::Load(FILE*f)
{
	int num, num_calc;
	CString name;

	if (!fgets(g_file_str, FILE_STR_LEN, f) || memcmp(g_file_str, "NAM ", 4) != 0)
		return NULL;
	name = g_file_str + 4;
	name = name.Left(name.GetLength() - 1);

	if (!fgets(g_file_str, FILE_STR_LEN, f) || sscanf(g_file_str, "NUM %d", &num) != 1)
		return NULL;
	if (!fgets(g_file_str, FILE_STR_LEN, f) || sscanf(g_file_str, "N_C %d", &num_calc) != 1)
		return NULL;
	STAGES_StageTemplate* res = new STAGES_StageTemplate();
	res->m_name = name;
	res->m_stage_template_num = num;
	STAGES_CalculationTemplate* calc_template;
	while (num_calc > 0)
	{
		num_calc--;
		calc_template = STAGES_CalculationTemplate::Load(f);
		if (calc_template == NULL)
		{
			delete res;
			return NULL;
		}
		res->m_calculation_templates.push_back(calc_template);
	}
	return res;
}
STAGES_StageInfoForSequence::STAGES_StageInfoForSequence(int template_num):
	m_stage_template_num(template_num)
{
	m_stage_name = GetNameOfTemplate(m_stage_template_num);
}
STAGES_TextInfo::STAGES_TextInfo(void* data, STAGES_DataType data_type, int x, DWORD text_color):
	m_x(x), m_text_color(text_color), m_w(-1),
	m_data(data), m_data_type(data_type), m_editable(false), m_right_alignment(false),
	m_shown(true), m_hidable(false),
	m_calculation(NULL), m_parent(NULL), m_disabled(false)
{
}

CString STAGES_TextInfo::GetText(bool for_edit)
{
	CString str;
	switch (m_data_type)
	{
		case STAGES_DATA_FLOAT:
			str = TXT("%.3f") << *((float*)m_data);
			if (m_calculation && (m_data != m_calculation->m_value_column + STAGES_COLUMN_PRESET) && !m_calculation->m_was_ok)
				str = "X.XXX";	
			return str;
		case STAGES_DATA_STRING:
				str = TXT("%s") << *((CString*)m_data);
				if (!for_edit && m_calculation && m_data == &m_calculation->m_name)
				{
					switch(m_calculation->m_status)
					{
						case FORMULA_CALC_STATUS_IN_PROGRESS: str += " (не готово)"; break;
						case FORMULA_CALC_STATUS_TIMEOUT: return str += " (ОТКЛ)"; break;
						case FORMULA_CALC_STATUS_CYCLE: return str += " (ЦИКЛ)"; break;
						case FORMULA_CALC_STATUS_NULL: return str += " (NULL)"; break;
					}
				}
				return str;
		case STAGES_DATA_START_TIME: return TXT("%s") << GetStartString((STAGES_Stage*)m_data);
		case STAGES_DATA_DURATION: return TXT("%s") << GetDurationString((STAGES_Stage*)m_data);
		default: return "ERROR GetText()";
	}
}

void STAGES_TextInfo::SetText(CString str)
{
	switch (m_data_type)
	{
		case STAGES_DATA_FLOAT:
			str.Replace(",", ".");
			float fl;
			if (sscanf(str, "%f", &fl) != 1)
				g_list_stages->MessageBox("Неправильный формат надписи", " Результат вычисления не будет изменён", MB_OK);
			else
			{
				*((float*)m_data) = fl;
				if (m_calculation && (m_data == m_calculation->m_value_column + STAGES_COLUMN_PRESET ||
										m_data == m_calculation->m_value_column + STAGES_COLUMN_VALUE))
					m_calculation->m_value_column[STAGES_COLUMN_DELTA] = m_calculation->m_value_column[STAGES_COLUMN_VALUE] - m_calculation->m_value_column[STAGES_COLUMN_PRESET];
			}
			break;
		case STAGES_DATA_STRING:
			*((CString*)m_data) = str;
			break;
		case STAGES_DATA_START_TIME:
			GetStageStartFromString((STAGES_Stage*)m_data, str);
			break;
		case STAGES_DATA_DURATION:
			GetStageDurationFromString((STAGES_Stage*)m_data, str);
			break;
	}
}

void STAGES_TextInfo::Draw(CDC* dc, int y, int w)
{		
	bool title_column = (m_data >= g_title_columns && m_data <= g_title_columns+6);
	CFont *prev_font = NULL;
	if (title_column)
	{
		bool selected = g_current_list == g_list_selected_stage;
		prev_font = dc->SelectObject(selected ? g_sel_stage_header_font : g_all_stages_header_font);
	}

	DWORD text_color = m_text_color;
	if (m_calculation != NULL && m_data != m_calculation->m_value_column + STAGES_COLUMN_PRESET && m_calculation->m_status != FORMULA_CALC_STATUS_COMPLETE && m_calculation->m_status != FORMULA_CALC_STATUS_NOT_READY)
		text_color = 0x0000FF;
	dc->SetTextColor(text_color);
	CString text = GetText();
	if (m_right_alignment)
		text = " " + text;

	CSize sz = dc->GetTextExtent(text);
	m_w = sz.cx;

	int text_x = m_x;
	int rect_x = m_x;
	int rect_w = m_w;
	
	if (text_x < 0)
	{
		int pos = (-text_x)/10;
		int col = (-text_x)%10;
		rect_w = ((w - 4) / col);
		text_x = 2 + rect_w * pos;
		//if (m_w > rect_w)
		if (text.GetLength() == 1)
			rect_w *= 0.75;
		else
			rect_w = m_w;
		rect_x = text_x;
	}
	if (m_right_alignment)
	{
		text_x -= m_w;
		rect_x -= rect_w;
	}
	
	CalculateWidth(dc, w);

	m_last_rect.left = rect_x;
	m_last_rect.right = rect_x + rect_w;
	m_last_rect.top = y;
	m_last_rect.bottom = y + sz.cy;
	
	if (m_shown)
		dc->TextOut(text_x, y, text);
	
	if (title_column)
		dc->SelectObject(prev_font);
}

int STAGES_TextInfo::CalculateWidth(CDC* dc, int w)
{
	if (dc->m_hDC == NULL)
		return 0;
	CSize sz = dc->GetTextExtent(GetText());
	m_w = sz.cx;
	if (m_x < 0)
	{
		int col = (-m_x)%10;
		sz.cx = (w - 4) / col;
		if (sz.cx > m_w)
			m_w = sz.cx;
		else
			m_w += 40;
	}
	return sz.cx;
}

void STAGES_TextInfo::OnClick(int x, int y)
{
	m_shown = !m_shown;
	for (int i = 0; i < 6; i++)
		if (m_data == m_calculation->m_value_column + i)
		{
			m_calculation->m_show_column[i] = m_shown;
			break;
		}
}
STAGES_Button::STAGES_Button(STAGES_Stage *stage, STAGES_Calculation* calculation, int button_id):
	STAGES_TextInfo(stage, STAGES_DATA_BUTTON, 0, 0xFFFFFF), m_button_id(button_id)
{
	m_calculation = calculation;
	m_editable = true;
}

void STAGES_Button::Draw(CDC* dc, int y, int w)
{
	CalculateWidth(dc, w);
	w += m_parent->m_buttons_num * (m_w + 1);

	m_disabled = false;

	STAGES_Stage* stage = (STAGES_Stage*)m_data;
	m_text_color = (stage == NULL || (m_parent->m_header_stage && stage->m_stage_num == g_selected_stage_num))?0xFFFFFF:0x444444;
	DWORD color = m_parent->m_bk_color;

	if (stage != NULL)
	{
		if (m_calculation == NULL)
		{
			if (stage->m_stage_num == 1 && m_button_id == ID_UP
				||
				stage->m_stage_num == g_stages.size() && m_button_id == ID_DOWN)
				m_disabled = true;
		}
		else
		{
			if (m_calculation == stage->m_calculations.front() && m_button_id == ID_UP
				||
				m_calculation == stage->m_calculations.back() && m_button_id == ID_DOWN)
				m_disabled = true;
		}
	}

	bool is_selected = !m_disabled && g_drawing_selected_text_info;
	
	m_last_rect.left = w - (m_w + 1)*m_button_num + 1;
	m_last_rect.right = m_last_rect.left + m_w;
	m_last_rect.top = y;
	m_last_rect.bottom = y + m_w;

	m_last_rect.InflateRect(-1, -1);
	CRect r = m_last_rect;
	r.InflateRect(-1, -1);
	dc->FillSolidRect(r, is_selected?color:m_text_color);
	r.InflateRect(-1, -1);
	dc->FillSolidRect(r, is_selected?m_text_color:color);

	if (m_disabled)
		color = MixColors(m_text_color, m_parent->m_bk_color, 0.5);
	else
	if (!is_selected)
		color = m_text_color;
			
	CPen pen(PS_SOLID, 1, color);
	CPen *prev_pen = dc->SelectObject(&pen);

	int x = r.left + 1;
	y = r.top + 1;
	int sz = m_w - 8;

	bool down = (stage == NULL && g_header_icon_down) || (stage != NULL && stage->m_collapsed_in_list);
	int start_y, dir;
	int count = 0;

	switch (m_button_id)
	{
		case ID_COLLAPSE:
			if (down)
			{
				dc->FillSolidRect(x, y, sz, 2, color);
				start_y = y + 3;
				dir = +1;
			}
			else
			{
				dc->FillSolidRect(x, y + sz - 2, sz, 2, color);
				start_y = y + sz - 4;
				dir = -1;
			}
			while (sz > 0)
			{
				dc->FillSolidRect(x + count, start_y, sz, 1, color);
				start_y += dir;
				count++;
				sz -=2;
			}
			break;
		case ID_MENU:
			dir = sz/2;
			while (count < sz)
			{
				//dc->FillSolidRect(x, y+count, dir + (sz - count)%dir, 1, color);
				dir = sz;
				if ((count/2 % 2) == 1)
					dir--;
				if ((count/2 % 4) == 1)
					dir--;
				dc->FillSolidRect(x, y+count, dir, 1, color);
				count+=2;
			}
			break;			
/*
		case ID_DELETE:
			dc->MoveTo(x, y);
			dc->LineTo(x + sz, y + sz);
			dc->MoveTo(x+1, y);
			dc->LineTo(x + sz, y + sz - 1);
			dc->MoveTo(x, y+1);
			dc->LineTo(x + sz - 1, y + sz);

			dc->MoveTo(x+sz-1, y);
			dc->LineTo(x-1, y + sz);
			dc->MoveTo(x+sz-2, y);
			dc->LineTo(x-1, y + sz - 1);
			dc->MoveTo(x+sz-1, y+1);
			dc->LineTo(x, y + sz);
			break;
		case ID_UP:
			start_y = y + sz/2 + sz/4;
			dir = -1;
			while (sz > 0)
			{
				dc->FillSolidRect(x + count, start_y, sz, 1, color);
				start_y += dir;
				count++;
				sz -=2;
			}
			break;
		case ID_DOWN:
			start_y = y + sz/2 - sz/4;
			dir = +1;
			while (sz > 0)
			{
				dc->FillSolidRect(x + count, start_y, sz, 1, color);
				start_y += dir;
				count++;
				sz -=2;
			}
			break;
		case ID_ADD:
			dc->FillSolidRect(x, y+sz/2-1, sz, 2, color);
			dc->FillSolidRect(x+sz/2-1, y, 2, sz, color);
			break;
		case ID_EDIT:
			dir = sz/2;
			while (count < sz)
			{
				dc->FillSolidRect(x, y+count, dir + (sz - count)%dir, 1, color);
				count+=2;
			}
			break;
*/			
	}
	dc->SelectObject(prev_pen);
}

int STAGES_Button::CalculateWidth(CDC* dc, int w)
{
	m_w = g_list_stages->m_h;
	return m_w;
}

void STAGES_Button::OnClick(int x, int y)
{
	int new_sel_stage;
	STAGES_Calculations::iterator current_calculation, other_calculation;
	STAGES_Stages::iterator current_stage, other_stage;
	STAGES_Stage* stage = (STAGES_Stage*)m_data;
	bool res = true;
	switch (m_button_id)
	{
		case ID_COLLAPSE:
			if (stage == NULL)
			{
				g_header_icon_down = !g_header_icon_down;
				current_stage = g_stages.begin();
				while (current_stage != g_stages.end())
				{
					(*current_stage)->m_collapsed_in_list = g_header_icon_down;
					current_stage++;
				}
			}
			else
			{
				stage->m_collapsed_in_list = !stage->m_collapsed_in_list;
				CheckTotalStatusForHeader();
			}
			g_list_stages->Rebuild();
			return;
		case ID_ADD:
			res = (stage == NULL) ? AddStage() : AddCalculation(stage);
			break;
		case ID_EDIT:
			res = EditCalculation(stage, m_calculation);
			break;
		case ID_MENU:
			//res = (m_calculation == NULL) ? DeleteStage(stage) : DeleteCalculation(stage, m_calculation);
			ShowMenu(stage, m_calculation, x, y);
			res = false;
			break;
		case ID_UP:
			if (m_calculation == NULL)
			{
				if (g_stages.size() < 2)
					return;
				current_stage = g_stages.begin();
				other_stage = current_stage;
				current_stage++;
				while (current_stage != g_stages.end())
				{
					if (*current_stage == stage)
					{
						new_sel_stage = g_selected_stage_num;
						if (g_selected_stage_num == (*other_stage)->m_stage_num)
							new_sel_stage++;
						else
						if (g_selected_stage_num == (*current_stage)->m_stage_num)
							new_sel_stage--;
						(*other_stage)->m_stage_num++;
						(*current_stage)->m_stage_num--;
						g_stages.insert(other_stage, *current_stage);
						g_stages.erase(current_stage);
						g_cement_view->SwapLists((*other_stage)->m_stage_num-1, (*other_stage)->m_stage_num-2);
						STAGES_SetSelectedStage(new_sel_stage);
						break;
					}
					current_stage++;
					other_stage++;
				}
			}
			else
			{
				if (stage->m_calculations.size() < 2)
					return;
				current_calculation = stage->m_calculations.begin();
				other_calculation = current_calculation;
				current_calculation++;
				while (current_calculation != stage->m_calculations.end())
				{
					if (*current_calculation == m_calculation)
					{
						stage->m_calculations.insert(other_calculation, *current_calculation);
						stage->m_calculations.erase(current_calculation);
						break;
					}
					current_calculation++;
					other_calculation++;
				}
			}
			break;
		case ID_DOWN:
			if (m_calculation == NULL)
			{
				if (g_stages.size() < 2)
					return;
				current_stage = g_stages.begin();
				other_stage = current_stage;
				other_stage++;
				while (other_stage != g_stages.end())
				{
					if (*current_stage == stage)
					{
						new_sel_stage = g_selected_stage_num;
						if (g_selected_stage_num == (*other_stage)->m_stage_num)
							new_sel_stage--;
						else
						if (g_selected_stage_num == (*current_stage)->m_stage_num)
							new_sel_stage++;
						(*other_stage)->m_stage_num--;
						(*current_stage)->m_stage_num++;
						g_stages.insert(current_stage, *other_stage);
						g_stages.erase(other_stage);
						g_cement_view->SwapLists((*current_stage)->m_stage_num-1, (*current_stage)->m_stage_num-2);
						STAGES_SetSelectedStage(new_sel_stage);
						break;
					}
					current_stage++;
					other_stage++;
				}
			}
			else
			{
				if (stage->m_calculations.size() < 2)
					return;
				current_calculation = stage->m_calculations.begin();
				other_calculation = current_calculation;
				other_calculation++;
				while (other_calculation != stage->m_calculations.end())
				{
					if (*current_calculation == m_calculation)
					{
						stage->m_calculations.insert(current_calculation, *other_calculation);
						stage->m_calculations.erase(other_calculation);
						break;
					}
					current_calculation++;
					other_calculation++;
				}
			}
			break;
	}
	if (res)
		RebuildStagesLists();
}
STAGES_LineInfo::STAGES_LineInfo(STAGES_Stage *stage, DWORD bk_color):
	m_stage(stage), m_bk_color(bk_color), m_last_line(false), m_buttons_num(0)
{
}

STAGES_LineInfo::~STAGES_LineInfo()
{
	list<STAGES_TextInfo*>::iterator current = m_elements.begin();
	while (current != m_elements.end())
	{
		delete *current;
		current++;
	}
}

void STAGES_LineInfo::AddTextInfo(STAGES_TextInfo* text_info)
{
	text_info->m_parent = this;
	m_elements.push_back(text_info);
}

void STAGES_LineInfo::AddButton(STAGES_Button* button)
{
	m_buttons_num++;
	button->m_button_num = m_buttons_num;
	button->m_parent = this;
	m_elements.push_back(button);
}
void RedrawPhaseFunction(CDC* dc, int i, int j, CRect* rect, STAGES_List* work_list)
{
	g_current_list = work_list;

	dc->SelectStockObject(NULL_BRUSH);
	dc->SetBkMode(OPAQUE);
	
	if (work_list->m_cdc.m_hDC == NULL)
		work_list->m_cdc.CreateCompatibleDC(dc);
	if (work_list->m_h == 0)
	{
		work_list->Rebuild();
		if (work_list->m_h == 0)
			return;
	}

	bool has_scroll_bar = work_list->m_column_width != work_list->m_list_width;
	
	if (i >= work_list->GetItemCount())
		return;
		
	DWORD data = work_list->GetItemData(i);
		
	int w = rect->Width(), h = rect->Height(), x = rect->left, y = rect->top;
	UINT line = data & 0xFFFF, stage_num = data >> 16;
	int top_index = work_list->GetTopIndex();
	bool header = (i == top_index);
	if (header)
	{
		line = 1;
		stage_num = 0;
		data = (stage_num << 16) | line;
	}
	bool is_selected_list = (work_list == g_list_selected_stage);
	bool is_executing_stage = (is_selected_list || (g_executing_stage_num == stage_num) || STAGES_IsFinished());
	bool is_selected_stage = is_selected_list || ((work_list == g_list_stages) && (g_selected_stage_num == stage_num));
	STAGES_ListInfo::iterator desired = work_list->m_list_info.find(data);
	if (desired == work_list->m_list_info.end())
	{
		dc->SetTextColor(0xFFFFFF);
		dc->SetBkColor(0x0000FF);
		dc->TextOut(x+2, y, CString(TXT("ERROR! (%06X)") << data));
		return;
	}
	STAGES_LineInfo* line_info = desired->second;
	if (i == work_list->m_mouse_in_item && !header && g_current_mode == STAGES_MODE_EDIT)
		dc->FillSolidRect(x, y, w, h, MixColors(0x000000, line_info->m_bk_color, 0.2f));
	else
		dc->FillSolidRect(x, y, w, h, line_info->m_bk_color);
	int ww = work_list->m_column_width;
	ww -= line_info->m_buttons_num * (work_list->m_h + 1) + 1;
	STAGES_TextInfo* text_info;
	list<STAGES_TextInfo*>::iterator current = line_info->m_elements.begin();
	while (current != line_info->m_elements.end())
	{
		text_info = *current;
		//dc->SetBkColor(line_info->m_bk_color);
		text_info->Draw(dc, y, ww);
		current++;
	}
	//work_list->SetToolTip("", i, 0);
	if (i == work_list->m_mouse_in_item)
	{
		current = line_info->m_elements.begin();
		while (current != line_info->m_elements.end())
		{
			text_info = *current;
			if (text_info->m_last_rect.PtInRect(work_list->m_mouse_point))
			{
				if (!text_info->m_disabled && (text_info->m_editable || text_info->m_data_type != STAGES_DATA_STRING))
				{
					g_selected_text_info = text_info;
					int xt = text_info->m_last_rect.left;
					int wt = text_info->m_last_rect.Width();
					DWORD col = text_info->m_text_color;
					DWORD bk_col = col;
					if (!text_info->m_editable)
						bk_col = MixColors(line_info->m_bk_color, bk_col, 0.5);
					text_info->m_text_color = line_info->m_bk_color;
					dc->FillSolidRect(text_info->m_last_rect, bk_col);
					g_drawing_selected_text_info = true;
					text_info->Draw(dc, y, ww);
					g_drawing_selected_text_info = false;
					text_info->m_text_color = col;
					dc->SetTextColor(text_info->m_text_color);
					//work_list->SetToolTip(text_info->m_tool_tip_info, i, 0);
					break;
				}
			}
			current++;
		}
		if (current == line_info->m_elements.end())
			g_selected_text_info = NULL;
	}
	if (!header && line == 0)
	{
		DWORD text_color = is_selected_stage?0xFFFFFF:0x000000;
		if ((g_current_mode == STAGES_MODE_EXECUTION && g_executing_stage_num == stage_num) ||
			(g_current_mode == STAGES_MODE_ARCHIVE && g_current_db_stage_num == stage_num))
		{
			DWORD arr_col;
			int hh = h-3-(h%2);
			int len = h*0.7 - 1, now_h;
			g_max_arrow_counter = hh/2+2;
			for (int zzz = 0; zzz < len; zzz++)
			{
				arr_col = MixColors(line_info->m_bk_color, text_color, fabs(zzz - 0.5*len)/(0.5*len));
				now_h = (zzz + g_max_arrow_counter - g_arrow_counter)%g_max_arrow_counter;
				if (hh - now_h*2 > 0)
					dc->FillSolidRect(x + 2 + zzz, y + 1 + now_h, 1, hh - now_h*2, arr_col);
			}
		}
		dc->SetBkMode(TRANSPARENT);
		dc->TextOut(x + 2 + h*0.7, y, CString(TXT("%d")<<stage_num));
	}
	if (header || ((line == 0 && line_info->m_stage->m_collapsed_in_list) || line_info->m_last_line))
		dc->FillSolidRect(x, y + h - 1, w, 1, header?0x000000:g_colors[5]); 
	if (has_scroll_bar)
		dc->FillSolidRect(x + w - 1, y, 1, h, 0); 
	
	int count_per_page = work_list->GetCountPerPage();
	if (i == top_index + count_per_page - 1 || i == work_list->GetItemCount()-1) 
	{
		CRect list_rect;
		work_list->GetClientRect(&list_rect);
		x = 0; y = 0; h = list_rect.bottom;
	}
}

void RedrawPhaseFunctionAll(CDC* dc, int i, int j, CRect* rect)
{
	RedrawPhaseFunction(dc, i, j, rect, g_list_stages);
}

void RedrawPhaseFunctionSelected(CDC* dc, int i, int j, CRect* rect)
{
	RedrawPhaseFunction(dc, i, j, rect, g_list_selected_stage);
}

void UpdateParamListInDB()
{
	if (m_MapParamCM_additional_only.GetCount() == 0)
		return;

	CParam *param;
	int num;
	POSITION pos = m_MapParamCM_additional_only.GetStartPosition();
	while (pos != NULL)
	{
		m_MapParamCM_additional_only.GetNextAssoc(pos, num, param);
		if (param->flDB != TRUE)
			DB_SendParamDescriptionToDB(param);
	}
	DB_connection_info.RequestParamList();
}
STAGES_List::STAGES_List():
	m_h(0)
{
	m_cdc.m_hDC = NULL;
}

STAGES_List::~STAGES_List()
{
	DeleteInfo();
}

BEGIN_MESSAGE_MAP(STAGES_List, CExtListCtrl)
//	ON_WM_SIZE()
	ON_MESSAGE(WM_MENU, OnMenu)
END_MESSAGE_MAP()

LRESULT STAGES_List::OnMenu(WPARAM data, LPARAM l)
{
	bool rebuild = false;
	bool add_delete = false;
	switch (data)
	{
		case ID_DELETE:
			if (g_stages_menu.m_stage == NULL)
				rebuild = DeleteAllStages1();
			else
				rebuild = (g_stages_menu.m_calculation == NULL) ? DeleteStage(g_stages_menu.m_stage) : DeleteCalculation(g_stages_menu.m_stage, g_stages_menu.m_calculation);
			break;
		case ID_ADD:
			rebuild = (g_stages_menu.m_stage == NULL) ? ::AddStage() : AddCalculation(g_stages_menu.m_stage);
			break;
		case ID_EDIT:
			/*rebuild = */EditCalculation(g_stages_menu.m_stage, g_stages_menu.m_calculation);
			break;
		case ID_UP:
			rebuild = MoveUp(g_stages_menu.m_stage, g_stages_menu.m_calculation);
			break;
		case ID_DOWN:
			rebuild = MoveDown(g_stages_menu.m_stage, g_stages_menu.m_calculation);
			break;
	}
	if (rebuild)
	{
		RebuildStagesLists();
		StagesSelectionChanged(true);
		ListSizeChanged();
	}
	return 0;
}

void STAGES_List::DeleteInfo()
{
	STAGES_ListInfo::iterator current = m_list_info.begin();
	while (current != m_list_info.end())
	{
		delete current->second;
		current++;
	}
	m_list_info.clear();
}

void STAGES_List::AddLine(STAGES_LineInfo* line_info, DWORD stage_num, DWORD line, bool add_data)
{
	line_info->m_header_stage = (line == 0);
	DWORD data = (stage_num << 16) | (line & 0xFFFF);
	if (add_data)
	{
		if (SetItemData(m_line_counter, data) == 0)
		{
			InsertItem(m_line_counter, "");
			SetItemData(m_line_counter, data);
		}
		SetRedrawFunction((this == g_list_stages)?RedrawPhaseFunctionAll:RedrawPhaseFunctionSelected, m_line_counter, 0);
		m_line_counter++;
	}
	m_list_info[data] = line_info;
}

void STAGES_List::AddLineInfoForCalculation(STAGES_LineInfo* line_info, STAGES_Calculation* calculation, bool selected)
{
	STAGES_TextInfo* text_info;
	bool is_selected_stage = (line_info->m_stage->m_stage_num == g_selected_stage_num);
	bool is_uneditable_stage =	(calculation != NULL && !calculation->m_was_ok) ||
								(
								g_current_mode == STAGES_MODE_EXECUTION
								&&
								(line_info->m_stage->m_stage_num >= g_executing_stage_num)
								);

	DWORD title_color = is_selected_stage?0x000000:0x333333;
	text_info = new STAGES_TextInfo((calculation == NULL) ? (&g_title_columns[0]) : (&calculation->m_name), STAGES_DATA_STRING, 2, title_color);
	text_info->m_tool_tip_info = g_title_columns[0];
	text_info->m_calculation = calculation;
	text_info->m_editable = (calculation != NULL/* && g_current_mode == STAGES_MODE_EDIT*/);
	line_info->AddTextInfo(text_info);

	int i, max_i = (selected) ? 1 : 6;
	//int col_w = (selected) ? ((m_column_width-4)/2) : ((m_column_width-4)/7);
	LPVOID ptr_array[6];
	bool show_column[6];
	for (i = 0; i < max_i; i++)
		if (calculation == NULL)
		{
			ptr_array[i] = &g_title_columns[i+1];
			show_column[i] = true;
		}
		else
		{
			ptr_array[i] = &calculation->m_value_column[i];
			show_column[i] = calculation->m_show_column[i];
		}
	STAGES_DataType data_type = (calculation == NULL) ? STAGES_DATA_STRING : STAGES_DATA_FLOAT;
	if (calculation != NULL)
		title_color = (is_selected_stage?0xCC0000:0xFF0000);
	for (i = 0; i < max_i; i++)
	{
		text_info = new STAGES_TextInfo(ptr_array[i], data_type, 2, title_color);
		text_info->m_tool_tip_info = g_title_columns[i+1];
		text_info->m_shown = show_column[i];
		text_info->m_x = (selected) ? -22 : (-(7 + (i+2)*10));
		text_info->m_right_alignment = true;
		text_info->m_hidable = (calculation != NULL);
		text_info->m_editable = (calculation != NULL) && ((i == 1) || !is_uneditable_stage) && (i != 2);
		text_info->m_calculation = calculation;
		line_info->AddTextInfo(text_info);
	}
}

void STAGES_List::AddStage(STAGES_Stage* stage)
{
	STAGES_LineInfo* line_info;
	STAGES_TextInfo* text_info;
	int line = 0;

	bool is_selected_stage = (stage->m_stage_num == g_selected_stage_num);
	bool is_selected_list = (this == g_list_selected_stage);
	bool selected_calcs = is_selected_stage && !is_selected_list;
	bool is_executable_stage = (stage->m_stage_num == g_executing_stage_num);

	DWORD text_color;
	line_info = new STAGES_LineInfo;
	line_info->m_stage = stage;
	line_info->m_bk_color = (is_selected_stage || is_selected_list)?0x448844:0xFFFFFF;
	text_color = (is_selected_stage || is_selected_list)?0xFFFFFF:0x000000;
	text_info = new STAGES_TextInfo(&stage->m_name, STAGES_DATA_STRING, 2*m_h, text_color);
	text_info->m_tool_tip_info = "Название этапа";
	text_info->m_editable = true;
	line_info->AddTextInfo(text_info);

	STAGES_Button* button;

	if (!is_selected_list && stage->m_start_time != 0)
	{
		text_info = new STAGES_TextInfo(&g_title_start_time, STAGES_DATA_STRING, -39/*36*/, text_color);
		text_info->m_tool_tip_info = "Начало этапа";
		text_info->m_right_alignment = true;
		line_info->AddTextInfo(text_info);
		
		text_info = new STAGES_TextInfo(stage, STAGES_DATA_START_TIME, -39/*36*/, text_color);
		text_info->m_tool_tip_info = "Начало этапа";
		text_info->m_editable = true;
		line_info->AddTextInfo(text_info);

		text_info = new STAGES_TextInfo(&g_title_duration, STAGES_DATA_STRING, -79/*56*/, text_color);
		text_info->m_tool_tip_info = "Длительность этапа";
		text_info->m_right_alignment = true;
		line_info->AddTextInfo(text_info);

		text_info = new STAGES_TextInfo(stage, STAGES_DATA_DURATION, -79/*56*/, text_color);
		text_info->m_tool_tip_info = "Длительность этапа";
		text_info->m_editable = !is_executable_stage;
		line_info->AddTextInfo(text_info);
	}

	if (!is_selected_list)
	{
		button = new STAGES_Button(stage, NULL, ID_COLLAPSE);
		button->m_tool_tip_info = "Свернуть/развернуть этап";
		line_info->AddButton(button);

		if (g_current_mode == STAGES_MODE_EDIT)
		{
			button = new STAGES_Button(stage, NULL, ID_MENU);
			button->m_tool_tip_info = "Меню этапа";
			line_info->AddButton(button);
		}
	}

	AddLine(line_info, stage->m_stage_num, line++, true);

	if (is_selected_list && stage->m_start_time != 0)
	{
		text_color = 0x000000;

		line_info = new STAGES_LineInfo;
		line_info->m_stage = stage;
		line_info->m_bk_color = ((line%2==0) ? 0xFFFFFF : 0xE8E8E8);

		text_info = new STAGES_TextInfo(&g_title_start_time, STAGES_DATA_STRING, m_h, text_color);
		text_info->m_tool_tip_info = "Начало этапа";
		text_info->m_right_alignment = false;
		line_info->AddTextInfo(text_info);

		text_info = new STAGES_TextInfo(stage, STAGES_DATA_START_TIME, -22, text_color);
		text_info->m_tool_tip_info = "Начало этапа";
		text_info->m_editable = true;
		text_info->m_right_alignment = true;
		line_info->AddTextInfo(text_info);

		AddLine(line_info, stage->m_stage_num, line++, true);

		line_info = new STAGES_LineInfo;
		line_info->m_stage = stage;
		line_info->m_bk_color = ((line%2==0) ? 0xFFFFFF : 0xE8E8E8);

		text_info = new STAGES_TextInfo(&g_title_duration, STAGES_DATA_STRING, m_h, text_color);
		text_info->m_tool_tip_info = "Длительность этапа";
		text_info->m_right_alignment = false;
		line_info->AddTextInfo(text_info);

		text_info = new STAGES_TextInfo(stage, STAGES_DATA_DURATION, -22, text_color);
		text_info->m_tool_tip_info = "Длительность этапа";
		text_info->m_editable = !is_executable_stage;
		text_info->m_right_alignment = true;
		line_info->AddTextInfo(text_info);
		
		AddLine(line_info, stage->m_stage_num, line++, true);
	}
	line_info = new STAGES_LineInfo;
	line_info->m_stage = stage;
	line_info->m_bk_color = selected_calcs ? ((line%2==1) ? 0xCCFFCC : 0xBBFFBB) : ((line%2==0) ? 0xFFFFFF : 0xE8E8E8);
	AddLineInfoForCalculation(line_info, NULL, is_selected_list);
	if (!is_selected_list && g_current_mode == STAGES_MODE_EDIT)
		line_info->m_buttons_num = 1;
	AddLine(line_info, stage->m_stage_num, line++, is_selected_list || !stage->m_collapsed_in_list);
	STAGES_Calculations::iterator current = stage->m_calculations.begin();
	while (current != stage->m_calculations.end())
	{
		line_info = new STAGES_LineInfo;
		line_info->m_stage = stage;
		line_info->m_bk_color = selected_calcs ? ((line%2==1) ? 0xCCFFCC : 0xBBFFBB) : ((line%2==0) ? 0xFFFFFF : 0xE8E8E8);

		AddLineInfoForCalculation(line_info, *current, is_selected_list);

		if (!is_selected_list && g_current_mode == STAGES_MODE_EDIT)
		{
			button = new STAGES_Button(stage, *current, ID_MENU);
			button->m_tool_tip_info = "Меню вычисления";
			line_info->AddButton(button);
		}
		
		AddLine(line_info, stage->m_stage_num, line++, is_selected_list || !stage->m_collapsed_in_list);

		current++;
	}
	line_info->m_last_line = true;
}

void STAGES_List::StartRebuild()
{
	if (m_h == 0)
	{
		CRect rect;
		if (GetItemRect(0, &rect, LVIR_BOUNDS))
		{
			m_h = rect.Height();
			m_column_width = rect.Width();
		}
	}

	DeleteInfo();
	m_line_counter = 0;

	bool is_selected_list = (this == g_list_selected_stage);

	STAGES_LineInfo* line_info = new STAGES_LineInfo;
	line_info->m_bk_color = g_colors[5];
	STAGES_TextInfo* text_info = new STAGES_TextInfo(is_selected_list?&g_title_selected_stage:&g_title_stages, STAGES_DATA_STRING, 2, 0xFFFFFF);
	line_info->AddTextInfo(text_info);

	if (!is_selected_list)
	{
		STAGES_Button* button;
		button = new STAGES_Button(NULL, NULL, ID_COLLAPSE);
		button->m_tool_tip_info = "Свернуть/развернуть все этапы";
		line_info->AddButton(button);
		
		if (g_current_mode == STAGES_MODE_EDIT)
		{
			button = new STAGES_Button(NULL, NULL, ID_MENU);
			button->m_tool_tip_info = "Меню";
			line_info->AddButton(button);
		}
	}

	AddLine(line_info, 0, 1, true); 
}

void STAGES_List::FinishRebuild()
{
	int count = GetItemCount();
	while (count > m_line_counter)
	{
		DeleteItem(m_line_counter);
		count--;
	}
	Invalidate(TRUE);
}

void STAGES_List::Rebuild()
{
	int prev_size = GetItemCount();
	int ensure_visible_line_counter = -1, ensure_visible_last_line_counter = -1, prev_bottom_index = GetTopIndex() + GetCountPerPage() - 1;
	m_h = 0;
	StartRebuild();
	bool is_selected_list = (this == g_list_selected_stage);
	STAGES_Stages::iterator current_stage = g_stages.begin();
	while (current_stage != g_stages.end())
	{
		if (is_selected_list)
		{
			if ((*current_stage)->m_stage_num == g_selected_stage_num)
			{
				g_list_selected_stage->AddStage(*current_stage);
				FinishRebuild();
				return;
			}
		}
		else
		{
			if ((*current_stage)->m_stage_num == g_selected_stage_num)
				ensure_visible_line_counter = g_list_stages->m_line_counter;
			g_list_stages->AddStage(*current_stage);
			if ((*current_stage)->m_stage_num == g_selected_stage_num)
				ensure_visible_last_line_counter = g_list_stages->m_line_counter;
		}
		current_stage++;
	}
	FinishRebuild();
	if (prev_size <= 1)
		EnsureVisible(0, FALSE);
	else
		EnsureVisible(prev_bottom_index, FALSE);
}

void STAGES_List::OnMouseMoveIn(CPoint point)
{
	m_mouse_point = point;
}

void STAGES_ChangeModeToEdit(CString work_path)
{
	CString path1 = work_path + "\\stages.cfg", path2 = path1 + "1";
	FILE* f1 = fopen(path1, "rt");
	if (f1 == NULL)
		return;
	FILE* f2 = fopen(path2, "wt");
	if (f2 == NULL)
	{
		fclose(f1);
		return;
	}

	int executing = 0, selected = 0, file_mode = 0;

	char zzz[1024];
	if (!fgets(zzz, 1024, f1) || sscanf(zzz, "VER %d", &g_stages_file_version) != 1)
	{
		fclose(f1);
		fclose(f2);
		remove(path2);
		return;
	}
	fputs(zzz, f2);
	if (g_stages_file_version >= 7)
	{
		if (!fgets(zzz, 1024, f1) || sscanf(zzz, "EXE %d %d %d", &executing, &selected, &file_mode) != 3)
			return;
		if (file_mode == 0)
		{
			fclose(f1);
			fclose(f2);
			remove(path2);
			return;
		}
		file_mode = 0;
		CString sss;
		sss.Format("EXE %d %d %d\n", executing, selected, file_mode);
		fputs(sss, f2);
	}
	while (!feof(f1))
	{
		if (!fgets(zzz, 1024, f1))
			break;
		fputs(zzz, f2);
	}
	fclose(f1);
	fclose(f2);
	remove(path1);
	rename(path2, path1);
}

STAGES_LoadResult STAGES_LoadStages(bool dialog, bool bak)
{
	g_stages_templates_file_version = -1;

	CString path = PRJ_GetWorkPath() + "\\stages.cfg";
	if (dialog)
	{
		path = STAGES_last_file_path;
		if (STAGES_GetCurrentMode() == STAGES_MODE_ARCHIVE &&
			g_list_stages->MessageBox("Загрузить шаблон задания?" + g_ensure_saved_text, g_title_request_action, MB_YESNO) != IDYES)
			return STAGES_LoadCancelled;
		CFileDialog dlg(TRUE, NULL, path, OFN_HIDEREADONLY, "Файлы конфигурации(*.cfg)|*.cfg||");
		if (dlg.DoModal() != IDOK)
			return STAGES_LoadCancelled;
		STAGES_last_file_path = path = dlg.GetPathName();
	}

//	g_additional_params_db_adjusting_counter = 0;

	g_selected_stage_num = 0;
	g_executing_stage = NULL;
	DeleteAllStages();
	RebuildStagesLists();

	int num = 0;
	char zzz[1024];
	STAGES_LoadResult res = STAGES_LoadFailed;
	int executing = 0, selected = 0, file_mode = STAGES_MODE_EDIT;

	if (bak)
		path += ".bak";

	FILE* f = fopen(path, "rt");
	if (f == NULL && !dialog && !bak)
	{
		f = fopen(path + ".bak", "rb");
		if (f)
		{
			fclose(f);
			CopyFile(path + ".bak", path + ".bak.used", FALSE);
			f = fopen(path + ".bak", "rt");
		}
	}

	FileCloser file_closer(f);
	g_stages_file_version = -1;

	if (f)
	{
		res = STAGES_LoadOK;
		if (!fgets(zzz, 1024, f) || sscanf(zzz, "VER %d", &g_stages_file_version) != 1)
			res = STAGES_LoadFailed;
		if (g_stages_file_version >= 7)
		{
			if (!fgets(zzz, 1024, f) || sscanf(zzz, "EXE %d %d %d", &executing, &selected, &file_mode) != 3)
				res = STAGES_LoadFailed;
			if (dialog)
				file_mode = STAGES_MODE_EDIT;
			if (file_mode == STAGES_MODE_ARCHIVE)
			{
				if (!DB_connection_info.IsReady())
					return STAGES_LoadDelayed;
				STAGES_SetCurrentMode(STAGES_MODE_ARCHIVE); 
				//if (DlgWaitDB_dlg.DoModal() != IDOK)
				//	return STAGES_LoadCancelled;
			}
		}
		else
		if (g_stages_file_version >= 2)
		{
			int end;
			if (!fgets(zzz, 1024, f) || sscanf(zzz, "END %d", &end) != 1)
				res = STAGES_LoadFailed;
		}
		if (!fgets(zzz, 1024, f) || sscanf(zzz, "N_S %d", &num) != 1 || num < 0)
			res = STAGES_LoadFailed;
	}
	else
	{
		num = 0;
		res = STAGES_LoadFailed;
	}

	FRingBuf<double>* prev_time_buff = g_cement_view->m_ptmData;
	FRingBuf<double>* prev_glub_buff = g_cement_view->m_pcurGlub;
	if (file_mode == STAGES_MODE_ARCHIVE)
	{
		g_cement_view->m_ptmData = pgtmDataDB;
		g_cement_view->m_pcurGlub = pgcurGlubDB;
	}
	else
	{
		g_cement_view->m_ptmData = pgtmData;
		g_cement_view->m_pcurGlub = pgcurGlub;
	}

	int prev_mode = g_current_mode;
	g_current_mode = (STAGES_Mode)file_mode;
//	STAGES_UpdateMapParam(selected);
	g_cement_view->InitDataView(num, 3, 0);
	g_cement_view->InitViews(&m_wndDialogBarCM, &m_MapParamCM);
    pApp->ChangeStyleAllView(FLD_DATA_ON_FIELD, gflgDataBegEnd);
	DeleteAdditionalParams();

	while (num > 0)
	{
		num--;
		if (STAGES_Stage::Load(f, file_mode) == NULL)
		{
			g_cement_view->m_ptmData = prev_time_buff;
			g_cement_view->m_pcurGlub = prev_glub_buff;
			return STAGES_LoadFailed;
		}
	}

	g_current_mode = (STAGES_Mode)prev_mode;

	LOGFONT lf;
	lf.lfHeight = 120;
	lf.lfWeight = FW_BOLD;
	sprintf(lf.lfFaceName, "Arial");
	if (g_stages_file_version >= 4)
	{
		if (!fgets(zzz, 1024, f) || sscanf(zzz, "FNT %d %d %[^;]s", &lf.lfHeight, &lf.lfWeight, &lf.lfFaceName) != 3)
			res = STAGES_LoadFailed;
	}
	g_all_stages_lf = g_sel_stage_lf = lf;
	if (g_stages_file_version >= 4)
	{
		if (fgets(zzz, 1024, f) && sscanf(zzz, "FNT %d %d %[^;]s", &lf.lfHeight, &lf.lfWeight, &lf.lfFaceName) == 3)
			g_all_stages_lf = lf;
	}
	g_all_stages_lf.lfWidth = g_sel_stage_lf.lfWidth = 0;
	g_all_stages_lf.lfEscapement = g_sel_stage_lf.lfEscapement = 0;
	g_all_stages_lf.lfOrientation = g_sel_stage_lf.lfOrientation = 0;
	g_all_stages_lf.lfItalic = g_sel_stage_lf.lfItalic = FALSE;
	g_all_stages_lf.lfUnderline = g_sel_stage_lf.lfUnderline = FALSE;
	g_all_stages_lf.lfStrikeOut = g_sel_stage_lf.lfStrikeOut = FALSE;
	g_all_stages_lf.lfCharSet = g_sel_stage_lf.lfCharSet = RUSSIAN_CHARSET;
	g_all_stages_lf.lfOutPrecision = g_sel_stage_lf.lfOutPrecision = OUT_STROKE_PRECIS;
	g_all_stages_lf.lfClipPrecision = g_sel_stage_lf.lfClipPrecision = CLIP_STROKE_PRECIS;
	g_all_stages_lf.lfQuality = g_sel_stage_lf.lfQuality = ANTIALIASED_QUALITY;
	g_all_stages_lf.lfPitchAndFamily = g_sel_stage_lf.lfPitchAndFamily = 34;
	CreateStagesFont(&g_sel_stage_main_font, &g_sel_stage_lf);
	CreateStagesFont(&g_all_stages_main_font, &g_all_stages_lf);

	g_selected_stage_num = selected;
	g_executing_stage_num = executing;

	SetExecutingStagePointer(false);

	STAGES_Mode mode = STAGES_Mode(file_mode);
	if (mode < STAGES_MODE_EDIT || mode > STAGES_MODE_ARCHIVE)
		mode = STAGES_MODE_EDIT;
	STAGES_SetCurrentMode(mode);

	CreateCalcHeaderFont(&g_sel_stage_header_font, &g_sel_stage_lf);
	g_list_selected_stage->SetListFont(g_sel_stage_main_font);
	CreateCalcHeaderFont(&g_all_stages_header_font, &g_all_stages_lf);
	g_list_stages->SetListFont(g_all_stages_main_font);

	STAGES_SetSelectedStage(g_selected_stage_num);
	m_wndDialogBarCM.m_PanelKnopki.Init(STAGES_GetStagesNumber(), CSize(20,20), g_cement_view);
	m_wndDialogBarCM.m_PanelKnopki.PushBtn(g_selected_stage_num - 1);
	m_wndDialogBarCM.Invalidate(TRUE);
	g_cement_view->ChangeaActiveListA(g_selected_stage_num - 1, 0);
	CheckTotalStatusForHeader();
	m_wndDialogPanelCement.RecalcListWidth();

	return res;
}

bool STAGES_SaveStages(bool dialog, PRJ_Branch branch)
{
	PRJ_Project* project = PRJ_GetProject();
	if (project == NULL || !project->m_stages_loaded)
		return false;
	CString filename = PRJ_GetWorkPath(branch);
	if (filename.IsEmpty())
		return false;
	filename += "\\stages";
	if (dialog)
	{
		filename = STAGES_last_file_path;
		int index = filename.ReverseFind('\\');
		if (index != -1)
			filename = filename.Left(index+1) + "stages";
		SYSTEMTIME st;
		GetLocalTime(&st);
		filename += TXT(" %04d.%02d.%02d %02d_%02d_%02d")<<st.wYear<<st.wMonth<<st.wDay<<st.wHour<<st.wMinute<<st.wSecond;
		CFileDialog dlg(FALSE, "cfg", filename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Файлы конфигурации(*.cfg)|*.cfg||");
		if (dlg.DoModal() != IDOK)
			return false;
		STAGES_last_file_path = filename = dlg.GetPathName();
	}
	else
		filename += ".cfg";

	FILE* f = fopen(filename, "rb");
	if (f)
	{
		fclose(f);
		CopyFile(filename, filename + ".bak", FALSE);
	}
	f = fopen(filename, "wt");
	if (!f)
		return false;

	fputs(CString(TXT("VER %d\n") << STAGES_FILE_VERSION), f);
	fputs(CString(TXT("EXE %d %d %d\n") << g_executing_stage_num << g_selected_stage_num << g_current_mode), f);
	fputs(CString(TXT("N_S %d\n") << g_stages.size()), f);
	STAGES_Stages::iterator current_stage = g_stages.begin();
	while (current_stage != g_stages.end())
	{
		(*current_stage)->Save(f);
		current_stage++;
	}
	fputs(CString(TXT("FNT %d %d %s;\n") << g_sel_stage_lf.lfHeight << g_sel_stage_lf.lfWeight << g_sel_stage_lf.lfFaceName), f);
	fputs(CString(TXT("FNT %d %d %s;\n") << g_all_stages_lf.lfHeight << g_all_stages_lf.lfWeight << g_all_stages_lf.lfFaceName), f);
	fclose(f);
	return true;
}

void SetupListCtrl(STAGES_List* the_list, CWnd* wnd, int width)
{
	the_list->m_show_selection = false;
	the_list->m_allow_move_selection_by_keys = false;
	the_list->SetListStyle(2);
	the_list->InsertColumn(0, "1", LVCFMT_LEFT, width);
	
	the_list->m_list_width = width;
	the_list->m_column_width = width;

	the_list->SetBkColor(g_colors[0]);
	the_list->m_lb_click_msg = (the_list == g_list_stages)?STAGES_LBD_MSG:STAGES_SELECTED_LBD_MSG;
	the_list->m_rb_click_msg = (the_list == g_list_stages)?STAGES_RBD_MSG:STAGES_SELECTED_RBD_MSG;
	the_list->m_messages_wnd = wnd;
}

void STAGES_PreInit()
{
    g_colors[0] = GetSysColor(COLOR_MENU);
	int r = GetRValue(g_colors[0]), g = GetGValue(g_colors[0]), b = GetBValue(g_colors[0]);
	double k1 = 1.1, k2 = 0.95, k3 = 0.9, k4 = 0.7, k5 = 0.4;
	g_colors[1] = RGB(min(255,r*k1), min(255,g*k1), min(255,b*k1));
	g_colors[2] = RGB(r*k2, g*k2, b*k2);
	g_colors[3] = RGB(r*k3, g*k3, b*k3);
	g_colors[4] = RGB(r*k4, g*k4, b*k4);
	g_colors[5] = RGB(r*k5, g*k5, b*k5);

	if (g_sel_stage_main_font == NULL)
	{
		g_sel_stage_lf.lfHeight = 120;
		g_sel_stage_lf.lfWidth = 0;
		g_sel_stage_lf.lfEscapement = 0;
		g_sel_stage_lf.lfOrientation = 0;
		g_sel_stage_lf.lfWeight = FW_BOLD;
		g_sel_stage_lf.lfItalic = FALSE;
		g_sel_stage_lf.lfUnderline = FALSE;
		g_sel_stage_lf.lfStrikeOut = FALSE;
		g_sel_stage_lf.lfCharSet = RUSSIAN_CHARSET;
		g_sel_stage_lf.lfOutPrecision = OUT_STROKE_PRECIS;
		g_sel_stage_lf.lfClipPrecision = CLIP_STROKE_PRECIS;
		g_sel_stage_lf.lfQuality = ANTIALIASED_QUALITY;
		g_sel_stage_lf.lfPitchAndFamily = 34;
		sprintf(g_sel_stage_lf.lfFaceName, "Arial");
		g_all_stages_lf = g_sel_stage_lf;
		CreateStagesFont(&g_sel_stage_main_font, &g_sel_stage_lf);
		CreateStagesFont(&g_all_stages_main_font, &g_all_stages_lf);
	}

	STAGES_LoadTemplates();
}

void STAGES_Init(STAGES_List* list_stages, STAGES_List* list_selected_stage, CWnd* wnd)
{
	if (g_inited || list_stages->m_hWnd == NULL)
		return;

	g_cement_view = (CCementView*)pApp->GetView(t_cement_rt);
	g_list_stages = list_stages;
	g_list_selected_stage = list_selected_stage;
	
	CreateCalcHeaderFont(&g_sel_stage_header_font, &g_sel_stage_lf);
	g_list_selected_stage->SetListFont(g_sel_stage_main_font);
	CreateCalcHeaderFont(&g_all_stages_header_font, &g_all_stages_lf);
	g_list_stages->SetListFont(g_all_stages_main_font);

	SetupListCtrl(g_list_stages, wnd, 500);
	SetupListCtrl(g_list_selected_stage, wnd, 212);

	RebuildStagesLists();
	CheckTotalStatusForHeader();

	g_cement_view->m_parOfView->SetNumActiveList(g_selected_stage_num - 1);
	g_inited = true;
}

void DeleteStageTempletes(bool on_exit = false)
{
	bool empty_exists = false;
	STAGES_StageTemplates::iterator current_template = STAGES_stage_templates.begin();
	while (current_template != STAGES_stage_templates.end())
	{
		if (on_exit || current_template->first != 0)
			delete current_template->second;
		if (current_template->first == 0)
			empty_exists = true;
		current_template++;
	}
	STAGES_StageTemplate* empty_template = NULL;
	if (empty_exists && !on_exit)
		empty_template = STAGES_stage_templates[0];
	STAGES_stage_templates.clear();
	if (empty_template)
		STAGES_stage_templates[0] = empty_template;
	if (!on_exit && !empty_exists)
	{
		//CHECK(empty_template == NULL);
		STAGES_StageTemplate* stage_template = new STAGES_StageTemplate;
		stage_template->m_name = "Пустой";
		STAGES_stage_templates[0] = stage_template;
	}
}

void DeleteFont(CFont** fnt)
{
	if (*fnt)
	{
		(*fnt)->DeleteObject();
		delete (*fnt);
		*fnt = NULL;
	}
}

void STAGES_Shutdown()
{
	if (g_inited)
		STAGES_StopGettingFromDB();

	g_list_selected_stage->DeleteInfo();
	g_list_selected_stage->DeleteAllItems();
	g_list_stages->DeleteInfo();
	g_list_stages->DeleteAllItems();

	DeleteFont(&g_sel_stage_main_font);
	DeleteFont(&g_all_stages_main_font);
	DeleteFont(&g_sel_stage_header_font);
	DeleteFont(&g_all_stages_header_font);

	DeleteStageTempletes(true);
	DeleteAllStages();
	DeleteAdditionalParams();
}

void STAGES_ClearAllStages()
{
	STAGES_SetCurrentMode(STAGES_MODE_EDIT);
	DeleteAllStages();
}

bool STAGES_IsInited()
{
	return g_inited;
}

void InternalOnClick(WPARAM w, LPARAM l, byte button, byte who)
{
	if (w == 0xFFFFFFFF)
		return;


	STAGES_List* work_list = (who == STAGES_WHO_ALL_STAGES)?g_list_stages:g_list_selected_stage;
	STAGES_List* other_list = (who == STAGES_WHO_ALL_STAGES)?g_list_selected_stage:g_list_stages;

	if (work_list == g_list_selected_stage && button == STAGES_LEFT_BUTTON)
		return;

	int item = w >> 8;
	int x = l >> 16, y = l & 0xFFFF;
	if (item == 0xFFFFFF)
		return;
	DWORD data = work_list->GetItemData(item);
	UINT stage_num = data >> 16;
	int line = data & 0xFFFF;
	bool header = ((work_list == g_list_stages) && (item == 0 || item == work_list->GetTopIndex()));
	if (header)
	{
		data = 0x00000001;
		line = 0;
		stage_num = 0;
	}

	STAGES_Stages::iterator current_stage = g_stages.begin();
	while (current_stage != g_stages.end() && (*current_stage)->m_stage_num < stage_num)
		current_stage++;
	if (current_stage == g_stages.end() && !header)
		return;
	STAGES_Stage* stage = (current_stage == g_stages.end()) ? NULL : *current_stage;

	STAGES_ListInfo::iterator desired = work_list->m_list_info.find(data);
	if (desired == work_list->m_list_info.end())
		return;
	STAGES_LineInfo* line_info = desired->second;

	if (button == STAGES_RIGHT_BUTTON)
	{
		if (!header && g_selected_text_info != NULL && g_selected_text_info->m_editable && g_selected_text_info->m_shown)
		{
			if (/*g_current_mode == STAGES_MODE_EDIT && */g_selected_text_info->m_calculation != NULL && g_selected_text_info->m_data_type == STAGES_DATA_STRING)
			{
			}
			else
			if (g_selected_text_info->m_calculation != NULL && !g_selected_text_info->m_calculation->m_was_ok &&
				g_selected_text_info->m_data != g_selected_text_info->m_calculation->m_value_column + STAGES_COLUMN_PRESET)
				return;
			STAGES_TextInfo* text_info = g_selected_text_info;
			CString str = text_info->GetText(true);
			CRect r;
			r = text_info->m_last_rect;
			r.top -= 2;
			if (text_info->m_right_alignment)
			{
				r.left -= 2*work_list->m_h;
				r.right += 2;
			}
			else
			{
				r.right += 2*work_list->m_h;
				r.left -= 2;
			}

			other_list->SetCapture(); 
			work_list->m_mouse_in_item = -1;
			
			work_list->ClientToScreen(&r);
			DlgEditText dlg(str, r, false, text_info->m_right_alignment? +1 : -1);
			dlg.m_main_font = work_list->GetFont();
			if (dlg.DoModal() == IDOK)
			{
				str = dlg.GetText();
				text_info->SetText(str);
			}
			return;
		}
		return;
	}

	if (button == STAGES_LEFT_BUTTON)
	{
		if (g_selected_text_info != NULL && (g_selected_text_info->m_hidable || g_selected_text_info->m_data_type == STAGES_DATA_BUTTON))
		{
			g_selected_text_info->OnClick(x, y);
			return;
		}		
		if (!header)
		{
			STAGES_SetSelectedStage(stage->m_stage_num);
		}
		return;
	}
}

void STAGES_OnClick(WPARAM w, LPARAM l, byte button, byte who)
{
	DWORD prev_stage = STAGES_GetSelectedStage(), prev_stage_size = STAGES_GetStagesNumber();
	g_prev_column_stages_width = m_wndDialogPanelCement.m_list_stages.m_column_width;
	g_prev_column_selected_width = m_wndDialogBarCM.m_list_selected_stage.m_column_width;

	InternalOnClick(w, l, button, who);

	DWORD now_stage = STAGES_GetSelectedStage(), new_stage_size = STAGES_GetStagesNumber();
	if (now_stage != prev_stage || prev_stage_size != new_stage_size)
		StagesSelectionChanged(prev_stage_size != new_stage_size);
	ListSizeChanged();
}

void STAGES_InvalidateLists()
{
	if (g_list_stages == NULL || g_list_stages->m_hWnd == NULL)
		return;

	g_arrow_counter++;
	if (g_arrow_counter >= g_max_arrow_counter)
		g_arrow_counter = 0;

	if (g_current_mode == STAGES_MODE_EXECUTION || g_current_mode == STAGES_MODE_PAUSE)
	{
		if (g_executing_stage != NULL)
			g_executing_stage->m_duration = COleDateTime::GetCurrentTime().m_dt - g_executing_stage->m_start_time;
	}

	g_list_stages->Invalidate(FALSE);
	g_list_selected_stage->Invalidate(FALSE);
}

void STAGES_EvaluateCalculations()
{
	if (g_executing_stage == NULL && !(g_current_mode == STAGES_MODE_ARCHIVE && g_current_db_stage_num > 0))
	{
		STAGES_InvalidateLists();
		return;
	}

	if (g_current_mode == STAGES_MODE_EXECUTION || g_current_mode == STAGES_MODE_PAUSE)
		g_executing_stage->m_duration = COleDateTime::GetCurrentTime().m_dt - g_executing_stage->m_start_time;

	if (g_current_mode == STAGES_MODE_EXECUTION)
		g_executing_stage->Calculate();

	STAGES_InvalidateLists();
}

void CheckStageSelectionChanged(int prev_selected_stage, bool start_graphs)
{
	DWORD now_selected_stage = STAGES_GetSelectedStage();
	CCementView* view = (CCementView*)pApp->GetView(t_cement_rt);
	if (STAGES_GetExecutingStage() == 1)
	{
		m_wndDialogBarCM.RecalcListWidth();
		m_wndDialogBarCM.m_PanelKnopki.Init(STAGES_GetStagesNumber(), CSize(20,20), pApp->GetView(t_cement_rt));
		m_wndDialogBarCM.m_PanelKnopki.PushBtn(now_selected_stage - 1);
		view->ChangeaActiveListA(now_selected_stage - 1, 0);
		view->RecalcStagesList();
	}
	else
	if (prev_selected_stage != now_selected_stage)
	{
		m_wndDialogBarCM.m_PanelKnopki.PushBtn(now_selected_stage - 1);
		view->ChangeaActiveListA(now_selected_stage - 1, 0);
	}
	if (start_graphs && g_executing_stage_num > 0 && g_executing_stage_num <= STAGES_GetStagesNumber())
		view->SetValidIndexBeginData(g_executing_stage_num - 1);
}

void STAGES_StartPauseStage()
{
	CCementView* view = (CCementView*)pApp->GetView(t_cement_rt);
	DWORD prev_selected_stage = STAGES_GetSelectedStage();
	switch (g_current_mode)
	{
		case STAGES_MODE_PAUSE:
			SetAdditionalParamsTimeout();
			STAGES_SetCurrentMode(STAGES_MODE_EXECUTION);
			break;
		case STAGES_MODE_EXECUTION:
			SetAdditionalParamsTimeout();
			if (KRS_running)
				KRS_SendCurrentParamsToBD(); 
			KRS_need_send_to_db_additional_to = true;
			STAGES_SetCurrentMode(STAGES_MODE_PAUSE);
			break;
		case STAGES_MODE_BREAK:
			g_executing_stage->m_start_time = COleDateTime::GetCurrentTime().m_dt;
			view->SetValidIndexBeginData(g_executing_stage_num - 1);
//			KRS_SendCurrentParamsToBD();
			STAGES_SetCurrentMode(STAGES_MODE_EXECUTION);
			RebuildStagesLists();
			break;
		case STAGES_MODE_EDIT:
			g_executing_stage_num = 1;
			SetAdditionalParamsTimeout();
			SetExecutingStagePointer(true);
			STAGES_SetSelectedStage(1);
			g_executing_stage->m_collapsed_in_list = false;
			CheckTotalStatusForHeader();
			g_executing_stage->m_start_time = COleDateTime::GetCurrentTime().m_dt;
			CheckStageSelectionChanged(prev_selected_stage, true);
			STAGES_SetCurrentMode(STAGES_MODE_EXECUTION);
			RebuildStagesLists();
			break;
	}
}

void STAGES_FinishStage() 
{
	CString text;
	if (STAGES_IsLastStage())
		text = "Завершить выполнение работы?";
	else
		text = "Завершить выполнение этапа?";

	if (g_list_stages->MessageBox(text, g_title_request_action, MB_YESNO) != IDYES)
		return;

	DWORD prev_selected_stage = STAGES_GetSelectedStage();
	bool move_selection = (g_selected_stage_num == g_executing_stage_num);

	g_executing_stage_num++;
	if (g_executing_stage_num > g_stages.size())
	{
		PRJ_timer_saving_available = false;
		STAGES_SetCurrentMode(STAGES_MODE_ARCHIVE);
		if (!PRJ_SaveProject(PRJ_NowSelectedBranch, PRJ_SaveAllProject))
		{
			::MessageBox(AfxGetMainWnd()->m_hWnd, "По завершению этапов проект не был сохранён!", "Ошибка сохранения", MB_OK);
		}
		KSR_StopAllConnections();
		PRJ_LoadProjectResult res = PRJ_LoadProject(PRJ_GetProjectPath());
		if (res != PRJ_LoadProjectOK)
		{
			DlgWork dlg_work("Открыть работу", DlgWork_OPEN);
			if (dlg_work.DoModal() == IDCANCEL)
			{
				pFrm->PostMessage(WM_CLOSE, 0, 0);//DestroyWindow();
				PRJ_timer_saving_available = false;
			}
			else
				PRJ_timer_saving_available = true;
		}
		else
			PRJ_timer_saving_available = true;
		PRJ_Project* prg = PRJ_GetProject();
		if (prg && prg->m_ok)
			KSR_StartAllConnections();
		return;
	}

	SetAdditionalParamsTimeout();
	KRS_SendCurrentParamsToBD(); 

	g_executing_stage->m_collapsed_in_list = true;
	SetExecutingStagePointer(false);
	g_executing_stage->m_collapsed_in_list = false;

	if (move_selection)
		STAGES_SetSelectedStage(g_executing_stage_num);
	else
		RebuildStagesLists();

	CheckStageSelectionChanged(prev_selected_stage, false);
	STAGES_SetCurrentMode(STAGES_MODE_BREAK);	
}

void STAGES_FinishAll() 
{
	if (g_list_stages->MessageBox("Прервать выполнение задания?", g_title_request_action, MB_YESNO) != IDYES)
		return;
	STAGES_SetCurrentMode(STAGES_MODE_ARCHIVE);
}

void STAGES_RestartStage() 
{
	if (g_list_stages->MessageBox("Сбросить данные текущего этапа?", g_title_request_action, MB_YESNO) != IDYES)
		return;

	if (g_executing_stage_num == 1)
		STAGES_SetCurrentMode(STAGES_MODE_EDIT);
	else
	{
		if (g_executing_stage->m_start_time > 0)
		{
			g_executing_stage->ResetCalculations();
			STAGES_SetCurrentMode(STAGES_MODE_BREAK);
		}
	}
	RebuildStagesLists();
}

void STAGES_RestartAll() 
{
	if (g_list_stages->MessageBox("Сбросить все данные работы?", g_title_request_action, MB_YESNO) != IDYES)
		return;
	STAGES_SetCurrentMode(STAGES_MODE_EDIT);	
}

bool STAGES_IsEvaluating()
{
	return g_current_mode == STAGES_MODE_EXECUTION;
}

bool STAGES_IsFinished()
{
	return g_executing_stage_num > g_stages.size();
}

bool STAGES_SaveTemplates()
{
	FILE*f = fopen(KRS_root_path + "\\stages_lib.cfg", "wt");
	if (!f)
		return false;

	fputs(CString(TXT("VER %d\n") << STAGE_TEMPLATES_FILE_VERSION), f);
	fputs(CString(TXT("N_T %d\n") << STAGES_stage_templates.size() - 1), f);
	STAGES_StageTemplates::iterator current = STAGES_stage_templates.begin();
	while (current != STAGES_stage_templates.end())
	{
		if (current->first != 0)
			current->second->Save(f);
		current++;
	}
	fclose(f);
	return true;	
}

bool STAGES_LoadTemplates()
{
	DeleteStageTempletes();
	g_stages_file_version = -1;

	FILE*f = fopen(KRS_root_path + "\\stages_lib.cfg", "rt");
	if (!f)
		return false;
	FileCloser file_closer(f);
	char zzz[1024];
	int num;
	if (!fgets(zzz, 1024, f) || sscanf(zzz, "VER %d", &g_stages_templates_file_version) != 1)
		return false;
	if (!fgets(zzz, 1024, f) || sscanf(zzz, "N_T %d", &num) != 1 || num < 0)
		return false;

	STAGES_StageTemplate* stage_template;
	while (num > 0)
	{
		num--;
		stage_template = STAGES_StageTemplate::Load(f);
		if (stage_template == NULL)
		{
			DeleteStageTempletes();
			return false;
		}
		STAGES_stage_templates[stage_template->m_stage_template_num] = stage_template;
	}
	return true;
}

void STAGE_ChooseFont(bool selected_stages_font)
{
	CFontDialog dlg;
	LOGFONT tmp_lf;
	CFont** fnt = selected_stages_font ? &g_sel_stage_main_font : &g_all_stages_main_font;
	CFont** fnt_header = selected_stages_font ? &g_sel_stage_header_font : &g_all_stages_header_font;
	LOGFONT* lf = selected_stages_font ? &g_sel_stage_lf : &g_all_stages_lf;
	(*fnt)->GetLogFont(&tmp_lf);
	dlg.m_cf.lpLogFont = &tmp_lf;
	dlg.m_cf.rgbColors = 0x000000;
	dlg.m_cf.Flags = CF_ENABLEHOOK | CF_SCREENFONTS | CF_ANSIONLY | CF_INITTOLOGFONTSTRUCT | CF_NOSCRIPTSEL | CF_NOVERTFONTS;
	if (dlg.DoModal() == IDOK) 
	{
		*lf = tmp_lf;
		lf->lfHeight = dlg.m_cf.iPointSize;
		lf->lfQuality = ANTIALIASED_QUALITY;
		CreateStagesFont(fnt, lf);
		CreateCalcHeaderFont(fnt_header, lf);
		STAGES_List* lst = selected_stages_font ? g_list_selected_stage : g_list_stages;
		lst->SetListFont(*fnt);
		RebuildStagesLists();
	}
}

bool STAGES_IsLastStage()
{
	return g_executing_stage_num == g_stages.size();
}

DWORD STAGES_GetStagesNumber()
{
	return g_stages.size();
}

void STAGES_UpdateMapParam(int num_stage)
{
	if (num_stage == -1)
		num_stage = g_selected_stage_num;

	m_MapParamCM.RemoveAll();
	CParam *param;
	int key;
	CParamMap* map = (g_current_mode == STAGES_MODE_ARCHIVE)?(&m_MapParamDB):(&m_MapParam);
	POSITION pos = map->GetStartPosition();
	while (pos != NULL)
	{
		map->GetNextAssoc(pos, key, param);
		if (key >= STAGES_START_ADDITIONAL_PARAM_NUM &&
			g_current_mode == STAGES_MODE_ARCHIVE)
			//!m_MapParamCM_additional_only.Lookup(key, param_2))
			continue;
		m_MapParamCM[key] = param;
	}
	{
		STAGES_Calculation *calculation;
		STAGES_Calculations::iterator current_calculation;
		STAGES_Stage *stage;
		STAGES_Stages::iterator current_stage = g_stages.begin();
		while (current_stage != g_stages.end())
		{
			stage = *current_stage;
			if (stage->m_stage_num == num_stage)
			{
				current_calculation = stage->m_calculations.begin();
				while (current_calculation != stage->m_calculations.end())
				{
					calculation = *current_calculation;
					if (calculation->m_has_graph && !m_MapParamDefault.Lookup(calculation->m_corresponding_param_num, param))
					{
						if (!m_MapParamCM_additional_only.Lookup(calculation->m_corresponding_param_num, param))
							g_list_stages->MessageBox(CString(TXT(" параметр #%d не обнаружен")<<calculation->m_corresponding_param_num), "Внутренняя ошибка", MB_OK);
						else
						{
							if (g_current_mode == STAGES_MODE_ARCHIVE && !m_MapParamDB.Lookup(calculation->m_corresponding_param_num, param))
								g_list_stages->MessageBox(CString(TXT(" параметр #%d не обнаружен")<<calculation->m_corresponding_param_num), "Внутренняя ошибка", MB_OK);
							m_MapParamCM[calculation->m_corresponding_param_num] = param;
						}
					}
					current_calculation++;
				}
				break;
			}
			current_stage++;
		}
	}
}

void STAGES_SetSelectedStage(DWORD num)
{
	if (num < 1 || num > g_stages.size())
		return;
	g_selected_stage_num = num;
	RebuildStagesLists();
	STAGES_UpdateMapParam();
}

DWORD STAGES_GetSelectedStage()
{
	return g_selected_stage_num;
}

DWORD STAGES_GetExecutingStage()
{
	return g_executing_stage_num;
}

int STAGES_RecalcBarHeight(int cy)
{
	if (g_list_stages == NULL || g_list_stages->m_h <= 0)
		return cy;
	return BS_ROUND(float(cy - 9)/g_list_stages->m_h)*g_list_stages->m_h + 9;
}

STAGES_Mode STAGES_GetCurrentMode()
{
	return g_current_mode;
}

/*
CString STAGES_GetStageName(int num_stage)
{
    STAGES_Stage *stage;
    STAGES_Stages::iterator current_stage = g_stages.begin();
    while (current_stage != g_stages.end())
    {
        stage = *current_stage;
        if (stage->m_stage_num == num_stage)
            return stage->m_name;
        current_stage++;
    }
    return "";
}
*/

STAGES_Stage* STAGES_GetStage(int num_stage)
{
    STAGES_Stages::iterator current_stage = g_stages.begin();
    while (current_stage != g_stages.end())
    {
        if ((*current_stage)->m_stage_num == num_stage)
            return *current_stage;
        current_stage++;
    }
    return NULL;
}
DWORD STAGES_GetCurrentStageDB()
{
	return g_current_db_stage_num;
}

bool SendOneStageRequest()
{
	STAGES_Stage* db_stage = STAGES_GetStage(g_current_db_stage_num);
	if (db_stage == NULL)
		return false;
	DB_RequestParamValuesPacket packet;
	packet.m_from_time = db_stage->m_start_time;
	packet.m_to_time = db_stage->m_start_time + db_stage->m_duration;

	CParamMap map_param;
	ParamOfView *pList = g_cement_view->m_parOfView->GetList(g_current_db_stage_num - 1);
	if (pList == NULL)
		return false;
	
	pList->AddActiveParam(&map_param);

	int key;
    CParam *param;
	POSITION pos = map_param.GetStartPosition();
	while (pos != NULL)
	{
		map_param.GetNextAssoc(pos, key, param);
		if (!param)
			continue;
		packet.m_params_indexes.push_back(key);
	}
	if (packet.m_params_indexes.size() == 0)
		return false;

	if (!DB_connection_info.IsReady())
		return false;

	DBGET_Get(&packet);
	return true;//DB_SendPacket(&packet);
}

UINT StagesGetFromDataBaseTreadProcedure(LPVOID param)
{
	m_wndDialogBarH.EnableViewSwitchers(FALSE);
	g_current_db_stage_num = 1;

	STAGES_Stage* stage;
	ParamOfView *pList;
	DWORD res;
	UINT pos;
	HANDLE events[2] = {g_stop_getting_from_db_event, g_stage_finished_from_db_event};
	while (g_current_db_stage_num <= g_stages.size())
	{
		pList = g_cement_view->m_parOfView->GetList(g_current_db_stage_num - 1);
		pos = g_cement_view->m_ptmData->GetCur();
		if (SendOneStageRequest()) 
		{
			if (pList)
			{
				pList->SetUMarkerPos(0, pos);
				pList->SetUMarkerPos(1, pos);
			}
			res = WaitForMultipleObjects(2, events, FALSE, INFINITE); 
			if (res != WAIT_OBJECT_0 + 1)
				break; 
			if (pList)
			{
				pList->SetUMarkerPos(0, pos);
				pList->SetUMarkerPos(1, pos);
				pos = g_cement_view->m_ptmData->GetCur();
				pList->SetUMarkerPos(3, pos);
				pList->SetUMarkerPos(2, pos);
				stage = STAGES_GetStage(g_current_db_stage_num);
				if (g_cement_view && g_cement_view->m_hWnd != NULL && ::IsWindow(g_cement_view->m_hWnd))
				{
					g_cement_view->SetStartX(stage->m_start_time, g_current_db_stage_num - 1, FALSE);
					//g_cement_view->Invalidate(FALSE);
				}
			}
		}
		g_current_db_stage_num++;
	}
	m_wndDialogBarH.EnableViewSwitchers(TRUE);
	g_current_db_stage_num = 0;
	g_cement_view->Invalidate(FALSE);
	SetEvent(g_getting_from_db_finished_event);
	return +1;
}

void STAGES_StopGettingFromDB()
{
	if (g_current_db_stage_num > 0)
	{
		SetEvent(g_stop_getting_from_db_event);
		WaitForSingleObject(g_getting_from_db_finished_event, INFINITE);
	}
}

void STAGES_GetFromDB()
{
	if (g_current_db_stage_num != 0)
	{
		g_list_stages->MessageBox("Идёт обработка предыдущего запроса", "Операция не будет выполнена", MB_OK);
		return;
	}
	if (!DB_connection_info.IsReady())
	{
		g_list_stages->MessageBox("Невозможно выполнить операцию.\nНет подключения к Базе Данных", "Ошибка", MB_OK);
		return;
	}
	if (g_current_mode != STAGES_MODE_ARCHIVE)
	{
		g_list_stages->MessageBox("Внутрення ошибка.\nНеверный режим работы с проектом", "Ошибка", MB_OK);
		return;
	}

	if(g_cement_view != NULL) 	
		g_cement_view->ClearAllParamsBuffers(&m_MapParamDB, g_cement_view->m_ptmData, g_cement_view->m_pcurGlub);

	AfxBeginThread(StagesGetFromDataBaseTreadProcedure, NULL, THREAD_PRIORITY_LOWEST);
}

void STAGES_DB_Reaction(CWnd* wnd, DB_Packet* packet)
{
	CCementView* view = (CCementView*)wnd;

	if (packet->m_packet_type == DB_TransferParamValues)
	{
		DB_TransferParamValuesPacket* transfer_packet = (DB_TransferParamValuesPacket*)packet;

		DBGET_HandlePacket(transfer_packet, view);
		if (DBGET_RequestFinished())
			SetEvent(g_stage_finished_from_db_event); 
	}
}

////////////////////////////////////////////////////////////////////////////////
// end