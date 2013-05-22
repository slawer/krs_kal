#include <StdAfx.h>
#include "DBGET.h"
#include "LOG_V0.h"

#define NOT_GETTING -1
#define GET_STARTED 0
#define DATA_STARTED 1
#define DATA_FINISHED 2

struct PrevValue
{
	PrevValue(int num = 0):
		m_param_num(num), m_prev_val((float)PARAM_TO_VAL), m_prev_time(0)
	{}
	int m_param_num;
	float m_prev_val;
	double m_prev_time;
};

static map<int, PrevValue> g_prev_values;

static DWORD g_start_pachka_time = 0;
static int g_getting_stage = NOT_GETTING;
#define INVALID_TIME_DEEPNESS_VALUE -1010101010
static double g_request_start_time, g_request_final_time, g_request_start_deepness, g_request_final_deepness;
static double g_last_time = -1, g_last_deepness = -1;
static bool g_wait_for_first_packet; 

void DBGET_Get(DB_RequestParamValuesPacket* packet)
{
	g_getting_stage = GET_STARTED;
	if (packet->m_from_time <= 0)
	{
		g_request_start_time = INVALID_TIME_DEEPNESS_VALUE;
		g_request_final_time = INVALID_TIME_DEEPNESS_VALUE;
		g_request_start_deepness = -packet->m_from_time;
		g_request_final_deepness = packet->m_to_time;
	}
	else
	{
		g_request_start_time = packet->m_from_time;
		g_request_final_time = packet->m_to_time;
		g_request_start_deepness = INVALID_TIME_DEEPNESS_VALUE;
		g_request_final_deepness = INVALID_TIME_DEEPNESS_VALUE;
	}
	g_last_time = g_last_deepness = -1;
	g_wait_for_first_packet = false;
	g_prev_values.clear();
	list<int>::iterator current = packet->m_params_indexes.begin();
	int index;
	CParam* param;
	while (current != packet->m_params_indexes.end())
	{
		index = *current;
		if (!m_MapParamDB.Lookup(index, param))
		{
			LOG_V0_AddMessage(LOG_V0_FROM_DB, TXT("Нет параметра %d") << index);
			continue;
		}
		param->StartTimeOutBD();

		g_prev_values[index].m_param_num = index;
		current++;
	}
    DB_SendPacket(packet);
}
void HandlePacketV1(DB_TransferParamValuesPacket* transfer_packet, CMainView* view)
{
	int i, j, param_num, time_pos, param_pos;
	double time, prev_time, diff_time, min_diff_time = 1.0/(24*60*60*10), k;
	float value, prev_val;
	BS_OneParamValue *om;
	BS_Measurement *m;
	bool
		signal_packet = ((transfer_packet->m_measurements_total == -1) || (transfer_packet->m_measurements_size == 1 && transfer_packet->m_measurements->m_param_values_size == 0)),
		was_timeout;
	CParam* param;

	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, TXT("DBGET_HandlePacket start stage = %d") << g_getting_stage);
	switch (g_getting_stage)
	{
		case GET_STARTED:
			if (signal_packet) 
			{
				g_getting_stage = DATA_STARTED;
				g_wait_for_first_packet = true;
				LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, TXT("DBGET_HandlePacket stage changed to = %d (DATA_STARTED)") << g_getting_stage);
			}
			else
			{
				m = transfer_packet->m_measurements;
				g_last_time = m->m_time;
				g_last_deepness = m->m_deepness;
				for (i = 0; i < transfer_packet->m_measurements_size; i++, m++)
				{
					om = m->m_param_values;
					for (j = 0; j < m->m_param_values_size; j++, om++)
					{
						param_num = om->m_param_index;
						g_prev_values[param_num].m_prev_time = m->m_time;
						g_prev_values[param_num].m_prev_val = om->m_param_value;
					}
				}
			}
			return;
		case DATA_STARTED:
		case DATA_FINISHED:
			if (signal_packet)
			{
				if (g_getting_stage == DATA_STARTED)
				{
					g_getting_stage = DATA_FINISHED;
					g_wait_for_first_packet = true;
					LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, TXT("DBGET_HandlePacket stage changed to = %d (DATA_FINISHED)") << g_getting_stage);
					return;
				}
				else
				{
					g_getting_stage = NOT_GETTING;
					view->UpdateInputDataDB();
					m_wndDialogBarDB.FinishRequestDB();
					LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, TXT("DBGET_HandlePacket stage changed to = %d (NOT_GETTING)") << g_getting_stage);
					return;
				}
			}

			if (transfer_packet->m_measurements_size <= 0)
				return;
			m = transfer_packet->m_measurements;
			if (g_wait_for_first_packet)
			{
				g_wait_for_first_packet = false;
				bool add = true;
				double time = (g_getting_stage == DATA_STARTED) ? g_request_start_time : g_request_final_time;
				double deepness = (g_getting_stage == DATA_STARTED) ? g_request_start_deepness : g_request_final_deepness;
				if (g_last_time == -1) 
				{
					deepness = m->m_deepness;
					time = m->m_time;
					add = (g_getting_stage == DATA_FINISHED);
				}
				else
				if (time == INVALID_TIME_DEEPNESS_VALUE) 
				{
					time = g_last_time + (deepness - g_last_deepness) * (m->m_time - g_last_time) / (m->m_deepness - g_last_deepness);
				}
				else
				{
					deepness = g_last_deepness + (time - g_last_time) * (m->m_deepness - g_last_deepness) / (m->m_time - g_last_time);
				}
				g_last_time = g_last_time = -1;
				if (add)
				{
					view->AddTimeGlub(time, deepness);
					/*
					time_pos = view->m_ptmData->GetSizeData() - 1;
					if (g_getting_stage == DATA_STARTED)
					{
						map<int, PrevValue>::iterator current = g_prev_values.begin();
						while(current != g_prev_values.end())
						{
							if (current->second.m_prev_val < PARAM_TO_VAL_CHECK)
							{
								if (!m_MapParamDB.Lookup(current->first, param))
								{
									LOG_V0_AddMessage(LOG_V0_FROM_DB, TXT("Нет параметра %d") << param_num);
									current++;
									continue;
								}
								param->FinishTimeOutBD(time_pos);
							}
							current++;
						}
					}
					/**/
				}
			}

			for (i = 0; i < transfer_packet->m_measurements_size; i++, m++)
			{
				g_last_time = m->m_time;
				g_last_deepness = m->m_deepness;
				if (g_getting_stage == DATA_STARTED)
				{
					view->AddTimeGlub(m->m_time, (double)m->m_deepness);
					if (m->m_time > 40493.5826)
						int xzzz=9;
				}
				time_pos = view->m_ptmData->GetSizeData() - 1;

				om = m->m_param_values;
				for (j = 0; j < m->m_param_values_size; j++, om++)
				{
					param_num = om->m_param_index;

					if (!m_MapParamDB.Lookup(param_num, param))
					{
						LOG_V0_AddMessage(LOG_V0_FROM_DB, TXT("Нет параметра %d") << param_num);
						continue;
					}

					if (om->m_param_value > PARAM_TO_VAL_CHECK)
					{
						param->StartTimeOutBD();
						g_prev_values[param_num].m_prev_time = m->m_time;
						g_prev_values[param_num].m_prev_val = om->m_param_value;
						continue;
					}

					was_timeout = param->IsTimedOutDB();
					if (was_timeout)
					{
						if (g_getting_stage == DATA_FINISHED)
							continue;
						param->FinishTimeOutBD(time_pos); 
					}
					param_pos = param->pBufParam->curFrag->GetCurGP();

					if (g_prev_values.find(param_num) == g_prev_values.end())
					{
						g_prev_values[param_num].m_prev_val = PARAM_TO_VAL;
						g_prev_values[param_num].m_prev_time = m->m_time;
					}

					if (g_getting_stage != DATA_FINISHED &&
						(	was_timeout || g_prev_values[param_num].m_prev_val > PARAM_TO_VAL_CHECK || 
							time_pos == param_pos + 1)
						) 
					{
						*(param->pBufParam) << om->m_param_value;
						param_pos = param->pBufParam->curFrag->GetCurGP();
					}
					else
					if (g_getting_stage == DATA_FINISHED || time_pos > param_pos + 1) 
					{
						prev_val = g_prev_values[param_num].m_prev_val;
						prev_time = g_prev_values[param_num].m_prev_time;
						diff_time = m->m_time - prev_time;
						if (fabs(diff_time) < min_diff_time)
							k = 0;
						else
							k = (om->m_param_value - prev_val)/diff_time;
						while (param_pos != time_pos)
						{
							time = view->m_ptmData->GetElement(++param_pos);
							value = prev_val + (time - prev_time) * k;
							*(param->pBufParam) << value;
						}
					}
					else
						int zzz = 0;

					g_prev_values[param_num].m_prev_time = m->m_time;
					g_prev_values[param_num].m_prev_val = om->m_param_value;
				}
			}
			return;
		default:
			return;
	}
}

void DBGET_Start()
{
	g_start_pachka_time = GetTickCount();
}

void HandlePacketV0(DB_TransferParamValuesPacket* transfer_packet, CMainView* view)
{
	CParam *param;
	m_wndDialogBarDB.IncrementBytes(transfer_packet->Size());

	if (transfer_packet->m_measurements_start_from  == -1 && transfer_packet->m_measurements_total == -1)
	{
		g_getting_stage = NOT_GETTING;
		m_wndDialogBarDB.FinishRequestDB();
		m_wndDialogBarDB.IncrementTime(GetTickCount() - g_start_pachka_time);
	}
	else
	{
		if (transfer_packet->m_measurements_start_from + transfer_packet->m_measurements_size >= transfer_packet->m_measurements_total) 
			m_wndDialogBarDB.IncrementTime(GetTickCount() - g_start_pachka_time);

		m_wndDialogBarDB.m_progress_db.SetRange32(0, transfer_packet->m_measurements_total); 
		m_wndDialogBarDB.m_progress_db.SetPos(transfer_packet->m_measurements_start_from + transfer_packet->m_measurements_size);
		BS_OneParamValue *om;
		int i, j, pos;

		double prev_time, prev_deepness;
		bool global_timeout;
		BS_Measurement *m = transfer_packet->m_measurements;
		for (i = 0; i < transfer_packet->m_measurements_size; i++, m++)
		{
			if (view->m_ptmData->GetSizeData() > 0)
			{
				prev_time = view->m_ptmData->GetLastData();
				prev_deepness = view->m_pcurGlub->GetLastData();
				global_timeout = m->m_time - prev_time > (10*1.0)/(24*60*60);
				if (global_timeout)
					view->AddTimeGlub(prev_time + 0.1/(24*60*60), prev_deepness);
			}
			else
				global_timeout = false;
			view->AddTimeGlub(m->m_time, (double)m->m_deepness);
			pos = view->m_ptmData->GetSizeData() - 1;

			om = m->m_param_values;
			for (j = 0; j < m->m_param_values_size; j++, om++)
			{
				if (!m_MapParamDB.Lookup(om->m_param_index, param))
				{
					LOG_V0_AddMessage(LOG_V0_FROM_DB, TXT("Нет параметра %d") << om->m_param_index);
					continue;
				}

				BOOL flg = param->m_index_of_last_measurement + 1 != m->m_index;

				param->m_index_of_last_measurement = m->m_index;
				*(param->pBufParam) << om->m_param_value;

				if (flg || global_timeout) param->StartTimeOutBD(); 
				param->FinishTimeOutBD(pos);
			}
		}
		view->UpdateInputDataDB();

		if (transfer_packet->m_measurements_start_from + transfer_packet->m_measurements_size >= transfer_packet->m_measurements_total)
		{
			m_wndDialogBarDB.m_progress_db.SetPos(0);
			g_start_pachka_time = 0;
		}
	}
}

bool DBGET_RequestFinished()
{
	return (g_getting_stage == NOT_GETTING);
}

void DBGET_HandlePacket(DB_TransferParamValuesPacket* transfer_packet, CMainView* view)
{
	if (DB_connection_info.m_db_version <= 0)
		HandlePacketV0(transfer_packet, view);
	else
		HandlePacketV1(transfer_packet, view);
}

////////////////////////////////////////////////////////////////////////////////
// end