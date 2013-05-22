// uic 18.04.2007

#include <StdAfx.h>
#include "LOG_V0.h"
#include "TXT.h"
#include <MutexWrap.h>

#include <map>
using namespace std;

CString LOG_V0_app_dir;
DWORD g_log_mask;

struct Logger
{
	Logger():
		m_file(NULL),
		m_prev_message_year(-1), m_prev_message_month(-1), m_prev_message_day(-1)
	{}

	void Init(CString file_name, CString mode)
	{
		MutexWrap log_access(m_log_mutex);
		if (m_file)
			ShutDown();
		SetupIndex(LOG_V0_MESSAGE, "");
		SetupIndex(LOG_V0_DEBUG_MESSAGE, "DBG:");
		SetupIndex(LOG_V0_TO_DB, "KRS > КБД:");
		SetupIndex(LOG_V0_FROM_DB, "KRS < КБД:");
	}

	bool IsInited()
	{
		MutexWrap log_access(m_log_mutex);
		return m_file != NULL;
	}

	void ShutDown()
	{
		MutexWrap log_access(m_log_mutex);

		if (m_file == NULL)
			return;
		fclose(m_file);
		m_file = NULL;
	}

	void SetupIndex(int index, CString text)
	{
		m_index_names[index] = text;
	}

	void AddMessage(int index, CString text, bool time, bool date)
	{
		MutexWrap log_access(m_log_mutex);

		SYSTEMTIME st;
		GetLocalTime(&st);

		CString line;
		CString str_date, str_time, str_date_time, file_name, str;
		str_date = TXT("%d.%02d.%02d")<<st.wYear<<st.wMonth<<st.wDay;
		str_time = TXT("%02d:%02d:%02d")<<st.wHour<<st.wMinute<<st.wSecond;
		if (time || date)
		{
			str_date_time = "[";
			if (date)
			{
				str_date_time += str_date;
				if (time)
					str_date_time += " ";
			}
			if (time)
				str_date_time += str_time;
			str_date_time += "] ";
		}
		line = m_index_names[index];
		if (line.GetLength() > 0)
			line += " ";
		line += text;

		if (m_prev_message_year != st.wYear || m_prev_message_month != st.wMonth || m_prev_message_day != st.wDay)
		{
			CString file_name = TXT("%04d.%02d.%02d.log.txt")<<st.wYear<<st.wMonth<<st.wDay;
			if (m_file)
			{
				str = TXT("[%s %s] Файл закрывается, продолжение отчёта в файле %s\n") << str_date << str_time << file_name;
				fputs(str, m_file);
				fclose(m_file);
			}
			m_file = fopen(LOG_V0_app_dir + "/LOG/" + file_name, "at");
			if (m_file)
			{
				if (m_prev_message_year != -1 && m_prev_message_month != -1 && m_prev_message_day != -1)
				{
					file_name = TXT("%04d.%02d.%02d.log.txt")<<m_prev_message_year<<m_prev_message_month<<m_prev_message_day;
					str = TXT("[%s %s] Продолжение файла-отчёта %s\n") << str_date << str_time << file_name;
					fputs(str, m_file);
				}
				m_prev_message_year = st.wYear;
				m_prev_message_month = st.wMonth;
				m_prev_message_day = st.wDay;
			}
		}
		if (m_file != NULL)
			fputs(str_date_time + line + "\n", m_file);
	}

	void Flush()
	{
		MutexWrap log_access(m_log_mutex);

		if (m_file == NULL)
			return;
		fflush(m_file);
	}

	FILE* m_file;

	map<WORD, CString> m_index_names;
	int m_prev_message_year, m_prev_message_month, m_prev_message_day;

	CMutex m_log_mutex;
};

static Logger g_logger;

void LOG_V0_Init()
{
	g_log_mask = 0xFFFFFFFF;
	SYSTEMTIME st;
	GetLocalTime(&st);
	char curr_dir[1024];
	GetCurrentDirectory(1024, curr_dir);
	LOG_V0_app_dir = curr_dir;
	CreateDirectory("LOG", NULL);
	g_logger.Init(LOG_V0_app_dir + "/LOG/" + (TXT("%04d.%02d.%02d.log.txt")<<st.wYear<<st.wMonth<<st.wDay), "a");
}

void LOG_V0_ShutDown()
{
	g_logger.ShutDown();
}

void LOG_V0_AddMessage(int index, CString text, bool time, bool date)
{
	if ((index & g_log_mask) == 0 || (g_logger.m_prev_message_year != -1 && !g_logger.IsInited()))
		return;
	g_logger.AddMessage(index, text, time, date);
	g_logger.Flush();
}

void LOG_V0_SetMask(DWORD mask)
{
	g_log_mask = mask;
}

void LOG_V0_Flush()
{
	if (!g_logger.IsInited())
		return;
	g_logger.Flush();
}
////////////////////////////////////////////////////////////////////////////////
// end