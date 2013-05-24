// DlgSelectCalibrParam.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"
#include "DlgSelectChannel.h"

#include <SERV.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DlgSelectChannel::DlgSelectChannel(CString name, bool allow_convertor, CWnd* pParent /*=NULL*/)
	: CDialog(DlgSelectChannel::IDD, pParent), m_name(name), m_allow_convertor(allow_convertor)
{
	//{{AFX_DATA_INIT(DlgSelectChannel)
	//}}AFX_DATA_INIT
	m_convertor_id = -1;
	m_channel_id = -1;
}


void DlgSelectChannel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgSelectChannel)
	DDX_Control(pDX, IDC_TREE_CHANNELS, m_channels_tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSelectChannel, CDialog)
	//{{AFX_MSG_MAP(DlgSelectChannel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CHANNELS, OnSelchangingTreeDevices)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HTREEITEM DlgSelectChannel::CheckDataOfChilds(HTREEITEM first_child, DWORD data)
{
	HTREEITEM child = first_child, sub_child, res;
	DWORD item_data;
	while (child != NULL)
	{
		item_data = m_channels_tree.GetItemData(child);
		if (item_data == data)
			return child;
		sub_child = m_channels_tree.GetChildItem(child);
		if (sub_child != NULL)
		{
			res = CheckDataOfChilds(sub_child, data);
			if (res != NULL)
				return res;
		}
		child = m_channels_tree.GetNextItem(child, TVGN_NEXT);
	}
	return NULL;
}

BOOL DlgSelectChannel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(m_name);

	m_image_list.Create(16, 16, ILC_COLOR4, 0, 2);
	m_bitmap_cfg.LoadBitmap(IDB_BITMAP_CFG);
	m_bitmap_device.LoadBitmap(IDB_BITMAP_DEVICE);
	m_bitmap_channel.LoadBitmap(IDB_BITMAP_CHANNEL);
	m_image_list.Add(&m_bitmap_cfg, 0x01);
	m_image_list.Add(&m_bitmap_device, 0x01);
	m_image_list.Add(&m_bitmap_channel, 0x01);

	m_channels_tree.SetImageList(&m_image_list, TVSIL_NORMAL);

	CString cs, cs1;

	HTREEITEM branch_item, device_item, channel_item;

	SERV_Device* device = &m_convertor_device;
	SERV_Channel* channel;
	SERV_ChannelsMap::iterator end_s, current_s;
/*
	if (m_allow_convertor)
	{
		branch_item = m_channels_tree.InsertItem("Каналы конвертора", 0, 0);
		m_channels_tree.SetItemData(branch_item, 0x00FFFF);
		current_s = device->GetChannelsBegin();
		end_s = device->GetChannelsEnd();
		while (current_s != end_s)
		{
			channel = current_s->second;
			channel_item = m_channels_tree.InsertItem(channel->GetInfo(), 2, 2, branch_item);
			m_channels_tree.SetItemData(channel_item, channel->GetUniqueNumber());
			current_s++;
		}
	}
	*/
	branch_item = m_channels_tree.InsertItem("Внутренняя конфигурация", 0, 0);
	m_channels_tree.SetItemData(branch_item, 0x01FFFF);
	SERV_DevicesMap::iterator current_d = SERV_current_device_cfg->GetDevicesBegin();
	while (current_d != SERV_current_device_cfg->GetDevicesEnd())
	{
		device = current_d->second;
		device_item = m_channels_tree.InsertItem(device->GetInfo(), 1, 1, branch_item);
		m_channels_tree.SetItemData(device_item, 0x0100FF | (device->GetAddr() << 8));
		current_s = device->GetChannelsBegin();
		end_s = device->GetChannelsEnd();
		while (current_s != end_s)
		{
			channel = current_s->second;
			channel_item = m_channels_tree.InsertItem(channel->GetInfo(), 2, 2, device_item);
			m_channels_tree.SetItemData(channel_item, 0x010000 | channel->GetUniqueNumber());
			current_s++;
		}
		m_channels_tree.Expand(device_item, TVE_EXPAND);
		current_d++;
	}

	DWORD data = 0xFFFFFF;
	if (m_convertor_id != -1)
	{
		if (m_channel_id != -1)
			data = (m_convertor_id<<16) | m_channel_id;
		else
			data = (m_convertor_id<<16) | 0xFFFF;

		HTREEITEM sel_item = CheckDataOfChilds(m_channels_tree.GetNextItem(NULL, TVGN_CHILD), data);
		if (sel_item != NULL)
			m_channels_tree.SelectItem(sel_item);
	}

	UpdateData(false);
	
	return TRUE;
}

void DlgSelectChannel::OnOK() 
{
	HTREEITEM item = m_channels_tree.GetSelectedItem();
	int data = 0xFF;
	if (item != NULL)
		data = m_channels_tree.GetItemData(item);
	if ((data & 0x0000FF) != 0xFF)
	{
		m_convertor_id = (data & 0xFF0000)>>16;
		m_channel_id = data & 0x00FFFF;
	}
	CDialog::OnOK();
}

void DlgSelectChannel::OnSelchangeListSelectCalibrParam() 
{
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void DlgSelectChannel::OnSelchangingTreeDevices(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM item = m_channels_tree.GetSelectedItem();
	int data = 0xFF;
	if (item != NULL)
		data = m_channels_tree.GetItemData(item);
	GetDlgItem(IDOK)->EnableWindow((data & 0x0000FF) != 0xFF);
	*pResult = 0;
}