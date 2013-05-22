#include "stdafx.h"
#include "krs.h"
#include "DlgEditMarkerType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_DB_DATA_RECEIVED (WM_USER + 1)
#define WM_DB_ERROR (WM_USER + 2)

HANDLE g_db_answered_ok_event = CreateEvent(NULL, FALSE, FALSE, NULL),
		g_db_error_event = CreateEvent(NULL, FALSE, FALSE, NULL);

int g_new_index = 0;
CString g_new_name = " - пусто -";

DlgEditMarkerType::DlgEditMarkerType(BS_MarkerType* marker, CWnd* pParent /*=NULL*/)
	: CDialog(DlgEditMarkerType::IDD, pParent),
	m_marker(marker), m_dlg_waiting(false)
{
	//{{AFX_DATA_INIT(DlgEditMarkerType)
	//}}AFX_DATA_INIT
}

void DlgEditMarkerType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEditMarkerType)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgEditMarkerType, CDialog)
	//{{AFX_MSG_MAP(DlgEditMarkerType)
	ON_MESSAGE(WM_DB_DATA_RECEIVED, OnDataDB)
	ON_MESSAGE(WM_DB_ERROR, OnErrorDB)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL DlgEditMarkerType::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_EDIT_NAME, m_marker->m_name);

	return TRUE;
}

LRESULT DlgEditMarkerType::OnDataDB(WPARAM, LPARAM)
{
	SetEvent(g_db_answered_ok_event);
	return +1;
}

LRESULT DlgEditMarkerType::OnErrorDB(WPARAM code, LPARAM string)
{
	SetEvent(g_db_error_event);
	return +1;
}

UINT UpdatingDBThreadProcedure(LPVOID param)
{
	DlgEditMarkerType* dlg = (DlgEditMarkerType*)param;
	while (!dlg->m_dlg_waiting.m_inited) Sleep(20);
	dlg->m_dlg_waiting.SetWindowText("Подождите. Идёт обмен данными с БД...");


	DB_TransferMarkerTypePacket transfer_packet;
	transfer_packet.m_marker_type.m_index = g_new_index;
	transfer_packet.m_marker_type.m_name = g_new_name;

	dlg->m_dlg_waiting.Start(10000);
	if (DB_SendPacket(&transfer_packet)) 
	{
		HANDLE events[2] = {g_db_answered_ok_event, g_db_error_event};
		DWORD res = WaitForMultipleObjects(2, events, FALSE, 10000);
		if (res == WAIT_OBJECT_0)
		{
			dlg->m_dlg_waiting.SignalToClose(IDOK);
			return +1;
		}
	}
	dlg->m_dlg_waiting.SignalToClose(IDCANCEL);
	return +1;
}

void DlgEditMarkerType::OnOK() 
{
	GetDlgItemText(IDC_EDIT_NAME, g_new_name);

	map<int, BS_MarkerType>::iterator current = DB_marker_types.begin();
	while (current != DB_marker_types.end())
	{
		if (g_new_name.Compare(current->second.m_name) == 0 &&
			m_marker->m_index != current->second.m_index)
		{
			MessageBox("Имя типа маркера не уникально", "Невозможно задать новое имя");
			return;
		}
		current++;
	}

	bool need_to_update_db = m_marker->m_index == 0 || (g_new_name.Compare(m_marker->m_name) != 0);

	if (m_marker->m_index == 0)
	{
		int max_count = DB_marker_types.size() + 1;
		for (g_new_index = 1; g_new_index <= max_count; g_new_index++)
		{
			if (DB_marker_types.find(g_new_index) == DB_marker_types.end())
				break;
		}
	}
	else
		g_new_index = m_marker->m_index;

	if (need_to_update_db)
	{
		DWORD old_data_msg = DB_connection_info.m_data_received_msg;
		DWORD old_err_msg = DB_connection_info.m_error_occur_msg;
		HWND old_wnd = DB_connection_info.m_wnd;

		DB_connection_info.m_data_received_msg = WM_DB_DATA_RECEIVED;
		DB_connection_info.m_error_occur_msg = WM_DB_ERROR;
		DB_connection_info.m_wnd = m_hWnd;

		ResetEvent(g_db_answered_ok_event);

		AfxBeginThread(UpdatingDBThreadProcedure, this, THREAD_PRIORITY_NORMAL);
		int res = m_dlg_waiting.DoModal();

		DB_connection_info.m_data_received_msg = old_data_msg;
		DB_connection_info.m_error_occur_msg = old_err_msg;
		DB_connection_info.m_wnd = old_wnd;

		if (res != IDOK)
		{
			MessageBox("Новый тип маркера не создан", "Ошибка связи с БД");
			return;
		}
	}

	m_marker->m_name = g_new_name;

	if (m_marker->m_index == 0)
	{
		m_marker->m_index = g_new_index;
		DB_marker_types[m_marker->m_index] = *m_marker;
	}

	CDialog::OnOK();
}
