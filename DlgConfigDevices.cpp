#include "stdafx.h"
#include "krs.h"
#include "DlgConfigDevices.h"

#include <direct.h>
#include <CHECK.h>
#include <SERV.h>
#include "DlgEditDevice.h"
#include "DlgEditSensor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgConfigDevices::DlgConfigDevices(CWnd* pParent /*=NULL*/)
	: CDialog(DlgConfigDevices::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgConfigDevices)
	//}}AFX_DATA_INIT
}

void DlgConfigDevices::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgConfigDevices)
	DDX_Control(pDX, IDC_TREE_DEVICES, m_devices_tree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgConfigDevices, CDialog)
	//{{AFX_MSG_MAP(DlgConfigDevices)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_DEVICES, OnSelchangingTreeDevices)
	ON_BN_CLICKED(IDC_BUTTON_ADD_DEVICE, OnButtonAddDevice)
	ON_BN_CLICKED(IDC_BUTTON_ADD_SENSOR, OnButtonAddChannel)
	ON_BN_CLICKED(IDC_SAVE_CONFIG, OnSaveConfig)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_LOAD_CONFIG, OnLoadConfig)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void DlgConfigDevices::ResetCfgName()
{
	CString path = m_tmp_cfg->GetFileName();
	CString name;
	int pos = path.Find(KRS_root_path);
	if (pos == 0)
	{
		name = path.Right(path.GetLength() - KRS_root_path.GetLength() - 1);
	}
	else
		name = path;
	SetDlgItemText(IDC_EDIT_CURRENT_CONFIG, name);
}

void DlgConfigDevices::RebuildTree()
{
	ResetCfgName();

	m_devices_tree.DeleteAllItems();

	HTREEITEM device_item, channel_item;
	SERV_Device* device;
	SERV_Channel* channel;
	SERV_ChannelsMap::iterator end_s, current_s;
	SERV_DevicesMap::iterator current_d = m_tmp_cfg->GetDevicesBegin();
	while (current_d != m_tmp_cfg->GetDevicesEnd())
	{
		device = current_d->second;
		device_item = m_devices_tree.InsertItem(device->GetInfo(), 0, 0);
		m_devices_tree.SetItemData(device_item, device->GetAddr());
		current_s = device->GetChannelsBegin();
		end_s = device->GetChannelsEnd();
		while (current_s != end_s)
		{
			channel = current_s->second;
			channel_item = m_devices_tree.InsertItem(channel->GetInfo(), 1, 1, device_item);
			m_devices_tree.SetItemData(channel_item, channel->GetUniqueNumber());
			current_s++;
		}
		//m_devices_tree.Expand(device_item, TVE_EXPAND);
		current_d++;
	}
}

BOOL DlgConfigDevices::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_image_list.Create(16, 16, ILC_COLOR4, 0, 2);
	m_bitmap_device.LoadBitmap(IDB_BITMAP_DEVICE);
	m_bitmap_channel.LoadBitmap(IDB_BITMAP_CHANNEL);
	m_image_list.Add(&m_bitmap_device, 0x01);
	m_image_list.Add(&m_bitmap_channel, 0x01);

	m_devices_tree.SetImageList(&m_image_list, TVSIL_NORMAL);

	{
		CHECK(SERV_current_device_cfg != NULL);
		MutexWrap cfg_access(SERV_current_device_cfg_mutex);
		m_tmp_cfg = SERV_current_device_cfg->MakeCopy();
	}

	RebuildTree();
	return TRUE;
}

void DlgConfigDevices::OnCancel()
{
	CHECK(m_tmp_cfg != NULL);
	delete m_tmp_cfg;
	CDialog::OnCancel();
}

void DlgConfigDevices::OnOK() 
{	
	MutexWrap cfg_access(SERV_current_device_cfg_mutex);
	CHECK(SERV_current_device_cfg != NULL);
	CHECK(m_tmp_cfg != NULL);
	SERV_DeviceCfg* prev_cfg = SERV_current_device_cfg;
	SERV_current_device_cfg = m_tmp_cfg;
	delete prev_cfg;
	SERV_current_device_cfg->Save();
	CDialog::OnOK();
}

void DlgConfigDevices::EnableEditDelete(BOOL enable)
{
	GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(enable);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(enable);	
}

void DlgConfigDevices::OnSelchangingTreeDevices(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	EnableEditDelete(TRUE);
	*pResult = 0;
}

void DlgConfigDevices::OnButtonAddDevice() 
{
	SERV_Device* new_device = new SERV_Device("Устройство", 0, 1000, m_tmp_cfg);

	DlgEditDevice dlg(new_device);
	if (dlg.DoModal() == IDCANCEL)
	{
		delete new_device;
		return;
	}

	HTREEITEM device_item = m_devices_tree.InsertItem(new_device->GetInfo(), 0, 0);
	m_devices_tree.SetItemData(device_item, new_device->GetAddr());
	m_devices_tree.SelectItem(device_item);
}

void DlgConfigDevices::OnButtonAddChannel() 
{
	HTREEITEM device_item = m_devices_tree.GetSelectedItem();
	if (device_item == 0)
	{
		MessageBox("Не выбрано устройство", "Добавление канала невозможно");
		return;
	}
	DWORD data = m_devices_tree.GetItemData(device_item);
	byte dev_addr;
	if ((data & 0xFF00) != 0)
	{
		device_item = m_devices_tree.GetParentItem(device_item);
		dev_addr = (BYTE)(data >> 8);
	}
	else
		dev_addr = (BYTE)data;

	SERV_Device* device = m_tmp_cfg->GetDevice(dev_addr);
	CHECK(device != NULL);
	byte offset = device->FindFreeOffset();
	if (offset == 0xFF)
	{
		MessageBox("Устройство не имеет свободных смещений данных", "Невозможно создать канал");
		return;
	}

	SERV_Channel* new_channel = new SERV_Channel("Канал", device, offset);

	DlgEditSensor dlg(new_channel);
	if (dlg.DoModal() == IDCANCEL)
	{
		device->RemoveChannel(new_channel);
		return;
	}

	HTREEITEM channel_item = m_devices_tree.InsertItem(new_channel->GetInfo(), 1, 1, device_item);
	m_devices_tree.SetItemData(channel_item, new_channel->GetUniqueNumber());
	m_devices_tree.SelectItem(channel_item);
}

void DlgConfigDevices::OnButtonEdit() 
{
	HTREEITEM node = m_devices_tree.GetSelectedItem();
	WORD data = (WORD)(m_devices_tree.GetItemData(node));

	if ((data & 0xFF00) != 0)
	{
		SERV_Channel* channel = m_tmp_cfg->GetChannel(data);
		CHECK(channel != NULL);
		DlgEditSensor dlg(channel);
		if (dlg.DoModal() == IDOK)
		{
			m_devices_tree.SetItemText(node, channel->GetInfo());
			m_devices_tree.SetItemData(node, channel->GetUniqueNumber());
		}
	}
	else
	{
		SERV_Device* device = m_tmp_cfg->GetDevice((BYTE)(data));
		byte prev_addr = device->GetAddr();
		CHECK(device != NULL);
		DlgEditDevice dlg(device);
		if (dlg.DoModal() == IDOK)
		{
			if (prev_addr != device->GetAddr())
				RebuildTree();
			else
				m_devices_tree.SetItemText(node, device->GetInfo());
		}
	}
}

void DlgConfigDevices::OnButtonDelete() 
{
	HTREEITEM node = m_devices_tree.GetSelectedItem();
	DWORD data = m_devices_tree.GetItemData(node);

	if ((data & 0xFF00) != 0)
	{
		SERV_Channel* channel = m_tmp_cfg->GetChannel(data);
		CHECK(channel != NULL);
		channel->GetDevice()->RemoveChannel(channel);
	}
	else
	{
		SERV_Device* device = m_tmp_cfg->GetDevice((BYTE)(data&0x000F));
		CHECK(device != NULL);
		delete device;
	}
	HTREEITEM next = m_devices_tree.GetNextItem(node, TVGN_NEXTVISIBLE);
	if (next != NULL && next != node)
		m_devices_tree.SelectItem(next);
	else
	{
		EnableEditDelete(FALSE);
	}
	m_devices_tree.DeleteItem(node);
}

void DlgConfigDevices::OnLoadConfig() 
{
	CFileDialog dlg(TRUE, NULL, m_tmp_cfg->GetFileName(), OFN_HIDEREADONLY, "Файлы конфигурации(*.cfg)|*.cfg||");
	if (dlg.DoModal() != IDOK)
		return;
	SERV_DeviceCfg* tmp2 = new SERV_DeviceCfg;
	if (tmp2->Load(dlg.GetPathName()))
	{
		SERV_DeviceCfg* to_del = m_tmp_cfg;
		m_tmp_cfg = tmp2;
		delete to_del;
		RebuildTree();
		AfxGetApp()->WriteProfileString("DEVICES", "Config file name", dlg.GetPathName());
	}
}

void DlgConfigDevices::OnSaveConfig() 
{
	CHECK(m_tmp_cfg != NULL);
	CFileDialog dlg(FALSE, "cfg", m_tmp_cfg->GetFileName(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Файлы конфигурации(*.cfg)|*.cfg||");
	if (dlg.DoModal() != IDOK)
		return;
	m_tmp_cfg->Save(dlg.GetPathName());
	ResetCfgName();
}