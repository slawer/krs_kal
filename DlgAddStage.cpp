#include "stdafx.h"
#include "KRS.h"
#include "STAGES.h"
#include "DlgAddStage.h"

IMPLEMENT_DYNAMIC(DlgAddStage, CDialog)

DlgAddStage::DlgAddStage(LPARAM lp, CWnd* pParent /*=NULL*/):
	CDialog(DlgAddStage::IDD, pParent), m_selected_stage_template(-1)
{
	m_left = lp>>16;
	m_top = lp & 0xFFFF;
	m_bind_to_mouse = !(lp == 0 || lp == -1);
}

DlgAddStage::~DlgAddStage()
{
}

void DlgAddStage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STAGES, m_list_stages);
}

BEGIN_MESSAGE_MAP(DlgAddStage, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_STAGES, OnSelchangeListStages)
	ON_MESSAGE(WM_USER+1, OnEnforceFlose)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

void DlgAddStage::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);
	
	if (lpwndpos != NULL && lpwndpos->flags == (SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE))
	{
		PostMessage(WM_USER+1, 0, NULL);
	}
}

BOOL DlgAddStage::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (STAGES_stage_templates.size() == 0) return false;

	int index;

	STAGES_StageTemplates::iterator current = STAGES_stage_templates.begin();
	while (current != STAGES_stage_templates.end())
	{
		index = m_list_stages.AddString(current->second->m_name);
		m_list_stages.SetItemData(index, current->second->m_stage_template_num);
		current++;
	}
	if (m_selected_stage_template != -1)
		m_list_stages.SetCurSel(m_selected_stage_template - 1);
	int vert_count = min(30, m_list_stages.GetCount());
	if (vert_count == 0)
	{
		EndDialog(IDABORT);
		return FALSE;
	}
	int h = max(20, 2 + m_list_stages.GetItemHeight(0) * vert_count), w = 200;
	m_list_stages.SetWindowPos(NULL, 0, 0, w, h, SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER);
	CRect rect;
	AfxGetMainWnd()->GetWindowRect(&rect);
	/*int x = (rect.right - rect.left - w)/2;
	int y = (rect.bottom - rect.top - h)/2;
	SetWindowPos(NULL, 100, 100, w, h, SWP_NOZORDER | SWP_NOMOVE);
	*/
	int x = m_left - w/2;
	int y = m_top - 10;
	x = max(rect.left, min(rect.right - w, x));
	y = max(rect.top + 10, min(rect.bottom - 10 - h, y));
	DWORD flag = m_bind_to_mouse?0:SWP_NOMOVE;
	SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER | flag);
	return TRUE;
}

LRESULT DlgAddStage::OnEnforceFlose(WPARAM, LPARAM)
{
	EndDialog(m_selected_stage_template == -1 ? IDCANCEL : IDOK);
	return 0;
}

void DlgAddStage::OnOK()
{
	if (m_selected_stage_template == -1)
		CDialog::OnCancel();
	else
		CDialog::OnOK();
}

void DlgAddStage::OnSelchangeListStages()
{
	int selected = m_list_stages.GetCurSel();
	if (selected >= 0) m_selected_stage_template = m_list_stages.GetItemData(selected);
	OnOK();
}
