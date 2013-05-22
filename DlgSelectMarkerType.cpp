// DlgSelectMarkerType.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "DlgSelectMarkerType.h"
#include "DlgEditMarkerType.h"
#include <CHECK.h>
#include <DB.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSelectMarkerType dialog


DlgSelectMarkerType::DlgSelectMarkerType(bool for_select, CWnd* pParent /*=NULL*/)
	: CDialog(DlgSelectMarkerType::IDD, pParent),
	m_for_select (for_select)
{
	//{{AFX_DATA_INIT(DlgSelectMarkerType)
	//}}AFX_DATA_INIT
}


void DlgSelectMarkerType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgSelectMarkerType)
	DDX_Control(pDX, IDC_LIST_MARKER_TYPES, m_marker_types_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgSelectMarkerType, CDialog)
	//{{AFX_MSG_MAP(DlgSelectMarkerType)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_LBN_SELCHANGE(IDC_LIST_MARKER_TYPES, OnSelchangeListMarkerTypes)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL DlgSelectMarkerType::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (!m_for_select)
		SetDlgItemText(IDC_BUTTON_SELECT, "Редактировать");

	int i;
	map<int, BS_MarkerType>::iterator current = DB_marker_types.begin();
	while (current != DB_marker_types.end())
	{
		i = m_marker_types_list.AddString(current->second.m_name);
		m_marker_types_list.SetItemData(i, current->second.m_index);
		current++;
	}
	return TRUE;
}

void DlgSelectMarkerType::OnButtonSelect() 
{
	if (!m_for_select) 
	{
		int list_index = m_marker_types_list.GetCurSel();
		CHECK(list_index >= 0);
		int marker_type_index = m_marker_types_list.GetItemData(list_index);
		CHECK(marker_type_index > 0);
		CHECK(DB_marker_types.find(marker_type_index) != DB_marker_types.end());
		BS_MarkerType* mt = &DB_marker_types[marker_type_index];
		DlgEditMarkerType dlg(mt);
		if (dlg.DoModal() == IDOK)
		{
			m_marker_types_list.InsertString(list_index, mt->m_name);
			m_marker_types_list.DeleteString(list_index + 1);
			m_marker_types_list.SetCurSel(list_index);
		}
	}
	else
		CDialog::OnOK();
}

void DlgSelectMarkerType::OnButtonAdd() 
{
	BS_MarkerType new_marker_type;
	DlgEditMarkerType dlg(&new_marker_type);
	if (dlg.DoModal() == IDOK)
	{
		int list_index = m_marker_types_list.AddString(new_marker_type.m_name);
		m_marker_types_list.SetItemData(list_index, new_marker_type.m_index);
		m_marker_types_list.SetCurSel(list_index);
		OnSelchangeListMarkerTypes();
	}
}

void DlgSelectMarkerType::OnSelchangeListMarkerTypes() 
{
	BOOL ena = (m_marker_types_list.GetCurSel() >= 0);
	GetDlgItem(IDC_BUTTON_SELECT)->EnableWindow(ena);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(ena);
}

void DlgSelectMarkerType::OnButtonDelete() 
{	
	int list_index = m_marker_types_list.GetCurSel();
	CHECK(list_index >= 0);
	int marker_type_index = m_marker_types_list.GetItemData(list_index);
	CHECK(marker_type_index > 0);
	CHECK(DB_marker_types.find(marker_type_index) != DB_marker_types.end());

	if (MessageBox("Удалить тип маркера?", "Удаление типа маркера", MB_YESNO) != IDYES)
		return;

	DB_marker_types.erase(marker_type_index);
	m_marker_types_list.DeleteString(list_index);
	OnSelchangeListMarkerTypes();
}
