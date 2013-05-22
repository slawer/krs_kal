////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"
#include "ChangeParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChangeParam::CChangeParam(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeParam::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeParam)
	m_min_val_gr = 0.0f;
	m_max_val_gr = 0.0f;
	//}}AFX_DATA_INIT
	m_line_width = 1;
	m_line_color = RGB(255,0,0);
}

void CChangeParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeParam)
	DDX_Control(pDX, IDC_STATIC_COLOR, m_ind_color);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MIN_GR_VAL, m_min_val_gr);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MAX_GR_VAL, m_max_val_gr);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MIN_GR_VAL, m_min_val_gr);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MIN_VAL, m_min_val);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MAX_VAL, m_max_val);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_BLOCK_VAL, m_block_val);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_CRUSH_VAL, m_crush_val);
	DDX_Control(pDX, IDC_COMBO_CONTROL_TYPE, m_combo_control_type);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChangeParam, CDialog)
	//{{AFX_MSG_MAP(CChangeParam)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_PARAM, OnButtonDeleteParam)
	ON_BN_CLICKED(IDC_BUTTON_MINUS, OnButtonMinus)
	ON_BN_CLICKED(IDC_BUTTON_PLUS, OnButtonPlus)
	ON_BN_CLICKED(IDC_STATIC_COLOR, OnStaticColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CChangeParam::OnCancel() 
{
	num_cmd = 0;
	CDialog::OnCancel();
}

void CChangeParam::OnOK() 
{
	num_cmd = 1;
	m_type_of_control = m_combo_control_type.GetCurSel();
	CDialog::OnOK();
}

void CChangeParam::OnButtonDeleteParam() 
{
	num_cmd = 2;
	CDialog::OnOK();
}

BOOL CChangeParam::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_STATIC_NAME_PAR)->SetWindowText(CString(TXT("%s (%d)")<<name_param<<m_num_param));

	CButton* b;
	b = (CButton*)GetDlgItem(IDC_BUTTON_MINUS);
	b->SetIcon(KRS_app.LoadIcon(IDI_ICON_MINUS));
	b = (CButton*)GetDlgItem(IDC_BUTTON_PLUS);
	b->SetIcon(KRS_app.LoadIcon(IDI_ICON_PLUS));
	m_ind_color.SetStateBkColor(STATE_UNKNOWN, m_line_color);
	m_ind_color.SetStateText(STATE_UNKNOWN, "");
	m_ind_color.SetState(STATE_UNKNOWN);
	OutputThick();
	m_combo_control_type.SetCurSel(m_type_of_control);

	return TRUE;  
}

void CChangeParam::OnButtonMinus() 
{
	if (m_line_width <= 1)
		return;
	m_line_width--;
	OutputThick();
}

void CChangeParam::OnButtonPlus() 
{
	if (m_line_width >= 10)
		return;
	m_line_width++;
	OutputThick();
}

void CChangeParam::OutputThick()
{
	SetDlgItemInt(IDC_STATIC_THICK_LINE, m_line_width);
}

void CChangeParam::OnStaticColor() 
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_line_color = dlg.GetColor();
		m_ind_color.SetStateBkColor(STATE_UNKNOWN, m_line_color);
	}
}

CDlgAddParam::CDlgAddParam(LPARAM lp, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddParam::IDD, pParent), m_show_additional_cement_params(true)
{
	m_key = -1;
	m_left = lp>>16;
	m_top = lp & 0xFFFF;

	m_for_calibration = (m_top == 0 && m_left == 0);
	m_bind_to_mouse = !(lp == 0 || lp == -1);

	//{{AFX_DATA_INIT(CDlgAddParam)
	//}}AFX_DATA_INIT
}

void CDlgAddParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddParam)
	DDX_Control(pDX, IDC_LIST_ADD_PARAM, m_ListParam);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAddParam, CDialog)
	//{{AFX_MSG_MAP(CDlgAddParam)
	ON_LBN_SELCHANGE(IDC_LIST_ADD_PARAM, OnSelchangeListAddParam)
	ON_MESSAGE(WM_USER+1, OnEnforceFlose)
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgAddParam::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_MapParamCurent->GetCount() == 0) return false;

	int index;

	CString str;
	CParam *param;
	int count = 0, max_count = m_MapParamCurent->GetCount();
	for (int i = 1; i < 10000; i++)
	{
		if (m_MapParamCurent->Lookup(i, param))
		{
			if (param != NULL)
			{
				if (!m_show_additional_cement_params && param->m_num_par >= STAGES_START_ADDITIONAL_PARAM_NUM)
					;
				else
				if (!m_for_calibration || (param->m_channel_conv_num == 1 && param->m_channel_num > 0))
				{
					str.Format("%03d  %s", param->m_num_par, param->sName);
					index = m_ListParam.AddString(str);
					m_ListParam.SetItemData(index, i);
					if (m_key == i)
						m_ListParam.SetCurSel(index);
				}
			}
			count++;
			if (count == max_count)
				break;
		}
	}
	int vert_count = min(30, m_ListParam.GetCount());
	if (vert_count == 0)
	{
		EndDialog(IDABORT);
		return FALSE;
	}
	int h = max(20, 2 + m_ListParam.GetItemHeight(0) * vert_count), w = 200;
//    int h = 2 + m_ListParam.GetItemHeight(0) * vert_count, w = 200;
	m_ListParam.SetWindowPos(NULL, 0, 0, w, h, SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER);

	CRect rect;
	AfxGetMainWnd()->GetWindowRect(&rect);
	int x = m_left - w/2;
	int y = m_top - 10;
	x = max(rect.left, min(rect.right - w, x));
	y = max(rect.top + 10, min(rect.bottom - 10 - h, y));
	DWORD flag = m_bind_to_mouse?0:SWP_NOMOVE;
	SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER | flag);

	UpdateData(false);

	return TRUE;  
}

void CDlgAddParam::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);

	if (lpwndpos != NULL && lpwndpos->flags == (SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE))
	{
		PostMessage(WM_USER+1, 0, NULL);
	}
}

LRESULT CDlgAddParam::OnEnforceFlose(WPARAM, LPARAM)
{
	EndDialog(m_key == -1 ? IDCANCEL : IDOK);
	return 0;
}

void CDlgAddParam::OnOK() 
{
	if (m_key == -1)
		CDialog::OnCancel();
	else
		CDialog::OnOK();
}

void CDlgAddParam::OnSelchangeListAddParam() 
{
	int index = m_ListParam.GetCurSel();
	if (index >= 0)
		m_key = m_ListParam.GetItemData(index);
	OnOK();
}

////////////////////////////////////////////////////////////////////////////////
// end