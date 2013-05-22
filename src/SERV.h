#ifndef __SERV_H_
#define __SERV_H_

#include "BS.h"
#include "TXT.h"
#include "../Param.h"

#define SERV_BO_FILE_VERSION 2
#define SERV_DEVICE_CFG_FILE_VERSION 2

#define SERV_RS_IDLE 0
#define SERV_RS_READ_USTAVKA 1
#define SERV_RS_READ_LIMIT 2
#define SERV_RS_READY 3

struct SERV_ConnectionInfo: BS_ConnectionInfo
{
	SERV_ConnectionInfo();

	virtual bool IsConnected();
	virtual void SignalError(int error_code, CString reason);

	DWORD m_calibration_table_ready_msg;
	HWND m_calibration_table_ready_wnd;

	UINT m_received_packets;
};
struct SERV_DE
{
	SERV_DE(byte length, FORMULA_Formula* formula);
	~SERV_DE();

	void Save(FILE* f);
	static SERV_DE* Load(FILE* f);

	CString GetString();
	CString GetInfo(int &offset);
	SERV_DE* MakeCopy();

	byte m_length;
	FORMULA_Formula* m_formula;

	bool m_has_ustavka_and_limit;
	WORD m_ustavka_addr, m_limit_addr;
	WORD m_ustavka, m_limit;
};

typedef list<SERV_DE*> SERV_DEs;

struct SERV_BO
{
	SERV_BO();
	~SERV_BO();

	void Save(FILE* f);
	static SERV_BO* Load(FILE* f);

	CString GetString();
	CString GetInfo();
	SERV_BO* MakeCopy();

	byte m_device_addr, m_device_addr_ust;
	WORD m_request_stage;

	SERV_DEs m_data_elements;
};

typedef list<SERV_BO*> SERV_BOs;
struct SERV_Device;
struct SERV_DeviceCfg;

struct SERV_CalibrationPoint
{
	SERV_CalibrationPoint(WORD dsp = 0, WORD phys = 0):
		m_dsp_val(dsp), m_phys_val(phys)
	{
	}
	WORD m_dsp_val, m_phys_val;
};

typedef list<SERV_CalibrationPoint> SERV_CalibrationTable;

struct SERV_Channel
{
	SERV_Channel(CString name = "Канал", SERV_Device* device = NULL, byte data_offset = 0, byte data_len = 2);

	virtual CString GetInfo();
	virtual CString GetExtendedInfo();

	bool Save(FILE* f);
	static bool Load(SERV_Device* device, FILE* f);

	virtual WORD GetUniqueNumber();
	virtual WORD GetUniqueNumber(byte dev_addr);
	static WORD GetUniqueNumber(byte dev_addr, byte data_offset);

	friend struct SERV_Device;
	friend struct SERV_DeviceCfg;

	byte GetDataOffset() { return m_data_offset; }
	void SetDataOffset(byte data_offset);

	byte GetDataLength() { return m_data_len; }
	void SetDataLength(byte data_len) { m_data_len = data_len; }

	virtual float GetLastValue() { return (float)m_last_value;	}
	void SetLastValue(DWORD val);

	CString GetName() { return m_name; }
	void SetName(CString name) { m_name = name; }

	SERV_Device* GetDevice() { return m_device; }

	virtual bool IsTimedOut();

	byte GetCalibrationStage() { return m_calibration_stage; }
	void SetCalibrationStage(byte stage) { m_calibration_stage = stage;	}

	void GetCalibrationTable(SERV_CalibrationTable &table) { table = m_calibration_table; }
	byte GetCalibrationTableSize() { return m_calibration_table.size(); }
	SERV_CalibrationTable::iterator GetCalibrationTableBegin() { return m_calibration_table.begin(); }
	SERV_CalibrationTable::iterator GetCalibrationTableEnd() { return m_calibration_table.end(); }
	void ResetCalibrationTable() { m_calibration_table.clear(); }
	void AddCalibrationPoint(const SERV_CalibrationPoint &p) { m_calibration_table.push_back(p); }

	byte GetDSPNum() { return m_dsp_num; }
	void SetDSPNum(byte num) { m_dsp_num = num; }

	virtual byte GetConvNumber() { return 1; }

	float m_v0, m_v1;
	WORD m_dsp1;
	bool m_use_voltage, m_check_jumps, m_value_not_set;
	int m_jump_up, m_jump_down;

protected:
	CString m_name;
	SERV_Device* m_device;
	byte m_data_offset, m_data_len;

	DWORD m_last_value;

	byte m_dsp_num, m_calibration_stage;
	SERV_CalibrationTable m_calibration_table;

	bool CanBeCalirated();
};

typedef map<WORD, SERV_Channel*> SERV_ChannelsMap;

struct SERV_Device
{
	SERV_Device(CString name = "Устройство", byte device_addr = 0, int timeout = 1000, SERV_DeviceCfg* cfg = NULL);
	~SERV_Device();

	bool AddChannel(SERV_Channel *channel);
	bool RemoveChannel(SERV_Channel *channel);
	void DeleteAllChannels();

	CString GetName() { return m_name; }
	void SetName(CString name) { m_name = name; }
	CString GetInfo();

	SERV_DeviceCfg* GetCfg() { return m_cfg; }

	byte GetAddr() { return m_device_addr; }
	void SetAddr(byte addr);
	SERV_ChannelsMap::iterator GetChannelsBegin() { return m_channels.begin(); }
	SERV_ChannelsMap::iterator GetChannelsEnd() { return m_channels.end(); }
	int GetChannelsSize() { return m_channels.size(); }

	byte FindFreeOffset();

	bool Save(FILE* f);
	static bool Load(SERV_DeviceCfg* cfg, FILE* f);

	void StartCalibrationPause() { m_calibration_pause_tick = GetTickCount(); }
	bool InCalibrationPause() { return GetTickCount() - m_calibration_pause_tick < 1000; }

	friend struct SERV_Channel;

	bool IsTimedOut();
	DWORD m_timeout_interval, m_last_received_tick;
protected:
	bool ProtectedRemoveChannel(SERV_Channel *channel);
	CString m_name;
	byte m_device_addr;
	SERV_ChannelsMap m_channels; 
	SERV_DeviceCfg* m_cfg;
	DWORD m_calibration_pause_tick;
};

typedef map<byte, SERV_Device*> SERV_DevicesMap;

struct SERV_DeviceCfg
{
	SERV_DeviceCfg();
	~SERV_DeviceCfg();

	void Clear();

	bool Load(CString filename);
	bool Load(FILE* file);
	bool Save(CString filename = "");
	void SetName(CString filename) { m_file_name = filename; } 
	SERV_DeviceCfg* MakeCopy();
	bool IsLoadedOk() { return m_successfully_loaded; }

	SERV_DevicesMap::iterator GetDevicesBegin() { return m_devices.begin(); }
	SERV_DevicesMap::iterator GetDevicesEnd() { return m_devices.end(); }
	int GetDevicesCount() { return m_devices.size(); }
	CString GetFileName() { return m_file_name; }

	SERV_Device* GetDevice(byte addr);
	SERV_Channel* GetChannel(WORD uniq_num);

	friend struct SERV_Device;
	friend struct SERV_Channel;

protected:
	bool m_successfully_loaded;
	CString m_file_name;
	SERV_DevicesMap m_devices; 
	CMutex m_devices_map_mutex;
	SERV_ChannelsMap m_channels; 
	CMutex m_channels_map_mutex;
};
extern Indicator* SERV_calibration_indicator;

extern SERV_ConnectionInfo SERV_connection_info;

extern SERV_DeviceCfg* SERV_current_device_cfg;
extern CMutex SERV_current_device_cfg_mutex;

extern CMutex SERV_mutex_BO;

extern SERV_BOs SERV_indication_blocks, SERV_tmp_indication_blocks;
void SERV_InitConnection();
void SERV_ShutDownConnection(bool sync = false);

void SERV_DeviceCfgInit(CString path);
void SERV_DeviceCfgShutDown();

bool SERV_RequestCalibrationTable(SERV_Channel* channel, bool from_device_certanly = false);
void SERV_StopCalibration();
bool SERV_WriteCalibrationTable(SERV_Channel* channel);

void SERV_RequestMemory(HWND wnd, UINT msg, int device_num, byte* mem_ptr); 
void SERV_StopMemoryHandling(bool aborting);
void SERV_UploadMemory(HWND wnd, UINT msg, int device_num, byte* mem);
int SERV_GetCurrentMemoryAddr();
int SERV_GetRepeatOfLastCommand();

bool SERV_SaveConfigBO(CString path);
bool SERV_LoadConfigBO(CString path);
void SERV_SendIndication();
void SERV_ClearListBO(SERV_BOs& list_bo);

void SERV_SendParamToBKSD(CParam* param, WORD val);

#endif
////////////////////////////////////////////////////////////////////////////////
// end