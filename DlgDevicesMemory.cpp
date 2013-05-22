#include "stdafx.h"
#include "KRS.h"
#include "DlgDevicesMemory.h"
#include "DlgHardCfg.h"
#include "src\LOG_V0.h"

#include <io.h>
#include <sys/stat.h>

#define WM_MEM_MSG (WM_USER+1)

IMPLEMENT_DYNAMIC(DlgDevicesMemory, CDialog)

CString DlgDevicesMemory::m_cfg_name;

DlgDevicesMemory::DlgDevicesMemory(bool download, CWnd* pParent):
	CDialog(DlgDevicesMemory::IDD, pParent), m_download(download)
{
}

DlgDevicesMemory::~DlgDevicesMemory()
{
}

void DlgDevicesMemory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_DEVICES, m_progress_devices);
	DDX_Control(pDX, IDC_PROGRESS_MEMORY, m_progress_memory);
	DDX_Control(pDX, IDC_LIST_LOG, m_list_log);
}

BEGIN_MESSAGE_MAP(DlgDevicesMemory, CDialog)
	ON_MESSAGE(WM_MEM_MSG, OnMessage)
	ON_WM_TIMER()
END_MESSAGE_MAP()

byte g_mem[7*256 + 256];
int g_devices_counter;
CString g_cfg_path;
SERV_Device* g_prev_device = NULL;
bool g_errors_occured, g_cancel_as_abort;
int g_num_of_OK_devices;

BOOL DlgDevicesMemory::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_download?" Получение данных от устройств":" Восстановление конфигурации устройств");

	g_prev_device = NULL;
	g_errors_occured = false;
	g_num_of_OK_devices = 0;
	g_cancel_as_abort = true;

	if (m_download)
	{
		char str[2048];
		GetCurrentDirectory(2048, str);
		CString prev_dir = str;
		SetCurrentDirectory(KRS_root_path);
		CreateDirectory("CFG", NULL);
		SetCurrentDirectory(KRS_root_path + "\\CFG");
		CreateDirectory("tmp.files", NULL);
		g_cfg_path = KRS_root_path + "\\CFG\\tmp.files";	
		SetCurrentDirectory(g_cfg_path);
		WIN32_FIND_DATA file_data;
		HANDLE file_handle = FindFirstFile("*.*", &file_data);
		while (file_handle != INVALID_HANDLE_VALUE)
		{
			if ((file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				_chmod(file_data.cFileName, _S_IREAD | _S_IWRITE); // делаем файл не READ_ONLY
				remove(file_data.cFileName);
			}
			if (FindNextFile(file_handle, &file_data) == 0)	
				file_handle = INVALID_HANDLE_VALUE;
		}
		SetCurrentDirectory(prev_dir);
		AddLog("Загрузка конфигурации из устройств:");
	}
	else
	{
		g_cfg_path = KRS_root_path + "\\CFG\\" + m_cfg_name + ".files";
		AddLog(CString("Восстановление конфигурации \"") + m_cfg_name + "\":");
	}
	
	g_devices_counter = 0;
	m_progress_memory.SetRange32(0, 2047);

	MutexWrap cfg_access(SERV_current_device_cfg_mutex);
	int count = SERV_current_device_cfg->GetDevicesCount();
	if (count > 0)
	{	
		m_progress_devices.SetRange32(0, count);
		StartDevice(SERV_current_device_cfg->GetDevicesBegin()->second);
		SetTimer(1, 100, NULL);
	}
	else
	{
		AddLog("Внутренняя конфигурация пуста!");
		MessageBox("Внутренняя конфигурация пуста, работа с устройствами невозможна", "Нет ни одного устройства");
		EndDialog(IDCANCEL);
	}
	return TRUE;
}

void DlgDevicesMemory::OnOK()
{
}

void DlgDevicesMemory::OnCancel()
{
	if (!g_cancel_as_abort)
		CDialog::OnCancel();
	else
	if (MessageBox("Прервать процесс обмена данными с устройствами?", " Прерывание", MB_YESNO) == IDYES)
	{
		if (::IsWindow(m_hWnd)) 
		{
			SERV_StopMemoryHandling(true);
			CDialog::OnCancel();
		}
	}
}

void DlgDevicesMemory::StartDevice(SERV_Device* device)
{	
	g_prev_device = device;
	m_progress_devices.SetPos(g_devices_counter);
	m_progress_memory.SetPos(0);
	SetDlgItemText(IDC_STATIC_DEVICE_INFO, device->GetInfo());
	SetDlgItemText(IDC_STATIC_MEMORY_INFO, "0/2048");
	if (m_download)
		SERV_RequestMemory(m_hWnd, WM_MEM_MSG, device->GetAddr(), g_mem);
	else
	{
		CString name = g_cfg_path + (TXT("\\dev_%02X") << device->GetAddr());
		FILE *f = fopen(name, "rb");
		if (f)
		{
			fread(g_mem, 7*256, 1, f);
			fclose(f);
		}
		else
		{
			g_errors_occured = true;
			AddNegativeLog(CString("-- Ошибка чтения с диска конфигурации устройства \"") + device->GetInfo() + "\"!");
			StartNext();
			return;
		}
		SERV_UploadMemory(m_hWnd, WM_MEM_MSG, device->GetAddr(), g_mem);
	}
}

LRESULT DlgDevicesMemory::OnMessage(WPARAM wp, LPARAM lp)
{
	if (SERV_current_device_cfg == NULL)
	{
		EndDialog(IDCANCEL);
		return -1;
	}
	
	if (wp == 0)
	{
		MutexWrap cfg_access(SERV_current_device_cfg_mutex);

		if (g_prev_device != NULL)
		{
			if (m_download)
			{
				AddLog(CString("+ Конфигурация устройства \"") + g_prev_device->GetInfo() + "\" получена");
				
				CString name = g_cfg_path + (TXT("\\dev_%02X") << g_prev_device->GetAddr());
				FILE *f = fopen(name, "wb");
				if (f)
				{
					fwrite(g_mem, 7*256, 1, f);
					fclose(f);
					_chmod(name, _S_IREAD); 
					g_num_of_OK_devices++;
				}
				else
				{
					AddNegativeLog(CString("-- Ошибка записи на диск конфигурации устройства \"") + g_prev_device->GetInfo() + "\"!");
					g_errors_occured = true;
				}
			}
			else
			{
				AddLog(CString("+ Конфигурация устройства \"") + g_prev_device->GetInfo() + "\" восстановлена");
				g_num_of_OK_devices++;
			}
		}

		StartNext();
	}
	return +1;
}

bool SaveCfg(CString cfg_name)
{
	bool errors_occure = false;
	char str[2048];
	GetCurrentDirectory(2048, str);
	CString prev_dir = str;

	CString src_path = KRS_root_path + "\\CFG\\tmp.files";
	CString dst_path = KRS_root_path + "\\CFG\\" + cfg_name + ".files";
	SetCurrentDirectory(KRS_root_path + "\\CFG");
	CreateDirectory(cfg_name + ".files", NULL);
	SetCurrentDirectory(src_path);
	CString file_name;
	WIN32_FIND_DATA file_data;
	HANDLE file_handle = FindFirstFile("*.*", &file_data);
	while (file_handle != INVALID_HANDLE_VALUE)
	{
		if ((file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			file_name = file_data.cFileName;
			if (!CopyFile(src_path + "\\" + file_name, dst_path + "\\" + file_name, TRUE))
			{
				MessageBox(NULL, src_path + "\\" + file_name, "Ошибка копирования файла", MB_OK);
				errors_occure = true;
			}
		}
		if (FindNextFile(file_handle, &file_data) == 0)	
			file_handle = INVALID_HANDLE_VALUE;
	}
	file_handle = CreateFile(KRS_root_path + "\\CFG\\" + cfg_name,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY,
		NULL);
	CloseHandle(file_handle);

	SetCurrentDirectory(prev_dir);
	return !errors_occure;
}

void DlgDevicesMemory::StartNext()
{
	SERV_StopMemoryHandling(false);
	g_devices_counter++;
	if (SERV_current_device_cfg->GetDevicesCount() <= g_devices_counter)
	{
		KillTimer(1);
		
		m_progress_devices.SetPos(g_devices_counter);
		m_progress_memory.SetPos(2048);
		SetDlgItemText(IDC_STATIC_DEVICE_INFO, "опрос завершен");
		SetDlgItemText(IDC_STATIC_MEMORY_INFO, "работа завершена");

		if (m_download && g_num_of_OK_devices > 0)
		{
			DlgHardCfg dlg(true);
			if (dlg.DoModal() == IDOK)
			{
				AddLog(CString("Сохранение конфигурации \"") + dlg.m_cfg_name + "\":");
				if (SaveCfg(dlg.m_cfg_name))
					AddLog("+ Конфигурация сохранена успешно");
				else
				{
					g_errors_occured = true;
					AddLog("-- Ошибка сохранения конфигурации");
				}
			}
			else
			{
				AddLog("Сохранение конфигурации:");
				g_errors_occured = true;
				AddLog("-- Конфигурация не была сохранена");
			}
		}

		//if (g_errors_occured)
		{
			SetDlgItemText(IDCANCEL, "Закрыть");
			g_cancel_as_abort = false;
		}
		//else
		//	EndDialog(IDOK);
	}
	else
	{
		SERV_DevicesMap::iterator current = SERV_current_device_cfg->GetDevicesBegin();
		for (int i = 0; i < g_devices_counter; i++)
			current++;
		StartDevice(current->second);
	}
}

void DlgDevicesMemory::AddLog(CString log)
{
	m_list_log.AddString(log);
	LOG_V0_AddMessage(LOG_V0_MESSAGE, log, true, true);
}

void DlgDevicesMemory::AddNegativeLog(CString log)
{
	AddLog(log);
	AddLog("-- Конфигурация для \"" + g_prev_device->GetInfo() + "\" не " + ((m_download)?"сохранена":"восстановлена"));
}

void DlgDevicesMemory::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1)
	{
		int addr = SERV_GetCurrentMemoryAddr();
		if (addr == 0xFFFF)
			addr = 0;
		m_progress_memory.SetPos(addr);
		SetDlgItemText(IDC_STATIC_MEMORY_INFO, (CString)(TXT("%d/2048")<<addr));
		
		if (SERV_GetRepeatOfLastCommand() >= 3)
		{
			g_errors_occured = true;
			AddNegativeLog(CString("-- Устройство \"") + g_prev_device->GetInfo() + "\" не отвечает");
			StartNext();
		}
	}
}

void DlgDevicesMemory_UploadMem()
{
	DlgHardCfg dlg1(false);
	if (dlg1.DoModal() == IDOK)
	{
		DlgDevicesMemory::m_cfg_name = dlg1.m_cfg_name;
		DlgDevicesMemory dlg2(false);
		dlg2.DoModal();
	}
}

void DlgDevicesMemory_DownloadMem()
{
	DlgDevicesMemory dlg(true);
	dlg.DoModal();
}