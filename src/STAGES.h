#ifndef __STAGES_H_
#define __STAGES_H_

#include "PRJ.h"
#include "FORMULA.h"
#include "..\ExtListCtrl.h"
#include "DB.h"

#define STAGES_START_ADDITIONAL_PARAM_NUM 1001

#define STAGES_LBD_MSG (WM_USER + 333)
#define STAGES_SELECTED_LBD_MSG (WM_USER + 334)
#define STAGES_RBD_MSG (WM_USER + 335)
#define STAGES_SELECTED_RBD_MSG (WM_USER + 336)

#define STAGES_WHO_ALL_STAGES 0
#define STAGES_WHO_SELECTED_STAGE 1

#define STAGES_LEFT_BUTTON 1
#define STAGES_RIGHT_BUTTON 2

#define STAGES_COLUMN_VALUE 0
#define STAGES_COLUMN_PRESET 1
#define STAGES_COLUMN_DELTA 2
#define STAGES_COLUMN_MIN 3
#define STAGES_COLUMN_MAX 4
#define STAGES_COLUMN_MID 5
enum STAGES_LoadResult
{
	STAGES_LoadOK = 1,
	STAGES_LoadFailed,
	STAGES_LoadCancelled,
	STAGES_LoadDelayed
};
enum STAGES_Mode
{
	STAGES_MODE_EDIT = 0,
	STAGES_MODE_EXECUTION,
	STAGES_MODE_ARCHIVE,
	STAGES_MODE_PAUSE, 
	STAGES_MODE_BREAK 
};
struct STAGES_CalculationTemplate
{
	STAGES_CalculationTemplate();
	~STAGES_CalculationTemplate();

	void Save(FILE*f, bool info_for_template_only);
	static STAGES_CalculationTemplate* Load(FILE*f);
	
	bool LoadTemplateData(FILE*f, bool info_for_template_only);

	CString m_name;
	FORMULA_Formula* m_formula;

	float m_preset_value;

	bool m_show_column[6];
	bool m_has_graph;
	byte m_auto_graph_mode;
	
	int CorrespondingParamNum();

	DWORD m_graph_color;
	int m_graph_line_width;
	double m_graph_min_val, m_graph_max_val, m_db_time_interval, m_db_data_interval;

	int m_corresponding_param_num;
};

typedef list<STAGES_CalculationTemplate*> STAGES_CalculationTemplates;
struct STAGES_Calculation: STAGES_CalculationTemplate
{
	STAGES_Calculation(STAGES_CalculationTemplate *calc_template = NULL);
	~STAGES_Calculation();

	void Save(FILE*f);
	static STAGES_Calculation* Load(FILE*f);

	void Reset();
	void Calculate();

	float m_value_column[6];
	DWORD m_mid_counter;
	byte m_status;
	bool m_was_ok;
};

typedef list<STAGES_Calculation*> STAGES_Calculations;
struct STAGES_StageTemplate
{
	STAGES_StageTemplate();
	STAGES_StageTemplate(STAGES_StageTemplate* etalon);
	~STAGES_StageTemplate();
	
	CString m_name;
	int m_stage_template_num;
	STAGES_CalculationTemplates m_calculation_templates;
	
	void Save(FILE*f);
	static STAGES_StageTemplate* Load(FILE*f);
};

typedef map<int, STAGES_StageTemplate*> STAGES_StageTemplates;
struct STAGES_StageInfoForSequence
{
	STAGES_StageInfoForSequence(int template_num = -1);
	int m_stage_template_num;
	CString m_stage_name;
};

typedef list<STAGES_StageInfoForSequence> STAGES_WorkingSequence;
struct STAGES_Stage
{
	STAGES_Stage(STAGES_StageTemplate* stage_template);
	~STAGES_Stage();

	void Save(FILE*f);
	static STAGES_Stage* Load(FILE*f, byte mode);

	void Calculate();
	void ResetCalculations();

	CString m_name;
	bool m_collapsed_in_list;
	UINT m_stage_num;
	double m_start_time;
	double m_duration;
	STAGES_Calculations m_calculations;
};

typedef list<STAGES_Stage*> STAGES_Stages;
enum STAGES_DataType
{
	STAGES_DATA_NONE = 0,
	STAGES_DATA_STRING,
	STAGES_DATA_FLOAT,
	STAGES_DATA_START_TIME,
	STAGES_DATA_DURATION,
	STAGES_DATA_BUTTON,
};
struct STAGES_LineInfo;
struct STAGES_TextInfo
{
	STAGES_TextInfo(void* data, STAGES_DataType data_type, int x, DWORD text_color);

	virtual void OnClick(int x, int y);
	virtual void Draw(CDC* dc, int y, int w);
	virtual int CalculateWidth(CDC* dc, int w);
	CString GetText(bool for_edit = false);
	void SetText(CString str);

	CRect m_last_rect;
	CString m_tool_tip_info;
	
	int m_x, m_w;
	DWORD m_text_color;

	bool m_editable;
	void* m_data;
	STAGES_DataType m_data_type;
	
	STAGES_Calculation* m_calculation; 

	bool m_right_alignment, m_shown, m_hidable, m_disabled;
	
	STAGES_LineInfo* m_parent;
};
struct STAGES_Button: STAGES_TextInfo
{
	STAGES_Button(STAGES_Stage *stage, STAGES_Calculation* calculation, int button_id);

	virtual void OnClick(int x, int y);
	virtual void Draw(CDC* dc, int y, int w);
	virtual int CalculateWidth(CDC* dc, int w);
	
	int m_button_id, m_button_num;
};
struct STAGES_LineInfo
{
	STAGES_LineInfo(STAGES_Stage *stage = NULL, DWORD bk_color = 0xFFFFFF);
	~STAGES_LineInfo();
	
	STAGES_Stage *m_stage;
	list<STAGES_TextInfo*> m_elements;
	bool m_last_line, m_header_stage;
	DWORD m_bk_color, m_buttons_num;
	
	void AddTextInfo(STAGES_TextInfo* text_info);
	void AddButton(STAGES_Button* button);
};

typedef map<DWORD, STAGES_LineInfo*> STAGES_ListInfo;
struct STAGES_List: CExtListCtrl
{
	STAGES_List();
	virtual ~STAGES_List();

	//afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual void OnMouseMoveIn(CPoint point);
	CPoint m_mouse_point;

	LRESULT OnMenu(WPARAM data, LPARAM l);
	
	void Rebuild();
	void StartRebuild();
	void FinishRebuild();
	void AddLineInfoForCalculation(STAGES_LineInfo* line_info, STAGES_Calculation* calculation, bool selected);
	void AddLine(STAGES_LineInfo* line_info, DWORD stage_num, DWORD line, bool add_data);
	void AddStage(STAGES_Stage*stage);

	void DeleteInfo();

	STAGES_ListInfo m_list_info;
	int m_list_width, m_column_width;
	CDC m_cdc;
	int m_h;

	DECLARE_MESSAGE_MAP()

private:
	int m_line_counter;	
};

void STAGES_PreInit();
void STAGES_Init(STAGES_List* list_stages, STAGES_List* list_selected_stage, CWnd* wnd);
void STAGES_Shutdown();
bool STAGES_IsInited();
STAGES_LoadResult STAGES_LoadStages(bool dialog, bool bak);
bool STAGES_SaveStages(bool dialog, PRJ_Branch branch);
void STAGES_ClearAllStages();
bool STAGES_LoadTemplates();
bool STAGES_SaveTemplates();

void STAGES_OnClick(WPARAM w, LPARAM l, byte button, byte who);
void STAGES_EvaluateCalculations();

void STAGES_StartPauseStage(); 
void STAGES_FinishStage(); 
void STAGES_FinishAll(); 
void STAGES_RestartStage(); 
void STAGES_RestartAll(); 

STAGES_Mode STAGES_GetCurrentMode();
void STAGES_SetCurrentMode(STAGES_Mode mode);

bool STAGES_IsEvaluating();
bool STAGES_IsFinished();
bool STAGES_IsLastStage();

void STAGE_ChooseFont(bool selected_stages_font);

DWORD STAGES_GetStagesNumber();
void STAGES_SetSelectedStage(DWORD num);
DWORD STAGES_GetSelectedStage();
DWORD STAGES_GetExecutingStage();

void STAGES_UpdateMapParam(int num_stage = -1);
int STAGES_RecalcBarHeight(int cy);
//CString STAGES_GetStageName(int num_stage);
STAGES_Stage* STAGES_GetStage(int num_stage);

void STAGES_GetFromDB();
void STAGES_StopGettingFromDB();
DWORD STAGES_GetCurrentStageDB();

void STAGES_InvalidateLists();
//void STAGES_AdjustAdditionalParamsDB();

void STAGES_DB_Reaction(CWnd* wnd, DB_Packet* packet);
extern STAGES_StageTemplates STAGES_stage_templates;
extern int STAGES_zazor;
//extern bool STAGES_next_open_stages_make_editable;
extern CString STAGES_last_file_path;

void STAGES_ChangeModeToEdit(CString work_path);

#endif
////////////////////////////////////////////////////////////////////////////////
// end