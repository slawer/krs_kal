// MapParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "MapParamDlg.h"
#include "DlgSelectChannel.h"
#include "DlgSelectAttr.h"
#include "NewCorrespondenceDlg.h"
#include "BS.h"
#include "FORMULA.h"
#include <check.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_LIST_SELECTION_CHANGED (WM_USER+1)

/////////////////////////////////////////////////////////////////////////////
// MapParamDlg dialog

MapParamDlg::MapParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MapParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(MapParamDlg)
	//}}AFX_DATA_INIT
}

void MapParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MapParamDlg)
	DDX_Control(pDX, IDC_LIST_TABLE, m_table_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MapParamDlg, CDialog)
	//{{AFX_MSG_MAP(MapParamDlg)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, OnButtonChange)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TABLE, OnDblClickListTable)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_MESSAGE(WM_LIST_SELECTION_CHANGED, OnListSelectionChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MapParamDlg message handlers

#define WIDTH_DATA_COL 270

BOOL MapParamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_frame = (CMainFrame *)AfxGetMainWnd();

	m_table_list.SetListFont(GetFont());
	m_table_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_table_list.m_show_selection = false;
	RECT r;
	m_table_list.GetClientRect(&r);
	int len_N = 25, len_other = (r.right - r.left) - len_N - WIDTH_DATA_COL - 16;
	m_table_list.InsertColumn(0, "N", LVCFMT_LEFT, len_N);
    m_table_list.InsertColumn(1, "Параметр", LVCFMT_LEFT, len_other);
	m_table_list.InsertColumn(3, "Исходные данные", LVCFMT_LEFT, WIDTH_DATA_COL);
	RebuildTable();

	m_prev_selection = m_table_list.GetCurItem();

	m_table_list.m_lb_click_msg = WM_LIST_SELECTION_CHANGED;
	m_table_list.m_key_down_msg = WM_LIST_SELECTION_CHANGED;
	m_table_list.m_messages_wnd = this;

	return TRUE;
}

void MapParamDlg::SetupLineColor(int item)
{
	DWORD param_num = m_table_list.GetItemData(item);
	CParam *param;
	if (m_MapParamDefault.Lookup(param_num, param))
	{
		int selected = m_table_list.GetCurItem();
		DWORD back_color = (param->bParamActive) ? (item == selected ? 0x558855 : 0xAAFFAA) : (item == selected ? 0x888888 : 0xFFFFFF);
		DWORD text_color = (param->bParamActive) ? (item == selected ? 0xFFFFFF : 0x000000) : (item == selected ? 0xFFFFFF : 0x000000);
		m_table_list.SetRowBkColor(item, back_color);
		m_table_list.SetRowTextColor(item, text_color);
	}
}

void MapParamDlg::SetupLine(int item, CParam *param)
{
	if (param)
		m_table_list.SetItemText(item, 1, param->sName);
	CString str = BS_GetChannelName(param);
	if (param->m_channel_num > 0 && param->m_attr_channel_num > 0)
		str += "  и  " + BS_GetAttrChannelName(param);
	m_table_list.SetItemText(item, 2, str);
	SetupLineColor(item);
}

void MapParamDlg::ChooseChannel(int item)
{
	MutexWrap params_access(m_params_map_mutex);

	int param_num = m_table_list.GetItemData(item);

	CParam* param = NULL;
	if (!m_MapParamDefault.Lookup(param_num, param))
		CRUSH("Параметр отсутствует");
	CHECK(param != NULL);
	NewCorrespondenceDlg dlg(param, false);
	if (dlg.DoModal() == IDCANCEL)
		return;

	if (param->bParamActive)
	{
		m_MapParam[param_num] = param;
		param->Init();
	}
	else
	{
		param->StartTimeOut();
		m_MapParam.RemoveKey(param_num);
	}
	SetupLine(item, param);
}

void MapParamDlg::RebuildTable()
{
	MutexWrap params_access(m_params_map_mutex);

	int key = 1, list_line = 0, index;
	char buf[50];
	CParam *param;
	CString str;

	m_table_list.DeleteAllItems();

	while (list_line < m_MapParamDefault.GetCount())
	{
		if (m_MapParamDefault.Lookup(key, param))
		{
			itoa(key, buf, 10);
			index = m_table_list.InsertItem(list_line, buf);
			m_table_list.SetItemData(index, key);
			SetupLine(index, param);
			list_line++;
		}
		key++;
	}
	m_table_list.SetColumnWidth(2, WIDTH_DATA_COL + ((m_table_list.GetCountPerPage() < m_table_list.GetItemCount()) ? 0 : 16));
}

void MapParamDlg::OnButtonChange() 
{
	ChooseChannel(m_table_list.GetCurItem());
}

void MapParamDlg::OnDblClickListTable(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnButtonChange();
	*pResult = 0;
}

LRESULT MapParamDlg::OnListSelectionChanged(WPARAM w, LPARAM l)
{
	int mark = m_table_list.GetCurItem();
	DWORD param_num = m_table_list.GetItemData(mark);
	SetupLineColor(m_prev_selection);
	SetupLineColor(mark);
	m_prev_selection = mark;
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(param_num > MAX_PARAM);
    return true;
}

void MapParamDlg::OnButtonDelete() 
{
	int mark = m_table_list.GetCurItem();
	DWORD param_num = m_table_list.GetItemData(mark);
	if (param_num <= MAX_PARAM)
	{
		MessageBox("Нельзя удалять зарезервированные параметры", "Параметр не будет удален");
		return;
	}
	if (MessageBox("Вы действительно хотите удалить параметр?", "Удалить параметр?", MB_YESNO) == IDYES)
	{
		{
			MutexWrap params_access(m_params_map_mutex); 
			CParam* param_to_delete;
			if (!m_MapParamDefault.Lookup(param_num, param_to_delete))
				return;
			pApp->SendMessageToActiveView(PM_DELETE_EXT_PARAM, param_num, 0);
			m_MapParamDefault.RemoveKey(param_num);
			m_MapParam.RemoveKey(param_num);
			delete param_to_delete;
			m_table_list.SetCurItem(mark - 1);
			m_prev_selection = m_table_list.GetCurItem();
		}
		RebuildTable();
	}
}

void MapParamDlg::OnButtonAdd() 
{
	CParam *param = new CParam(), *other;
	param->sName = "Новый параметр";
	param->m_channel_num = -1;

	for (param->m_num_par = MAX_PARAM + 1; param->m_num_par < 10000; param->m_num_par++)
		if (!m_MapParamDefault.Lookup(param->m_num_par, other))
			break;

	NewCorrespondenceDlg dlg(param, true);
	if (dlg.DoModal() == IDOK)
	{
		param->Init();
		m_MapParamDefault[param->m_num_par] = param; 
		if (param->bParamActive)
			m_MapParam[param->m_num_par] = param;
		RebuildTable();
	}
}

void MapParamDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
