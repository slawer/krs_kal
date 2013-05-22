// uic 5.08.2005

#ifndef __NET_H_
#define __NET_H_

#include <winsock2.h>
#include <afxmt.h>
#include <RingBuffer.h>
#include <list>
using namespace std;

struct NET_BaseSocket;

extern bool NET_shutdown;

////////////////////////////////////////////////////////////////////////////////
// +++ NET_Thread +++
////////////////////////////////////////////////////////////////////////////////
struct NET_Thread
{
	NET_Thread(NET_BaseSocket* socket = NULL);
	~NET_Thread();

	virtual CString GetName() { return "NET_Thread"; }

	void ReserveHandleSpace(DWORD num);
//	HANDLE* GetHandleAddr(DWORD num);
	HANDLE& Event(DWORD num);

//	void SetEvents(HANDLE* events, DWORD events_num);
	void SetSocket(NET_BaseSocket* socket);

	void Start(int priority = THREAD_PRIORITY_LOWEST);
	void Stop();

	friend UINT ThreadProcedure(LPVOID param);

private:
	void CloseAllHandles();

	virtual void BeforeStart() {}
	virtual void BeforeStop() {}
	virtual void AfterStop() {}
	virtual void OnEvent(DWORD event_num);
	void Execute();

//	CWinThread* m_win_thread;

	bool m_executing;
	HANDLE *m_events, m_started_event, m_thread_stopped_event;
	DWORD m_events_num;

protected:
	NET_BaseSocket* m_socket;
};

////////////////////////////////////////////////////////////////////////////////
// +++ NET_ReadThread +++
////////////////////////////////////////////////////////////////////////////////
struct NET_ReadThread: NET_Thread
{
	NET_ReadThread(NET_BaseSocket* socket = NULL);
	WSAOVERLAPPED* GetOverlapped() { return &m_overlapped; }

	virtual CString GetName() { return "NET_ReadThread"; }

private:
	virtual void OnEvent(DWORD event_num);
	virtual void BeforeStart();
	virtual void BeforeStop();
	virtual void AfterStop();

	HANDLE m_read_events[1];
	WSAOVERLAPPED m_overlapped;
};

////////////////////////////////////////////////////////////////////////////////
// +++ NET_WriteThread +++
////////////////////////////////////////////////////////////////////////////////
struct NET_WriteThread: NET_Thread
{
	NET_WriteThread(NET_BaseSocket* socket = NULL);
	WSAOVERLAPPED* GetOverlapped() { return &m_overlapped; }

	virtual CString GetName() { return "NET_WriteThread"; }
private:
	virtual void OnEvent(DWORD event_num);
	virtual void BeforeStart();
	virtual void BeforeStop();
	virtual void AfterStop();

	HANDLE m_write_events[2];
	WSAOVERLAPPED m_overlapped;
};

////////////////////////////////////////////////////////////////////////////////
// +++ NET_WaitThread +++
////////////////////////////////////////////////////////////////////////////////
#define WT_CONNECTED 0
#define WT_ACCEPTED 1
#define WT_CLOSE 2
#define WT_STOP_CONNECT 3
#define WT_START_CONNECT 4

struct NET_WaitThread: NET_Thread
{
	NET_WaitThread(NET_BaseSocket* socket = NULL);

	virtual CString GetName() { return "NET_WaitThread"; }

private:
	virtual void OnEvent(DWORD event_num);
	virtual void BeforeStart();
	virtual void BeforeStop();

	HANDLE m_wait_events[WT_START_CONNECT + 1];
};

////////////////////////////////////////////////////////////////////////////////
// +++ NET_BaseSocket +++
////////////////////////////////////////////////////////////////////////////////
#define NET_TYPE_CONNECTION 0
#define NET_TYPE_CLIENT 1
#define NET_TYPE_SERVER 2

typedef RingBuffer<char> NET_Buffer;

struct NET_BaseSocket
{
	NET_BaseSocket(byte type, byte subtype = 0);
	~NET_BaseSocket();

	virtual CString GetName() { return "NET_BaseSocket"; }

	SOCKET& GetSocket() { return m_socket; }
	int GetLastError() { return m_last_error; }

	virtual void OnCreateBuffers() {}
	virtual void OnDestroyBuffers() {}

	virtual void OnDataReady(char* data, DWORD data_size) {}
	virtual void OnReceived();
	virtual void OnSend() {}
	virtual void OnError(CString err) {}
	virtual void OnClose() {}

	void SetBuffersSize(DWORD in_one_packet, DWORD in_all_packets, DWORD out_one_packet, DWORD out_all_packets);
	DWORD GetMaxPacketSize() { return m_in_buf_size; }
	bool SendData(const void* data, DWORD data_size);

	friend NET_Thread;
	friend NET_ReadThread;
	friend NET_WriteThread;
	friend NET_WaitThread;

	friend struct NET_Connection;
	friend struct NET_Client;
	friend struct NET_Server;

	friend UINT TerminationThreadProcedure(LPVOID param);

	NET_Buffer* GetOutBuffer() { return m_out_data; }
	void SignalOnPacketWriten() { WSASetEvent(m_write_thread.Event(0)); }
	NET_Buffer* GetInBuffer() { return m_in_data; }

private:
	virtual void PrivateCreateBuffers();
	virtual void PrivateDestroyBuffers();

	void Create();
	void PrivateOnReceived();
	void PrivateOnSend();
	void PrivateClose(bool from_thread = false);

	void SelectEvent(HANDLE& event_obj, LONG net_event);

	bool m_created;
	SOCKET m_socket;

	int m_result;
	int m_last_error;

	NET_ReadThread m_read_thread;
	NET_WriteThread m_write_thread;
	NET_WaitThread m_wait_thread;

	WSABUF m_in_wsa_buf;
	DWORD m_in_buf_size, m_in_data_size;
	char* m_in_buf;
	NET_Buffer* m_in_data;

	WSABUF m_out_wsa_buf;
	DWORD m_out_buf_size, m_out_data_size;
	char* m_out_buf;
	NET_Buffer* m_out_data;

	byte m_socket_type, m_socket_subtype;

	CMutex m_socket_mutex;
};

////////////////////////////////////////////////////////////////////////////////
// +++ NET_Connection +++
////////////////////////////////////////////////////////////////////////////////
struct NET_Connection: NET_BaseSocket
{
	NET_Connection(byte sybtype = 0);
	virtual ~NET_Connection() {}

	virtual CString GetName() { return "NET_Connection"; }

	friend struct NET_Server;
	friend struct NET_BaseSocket;

	NET_Server* GetServer() { return m_server; }

private:
	void PrivateOnClose();
	void InitConnection(SOCKET& socket, NET_Server* server);

	NET_Server* m_server;
};

////////////////////////////////////////////////////////////////////////////////
// +++ NET_Client +++
////////////////////////////////////////////////////////////////////////////////
struct NET_Client: NET_BaseSocket
{
	NET_Client(byte sybtype = 0);
	virtual ~NET_Client() {}///!!!!!!!!Вылетает

	virtual CString GetName() { return "NET_Client"; }

	void StartConnect(CString to_host, WORD to_port);
	void StopConnect(bool asyncro = false);

	virtual void OnConnected() {}
	virtual void OnBreakedConnection() {}

	virtual bool IsConnected() { return m_connected; }
	virtual bool IsConnecting() { return m_connecting; }

	friend NET_WaitThread;
	friend struct NET_BaseSocket;

private:
	CString m_to_host;
	WORD m_to_port;

	void PrivateStopConnect();
	void PrivateOnClose();
	void PrivateOnConnected();
	bool PrivateConnect();

	bool CalculateSocketAddress();
	sockaddr_in m_connect_addr;
	bool m_connecting, m_connected;
};

////////////////////////////////////////////////////////////////////////////////
// +++ NET_Server +++
////////////////////////////////////////////////////////////////////////////////
typedef list<NET_Connection*> NET_Connections;

struct NET_Server: NET_BaseSocket
{
	NET_Server(byte sybtype = 0);
	virtual ~NET_Server() {}

	virtual CString GetName() { return "NET_Server"; }

	void StartListen(WORD port);
	void StopListen();

	virtual void OnConnectionAccepted(NET_Connection* connection) {}
	virtual void OnConnectionClosed(NET_Connection* connection) {}

	friend NET_WaitThread;
	friend struct NET_Connection;
	friend struct NET_BaseSocket;

	NET_Connections* GetConnections() { return &m_connections; }
	CMutex& GetConnectionsMutex() { return m_connections_mutex; }

private:
	void PrivateStopListen();
	void PrivateOnClose();
	void PrivateOnConnectionAccepted();
	void PrivateOnConnectionClosed(NET_Connection* connection);

	virtual NET_Connection* CreateNewConnection() { return new NET_Connection; }

	WORD m_listening_port;
	NET_Connections m_connections;
	CMutex m_connections_mutex;
	bool m_listening;
};

#endif
////////////////////////////////////////////////////////////////////////////////
// end