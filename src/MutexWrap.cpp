#include <StdAfx.h>
#include "./MutexWrap.h"

#ifdef _DEBUG
#ifdef MutexWrap_DEBUG_LISTS
#include <list>
using namespace std;
typedef list<CMutex*> MutexList;
MutexList *g_debug_error_mutex = NULL;
MutexList *g_debug_ok_mutex = NULL;
#endif
#endif

void MutexWrap_Init()
{
#ifdef _DEBUG
#ifdef MutexWrap_DEBUG_LISTS
	if (!g_debug_error_mutex)
		g_debug_error_mutex = new MutexList;
	if (!g_debug_ok_mutex)
		g_debug_ok_mutex = new MutexList;
#endif
#endif
}

void MutexWrap_ShutDown()
{
#ifdef _DEBUG
#ifdef MutexWrap_DEBUG_LISTS
	if (g_debug_error_mutex)
	{
		delete g_debug_error_mutex;
		g_debug_error_mutex = NULL;
	}
	if (g_debug_ok_mutex)
	{
		delete g_debug_ok_mutex;
		g_debug_ok_mutex = NULL;
	}
#endif
#endif
}

MutexWrap::MutexWrap(CMutex &mutex, const char* name):
	m_mutex(&mutex)
#ifdef _DEBUG
	, m_name(name)
#endif
{
#ifdef _DEBUG
m_debug_error = !MutexWrap_LockMutex(m_mutex); 
#else
	m_mutex->Lock();
#endif
}

MutexWrap::~MutexWrap()
{
#ifdef _DEBUG
	if (!m_debug_error)
		MutexWrap_UnLockMutex(m_mutex);
	else
	{
#ifdef MutexWrap_DEBUG_LISTS
		// убираем из списка плохих >>
		MutexList::iterator current = g_debug_error_mutex->begin();
		while (current != g_debug_error_mutex->end())
		{
			if (m_mutex == *current)
			{
				g_debug_error_mutex->erase(current); 
				break;
			}
			current++;
		}
#endif
	}
#else
	m_mutex->Unlock();
#endif
}

bool MutexWrap_LockMutex(CMutex *mutex)
{
#ifdef _DEBUG
	if (!mutex->Lock(5000))
	{
#ifdef MutexWrap_DEBUG_LISTS
		MutexList::iterator current = g_debug_error_mutex->begin();
		while (current != g_debug_error_mutex->end())
		{
			if (mutex == *current)
				return false; 
			current++;
		}
		g_debug_error_mutex->push_back(mutex); 
		//MessageBox(NULL, CString("LOCK failed: ") + name, "ERROR", MB_OK);
#endif
		return false;
	}
#ifdef MutexWrap_DEBUG_LISTS
	else
	{
		g_debug_ok_mutex->push_back(mutex); 
	}
#endif
#else
	mutex->Lock();
#endif
	return true;
}

void MutexWrap_UnLockMutex(CMutex *mutex)
{
	mutex->Unlock();
#ifdef _DEBUG
#ifdef MutexWrap_DEBUG_LISTS
	// убираем из списка хороших >>
	MutexList::iterator current = g_debug_ok_mutex->begin();
	while (current != g_debug_ok_mutex->end())
	{
		if (mutex == *current)
		{
			g_debug_ok_mutex->erase(current);
			break;
		}
		current++;
	}
#endif
#endif
}

////////////////////////////////////////////////////////////////////////////////
// end