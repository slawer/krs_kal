#ifndef __DB_H_
#define __DB_H_

#include "BS.h"

#define DB_VERSION 2

#define DB_Report 0
#define DB_RequestParamList 1
#define DB_TransferParamList 2
#define DB_RequestParamDescription 3
#define DB_TransferParamDescription 4
#define DB_TransferParamValues 5
#define DB_SQLString 6
#define DB_PacketsSet 7
#define DB_RequestParamValues 8
#define DB_TransferSpecialValues 9
#define DB_LoginPassword 10
#define DB_Logout 11 // unused
#define DB_RequestMarkerTypeList 12
#define DB_TransferMarkerTypeList 13
#define DB_RequestMarkerType 14
#define DB_TransferMarkerType 15
#define DB_RequestMarker 16
#define DB_TransferMarker 17
#define DB_CreateDB 20
#define DB_CreateDB2 21

#define DB_ValuesRequestType_OnlyValues 0
#define DB_ValuesRequestType_UpperBlockEvents 1
#define DB_ValuesRequestType_LowerBlockEvents 2
#define DB_ValuesRequestType_CrushEvents 4
#define DB_ValuesRequestType_MinEvents 8
#define DB_ValuesRequestType_MaxEvents 16
#define DB_ValuesRequestType_AllEvents (DB_ValuesRequestType_UpperBlockEvents |\
										DB_ValuesRequestType_LowerBlockEvents |\
										DB_ValuesRequestType_CrushEvents |\
										DB_ValuesRequestType_MinEvents |\
										DB_ValuesRequestType_MaxEvents)

struct DB_Packet
{
	DB_Packet(byte type);
	virtual ~DB_Packet();
	byte m_packet_type;

	bool PutIntoBuffer(BS_RingBuffer* buffer);
	static DB_Packet* GetFromBuffer(BS_RingBuffer* buffer, bool read_size = true);

	virtual int Size();

	virtual CString GetLog() = 0;

	friend struct DB_PacketsSetPacket;
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff); 
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff); 
};

struct DB_RequestParamListPacket: DB_Packet
{
	DB_RequestParamListPacket();

	virtual CString GetLog();
};

struct DB_TransferParamListPacket: DB_Packet
{
	DB_TransferParamListPacket(byte type = DB_TransferParamList);

	virtual CString GetLog();

	list<int> m_indexes;

protected:
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct DB_RequestParamDescriptionPacket: DB_Packet
{
	DB_RequestParamDescriptionPacket();

	virtual CString GetLog();

	int m_param_index;
	list<int> m_fields_indexes;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
};

struct DB_TransferParamDescriptionPacket: DB_Packet
{
	DB_TransferParamDescriptionPacket();
	virtual ~DB_TransferParamDescriptionPacket();

	virtual CString GetLog();

	byte m_transaction_type;
	BS_ParamDescription m_param_description;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct DB_TransferParamValuesPacket: DB_Packet
{
	DB_TransferParamValuesPacket();
	virtual ~DB_TransferParamValuesPacket();

	virtual CString GetLog();

	byte m_transaction_type;
	int m_measurements_total; 
	int m_measurements_start_from; 
	int m_measurements_size;
	BS_Measurement *m_measurements;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct DB_TransferSpecialValuesPacket: DB_Packet
{
	DB_TransferSpecialValuesPacket();
	virtual ~DB_TransferSpecialValuesPacket();

	virtual CString GetLog();

	byte m_transaction_type;
	int m_measurements_total; 
	int m_measurements_start_from; 
	int m_measurements_size;
	BS_Measurement *m_measurements;

protected:
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct DB_SQLStringPacket: DB_Packet
{
	DB_SQLStringPacket();

	virtual CString GetLog();

	byte m_transaction_type;
	int m_request_text_length;
	char* m_request_text;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct DB_PacketsSetPacket: DB_Packet
{
	DB_PacketsSetPacket();
	~DB_PacketsSetPacket();

	virtual CString GetLog();

	list<DB_Packet*> m_packets;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct DB_ReportPacket: DB_Packet
{
	DB_ReportPacket();
	virtual ~DB_ReportPacket();

	virtual CString GetLog();

	int m_report_code;
	int m_report_text_length;
	char* m_report_text;

protected:
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct DB_LoginPasswordPacket: DB_Packet
{
	DB_LoginPasswordPacket(const char* server_name, const char* db_name, const char* login, const char* password, byte pt = DB_LoginPassword);
	virtual ~DB_LoginPasswordPacket();

	virtual CString GetLog();

	int m_server_name_length;
	char* m_server_name;
	int m_db_name_length;
	char* m_db_name;
	int m_login_length;
	char* m_login;
	int m_password_length;
	char* m_password;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
};

struct DB_CreateDBPacket: DB_LoginPasswordPacket
{
	DB_CreateDBPacket(const char* server_name, const char* db_name, const char* login, const char* password, byte pt = DB_CreateDB);
	virtual CString GetLog();
};

struct DB_CreateDB2Packet: DB_CreateDBPacket
{
	DB_CreateDB2Packet(const char* server_name, const char* db_name, int db_version, const char* login, const char* password);

	virtual CString GetLog();

	int m_db_version;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
};

struct DB_RequestParamValuesPacket: DB_Packet
{
	DB_RequestParamValuesPacket();

	virtual CString GetLog();

	int m_type_of_request;
	BS_Time m_from_time, m_to_time;
	list<int> m_params_indexes;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
};

struct DB_RequestMarkerTypeListPacket: DB_Packet
{
	DB_RequestMarkerTypeListPacket();

	virtual CString GetLog();
};

struct DB_TransferMarkerTypeListPacket: DB_TransferParamListPacket
{
	DB_TransferMarkerTypeListPacket();

	virtual CString GetLog();
};

struct DB_RequestMarkerTypePacket: DB_Packet
{
	DB_RequestMarkerTypePacket(int index);

	virtual CString GetLog();

	int m_marker_type_index;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
};

struct DB_TransferMarkerTypePacket: DB_Packet
{
	DB_TransferMarkerTypePacket();

	virtual CString GetLog();

	BS_MarkerType m_marker_type;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

struct DB_RequestMarkerPacket: DB_Packet
{
	DB_RequestMarkerPacket(BS_Time from, BS_Time to);

	virtual CString GetLog();

	BS_Time m_from, m_to;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
};

struct DB_TransferMarkerPacket: DB_Packet
{
	DB_TransferMarkerPacket();

	virtual CString GetLog();

	BS_Marker m_marker;

	virtual int Size();
protected:
	virtual void OwnPutIntoBuffer(BS_RingBuffer* buff);
	virtual void OwnGetFromBuffer(BS_RingBuffer* buff);
};

typedef void (*DB_ActivityFunction)(CWnd*, DB_Packet*);

struct DB_Activity
{
	byte m_packet_type;
	DB_ActivityFunction m_function;
};

struct DB_Activities
{
	CWnd* m_wnd;
	map<byte, DB_Activity> m_activities;
};
struct DB_ConnectionInfo: BS_ConnectionInfo
{
	DB_ConnectionInfo();

    virtual bool IsConnected() { return false; }
    virtual bool IsRealyConnected();
	bool IsReady();

	bool IsParamListReady();
	void RequestParamList();

	bool IsMarkerTypeListReady();
	void RequestMarkerTypeList();

	void ResetListsInfo();

	virtual void SignalError(int error_code, CString reason);


	void AnalyzePacketDB(DB_Packet *packet);
	void OnDataReady();

	int m_client_version, m_db_version;
	bool m_param_list_ready, m_marker_type_list_ready;
	CString m_db_server, m_db_name, m_login, m_password;
	map<CWnd*, DB_Activities> m_activities;
};
extern DB_ConnectionInfo DB_connection_info;
extern map<int, BS_ParamDescription> DB_params_from_db;
extern map<int, BS_MarkerType> DB_marker_types;
extern CString DB_current_status_str;
void DB_InitConnection();
void DB_ShutDownConnection(bool sync = false, bool send_msg = false);
bool DB_SendParamValues(BS_Measurement *m);
bool DB_SendPacket(DB_Packet *p);
void DB_AddActivity(CWnd* wnd, byte packet_type, DB_ActivityFunction func);
void DB_RemoveActivity(CWnd* wnd, byte packet_type = 0xFF);
void DB_SendParamDescriptionToDB(CParam*);
void DB_SendPacketToCreateDB(CString db_server, CString db_name, CString login, CString password);

#endif
////////////////////////////////////////////////////////////////////////////////
// end