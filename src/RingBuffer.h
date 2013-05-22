#ifndef __RingBuffer_H
#define __RingBuffer_H

#include <MutexWrap.h>

#define RingBuffer_READ false
#define RingBuffer_WRITE true

template<class T> struct RingBuffer
{
	RingBuffer<T>(int siz):
		m_read_pos(0), m_write_pos(0), m_full(false), m_size(siz)
	{
		m_element_size = sizeof(T);
		m_storage = new T[siz];
#ifdef _DEBUG
		memset(m_storage, 0, m_element_size*m_size);
#endif
	}

	~RingBuffer<T>()
	{
		delete[] m_storage;
		m_storage = NULL;
	}

	void Clear()
	{
		MutexWrap read_access(m_r_mutex);
		MutexWrap write_access(m_w_mutex);
		m_read_pos = 0;
		m_write_pos = 0;
		m_full = false;
	}

	bool Write(const T element)
	{
		return Write(&element, 1);
	}

	bool Write(const T* elements, int num)
	{
		if (NoLockSpaceFor(RingBuffer_WRITE)<num) return false;
		if (num < 1)
			return false;
		int rest = m_size - m_write_pos;
		if (rest < num)
		{
			memcpy(&m_storage[m_write_pos], elements, m_element_size*rest);
			memcpy(m_storage, &elements[rest], m_element_size*(num-rest));
		}
		else
		{
			memcpy(&m_storage[m_write_pos], elements, m_element_size*num);
		}
		Inc(m_write_pos, num);
		m_full = (m_write_pos == m_read_pos);
		return true;
	}

	bool Read(T& element, bool shift = true)
	{
		return Read(&element, 1, shift);
	}

	bool Read(T* elements, int num, bool shift = true)
	{
		//MutexWrap read_access(m_r_mutex);
		if (NoLockSpaceFor(RingBuffer_READ)<num) return false;
		int rest = m_size - m_read_pos;
		if (rest < num)
		{
			memcpy(elements, &m_storage[m_read_pos], m_element_size*rest);
			memcpy(&elements[rest], m_storage, m_element_size*(num-rest));
#ifdef _DEBUG
			if (shift)
			{
				memset(&m_storage[m_read_pos], 0, m_element_size*rest);
				memset(m_storage, 0, m_element_size*(num-rest));
			}
#endif
		}
		else
		{
			memcpy(elements, &m_storage[m_read_pos], m_element_size*num);
#ifdef _DEBUG
			if (shift)
				memset(&m_storage[m_read_pos], 0, m_element_size*num);
#endif
		}
		if (shift)
		{
			Inc(m_read_pos, num);
			m_full = false;
		}
		return true;
	}

	bool Empty()
	{
		MutexWrap read_access(m_r_mutex);
		MutexWrap write_access(m_w_mutex);
		return (m_write_pos == m_read_pos) && !m_full;
	}

	int SpaceFor(bool for_write)
	{
		MutexWrap read_access(m_r_mutex);
		MutexWrap write_access(m_w_mutex);
		return NoLockSpaceFor(for_write);
	}

	int NoLockSpaceFor(bool for_write) const 
	{
		int from_pos = (for_write)?m_write_pos:m_read_pos;
		int to_pos = (for_write)?m_read_pos:m_write_pos;
		if (to_pos < from_pos) return m_size-(from_pos-to_pos);
		if (to_pos > from_pos) return to_pos-from_pos;
		return (for_write ^ m_full)?m_size:0;
	}

	CMutex m_w_mutex, m_r_mutex;

private:

	T* m_storage;
	int m_element_size;
	int m_size, m_read_pos, m_write_pos;
	bool m_full;

	void Inc(int&pos, int inc)
	{
		pos = (pos+inc)%m_size;
	}
};

#endif
////////////////////////////////////////////////////////////////////////////////
// end