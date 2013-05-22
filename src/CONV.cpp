#include <StdAfx.h>
//#pragma warning(disable:4786)
#include <map>
#include "CONV.h"
#include "SERV.h"
#include "../KRS.h"
#include "../FUNC.h"
#include "LOG_V0.h"
#include "TXT.h"
#include "../KRS.h"

#include "../MainFrm.h"

CMainFrame* g_main_frame = NULL;

CONV_Channel::CONV_Channel(CString name, SERV_Device* device, WORD num):
	SERV_Channel(name), m_last_received_tick(0)
{
	m_channel_num = num;
}

CString CONV_Channel::GetInfo()
{
	CString res;
	res.Format("%s (%d)", m_name, GetUniqueNumber());
	return res;
}

CString CONV_Channel::GetExtendedInfo()
{
	CString res;
	res.Format("%s (конв № %d)", m_name, GetUniqueNumber());
	return res;
}

WORD CONV_Channel::GetUniqueNumber()
{
	return m_channel_num;
}

WORD CONV_Channel::GetUniqueNumber(byte dev_addr)
{
	return m_channel_num;
}

bool CONV_Channel::IsTimedOut()
{
	return GetTickCount() - m_last_received_tick > 1000;
}

CONV_ConnectionInfo CONV_connection_info;
struct CurrentMeasurenemtStore
{
	CurrentMeasurenemtStore():
		m_array_size(0), m_last_data_tick(0)
	{
	}
	void UpdateChannel(int m_channel_num, BS_ParamValueType channel_value)
	{
		m_channel_values[m_channel_num] = channel_value;
		m_last_data_tick = GetTickCount();
		m_measurement.m_time = KRS_syncro_ole_time.m_dt + TicksToOleTime(m_last_data_tick - KRS_syncro_tick);
	}

	BS_Measurement* GetMeasurement()
	{
		int size = m_channel_values.size();
		bool resize = (m_measurement.m_param_values == NULL || size > m_array_size);
		if (resize)
		{
			if (m_measurement.m_param_values != 0)
				delete[] m_measurement.m_param_values;
			m_array_size = size;
			if (size > 0)
				m_measurement.m_param_values = new BS_OneParamValue[m_array_size];
			else
				m_measurement.m_param_values = NULL;
		}
		m_measurement.m_param_values_size = size;
		BS_OneParamValue* current_param_value = m_measurement.m_param_values;
		map<int, BS_ParamValueType>::iterator current_channel = m_channel_values.begin();
		while (current_channel != m_channel_values.end())
		{
			current_param_value->m_param_index = current_channel->first;
			current_param_value->m_param_value = current_channel->second;
			current_param_value++;
			current_channel++;
		}
		return &m_measurement;
	}

	DWORD m_last_data_tick;
protected:
	map<int, BS_ParamValueType> m_channel_values;
	BS_Measurement m_measurement;
	int m_array_size;
};

CurrentMeasurenemtStore g_measurement_store;

struct ClientCONV: NET_Client
{
	ClientCONV():
		m_middle_in_buffer(NULL), m_middle_in_buffer_size(0)
	{
		SetBuffersSize(BS_MAX_TEXT_PACKET_LENGTH, BS_MAX_TEXT_PACKET_LENGTH, 20*BS_MAX_TEXT_PACKET_LENGTH, 20*BS_MAX_TEXT_PACKET_LENGTH);
	}

	~ClientCONV()
	{
		if (m_middle_in_buffer)
			delete[] m_middle_in_buffer;
	}

	virtual CString GetName() { return "ClientCONV"; }

	virtual void OnConnected()
	{
		LOG_V0_AddMessage(LOG_V0_MESSAGE, "Соединение с DSN_Convertor установлено");

		m_prev_channel_num = -1;
		g_main_frame = (CMainFrame*)(AfxGetMainWnd());
		PostMessage(CONV_connection_info.m_wnd, CONV_connection_info.m_initialization_complete_msg, 0, 0);
	}

	virtual void OnBreakedConnection()
	{
		CONV_connection_info.SignalError(BS_CONNECTION_BROKEN, "Соединение разорвано");
	}

	virtual void OnError(CString err)
	{
		CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, err);
	}

	NET_Buffer *m_in_buffer;
	int m_prev_channel_num, m_channel_num;
	virtual void OnReceived()
	{
		m_in_buffer = GetInBuffer();
		if (!CONV_connection_info.m_in_buffer.FillFrom(m_in_buffer))
			CONV_connection_info.SignalError(BS_NOT_ENOUGH_MEMORY, "Недостаточно памяти для входного буфера");

		BS_TextPacket packet;
		static int dev, addr;
		static char str_channel_info[BS_MAX_TEXT_PACKET_LENGTH], *str_channel_value;
		static BS_ParamValueType channel_value;
		while (packet.GetFromBuffer(&CONV_connection_info.m_in_buffer))
		{
			CONV_connection_info.m_received_packets++;
			if (packet.m_packet_str[0] == 'C' && CONV_connection_info.m_measurement_started) 
			{
				if (sscanf(packet.m_packet_str, "CH%d#%s", &m_channel_num, &str_channel_info) != 2)
				{
					CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, TXT("Получен непонятный пакет: %s") << packet.m_packet_str);
					continue;
				}
				if (m_prev_channel_num >= m_channel_num)
				{
					//g_main_frame->ConvDataReady();
					::SendNotifyMessage(CONV_connection_info.m_wnd, CONV_connection_info.m_data_received_msg, 0, 0);
				}
				m_prev_channel_num = m_channel_num;

				str_channel_value = str_channel_info + 3;
				if (str_channel_value[0] == 0)
					continue;
				if (sscanf(str_channel_value, "%f", &channel_value) != 1) 
					continue; //CRUSH("Wrong param value");
				g_measurement_store.UpdateChannel(m_channel_num, channel_value);
			}
			else
			if (packet.m_packet_str[0] == '#' && packet.m_packet_str_length > 7)
			{
				if (sscanf(packet.m_packet_str, "#L#%d#", &m_channel_num) == 1)
				{
					BS_ParamDescription param_description;
					param_description.m_param_index = m_channel_num;

					char *str_end = packet.m_packet_str + 3, *str_start = str_end;
					BS_StringParamDescriptionField *str_field;
					BS_ParamValueParamDescriptionField *value_field;

					while (*str_end != '#' && str_end - packet.m_packet_str < BS_MAX_TEXT_PACKET_LENGTH)
						str_end++;
					str_end++;
					str_start = str_end;

					str_field = new BS_StringParamDescriptionField();
					str_field->m_field_index = BS_FIELD_NAME; 
					while (*str_end != '#' && str_end - packet.m_packet_str < BS_MAX_TEXT_PACKET_LENGTH)
						str_end++;
					if (str_end - str_channel_info >= BS_MAX_TEXT_PACKET_LENGTH)
						CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, "Неправильная строка в описании параметра");
					str_field->m_text_length = str_end - str_start;
					str_field->m_text_string = new char[str_field->m_text_length + 1];
					memcpy(str_field->m_text_string, str_start, str_field->m_text_length);
					str_field->m_text_string[str_field->m_text_length] = 0;
					param_description.m_param_description_fields[str_field->m_field_index] = str_field;
					str_end++;
					str_start = str_end;

					str_field = new BS_StringParamDescriptionField();
					str_field->m_field_index = BS_FIELD_MU; 
					while (*str_end != '#' && str_end - packet.m_packet_str < BS_MAX_TEXT_PACKET_LENGTH)
						str_end++;
					if (str_end - str_channel_info >= BS_MAX_TEXT_PACKET_LENGTH)
						CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, "Неправильная строка в описании параметра");
					str_field->m_text_length = str_end - str_start;
					str_field->m_text_string = new char[str_field->m_text_length + 1];
					memcpy(str_field->m_text_string, str_start, str_field->m_text_length);
					str_field->m_text_string[str_field->m_text_length] = 0;
					param_description.m_param_description_fields[str_field->m_field_index] = str_field;
					str_end++;
					str_start = str_end;

					value_field = new BS_ParamValueParamDescriptionField();
					value_field->m_field_index = BS_FIELD_MINIMAL_VALUE; 
					while (*str_end != '#' && str_end - packet.m_packet_str < BS_MAX_TEXT_PACKET_LENGTH)
						str_end++;
					if (str_end - str_channel_info >= BS_MAX_TEXT_PACKET_LENGTH)
						CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, "Неправильная строка в описании параметра");
					if (sscanf(str_start, "%f", &value_field->m_param_value) != 1)
						CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, "Отсутствует число в описании параметра (минимальное значение)");
					param_description.m_param_description_fields[value_field->m_field_index] = value_field;
					str_end++;
					str_start = str_end;

					value_field = new BS_ParamValueParamDescriptionField();
					value_field->m_field_index = BS_FIELD_MAXIMAL_VALUE; 
					while (*str_end != '#' && str_end - packet.m_packet_str < BS_MAX_TEXT_PACKET_LENGTH)
						str_end++;
					if (str_end - str_channel_info >= BS_MAX_TEXT_PACKET_LENGTH)
						CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, "Неправильная строка в описании параметра");
					if (sscanf(str_start, "%f", &value_field->m_param_value) != 1)
						CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, "Отсутствует число в описании параметра (максимальное значение)");
					param_description.m_param_description_fields[value_field->m_field_index] = value_field;
					str_end++;
					str_start = str_end;

					value_field = new BS_ParamValueParamDescriptionField();
					value_field->m_field_index = BS_FIELD_UPPER_BLOCK_VALUE; 
					while (*str_end != '#' && str_end - packet.m_packet_str < BS_MAX_TEXT_PACKET_LENGTH)
						str_end++;
					if (str_end - str_channel_info >= BS_MAX_TEXT_PACKET_LENGTH)
						CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, "Неправильная строка в описании параметра");
					if (sscanf(str_start, "%f", &value_field->m_param_value) != 1)
						CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, "Отсутствует число в описании параметра (блокиновочное значение)");
					param_description.m_param_description_fields[value_field->m_field_index] = value_field;
					str_end++;
					str_start = str_end;

					value_field = new BS_ParamValueParamDescriptionField();
					value_field->m_field_index = BS_FIELD_CRUSH_VALUE; 
					while (*str_end != '#' && str_end - packet.m_packet_str < BS_MAX_TEXT_PACKET_LENGTH)
						str_end++;
					if (str_end - str_channel_info >= BS_MAX_TEXT_PACKET_LENGTH)
						CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, "Неправильная строка в описании параметра");
					if (sscanf(str_start, "%f", &value_field->m_param_value) != 1)
						CONV_connection_info.SignalError(BS_UNKNOWN_ERROR, "Отсутствует число в описании параметра (аварийное значение)");
					param_description.m_param_description_fields[value_field->m_field_index] = value_field;
					str_end++;
					str_start = str_end;

					CONV_connection_info.m_param_descriptions[param_description.m_param_index] = param_description;
				}
			}
			else
			if (packet.m_packet_str[0] == 'L')
			{
				if (packet.m_packet_str_length == 7)
					if (memcmp(packet.m_packet_str, "LISTEND", 7) == 0)
					{
						PostMessage(CONV_connection_info.m_wnd, CONV_connection_info.m_params_info_ready_msg, 0, 0);
					}
			}
		}
	}

	char *m_middle_in_buffer;
	DWORD m_middle_in_buffer_size;
};
static ClientCONV g_client;
static HANDLE g_connection_closing_event = CreateEvent(NULL, FALSE, FALSE, NULL);
static bool g_inited = false;
static UINT ConnectionThreadProcedure(LPVOID param)
{
	HANDLE multiple_events[2] = {g_connection_closing_event, CONV_connection_info.m_data_ready_for_sending_event};
	DWORD res;
	LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Попытка соединения с DSN_Convertor: ip = %s, port = %d") << CONV_connection_info.m_server_ip << CONV_connection_info.m_server_port);
	g_client.StartConnect(CONV_connection_info.m_server_ip, CONV_connection_info.m_server_port);
	while (true)
	{
		res = WaitForMultipleObjects(2, multiple_events, FALSE, INFINITE);
		if (res == WAIT_OBJECT_0) 
			break;
		if (res == WAIT_OBJECT_0 + 1 && g_client.IsConnected()) 
		{
			//g_client.SendAllReadyData();
		}
	}
	g_client.StopConnect();
	SetEvent(CONV_connection_info.m_connection_closed_event);
	g_inited = false;
	return +1;
}
void CONV_InitConnection()
{
	if (KRS_shutdown)
		return;
		
	g_inited = true;
	CONV_StopMeasure();
	ResetEvent(CONV_connection_info.m_connection_closed_event);
	AfxBeginThread(ConnectionThreadProcedure, THREAD_PRIORITY_NORMAL);
}
void CONV_ShutDownConnection(bool sync)
{
	m_wndDialogBar.m_ind_conv.SetState(STATE_UNKNOWN);
	if (!g_inited)
		return;
	CONV_StopMeasure();
	SetEvent(g_connection_closing_event);
	if (sync)
		WaitForSingleObject(CONV_connection_info.m_connection_closed_event, INFINITE);
}

void CONV_RequestParamInfo()
{
	if (!g_client.IsConnected())
	{
		CONV_connection_info.SignalError(BS_CONNECTION_BROKEN, "Попытка отправить данные при неустановленном соединении");
		return;
	}

	map<int, BS_ParamDescription>::iterator current = CONV_connection_info.m_param_descriptions.begin();
	while (current != CONV_connection_info.m_param_descriptions.end())
	{
		current->second.Delete();
		current++;
	}
	CONV_connection_info.m_param_descriptions.clear();

	char *connnected_packed = "@LIST$@LISTDEV$";
	g_client.SendData(connnected_packed, strlen(connnected_packed));
}

void CONV_StartMeasure()
{
	CONV_connection_info.m_measurement_started = true;
}

void CONV_StopMeasure()
{
	CONV_connection_info.m_measurement_started = false;
}

BS_Measurement* CONV_GetMeasurement(bool &timeout)
{
	timeout = (GetTickCount() - g_measurement_store.m_last_data_tick) > 1000;
	return g_measurement_store.GetMeasurement();
}

DWORD CONV_GetLastTick()
{
	return g_measurement_store.m_last_data_tick;
}

CONV_ConnectionInfo::CONV_ConnectionInfo():
	BS_ConnectionInfo(2000, 0), m_params_info_ready_msg(0),
	m_received_packets(0), m_measurement_started(false),
	m_get_measurement_pos(0), m_receive_measurement_pos(0), m_max_param_size(0)
{
	m_server_ip = "localhost";
	m_server_port = 58000;
}

CONV_ConnectionInfo::~CONV_ConnectionInfo()
{
	map<int, BS_ParamDescription>::iterator current = m_param_descriptions.begin();
	while (current != m_param_descriptions.end())
	{
		current->second.Delete();
		current++;
	}
}

bool CONV_ConnectionInfo::IsConnected()
{
	return g_client.IsConnected();
}

void CONV_ConnectionInfo::SignalError(int error_code, CString reason)
{
	BS_ConnectionInfo::SignalError(error_code, reason);
	LOG_V0_AddMessage(LOG_V0_MESSAGE, "Ошибка при работе с DSN_Convertor: " + reason);
}

////////////////////////////////////////////////////////////////////////////////
// end