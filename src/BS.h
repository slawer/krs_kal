// [B]ase [S]tructures module -- uic 16.05.2006

#ifndef __BS_H_
#define __BS_H_

//#pragma warning(disable:4786)
#include <map>
#include <list>

#include "../Param.h"
#include "Net.h"

#include <MATH.H>

////////////////////////////////////////////////////////////////////////////////
// +++ DEFINES +++
////////////////////////////////////////////////////////////////////////////////
// error codes
#define BS_UNKNOWN_ERROR 0
#define BS_CONNECTION_BROKEN 1
#define BS_NOT_ENOUGH_MEMORY 2
// ring buffer constants
#define BS_RingBuffer_READ false
#define BS_RingBuffer_WRITE true

#define BS_PARAM_TIME 10
#define BS_PARAM_TIME_OF_DRILLING 11
#define BS_PARAM_SYNCRO 21

#define BS_FIELD_NAME 5
#define BS_FIELD_MU 6
#define BS_FIELD_UPPER_BLOCK_VALUE 7
#define BS_FIELD_LOWER_BLOCK_VALUE 8
#define BS_FIELD_CRUSH_VALUE 9
#define BS_FIELD_MAXIMAL_VALUE 10
#define BS_FIELD_MINIMAL_VALUE 11
#define BS_FIELD_CALIBRE_VALUE_1 12
#define BS_FIELD_CALIBRE_VALUE_2 13
#define BS_FIELD_CALIBRE_VALUE_3 14
#define BS_FIELD_CALIBRE_VALUE_4 15
#define BS_FIELD_CALIBRE_VALUE_5 16
#define BS_FIELD_CALIBRE_VALUE_6 17
#define BS_FIELD_CALIBRE_VALUE_7 18
#define BS_FIELD_CALIBRE_VALUE_8 19
#define BS_FIELD_CALIBRE_VALUE_9 20
#define BS_FIELD_CALIBRE_VALUE_10 21
#define BS_FIELD_PATH_TO_CRASH_SOUND 22
#define BS_FIELD_PATH_TO_MAX_SOUND 23
#define BS_FIELD_GRAPH_SWITCH 24
#define BS_FIELD_GRAPH_DIAPAZON 25
#define BS_FIELD_GRAPH_MIN 26
#define BS_FIELD_GRAPH_MAX 27
#define BS_FIELD_CONTROL 28
//...

// types of field
#define BS_INTEGER 0
#define BS_PARAM_VALUE 1
#define BS_STRING 2
#define BS_TIME 3

#define BS_MAX_TEXT_PACKET_LENGTH 1024

#define BS_ROUND(f) ((fabs((f) - (int)(f)) > 0.5 ) ? (int(f) + 1) : int(f))

struct BS_RingBuffer
{
	BS_RingBuffer(int size);
	~BS_RingBuffer();

	int SpaceFor(bool for_write); 
	byte GetByte(); 
	void PutByte(byte b); 
	int FilledSize();
	void FillTo(void* buf, int size); 
	void FillFrom(void* buf, int size); 
	bool FillFrom(NET_Buffer* buf);
	void Clear();

	char *m_storage, *m_read_ptr, *m_write_ptr;
	int m_size;
};

struct BS_ConnectionInfo
{
	BS_ConnectionInfo(int in_buff_size = 1024*300, int out_buff_size = 20000);

	HWND m_wnd;
	DWORD m_initialization_complete_msg, m_data_received_msg, m_error_occur_msg;
	HANDLE	m_connection_closed_event,
			m_data_ready_for_sending_event;
	BS_RingBuffer m_in_buffer, m_out_buffer;

	CString m_server_ip;
	WORD m_server_port;

    virtual bool IsConnected() = 0;
	virtual void SignalError(int error_code, CString reason);
	CString GetLastError();
	CString m_last_error;
};

struct BS_TextPacket
{
	BS_TextPacket(CString str = "");
	virtual bool GetFromBuffer(BS_RingBuffer* buff);

	char m_packet_str[BS_MAX_TEXT_PACKET_LENGTH];
	int m_packet_str_length;
};

typedef double BS_Time;
typedef float BS_ParamValueType;

struct BS_ParamDescriptionField
{
	virtual ~BS_ParamDescriptionField() {}
	int m_field_index;

	void PutIntoBuffer(BS_RingBuffer* buff);
	static BS_ParamDescriptionField* GetFromBuffer(BS_RingBuffer* buff);
	static int TypeOfField(int index);

	virtual int Size();
	virtual bool IsEqualTo(BS_ParamDescriptionField*other);
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct BS_IntParamDescriptionField: BS_ParamDescriptionField
{
	int m_int;

	virtual int Size();
	virtual bool IsEqualTo(BS_ParamDescriptionField*other);
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct BS_ParamValueParamDescriptionField: BS_ParamDescriptionField
{
	BS_ParamValueType m_param_value;

	virtual int Size();
	virtual bool IsEqualTo(BS_ParamDescriptionField*other);
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct BS_StringParamDescriptionField: BS_ParamDescriptionField
{
	BS_StringParamDescriptionField();
	virtual ~BS_StringParamDescriptionField();
	int m_text_length;
	char* m_text_string;

	virtual int Size();
	virtual bool IsEqualTo(BS_ParamDescriptionField*other);
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct BS_ParamDescription
{
	bool EqualToBy(BS_ParamDescription& other, int field_index);
	void Delete();
	int m_param_index;
	map<int, BS_ParamDescriptionField*> m_param_description_fields;
};

struct BS_MarkerType
{
	BS_MarkerType():
		m_index(0), m_name("- Макер -")
	{}
	int m_index;
	CString m_name;
};

struct BS_Marker
{
	BS_Marker(int index = 0, BS_Time time = 0): m_index_of_type(index), m_time(time)
	{
		m_comment[0] = '\0';
		m_reserve[0] = '\0';
	}

	int m_index_of_type;
	BS_Time m_time;
	BS_ParamValueType m_deepness;
	char m_comment[128];
	char m_reserve[128];
};

struct BS_OneParamValue
{
	BS_OneParamValue():
		m_param_index(0), m_param_value(0)
	{
	}
	int m_param_index;
	BS_ParamValueType m_param_value;
};

struct BS_OneParamValueExt: BS_OneParamValue
{
	BS_OneParamValueExt():
		m_flag(0)
	{
	}
	int m_flag;
};

struct BS_Measurement
{
	BS_Measurement():
		m_param_values(NULL), m_param_values_size(0), m_time(0), m_index(-1)
	{
	}
	virtual ~BS_Measurement()
	{
		if (m_param_values)
			delete[] m_param_values;
	}
	BS_Time m_time;
	BS_ParamValueType m_deepness;
	int m_index;
	int m_param_values_size;
	BS_OneParamValue *m_param_values;
};

void BS_PutIntIntoBuffer(int i, BS_RingBuffer *buff);
int BS_GetIntFromBuffer(BS_RingBuffer *buff);
bool BS_GetIntFromBufferCarefully(int &i, BS_RingBuffer *buff);
void BS_PutParamValueIntoBuffer(BS_ParamValueType pv, BS_RingBuffer *buff);
BS_ParamValueType BS_GetParamValueFromBuffer(BS_RingBuffer *buff);
void BS_PutStringIntoBuffer(char* str, int str_length, BS_RingBuffer *buff);
char* BS_GetStringFromBuffer(int &str_length, BS_RingBuffer *buff);
void BS_PutTimeIntoBuffer(BS_Time t, BS_RingBuffer *buff);
BS_Time BS_GetTimeFromBuffer(BS_RingBuffer *buff);
int BS_SizeOfInt(int i);
int BS_SizeOfParamValue(BS_ParamValueType f);
int BS_SizeOfTime(BS_Time t);

CString BS_GetStandardName(int param_num);
CString BS_GetChannelName(CParam* param);
CString BS_GetAttrChannelName(CParam* param);

CString BS_GetTimeString(double time);

CString BS_FloatWOZeros(double f, int precision);

#endif