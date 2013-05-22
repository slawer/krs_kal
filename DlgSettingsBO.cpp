#include "stdafx.h"
#include "krs.h"
#include "DlgSettingsBO.h"
#include "SERV.h"
#include "DlgEditBO.h"
#include "LOG_V0.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgSettingsBO::DlgSettingsBO(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSettingsBO::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgSettingsBO)
	//}}AFX_DATA_INIT
}

void DlgSettingsBO::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgSettingsBO)
	DDX_Control(pDX, IDC_LIST_BO, m_list_bo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgSettingsBO, CDialog)
	//{{AFX_MSG_MAP(DlgSettingsBO)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_LBN_SELCHANGE(IDC_LIST_BO, OnSelchangeListBo)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PLUS_BO_TIME, &DlgSettingsBO::OnBnClickedPlusBoTime)
	ON_BN_CLICKED(IDC_MINUS_BO_TIME, &DlgSettingsBO::OnBnClickedMinusBoTime)
	ON_BN_CLICKED(IDOK, &DlgSettingsBO::OnBnClickedOk)
END_MESSAGE_MAP()

void DlgSettingsBO::OnOK()
{
	MutexWrap mutex_BO_access(SERV_mutex_BO);

	SERV_ClearListBO(SERV_indication_blocks);
	SERV_BO* copy;
	SERV_BOs::iterator current = SERV_tmp_indication_blocks.begin();
	while (current != SERV_tmp_indication_blocks.end())
	{
		copy = (*current)->MakeCopy();
		SERV_indication_blocks.push_back(copy);
		current++;
	}

	if (!SERV_SaveConfigBO(KRS_root_path))
		LOG_V0_AddMessage(LOG_V0_MESSAGE, "Ошибка при записи файла конфигурации БО");

	CDialog::OnOK();
}

BOOL DlgSettingsBO::OnInitDialog()
{
	CDialog::OnInitDialog();

	SERV_ClearListBO(SERV_tmp_indication_blocks);
	SERV_BO* copy;
	SERV_BOs::iterator current = SERV_indication_blocks.begin();
	while (current != SERV_indication_blocks.end())
	{
		copy = (*current)->MakeCopy();
		SERV_tmp_indication_blocks.push_back(copy);
		m_list_bo.AddString(copy->GetInfo());
		current++;
	}
	m_bo_time = KRS_send_to_bo_interval;
	OutputBOTime();

	return TRUE;
}

void DlgSettingsBO::OnButtonAdd()
{
	SERV_BO *new_bo = new SERV_BO();
	DlgEditBO dlg(new_bo, NULL);
	if (dlg.DoModal() == IDOK)
	{
		SERV_tmp_indication_blocks.push_back(new_bo);
		m_list_bo.AddString(new_bo->GetInfo());
	}
	else
		delete new_bo;
}

void DlgSettingsBO::OnButtonEdit()
{
	int ind = m_list_bo.GetCurSel(), ind_tmp = 0;
	SERV_BOs::iterator current = SERV_tmp_indication_blocks.begin();
	while (current != SERV_tmp_indication_blocks.end())
	{
		if (ind_tmp == ind)
		{
			SERV_BO* copy = (*current)->MakeCopy();
			DlgEditBO dlg(copy, *current);
			if (dlg.DoModal() == IDOK)
			{
				SERV_tmp_indication_blocks.insert(current, copy);
				SERV_tmp_indication_blocks.erase(current);
				m_list_bo.InsertString(ind, copy->GetInfo());
				m_list_bo.DeleteString(ind + 1);
				m_list_bo.SetCurSel(ind);
			}
			else
				delete copy;
			break;
		}
		ind_tmp++;
		current++;
	}
}

void DlgSettingsBO::OnButtonDelete()
{
	if (MessageBox("Удалить выделенный блок отображения?", "Удаление блока отображения", MB_YESNO) == IDNO)
		return;

	int ind = m_list_bo.GetCurSel(), ind_tmp = 0;
	SERV_BOs::iterator current = SERV_tmp_indication_blocks.begin();
	while (current != SERV_tmp_indication_blocks.end())
	{
		if (ind_tmp == ind)
		{
			SERV_tmp_indication_blocks.erase(current);
			m_list_bo.DeleteString(ind);
			m_list_bo.SetCurSel(-1);
			OnSelchangeListBo();
			break;
		}
		ind_tmp++;
		current++;
	}
}

void DlgSettingsBO::OnSelchangeListBo()
{
	BOOL ena = TRUE;
	int ind = m_list_bo.GetCurSel();
	if (ind == -1)
		ena = FALSE;
	GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(ena);
}

void DlgSettingsBO::OutputBOTime()
{
	SetDlgItemInt(IDC_STATIC_BO_TIME, m_bo_time);
}

void DlgSettingsBO::OnBnClickedPlusBoTime()
{
	m_bo_time += 100;
	OutputBOTime();
}

void DlgSettingsBO::OnBnClickedMinusBoTime()
{
	m_bo_time -= 100;
	m_bo_time = max(300, m_bo_time);
	OutputBOTime();
}

void DlgSettingsBO::OnBnClickedOk()
{
	if (m_bo_time != KRS_send_to_bo_interval)
	{
		KRS_send_to_bo_interval = m_bo_time;
		pFrm->KillTimer(3);
		pFrm->KillTimer(4);
		pFrm->SetTimer(3, 1000, NULL);
	}
	OnOK();
}
