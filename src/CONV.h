#ifndef __CONV_H_
#define __CONV_H_

#include "BS.h"
#include "SERV.h"
#include <check.h>

#define CONV_MEASUREMENTS_TO_KEEP 500

struct CONV_ConnectionInfo: BS_ConnectionInfo
{
	CONV_ConnectionInfo();
	~CONV_ConnectionInfo();

	UINT m_params_info_ready_msg;
	UINT m_received_packets;
	bool m_measurement_started;

	map<int, BS_ParamDescription> m_param_descriptions;

	virtual bool IsConnected();
	virtual void SignalError(int error_code, CString reason);

protected:
	BS_Measurement m_last_measuremens[CONV_MEASUREMENTS_TO_KEEP];
	int m_get_measurement_pos, m_receive_measurement_pos, m_max_param_size;
};

struct CONV_Channel: SERV_Channel
{
	CONV_Channel(CString name = "Канал", SERV_Device* device = NULL, WORD num = 0);

	virtual CString GetInfo();
	virtual CString GetExtendedInfo();

	virtual WORD GetUniqueNumber();
	virtual WORD GetUniqueNumber(byte dev_addr);

	virtual float GetLastValue() { return m_last_float_value; }
	void SetLastValue(float val) { m_last_float_value = val; m_last_received_tick = GetTickCount(); }

	virtual bool IsTimedOut();

	virtual byte GetConvNumber() { return 0; }

protected:
	float m_last_float_value;

	DWORD m_last_received_tick;
	WORD m_channel_num;
};

extern CONV_ConnectionInfo CONV_connection_info;
void CONV_InitConnection();
void CONV_ShutDownConnection(bool sync = false);
void CONV_RequestParamInfo();
void CONV_GetParamInfo(BS_ParamDescription* param_description);
void CONV_StartMeasure();
void CONV_StopMeasure();
BS_Measurement* CONV_GetMeasurement(bool &timeout);
DWORD CONV_GetLastTick();

#endif
////////////////////////////////////////////////////////////////////////////////
// end