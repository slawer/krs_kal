inline double MinuteToOleTime(float minute);
inline double TicksToOleTime(DWORD nticks);

float OleToSec(double OleSpan);

CParam* GetParamA(CString sName, CParam **pTBLParam, int maxParam);

float GetFValueFromEdit(CWnd *wnd);
void SetFValueToEdit(CWnd *wnd, float ff);
CString GetMonth(WORD& wmonth);
CString GetStrDataTime(COleDateTime tm);

