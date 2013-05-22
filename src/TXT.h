// uic 18.04.2007

#ifndef __TXT_H_
#define __TXT_H_

struct TXT
{
	TXT(const char* initial):
		m_data(initial)
	{}

	operator CString()
	{
		return m_data;
	}

	const char* GetChars()
	{
		return m_data.GetBuffer(1);
	}

	template <typename T> TXT& operator << (const T& param)
	{
		CString format = m_data;
		int ind = format.Find('%');
		if (ind == -1)
			return *this; 
		format.SetAt(ind, '#');
		format.Replace("%", "%%");
		format.SetAt(ind, '%');
		m_data.Format(format, param);
		return *this;
	}

private:
	CString m_data;
};

#endif
////////////////////////////////////////////////////////////////////////////////
// end