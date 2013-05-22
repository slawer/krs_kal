#include "stdafx.h"
#include "krs.h"
#include "DlgEditDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgEditDevice::DlgEditDevice(SERV_Device* device, CWnd* pParent /*=NULL*/)
	: CDialog(DlgEditDevice::IDD, pParent),
	m_device(device)
{
	//{{AFX_DATA_INIT(DlgEditDevice)
	m_string_name = _T(m_device->GetName());
	m_to_interval = m_device->m_timeout_interval;
	//}}AFX_DATA_INIT
}

void DlgEditDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEditDevice)
	DDX_Control(pDX, IDC_COMBO_ADDR, m_combo_addr);
	DDX_Text(pDX, IDC_EDIT_NAME, m_string_name);
	DDX_Text(pDX, IDC_EDIT_TIMEOUT, m_to_interval);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgEditDevice, CDialog)
	//{{AFX_MSG_MAP(DlgEditDevice)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void DlgEditDevice::OnOK() 
{
	if (!UpdateData())
		return;

	if (m_string_name.IsEmpty())
	{
		MessageBox("Устройство не имеет имени", "Невозможно создать устройство");
		return;
	}
	int addr = m_combo_addr.GetCurSel();
	if (addr == 0)
	{
		MessageBox("Устройство не имеет адреса", "Невозможно создать устройство");
		return;
	}
	SERV_Device* old = m_device->GetCfg()->GetDevice(addr);
	if (old != NULL && old != m_device)
	{
		MessageBox("Устройство с таким адресом уже существует", "Невозможно создать устройство");
		return;
	}
	m_device->SetName(m_string_name);
	m_device->SetAddr(addr);
	m_device->m_timeout_interval = m_to_interval;
	CDialog::OnOK();
}

BOOL DlgEditDevice::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_combo_addr.SetCurSel(m_device->GetAddr());
	return TRUE;
}
