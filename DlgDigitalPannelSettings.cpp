// DlgDigitalPannelSettings.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "DlgDigitalPannelSettings.h"


// DlgDigitalPannelSettings dialog

IMPLEMENT_DYNAMIC(DlgDigitalPannelSettings, CDialog)

DlgDigitalPannelSettings::DlgDigitalPannelSettings(CWnd* pParent /*=NULL*/)
	: CDialog(DlgDigitalPannelSettings::IDD, pParent)
{
	m_cmd_num = -1;
}

DlgDigitalPannelSettings::~DlgDigitalPannelSettings()
{
}

void DlgDigitalPannelSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_ind_color);
}

BEGIN_MESSAGE_MAP(DlgDigitalPannelSettings, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_PARAM, &DlgDigitalPannelSettings::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDOK, &DlgDigitalPannelSettings::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &DlgDigitalPannelSettings::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_STATIC_COLOR, OnStaticColor)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_FONT, &DlgDigitalPannelSettings::OnBnClickedButtonChooseFont)
END_MESSAGE_MAP()

BOOL DlgDigitalPannelSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_STATIC_NAME_PAR)->SetWindowText(m_param_name + (TXT(" (%d)") << m_param_num));
	m_ind_color.SetStateBkColor(STATE_UNKNOWN, m_color);
	m_ind_color.SetStateText(STATE_UNKNOWN, "");
	m_ind_color.SetState(STATE_UNKNOWN);

/*	LOGFONT lf = {
		-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
		OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	m_font = lf;
*/
	return TRUE;
}

void DlgDigitalPannelSettings::OnBnClickedCancel()
{
	m_cmd_num = 0;
	OnCancel();
}

void DlgDigitalPannelSettings::OnBnClickedOk()
{
	m_cmd_num = 1;
	OnOK();
}

void DlgDigitalPannelSettings::OnBnClickedButtonDelete()
{
	m_cmd_num = 2;
	CDialog::OnOK();
}

void DlgDigitalPannelSettings::OnStaticColor() 
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_color = dlg.GetColor();
		m_ind_color.SetStateBkColor(STATE_UNKNOWN, m_color);
	}
}

void DlgDigitalPannelSettings::OnBnClickedButtonChooseFont()
{
	CFontDialog dlg;
	LOGFONT lf = m_font;
	dlg.m_cf.lpLogFont = &lf;
	dlg.m_cf.rgbColors = m_color;

	dlg.m_cf.Flags = CF_ENABLEHOOK | CF_SCREENFONTS | CF_ANSIONLY | CF_INITTOLOGFONTSTRUCT | CF_NOVERTFONTS |
					CF_NOSCRIPTSEL/* | CF_NOFACESEL | CF_NOSTYLESEL*/;
	if (dlg.DoModal() == IDOK) 
	{
		dlg.GetCurrentFont(&m_font);
		m_color = dlg.m_cf.rgbColors;
	}
	m_ind_color.SetStateBkColor(STATE_UNKNOWN, m_color);
}
