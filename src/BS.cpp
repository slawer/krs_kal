// [B]ase [S]tructures module -- uic 16.05.2006

#include <stdafx.h>
#include <check.h>
#include <math.h>
#include "BS.h"
#include "LOG_V0.h"
#include "SERV.h"
#include "../KRS.h"
#include "../MainFrm.h"

////////////////////////////////////////////////////////////////////////////////
// BS_ConnectionInfo
////////////////////////////////////////////////////////////////////////////////
BS_ConnectionInfo::BS_ConnectionInfo(int in_buff_size, int out_buff_size):
	m_wnd(NULL), m_data_received_msg(0), m_error_occur_msg(0),
	m_in_buffer(in_buff_size), m_out_buffer(out_buff_size)
{
	m_connection_closed_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_data_ready_for_sending_event = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void BS_ConnectionInfo::SignalError(int error_code, CString reason)
{
	m_last_error = reason;
	if (m_wnd != NULL && m_error_occur_msg != 0)
		PostMessage(m_wnd, m_error_occur_msg, error_code, 0);
}

CString BS_ConnectionInfo::GetLastError()
{
	return m_last_error;
}

////////////////////////////////////////////////////////////////////////////////
// BS_RingBuffer
////////////////////////////////////////////////////////////////////////////////
BS_RingBuffer::BS_RingBuffer(int size):
	m_size(size)
{
	m_storage = new char[m_size];
#ifdef _DEBUG
	memset(m_storage, 0, m_size);
#endif
	m_read_ptr = m_write_ptr = m_storage;
}

BS_RingBuffer::~BS_RingBuffer()
{
	if (m_storage)
		delete[] m_storage;
}

int BS_RingBuffer::SpaceFor(bool for_write)
{
	char* from_pos = (for_write)?m_write_ptr:m_read_ptr;
	char* to_pos = (for_write)?m_read_ptr:m_write_ptr;
	if (to_pos < from_pos) return m_size - (from_pos - to_pos);
	if (to_pos > from_pos) return to_pos - from_pos;
	return for_write?m_size:0;
}

byte BS_RingBuffer::GetByte() 
{
	byte b = *m_read_ptr;
	m_read_ptr++;
	if (m_read_ptr == m_storage + m_size)
		m_read_ptr = m_storage;
	return b;
}

void BS_RingBuffer::PutByte(byte b) 
{
	*m_write_ptr = b;
	m_write_ptr++;
	if (m_write_ptr == m_storage + m_size)
		m_write_ptr = m_storage;
}

void BS_RingBuffer::FillTo(void* buf, int size) 
{
	char* ch_buff = (char*)buf;
	int diff = (m_read_ptr + size) - (m_storage + m_size);
	if (diff > 0)
	{
		memcpy(ch_buff, m_read_ptr, size - diff);
#ifdef _DEBUG
		memset(m_read_ptr, 0, size - diff);
#endif
		memcpy(ch_buff + (size - diff), m_storage, diff);
#ifdef _DEBUG
		memset(m_storage, 0, diff);
#endif
		m_read_ptr = m_storage + diff;
	}
	else
	{
		memcpy(ch_buff, m_read_ptr, size);
#ifdef _DEBUG
		memset(m_read_ptr, 0, size);
#endif
		if (diff == 0)
			m_read_ptr = m_storage;
		else
			m_read_ptr += size;
	}
}

void BS_RingBuffer::FillFrom(void* buf, int size) 
{
	if (size == 0)
		return;
		
	char* ch_buff = (char*)buf;
	int diff = (m_write_ptr + size) - (m_storage + m_size);
	if (diff > 0)
	{
		memcpy(m_write_ptr, ch_buff, size - diff);
		memcpy(m_storage, ch_buff + (size - diff), diff);
		m_write_ptr = m_storage + diff;
	}
	else
	{
		memcpy(m_write_ptr, ch_buff, size);
		if (diff == 0)
			m_write_ptr = m_storage;
		else
			m_write_ptr += size;
	}
}

bool BS_RingBuffer::FillFrom(NET_Buffer* buf)
{
	int ready_data_size;
	MutexWrap in_read_access(buf->m_r_mutex);
	{
		MutexWrap in_write_access(buf->m_w_mutex);
		ready_data_size = buf->SpaceFor(RingBuffer_READ);
	}
	if (ready_data_size > m_size - 1 - SpaceFor(BS_RingBuffer_READ))
		return false;
	int diff = (m_write_ptr + ready_data_size) - (m_storage + m_size);
	if (diff > 0)
	{
		buf->Read(m_write_ptr, ready_data_size - diff);
		buf->Read(m_storage, diff);
		m_write_ptr = m_storage + diff;
	}
	else
	{
		buf->Read(m_write_ptr, ready_data_size);
		if (diff == 0)
			m_write_ptr = m_storage;
		else
			m_write_ptr += ready_data_size;
	}
	return true;
}

void BS_RingBuffer::Clear()
{
	m_read_ptr = m_write_ptr = m_storage;
}

void BS_ParamDescription::Delete()
{
	map<int, BS_ParamDescriptionField*>::iterator current = m_param_description_fields.begin();
	while (current != m_param_description_fields.end())
	{
		delete current->second;
		current++;
	}
	m_param_description_fields.clear();
}

bool BS_ParamDescription::EqualToBy(BS_ParamDescription& other, int field_index)
{
	map<int, BS_ParamDescriptionField*>::iterator own = m_param_description_fields.find(field_index);
	if (own == m_param_description_fields.end())
		return false;
	map<int, BS_ParamDescriptionField*>::iterator nwo = other.m_param_description_fields.find(field_index);
	if (nwo == other.m_param_description_fields.end())
		return false;
	return own->second->IsEqualTo(nwo->second);
}

void BS_ParamDescriptionField::PutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutIntIntoBuffer(m_field_index, buff);
	OwnPutIntoBuffer(buff);
}

BS_ParamDescriptionField* BS_ParamDescriptionField::GetFromBuffer(BS_RingBuffer* buff)
{
	double ddd;
	BS_ParamDescriptionField* field = NULL;
	int index = BS_GetIntFromBuffer(buff), type = BS_ParamDescriptionField::TypeOfField(index);
	switch (type)
	{
		case BS_INTEGER: field = new BS_IntParamDescriptionField(); break;
		case BS_STRING: field = new BS_StringParamDescriptionField(); break;
		case BS_TIME: ddd = BS_GetTimeFromBuffer(buff); field = new BS_StringParamDescriptionField();
			((BS_StringParamDescriptionField*)field)->m_text_length = 10;
			((BS_StringParamDescriptionField*)field)->m_text_string = new char[10];
			memcpy(((BS_StringParamDescriptionField*)field)->m_text_string, "datetime", 9); // TODO
			return field;
		case BS_PARAM_VALUE: field = new BS_ParamValueParamDescriptionField(); break;
		default: CRUSH("unknown field index");
	}
	field->m_field_index = index;
	field->OwnGetFromBuffer(buff);
	return field;
}

int BS_ParamDescriptionField::TypeOfField(int index)
{
	switch (index)
	{
		case 1: case 3: case 4: case 24: case 28: case 32: case 33: return BS_INTEGER;
		case 5: case 6: case 22: case 23: case 29: return BS_STRING;
		case 2: return BS_TIME;
		case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 25: case 26: case 27: case 30: case 31: return BS_PARAM_VALUE;
		default: CRUSH("unknown field index"); return -1;
	}
}

int BS_ParamDescriptionField::Size()
{
	return BS_SizeOfInt(m_field_index);
}

bool BS_ParamDescriptionField::IsEqualTo(BS_ParamDescriptionField*other)
{
	return false;
}

void BS_ParamDescriptionField::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
}

void BS_ParamDescriptionField::OwnGetFromBuffer(BS_RingBuffer* buff)
{
}

int BS_IntParamDescriptionField::Size()
{
	return BS_ParamDescriptionField::Size() + BS_SizeOfInt(m_int);
}

bool BS_IntParamDescriptionField::IsEqualTo(BS_ParamDescriptionField*other)
{
	if (BS_ParamDescriptionField::TypeOfField(other->m_field_index) != BS_INTEGER)
		return false;
	return m_int == ((BS_IntParamDescriptionField *)other)->m_int;
}

void BS_IntParamDescriptionField::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutIntIntoBuffer(m_int, buff);
}

void BS_IntParamDescriptionField::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	m_int = BS_GetIntFromBuffer(buff);
}

int BS_ParamValueParamDescriptionField::Size()
{
	return BS_ParamDescriptionField::Size() + BS_SizeOfParamValue(m_param_value);
}

bool BS_ParamValueParamDescriptionField::IsEqualTo(BS_ParamDescriptionField*other)
{
	if (BS_ParamDescriptionField::TypeOfField(other->m_field_index) != BS_PARAM_VALUE)
		return false;
	return fabs(m_param_value - ((BS_ParamValueParamDescriptionField *)other)->m_param_value) < 1e-7;
}

void BS_ParamValueParamDescriptionField::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutParamValueIntoBuffer(m_param_value, buff);
}

void BS_ParamValueParamDescriptionField::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	m_param_value = BS_GetParamValueFromBuffer(buff);
}

BS_StringParamDescriptionField::BS_StringParamDescriptionField():
	m_text_string(NULL), m_text_length(0)
{
}
	
BS_StringParamDescriptionField::~BS_StringParamDescriptionField()
{
	if (m_text_string)
		delete[] m_text_string;
}

int BS_StringParamDescriptionField::Size()
{
	return BS_ParamDescriptionField::Size() + BS_SizeOfInt(m_text_length) + m_text_length;
}

bool BS_StringParamDescriptionField::IsEqualTo(BS_ParamDescriptionField*other)
{
	if (BS_ParamDescriptionField::TypeOfField(other->m_field_index) != BS_STRING)
		return false;
	if (m_text_length != ((BS_StringParamDescriptionField *)other)->m_text_length)
		return false;
	return memcmp(m_text_string, ((BS_StringParamDescriptionField *)other)->m_text_string, m_text_length) == 0;
}

void BS_StringParamDescriptionField::OwnPutIntoBuffer(BS_RingBuffer* buff)
{
	BS_PutStringIntoBuffer(m_text_string, m_text_length, buff);
}

void BS_StringParamDescriptionField::OwnGetFromBuffer(BS_RingBuffer* buff)
{
	m_text_string = BS_GetStringFromBuffer(m_text_length, buff);
}

BS_TextPacket::BS_TextPacket(CString str)
{
	m_packet_str_length = min(strlen(str), BS_MAX_TEXT_PACKET_LENGTH);
	if (m_packet_str_length > 0)
	{
		memcpy(m_packet_str, str, m_packet_str_length);
	}
}

bool BS_TextPacket::GetFromBuffer(BS_RingBuffer* buff)
{
	int for_read = buff->SpaceFor(BS_RingBuffer_READ);
	if (for_read < 2)
		return false;
	char* curr = buff->m_read_ptr;

	int count = 1;
	while (*curr != '@')
	{
		if (count >= for_read)
		{
			char text[10*1024 + 1];
			int min_len = min(10*1024, count);
			memcpy(text, buff->m_read_ptr, min_len);
			text[min_len] = '\0';
			LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, CString("SERV плохие данные: ") + text);
			buff->m_read_ptr = curr;
			return false;
		}
		count++;
		curr++;
		if (curr - buff->m_storage == buff->m_size)
			curr = buff->m_storage;
	}
	curr++;
	count++;
	if (curr - buff->m_storage == buff->m_size)
		curr = buff->m_storage;	
	char* start_ptr = curr;

	int control_count = 1;
	while (*curr != '$')
	{
		if (count >= for_read)
		{
			return false;
		}
		if (control_count > BS_MAX_TEXT_PACKET_LENGTH)
		{
			LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, CString("SERV плохие данные: count > BS_MAX_TEXT_PACKET_LENGTH"));
			buff->m_read_ptr = curr;
			return false;
		}
		count++;
		control_count++;
		curr++;
		if (curr - buff->m_storage == buff->m_size)
			curr = buff->m_storage;
	}

	buff->m_read_ptr = start_ptr;

	buff->FillTo(m_packet_str, control_count-1);
	m_packet_str[control_count-1] = 0;
	m_packet_str_length = control_count-1;

	buff->m_read_ptr++;
	if (buff->m_read_ptr - buff->m_storage == buff->m_size)
		buff->m_read_ptr = buff->m_storage;

	return true;
}

void BS_PutIntIntoBuffer(int i, BS_RingBuffer *buff)
{
	while (i < -64 || i > 63)
	{
		buff->PutByte(i & 0x7F); 
		i >>= 7;
	}
	buff->PutByte(i + 192);
}

int BS_GetIntFromBuffer(BS_RingBuffer *buff)
{
	int shift = 0, res = 0;
	byte b = buff->GetByte();
	while (b < 0x80) 
	{
		res += b << shift;
		shift += 7;
		CHECK(shift <= 7*4);
		b = buff->GetByte();
	}
	return res + ((b - 192) << shift);
}

bool BS_GetIntFromBufferCarefully(int &i, BS_RingBuffer *buff)
{
	int shift = 0, res = 0;
	if (buff->SpaceFor(RingBuffer_READ) < 1)
		return false;
	byte b = buff->GetByte();
	while (b < 0x80) 
	{
		res += b << shift;
		shift += 7;
		CHECK(shift <= 7*4);
		if (buff->SpaceFor(RingBuffer_READ) < 1)
			return false;
		b = buff->GetByte();
	}
	res += ((b - 192) << shift);
	i = res;
	return true;
}

void BS_PutParamValueIntoBuffer(BS_ParamValueType pv, BS_RingBuffer *buff)
{
	buff->FillFrom(&pv, 4);
}

BS_ParamValueType BS_GetParamValueFromBuffer(BS_RingBuffer *buff)
{
	BS_ParamValueType pv;
	buff->FillTo(&pv, 4);
	return pv;
}

void BS_PutStringIntoBuffer(char* str, int str_length, BS_RingBuffer *buff)
{
	BS_PutIntIntoBuffer(str_length, buff);
	buff->FillFrom(str, str_length);
}

char* BS_GetStringFromBuffer(int& str_length, BS_RingBuffer *buff)
{
	str_length = BS_GetIntFromBuffer(buff);
	CHECK(str_length >= 0);
	if (str_length > 0)
	{
		char* str = new char[str_length + 1];
		buff->FillTo(str, str_length);
		str[str_length] = 0;
		return str;
	}
	else
		return NULL;
}

void BS_PutTimeIntoBuffer(BS_Time t, BS_RingBuffer *buff)
{
	buff->FillFrom(&t, 8);
}

BS_Time BS_GetTimeFromBuffer(BS_RingBuffer *buff)
{
	BS_Time t;
	buff->FillTo(&t, 8);
	return t;
}

int BS_SizeOfInt(int i)
{
	int res = 0;
	while (i < -64 || i > 63)
	{
		res++;
		i >>= 7;
	}
	return res+1;
}

int BS_SizeOfParamValue(BS_ParamValueType pv)
{
	return 4;
}

int BS_SizeOfTime(BS_Time t)
{
	return 8;
}

CString BS_GetStandardName(int param_num)
{
	switch (param_num)
	{
		case 1: return "i-забой";
		case 2: return "Высота т-блока";
		case 3: return "Глубина";
		case 4: return "L Инструмент";
		case 5: return "№ кбк";
		case 6: return "Заход квадрата";
		case 7: return "Высота элеватора над роторным столом";
		case 8: return "Над забоем положение НКБК";
		case 9: return "Вес на крюке";
		case 10: return "Нагрузка на долото";
		case 11: return "Момент на ключе";
		case 12: return "Крутящий момент роторного стола";
		case 13: return "Давление в нагнетающей линии буровых насосов";
		case 14: return "Мех. скорость";
		case 15: return "Скорость СПО";
		case 16: return "Время циркуляции";
		case 17: return "Время бурения";
		case 18: return "Время";
		case 19: return "Шурф";
	}
	return "-- Ошибка: нет такого параметра --";
}

CString GetXChannelName(CParam* param, int num, int conv_num)
{
	if (param == NULL)
		return "- канал не указан -";
	if (param->m_nTypePar == PARAM_TYPE_CALCULATED)
		return "Внутренние вычисления";
	if (param->m_nTypePar == PARAM_TYPE_FORMULA)
	{
		if (param->m_formula == NULL)
			return "- формула не указана -";
		return param->m_formula->GetText(FORMULA_MODE_RESULT);
	}
	CHECK(param->m_nTypePar == PARAM_TYPE_CHANNEL);
	if (num == -1)
		return "- канал не указан -";
	if (conv_num == 0)
	{
		CMainFrame *frame = (CMainFrame *)AfxGetMainWnd();
		SERV_Channel *channel = m_convertor_cfg.GetChannel(num);
		if (channel == NULL)
		{
			CString str;
			str.Format("- несуществующий канал (%d) -", num);
			return str;
		}
		return channel->GetInfo();
	}
	// else
	if (SERV_current_device_cfg == NULL)
		return "- внутренняя ошибка (SERV_current_device_cfg == NULL) -";
	SERV_Channel *channel = SERV_current_device_cfg->GetChannel(num);
	if (channel == NULL)
	{
		CString str;
		str.Format("- несуществующий канал %d устройства %d -", num & 0xFF, num>>8);
		return str;
	}
	return channel->GetExtendedInfo();
}

CString GetXAttrName(CParam* param, int num, int conv_num)
{
	/*
	if (SERV_current_device_cfg == NULL)
		return "- внутренняя ошибка (SERV_current_device_cfg == NULL) -";
	SERV_Channel *channel = SERV_current_device_cfg->GetChannel(num);
	if (channel == NULL)
	{
		CString str;
		str.Format("- несуществующий канал %d устройства %d -", num & 0xFF, num>>8);
		return str;
	}
	return channel->GetExtendedInfo();
	*/
	CString str="- ошибка в аттрибуте -";
	
	if (conv_num==1)
		str="- 1 знак после запятой -";
	if (conv_num==2)
		str="- 2 знака после запятой -";
	if (conv_num==3)
		str="- 3 знака после запятой -";
	if (conv_num==4)
		str="- 4 знака после запятой -";
	if (conv_num==5)
		str="- 5 знаков после запятой -";
	

	
	// str.Format("- %d  %d %d  %d -", num, conv_num, num & 0xFF, num>>8);

	return str;
}

CString BS_GetChannelName(CParam* param)
{
	return GetXChannelName(param, (param == NULL)?0:param->m_channel_num, (param == NULL)?0:param->m_channel_conv_num);
}

CString BS_GetAttrChannelName(CParam* param)
{
//	return str.Format("- несуществующий канал %d устройства %d -", num & 0xFF, num>>8);
//	return str.Format("- несуществующий канал %d устройства %d -", num & 0xFF, num>>8);

//	return "- запятая 1";
//	return GetXAttrName(param, (param == NULL)?0:param->m_channel_num, (param == NULL)?0:param->m_channel_conv_num);

	return GetXAttrName(param, (param == NULL)?0:param->m_attr_channel_num, (param == NULL)?0:param->m_attr_channel_conv_num);


//	return GetXChannelName(param, (param == NULL)?0:param->m_attr_channel_num, (param == NULL)?0:param->m_attr_channel_conv_num);
}

CString BS_FloatWOZeros(double f, int precision)
{
	CString result, format;
	format.Format("%%.%df", precision);
	result.Format(format, f);
	int len = result.GetLength(), i = len - 1;
	if (result.Find(".") != -1)
	{
		while (result.GetAt(i) == '0') i--;
		if (i < len - 1)
		{
			if (result.GetAt(i) == '.') i--;
			return result.Left(i+1);
		}
	}
	return result;
}

CString BS_GetTimeString(double time)
{
	COleDateTimeSpan ts(time);
	double sec = ts.GetSeconds();
	double msec = (24*3600*ts.m_span - ts.GetTotalSeconds())*1000;
	if (msec < 0)
	{
		if (msec > -99)
			msec = 0;
		else
		{
			sec--;
			msec += 1000;
		}
	}
	CString result;
	result.Format("%02d:%02d:%02d.%d", ts.GetHours(), ts.GetMinutes(), int(sec), BS_ROUND(msec*0.01));
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// end