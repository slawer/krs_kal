#include <StdAfx.h>
#include <check.h>
#include <MutexWrap.h>
#include "NET.h"
#include "LOG_V0.h"
#include "TXT.h"

bool NET_shutdown = false;

NET_Thread::NET_Thread(NET_BaseSocket*socket):
	m_socket(socket),
	m_executing(false),
	m_events(NULL), m_events_num(0)
{
	m_started_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_thread_stopped_event = CreateEvent(NULL, FALSE, FALSE, NULL);
}

NET_Thread::~NET_Thread()
{
	CHECK(!m_executing);
	CloseAllHandles();
}

void NET_Thread::SetSocket(NET_BaseSocket* socket)
{
	m_socket = socket;
}

void NET_Thread::CloseAllHandles()
{
	if (m_events)
	{
		for (int i = 0; i < (int)m_events_num; i++)
			CloseHandle(m_events[i]);
		delete[] m_events;
		m_events = NULL;
	}
}

void NET_Thread::ReserveHandleSpace(DWORD num)
{
	CloseAllHandles();
	m_events_num = num + 1;
	m_events = new HANDLE[m_events_num];
	m_events[0] = WSACreateEvent();
}

/*
HANDLE* NET_Thread::GetHandleAddr(DWORD num)
{
	return m_events + 1 + num;
}
*/

HANDLE& NET_Thread::Event(DWORD num)
{
	return m_events[num + 1];
}

/*
void NET_Thread::SetEvents(HANDLE* events, DWORD events_num)
{
	m_events_num = events_num + 1;
	if (m_events)
		delete m_events;
	m_events = new HANDLE[m_events_num];
	m_events[0] = m_finishing_event;
	for (int i = 0; i < events_num; i++)
		m_events[i + 1] = events[i];
}
*/

UINT ThreadProcedure(LPVOID param)
{
	NET_Thread* the_thread = ((NET_Thread*)param);
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, the_thread->m_socket->GetName() + " " + the_thread->GetName() + " ThreadProcedure started");
	the_thread->BeforeStart();
	the_thread->Execute();
	the_thread->AfterStop();
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, the_thread->m_socket->GetName() + " " + the_thread->GetName() + " ThreadProcedure finished");
	the_thread->m_executing = false;
	SetEvent(the_thread->m_thread_stopped_event);
	return 1;
}

void NET_Thread::Start(int priority)
{
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, m_socket->GetName() + " " + GetName() + "::Start()");
	CHECK(m_events && m_events_num > 0);
	WSAResetEvent(m_events[0]);
	AfxBeginThread(ThreadProcedure, this, priority);
	WaitForSingleObject(m_started_event, INFINITE);
}

void NET_Thread::Stop()
{
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, m_socket->GetName() + " " + GetName() + "::Stop() started");
	if (!m_executing)
	{
		LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, m_socket->GetName() + " " + GetName() + "::Stop() finished (was not executing)");
		return;
	}
	BeforeStop();
	WSASetEvent(m_events[0]); 
	WaitForSingleObject(m_thread_stopped_event, INFINITE);
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, m_socket->GetName() + " " + GetName() + "::Stop() finished");
}

void NET_Thread::OnEvent(DWORD event_num)
{
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, m_socket->GetName() + " " + GetName() + (TXT("::OnEvent(%d)  - unhandled event!") << event_num));
	CString str;
	str.Format("Event %d unhandled", event_num);
	CRUSH(str);
}

void NET_Thread::Execute()
{
	m_executing = true;
	SetEvent(m_started_event);
	DWORD res, i;
	HANDLE one_event;
	while (true)
	{
		res = WaitForMultipleObjects(m_events_num, m_events, FALSE, INFINITE);
		if (res == WAIT_OBJECT_0)
			return;
		for (i = 0; i < m_events_num - 1; i++)
		{
			one_event = Event(i);
			if (res == WAIT_OBJECT_0 + (i + 1) || WaitForSingleObject(one_event, 0) == WAIT_OBJECT_0)
			{
				WSAResetEvent(one_event);
				{
					//MutexWrap socket_access(m_socket->m_socket_mutex);
					if (m_socket->m_created)
					{
						MutexWrap socket_access(m_socket->m_socket_mutex);
						OnEvent(i);
					}
				}
			}
		}
	}
	CRUSH("Wrong NET_Thread::Execute() exit");
}
NET_ReadThread::NET_ReadThread(NET_BaseSocket* socket):
	NET_Thread(socket)
{
	ReserveHandleSpace(1);
	Event(0) = WSACreateEvent(); 
}

void NET_ReadThread::BeforeStart()
{
	m_socket->SelectEvent(Event(0), FD_READ | FD_CLOSE); 
	m_overlapped.hEvent = WSACreateEvent();
}

void NET_ReadThread::BeforeStop()
{
	m_socket->SelectEvent(Event(0), 0); 
	WSASetEvent(m_overlapped.hEvent);
}

void NET_ReadThread::AfterStop()
{
	WSACloseEvent(m_overlapped.hEvent);
}

void NET_ReadThread::OnEvent(DWORD event_num)
{
	CString log;

	WSANETWORKEVENTS wsa_events;
	switch(event_num)
	{
		case 0: 
			WSAEnumNetworkEvents(m_socket->GetSocket(), Event(0), &wsa_events);

/**/
			log = m_socket->GetName() + " " + GetName() + (TXT(" events = 0x%08X err =") << wsa_events.lNetworkEvents);
			for (int i = 0; i < FD_MAX_EVENTS; i++)
				if (wsa_events.lNetworkEvents & (1<<i))
					log += TXT(" %d") << wsa_events.iErrorCode[i];

/**/
			if (wsa_events.lNetworkEvents & FD_CLOSE)
			{
				LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, log);
				m_socket->SelectEvent(Event(0), 0);
				AfxBeginThread(TerminationThreadProcedure, m_socket, THREAD_PRIORITY_LOWEST);
				return;
			}
			if (wsa_events.lNetworkEvents & FD_READ && wsa_events.iErrorCode[FD_READ_BIT] == 0)
				m_socket->PrivateOnReceived();
			else
			if (wsa_events.lNetworkEvents != 0)
			{
				CString err;
				err.Format("NET_ReadThread::OnEvent, events = 0x%08X", wsa_events.lNetworkEvents);
				m_socket->OnError(err);
			}
			break;
		default:
			CRUSH("wrong event_num in NET_ReadThread::OnEvent");
	}
}
NET_WriteThread::NET_WriteThread(NET_BaseSocket* socket):
	NET_Thread(socket)
{
	ReserveHandleSpace(1);
	Event(0) = WSACreateEvent(); // write
}

void NET_WriteThread::BeforeStart()
{
	m_overlapped.hEvent = WSACreateEvent();
}

void NET_WriteThread::BeforeStop()
{
	WSASetEvent(m_overlapped.hEvent);
}

void NET_WriteThread::AfterStop()
{
	WSACloseEvent(m_overlapped.hEvent);
}

void NET_WriteThread::OnEvent(DWORD event_num)
{
	switch(event_num)
	{
		case 0:
			m_socket->PrivateOnSend();
			break;
		default:
			CRUSH("wrong event_num in NET_WriteThread::OnEvent");
	}
}
NET_WaitThread::NET_WaitThread(NET_BaseSocket* socket):
	NET_Thread(socket)
{
	ReserveHandleSpace(WT_START_CONNECT + 1);
	Event(WT_CONNECTED) = WSACreateEvent(); 
	Event(WT_ACCEPTED) = WSACreateEvent(); 
	Event(WT_CLOSE) = WSACreateEvent(); 
	Event(WT_STOP_CONNECT) = WSACreateEvent(); 
	Event(WT_START_CONNECT) = WSACreateEvent(); 
}

void NET_WaitThread::BeforeStart()
{
}

void NET_WaitThread::BeforeStop()
{
}

void NET_WaitThread::OnEvent(DWORD event_num)
{
	WSANETWORKEVENTS wsa_events;
	CString log;
	switch(event_num)
	{
		case WT_CONNECTED:
			WSAEnumNetworkEvents(m_socket->GetSocket(), Event(WT_CONNECTED), &wsa_events);
			if ((wsa_events.lNetworkEvents & FD_CONNECT) == 0 || wsa_events.iErrorCode[FD_CONNECT_BIT] != 0)
			{
				if (wsa_events.iErrorCode[FD_CONNECT_BIT] == 0)
				{
					CString err;
					err.Format("NET_WaitThread::OnEvent, WT_CONNECTED error_code = %d", wsa_events.iErrorCode[FD_CONNECT_BIT]);
					m_socket->OnError(err);
				}
				break;
			}
			m_socket->SelectEvent(Event(WT_CONNECTED), 0);
			((NET_Client*)m_socket)->PrivateOnConnected();
			break;
		case WT_ACCEPTED:
			((NET_Server*)m_socket)->PrivateOnConnectionAccepted();
			break;
		case WT_STOP_CONNECT:
			m_socket->SelectEvent(m_socket->m_read_thread.Event(0), 0);
			AfxBeginThread(TerminationThreadProcedure, m_socket, THREAD_PRIORITY_LOWEST);
			break;
		case WT_START_CONNECT:
			if (NET_shutdown)
			{
				LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, "Trying to Connect!!!");
				return;
			}
			if (!((NET_Client*)m_socket)->PrivateConnect())
			{
				Sleep(200);
				if (!NET_shutdown)
					WSASetEvent(Event(WT_START_CONNECT));
			}
			break;
		default:
			CRUSH("wrong event_num in NET_WaitThread::OnEvent");
	}
}
#define OP2(func, cond, err) m_result = func; if (m_result != 0) { m_last_error = WSAGetLastError(); if (cond) { OnError(err); return; } }
#define OP3(func, err) OP2(func, false, err);
#define OP(func, err) OP2(func, true, err);
NET_Connection::NET_Connection(byte sybtype):
	NET_BaseSocket(NET_TYPE_CONNECTION, sybtype),
	m_server(NULL)
{
}

void NET_Connection::PrivateOnClose()
{
	if (m_server)
		m_server->PrivateOnConnectionClosed(this);
	else
		OnClose();
}

void NET_Connection::InitConnection(SOCKET& socket, NET_Server* server)
{
	m_socket = socket;
	m_created = true;
	m_server = server;
	PrivateCreateBuffers();
	m_read_thread.Start(THREAD_PRIORITY_NORMAL);
	m_write_thread.Start();
}
NET_Server::NET_Server(byte sybtype):
	NET_BaseSocket(NET_TYPE_SERVER, sybtype),
	m_listening(false),
	m_listening_port(-1)
{
}

void NET_Server::PrivateOnClose()
{
	if (m_listening)
		PrivateStopListen();
}

void NET_Server::StartListen(WORD port)
{
//	MutexWrap socket_access(m_socket_mutex);
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + (TXT("::StartListen(%d)") << port));

	if (m_listening)
		PrivateStopListen();

	if (!m_created)
		Create();
	if (!m_created)
		return;

	if (m_listening_port != port)
	{
		sockaddr_in local_addr;
		local_addr.sin_family = AF_INET;
		local_addr.sin_port = htons(port);
		local_addr.sin_addr.s_addr = 0;
		OP( bind(m_socket, (sockaddr *)&local_addr, sizeof(local_addr)), "NET_Server::StartListen, bind" );
		m_listening_port = port;
	}
	OP( listen(m_socket, 0x100), "NET_Server::StartListen, listen" );
	SelectEvent(m_wait_thread.Event(WT_ACCEPTED), FD_ACCEPT);
	m_listening = true;
}

void NET_Server::StopListen()
{
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + "::StopListen()");

	PrivateStopListen();
}

void NET_Server::PrivateStopListen()
{
	if (!m_created || !m_listening)
		return;

	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + "::PrivateStopListen() started");

	CHECK(m_socket != INVALID_SOCKET);
	SelectEvent(m_wait_thread.Event(WT_ACCEPTED), 0);
	{
		MutexWrap connections_access(m_connections_mutex);
		while (!m_connections.empty())
		{
			NET_Connection* connection = m_connections.back();
			connection->m_read_thread.Stop();
			connection->m_write_thread.Stop();
			connection->m_server = NULL;
			m_connections.pop_back();
			connection->PrivateClose();

			OnConnectionClosed(connection);
			delete connection;
		}
	}
	m_wait_thread.Stop();
	OP2( shutdown(m_socket, SD_BOTH), m_last_error != WSAENOTCONN, "NET_Server::PrivateStopListen, shutdown" );
	OP( closesocket(m_socket), "NET_Server::PrivateStopListen, closesocket" );
	m_socket = INVALID_SOCKET;
	m_listening = false;
	m_listening_port = -1;
	m_created = false;

	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + "::PrivateStopListen() finished");
}

void NET_Server::PrivateOnConnectionAccepted()
{
	sockaddr sa;
	int sa_len = sizeof(sa);
	SOCKET new_conn_socket = WSAAccept(m_socket, &sa, &sa_len, NULL, 0);
	if (new_conn_socket == INVALID_SOCKET)
	{
		OnError("NET_Server::PrivateOnConnectionAccepted, new_conn_socket == INVALID_SOCKET");
		return;
	}
	NET_Connection* new_connection = CreateNewConnection();
	new_connection->SetBuffersSize(m_in_buf_size, m_in_data_size, m_out_buf_size, m_out_data_size);
	{
		MutexWrap connections_access(m_connections_mutex);
		new_connection->InitConnection(new_conn_socket, this);
		m_connections.push_back(new_connection);
	}
	OnConnectionAccepted(new_connection);
}

void NET_Server::PrivateOnConnectionClosed(NET_Connection* connection)
{
	CString connection_name = connection->GetName();
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + (TXT("::PrivateOnConnectionClosed(%s) started") << connection_name));

	list<NET_Connection*>::iterator current = m_connections.begin();
	while (current != m_connections.end())
	{
		if (*current == connection)
		{
			connection->m_read_thread.Stop();
			connection->m_write_thread.Stop();
			connection->m_server = NULL;
			m_connections.erase(current);
			connection->PrivateClose();

			OnConnectionClosed(connection);
			delete connection;
			break;
		}
		current++;
	}
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + (TXT("::PrivateOnConnectionClosed(%s) finished") << connection_name));
}
NET_Client::NET_Client(byte sybtype):
	NET_BaseSocket(NET_TYPE_CLIENT, sybtype),
	m_connecting(false), m_connected(false)
{
}

void NET_Client::PrivateOnClose()
{
	{
		PrivateStopConnect();
		OnBreakedConnection();
	}
}

bool NET_Client::CalculateSocketAddress()
{
	m_connect_addr.sin_family = AF_INET;
	m_connect_addr.sin_port = htons(m_to_port);
	HOSTENT *hst;

	DWORD inet_address = inet_addr(m_to_host);
	if (inet_address != INADDR_NONE)
		m_connect_addr.sin_addr.s_addr = inet_address;
	else
	{
		hst = gethostbyname(m_to_host);
		if (hst == NULL)
			return false;
		((unsigned long *)&m_connect_addr.sin_addr)[0] = ((unsigned long **)hst->h_addr_list)[0][0];
	}
	return true;
}

void NET_Client::StartConnect(CString to_host, WORD to_port)
{
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + (TXT("::StartConnect(%s, %d)") << to_host << to_port));

	if (m_connecting || m_connected)
		PrivateStopConnect();

	if (!m_created)
		Create();

	m_connecting = true;
	m_to_host = to_host;
	m_to_port = to_port;
	if (!CalculateSocketAddress())
	{
		m_connecting = false;
		return;
	}
	SelectEvent(m_wait_thread.Event(WT_CONNECTED), FD_CONNECT);
	WSASetEvent(m_wait_thread.Event(WT_START_CONNECT));
}

void NET_Client::StopConnect(bool asyncro)
{
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + (TXT("::StopConnect(%s)") << (asyncro?"async":"sync")));

//	MutexWrap socket_access(m_socket_mutex);
	if (!m_created/* || (!m_connecting && !m_connected)*/)
		return;

	if (asyncro)
	{
		WSASetEvent(m_wait_thread.Event(WT_STOP_CONNECT));
	}
	else
		PrivateStopConnect();
}

void NET_Client::PrivateStopConnect()
{
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + (TXT("::PrivateStopConnect(m_connecting = %d, m_connected = %d, m_created = %d)") << (m_connecting?1:0) << (m_connected?1:0) << (m_created?1:0) ));
	m_connecting = false;
	m_connected = false;

	if (!m_created)
		return;

	PrivateClose();
}

void NET_Client::PrivateOnConnected()
{
//	MutexWrap socket_access(m_socket_mutex);

	m_connecting = false;
	m_connected = true;
	m_read_thread.Start(THREAD_PRIORITY_NORMAL);
	m_write_thread.Start(THREAD_PRIORITY_NORMAL);
	OnConnected();
}

bool NET_Client::PrivateConnect()
{
	CalculateSocketAddress();
	if (connect(m_socket, (sockaddr*)&m_connect_addr, sizeof(sockaddr_in)) != 0)
		return !m_connecting;
	return true;
}
NET_BaseSocket::NET_BaseSocket(byte type, byte subtype):
	m_created(false), m_socket(INVALID_SOCKET),
	m_in_buf(NULL), m_in_buf_size(0),
	m_in_data(NULL), m_in_data_size(0),
	m_out_buf(NULL), m_out_buf_size(0),
	m_out_data(NULL), m_out_data_size(0),
	m_socket_type(type), m_socket_subtype(subtype)
{
	if (m_socket_type != NET_TYPE_SERVER)
	{
		m_read_thread.SetSocket(this);
		m_write_thread.SetSocket(this);
	}
	if (m_socket_type != NET_TYPE_CONNECTION)
		m_wait_thread.SetSocket(this);
}

NET_BaseSocket::~NET_BaseSocket()
{
	PrivateClose();
}

void NET_BaseSocket::SelectEvent(HANDLE& event_obj, LONG net_event)
{
	OP( WSAEventSelect(m_socket, event_obj, net_event), "NET_BaseSocket::SelectEvent, WSAEventSelect" );
}

void NET_BaseSocket::Create()
{
//	MutexWrap socket_access(m_socket_mutex);

	CHECK(m_socket_type != NET_TYPE_CONNECTION);

	if (m_created)
		return;

	m_socket = WSASocket(AF_INET, SOCK_STREAM, 0, (LPWSAPROTOCOL_INFO)NULL, 0, 0/*WSA_FLAG_OVERLAPPED*/);
	m_created = (m_socket != INVALID_SOCKET);
	if (!m_created)
	{
		OnError("NET_BaseSocket::Create, !m_created");
		return;
	}

	PrivateCreateBuffers();
	
	m_wait_thread.Start(THREAD_PRIORITY_NORMAL);
}

void NET_BaseSocket::PrivateCreateBuffers()
{
	if (m_socket_type == NET_TYPE_SERVER)
		return;

	if (m_in_buf)
		delete[] m_in_buf;
	if (m_in_buf_size)
		m_in_buf = new char[m_in_buf_size];
	else
		m_in_buf = NULL;
	if (m_in_data)
		delete m_in_data;
	if (m_in_data_size)
		m_in_data = new NET_Buffer(m_in_data_size);
	else
		m_in_data = NULL;
	if (m_out_buf)
		delete[] m_out_buf;
	if (m_out_buf_size)
		m_out_buf = new char[m_out_buf_size];
	else
		m_out_buf = NULL;
	if (m_out_data)
		delete m_out_data;
	if (m_out_data_size)
		m_out_data = new NET_Buffer(m_out_data_size);
	else
		m_out_data = NULL;
	OnCreateBuffers();
}

void NET_BaseSocket::SetBuffersSize(DWORD in_one_packet, DWORD in_all_packets, DWORD out_one_packet, DWORD out_all_packets)
{
	CHECK(m_in_buf_size <= m_in_data_size && m_out_buf_size <= m_out_data_size);
	m_in_buf_size = in_one_packet;
	m_in_data_size = in_all_packets;
	m_out_buf_size = out_one_packet;
	m_out_data_size = out_all_packets;
}

void NET_BaseSocket::PrivateDestroyBuffers()
{
	OnDestroyBuffers();
	if (m_in_buf)
	{
		delete[] m_in_buf;
		m_in_buf = NULL;
	}
	if (m_in_data)
	{
		delete m_in_data;
		m_in_data = NULL;
	}
	if (m_out_buf)
	{
		delete[] m_out_buf;
		m_out_buf = NULL;
	}
	if (m_out_data)
	{
		delete m_out_data;
		m_out_data = NULL;
	}
}

UINT TerminationThreadProcedure(LPVOID param)
{
	NET_BaseSocket* socket = (NET_BaseSocket*)param;
	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, socket->GetName() + " termination by thread");
	socket->PrivateClose(true);
	return 1;
}

void NET_BaseSocket::PrivateClose(bool from_thread)
{
	if (!m_created)
		return;

	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + "::PrivateClose() started");
	{
		if (m_socket_type != NET_TYPE_SERVER)
		{
			m_read_thread.Stop();
			m_write_thread.Stop();
		}
		if (m_socket_type != NET_TYPE_CONNECTION)
			m_wait_thread.Stop();

		if (m_socket != INVALID_SOCKET)
		{
			OP2( shutdown(m_socket, SD_BOTH), m_last_error != WSAENOTCONN && m_last_error != WSANOTINITIALISED, "NET_BaseSocket::PrivateClose, shutdown" );
			OP3( closesocket(m_socket), "NET_BaseSocket::PrivateClose, closesocket" );
			m_socket = INVALID_SOCKET;
		}
		PrivateDestroyBuffers();
		if (m_created)
			m_created = false;

		switch (m_socket_type)
		{
			case NET_TYPE_CONNECTION:
				break;
			case NET_TYPE_CLIENT:
				((NET_Client*)this)->PrivateOnClose();
				OnClose();
				break;
			case NET_TYPE_SERVER:
				((NET_Server*)this)->PrivateOnClose();
				OnClose();
				break;
			default:
				OnError("NET_BaseSocket::PrivateClose, unknown socket type");
		}
	}
	if (m_socket_type == NET_TYPE_CONNECTION)
		((NET_Connection*)this)->PrivateOnClose();

	LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, GetName() + "::PrivateClose() finished");
}

void NET_BaseSocket::OnReceived()
{
	MutexWrap in_read_access(m_in_data->m_r_mutex);
	int ready_data_size;
	{
		MutexWrap in_write_access(m_in_data->m_w_mutex);
		ready_data_size = m_in_data->NoLockSpaceFor(RingBuffer_READ);
	}
	char* data = new char[ready_data_size];
	if (data == NULL)
	{
		OnError("NET_BaseSocket::OnReceived, data == NULL");
		return;
	}
	m_in_data->Read(data, ready_data_size);
	OnDataReady(data, ready_data_size);
	delete[] data;
}

void NET_BaseSocket::PrivateOnReceived()
{
	if (!m_created)
		return;
	DWORD to_read, received = 0, flags = 0, space;
	OP( ioctlsocket(m_socket, FIONREAD, &to_read), "NET_BaseSocket::PrivateOnReceived, ioctlsocket" );
	bool repeat = to_read > m_in_buf_size;
	if (repeat)
		to_read = m_in_buf_size;
	m_in_wsa_buf.buf = m_in_buf;
	m_in_wsa_buf.len = to_read;
	m_last_error = 0;
	OP2( WSARecv(m_socket, &m_in_wsa_buf, 1, &received, &flags, m_read_thread.GetOverlapped(), NULL), m_last_error != WSA_IO_PENDING, "NET_BaseSocket::PrivateOnReceived, WSARecv" );
	if (m_last_error == WSA_IO_PENDING)
	{
		LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, TXT("%s::PrivateOnReceived -- PENDING") << GetName());
		WSAResetEvent(m_read_thread.GetOverlapped()->hEvent);
		return;
	}
	{
		MutexWrap in_write_access(m_in_data->m_w_mutex);
		{
			MutexWrap in_read_access(m_in_data->m_r_mutex);
			space = m_in_data->SpaceFor(RingBuffer_WRITE);
		}
		if (space < received)
			OnError("NET_BaseSocket::PrivateOnReceived -- space < received !!!");
		else
		{
			LOG_V0_AddMessage(LOG_V0_DEBUG_MESSAGE, TXT("%s -- получено %d") << GetName() << received);
			m_in_data->Write(m_in_buf, received);
		}
	}

	OnReceived();
	if (repeat || received < to_read)
		WSASetEvent(m_read_thread.Event(0));
}

bool NET_BaseSocket::SendData(const void* data, DWORD data_size)
{
//	MutexWrap socket_access(m_socket_mutex);
	if (!m_created)
		return false;

	if (data_size > m_out_buf_size)
	{
		CString str;
		str.Format("NET_BaseSocket::SendData, Размер пакета: %d, размер буфера: %d", data_size, m_out_buf_size);
		OnError(str);
		return false;
	}
	int free_size = m_out_data->SpaceFor(RingBuffer_WRITE);
	if ((int)data_size > free_size)
	{
		OnError("NET_BaseSocket::SendData, data_size > free_size");
		return false;
	}
	{
		MutexWrap out_write_access(m_out_data->m_w_mutex);
		if (!m_out_data->Write((const char*)data, data_size))
		{
			OnError("NET_BaseSocket::SendData, !m_out_data->Write");
			return false;
		}
	}
	SignalOnPacketWriten();
	return true;
}

void NET_BaseSocket::PrivateOnSend()
{
	if (!m_created)
		return;

	MutexWrap out_read_access(m_out_data->m_r_mutex);
	int filled_size;
	{
		MutexWrap out_write_access(m_out_data->m_w_mutex);
		filled_size = m_out_data->NoLockSpaceFor(RingBuffer_READ);
	}
	if (filled_size <= 0)
		return;		
	int size_to_send = min((int)m_out_buf_size, filled_size);
	m_out_data->Read(m_out_buf, size_to_send);
	DWORD bytes_written = 0, flags = 0;
	m_out_wsa_buf.buf = m_out_buf;
	m_out_wsa_buf.len = size_to_send;
	m_last_error = 0;
	OP2( WSASend(m_socket, &m_out_wsa_buf, 1, &bytes_written, flags, m_write_thread.GetOverlapped(), NULL), m_last_error != WSA_IO_PENDING, "NET_BaseSocket::PrivateOnSend, WSASend" );
	if (m_last_error == WSA_IO_PENDING)
	{
		WSAResetEvent(m_write_thread.GetOverlapped()->hEvent);
		return;
	}
	CHECK(bytes_written == size_to_send);
	OnSend();
}

////////////////////////////////////////////////////////////////////////////////
// end