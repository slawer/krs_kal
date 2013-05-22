// DlgEditSensor.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "DlgEditSensor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgEditSensor::DlgEditSensor(SERV_Channel* channel, CWnd* pParent /*=NULL*/)
	: CDialog(DlgEditSensor::IDD, pParent),
	m_channel(channel)
{
	m_channel_name = _T(m_channel->GetName());
	m_data_offset = m_channel->GetDataOffset();
	m_data_len = m_channel->GetDataLength();
	m_use_voltage = m_channel->m_use_voltage;
	m_dsp0 = 0;
	m_dsp1 = m_channel->m_use_voltage?m_channel->m_dsp1:0xFFFF;
	m_v0 = m_channel->m_use_voltage?m_channel->m_v0:0.0f;
	m_v1 = m_channel->m_use_voltage?m_channel->m_v1:1.0f;

	m_check_jumps = m_channel->m_check_jumps;
	m_jump_up = m_channel->m_jump_up;
	m_jump_down = m_channel->m_jump_down;

	m_channel_original = NULL;
	if (SERV_current_device_cfg)
	{
		m_channel_original = SERV_current_device_cfg->GetChannel(m_channel->GetUniqueNumber());
	}
}

void DlgEditSensor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEditSensor)
	DDX_Control(pDX, IDC_IND_VALUE, m_ind_value);
	DDX_Text(pDX, IDC_EDIT_NAME, m_channel_name);
	DDX_Text(pDX, IDC_EDIT_DATA_OFFSET, m_data_offset);
	DDX_Text(pDX, IDC_EDIT_DATA_LEN, m_data_len);
	DDX_Check(pDX, IDC_CHECK_USE_VOLTAGE, m_use_voltage);
	DDX_Text(pDX, IDC_EDIT_DSP0, m_dsp0);
	DDX_Text(pDX, IDC_EDIT_DSP1, m_dsp1);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_V0, m_v0);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_V1, m_v1);
	DDX_Check(pDX, IDC_CHECK_JUMPS, m_check_jumps);
	DDX_Text(pDX, IDC_EDIT_JUMP_UP, m_jump_up);
	DDX_Text(pDX, IDC_EDIT_JUMP_DOWN, m_jump_down);
	//}}AFX_DATA_MAP

	if (m_data_offset > 31)
	{
		MessageBox("Смещение должно быть в диаапазоне 0-31", "Введите корректное число");
		pDX->Fail();
		return;
	}
	if (m_data_len < 1 || m_data_len > 4)
	{
		MessageBox("Длина данных должна быть в диаапазоне 1-4", "Введите корректное число");
		pDX->Fail();
		return;
	}
}

BEGIN_MESSAGE_MAP(DlgEditSensor, CDialog)
	//{{AFX_MSG_MAP(DlgEditSensor)
	ON_BN_CLICKED(IDC_CHECK_USE_VOLTAGE, OnCheckUseVoltage)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void DlgEditSensor::OnOK() 
{
	if (!UpdateData())
		return;

	if (m_channel_name.IsEmpty())
	{
		MessageBox("Датчик не имеет имени", "Невозможно создать датчик");
		return;
	}

	if (m_channel->GetDataOffset() != m_data_offset) 
	{
		SERV_Device* device = m_channel->GetDevice();
		SERV_DeviceCfg* cfg = device->GetCfg();
		WORD new_uniq = SERV_Channel::GetUniqueNumber(device->GetAddr(), m_data_offset);
		if (cfg->GetChannel(new_uniq) != NULL)
		{
			MessageBox("Устройство уже имеет датчик на этом смещении", "Невозможно создать датчик");
			return;
		}
		m_channel->SetDataOffset(m_data_offset);
	}
	m_channel->SetDataLength(m_data_len);
	m_channel->SetName(m_channel_name);
	m_channel->m_use_voltage = (m_use_voltage == TRUE);
	m_channel->m_v0 = m_v0;
	m_channel->m_v1 = m_v1;
	m_channel->m_dsp1 = m_dsp1;
	m_channel->m_check_jumps = (m_check_jumps == TRUE);
	m_channel->m_jump_up = m_jump_up;
	m_channel->m_jump_down = m_jump_down;
	CDialog::OnOK();
}

void DlgEditSensor::OnCheckUseVoltage() 
{
	m_use_voltage = IsDlgButtonChecked(IDC_CHECK_USE_VOLTAGE);
	GetDlgItem(IDC_EDIT_V0)->EnableWindow(m_use_voltage);
	GetDlgItem(IDC_EDIT_V1)->EnableWindow(m_use_voltage);
	GetDlgItem(IDC_EDIT_DSP1)->EnableWindow(m_use_voltage);
}

BOOL DlgEditSensor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	OnCheckUseVoltage();
	m_ind_value.SetStateText(STATE_OFF, "ОТКЛ");
	m_ind_value.SetState(STATE_UNKNOWN);
	SetTimer(1, 100, NULL);
	return TRUE;
}

void DlgEditSensor::OnTimer(UINT nIDEvent) 
{
	static DWORD lv = 0;
	static CString str;
	if (nIDEvent == 1)
	{
		if (m_channel_original)
		{
			if (m_channel_original->IsTimedOut())
				m_ind_value.SetState(STATE_OFF);
			else
			{
				lv = (DWORD)m_channel_original->GetLastValue();
				if (m_last_value != lv)
				{
					m_last_value = lv;
					str.Format("%X", m_last_value);
					m_ind_value.SetStateText(STATE_ON, str);
				}
				m_ind_value.SetState(STATE_ON);
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}
