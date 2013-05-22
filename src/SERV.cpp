#include <StdAfx.h>
#include "SERV.h"
#include <CHECK.h>
#include "LOG_V0.h"
#include "TXT.h"
#include "BS.h"
#include "../KRS.h"

#define STAGE_IDLE 0
#define STAGE_REQUEST_RESULTS_TABLE_SIZE 1
#define STAGE_REQUEST_RESULTS_TABLE 2
#define STAGE_REQUEST_CALLIBRATION_TABLE_SIZE 3
#define STAGE_REQUEST_CALLIBRATION_TABLE 4
#define STAGE_WRITE_ENABLE_WRITING 12
#define STAGE_WRITE_CALIBRATION_TABLE_SIZE 13
#define STAGE_WRITE_CALIBRATION_TABLE 14
#define STAGE_UNCALIBRATABLE 0xFF

#define RESULTS_TABLE_ADDRESS 0x07b2
#define CALIBRATION_TABLE_ADDRESS 0x0400

SERV_ConnectionInfo SERV_connection_info;
Indicator* SERV_calibration_indicator = NULL;
SERV_DeviceCfg* SERV_current_device_cfg = NULL;
CMutex SERV_current_device_cfg_mutex;
SERV_BOs SERV_indication_blocks, SERV_tmp_indication_blocks;
CMutex SERV_mutex_BO;

static int g_bo_file_version = -1;
static int g_devices_cfg_file_version = -1;

void SERV_DeviceCfgInit(CString path)
{
	SERV_DeviceCfgShutDown();

	SERV_current_device_cfg = new SERV_DeviceCfg;
	
	CString filename = path;

	FILE* f = fopen(filename, "rt");
	if (f)
	{
		fclose(f);
		SERV_current_device_cfg->Load(filename);
	}
	else
		SERV_current_device_cfg->SetName(filename);
}

void SERV_DeviceCfgShutDown()
{
	if (SERV_current_device_cfg)
		delete SERV_current_device_cfg;
}
void UpdateTextOfCalibrationStage()
{
	if (SERV_calibration_indicator == NULL || !::IsWindow(SERV_calibration_indicator->m_hWnd))
		return;
	CString str = SERV_calibration_indicator->GetCurrentStateText();
	SERV_calibration_indicator->SetStateText(SERV_calibration_indicator->GetState(), str + '.');
}

void SetTextOfCalibrationStage(CString str, byte state)
{
	if (SERV_calibration_indicator == NULL || !::IsWindow(SERV_calibration_indicator->m_hWnd))
		return;
	SERV_calibration_indicator->SetStateText(state, str);
	byte prev_state = SERV_calibration_indicator->GetState();
	if (prev_state != state)
		SERV_calibration_indicator->SetState(state);
}
SERV_Channel::SERV_Channel(CString name, SERV_Device* device, byte data_offset, byte data_len):
	m_name(name), m_device(device), m_data_offset(data_offset), m_data_len(data_len),
	m_last_value(0xFFFF), m_calibration_stage(STAGE_IDLE),
	m_use_voltage(false), m_v0(0), m_v1(1), m_dsp1(0xFFFF),
	m_check_jumps(false), m_jump_up(0), m_jump_down(0),
	m_value_not_set(true)
{
	if (m_device)
		m_device->AddChannel(this);
}

WORD SERV_Channel::GetUniqueNumber()
{
	CHECK(m_device != NULL);
	return GetUniqueNumber(m_device->m_device_addr, m_data_offset);
}

WORD SERV_Channel::GetUniqueNumber(byte dev_addr)
{
	return GetUniqueNumber(dev_addr, m_data_offset);
}

WORD SERV_Channel::GetUniqueNumber(byte dev_addr, byte data_offset)
{
	return (dev_addr << 8) | data_offset;
}

CString SERV_Channel::GetInfo()
{
	CString res;
	res.Format("%s (%d-%d)", m_name, m_data_offset, m_data_offset + m_data_len - 1);
	return res;
}

CString SERV_Channel::GetExtendedInfo()
{
	CString res;
	CString zzz;
	byte dev_addr = 0xFF;
	if (m_device != NULL)
	{
		zzz.Format("%02xh", m_device->GetAddr());
	}
	else
		zzz = "- нет устройства - ";
	res.Format("%s (%s %d-%d)", m_name, zzz, m_data_offset, m_data_offset + m_data_len - 1);
	return res;
}

bool SERV_Channel::Load(SERV_Device* device, FILE* f)
{
	int offset, len, dsp, use_voltage, jump, jump_up, jump_down;
	float v0, v1;
	char str[1024], name[1024];
	if (!fgets(str, 1024, f) || sscanf(str, "CH offset=%d len=%d v=%d v0=%f v1=%f ацп1=%d j=%d j_up=%d j_dn=%d имя=%[^\0\n]s", &offset, &len, &use_voltage, &v0, &v1, &dsp, &jump, &jump_up, &jump_down, name) != 10)
		return false;
	SERV_Channel* channel = new SERV_Channel(name, device, offset, len);
	channel->m_use_voltage = (use_voltage == 1);
	channel->m_dsp1 = dsp;
	channel->m_v0 = v0;
	channel->m_v1 = v1;
	channel->m_check_jumps = (jump == 1);
	channel->m_jump_up = jump_up;
	channel->m_jump_down = jump_down;
	return true;
}

bool SERV_Channel::Save(FILE* f)
{
		fputs(CString(TXT("CH offset=%d len=%d v=%d v0=%s v1=%s ацп1=%d j=%d j_up=%d j_dn=%d имя=%s\n") << m_data_offset << m_data_len << (m_use_voltage?1:0) <<BS_FloatWOZeros(m_v0, 6) << BS_FloatWOZeros(m_v1, 6) << m_dsp1 << (m_check_jumps?1:0) << m_jump_up << m_jump_down << m_name), f);
	return true;
}

bool SERV_Channel::IsTimedOut()
{
	return (m_device == NULL) || m_device->IsTimedOut();
}

void SERV_Channel::SetDataOffset(byte data_offset)
{
	if (m_device == NULL)
		return;
	if (m_data_offset == data_offset)
		return;
	WORD old_uniq = GetUniqueNumber();
	WORD new_uniq = GetUniqueNumber(m_device->m_device_addr, data_offset);
	m_device->m_channels.erase(old_uniq);
	m_device->m_cfg->m_channels.erase(old_uniq);
	m_data_offset = data_offset;
	m_device->m_channels[new_uniq] = this;
	m_device->m_cfg->m_channels[new_uniq] = this;
}

void SERV_Channel::SetLastValue(DWORD val)
{
	if (m_value_not_set)
		m_value_not_set = false;
	else
	if (m_check_jumps)
	{
		if (int(val) - m_jump_up > int(m_last_value))
			return;
		if (int(m_last_value) - m_jump_down > int(val))
			return;
	}
	m_last_value = val;
}

SERV_Device::SERV_Device(CString name, byte device_addr, int timeout, SERV_DeviceCfg* cfg):
	m_name(name), m_device_addr(device_addr), m_cfg(cfg), m_timeout_interval(timeout),
	m_last_received_tick(0),
	m_calibration_pause_tick(0)
{
	if (m_cfg)
		m_cfg->m_devices[device_addr] = this;
}

bool SERV_Device::IsTimedOut()
{
	DWORD diff = GetTickCount() - m_last_received_tick;
	bool res = diff > m_timeout_interval;
	if (res)
		int i = 0;
	return res;//(GetTickCount() - m_last_received_tick) > m_timeout_interval;
}

void SERV_Device::DeleteAllChannels()
{
	if (m_cfg == NULL)
		return;
	MutexWrap channels_map_access(m_cfg->m_channels_map_mutex);
	SERV_Channel* channel;
	map<WORD, SERV_Channel*>::iterator current = m_channels.begin();
	while (current != m_channels.end())
	{
		channel = current->second;
		m_cfg->m_channels.erase(current->first);
		delete current->second;
		current++;
	}
	m_channels.clear();
}

SERV_Device::~SERV_Device()
{
	if (m_cfg == NULL)
		return;
	DeleteAllChannels();
	m_cfg->m_devices.erase(m_device_addr);
}

CString SERV_Device::GetInfo()
{
	CString res;
	res.Format("%s (%02Xh)", m_name, m_device_addr);
	return res;
}

bool SERV_Device::Load(SERV_DeviceCfg* cfg, FILE* f)
{
	int addr, to_interval, num_channels;
	char str[1024], name[1024];
	if (!fgets(str, 1024, f) || sscanf(str, "DEV addr=%02X timeout=%d channels=%d имя=%[^\0\n]s", &addr, &to_interval, &num_channels, name) != 4)
		return false;
	SERV_Device *device = new SERV_Device(name, addr, to_interval, cfg);
	for (int i = 0; i < num_channels; i++)
		if (!SERV_Channel::Load(device, f))
			return false;
	return true;
}

bool SERV_Device::Save(FILE* f)
{
	fputs(CString(TXT("DEV addr=%02X timeout=%d channels=%d имя=%s\n") << m_device_addr << m_timeout_interval << m_channels.size() << m_name), f);
	SERV_ChannelsMap::iterator current_s = m_channels.begin();
	while (current_s != m_channels.end())
	{
		if (!current_s->second->Save(f))
			return false;
		current_s++;
	}
	return true;
}

bool SERV_Device::ProtectedRemoveChannel(SERV_Channel *channel)
{
	CHECK(channel != NULL);
	WORD uniq_num = channel->GetUniqueNumber();
	if (m_channels.find(uniq_num) == m_channels.end())
		return false;
	m_channels.erase(uniq_num);
	CHECK(m_cfg->m_channels.find(uniq_num) != m_cfg->m_channels.end());
	m_cfg->m_channels.erase(uniq_num);
	return true;
}

bool SERV_Device::RemoveChannel(SERV_Channel *channel)
{
	CHECK(m_cfg != NULL);
	MutexWrap channels_map_access(m_cfg->m_channels_map_mutex);
	bool res = ProtectedRemoveChannel(channel);
	if (res)
		delete channel;
	return res;
}

bool SERV_Device::AddChannel(SERV_Channel *channel)
{
	CHECK(m_cfg != NULL);
	CHECK(channel != NULL);
	MutexWrap channels_map_access(m_cfg->m_channels_map_mutex);
	WORD new_uniq_num = channel->GetUniqueNumber(m_device_addr);
	if (channel->m_device != this)
	{
		if (m_cfg->m_channels.find(new_uniq_num) != m_cfg->m_channels.end()) 
			return false;
		if (channel->m_device != NULL)
		{
			bool res = channel->m_device->ProtectedRemoveChannel(channel);
			CHECK(res == true);
		}
		channel->m_device = this;
	}
	m_channels[new_uniq_num] = channel;
	m_cfg->m_channels[new_uniq_num] = channel;
	return true;
}

void SERV_Device::SetAddr(byte mem_addr)
{
	if (mem_addr == m_device_addr)
		return;

	CHECK(m_cfg->m_devices.find(mem_addr) == m_cfg->m_devices.end()); 

	SERV_ChannelsMap::iterator current_s;
	SERV_ChannelsMap new_channels;
	SERV_Channel* channel;
	WORD new_uniq_number;
	current_s = m_channels.begin();
	while (current_s != m_channels.end())
	{
		channel = current_s->second;
		new_uniq_number = channel->GetUniqueNumber(mem_addr);
		new_channels[new_uniq_number] = channel;
		m_cfg->m_channels.erase(channel->GetUniqueNumber());
		CHECK(m_cfg->m_channels.find(new_uniq_number) == m_cfg->m_channels.end()); 
		m_cfg->m_channels[new_uniq_number] = channel;
		current_s++;
	}
	m_channels = new_channels;

	m_cfg->m_devices.erase(m_device_addr);
	m_device_addr = mem_addr;
	m_cfg->m_devices[m_device_addr] = this;
}

byte SERV_Device::FindFreeOffset()
{
	byte offset;
	SERV_Channel* channel;
	SERV_ChannelsMap::iterator desired;
	for (offset = 0; offset < 31; offset++)
	{
		desired = m_channels.find(SERV_Channel::GetUniqueNumber(m_device_addr, offset));
		if (desired == m_channels.end()) 
			return offset;
		else
		{
			channel = desired->second;
			offset += channel->m_data_len - 1;
		}
	}
	return 0xFF;
}
SERV_DeviceCfg::SERV_DeviceCfg():
	m_successfully_loaded(false)
{
}

SERV_DeviceCfg::~SERV_DeviceCfg()
{
	Clear();
}

void SERV_DeviceCfg::Clear()
{
	SERV_DevicesMap::iterator current_d;
	while (m_devices.size() > 0)
	{
		current_d = m_devices.begin();
		delete (current_d->second); 
	}
	CHECK(m_channels.size() == 0) 
}

SERV_DeviceCfg* SERV_DeviceCfg::MakeCopy()
{
	SERV_DeviceCfg* clone_cfg = new SERV_DeviceCfg;
	clone_cfg->m_file_name = m_file_name;
	clone_cfg->m_successfully_loaded = true;

	SERV_Device* device, *clone_device;
	SERV_Channel* channel, *clone_channel;
	SERV_ChannelsMap::iterator current_s;
	SERV_DevicesMap::iterator current_d = m_devices.begin();
	while (current_d != m_devices.end())
	{
		device = current_d->second;
		clone_device = new SERV_Device(device->GetName(), device->GetAddr(), device->m_timeout_interval, clone_cfg);

		current_s = device->GetChannelsBegin();
		while (current_s != device->GetChannelsEnd())
		{
			channel = current_s->second;
			clone_channel = new SERV_Channel(channel->m_name, clone_device, channel->m_data_offset, channel->m_data_len);
			clone_channel->m_use_voltage = channel->m_use_voltage;
			clone_channel->m_dsp1 = channel->m_dsp1;
			clone_channel->m_v0 = channel->m_v0;
			clone_channel->m_v1 = channel->m_v1;
			clone_channel->m_check_jumps = channel->m_check_jumps;
			clone_channel->m_jump_up = channel->m_jump_up;
			clone_channel->m_jump_down = channel->m_jump_down;
			current_s++;
		}
		current_d++;
	}
	return clone_cfg;
}

bool SERV_DeviceCfg::Load(FILE* f)
{
	char str[1024];
	if (!fgets(str, 1024, f) || sscanf(str, "VER %d", &g_devices_cfg_file_version) != 1)
		return false;
	int i, num;
	if (!fgets(str, 1024, f) || sscanf(str, "NUM %d", &num) != 1 || num < 1)
		return false;
	for (i = 0; i < num; i++)
	{
		if (!SERV_Device::Load(this, f))
			break;
	}
	if (i != num)
		Clear();
	return i == num;
}

bool SERV_DeviceCfg::Load(CString filename)
{
	m_file_name = filename;
	m_successfully_loaded = false;

	FILE*f = fopen(filename, "rt");
	if (!f)
	{
		MessageBox(NULL, "Файл не найден", "Невозможно считать файл конфигурации", MB_OK);
		return false;
	}

	CString str;
	SERV_Device* current_device = NULL;
	SERV_Channel* channel = NULL;
	char buff[1024];
	float f1, f2;
	int n1, n2, n3, i, j;
	bool use_voltage;
	g_devices_cfg_file_version = 1;
	while (!feof(f))
	{
		if (fgets(buff, 1024, f) == NULL)
			break;
		n1 = strlen(buff);
		if (n1 < 1)
			continue;
		if (buff[0] == 'V') 
		{
			fseek(f, 0, SEEK_SET);
			m_successfully_loaded = Load(f);
			fclose(f);
			return m_successfully_loaded;
		}
		if (buff[n1-1] == '\n')
			buff[n1-1] = '\0';
		if (buff[0] == '\t')
		{
			if (current_device == NULL)
			{
				MessageBox(NULL, "Неправильный формат", "Невозможно считать файл конфигурации", MB_OK);
				fclose(f);
				return false;
			}
			if (sscanf(buff+1, "%02d:%02d:", &n1, &n2) != 2)
			{
				MessageBox(NULL, "Неправильный формат", "Невозможно считать файл конфигурации", MB_OK);
				fclose(f);
				return false;
			}
			if (n1 < 0 || n1 > 31)
			{
				MessageBox(NULL, "Неправильный формат", "Смещение канала должно быть в пределах 0-31", MB_OK);
				fclose(f);
				return false;
			}
			if (n2 < 1 || n2 > 4)
			{
				MessageBox(NULL, "Неправильный формат", "Размер данных канала должен быть в пределах 1-4", MB_OK);
				fclose(f);
				return false;
			}
			use_voltage = (*(buff+7) == 'v');
			if (use_voltage)
			{
				if (sscanf(buff+7, "v0=%f v1=%f ацп1=%d:", &f1, &f2, &n3) != 3)
				{
					MessageBox(NULL, "Неправильный формат", "Невозможно считать файл конфигурации", MB_OK);
					fclose(f);
					return false;
				}
			}
			str = buff;
			j = -1;
			for (i = 0; i < 3; i++)
				j = str.Find(":", j + 1);
			channel = new SERV_Channel(buff+j+1, current_device, n1, n2);
			channel->m_use_voltage = use_voltage;
			if (channel->m_use_voltage)
			{
				channel->m_v0 = f1;
				channel->m_v1 = f2;
				channel->m_dsp1 = n3;
			}
		}
		else
		{
			if (sscanf(buff, "%02X:%04X:", &n1, &n2) != 2)
			{
				MessageBox(NULL, "Неправильный формат", "Невозможно считать файл конфигурации", MB_OK);
				fclose(f);
				return false;
			}
			if (n1 < 0 || n1 >= 0x80)
			{
				MessageBox(NULL, "Неправильный формат", "Адрес устройства должен быть в пределах 01-7F", MB_OK);
				fclose(f);
				return false;
			}
			current_device = new SERV_Device(buff + 8, n1, n2, this);
		}
	}
	fclose(f);
	m_successfully_loaded = true;
	return true;
}

bool SERV_DeviceCfg::Save(CString filename)
{
	if (filename.GetLength() == 0)
		filename = m_file_name;
	FILE*f = fopen(filename, "wt");
	if (!f)
		return false;
	fputs(CString(TXT("VER %d\n") << SERV_DEVICE_CFG_FILE_VERSION), f);
	fputs(CString(TXT("NUM %d\n") << m_devices.size()), f);
	SERV_DevicesMap::iterator current_d = m_devices.begin();
	while (current_d != m_devices.end())
	{
		if (!current_d->second->Save(f))
		{
			MessageBox(NULL, "Ошибка записи на диск", "Не удается сохранить файл конфигурации", MB_OK);
			fclose(f);
			return false;
		}
		current_d++;
	}
	fclose(f);
	m_file_name = filename;
	return true;
}

SERV_Device* SERV_DeviceCfg::GetDevice(byte mem_addr)
{
	SERV_DevicesMap::iterator desired = m_devices.find(mem_addr);
	return (desired == m_devices.end())? NULL : desired->second;
}

SERV_Channel* SERV_DeviceCfg::GetChannel(WORD uniq_num)
{
	SERV_ChannelsMap::iterator desired = m_channels.find(uniq_num);
	return (desired == m_channels.end())? NULL : desired->second;
}
void WriteEEPROM(int dev_addr, int mem_addr, byte* data, byte size, bool wait_for_wnswer);
void ReadEEPROM(int dev_addr, int mem_addr, byte size, bool wait_for_wnswer);

static bool g_working = true;

struct ClientSERV: NET_Client
{
	ClientSERV():
		m_middle_in_buffer(NULL), m_middle_in_buffer_size(0),
		m_middle_out_buffer(NULL), m_middle_out_buffer_size(0)
	{
		SetBuffersSize(1024*1, 1024*100, 1024*1, 1024*100);
	}
	virtual ~ClientSERV()
	{
		if (m_middle_in_buffer)
			delete[] m_middle_in_buffer;
		if (m_middle_out_buffer)
			delete[] m_middle_out_buffer;
	}

	virtual CString GetName() { return "ClientSERV"; }

	virtual void OnConnected()
	{
		LOG_V0_AddMessage(LOG_V0_MESSAGE, "Соединение с DSN_Server установлено");

		PostMessage(SERV_connection_info.m_wnd, SERV_connection_info.m_initialization_complete_msg, 0, 0);
		CString connnected_packed = "@PORT1$";
		SendData(connnected_packed, connnected_packed.GetLength());
		SERV_BOs::iterator current_bo = SERV_indication_blocks.begin();
		while (current_bo != SERV_indication_blocks.end())
		{
			(*current_bo)->m_request_stage = SERV_RS_READY; // если не будет DE требующих уставок, то девайс готов
			current_bo++;
		}
	}
	virtual void OnBreakedConnection()
	{
		m_wndDialogBar.m_ind_serv.SetState(STATE_UNKNOWN);
		if (g_working)
			SERV_connection_info.SignalError(BS_CONNECTION_BROKEN, "Соединение разорвано");
	}
	virtual void OnError(CString err)
	{
		SERV_connection_info.SignalError(BS_UNKNOWN_ERROR, err);
	}
	virtual void OnReceived();
	void SendAllReadyData()
	{
		int ready_data_size = SERV_connection_info.m_out_buffer.SpaceFor(BS_RingBuffer_READ);
		bool resize = (m_middle_out_buffer == NULL || (int)m_middle_out_buffer_size < ready_data_size);
		if (resize)
		{
			if (m_middle_out_buffer)
				delete[] m_middle_out_buffer;
			m_middle_out_buffer_size = ready_data_size;
			m_middle_out_buffer = new char[m_middle_out_buffer_size];
			if (m_middle_out_buffer == NULL)
			{
				SERV_connection_info.SignalError(BS_NOT_ENOUGH_MEMORY, "Недостаточно памяти для выходного буфера");
				return;
			}
		}
		SERV_connection_info.m_out_buffer.FillTo(m_middle_out_buffer, ready_data_size);
		SendData(m_middle_out_buffer, ready_data_size);
	}
	char *m_middle_in_buffer, *m_middle_out_buffer;
	DWORD m_middle_in_buffer_size, m_middle_out_buffer_size;
	NET_Buffer* m_in_buffer;
};
static ClientSERV g_client;
static HANDLE g_connection_closing_event = CreateEvent(NULL, FALSE, FALSE, NULL);
static bool g_inited = false;
static UINT ConnectionThreadProcedure(LPVOID param)
{
	g_inited = true;
	g_working = true;
	HANDLE multiple_events[2] = {g_connection_closing_event, SERV_connection_info.m_data_ready_for_sending_event};
	DWORD res;
	LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Попытка соединения с DSN_Server: ip = %s, port = %d") << SERV_connection_info.m_server_ip << SERV_connection_info.m_server_port);
	g_client.StartConnect(SERV_connection_info.m_server_ip, SERV_connection_info.m_server_port);
	while (true)
	{
		res = WaitForMultipleObjects(2, multiple_events, FALSE, INFINITE);
		if (res == WAIT_OBJECT_0) 
			break;
		if (res == WAIT_OBJECT_0 + 1) 
		{
			g_client.SendAllReadyData();
		}
	}
	if (g_client.IsConnected())
	{
		g_working = false;
		CString stop_str;
		stop_str.Format("%c$", 4);
		g_client.SendData(stop_str, stop_str.GetLength());
	}
	g_client.StopConnect();
	SetEvent(SERV_connection_info.m_connection_closed_event);
	g_inited = false;
	return +1;
}
void SERV_InitConnection()
{
	if (KRS_shutdown)
		return;

	if (g_inited)
		SERV_ShutDownConnection(true);

	ResetEvent(SERV_connection_info.m_connection_closed_event);
	AfxBeginThread(ConnectionThreadProcedure, THREAD_PRIORITY_NORMAL, NULL);
}
void SERV_ShutDownConnection(bool sync)
{
	m_wndDialogBar.m_ind_serv.SetState(STATE_UNKNOWN);
	if (!g_inited)
		return;
	SetEvent(g_connection_closing_event);
	if (sync)
		WaitForSingleObject(SERV_connection_info.m_connection_closed_event, INFINITE);
}
SERV_Channel* g_calibrating_channel = NULL;
SERV_Device* g_calibrating_device = NULL;

int g_repeat_of_last_cmd = 0;

WORD g_waited_cal_device_addr = 0xFFFF; 
WORD g_waited_cal_mem_addr = 0xFFFF; 
WORD g_left_table_size, g_table_size;

HWND g_mem_wnd; 
UINT g_mem_msg; 
WORD g_waited_mem_device_addr = 0xFFFF; 
WORD g_waited_mem_mem_addr = 0xFFFF; 
int g_waited_mem_mem_size = 0; 
#define MAX_MEM_SIZE 16
byte *g_mem_ptr;
bool g_mem_read;

DWORD g_last_request_tick = 0; 
CString g_last_request_text; 
HANDLE g_checking_thread_started_event = CreateEvent(NULL, FALSE, FALSE, NULL); 
HANDLE g_checking_thread_finishing_event = CreateEvent(NULL, FALSE, FALSE, NULL); 

void WriteEEPROM(int dev_addr, int mem_addr, byte* data, byte size, bool wait_for_answer)
{
	byte LADD = dev_addr;
	byte LPAK = 0x7 + size;
	CHECK(mem_addr < 256*8);
	byte CMD = ((wait_for_answer)?0x03:0x02) | ((mem_addr/256)<<5);
	byte ADR = mem_addr % 256;
	byte LDATA = size;

	CString req, byte_str;
	req.Format("@JOB#000#%02X%02X%02X%02X%02X", LADD, LPAK, CMD, ADR, LDATA);
	for (int i = 0; i < size; i++, data++)
	{
		byte_str.Format("%02X", *data);
		req += byte_str;
	}
	req += "00$"; 
	if (wait_for_answer)
	{
		g_last_request_tick = GetTickCount();
		g_last_request_text = req;
		g_repeat_of_last_cmd = 0;
	}
	g_client.SendData(req, req.GetLength());
}

void ReadEEPROM(int dev_addr, int mem_addr, byte size, bool wait_for_answer)
{
	byte LADD = dev_addr;
	byte LPAK = 0x7;
	CHECK(mem_addr < 256*8);
	byte CMD = 0x02 /*read*/ | ((mem_addr/256)<<5);
	byte ADR = mem_addr % 256;
	byte LDATA = size;

	CString req;
	req.Format("@JOB#000#%02X%02X%02X%02X%02X00$", LADD, LPAK, CMD, ADR, LDATA);
	if (wait_for_answer)
	{
		g_last_request_tick = GetTickCount();
		g_last_request_text = req;
		g_repeat_of_last_cmd = 0;
	}
	g_client.SendData(req, req.GetLength());
}

static UINT TimeoutCheckThreadProcedure(LPVOID param)
{
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "TimeoutCheckThreadProcedure started");
	SetEvent(g_checking_thread_started_event);
	DWORD res = WAIT_TIMEOUT;
	while (true)
	{
		res = WaitForSingleObject(g_checking_thread_finishing_event, 200);
		if (res == WAIT_OBJECT_0)
		{
			LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "TimeoutCheckThreadProcedure EVENT to stop");
			break;
		}
		if (g_last_request_tick > 0 && GetTickCount() - g_last_request_tick > 1500)
		{
			g_last_request_tick = GetTickCount();
			g_repeat_of_last_cmd++;
			g_client.SendData(g_last_request_text, g_last_request_text.GetLength()); 
		}
	}
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "TimeoutCheckThreadProcedure finished");
	return +1;
}

bool StartCheckingThread()
{
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "StartCheckingThread() started");
	g_last_request_tick = 0;
	g_last_request_text = "";
	ResetEvent(g_checking_thread_started_event);
	AfxBeginThread(TimeoutCheckThreadProcedure, THREAD_PRIORITY_NORMAL, NULL);
	while (true)
	{
		if (WaitForSingleObject(g_checking_thread_started_event, 10*1000) == WAIT_OBJECT_0)
		{
			LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "StartCheckingThread() success");
			return true;
		}
		if (MessageBox(NULL, "Не запустился поток, повторить попытку?", "Ошибка программы", MB_YESNO) == IDNO)
		{
			LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "StartCheckingThread() cancelled");
			return false;
		}
	}
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "StartCheckingThread() unsuccess");
	return false;
}

void SERV_StopCalibration()
{
	if (g_waited_cal_device_addr == 0xFFFF && g_waited_cal_mem_addr == 0xFFFF)
		return;

	SetEvent(g_checking_thread_finishing_event); 

	g_waited_cal_device_addr = 0xFFFF;
	g_waited_cal_mem_addr = 0xFFFF;

	LOG_V0_AddMessage(LOG_V0_MESSAGE, "Калибровка остановлена");

	if (g_calibrating_channel == NULL)
		return;
	g_calibrating_channel->SetCalibrationStage(STAGE_IDLE);
	g_calibrating_channel = NULL;
}

void AnalyzeCalibrationAnswer(byte* data, int data_size)
{
	struct Corrector
	{
		Corrector()
		{
			m_interrupt_calibration = true;
		}
		~Corrector()
		{
			if (m_interrupt_calibration)
				SERV_StopCalibration();
		}
		bool m_interrupt_calibration;
	};
	Corrector corrector;

	if (g_calibrating_channel == NULL)
		return;

	byte *pd, out_data[16];
	int i;
	WORD addr_of_channel_result;
	SERV_CalibrationPoint cp;
	SERV_CalibrationTable::iterator current_cp;

	byte stage = g_calibrating_channel->GetCalibrationStage();
	switch (stage)
	{
		case STAGE_IDLE:
			return;
		case STAGE_REQUEST_RESULTS_TABLE_SIZE:
			g_table_size = g_left_table_size = *(data + 1);
			SetTextOfCalibrationStage("Запрос таблицы размещения результатов", STATE_UNKNOWN);
			g_calibrating_channel->SetCalibrationStage(STAGE_REQUEST_RESULTS_TABLE);
			g_waited_cal_mem_addr += 2; 
			ReadEEPROM(g_waited_cal_device_addr, g_waited_cal_mem_addr, min(16, g_left_table_size), true); 
			corrector.m_interrupt_calibration = false;
			return;
		case STAGE_REQUEST_RESULTS_TABLE:
			pd = data;
			for (i = 0; i < data_size/2; i++, pd +=2)
			{
				addr_of_channel_result = (*(pd + 1)) >> 3;
				if (addr_of_channel_result == g_calibrating_channel->GetDataOffset()) 
				{
					if ((*pd) < 0x80)
					{
						MessageBox(NULL, "Ошибка в таблице размещения результатов", "Попытка калибровать вычисляемый канал", MB_OK);
						SetTextOfCalibrationStage("Ошибка в таблице размещения результатов #1", STATE_ERROR);
						return;
					}
					SetTextOfCalibrationStage("Запрос размера калибровочной таблицы", STATE_UNKNOWN);
					g_calibrating_channel->SetDSPNum(((*pd) - 0x80)/0x10);
					g_calibrating_channel->SetCalibrationStage(STAGE_REQUEST_CALLIBRATION_TABLE_SIZE);
					g_waited_cal_mem_addr = CALIBRATION_TABLE_ADDRESS + g_calibrating_channel->GetDSPNum()*48 + 2;
					ReadEEPROM(g_waited_cal_device_addr, g_waited_cal_mem_addr, 2, true);
					corrector.m_interrupt_calibration = false;
					return;
				}
			}
			if (g_left_table_size <= data_size) 
			{
				MessageBox(NULL, "Ошибка в таблице размещения результатов", "Отсутствует искомая запись", MB_OK);
				SetTextOfCalibrationStage("Ошибка в таблице размещения результатов #2", STATE_ERROR);
				return;
			}
			UpdateTextOfCalibrationStage();
			g_left_table_size -= data_size;
			g_waited_cal_mem_addr += 16;
			ReadEEPROM(g_waited_cal_device_addr, g_waited_cal_mem_addr, min(16, g_left_table_size), true); 
			corrector.m_interrupt_calibration = false;
			return;
		case STAGE_REQUEST_CALLIBRATION_TABLE_SIZE:
			g_table_size = g_left_table_size = *(data + 1);
			if (g_table_size < 4)
			{
				SetTextOfCalibrationStage("Запросы выполнены, таблицы нет", STATE_ON);
				::PostMessage(SERV_connection_info.m_calibration_table_ready_wnd, SERV_connection_info.m_calibration_table_ready_msg, g_calibrating_channel->GetUniqueNumber(), g_calibrating_channel->GetConvNumber());

				LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Калибровочная таблица отсутствует (устройство %s, канал %s") << g_calibrating_device->GetInfo() << g_calibrating_channel->GetInfo());

				return;
			}
			SetTextOfCalibrationStage("Запрос калибровочной таблицы", STATE_UNKNOWN);
			g_calibrating_channel->SetCalibrationStage(STAGE_REQUEST_CALLIBRATION_TABLE);
			g_waited_cal_mem_addr += 2; 
			ReadEEPROM(g_waited_cal_device_addr, g_waited_cal_mem_addr, min(16, g_left_table_size), true); 
			corrector.m_interrupt_calibration = false;
			return;
		case STAGE_REQUEST_CALLIBRATION_TABLE:
			pd = data;
			for (i = 0; i < data_size/4; i++, pd +=4)
			{
				cp.m_dsp_val = (*(pd + 0) << 8) | (*(pd + 1));
				cp.m_phys_val = (*(pd + 2) << 8) | (*(pd + 3));
				g_calibrating_channel->AddCalibrationPoint(cp);
			}
			if (g_left_table_size <= data_size) 
			{
				SetTextOfCalibrationStage("Запросы выполнены, таблица прочитана", STATE_ON);
				::PostMessage(SERV_connection_info.m_calibration_table_ready_wnd, SERV_connection_info.m_calibration_table_ready_msg, g_calibrating_channel->GetUniqueNumber(), g_calibrating_channel->GetConvNumber());

				LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Калибровочная таблица прочитана (устройство %s, канал %s") << g_calibrating_device->GetInfo() << g_calibrating_channel->GetInfo());

				return;
			}
			UpdateTextOfCalibrationStage();
			g_left_table_size -= data_size;
			g_waited_cal_mem_addr += 16;
			ReadEEPROM(g_waited_cal_device_addr, g_waited_cal_mem_addr, min(16, g_left_table_size), true); 
			corrector.m_interrupt_calibration = false;
			return;
		case STAGE_WRITE_ENABLE_WRITING:
			g_calibrating_device->StartCalibrationPause(); 
			SetTextOfCalibrationStage("Сохранение размера таблицы калибровки", STATE_UNKNOWN);
			out_data[0] = 0;
			out_data[1] = g_calibrating_channel->GetCalibrationTableSize()*4;
			g_calibrating_channel->SetCalibrationStage(STAGE_WRITE_CALIBRATION_TABLE_SIZE);
			g_waited_cal_mem_addr = CALIBRATION_TABLE_ADDRESS + g_calibrating_channel->GetDSPNum() * 48 + 2;
			WriteEEPROM(g_waited_cal_device_addr, g_waited_cal_mem_addr, out_data, 2, true); 
			corrector.m_interrupt_calibration = false;
			return;
		case STAGE_WRITE_CALIBRATION_TABLE_SIZE:
			g_calibrating_device->StartCalibrationPause(); 
			SetTextOfCalibrationStage("Cохранение таблицы калибровки", STATE_UNKNOWN);
			i = 0;
			current_cp = g_calibrating_channel->GetCalibrationTableBegin();
			while (current_cp != g_calibrating_channel->GetCalibrationTableEnd() && i < 4)
			{
				cp = *current_cp;
				out_data[i*4 + 0] = cp.m_dsp_val >> 8;
				out_data[i*4 + 1] = cp.m_dsp_val & 0xFF;
				out_data[i*4 + 2] = cp.m_phys_val >> 8;
				out_data[i*4 + 3] = cp.m_phys_val & 0xFF;
				i++;
				current_cp++;
			}
			g_calibrating_channel->SetCalibrationStage(STAGE_WRITE_CALIBRATION_TABLE);
			g_waited_cal_mem_addr += 2; 
			WriteEEPROM(g_waited_cal_device_addr, g_waited_cal_mem_addr, out_data, i*4, true); 
			corrector.m_interrupt_calibration = false;
			return;
		case STAGE_WRITE_CALIBRATION_TABLE:
			i = ( (g_waited_cal_mem_addr + data_size) - (CALIBRATION_TABLE_ADDRESS + g_calibrating_channel->GetDSPNum() * 48 + 4) ) / 4; 
			if (i == g_calibrating_channel->GetCalibrationTableSize()) 
			{
				LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Калибровочная таблица записана (устройство %s, канал %s") << g_calibrating_device->GetInfo() << g_calibrating_channel->GetInfo());

				g_calibrating_device->StartCalibrationPause(); 
				SetTextOfCalibrationStage("Таблица сохранена", STATE_ON);
				::PostMessage(SERV_connection_info.m_calibration_table_ready_wnd, SERV_connection_info.m_calibration_table_ready_msg, g_calibrating_channel->GetUniqueNumber(), g_calibrating_channel->GetConvNumber());
				{
					CString req = TXT("@JOB#000#%02X0705100100$") << g_waited_cal_device_addr;
					g_client.SendData(req, req.GetLength());
				}
				corrector.m_interrupt_calibration = true;
				return;
			}
			UpdateTextOfCalibrationStage();
			current_cp = g_calibrating_channel->GetCalibrationTableBegin();
			while (i > 0 && current_cp != g_calibrating_channel->GetCalibrationTableEnd())
			{
				i--; current_cp++;
			}
			while (current_cp != g_calibrating_channel->GetCalibrationTableEnd() && i < 4)
			{
				cp = *current_cp;
				out_data[i*4 + 0] = cp.m_dsp_val >> 8;
				out_data[i*4 + 1] = cp.m_dsp_val & 0xFF;
				out_data[i*4 + 2] = cp.m_phys_val >> 8;
				out_data[i*4 + 3] = cp.m_phys_val & 0xFF;
				i++;
				current_cp++;
			}
			g_calibrating_device->StartCalibrationPause(); 
			g_waited_cal_mem_addr += data_size; 
			WriteEEPROM(g_waited_cal_device_addr, g_waited_cal_mem_addr, out_data, i*4, true); 
			corrector.m_interrupt_calibration = false;
			return;
	}
}

void SERV_StopMemoryHandling(bool aborting)
{
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "SERV_StopMemoryHandling started");
	if (g_waited_mem_device_addr == 0xFFFF && g_waited_mem_mem_addr == 0xFFFF)
	{
		LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "SERV_StopMemoryHandling breaked");
		return;
	}

	SetEvent(g_checking_thread_finishing_event); 
		
	g_waited_mem_device_addr = 0xFFFF;
	g_waited_mem_mem_addr = 0xFFFF;

	LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Процесс %s данных %s")<< ((g_mem_read)?"чтения":"записи") << ((aborting)?"прерван пользователем":"завершен"));
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "SERV_StopMemoryHandling finished");
}

void AnalyzeMemAnswer(byte* data)
{
	if (!g_mem_read && g_waited_mem_mem_addr == 0x20) 
	{
		g_calibrating_device->StartCalibrationPause(); 
		g_waited_mem_mem_addr = 256; 
		g_waited_mem_mem_size = MAX_MEM_SIZE;
		WriteEEPROM(g_waited_mem_device_addr, g_waited_mem_mem_addr, g_mem_ptr + g_waited_mem_mem_addr - 256, g_waited_mem_mem_size, true);
		return;
	}

	if (g_mem_read)
		memcpy(g_mem_ptr + g_waited_mem_mem_addr - 256, data, g_waited_mem_mem_size);
	g_waited_mem_mem_addr += g_waited_mem_mem_size;
	if (g_waited_mem_mem_addr >= 8*256)
	{
		WORD device_addr = g_waited_mem_device_addr; 
		SERV_StopMemoryHandling(false);
		if (!g_mem_read)
		{
			g_calibrating_device->StartCalibrationPause(); 
			g_calibrating_device = NULL;
			CString req = TXT("@JOB#000#%02X0705100100$") << device_addr;
			g_client.SendData(req, req.GetLength());
		}
		else
			g_calibrating_device = NULL;
		::PostMessage(g_mem_wnd, g_mem_msg, 0, device_addr);
		return;
	}

	g_waited_mem_mem_size = min(MAX_MEM_SIZE, 8*256 - g_waited_mem_mem_addr);
	if (g_mem_read)
		ReadEEPROM(g_waited_mem_device_addr, g_waited_mem_mem_addr, g_waited_mem_mem_size, true);	
	else
	{
		g_calibrating_device->StartCalibrationPause(); 
		WriteEEPROM(g_waited_mem_device_addr, g_waited_mem_mem_addr, g_mem_ptr + g_waited_mem_mem_addr - 256, g_waited_mem_mem_size, true);	
	}
}
bool OnServerPacketReceived(byte* packet, int packet_size)
{
	if (packet_size < 5)
		return false;

	byte dev_addr = packet[0] ^ 0x80;
	int mem_addr = (packet[2] >> 5) * 256 + packet[3]; 
	byte data_len = packet[4];
	byte *data = packet + 5;

	SERV_BOs::iterator current_bo = SERV_indication_blocks.begin();
	while (current_bo != SERV_indication_blocks.end())
	{
		if (dev_addr == (*current_bo)->m_device_addr &&
			(*current_bo)->m_request_stage < SERV_RS_READY && data_len == 2) 
		{
			WORD data_word = (*(data + 0) << 8) | *(data + 1);
			SERV_DEs::iterator current_de = (*current_bo)->m_data_elements.begin();
			while (current_de != (*current_bo)->m_data_elements.end())
			{
				if ((*current_de)->m_has_ustavka_and_limit)
				{
					if ((*current_bo)->m_request_stage == SERV_RS_READ_USTAVKA && (*current_de)->m_ustavka == 0xFFFF)
					{
						if (data_word == 0xFFFF) data_word = 0xFFFE; 
						(*current_de)->m_ustavka = data_word;
						(*current_bo)->m_request_stage = SERV_RS_READ_LIMIT;
						ReadEEPROM((*current_bo)->m_device_addr, (*current_de)->m_limit_addr, 2, false);
						return true;
					}
					if ((*current_bo)->m_request_stage == SERV_RS_READ_LIMIT && (*current_de)->m_limit == 0xFFFF)
					{
						if (data_word == 0xFFFF) data_word = 0xFFFE; 
						(*current_de)->m_limit = data_word;
						current_de++; 
						while (current_de != (*current_bo)->m_data_elements.end())
						{
							if ((*current_de)->m_has_ustavka_and_limit)
							{
								(*current_de)->m_ustavka = 0xFFFF;
								(*current_de)->m_limit = 0xFFFF;
								(*current_bo)->m_request_stage = SERV_RS_READ_USTAVKA;
								ReadEEPROM((*current_bo)->m_device_addr, (*current_de)->m_ustavka_addr, 2, false);
								return true;
							}
							current_de++;
						}
						(*current_bo)->m_request_stage = SERV_RS_READY; 
						return true;
					}
				}
				current_de++;
			}
		}
		current_bo++;
	}
	if (g_waited_cal_device_addr == dev_addr && g_waited_cal_mem_addr == mem_addr)
	{
		AnalyzeCalibrationAnswer(data, data_len);
		return true;
	}
	if (g_waited_mem_device_addr == dev_addr && g_waited_mem_mem_addr == mem_addr && g_waited_mem_mem_size == data_len)
	{
		AnalyzeMemAnswer(data);
		return true;
	}
	if (mem_addr != 0)
		return false;

	if (SERV_current_device_cfg == NULL || !SERV_current_device_cfg->IsLoadedOk())
		return false;

	{
		MutexWrap cfg_access(SERV_current_device_cfg_mutex);
		SERV_Device* device = SERV_current_device_cfg->GetDevice(dev_addr);
		if (device == NULL)
			return false;
		SERV_Channel* channel;
		DWORD value;
		byte i, channel_data_len;
		SERV_ChannelsMap::iterator current = device->GetChannelsBegin(), last = device->GetChannelsEnd();
		while (current != last)
		{
			channel = current->second;
			channel_data_len = channel->GetDataLength();
			if (channel->GetDataOffset() +  channel_data_len <= data_len)
			{
				value = 0;
				for (i = 0; i < channel_data_len; i++)
				{
					value = (value << 8) | *(data + channel->GetDataOffset() + i);
				}
				channel->SetLastValue(value);
			}
			current++;
		}
		device->m_last_received_tick = GetTickCount();
	}
	return true;
}

byte GetHexDigit(char ch)
{
	if (ch >= '0' && ch <= '9')
		return byte(ch) - 48;
	if (ch >= 'A' && ch <= 'F')
		return byte(ch) - byte('A') + 10;
	return byte(ch) - byte('a') + 10;
}

void HexStringToByteArray(char* str, byte* data, int data_len)
{
	while (data_len > 0)
	{
		*data = GetHexDigit(*str)*16 + GetHexDigit(*(str + 1));
		str += 2;
		data ++;
		data_len--;
	}
}

void ClientSERV::OnReceived()
{
	m_in_buffer = GetInBuffer();
	if (!SERV_connection_info.m_in_buffer.FillFrom(m_in_buffer))
		SERV_connection_info.SignalError(BS_NOT_ENOUGH_MEMORY, "Недостаточно памяти для входного буфера");
	{
		int sz = SERV_connection_info.m_in_buffer.SpaceFor(BS_RingBuffer_READ);
		LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, TXT("ClientSERV::OnReceived() used_size = %d") << sz);
	}
	
	static BS_TextPacket packet;
	static BS_ParamDescription param_description;
#define MAX_DATA_SIZE 100
	static byte data[MAX_DATA_SIZE];
	int data_len;
	while (packet.GetFromBuffer(&SERV_connection_info.m_in_buffer))
	{
		SERV_connection_info.m_received_packets++;

		{
			int sz = SERV_connection_info.m_in_buffer.SpaceFor(BS_RingBuffer_READ);
			LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, TXT("ClientSERV::OnReceived() used_size = %d, packet = %s") << sz << packet.m_packet_str);
		}
		if (packet.m_packet_str[0] == '%') 
		{
			if (packet.m_packet_str_length % 2 != 1) 
				continue;
			data_len = min((packet.m_packet_str_length - 1)/2, MAX_DATA_SIZE);
			HexStringToByteArray(packet.m_packet_str + 1, data, data_len);
			if (data[0] & 0x80) 
			{
				OnServerPacketReceived(data, data_len);
			}
		}
		else 
		{
		}
	}
}

bool SERV_RequestCalibrationTable(SERV_Channel* channel, bool from_device_certanly)
{
	struct ErrorsHandler
	{
		ErrorsHandler():
			m_error_occure(true), m_error_text("Необработанная ошибка")
		{}
		~ErrorsHandler()
		{
			if (m_error_occure)
				MessageBox(NULL, m_error_text, "Запрос калибровочной таблицы прерван", MB_OK);
		}
		bool m_error_occure;
		CString m_error_text;
	};
	ErrorsHandler error_handler;

	if (g_calibrating_channel != NULL)
	{
		error_handler.m_error_text = "g_calibrating_channel != NULL";
		return false;
	}

	if (channel == NULL || channel->GetCalibrationStage() != STAGE_IDLE)
	{
		error_handler.m_error_text = "channel == NULL || channel->GetCalibrationStage() != STAGE_IDLE";
		return false;
	}

	if (channel->GetCalibrationTableSize() > 0 && !from_device_certanly)
	{
		error_handler.m_error_occure = false;
		::PostMessage(SERV_connection_info.m_calibration_table_ready_wnd, SERV_connection_info.m_calibration_table_ready_msg, channel->GetUniqueNumber(), channel->GetConvNumber());
		return true;
	}

	if (!g_client.IsConnected())
	{
		error_handler.m_error_text = "Невозможно выполнить! Нет соединения с сервером";
		SERV_connection_info.SignalError(BS_UNKNOWN_ERROR, error_handler.m_error_text);
		return false;
	}

	g_calibrating_device = channel->GetDevice();
	if (g_calibrating_device == NULL)
	{
		error_handler.m_error_text = "g_calibrating_device == NULL";
		return false;
	}
	if (!StartCheckingThread())
	{
		error_handler.m_error_text = "!StartCheckingThread()";
		g_calibrating_device = NULL;
		return false;
	}

	g_calibrating_channel = channel;
	g_calibrating_channel->ResetCalibrationTable();

	LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Начало чтения калибровочной таблицы (устройство %s, канал %s") << g_calibrating_device->GetInfo() << g_calibrating_channel->GetInfo());

	SetTextOfCalibrationStage("Запрос размера таблицы размещения результатов", STATE_UNKNOWN);
	g_calibrating_channel->SetCalibrationStage(STAGE_REQUEST_RESULTS_TABLE_SIZE);
	g_waited_cal_mem_addr = RESULTS_TABLE_ADDRESS;
	g_waited_cal_device_addr = g_calibrating_device->GetAddr();
	ReadEEPROM(g_waited_cal_device_addr, g_waited_cal_mem_addr, 2, true); 
	error_handler.m_error_occure = false;
	return true;
}

bool SERV_WriteCalibrationTable(SERV_Channel* channel)
{
	if (g_calibrating_channel != NULL || channel == NULL || channel->GetCalibrationStage() != STAGE_IDLE)
		return false;

	g_calibrating_device = channel->GetDevice();
	if (g_calibrating_device == NULL)
		return false;
	if (!StartCheckingThread())
	{
		g_calibrating_device = NULL;
		return false;
	}

	LOG_V0_AddMessage(LOG_V0_MESSAGE, "Начало сохранения калибровочной таблицы");

	g_calibrating_channel = channel;
	g_calibrating_channel->SetCalibrationStage(STAGE_WRITE_ENABLE_WRITING);
	SetTextOfCalibrationStage("Получение разрешения на запись", STATE_UNKNOWN);
	g_waited_cal_device_addr = g_calibrating_device->GetAddr();
	g_waited_cal_mem_addr = 0x20; 
	byte data[2] = {0xA0, 0x01};
	WriteEEPROM(g_waited_cal_device_addr, g_waited_cal_mem_addr, data, 2, true); 
	return true;
}

void SERV_RequestMemory(HWND wnd, UINT msg, int device_num, byte* mem_ptr)
{
	if (!StartCheckingThread())
		return;

	LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Начало чтения из памяти (устройство %02Xh)") << device_num);
	g_mem_read = true;
	g_mem_wnd = wnd;
	g_mem_msg = msg;
	g_mem_ptr = mem_ptr;
	g_waited_mem_device_addr = device_num;
	g_waited_mem_mem_addr = 256; 
	g_waited_mem_mem_size = MAX_MEM_SIZE;
	ReadEEPROM(g_waited_mem_device_addr, g_waited_mem_mem_addr, g_waited_mem_mem_size, true);
}

void SERV_UploadMemory(HWND wnd, UINT msg, int device_num, byte* mem_ptr)
{
	if (!StartCheckingThread())
		return;

	{
		MutexWrap cfg_access(SERV_current_device_cfg_mutex);
		g_calibrating_device = SERV_current_device_cfg->GetDevice(device_num);
	}

	if (g_calibrating_device == NULL)
	{
		LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Ошибка устройство %02Xh не обнаружено") << device_num);
		return; 
	}

	LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Начало записи в память (устройство %02Xh)") << device_num);
	g_mem_read = false;
	g_mem_wnd = wnd;
	g_mem_msg = msg;
	g_mem_ptr = mem_ptr;
	g_waited_mem_device_addr = device_num;
	g_waited_mem_mem_addr = 0x20; 
	g_waited_mem_mem_size = 2;
	byte data[2] = {0xA0, 0x01};
	WriteEEPROM(g_waited_mem_device_addr, g_waited_mem_mem_addr, data, g_waited_mem_mem_size, true); 
}

int SERV_GetCurrentMemoryAddr()
{
	return g_waited_mem_mem_addr;
}

int SERV_GetRepeatOfLastCommand()
{
	return g_repeat_of_last_cmd;
}
CString OneWord(WORD w)
{
	CString str;
	str.Format("%02X%02X", w>>8, w&0xFF);
	return str;
}
SERV_DE::SERV_DE(byte length, FORMULA_Formula* formula):
	m_length(length), m_formula(formula),
	m_has_ustavka_and_limit(false), m_ustavka_addr(0), m_limit_addr(0), m_ustavka(0), m_limit(0)
{}

SERV_DE::~SERV_DE()
{
	if (m_formula)
		delete m_formula;
}

CString SERV_DE::GetInfo(int &offset)
{
	offset += m_length;
	CString res = "смещение: ";
	if (offset-m_length < 10)
		res += "  ";
	return res + (TXT("%d,  длина: %d,  данные: %s") << offset-m_length << m_length << m_formula->GetText(FORMULA_MODE_RESULT));
}

CString SERV_DE::GetString()
{
	byte status = FORMULA_CALC_STATUS_NOT_READY;
	DWORD result = (DWORD)m_formula->Calculate(status);
	if (m_length == 1)
		result &= 0xFF;
	else
	if (m_length == 2)
		result &= 0xFFFF;
	else
	if (m_length == 3)
		result &= 0xFFFFFF;
    CString cs = (TXT("0%dX") << (m_length*2));
	CString format = "%" + cs;
	return TXT(format) << result;
}

SERV_DE* SERV_DE::MakeCopy()
{
	SERV_DE* copy = new SERV_DE(m_length, m_formula->MakeCopy());
	copy->m_has_ustavka_and_limit = m_has_ustavka_and_limit;
	copy->m_ustavka_addr = m_ustavka_addr;
	copy->m_limit_addr = m_limit_addr;
	//copy->m_ustavka = m_ustavka;
	//copy->m_limit = m_limit;
	return copy;
}

void SERV_DE::Save(FILE* f)
{
	fputs(CString(TXT("LEN %d\n") << m_length), f);
	fputs("DAT " + m_formula->GetTextForSave() + "\n", f);
	fputs(CString(TXT("HAS %d\n") << (m_has_ustavka_and_limit ? 1 : 0)), f);
	if (m_has_ustavka_and_limit)
	{
		fputs(CString(TXT("UAD %04X\n") << m_ustavka_addr), f);
		fputs(CString(TXT("LAD %04X\n") << m_limit_addr), f);
	}
}

SERV_DE* SERV_DE::Load(FILE* f)
{
	char zzz[1024];
	int len, has, uad, lad;
	if (!fgets(zzz, 1024, f) || sscanf(zzz, "LEN %d", &len) != 1 || len <= 0)
		return NULL;
	if (!fgets(zzz, 1024, f))
		return NULL;
	FORMULA_Formula* formula = FORMULA_Formula::CreateFromText(zzz + 4);
	if (f == NULL)
		return NULL;
	if (!fgets(zzz, 1024, f) || sscanf(zzz, "HAS %d", &has) != 1 || !(has == 0 || has == 1))
	{
		delete formula;
		return NULL;
	}
	if (has == 1)
	{
		if (!fgets(zzz, 1024, f) || sscanf(zzz, "UAD %X", &uad) != 1 || uad <= 0)
		{
			delete formula;
			return NULL;
		}
		if (!fgets(zzz, 1024, f) || sscanf(zzz, "LAD %X", &lad) != 1 || lad <= 0)
		{
			delete formula;
			return NULL;
		}
	}
	SERV_DE* res = new SERV_DE(len, formula);
	res->m_has_ustavka_and_limit = (has == 1);
	res->m_ustavka_addr = (has == 1) ? uad : 0;
	res->m_ustavka = 0xFFFF;
	res->m_limit_addr = (has == 1) ? lad : 0;
	res->m_limit = 0xFFFF;
	return res;
}
SERV_BO::SERV_BO():
	m_device_addr(0), m_device_addr_ust(0), m_request_stage(SERV_RS_READY/*SERV_RS_IDLE*/)
{}

SERV_BO::~SERV_BO()
{
	SERV_DEs::iterator current = m_data_elements.begin();
	while (current != m_data_elements.end())
	{
		delete (*current);
		current++;
	}	
}

CString SERV_BO::GetInfo()
{
	return TXT("Устройство %02Xh, %02Xh") << m_device_addr << m_device_addr_ust;
}

CString SERV_BO::GetString()
{
	CString data_str;
	byte data_length = 0;
	SERV_DEs::iterator current = m_data_elements.begin();
	while (current != m_data_elements.end())
	{
		data_length += (*current)->m_length;
		data_str += (*current)->GetString();
		current++;
	}
	CString res;
	res.Format("@JOB#000#%02X%02X0200%02X%s00$", m_device_addr, data_length+7, data_length, data_str);
	return res;
}

SERV_BO* SERV_BO::MakeCopy()
{
	SERV_DE* copy_de;
	SERV_BO* copy = new SERV_BO;
	copy->m_device_addr = m_device_addr;
	copy->m_device_addr_ust = m_device_addr_ust;
	copy->m_request_stage = SERV_RS_READY;
	SERV_DEs::iterator current = m_data_elements.begin();
	while (current != m_data_elements.end())
	{
		copy_de = (*current)->MakeCopy();
		copy->m_data_elements.push_back(copy_de);
		current++;
	}
	return copy;
}

void SERV_BO::Save(FILE* f)
{
	fputs(CString(TXT("DEV %02X\n") << m_device_addr), f);
	fputs(CString(TXT("UST %02X\n") << m_device_addr_ust), f);
	fputs(CString(TXT("%d\n") << m_data_elements.size()), f);
	SERV_DEs::iterator current = m_data_elements.begin();
	while (current != m_data_elements.end())
	{
		(*current)->Save(f);
		current++;
	}
}

SERV_BO* SERV_BO::Load(FILE* f)
{
	char zzz[1024];
	int ust = 0, dev, size;
	if (!fgets(zzz, 1024, f) || sscanf(zzz, "DEV %X", &dev) != 1 || dev <= 0)
		return NULL;
	if (g_bo_file_version == 2)
	{
		if (!fgets(zzz, 1024, f) || sscanf(zzz, "UST %X", &ust) != 1 || ust < 0)
			return NULL;
	}
	if (!fgets(zzz, 1024, f) || sscanf(zzz, "%d", &size) != 1 || size < 0)
		return NULL;
	SERV_BO* res = new SERV_BO();
	SERV_DE* de;
	for (int i = 0; i < size; i++)
	{
		de = SERV_DE::Load(f);
		if (de == NULL)
		{
			delete res;
			return NULL;
		}
		res->m_data_elements.push_back(de);
	}
	res->m_device_addr = dev;
	res->m_device_addr_ust = ust;
	res->m_request_stage = SERV_RS_READY;
	return res;
}
bool SERV_SaveConfigBO(CString path)
{
	FILE*f = fopen(path + "\\БО.cfg", "wt");
	if (!f)
		return false;

	MutexWrap mutex_BO_access(SERV_mutex_BO);

	fputs(CString(TXT("VER %d\n") << SERV_BO_FILE_VERSION), f);
	fputs(CString(TXT("NUM %d\n") << SERV_indication_blocks.size()), f);
	SERV_BOs::iterator current = SERV_indication_blocks.begin();
	while (current != SERV_indication_blocks.end())
	{
		(*current)->Save(f);
		current++;
	}
	fclose(f);
	return true;
}

bool SERV_LoadConfigBO(CString path)
{
	struct FileCloser
	{
		FileCloser(FILE*f):
			m_file(f)
		{
		}
		~FileCloser()
		{
			if (m_file)
				fclose(m_file);
		}
		FILE* m_file;
	};
	FILE*f = fopen(path + "\\БО.cfg", "rt");
	if (!f)
		return false;
	FileCloser file_closer(f);

	MutexWrap mutex_BO_access(SERV_mutex_BO);

	char zzz[1024];
	int num;
	if (!fgets(zzz, 1024, f) || sscanf(zzz, "VER %d", &g_bo_file_version) != 1/* || g_bo_file_version != SERV_BO_FILE_VERSION*/)
		return false;
	if (!fgets(zzz, 1024, f) || sscanf(zzz, "NUM %d", &num) != 1 || num < 0)
		return false;

	SERV_ClearListBO(SERV_indication_blocks);
	SERV_BO* bo;
	for (int i = 0; i < num; i++)
	{
		bo = SERV_BO::Load(f);
		if (bo == NULL)
			return false;
		SERV_indication_blocks.push_back(bo);
	}
	return true;
}

void SERV_ClearListBO(SERV_BOs& list_bo)
{
	SERV_BOs::iterator current_bo = list_bo.begin();
	while (current_bo != list_bo.end())
	{
		delete *current_bo;
		current_bo++;
	}
	list_bo.clear();
}
void SERV_SendIndication()
{
	if (!g_client.IsConnected())
		return;

	MutexWrap mutex_BO_access(SERV_mutex_BO);
	CString req;

	SERV_BOs::iterator current = SERV_indication_blocks.begin();
	while (current != SERV_indication_blocks.end())
	{
		if ((*current)->m_request_stage == SERV_RS_READY)
		{
			req = (*current)->GetString();
			g_client.SendData(req, req.GetLength());
		}
		current++;
	}
}
SERV_ConnectionInfo::SERV_ConnectionInfo()
{
	m_server_ip = "localhost";
	m_server_port = 56000;
}

bool SERV_ConnectionInfo::IsConnected()
{
	return g_client.IsConnected();
}

void SERV_ConnectionInfo::SignalError(int error_code, CString reason)
{
	LOG_V0_AddMessage(LOG_V0_MESSAGE, "Ошибка при работе с DSN_Server: " + reason);
	BS_ConnectionInfo::SignalError(error_code, reason);
}

void SERV_SendParamToBKSD(CParam* param, WORD val)
{
	if (param == NULL || SERV_current_device_cfg == NULL)
		return;

	int channel_num = -1;

	if (param->m_channel_num == -1)
	{
		if (param->m_formula == NULL)
			return;
		channel_num = param->m_formula->CorrespondingChannelNum();
	}
	else
		channel_num = param->m_channel_num;

	if (channel_num == -1)
		return;

	SERV_Channel* channel = SERV_current_device_cfg->GetChannel(channel_num);

	if (channel == NULL)
		return;

	SERV_Device* device = channel->GetDevice();

	if (device == NULL)
		return;

	byte specific_addr = (param == pTalblock) ? 0x40 : 0x50; 
	byte data[4] = {0xC1, specific_addr, (val >> 8), (val & 0xFF)};

	WriteEEPROM(device->GetAddr(), 0x20, data, 4, false);
}

////////////////////////////////////////////////////////////////////////////////
// end