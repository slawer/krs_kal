#include <StdAfx.h>
#include <check.h>
#include "DB.h"
#include "NET.h"
#include "TXT.h"
#include "LOG_V0.h"
#include "..\KRS.h"
#include "..\MainFrm.h"

CString DB_current_status_str = "Связь не инициализирована";

DB_ConnectionInfo DB_connection_info;
map<int, BS_ParamDescription> DB_params_from_db;
map<int, BS_MarkerType> DB_marker_types;

CString CharToString(char* str, int len)
{
	if (str == NULL || len == 0)
		return "<пусто>";

	str[len] = '\0';
	CString res = str;
	return res.Left(len);
}

struct StoreElement
{
	StoreElement(): m_values(NULL), m_values_size(0), m_used_values_size(0)
	{}
	~StoreElement()
	{
		if (m_values)
			delete[] m_values;
	}

	void Set(BS_Measurement* m)
	{
		bool resize = (m_values == NULL || m_values_size < m->m_param_values_size);
		CHECK(m->m_param_values_size > 0);
		if (resize)
		{
			if (m_values)
				delete[] m_values;
			m_values_size = m->m_param_values_size;
			m_values = new BS_OneParamValue[m_values_size];
		}
		m_used_values_size = m->m_param_values_size;
		memcpy(m_values, m->m_param_values, m_used_values_size*sizeof(BS_OneParamValue));
		m_time = m->m_time;
		m_deepness = m->m_deepness;
	}

	friend struct MeasurementStoreDB;
protected:
	int m_values_size, m_used_values_size;
	BS_OneParamValue *m_values;
	BS_Time m_time;
	BS_ParamValueType m_deepness;
};

struct MeasurementStoreDB
{
	MeasurementStoreDB(int size_for_send): m_store_size(size_for_send), m_used_store_size(0)
	{
		CHECK(m_store_size > 0 && m_store_size < 100000);
		m_store = new StoreElement[m_store_size];

		m_transfer_packet.m_measurements = new BS_Measurement[m_store_size];
		m_transfer_packet.m_transaction_type = 0;
		m_last_send_time = 0;
	}
	~MeasurementStoreDB()
	{
		if (m_store)
			delete[] m_store;
	}

	void Clear()
	{
		m_used_store_size = 0;
		m_last_send_time = 0;
	}

	friend bool DB_SendParamValues(BS_Measurement *m);
	friend static UINT ConnectionThreadProcedure(LPVOID param);
	
protected:
	bool Add(BS_Measurement* m)
	{
		if (m_used_store_size == m_store_size)
			return false;
		m_store[m_used_store_size].Set(m);
		m_used_store_size++;
		if (m_last_send_time == 0)
			m_last_send_time = m->m_time;
		if (m_used_store_size == m_store_size || m->m_time - m_last_send_time > 10.0/(24*60*60)) 
		{
			m_last_send_time = m->m_time;
			return SendStoredData();
		}
		return true;
	}

private:
	bool SendStoredData()
	{
		if (m_used_store_size == 0)
			return true;
		BS_Measurement *m = m_transfer_packet.m_measurements;
		StoreElement *se = m_store;
		int i;
		for(i=0; i<m_used_store_size; i++, m++, se++)
		{
			m->m_deepness = se->m_deepness;
			m->m_time = se->m_time;
			m->m_param_values = se->m_values; 
			m->m_param_values_size = se->m_used_values_size;
		}
		m_transfer_packet.m_measurements_size = m_used_store_size;
		bool res = DB_SendPacket(&m_transfer_packet);
		m = m_transfer_packet.m_measurements;
		for(i=0; i<m_used_store_size; i++, m++)
			m->m_param_values = NULL;
		m_transfer_packet.m_measurements_size = m_used_store_size;

		if (res)
			m_used_store_size = 0;
		return res;
	}

	int m_store_size, m_used_store_size;
	StoreElement *m_store;
	DB_TransferParamValuesPacket m_transfer_packet;
	double m_last_send_time;
};

static MeasurementStoreDB g_store(100);
struct ClientDB: NET_Client
{
	ClientDB():
		m_middle_in_buffer(NULL),
		m_middle_out_buffer(NULL), m_middle_out_buffer_size(0)
	{
		m_middle_in_buffer_size = 1024*300;
		m_middle_in_buffer = new char[m_middle_in_buffer_size];
		m_middle_out_buffer_size = 1024*100;
		m_middle_out_buffer = new char[m_middle_out_buffer_size];
		SetBuffersSize(m_middle_in_buffer_size, m_middle_in_buffer_size * 10, m_middle_out_buffer_size, m_middle_out_buffer_size*10);
	}

	~ClientDB()
	{
		if (m_middle_in_buffer)
		{
			delete[] m_middle_in_buffer;
			m_middle_in_buffer = NULL;
		}
		if (m_middle_out_buffer)
		{
			delete[] m_middle_out_buffer;
			m_middle_out_buffer = NULL;
		}

		map<int, BS_ParamDescription>::iterator current = DB_params_from_db.begin();
		while (current != DB_params_from_db.end())
		{
			current->second.Delete();
			current++;
		}
		DB_params_from_db.clear();
	}

	virtual CString GetName() { return "ClientDB"; }

	virtual void OnConnected()
	{
		m_wndDialogBar.m_ind_db.SetState(STATE_OFF);
		DB_current_status_str = "Соединение с КБД установлено";
		LOG_V0_AddMessage(LOG_V0_MESSAGE, "Соединение с КБД установлено");

		DB_connection_info.ResetListsInfo();
		g_store.Clear();

		DB_LoginPasswordPacket lp_packet(DB_connection_info.m_db_server, DB_connection_info.m_db_name, DB_connection_info.m_login, DB_connection_info.m_password);
		DB_SendPacket(&lp_packet);
		//PostMessage(DB_connection_info.m_wnd, DB_connection_info.m_initialization_complete_msg, 0, 0);
	}

	virtual void OnBreakedConnection()
	{
		DB_connection_info.m_client_version = 0; 
		DB_connection_info.m_db_version = -1; 
		DB_connection_info.ResetListsInfo();
		g_store.Clear();

		m_wndDialogBar.m_ind_db.SetState(STATE_UNKNOWN);
		if (m_send_msg)
			DB_connection_info.SignalError(BS_CONNECTION_BROKEN, "Соединение разорвано");
		DB_connection_info.m_out_buffer.Clear();
	}

	virtual void OnError(CString err)
	{
		int err_num = GetLastError();
		if (err_num != WSAECONNABORTED)
		{
			DB_connection_info.SignalError(BS_UNKNOWN_ERROR, TXT("Неизвестная ошибка: (%06X) %s") << err_num << err);
		}
	}

	NET_Buffer *m_in_buffer;

	virtual void OnReceived()
	{
		int ready_data_size = -1;
		m_in_buffer = GetInBuffer();
		{
			MutexWrap in_read_access(m_in_buffer->m_r_mutex);
			{
				MutexWrap in_write_access(m_in_buffer->m_r_mutex);
				ready_data_size = m_in_buffer->NoLockSpaceFor(RingBuffer_READ);
			}
			CHECK(ready_data_size > 0);
			bool resize = (m_middle_in_buffer == NULL || (int)m_middle_in_buffer_size < ready_data_size);
			if (resize)
			{
				if (m_middle_in_buffer)
					delete[] m_middle_in_buffer;
				m_middle_in_buffer_size = ready_data_size;
				m_middle_in_buffer = new char[m_middle_in_buffer_size];
				if (m_middle_in_buffer == NULL)
				{
					DB_connection_info.SignalError(BS_NOT_ENOUGH_MEMORY, "Недостаточно памяти для входного буфера");
					return;
				}
			}
			m_in_buffer->Read(m_middle_in_buffer, ready_data_size);
		}
		while (DB_connection_info.m_in_buffer.SpaceFor(BS_RingBuffer_WRITE) < ready_data_size)
		{
			DB_connection_info.OnDataReady();
			if (KRS_shutdown) 
				return;
		}
		DB_connection_info.m_in_buffer.FillFrom(m_middle_in_buffer, ready_data_size);
		DB_connection_info.OnDataReady();
	}

	char *m_middle_in_buffer, *m_middle_out_buffer;
	DWORD m_middle_in_buffer_size, m_middle_out_buffer_size;
	bool m_send_msg;

	friend UINT ConnectionThreadProcedure(LPVOID param);

protected:
	void SendAllReadyData()
	{
		int ready_data_size = DB_connection_info.m_out_buffer.SpaceFor(BS_RingBuffer_READ);
		bool resize = (m_middle_out_buffer == NULL || (int)m_middle_out_buffer_size < ready_data_size);
		if (resize)
		{
			if (m_middle_out_buffer)
				delete[] m_middle_out_buffer;
			m_middle_out_buffer_size = ready_data_size;
			m_middle_out_buffer = new char[m_middle_out_buffer_size];
			if (m_middle_out_buffer == NULL)
			{
				DB_connection_info.SignalError(BS_NOT_ENOUGH_MEMORY, "Недостаточно памяти для выходного буфера");
				return;
			}
		}
		if (ready_data_size > 0)
		{
			DB_connection_info.m_out_buffer.FillTo(m_middle_out_buffer, ready_data_size);
			if (IsConnected())
				SendData(m_middle_out_buffer, ready_data_size);
			else
				DB_connection_info.SignalError(BS_UNKNOWN_ERROR, "Попытка отправки данных при отсутствии соединения");
		}
	}
};

static ClientDB g_client;
static HANDLE g_connection_closing_event = CreateEvent(NULL, FALSE, FALSE, NULL);
static bool g_inited = false;
static UINT ConnectionThreadProcedure(LPVOID param)
{
	HANDLE multiple_events[2] = {g_connection_closing_event, DB_connection_info.m_data_ready_for_sending_event};
	DWORD res;
	LOG_V0_AddMessage(LOG_V0_MESSAGE, TXT("Попытка соединения с КБД: ip = %s, port = %d") << DB_connection_info.m_server_ip << DB_connection_info.m_server_port);
	DB_current_status_str = "Попытка установить соединение";
	g_client.StartConnect(DB_connection_info.m_server_ip, DB_connection_info.m_server_port);
	while (true)
	{
		res = WaitForMultipleObjects(2, multiple_events, FALSE, INFINITE);
		if (res == WAIT_OBJECT_0) 
			break;
		if (res == WAIT_OBJECT_0 + 1) 
			g_client.SendAllReadyData();
	}
	if (g_client.IsConnected())
	{
		g_store.SendStoredData();
		g_client.SendAllReadyData();
	}
	g_client.StopConnect();
	DB_current_status_str = "Соединение разорвано";
	SetEvent(DB_connection_info.m_connection_closed_event);
	return +1;
}
void DB_SendPacketToCreateDB(CString db_server, CString db_name, CString login, CString password)
{
	DB_current_status_str = "Создание новой БД";
	if (DB_connection_info.m_client_version >= 3)
	{
		DB_CreateDB2Packet create_db_packet(db_server, db_name, DB_VERSION, login, password);
		DB_SendPacket(&create_db_packet); 
	}
	else
	{
		DB_CreateDBPacket create_db_packet(db_server, db_name, login, password);
		DB_SendPacket(&create_db_packet); 
	}
}

void DB_SendParamDescriptionToDB(CParam* param)
{
	DB_TransferParamDescriptionPacket transfer_param_packet;
	transfer_param_packet.m_transaction_type = 0;
	
	BS_ParamDescription *param_description = &transfer_param_packet.m_param_description;
	BS_StringParamDescriptionField *param_name_field = new BS_StringParamDescriptionField;
	param_name_field->m_field_index = BS_FIELD_NAME;
	BS_StringParamDescriptionField *param_mu_field = new BS_StringParamDescriptionField;
	param_mu_field->m_field_index = BS_FIELD_MU;
	BS_ParamValueParamDescriptionField *param_min_val_field = new BS_ParamValueParamDescriptionField;
	param_min_val_field->m_field_index = BS_FIELD_MINIMAL_VALUE;
	BS_ParamValueParamDescriptionField *param_max_val_field = new BS_ParamValueParamDescriptionField;
	param_max_val_field->m_field_index = BS_FIELD_MAXIMAL_VALUE;
	BS_ParamValueParamDescriptionField *param_block_val_field = new BS_ParamValueParamDescriptionField;
	param_block_val_field->m_field_index = BS_FIELD_UPPER_BLOCK_VALUE;
	BS_ParamValueParamDescriptionField *param_crush_val_field = new BS_ParamValueParamDescriptionField;
	param_crush_val_field->m_field_index = BS_FIELD_CRUSH_VALUE;
	BS_ParamValueParamDescriptionField *param_graf_diapazon_field = new BS_ParamValueParamDescriptionField;
	param_graf_diapazon_field->m_field_index = BS_FIELD_GRAPH_DIAPAZON;
	BS_ParamValueParamDescriptionField *param_graf_min_field = new BS_ParamValueParamDescriptionField;
	param_graf_min_field->m_field_index = BS_FIELD_GRAPH_MIN;
	BS_ParamValueParamDescriptionField *param_graf_max_field = new BS_ParamValueParamDescriptionField;
	param_graf_max_field->m_field_index = BS_FIELD_GRAPH_MAX;

	param_description->m_param_index = param->m_num_par;

	param_description->m_param_description_fields[BS_FIELD_NAME] = param_name_field;
	param_description->m_param_description_fields[BS_FIELD_MU] = param_mu_field;
	param_description->m_param_description_fields[BS_FIELD_MINIMAL_VALUE] = param_min_val_field;
	param_description->m_param_description_fields[BS_FIELD_MAXIMAL_VALUE] = param_max_val_field;
	param_description->m_param_description_fields[BS_FIELD_UPPER_BLOCK_VALUE] = param_block_val_field;
	param_description->m_param_description_fields[BS_FIELD_CRUSH_VALUE] = param_crush_val_field;
	param_description->m_param_description_fields[BS_FIELD_GRAPH_DIAPAZON] = param_graf_diapazon_field;
	param_description->m_param_description_fields[BS_FIELD_GRAPH_MIN] = param_graf_min_field;
	param_description->m_param_description_fields[BS_FIELD_GRAPH_MAX] = param_graf_max_field;

	if (param_name_field->m_text_string)
		delete[] param_name_field->m_text_string;
	param_name_field->m_text_length = param->sName.GetLength();
	if (param_name_field->m_text_length > 0)
	{
		param_name_field->m_text_string = new char[param_name_field->m_text_length];
		memcpy(param_name_field->m_text_string, param->sName, param_name_field->m_text_length);
	}
	else
		param_name_field->m_text_string = NULL;

	if (param_mu_field->m_text_string)
		delete[] param_mu_field->m_text_string;
	param_mu_field->m_text_length = param->sRazmernPar.GetLength();
	if (param_mu_field->m_text_length > 0)
	{
		param_mu_field->m_text_string = new char[param_mu_field->m_text_length];
		memcpy(param_mu_field->m_text_string, param->sRazmernPar, param_mu_field->m_text_length);
	}
	else
		param_mu_field->m_text_string = NULL;

	param_min_val_field->m_param_value = param->fMin;
	param_max_val_field->m_param_value = param->fMax;
	param_block_val_field->m_param_value = param->fBlock;
	param_crush_val_field->m_param_value = param->fAvaria;

	param_graf_diapazon_field->m_param_value = param->fDiapazon;
	param_graf_min_field->m_param_value = param->fGraphMIN;
	param_graf_max_field->m_param_value = param->fGraphMAX;

	DB_SendPacket(&transfer_param_packet);
}

void DB_InitConnection()
{
	if (KRS_shutdown)
		return;

	if (g_inited)
		DB_ShutDownConnection(true);

	g_inited = true;
	g_client.m_send_msg = true;
	ResetEvent(DB_connection_info.m_connection_closed_event);
	AfxBeginThread(ConnectionThreadProcedure, THREAD_PRIORITY_NORMAL, NULL);
}

void DB_ShutDownConnection(bool sync, bool send_msg)
{
	//m_wndDialogBar.m_ind_db.SetState(STATE_UNKNOWN);
	g_client.m_send_msg = send_msg;
	g_client.OnBreakedConnection();

	if (!g_inited)
		return;

	SetEvent(g_connection_closing_event);
	if (sync)
		WaitForSingleObject(DB_connection_info.m_connection_closed_event, INFINITE);
	g_inited = false;

	g_client.StopConnect(); // без этого падает !!!
}

bool DB_SendParamValues(BS_Measurement *m)
{
	if (!DB_connection_info.IsReady())
		return false;
	return g_store.Add(m);
}

bool DB_SendPacket(DB_Packet *p)
{
	LOG_V0_AddMessage(LOG_V0_TO_DB, p->GetLog());
	if (p->PutIntoBuffer(&DB_connection_info.m_out_buffer))
	{
		SetEvent(DB_connection_info.m_data_ready_for_sending_event);
		return true;
	}
	else
		return false;
}

void DB_AddActivity(CWnd* wnd, byte packet_type, DB_ActivityFunction func)
{
	if (wnd == NULL)
		return;
	DB_connection_info.m_activities[wnd].m_wnd = wnd;
	DB_connection_info.m_activities[wnd].m_activities[packet_type].m_packet_type = packet_type;
	DB_connection_info.m_activities[wnd].m_activities[packet_type].m_function = func;
}

void DB_RemoveActivity(CWnd* wnd, byte packet_type)
{
	if (packet_type != 0xFF)
		DB_connection_info.m_activities[wnd].m_activities.erase(packet_type);
	if (packet_type == 0xFF || DB_connection_info.m_activities[wnd].m_activities.empty())
		DB_connection_info.m_activities.erase(wnd);
}

DB_Packet::DB_Packet(byte type): m_packet_type(type)
{}

DB_Packet::~DB_Packet()
{}

bool DB_Packet::PutIntoBuffer(BS_RingBuffer* buff)
{
	int size = Size();
	if (buff->SpaceFor(BS_RingBuffer_WRITE) < BS_SizeOfInt(size) + size)
		return false;
	BS_PutIntIntoBuffer(size, buff);
	BS_PutIntIntoBuffer(m_packet_type, buff);
	OwnPutIntoBuffer(buff);
	return true;
}

DB_Packet* DB_Packet::GetFromBuffer(BS_RingBuffer* buff, bool read_size)
{
	char* read_ptr = buff->m_read_ptr;
	int size = 0;
	if (read_size)
	{
		if (!BS_GetIntFromBufferCarefully(size, buff))
			return NULL;
		if (size > buff->SpaceFor(BS_RingBuffer_READ))
		{
			buff->m_read_ptr = read_ptr;
			return NULL;
		}
	}
	int packet_type;
	DB_Packet *packet = NULL;
	packet_type = BS_GetIntFromBuffer(buff);
#define CASE_PT(PT) case PT: packet = new PT##Packet; break;
	CString err;
	switch (packet_type)
	{
		CASE_PT(DB_Report);
		CASE_PT(DB_TransferParamList);
		CASE_PT(DB_TransferParamDescription);
		CASE_PT(DB_TransferParamValues);
		CASE_PT(DB_SQLString);
		CASE_PT(DB_PacketsSet);
		CASE_PT(DB_TransferSpecialValues);
		CASE_PT(DB_TransferMarkerTypeList);
		CASE_PT(DB_TransferMarkerType);
		CASE_PT(DB_TransferMarker);
		default: err.Format("invalid packet type %d", packet_type); CRUSH(err); return NULL;
	}
#undef CASE_PT
	packet->OwnGetFromBuffer(buff);
	return packet;
}

int DB_Packet::Size()
{
	return BS_SizeOfInt(m_packet_type);
}

void DB_Packet::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
}

void DB_Packet::OwnGetFromBuffer(BS_RingBuffer* buff)
{
}
DB_RequestParamListPacket::DB_RequestParamListPacket():
	DB_Packet(DB_RequestParamList)
{}

CString DB_RequestParamListPacket::GetLog()
{
	return "DB_RequestParamListPacket";
}
DB_TransferParamListPacket::DB_TransferParamListPacket(byte type):
	DB_Packet(type)
{}

CString DB_TransferParamListPacket::GetLog()
{
	CString res = TXT("DB_TransferParamListPacket (%d: ") << m_indexes.size();
	list<int>::iterator current = m_indexes.begin();
	while (current != m_indexes.end())
	{
		if (current != m_indexes.begin())
			res += ", ";
		res += TXT("%d") << *current;
		current++;
	}
	return res + ")";
}

void DB_TransferParamListPacket::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	m_indexes.clear();
	int size = BS_GetIntFromBuffer(buff);
	while (size > 0)
	{
		m_indexes.push_back(BS_GetIntFromBuffer(buff));
		size--;
	}
}
DB_RequestParamDescriptionPacket::DB_RequestParamDescriptionPacket():
	DB_Packet(DB_RequestParamDescription)
{}

CString DB_RequestParamDescriptionPacket::GetLog()
{
	return TXT("DB_RequestParamDescriptionPacket (параметр %d)") << m_param_index;
}

int DB_RequestParamDescriptionPacket::Size()
{
	int res = DB_Packet::Size() + BS_SizeOfInt(m_param_index) + BS_SizeOfInt(m_fields_indexes.size());
	list<int>::iterator current = m_fields_indexes.begin();
	while (current != m_fields_indexes.end())
	{
		res += BS_SizeOfInt(*current);
		current++;
	}
	return res;
}

void DB_RequestParamDescriptionPacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutIntIntoBuffer(m_param_index, buff);
	BS_PutIntIntoBuffer(m_fields_indexes.size(), buff);
	list<int>::iterator current = m_fields_indexes.begin();
	while (current != m_fields_indexes.end())
	{
		BS_PutIntIntoBuffer(*current, buff);
		current++;
	}
}

DB_TransferParamDescriptionPacket::DB_TransferParamDescriptionPacket():
	DB_Packet(DB_TransferParamDescription)
{}

DB_TransferParamDescriptionPacket::~DB_TransferParamDescriptionPacket()
{
	m_param_description.Delete();
}

CString DB_TransferParamDescriptionPacket::GetLog()
{
	return TXT("DB_TransferParamDescriptionPacket (параметр %d)") << m_param_description.m_param_index;
}

int DB_TransferParamDescriptionPacket::Size()
{
	int res = DB_Packet::Size() + BS_SizeOfInt(m_transaction_type) +
		BS_SizeOfInt(m_param_description.m_param_index) + BS_SizeOfInt(m_param_description.m_param_description_fields.size());
	map<int, BS_ParamDescriptionField*>::iterator current = m_param_description.m_param_description_fields.begin();
	while (current != m_param_description.m_param_description_fields.end())
	{
		res += current->second->Size();
		current++;
	}
	return res;
}

void DB_TransferParamDescriptionPacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutIntIntoBuffer(m_transaction_type, buff);
	BS_PutIntIntoBuffer(m_param_description.m_param_index, buff);
	BS_PutIntIntoBuffer(m_param_description.m_param_description_fields.size(), buff);
	map<int, BS_ParamDescriptionField*>::iterator current = m_param_description.m_param_description_fields.begin();
	while (current != m_param_description.m_param_description_fields.end())
	{
		current->second->PutIntoBuffer(buff);
		current++;
	}
}

void DB_TransferParamDescriptionPacket::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	m_transaction_type = BS_GetIntFromBuffer(buff);
	m_param_description.m_param_index = BS_GetIntFromBuffer(buff);
	int size = BS_GetIntFromBuffer(buff);
	CHECK(size >= 0);
	m_param_description.m_param_description_fields.clear();
	BS_ParamDescriptionField *field = NULL;
	while (size > 0)
	{
		field = BS_ParamDescriptionField::GetFromBuffer(buff);
		CHECK(field != NULL);
		m_param_description.m_param_description_fields[field->m_field_index] = field;
		size--;
	}
}
DB_TransferParamValuesPacket::DB_TransferParamValuesPacket():
	DB_Packet(DB_TransferParamValues), m_measurements(NULL), m_measurements_size(0)
{}

DB_TransferParamValuesPacket::~DB_TransferParamValuesPacket()
{
	if (m_measurements)
		delete[] m_measurements;
}

CString DB_TransferParamValuesPacket::GetLog()
{
	if (m_transaction_type == 1)
		return TXT("DB_TransferParamValuesPacket (%d измерений, total %d, start %d)") << m_measurements_size << m_measurements_total << m_measurements_start_from;
	else
	{
		CString str = TXT("DB_TransferParamValuesPacket (%d измерений)") << m_measurements_size;
		if (m_measurements_size == 1)
		{
			str += TXT(" : %d %f)") << m_measurements->m_param_values->m_param_index << m_measurements->m_param_values->m_param_value;
		}
		return str;
	}
}

int DB_TransferParamValuesPacket::Size()
{
	int i, j;
	BS_Measurement *measurement;
	BS_OneParamValue *one_param_value;

	int res = DB_Packet::Size() + BS_SizeOfInt(m_transaction_type);

	if (m_transaction_type == 1)
	{
		res += BS_SizeOfInt(m_measurements_total);
		res += BS_SizeOfInt(m_measurements_start_from);
	}

	res += BS_SizeOfInt(m_measurements_size);
	for (i = 0, measurement = m_measurements; i < m_measurements_size; i++, measurement++)
	{
		res += BS_SizeOfTime(measurement->m_time); 
		res += BS_SizeOfParamValue(measurement->m_deepness); 
		if (m_transaction_type == 1)
			res += BS_SizeOfInt(measurement->m_index); 

		res += BS_SizeOfInt(measurement->m_param_values_size);
		for (j = 0, one_param_value = measurement->m_param_values; j < measurement->m_param_values_size; j++, one_param_value++)
		{
			res += BS_SizeOfInt(one_param_value->m_param_index);
			res += BS_SizeOfParamValue(one_param_value->m_param_value);
		}
	}
	return res;
}

void DB_TransferParamValuesPacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	int i, j;
	BS_Measurement *measurement;
	BS_OneParamValue *one_param_value;

	BS_PutIntIntoBuffer(m_transaction_type, buff);

	BS_PutIntIntoBuffer(m_measurements_size, buff);
	for (i = 0, measurement = m_measurements; i < m_measurements_size; i++, measurement++)
	{
		BS_PutTimeIntoBuffer(measurement->m_time, buff); 
		BS_PutParamValueIntoBuffer(measurement->m_deepness, buff); 
		BS_PutIntIntoBuffer(measurement->m_param_values_size, buff);
		for (j = 0, one_param_value = measurement->m_param_values; j < measurement->m_param_values_size; j++, one_param_value++)
		{
			BS_PutIntIntoBuffer(one_param_value->m_param_index, buff);
			BS_PutParamValueIntoBuffer(one_param_value->m_param_value, buff);
		}
	}
}

void DB_TransferParamValuesPacket::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	int i, j;
	BS_Measurement *measurement;
	BS_OneParamValue *one_param_value;

	m_transaction_type = BS_GetIntFromBuffer(buff);

	m_measurements_total = BS_GetIntFromBuffer(buff);
	m_measurements_start_from = BS_GetIntFromBuffer(buff);

	m_measurements_size = BS_GetIntFromBuffer(buff);
	CHECK(m_measurements_size >= 0);
	if (m_measurements)
	{
		delete[] m_measurements;
		m_measurements = NULL;
	}
	if (m_measurements_size > 0)
	{
		m_measurements = new BS_Measurement[m_measurements_size];
		for (i = 0, measurement = m_measurements; i < m_measurements_size; i++, measurement++)
		{
			measurement->m_time = BS_GetTimeFromBuffer(buff); 
			measurement->m_deepness = BS_GetParamValueFromBuffer(buff); 
			measurement->m_index = BS_GetIntFromBuffer(buff); 
			measurement->m_param_values_size = BS_GetIntFromBuffer(buff);
			CHECK(measurement->m_param_values_size >= 0);
			if (measurement->m_param_values_size > 0)
			{
				measurement->m_param_values = new BS_OneParamValue[measurement->m_param_values_size];
				for (j = 0, one_param_value = measurement->m_param_values; j < measurement->m_param_values_size; j++, one_param_value++)
				{
					one_param_value->m_param_index = BS_GetIntFromBuffer(buff);
					one_param_value->m_param_value = BS_GetParamValueFromBuffer(buff);
				}
			}				
		}
	}
}
DB_TransferSpecialValuesPacket::DB_TransferSpecialValuesPacket():
	DB_Packet(DB_TransferSpecialValues), m_measurements(NULL), m_measurements_size(0)
{}

DB_TransferSpecialValuesPacket::~DB_TransferSpecialValuesPacket()
{
	if (m_measurements)
		delete[] m_measurements;
}

CString DB_TransferSpecialValuesPacket::GetLog()
{
	return TXT("DB_TransferSpecialValuesPacket (%d измерений)") << m_measurements_size;
}

void DB_TransferSpecialValuesPacket::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	int i, j;
	BS_Measurement *measurement;
	BS_OneParamValueExt *one_param_value;

	m_transaction_type = BS_GetIntFromBuffer(buff);

	m_measurements_total = BS_GetIntFromBuffer(buff);
	m_measurements_start_from = BS_GetIntFromBuffer(buff);

	m_measurements_size = BS_GetIntFromBuffer(buff);
	CHECK(m_measurements_size >= 0);
	if (m_measurements)
	{
		delete[] m_measurements;
		m_measurements = NULL;
	}
	if (m_measurements_size > 0)
	{
		m_measurements = new BS_Measurement[m_measurements_size];
		for (i = 0, measurement = m_measurements; i < m_measurements_size; i++, measurement++)
		{
			measurement->m_time = BS_GetTimeFromBuffer(buff); 
			measurement->m_deepness = BS_GetParamValueFromBuffer(buff); 
			measurement->m_param_values_size = BS_GetIntFromBuffer(buff);
			CHECK(measurement->m_param_values_size >= 0);
			if (measurement->m_param_values_size > 0)
			{
				measurement->m_param_values = new BS_OneParamValueExt[measurement->m_param_values_size];
				one_param_value = (BS_OneParamValueExt*)measurement->m_param_values;
				for (j = 0; j < measurement->m_param_values_size; j++, one_param_value++)
				{
					one_param_value->m_param_index = BS_GetIntFromBuffer(buff);
					one_param_value->m_param_value = BS_GetParamValueFromBuffer(buff);
					one_param_value->m_flag = BS_GetIntFromBuffer(buff);
				}
			}				
		}
	}
}
DB_SQLStringPacket::DB_SQLStringPacket():
	DB_Packet(DB_SQLString), m_request_text(NULL), m_request_text_length(0)
{}

CString DB_SQLStringPacket::GetLog()
{
	return TXT("DB_SQLStringPacket (%s)") << CharToString(m_request_text, m_request_text_length);
}

int DB_SQLStringPacket::Size()
{
	return DB_Packet::Size() + BS_SizeOfInt(m_request_text_length) + m_request_text_length;
}

void DB_SQLStringPacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutIntIntoBuffer(m_transaction_type, buff);
	BS_PutStringIntoBuffer(m_request_text, m_request_text_length, buff);
}

void DB_SQLStringPacket::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	m_transaction_type = BS_GetIntFromBuffer(buff);
	m_request_text = BS_GetStringFromBuffer(m_request_text_length, buff);
}
DB_PacketsSetPacket::DB_PacketsSetPacket():
	DB_Packet(DB_PacketsSet)
{}

DB_PacketsSetPacket::~DB_PacketsSetPacket()
{
	list<DB_Packet*>::iterator current = m_packets.begin();
	while (current != m_packets.end())
	{
		delete *current;
		current++;
	}
}

CString DB_PacketsSetPacket::GetLog()
{
	CString res = TXT("DB_PacketsSetPacket (%d: ") << m_packets.size();
	list<DB_Packet*>::iterator current = m_packets.begin();
	while (current != m_packets.end())
	{
		if (current != m_packets.begin())
			res += ", ";
		res += (*current)->GetLog();
		current++;
	}
	return res + ")";
}

int DB_PacketsSetPacket::Size()
{
	int res = DB_Packet::Size() + BS_SizeOfInt(m_packets.size());
	list<DB_Packet*>::iterator current = m_packets.begin();
	while (current != m_packets.end())
	{
		res += (*current)->Size();
		current++;
	}
	return res;
}

void DB_PacketsSetPacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutIntIntoBuffer(m_packets.size(), buff);
	list<DB_Packet*>::iterator current = m_packets.begin();
	while (current != m_packets.end())
	{
		BS_PutIntIntoBuffer((*current)->m_packet_type, buff);
		(*current)->OwnPutIntoBuffer(buff);
		current++;
	}
}

void DB_PacketsSetPacket::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	DB_Packet* packet;
	int size = BS_GetIntFromBuffer(buff);
	CHECK(size > 0); 
	while(size > 0)
	{
		packet = DB_Packet::GetFromBuffer(buff, false);
		CHECK(packet != NULL);
		m_packets.push_back(packet);
		size--;
	}
}
DB_ReportPacket::DB_ReportPacket():
	DB_Packet(DB_Report), m_report_text(NULL), m_report_text_length(0)
{}

DB_ReportPacket::~DB_ReportPacket()
{
	if (m_report_text)
		delete[] m_report_text;
}

CString DB_ReportPacket::GetLog()
{
	return TXT("DB_ReportPacket (%d: \"%s\")") << m_report_code << CharToString(m_report_text, m_report_text_length);
}

void DB_ReportPacket::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	m_report_code = BS_GetIntFromBuffer(buff);
	m_report_text = BS_GetStringFromBuffer(m_report_text_length, buff);
}
DB_LoginPasswordPacket::DB_LoginPasswordPacket(const char* server_name, const char* db_name, const char* login, const char* password, byte pt):
	DB_Packet(pt)
{
	if (m_packet_type != DB_LoginPassword && m_packet_type != DB_CreateDB && m_packet_type != DB_CreateDB2)
		CRUSH("DB_LoginPasswordPacket::m_packet_type != DB_LoginPassword || DB_CreateDB || DB_CreateDB2");

	m_server_name_length = strlen(server_name);
	if (m_server_name_length > 0)
	{
		m_server_name = new char[m_server_name_length + 1];
		m_server_name[m_server_name_length] = '\0';
		memcpy(m_server_name, server_name, m_server_name_length);
	}
	else
		m_server_name = NULL;

	m_db_name_length = strlen(db_name);
	if (m_db_name_length > 0)
	{
		m_db_name = new char[m_db_name_length + 1];
		m_db_name[m_db_name_length] = '\0';
		memcpy(m_db_name, db_name, m_db_name_length);

	}
	else
		m_db_name = NULL;

	m_login_length = strlen(login);
	if (m_login_length > 0)
	{
		m_login = new char[m_login_length + 1];
		m_login[m_login_length] = '\0';
		memcpy(m_login, login, m_login_length);
	}
	else
		m_login = NULL;

	m_password_length = strlen(password);
	if (m_password_length > 0)
	{
		m_password = new char[m_password_length + 1];
		m_password[m_password_length] = '\0';
		memcpy(m_password, password, m_password_length);
	}
	else
		m_password = NULL;
}

DB_LoginPasswordPacket::~DB_LoginPasswordPacket()
{
	if (m_server_name)
		{delete[] m_server_name; m_server_name = NULL;}
	if (m_db_name)
		{delete[] m_db_name; m_db_name = NULL;}
	if (m_login)
		{delete[] m_login; m_login = NULL;}
	if (m_password)
		{delete[] m_password; m_password = NULL;}
}

CString DB_LoginPasswordPacket::GetLog()
{
	return TXT("DB_LoginPasswordPacket (server: %s, db: %s, login: %s)") << CharToString(m_server_name, m_server_name_length) << CharToString(m_db_name, m_db_name_length) << CharToString(m_login, m_login_length);
}

int DB_LoginPasswordPacket::Size()
{
	return DB_Packet::Size()
		+ BS_SizeOfInt(m_server_name_length) + m_server_name_length
		+ BS_SizeOfInt(m_db_name_length) + m_db_name_length
		+ BS_SizeOfInt(m_login_length) + m_login_length
		+ BS_SizeOfInt(m_password_length) + m_password_length;
}

void DB_LoginPasswordPacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutStringIntoBuffer(m_server_name, m_server_name_length, buff);
	BS_PutStringIntoBuffer(m_db_name, m_db_name_length, buff);
	BS_PutStringIntoBuffer(m_login, m_login_length, buff);
	BS_PutStringIntoBuffer(m_password, m_password_length, buff);
}
DB_CreateDBPacket::DB_CreateDBPacket(const char* server_name, const char* db_name, const char* login, const char* password, byte pt):
	DB_LoginPasswordPacket(server_name, db_name, login, password, pt)
{
}

CString DB_CreateDBPacket::GetLog()
{
	return TXT("DB_CreateDBPacket (server: %s, db: %s, login: %s)") << CharToString(m_server_name, m_server_name_length) << CharToString(m_db_name, m_db_name_length) << CharToString(m_login, m_login_length);
}
DB_CreateDB2Packet::DB_CreateDB2Packet(const char* server_name, const char* db_name, int db_version, const char* login, const char* password):
	DB_CreateDBPacket(server_name, db_name, login, password, DB_CreateDB2), m_db_version(db_version)
{
}

CString DB_CreateDB2Packet::GetLog()
{
	return TXT("DB_CreateDB2Packet (server: %s, db: %s, ver: %d, login: %s)") << CharToString(m_server_name, m_server_name_length) << CharToString(m_db_name, m_db_name_length) << m_db_version << CharToString(m_login, m_login_length);
}

int DB_CreateDB2Packet::Size()
{
	return DB_CreateDBPacket::Size() + BS_SizeOfInt(m_db_version);
}

void DB_CreateDB2Packet::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	DB_CreateDBPacket::OwnPutIntoBuffer(buff);
	BS_PutIntIntoBuffer(m_db_version, buff);
}
DB_RequestParamValuesPacket::DB_RequestParamValuesPacket():
	DB_Packet(DB_RequestParamValues), m_type_of_request(DB_ValuesRequestType_OnlyValues), m_from_time(0), m_to_time(0)
{
}

CString DB_RequestParamValuesPacket::GetLog()
{
	CString res = TXT("DB_RequestParamValuesPacket (тип %d, время с: %lf, время по: %lf, параметры [%d: ") << m_type_of_request << m_from_time << m_to_time << m_params_indexes.size();
	list<int>::iterator current = m_params_indexes.begin();
	while (current != m_params_indexes.end())
	{
		if (current != m_params_indexes.begin())
			res += ", ";
		res += TXT("%d")<<*current;
		current++;
	}
	return res + "])";
}

int DB_RequestParamValuesPacket::Size()
{
	int res = DB_Packet::Size()	+ BS_SizeOfInt(m_type_of_request) + BS_SizeOfTime(m_from_time) + BS_SizeOfTime(m_to_time) + BS_SizeOfInt(m_params_indexes.size());
	list<int>::iterator current = m_params_indexes.begin();
	while (current != m_params_indexes.end())
	{
		res += BS_SizeOfInt(*current);
		current++;
	}
	return res;
}

void DB_RequestParamValuesPacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutIntIntoBuffer(m_type_of_request, buff);
	BS_PutTimeIntoBuffer(m_from_time, buff);
	BS_PutTimeIntoBuffer(m_to_time, buff);
	BS_PutIntIntoBuffer(m_params_indexes.size(), buff);
	list<int>::iterator current = m_params_indexes.begin();
	while (current != m_params_indexes.end())
	{
		BS_PutIntIntoBuffer(*current, buff);
		current++;
	}
}
DB_RequestMarkerTypeListPacket::DB_RequestMarkerTypeListPacket():
	DB_Packet(DB_RequestMarkerTypeList)
{
}

CString DB_RequestMarkerTypeListPacket::GetLog()
{
	return TXT("DB_RequestMarkerTypeListPacket");
}
DB_TransferMarkerTypeListPacket::DB_TransferMarkerTypeListPacket():
	DB_TransferParamListPacket(DB_TransferMarkerTypeList)
{
}

CString DB_TransferMarkerTypeListPacket::GetLog()
{
	return TXT("DB_TransferMarkerTypeListPacket");
}
DB_RequestMarkerTypePacket::DB_RequestMarkerTypePacket(int index):
	DB_Packet(DB_RequestMarkerType), m_marker_type_index(index)
{
}

CString DB_RequestMarkerTypePacket::GetLog()
{
	return TXT("DB_RequestMarkerTypePacket");
}

int DB_RequestMarkerTypePacket::Size()
{
	return DB_Packet::Size() + BS_SizeOfInt(m_marker_type_index);
}

void DB_RequestMarkerTypePacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutIntIntoBuffer(m_marker_type_index, buff);
}
DB_TransferMarkerTypePacket::DB_TransferMarkerTypePacket():
	DB_Packet(DB_TransferMarkerType)
{
}

CString DB_TransferMarkerTypePacket::GetLog()
{
	return TXT("DB_TransferMarkerTypePacket");
}

int DB_TransferMarkerTypePacket::Size()
{
	int len = m_marker_type.m_name.GetLength();
	return DB_Packet::Size() + BS_SizeOfInt(m_marker_type.m_index) + BS_SizeOfInt(len) + len + BS_SizeOfInt(0);
}

void DB_TransferMarkerTypePacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutIntIntoBuffer(m_marker_type.m_index, buff);
	BS_PutStringIntoBuffer(m_marker_type.m_name.GetBuffer(1), m_marker_type.m_name.GetLength(), buff);
	BS_PutStringIntoBuffer(NULL, 0, buff);
}

void DB_TransferMarkerTypePacket::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	m_marker_type.m_index = BS_GetIntFromBuffer(buff);
	int tmp_len;
	char* tmp = BS_GetStringFromBuffer(tmp_len, buff);
	if (tmp)
	{
		m_marker_type.m_name = tmp;
		delete[] tmp;
	}
	else
		m_marker_type.m_name = "-- КБД дал пустое имя типа маркера! --";

	tmp = BS_GetStringFromBuffer(tmp_len, buff);
	if (tmp)
		delete[] tmp;
}
DB_RequestMarkerPacket::DB_RequestMarkerPacket(BS_Time from, BS_Time to):
	DB_Packet(DB_RequestMarker), m_from(from), m_to(to)
{
}

CString DB_RequestMarkerPacket::GetLog()
{
	return TXT("DB_RequestMarkerPacket");
}

int DB_RequestMarkerPacket::Size()
{
	return DB_Packet::Size() + BS_SizeOfTime(m_from) + BS_SizeOfTime(m_to);
}

void DB_RequestMarkerPacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutTimeIntoBuffer(m_from, buff);
	BS_PutTimeIntoBuffer(m_to, buff);
}
DB_TransferMarkerPacket::DB_TransferMarkerPacket():
	DB_Packet(DB_TransferMarker)
{
}

CString DB_TransferMarkerPacket::GetLog()
{
	return TXT("DB_TransferMarkerPacket");
}

int DB_TransferMarkerPacket::Size()
{
	int len_comment = strlen(m_marker.m_comment),
		len_reserve = strlen(m_marker.m_reserve);
	return	DB_Packet::Size() + BS_SizeOfInt(m_marker.m_index_of_type) + 
			BS_SizeOfTime(m_marker.m_time) + BS_SizeOfParamValue(m_marker.m_deepness) + 
			BS_SizeOfInt(len_comment) + len_comment+
			BS_SizeOfInt(len_reserve) + len_reserve;
}

void DB_TransferMarkerPacket::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutIntIntoBuffer(m_marker.m_index_of_type, buff);
	BS_PutTimeIntoBuffer(m_marker.m_time, buff);
	BS_PutParamValueIntoBuffer(m_marker.m_deepness, buff);
	BS_PutStringIntoBuffer(m_marker.m_comment, strlen(m_marker.m_comment), buff);
	BS_PutStringIntoBuffer(m_marker.m_reserve, strlen(m_marker.m_reserve), buff);
}

void DB_TransferMarkerPacket::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	char* tmp;
	int tmp_size;
	m_marker.m_index_of_type = BS_GetIntFromBuffer(buff);
	m_marker.m_time = BS_GetTimeFromBuffer(buff);
	m_marker.m_deepness = BS_GetParamValueFromBuffer(buff);

	tmp = BS_GetStringFromBuffer(tmp_size, buff);
	CHECK(tmp_size >=0 && tmp_size < 128);
	CHECK(tmp != NULL);
	memcpy(m_marker.m_comment, tmp, tmp_size);
	delete[] tmp;

	tmp = BS_GetStringFromBuffer(tmp_size, buff);
	CHECK(tmp_size >=0 && tmp_size < 128);
	CHECK(tmp != NULL);
	memcpy(m_marker.m_reserve, tmp, tmp_size);
	delete[] tmp;
}
DB_ConnectionInfo::DB_ConnectionInfo():
	BS_ConnectionInfo(1024*300*5, 100*200*1024),
	m_param_list_ready(false), m_marker_type_list_ready(false),
	m_client_version(0), m_db_version(-1)
{
	m_server_ip = "192.168.252.14";
	m_server_port = 50;

	m_db_server = "evg";
	m_db_name = "AlexPrq1";
	m_login = "AlexPrq1_oreol";
	m_password = "oreol";
}

void DB_ConnectionInfo::SignalError(int error_code, CString reason)
{
	BS_ConnectionInfo::SignalError(error_code, reason);
	LOG_V0_AddMessage(LOG_V0_MESSAGE, "Ошибка при работе с КБД: " + reason);
}

bool DB_ConnectionInfo::IsReady()
{
	return g_client.IsConnected() && m_param_list_ready;
}

bool DB_ConnectionInfo::IsRealyConnected()
{
	return g_client.IsConnected();
}

bool DB_ConnectionInfo::IsParamListReady()
{
	return m_param_list_ready;
}

void DB_ConnectionInfo::RequestParamList()
{
	m_param_list_ready = false;

	map<int, BS_ParamDescription>::iterator current = DB_params_from_db.begin();
	while (current != DB_params_from_db.end())
	{
		current->second.Delete();
		current++;
	}
	DB_params_from_db.clear();

	DB_RequestParamListPacket request_packet;
	DB_SendPacket(&request_packet); 
}

bool DB_ConnectionInfo::IsMarkerTypeListReady()
{
	return m_param_list_ready;
}

void DB_ConnectionInfo::RequestMarkerTypeList()
{
	return;

	m_marker_type_list_ready = false;
	DB_marker_types.clear();
	DB_RequestMarkerTypeListPacket request_packet;
	DB_SendPacket(&request_packet); 
}

void DB_ConnectionInfo::ResetListsInfo()
{
	m_param_list_ready = false;
	m_marker_type_list_ready = false;
}

void DB_ConnectionInfo::AnalyzePacketDB(DB_Packet *packet)
{
	if (packet->m_packet_type == DB_Report) 
	{
		DB_ReportPacket *report_packet = (DB_ReportPacket*)packet;
		if (report_packet->m_report_code == 101) 
		{
			int v1;
			if (sscanf(report_packet->m_report_text, "Client.%d", &v1) == 1)
				m_client_version = v1;
		}
		else
		if (report_packet->m_report_code == 102) 
		{
			int v1;
			if (sscanf(report_packet->m_report_text, "DBase.%d", &v1) == 1)
				m_db_version = v1;
		}
		else
		if (report_packet->m_report_code == 103) 
		{
			double d, t1, t2;
			char *ch = report_packet->m_report_text;
			int len = strlen(report_packet->m_report_text);
			while (len != 0)
			{
				if (*ch == ',')
					*ch = '.';
				ch++; len--;	
			}
			if (sscanf(report_packet->m_report_text, "db_depth=%lf db_time=%lf cl_time=%lf", &d, &t1, &t2) == 3)
			{
				gGlubina = d;
				if (KRS_project_flag & KRS_PRJ_FLAG_BURENIE)
					pGlubina->fCur = pGlubina->fCurPrevious = pGlubina->fCurAvg = gGlubina;
			}
		}
		else
		if (report_packet->m_report_code == 1) 
		{
			DB_current_status_str = "Связь с БД установлена";
			DB_connection_info.RequestParamList();
			DB_connection_info.RequestMarkerTypeList();
		}
		else
		if (report_packet->m_report_code == 5) 
		{
			DB_current_status_str = "БД успешно создана";
			DB_LoginPasswordPacket lp_packet(DB_connection_info.m_db_server, DB_connection_info.m_db_name, DB_connection_info.m_login, DB_connection_info.m_password);
			if ((KRS_project_flag & KRS_PRJ_FLAG_STAGES) && !PRJ_IsCreatedDB())
			{
				PRJ_SetCreatedDB(true); 
				DB_SendPacket(&lp_packet);
			}
			else
			if ((KRS_project_flag & KRS_PRJ_FLAG_STAGES) == 0)
				DB_SendPacket(&lp_packet);
			MessageBox(AfxGetMainWnd()->m_hWnd, DB_current_status_str, "БД", MB_OK);
		}
		else
		if (report_packet->m_report_code < 0)
		{
			if (DB_current_status_str != "Создание новой БД" && !PRJ_IsCreatedDB())
			{
				DB_SendPacketToCreateDB(DB_connection_info.m_db_server, DB_connection_info.m_db_name, DB_connection_info.m_login, DB_connection_info.m_password);
			}
			else
			{
				DB_current_status_str = "Соединение с БД невозможно";
				if (report_packet->m_report_text)
				{
					char sss[2048];
					memcpy(sss, report_packet->m_report_text, report_packet->m_report_text_length);
					sss[report_packet->m_report_text_length] = '\0';
					DB_current_status_str += CString("\n") + sss;
				}
				m_wndDialogBar.m_ind_db.SetState(STATE_ERROR); 
			}
		}
	}
	else
	if (packet->m_packet_type == DB_TransferParamList) 
	{
		DB_TransferParamListPacket *param_list_packet = (DB_TransferParamListPacket *)packet;
		if (param_list_packet->m_indexes.size() == 0)
		{
			DB_connection_info.m_param_list_ready = true;
			pFrm->PostMessage(WM_PARAM_LIST_READY, 0, 0);
		}
		else
		{
			DB_PacketsSetPacket packet_set_packet; 
			DB_RequestParamDescriptionPacket *request_param_description_packet;
			list<int>::iterator current = param_list_packet->m_indexes.begin();
			while (current != param_list_packet->m_indexes.end())
			{
				request_param_description_packet = new DB_RequestParamDescriptionPacket;
				request_param_description_packet->m_param_index = *current;
				packet_set_packet.m_packets.push_back(request_param_description_packet);
				current++;
			}
			DB_SendPacket(&packet_set_packet); 
		}
	}
	else
	if (packet->m_packet_type == DB_TransferMarkerTypeList) 
	{
		DB_TransferMarkerTypeListPacket *marker_type_list_packet = (DB_TransferMarkerTypeListPacket *)packet;
		if (marker_type_list_packet->m_indexes.size() == 0)
		{
			DB_connection_info.m_marker_type_list_ready = true;
		}
		else
		{
			DB_PacketsSetPacket packet_set_packet; 
			list<int>::iterator current = marker_type_list_packet->m_indexes.begin();
			while (current != marker_type_list_packet->m_indexes.end())
			{
				packet_set_packet.m_packets.push_back(new DB_RequestMarkerTypePacket(*current));
				current++;
			}
			DB_SendPacket(&packet_set_packet); 
		}
	}
	else
	if (packet->m_packet_type == DB_TransferParamDescription) 
	{
		DB_TransferParamDescriptionPacket *param_description_packet = (DB_TransferParamDescriptionPacket *)packet;
		DB_params_from_db[param_description_packet->m_param_description.m_param_index] = param_description_packet->m_param_description;
		param_description_packet->m_param_description.m_param_description_fields.clear();
	}
	else
	if (packet->m_packet_type == DB_TransferMarkerType) 
	{
		DB_TransferMarkerTypePacket *description_packet = (DB_TransferMarkerTypePacket*)packet;
		DB_marker_types[description_packet->m_marker_type.m_index] = description_packet->m_marker_type;
	}
	else
	if (packet->m_packet_type == DB_PacketsSet) 
	{
		bool transfer_params_info = true, transfer_marker_types_info = true;
		DB_PacketsSetPacket* set_packet = (DB_PacketsSetPacket*)packet;
		list<DB_Packet*>::iterator current = set_packet->m_packets.begin();
		while (current != set_packet->m_packets.end())
		{
			if (transfer_params_info &&
				(*current)->m_packet_type != DB_TransferParamDescription &&
				(*current)->m_packet_type != DB_Report)
				transfer_params_info = false;
			if (transfer_params_info &&
				(*current)->m_packet_type != DB_TransferParamDescription &&
				(*current)->m_packet_type != DB_Report)
				transfer_marker_types_info = false;
			AnalyzePacketDB(*current);
			current++;
		}
		if (transfer_params_info)
		{
			DB_connection_info.m_param_list_ready = true;
			MessageBeep(MB_OK);
			pFrm->PostMessage(WM_PARAM_LIST_READY, 0, 0);
		}
		if (transfer_marker_types_info)
		{
			DB_connection_info.m_marker_type_list_ready = true;
		}
	}
}

void DB_ConnectionInfo::OnDataReady()
{
	map<CWnd*, DB_Activities>::iterator current_wnd_activity;
	map<byte, DB_Activity>::iterator desired;
	DB_Packet *packet;
	while (true)
	{
		packet = DB_Packet::GetFromBuffer(&m_in_buffer);
		if (packet == NULL)
			break;
		LOG_V0_AddMessage(LOG_V0_FROM_DB, packet->GetLog());
		AnalyzePacketDB(packet);
		current_wnd_activity = m_activities.begin();
		while (current_wnd_activity != m_activities.end()) 
		{
			desired = current_wnd_activity->second.m_activities.find(packet->m_packet_type); 
			if (desired != current_wnd_activity->second.m_activities.end()) 
				desired->second.m_function(current_wnd_activity->first, packet); 
			current_wnd_activity++;
		}
		delete packet;
	}
}

////////////////////////////////////////////////////////////////////////////////
// end