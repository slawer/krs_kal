#include "stdafx.h"
#include "KRS.h"
#include "MainFrm.h"

#include <stdio.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
// Тест команды 2
///////////////////////////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////////////////////////
//
const char Msg1[] = "Не задано значение калибровки параметра\n\n";

BOOL TestParam1(CParam *pPrm, const char *sMsg)
{
	if(pPrm->fKalibr != 0) return true;

	CString cs = sMsg;
	cs += "\"" + pPrm->sName + "\"";
	AfxMessageBox(cs);
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
const char Msg2[] = "Не задан диаппзон параметра\n\n";

BOOL TestParam2(CParam *pPrm, const char *sMsg)
{
	if(pPrm->fDiapazon != 0) return true;

	CString cs = sMsg;
	cs += "\"" + pPrm->sName + "\"";
	AfxMessageBox(cs);
	return false;
}

WORD GetWORD(CParam *pPrm)
{
	WORD LByte, HByte;

	LByte = ((WORD)int(pPrm->fKalibrA)) & 0x00ff;	
	HByte = ((WORD)(256*int(pPrm->fKalibrK*100))) & 0xff00;
	return HByte + LByte;
}

double MinuteToOleTime(float minute)
{
	return (60.0*(minute/(24*60*60)));
}

double TicksToOleTime(DWORD nticks)
{
	double dd = 24*60*60*1000.0;
	return double(nticks)/dd;
}

float OleToSec(double OleSpan)
{
	return float(24*60*60*OleSpan);
}

CParam* GetParamA(CString sName, CParam **pTBLParam, int maxParam)
{
	if(maxParam > MAX_PARAM) return NULL;
	for(int i = 0; i < maxParam; i++)
	{
		if(sName == pTBLParam[i]->sName) return pTBLParam[i];
	}

	return NULL;
}

float GetFValueFromEdit(CWnd *wnd)
{
	if(!wnd) return 0;
	float ff;
	CString cs;
	wnd->GetWindowText(cs);
	sscanf(cs,"%f", &ff);
	return ff;
}

void SetFValueToEdit(CWnd *wnd, float ff)
{
	if(!wnd) return;
	CString cs;
	cs.Format("%.2f", ff);
	wnd->SetWindowText(cs);
}

CString GetMonth(WORD& wmonth)
{
	CString month;
	switch(wmonth)
	{
		case 1: month  = " Январь "; break;
		case 2: month  = "Февраль "; break;
		case 3: month  = "  Март  "; break;
		case 4: month  = " Апрель "; break;
		case 5: month  = "  Май   "; break;
		case 6: month  = "  Июнь  "; break;
		case 7: month  = "  Июль  "; break;
		case 8: month  = " Август "; break;
		case 9: month  = "Сентябрь"; break;
		case 10: month = "Октябрь "; break;
		case 11: month = " Ноябрь "; break;
		case 12: month = _T("Декабрь "); break;
	}

	return month;
}

CString GetStrDataTime(COleDateTime tm)
{
	CString str, str1;
	int day = tm.GetDay();
	WORD month = tm.GetMonth();
	CString monthS = GetMonth(month);
	int year = tm.GetYear();
	int hour = tm.GetHour();
	int minute = tm.GetMinute();
	int second = tm.GetSecond();
	str.Format("%d ", day);
	str += monthS;
	str1.Format(" %d %02d:%02d:%02d", year, hour, minute, second);
	str += str1;
	return str;
}
