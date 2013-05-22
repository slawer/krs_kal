#pragma once

#include <afxmt.h>

struct MutexWrap
{
	MutexWrap(CMutex &mutex, const char* name = "");
	~MutexWrap();
	CMutex* m_mutex;
	CString m_name;
#ifdef _DEBUG
	bool m_debug_error;
#endif
};

void MutexWrap_Init();
void MutexWrap_ShutDown();

bool MutexWrap_LockMutex(CMutex *mutex);
void MutexWrap_UnLockMutex(CMutex *mutex);
////////////////////////////////////////////////////////////////////////////////
// end