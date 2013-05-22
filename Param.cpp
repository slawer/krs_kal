// Param.cpp : implementation file
//
#include "stdafx.h"
#include "KRS.h"
#include "MainFrm.h"
#include "KRSDoc.h"
#include "KRSView.h"
#include "Param.h"
#include "Func.h"

#include <math.h>

#include "DlgEditText.h"
#include "DlgDigitalPannelSettings.h"
#include "DlgListOrder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_EDIT_STR 100

DWORD Param_clr_min = RGB(200,230,230);
DWORD Param_clr_norm = RGB(255,255,255);
DWORD Param_clr_max = RGB(255,230,0);
DWORD Param_clr_avaria = RGB(255,100,0);

int Param_blink_counter = 0;

CString sCaseAvaria("Аварийность");
CString sCaseBlock("Блокировочное");
CString sCaseMax("Максимальное");
CString sCaseMin("Минимальное");

CParam::CParam()
{
	fCur = 0.0f;
	nMainList= 0;
	nViewList= 0;
	bMain	= false;
	bView	= true;
	sName	= "";
	sDiscript= "";
	sFile1	= "";
	sFile2	= "";
	sFile3	= "";
	sFile4	= "";
	flGraph = flControl1 = flControl2 = false;
	fDiapazon1 = 0;
	nKodControl = 0;
	fImplLebedki = saveImplLebedki = 0.0f;

	fGraphMIN = 0.0f;
	fGraphMAX = fGraphMIN + 1.0f;
	fDiapazon = fGraphMAX - fGraphMIN;
	fKalibrA = 0.0f;

	fMin = fMax = fAvaria = fBlock = 0.0f;
	nKodControl = 0;

//Решено заменить K=100.0 - > K=1.0
//	fKalibrK = 100.0f;
	fKalibrK = 1.0f;

	nViewList = 256;
	bView = true;

	sRazmernPar = "";

//--------------------------------------------------------------------------------
	sizeBuf = 60*60*10; 
	pData = NULL;
	ptmData = NULL;
	sizeAverage = 300;


	timeAvrg = 0;

	lastTimeAverage = 0;

	flagLastAverage = false;

	flPodacha = false;

	prevNumTimeInterval = 1;
	prevTimeInterval = 0;
	prevValue = 0.0f;
	prevTime = 0.0f;


	m_num_par = 0;
	bParamActive = true;
	m_channel_num = -1;
	m_channel_conv_num = 0;
	m_attr_channel_num = -1;
	m_attr_channel_conv_num = 0;
	m_nTypePar = PARAM_TYPE_CHANNEL;
	m_formula = NULL;
	m_shown_digits = 3;

	flDB = 1;

	fCurPrevious = 0;
	fCurAvgPrevious = 0;
	fCurAvg = 0;
	fCurSave = 0;
	pBufParam = NULL;
	m_last_good_val = 0;

	m_color = 0;
	m_line_width = 2;

	m_calc_status = FORMULA_CALC_STATUS_TIMEOUT;
    m_prev_calc_status = FORMULA_CALC_STATUS_TIMEOUT;
    m_always_not_timed_out = false;
    m_timed_out_db = true;

	numAlert = 0;

	m_db_time_interval = 1.0/(24*3600);
	m_db_data_interval = 1;
	m_last_time_send_to_db = 0;
	m_send_to_db_status = PARAM_DB_STATUS_TO;
	m_last_value_send_to_db = 0;
}

CParam::CParam(CString ValName, float Val, float ValMin, float ValMax, 
				float ValBlock, float ValAvaria)
{
	fCur		= 0;
	fMin		= ValMin;
	fMax		= ValMax;
	fBlock	= ValBlock;
	fAvaria	= ValAvaria;
	nMainList= 0;
	nViewList= 0;
	bMain	= false;
	bView	= true;
	sName	= ValName;
	sDiscript= "";
	sFile1	= "";
	sFile2	= "";
	sFile3	= "";
	sFile4	= "";
	flGraph = flControl1 = flControl2 = false;
	fImplLebedki = saveImplLebedki = 0.0f;
	fKalibrA = 0.0f;
	fKalibrK = 1.0f;
	flPodacha = false;

	prevNumTimeInterval = 1;
	prevTimeInterval = 0;
	prevValue = 0.0f;
	prevTime = 0.0f;
	m_num_par = 0;
//	m_num_par_local = 0;
	bParamActive = true;
	m_channel_num = -1;
	m_channel_conv_num = 0;
	m_attr_channel_num = -1;
	m_attr_channel_conv_num = 0;
	m_nTypePar = PARAM_TYPE_CHANNEL;
	m_formula = NULL;
	m_shown_digits = 3;

	flDB = 1;

	fCurPrevious = 0;
	fCurAvgPrevious = 0;
	fCurAvg = 0;
	fCurSave = 0;
	pBufParam = NULL;
	m_last_good_val = 0;

	m_color = 0;
	m_line_width = 2;

    m_calc_status = FORMULA_CALC_STATUS_TIMEOUT;
    m_prev_calc_status = FORMULA_CALC_STATUS_TIMEOUT;
    m_always_not_timed_out = false;

    m_timed_out_db = true;

	numAlert = 0;

	m_db_time_interval = 1.0/(24*3600);
	m_db_data_interval = 1;
	m_last_time_send_to_db = 0;
	m_send_to_db_status = PARAM_DB_STATUS_TO;
	m_last_value_send_to_db = 0;
}

CParam::CParam(CParam& prm)
{
	nPar		= prm.nPar;

	fCur		= prm.fCur;
	fBlock		= prm.fBlock;
	fAvaria		= prm.fAvaria;
	fDiapazon1	= prm.fDiapazon1;
	fDiapazon	= prm.fDiapazon;
	fGraphMIN	= prm.fGraphMIN;
	fGraphMAX	= prm.fGraphMAX;
	fMin		= prm.fMin;
	fMax		= prm.fMax;
	
	nMainList	= prm.nMainList;
	nViewList	= prm.nViewList;

	bMain		= prm.bMain;
	bView		= prm.bView;

	flGraph		= prm.flGraph;
	flControl1	= prm.flControl1;
	flControl2	= prm.flControl2;
	
	fKalibr		= prm.fKalibr;
	fKalibrA	= prm.fKalibrA;
	fKalibrK	= prm.fKalibrK;
	
	nKodControl = prm.nKodControl;

	nViewList	= prm.nViewList;

	sName		= prm.sName;
	sGraphName	= prm.sGraphName;
	sDiscript	= prm.sDiscript;
	sFile1		= prm.sFile1;
	sFile2		= prm.sFile2;
	sFile3		= prm.sFile3;
	sFile4		= prm.sFile4;
	sRazmernPar = prm.sRazmernPar;

	
	sizeBuf		= prm.sizeBuf;
	pData = NULL;
	ptmData = NULL;
	indexWirite		= prm.indexWirite;
	indexRead		= prm.indexRead;
	sizeAverage		= prm.sizeAverage;
	timeAvrg		= prm.timeAvrg;
	lastTimeAverage = prm.lastTimeAverage;
	flagLastAverage = false;

	flPodacha		= prm.flPodacha;

	prevNumTimeInterval = prm.prevNumTimeInterval;
	prevTimeInterval = prm.prevTimeInterval;
	prevValue = prm.prevValue;
	prevTime = prm.prevTime;

	m_num_par = prm.m_num_par;
//	m_num_par_local = prm.m_num_par_local;
	bParamActive = prm.bParamActive;
	m_channel_num = prm.m_channel_num;
	m_channel_conv_num = prm.m_channel_conv_num;
	m_attr_channel_num = prm.m_attr_channel_num;
	m_attr_channel_conv_num = prm.m_attr_channel_conv_num;
	m_nTypePar = prm.m_nTypePar;
	m_formula = NULL;//prm.m_formula;
	m_text_formula_for_registry = prm.m_text_formula_for_registry;
	m_shown_digits = prm.m_shown_digits;

	flDB = prm.flDB;
	fCurPrevious = prm.fCurPrevious;
	fCurAvgPrevious = prm.fCurAvgPrevious;
	fCurAvg = prm.fCurAvg;
	fCurSave = prm.fCurSave;
	*pBufParam = *(prm.pBufParam);
	m_last_good_val = prm.m_last_good_val;

	m_color = prm.m_color;
	m_line_width = prm.m_line_width;

	m_calc_status = prm.m_calc_status;
    m_prev_calc_status = prm.m_prev_calc_status;
    m_always_not_timed_out = prm.m_always_not_timed_out;
    m_timed_out_db = prm.m_timed_out_db;

	numAlert = prm.numAlert;

	m_db_time_interval = prm.m_db_time_interval;
	m_db_data_interval = prm.m_db_data_interval;
	m_last_time_send_to_db = prm.m_last_time_send_to_db;
	m_send_to_db_status = prm.m_send_to_db_status;
	m_last_value_send_to_db = prm.m_last_value_send_to_db;
}

CParam::CParam(CParam* prm)
{
//	gGraph		= prm->gGraph;
	nPar		= prm->nPar;

	fCur		= prm->fCur;
	fBlock		= prm->fBlock;
	fAvaria		= prm->fAvaria;
	fDiapazon1	= prm->fDiapazon1;
	fDiapazon	= prm->fDiapazon;
	fGraphMIN	= prm->fGraphMIN;
	fGraphMAX	= prm->fGraphMAX;
	fMin		= prm->fMin;
	fMax		= prm->fMax;
	
	nMainList	= prm->nMainList;
	nViewList	= prm->nViewList;

	bMain		= prm->bMain;
	bView		= prm->bView;

	flGraph		= prm->flGraph;
	flControl1	= prm->flControl1;
	flControl2	= prm->flControl2;
	
	fKalibr		= prm->fKalibr;
	fKalibrA	= prm->fKalibrA;
	fKalibrK	= prm->fKalibrK;
	
	nKodControl = prm->nKodControl;

	nViewList	= prm->nViewList;

	sName		= prm->sName;
	sGraphName	= prm->sGraphName;
	sDiscript	= prm->sDiscript;
	sFile1		= prm->sFile1;
	sFile2		= prm->sFile2;
	sFile3		= prm->sFile3;
	sFile4		= prm->sFile4;
	sRazmernPar = prm->sRazmernPar;

	
	sizeBuf		= prm->sizeBuf;
	pData = NULL;
	ptmData = NULL;
	indexWirite		= prm->indexWirite;
	indexRead		= prm->indexRead;
	sizeAverage		= prm->sizeAverage;
	timeAvrg		= prm->timeAvrg;
	lastTimeAverage = prm->lastTimeAverage;
	flagLastAverage = false;

	flPodacha		= prm->flPodacha;

	prevNumTimeInterval = prm->prevNumTimeInterval;
	prevTimeInterval = prm->prevTimeInterval;
	prevValue = prm->prevValue;
	prevTime = prm->prevTime;

	m_num_par = prm->m_num_par;
//	m_num_par_local = prm->m_num_par_local;
	bParamActive = prm->bParamActive;
	m_channel_num = prm->m_channel_num;
	m_channel_conv_num = prm->m_channel_conv_num;
	m_attr_channel_num = prm->m_attr_channel_num;
	m_attr_channel_conv_num = prm->m_attr_channel_conv_num;
	m_nTypePar = prm->m_nTypePar;
	m_formula = NULL;//prm->m_formula;
	m_text_formula_for_registry = prm->m_text_formula_for_registry;
	m_shown_digits = prm->m_shown_digits;

	flDB = prm->flDB;

	fCurPrevious = prm->fCurPrevious;
	fCurAvgPrevious = prm->fCurAvgPrevious;
	fCurAvg = prm->fCurAvg;
	fCurSave = prm->fCurSave;
	*pBufParam = *(prm->pBufParam);
	m_last_good_val = prm->m_last_good_val;

	m_color = prm->m_color;
	m_line_width = prm->m_line_width;

	m_calc_status = prm->m_calc_status;
    m_prev_calc_status = prm->m_prev_calc_status;
    m_always_not_timed_out = prm->m_always_not_timed_out;
    m_timed_out_db = prm->m_timed_out_db;

	numAlert = prm->numAlert;

	m_db_time_interval = prm->m_db_time_interval;
	m_db_data_interval = prm->m_db_data_interval;
	m_last_time_send_to_db = prm->m_last_time_send_to_db;
	m_send_to_db_status = prm->m_send_to_db_status;
	m_last_value_send_to_db = prm->m_last_value_send_to_db;
}

CParam& CParam::operator=(CParam& prm)
{
//	gGraph		= prm.gGraph;

	nPar		= prm.nPar;

	fCur		= prm.fCur;
	fBlock		= prm.fBlock;
	fAvaria		= prm.fAvaria;
	fDiapazon1	= prm.fDiapazon1;
	fDiapazon	= prm.fDiapazon;
	fGraphMIN	= prm.fGraphMIN;
	fGraphMAX	= prm.fGraphMAX;
	fMin		= prm.fMin;
	fMax		= prm.fMax;
	
	nMainList	= prm.nMainList;
	nViewList	= prm.nViewList;

	bMain		= prm.bMain;
	bView		= prm.bView;

	flGraph		= prm.flGraph;
	flControl1	= prm.flControl1;
	flControl2	= prm.flControl2;
	
	fKalibr		= prm.fKalibr;
	fKalibrA	= prm.fKalibrA;
	fKalibrK	= prm.fKalibrK;
	
	nKodControl = prm.nKodControl;

	nViewList	= prm.nViewList;

	sName		= prm.sName;
	sGraphName	= prm.sGraphName;
	sDiscript	= prm.sDiscript;
	sFile1		= prm.sFile1;
	sFile2		= prm.sFile2;
	sFile3		= prm.sFile3;
	sFile4		= prm.sFile4;
	sRazmernPar = prm.sRazmernPar;

	
	sizeBuf		= prm.sizeBuf;
	pData = NULL;
	ptmData = NULL;
	indexWirite		= prm.indexWirite;
	indexRead		= prm.indexRead;
	sizeAverage		= prm.sizeAverage;
	timeAvrg		= prm.timeAvrg;
	lastTimeAverage = prm.lastTimeAverage;
	flagLastAverage = false;

	flPodacha		= prm.flPodacha;

	prevNumTimeInterval = prm.prevNumTimeInterval;
	prevTimeInterval = prm.prevTimeInterval;
	prevValue = prm.prevValue;
	prevTime = prm.prevTime;

	m_num_par = prm.m_num_par;
//	m_num_par_local = prm.m_num_par_local;
	bParamActive = prm.bParamActive;
	m_channel_num = prm.m_channel_num;
	m_channel_conv_num = prm.m_channel_conv_num;
	m_attr_channel_num = prm.m_attr_channel_num;
	m_attr_channel_conv_num = prm.m_attr_channel_conv_num;
	m_nTypePar = prm.m_nTypePar;
	m_formula = NULL;
	m_text_formula_for_registry = prm.m_text_formula_for_registry;
	m_shown_digits = prm.m_shown_digits;

	flDB = prm.flDB;

	fCurPrevious = prm.fCurPrevious;
	fCurAvgPrevious = prm.fCurAvgPrevious;
	fCurAvg = prm.fCurAvg;
	fCurSave = prm.fCurSave;
	if(prm.pBufParam != NULL) *pBufParam = *(prm.pBufParam);
	m_last_good_val = prm.m_last_good_val;

	m_color = prm.m_color;
	m_line_width = prm.m_line_width;

	m_calc_status = prm.m_calc_status;
    m_prev_calc_status = prm.m_prev_calc_status;
    m_always_not_timed_out = prm.m_always_not_timed_out;
    m_timed_out_db = prm.m_timed_out_db;

	numAlert = prm.numAlert;

	m_db_time_interval = prm.m_db_time_interval;
	m_db_data_interval = prm.m_db_data_interval;
	m_last_time_send_to_db = prm.m_last_time_send_to_db;
	m_send_to_db_status = prm.m_send_to_db_status;
	m_last_value_send_to_db = prm.m_last_value_send_to_db;

	return *this;
}

CParam::~CParam()
{
	if (pData)
	{
		delete pData; 
		pData = NULL;
	}
	if (ptmData)
	{
		delete ptmData; 
		ptmData = NULL;
	}
	if(pBufParam != NULL)
	{
		delete pBufParam; 
		pBufParam = NULL;
	}
	if (m_formula)
		delete m_formula;
}

////////////////////////////////////////////////////////////////////////////////

void CParam::Init(int base_pos)
{
	if(pData == NULL) 
		pData = new RBuf<float>(sizeBuf);

	if(ptmData == NULL) 
		ptmData = new RBuf<F>(sizeBuf);

	if(pBufParam == NULL) 
	{
		pBufParam = new FRingBuf<F>(MAX_SIIZE_BUF, 0, 0, 0, NULL);
		pBufParam->AddFragment(base_pos);
//		pBufParam->InitBufAvg(50, true);
	}
}

void CParam::StartTimeOut()
{
    if (CanBeTimedOut())
        m_calc_status = FORMULA_CALC_STATUS_TIMEOUT;
}

bool CParam::CanBeTimedOut()
{
	return !KRS_running || !m_always_not_timed_out;
}

void CParam::FinishTimeOut(int pos)
{
	if (m_prev_calc_status == FORMULA_CALC_STATUS_TIMEOUT)
	{
		if (pBufParam)
			pBufParam->AddFragment(pos, 0, true);
	}
}

bool CParam::IsTimedOut()
{
    if (!CanBeTimedOut())
        return false;

    if (m_calc_status == FORMULA_CALC_STATUS_NOT_READY)
        GetCurrentValue(m_calc_status);

    return (m_calc_status != FORMULA_CALC_STATUS_COMPLETE);
}

void CParam::StartTimeOutBD()
{
    m_timed_out_db = true;
}

void CParam::FinishTimeOutBD(int pos)
{
    if (m_timed_out_db == true)
    {
		if (m_num_par == 82)
			int zzz = 0;
        if (pBufParam)
            pBufParam->AddFragment(pos, 0, false);
        m_timed_out_db = false;
    }
}

void CParam::SetScaleTime(LPVOID pBufX, 
		double m_ScaleXStart, double m_ScaleXEnd, BOOL m_flagTimeGlub)
{
}

void CParam::DestroyParam()
{
}

float CParam::GetChangeSpeed(double curTime, float prevSpeed)
{
	float res;
	double dT = (curTime - prevTime)*24.0f;
	if (fCur != prevValue)
	{
		if (pData->GetSizeData() > 1 && dT > 0.0f) res = (fCur - prevValue)/(float)dT;
		else res = 0.0f;
		prevValue = fCur;
		prevTime = curTime;
		prevTimeInterval = dT;
		prevNumTimeInterval = 1;
	}
	else
	{
		if (pData->GetSizeData() > 1)
		{
			if (dT > prevTimeInterval*prevNumTimeInterval) 
			{
				if (prevNumTimeInterval > 0) res = prevSpeed/prevNumTimeInterval;
				else res = 0.0f;
				prevNumTimeInterval++;
			}
			else res = prevSpeed;
		}
		else res = 0.0f;
	}
	return res;
}

int CParam::AddTail(double dTime)
{

	if(pData == NULL) 
		pData = new RBuf<float>(sizeBuf);
	if(ptmData == NULL) 
		ptmData = new RBuf<F>(sizeBuf);
	
	fCurPrevious = fCur;

	*pData << fCur;
	*ptmData << dTime;

	flagLastAverage = false;

	lastTime = dTime;
	
	return pData->GetSizeData();
}

void CParam::SaveDOL()
{
	saveImplLebedki = fImplLebedki;
}

float CParam::GetIndexValue(double& dtm, int nShift)
{
	if(pData == NULL || ptmData == NULL) return 0;
	int nn;

	if((nn = pData->GetSizeData() - nShift) < 0) return 0;

	dtm = (*ptmData)[nn];
	return (*pData)[nn];
}


int CParam::GetIndexDTime(double dTime)
{
	double dTm, dTm0;
	int index;

	index = 0;
	GetIndexValue(dTm0, index++);

	do GetIndexValue(dTm, index++);
	while(dTm0 - dTm <= dTime && index <= pData->GetSizeData());
		
	return index - 1;
}

float CParam::GetAvrgShift(int nShift)
{
	float sum, fV;
	int num;
	double dtm, dtm0;

	if(pData->GetSizeData() == 0) return 0;

	sum = GetIndexValue(dtm0, nShift);
    double msec = 1e-3/(24*3600);
    if (timeAvrg < msec)
        return sum;
    num = 1;
    //dtm = dtm0;
    while(nShift + num <= pData->GetSizeData())
    {
	    fV = GetIndexValue(dtm, nShift + num);
        if (dtm0 - dtm > timeAvrg + msec)
            break;
	    sum += fV;
        num++;
    }
	//while(dtm0 - dtm < timeAvrg && nShift + num <= sizeData);

	return sum/num;
}

float CParam::GetAvrgPrevious(double dTime)
{
	int indx = GetIndexDTime(dTime);

	previousAvrg = GetAvrgShift(indx);
	return previousAvrg;
}

float CParam::GetAvrg()
{
	float avrg;

	if(flagLastAverage) avrg = lastAvrg;
	else
	{
		avrg = GetAvrgShift(0);
		
		previousAvrg = lastAvrg;
		lastAvrg = avrg;

		flagLastAverage = true;
	}

	return avrg;
}

void CParam::SaveAvrg()
{
	fCurAvgPrevious = fCurAvg;
}

float CParam::GetAvrgInterval(int dTime)
{
	double sav = timeAvrg;
	timeAvrg = MinuteToOleTime(dTime/60.0f);

	float ff = GetAvrgShift(0);
	
	timeAvrg = sav; 

	return ff;
}

float CParam::GetAvrgP()
{
	previousAvrg = GetAvrgPrevious(timeAvrg);
	return previousAvrg;
}

double CParam::GetLastTime()
{
	double dtm;
	
	if(pData->GetSizeData() > 0) GetIndexValue(dtm, 1);
	else dtm = 0.0;

	return dtm;
}

float CParam::GetDiff()
{
	float fV;
	double dtm;

	if(pData->GetSizeData() > 1) fV = GetIndexValue(dtm, 2);
	else fV = fCur;

	return fCur - fV;
}

float CParam::GetDrv(float dTime)
{
	double drv, dTm, dTm0;
	float fV, fCur0;

	if(pData->GetSizeData() < 2) return 0.0f;

	if(dTime == 0.0f) 
	{
		fV = GetIndexValue(dTm, 2);
		fCur0 = GetIndexValue(dTm0, 1);
		dTm = dTm0 - dTm;
	}
	else
	{
		dTm = dTime/(24*60); 
		fV = GetIndexValue(dTm, GetIndexDTime(dTm));
		fCur0 = GetIndexValue(dTm0, 1);
		dTm = dTm0 - dTm;
	}

	dTm *= (24*60); 

	if(dTm < 1.0f/600) return 0.0f;  

	drv = (fCur0 - fV)/dTm;

	return float(drv);
}

float CParam::GetDiffValue(float dTime)
{
	double dTm, dTm0;
	float fV, fCur0;

	if(pData->GetSizeData() < 2) return 0.0f;

	if(dTime == 0.0f) 
	{
		fV = GetIndexValue(dTm, 2);
		fCur0 = GetIndexValue(dTm0, 1);
		dTm = dTm0 - dTm;
	}
	else
	{
		dTm = dTime/(24*60); 
		fV = GetIndexValue(dTm, GetIndexDTime(dTm));
		fCur0 = GetIndexValue(dTm0, 1);
		dTm = dTm0 - dTm;
	}

	dTm *= (24*60); 

	if(dTm < 1.0f/600) return 0.0f;  


	return (fCur0 - fV);
}

float CParam::GetDfDOL()
{
	if(pData->GetSizeData() > 1) return fImplLebedki - saveImplLebedki;
	else return 0.0f;
}
float CParam::GetDfPar()
{
	float ff;
	if(lastAvrg != 0) ff = (float)fabs(1.0 - previousAvrg/lastAvrg);
	else ff = 0.0f;

	return ff;
}
float CParam::GetDTime()
{
	double dTm, dTm0;
	
	if(pData->GetSizeData() < 4) return 0.0f;

	GetIndexValue(dTm0, 0);
	GetIndexValue(dTm,  1);

	if(dTm0 == 0 || dTm == 0) return 0;
	return float((dTm0 - dTm)*24);
}
float CParam::GetSPO()
{
	double dTm, dTm0;
	
	if(pData->GetSizeData() < 2) return 0.0f;

	GetIndexValue(dTm0, 1);
	GetIndexValue(dTm,  2);

	dTm = (dTm0 - dTm)*24*60*60;
	return float(fKalibr*GetDfDOL()/(dTm*100));
}

int CParam::AddData(float ff)
{
	*pData << ff;
	return pData->GetSizeData();
}

int CParam::GetSizeData()
{
	return pData->GetSizeData();
}

void CParam::ResetIndex()
{
	pData->Clear0();
	ptmData->Clear0();
}

CString CParam::GetNameForShow()
{
	CString res;
	res.Format("%s (%d)", sName, m_num_par);
	return res;
}

CString CParam::TestCase(int nCase, float fValue)
{
	switch(nCase)
	{
		case 0:
			if((nKodControl == 3 || nKodControl == 5) && fValue >= fAvaria) return sCaseAvaria;
		break;

		case 1:
//			if((nKodControl == 2 || nKodControl == 5) && fValue <= fBlock) return sCaseBlock;
		break;

		case 2:
			if(fValue >= fMax) return sCaseMax;
		break;

		case 3:
			if((nKodControl == 1 || nKodControl == 5) && fValue <= fMin) return sCaseMin;
		break;

		case 4:
			if((nKodControl == 3 || nKodControl == 5) && fValue >= fAvaria) return sCaseAvaria;
//			else if((nKodControl == 2 || nKodControl == 5) && fValue <= fBlock) return sCaseBlock;
			else if(fValue >= fMax) return sCaseMax;
			else if((nKodControl == 1 || nKodControl == 5) && fValue <= fMin) return sCaseMin;
		break;
	}
	return "";
}
void CParam::SetAlertLine()
{
}

/////////////////////////////////////////////////////////////////////////////
// CParam

IMPLEMENT_SERIAL(CStrPrn, CWnd, 0)

BEGIN_MESSAGE_MAP(CStrPrn, CWnd)
	//{{AFX_MSG_MAP(CStrPrn)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

CStrPrn::CStrPrn()
{
	strName				= "";
	strText				= "";
	numStr				= 0;	
	kInterval			= 1;	
	intervalNameText	= 5;	
	iOtstup				= 1;	
	iBold				= 400;  
	bItalic				= false;
	bUnderline			= false;
	sizeFnt				= 11;
}


CStrPrn::CStrPrn(CString sName, CString sText, int nStr, float kIntrv, int intrvNameText, int iOtst, int iBld,
			BOOL bItal, BOOL bUnderl, int szFnt)
{
	strName				= sName;
	strText				= sText;
	numStr				= nStr;
	kInterval			= kIntrv; 
	intervalNameText	= intrvNameText; 
	iOtstup				= iOtst; 
	iBold				= iBld;  
	bItalic				= bItal;
	bUnderline			= bUnderl; 
	sizeFnt				= szFnt;

	LOGFONT  lfntN = {
		-sizeFnt, 0, 0, 0, iBold, bItalic, bUnderline, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};
	
	LOGFONT lfntT = {
		-sizeFnt, 0, 0, 0, iBold, bItalic, bUnderline, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};


	m_lfntN = lfntN;
	m_lfntT = lfntT;

	if(fFontName.m_hObject) fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&m_lfntN);
	if(fFontText.m_hObject) fFontText.DeleteObject();
	fFontText.CreateFontIndirect(&m_lfntT);
}


CStrPrn::~CStrPrn()
{
	fFontName.DeleteObject();
	fFontText.DeleteObject();
}

CStrPrn& CStrPrn::operator=(CStrPrn& strP)
{
	strName				= strP.strName;
	strText				= strP.strText;
	numStr				= strP.numStr;
	kInterval			= strP.kInterval;
	intervalNameText	= strP.intervalNameText;
	iOtstup				= strP.iOtstup;
	iBold				= strP.iBold;
	bItalic				= strP.bItalic;
	bUnderline			= strP.bUnderline;
	sizeFnt				= strP.sizeFnt;

	LOGFONT  lfntN = {
		-sizeFnt, 0, 0, 0, iBold, bItalic, bUnderline, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	LOGFONT lfntT = {
		-sizeFnt, 0, 0, 0, iBold, bItalic, bUnderline, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};


	m_lfntN = lfntN;
	m_lfntT = lfntT;

	if(fFontName.m_hObject) fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&m_lfntN);
	if(fFontText.m_hObject) fFontText.DeleteObject();
	fFontText.CreateFontIndirect(&m_lfntT);

	return *this;
}

void CStrPrn::setParam2T(int iBld, BOOL bItal, BOOL bUnderl, int szFnt)
{
	fFontText.DeleteObject();

	LOGFONT lfntT = {
		-szFnt, 0, 0, 0, iBld, bItal, bUnderl, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	fFontText.CreateFontIndirect(&m_lfntT);
}

void CStrPrn::SetStrText(CString str)
{
	strText = str;
}

void CStrPrn::GetStrText(CString& str, int& otst)
{
	CString cs = "000";
	strText = str;
}

int CStrPrn::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_cEdit.Create(WS_OVERLAPPED | ES_LEFT | WS_TABSTOP | ES_AUTOHSCROLL, CRect(0,0,2,2), this, ID_EDIT_STR);
	m_cEdit.SetFont(&fFontText);
	
	return 0;
}

void CStrPrn::CreatePrnStr(CWnd *pWnd)
{
	m_cEdit.Create(WS_OVERLAPPED | ES_LEFT | WS_TABSTOP | ES_AUTOHSCROLL, CRect(0,0,2,2), pWnd, ID_EDIT_STR);
	m_cEdit.SetFont(&fFontText);

}

void CStrPrn::SetPosPrnStr(CWnd *pWnd, CDC* pDC)
{
	CString cs = strName;
	cs += "000";

	CSize sizeText = pDC->GetTextExtent(cs);
	CRect rec, recEdt, recName;
	pWnd->GetClientRect(&rec);

	recEdt.top = numStr*sizeText.cy;
	recEdt.bottom = recEdt.top + sizeText.cy;
	recEdt.left =  sizeText.cx;
	recEdt.right = sizeText.cx + rec.Width()/2;

	m_cEdit.SetWindowPos(&CWnd::wndTop, recEdt.left, recEdt.top, recEdt.Width(), recEdt.Height(), SWP_SHOWWINDOW);

	recName.top = numStr*sizeText.cy;
	recName.bottom = recEdt.top + sizeText.cy;
	recName.left =  0;
	recName.right = sizeText.cx;

	ClientToScreen(&recName);
	pWnd->ScreenToClient(&recName);

	SetWindowPos(&CWnd::wndTop, recName.left, recName.top, recName.Width(), recName.Height(), SWP_SHOWWINDOW);
	

	CRect rec2;
	GetClientRect(&rec2);

	ClientToScreen(&rec2);
	pWnd->ScreenToClient(&rec2);
	
	rec2.NormalizeRect();
	pDC->FillSolidRect(&rec2, RGB(255,0,0));
}

CBurDevice::CBurDevice()
{
}

CBurDevice::CBurDevice(CString ValName, float Val, float ValMin, float ValMax, 
				float ValBlock, float ValAvaria)
{
	fVal		= Val;
	fValMin		= ValMin;
	fValMax		= ValMax;
	fValBlock	= ValBlock;
	fValAvaria	= ValAvaria;
	nValMainList= 0;
	nValViewList= 0;
	bValMain	= false;
	bValView	= true;
	sValName	= ValName;
	sValDiscript= "";
	sValFile1	= "";
	sValFile2	= "";
}


CBurDevice::~CBurDevice()
{
}
CPanelWnd::CPanelWnd()
{
	m_flag_visible_field = false;
	m_pParentForGraph = NULL;
	m_pParentForPanel = NULL;
	m_pBufX = NULL;
	num_panel = 0;
	type_panel = true;
	m_ScaleXStart = 0;
	m_deltaX = 0;
	m_flagTimeGlub = true;
	sNamePanel = "Параметры";
	m_flag_size = true;
	t_view = t_real_time;
    m_flag_orient = true;
}

BEGIN_MESSAGE_MAP(CPanelWnd, CStatic)
	//{{AFX_MSG_MAP(CPanelWnd)
	ON_MESSAGE(PM_DATA_CHANGED, OnDataChanged)
	ON_MESSAGE(PM_DATA_DB_CHANGED, OnDataDBChanged)
	ON_MESSAGE(PM_CHANGE_PARAM_PROPERT, OnChangeParamProperty)
	ON_MESSAGE(PM_ADD_PARAM, OnAddParam)
	ON_MESSAGE(PM_STYLE_GRF_CHANGED, OnChangeStyleGrf)
//	ON_MESSAGE(PM_DELETE_PARAM, OnDeleteParam)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(PM_R_BUTTON_DOWN, OnRButtonDown1)
    ON_MESSAGE(PM_SET_STYLE, OnSetStyleGrf)
	ON_MESSAGE(PM_RESET_STYLE, OnResetStyleGrf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0, SS_NOTIFY);
//--------------------------------------------------------------------
	LOGFONT  logfontN = {
		-11, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
		OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
	};

	m_logfontN = logfontN;

	if(fFontName.m_hObject) fFontName.DeleteObject();
	fFontName.CreateFontIndirect(&m_logfontN);
//--------------------------------------------------------------------
	
	if(m_pParentForGraph == NULL) 
	{
		type_panel = true;

		CClientDC dc(this);
		
		CFont *pFont = dc.SelectObject(&fFontName);
		ASSERT(pFont);		
		CSize sz = dc.GetTextExtent("P");
		dc.SelectObject(pFont);

		m_splitH = 3*sz.cy/2;

		return 0;
	}
	
	CRect recGraph(0,0,30,30);
	if(t_view == t_real_time || t_view == gt_cement) m_parGraph = CGraph("", "", 31, 6, 0x009C01BC60040401);
	else m_parGraph = CGraph("", "", 31, 6, 0x009C01BC60040405);

	m_parGraph.num_self = num_panel;
	
	if(m_parGraph.m_hWnd) 
	{
		m_parGraph.DestroyWindow();
		m_parGraph.DestroyBuf();
	}

	m_parGraph.Create(NULL, WS_CHILD, recGraph, m_pParentForGraph);
	m_parGraph.SetMinMaxT(0, 1);
	m_parGraph.SetMinMaxF(0, 1);
	m_parGraph.SetAdrBufX(0, *((FRingBuf<double>*)m_pBufX));
	m_parGraph.SetTypeGraph(1);
    m_parGraph.SetRot90(m_flag_orient);
	type_panel = false;
	return 0;
}

void CPanelWnd::OnDestroy() 
{
	CStatic::OnDestroy();
	
	fFontName.DeleteObject();
	if(m_parGraph.m_hWnd) m_parGraph.DestroyBuf();
}

void CPanelWnd::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	RecalcFieldSize();
}

void CPanelWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rec;
	GetClientRect(&rec);

//--------------------------------------------------------------------------------
	COLORREF clr = GetSysColor(COLOR_MENU);
	clr = RGB(CR(clr) - 10, CG(clr) - 10, CB(clr) - 10);
	if (type_panel)
	{
		dc.FillSolidRect(&rec, g_color_interface.color_panel_dgt_bk);
	
		CSize sz = dc.GetTextExtent("P");
		CRect recName(CPoint(rec.left, rec.top), CSize(rec.Width(), sz.cy));

		CFont* pFont = dc.SelectObject(&fFontName);
		ASSERT(pFont);
		dc.SetBkColor(clr);
		dc.ExtTextOut(rec.left + 10, rec.top + 1, ETO_CLIPPED|ETO_OPAQUE,
			recName, sNamePanel, NULL);

		dc.SelectObject(pFont);

		rect_header	= CRect(rec.left, rec.top + 1, rec.right, rec.top + 16);
		rect_add	= CRect(rec.right - 14 - 1, rec.top + 1, rec.right, rec.top + 15);

		int x = rec.right - 15 - 1 - 14 - 1, y = rec.top + 1;

		if (m_ParamArray.GetSize() == 0)
			m_flag_size = true;
		HICON icon = KRS_app.LoadIcon(m_flag_size ? ((m_ParamArray.GetSize() == 0) ? IDI_ICON_UP2 : IDI_ICON_UP):IDI_ICON_DOWN);
		DrawIcon(dc.m_hDC, x, y, icon);
		DrawIcon(dc.m_hDC, x + 15 + 1, y, KRS_app.LoadIcon(IDI_ICON_ADD_PARAM));
	}
	else dc.FillSolidRect(&rec, g_color_interface.color_panel_grf_bk);
//--------------------------------------------------------------------------------
}

LRESULT CPanelWnd::OnRButtonDown1(WPARAM wParam, LPARAM lParam)
{
	return true;
}
LRESULT CPanelWnd::OnSetStyleGrf(WPARAM wParam, LPARAM lParam)
{
    ULONG64 style = (ULONG64(wParam)<<32)|ULONG64(lParam);
    if(style == GRF_ORIENT_FLD) m_parGraph.SetRot90(true);
	else m_parGraph.SetStyle(style);
    return true;
}
LRESULT CPanelWnd::OnResetStyleGrf(WPARAM wParam, LPARAM lParam)
{
	ULONG64 style = (ULONG64(wParam)<<32)|ULONG64(lParam);
	if(style == GRF_ORIENT_FLD) m_parGraph.SetRot90(false);
	else m_parGraph.ResetStyle(style);
	return true;
}
ULONG64 CPanelWnd::GetStyleGrf()
{
	return m_parGraph.GetStyle();
}
ULONG64 CPanelWnd::GetBitStyle(ULONG64 g_style)
{
	return m_parGraph.GetBitStyle(g_style);
}
LRESULT CPanelWnd::OnDataChanged(WPARAM wParam, LPARAM lParam)
{
	ParView *pParView;
	int nn = m_ParamArray.GetSize();
	int nn1 = GetTickCount();

    if(m_pParentForGraph)
    {
        VFRingBuf<F>* pMain = m_parGraph.GetVXBuffer();
        if(pMain) pMain->SetPosUMarker23ToEndBuf();
    }

	if(nn > 0)
	{
		for(int i = 0; i < m_ParamArray.GetSize(); i++)
		{
			pParView = m_ParamArray[i];
			if(pParView == NULL) return false;
			if(m_pParentForGraph && pParView->pPrm && !pParView->pPrm->IsTimedOut() && wParam)
            {
				if(m_parGraph.DrawLastData(pParView->num_grf, 1)) 
                    return true;
            }
			if(pParView->m_dgtLabel != NULL) pParView->m_dgtLabel->Invalidate(FALSE);
		}
	}
	int nn2 = GetTickCount();
	return false;
}

LRESULT CPanelWnd::OnChangeStyleGrf(WPARAM wParam, LPARAM lParam)
{
	ParView *pParView;
	int nn = m_ParamArray.GetSize();
	int nn1 = GetTickCount();

	if(nn > 0)
	{
		for(int i = 0; i < m_ParamArray.GetSize(); i++)
		{
			pParView = m_ParamArray[i];
			if(pParView == NULL) return false;
			if(m_pParentForGraph && pParView->pPrm && !pParView->pPrm->IsTimedOut())
				m_parGraph.DrawLastData(pParView->num_grf, 1);
			//pPar->m_dgtLabel->Invalidate(true);
			pParView->m_dgtLabel->Invalidate(FALSE);
		}
	}
	//	else
	//	{
	//		if(m_pParentForGraph) m_parGraph.Pinok0(1);
	//	}

	int nn2 = GetTickCount();
	//	TRACE1("\n ----------------%d", nn2 - nn1);

	return true;
}
LRESULT CPanelWnd::OnDataDBChanged(WPARAM wParam, LPARAM lParam)
{
	if(m_pParentForGraph) 
	{
		m_parGraph.UpdateViewRange();
	}
	return true;
}

void CPanelWnd::DeleteParam(int num_par_local)
{
	ParView *pParV = m_ParamArray[num_par_local];
	int num_grf = pParV->num_grf;
	pParV->m_dgtLabel->DestroyWindow();
	delete pParV->m_dgtLabel;
	pParV->m_dgtLabel = NULL;
//	pParV->pPrm = NULL;
	delete pParV;

	m_ParamArray.RemoveAt(num_par_local);

	int nn = m_ParamArray.GetSize();
	for(int i = 0; i < nn; i++) 
	{
		int j = m_ParamArray[i]->m_num_par_local;
		m_ParamArray[i]->m_num_par_local = i;
	}

	if(!type_panel) 
		m_parGraph.RemoveBufer(num_grf);

    CView* pView = pApp->GetActiveView();
    if (pView != NULL) pView->SendMessage(PM_DELETE_PARAM_FROM_LIST, num_par_local, num_panel);

	RecalcFieldSize();
}

void CPanelWnd::DeleteParamByNum(int num_par)
{
	ParView *pParView;
	for(int i = 0; i < m_ParamArray.GetSize(); i++)
	{
		pParView = m_ParamArray[i];
		if(pParView != NULL && pParView->pPrm != NULL && pParView->pPrm->m_num_par == num_par)
			DeleteParam(i);
	}
}

LRESULT CPanelWnd::OnAddParam(WPARAM wParam, LPARAM lParam)
{
    ASSERT(pFrm);
	pFrm->PostMessage(PM_ADD_PARAM, (num_panel<<16) | wParam, lParam);
	return +1;
}

LRESULT CPanelWnd::OnChangeParamProperty(WPARAM wParam, LPARAM lParam)
{
	int num_par_local		= (int)wParam;
	ParView *pParV = m_ParamArray[num_par_local];

	bool cmd_delete = false;
	
	if (type_panel == 0)
	{
		int num_grf = pParV->num_grf;		
		m_dlgChangeParam.name_param = pParV->pPrm->sName;
		m_dlgChangeParam.m_num_param = pParV->pPrm->m_num_par;
		m_dlgChangeParam.m_line_color = pParV->colorGraph;
		m_dlgChangeParam.m_line_width = pParV->line_width;
		if (pParV->pPrm && pParV->pPrm->m_shown_digits == 0xFF)
		{
			COleDateTimeSpan odts_min((double)pParV->f_minGraph);
			m_dlgChangeParam.m_min_val_gr = odts_min.GetTotalMinutes();
			COleDateTimeSpan odts_max((double)pParV->f_maxGraph);
			m_dlgChangeParam.m_max_val_gr = odts_max.GetTotalMinutes();
		}
		else
		{
			m_dlgChangeParam.m_min_val_gr = pParV->f_minGraph;
			m_dlgChangeParam.m_max_val_gr = pParV->f_maxGraph;
		}
		m_dlgChangeParam.m_min_val = pParV->pPrm->fMin;
		m_dlgChangeParam.m_max_val = pParV->pPrm->fMax;
		m_dlgChangeParam.m_block_val = pParV->pPrm->fBlock;
		m_dlgChangeParam.m_crush_val = pParV->pPrm->fAvaria;
		m_dlgChangeParam.m_type_of_control = pParV->pPrm->nKodControl;
		
		if (m_dlgChangeParam.DoModal() == IDCANCEL)
			return true;

		if (m_dlgChangeParam.num_cmd == 1) 
		{
			pParV->colorGraph = m_dlgChangeParam.m_line_color;
			pParV->line_width = m_dlgChangeParam.m_line_width;
			if (pParV->pPrm && pParV->pPrm->m_shown_digits == 0xFF)
			{
				COleDateTimeSpan odts_min(0, 0, m_dlgChangeParam.m_min_val_gr, 0);
				pParV->f_minGraph = odts_min.m_span;
				COleDateTimeSpan odts_max(0, 0, m_dlgChangeParam.m_max_val_gr, 0);
				pParV->f_maxGraph = odts_max.m_span;
			}
			else
			{
				pParV->f_minGraph = m_dlgChangeParam.m_min_val_gr;
				pParV->f_maxGraph = m_dlgChangeParam.m_max_val_gr;
			}
			m_parGraph.SetColorDiagram(num_grf, pParV->colorGraph, false);
			m_parGraph.SetThicknessDiagram(num_grf, pParV->line_width, true);
			m_parGraph.SetMinMaxF(pParV->f_minGraph, pParV->f_maxGraph, pParV->num_grf);
			pParV->pPrm->fMin = m_dlgChangeParam.m_min_val;
			pParV->pPrm->fMax = m_dlgChangeParam.m_max_val;
			pParV->pPrm->fBlock = m_dlgChangeParam.m_block_val;
			pParV->pPrm->fAvaria = m_dlgChangeParam.m_crush_val;
			pParV->pPrm->nKodControl = m_dlgChangeParam.m_type_of_control;
		}
		else
			cmd_delete = true;
	}
	else
	{
		DlgDigitalPannelSettings dlg;
		dlg.m_param_name = pParV->pPrm->sName;
		dlg.m_color = pParV->color_font_dgt;
		dlg.m_font = pParV->font_dgt;
		dlg.m_param_num = pParV->pPrm->m_num_par;
		dlg.m_font = pParV->font_dgt;
		if (dlg.DoModal() == IDCANCEL)
			return true;
		if (dlg.m_cmd_num == 1)
		{
			pParV->color_font_dgt = dlg.m_color;
			pParV->font_dgt = dlg.m_font;

            CView* pView = pApp->GetActiveView();
            if (pView != NULL) pView->SendMessage(PM_CHANGE_PARAM_PROPERT_1, num_par_local, LPARAM(pParV->m_dgtLabel));

		}
		else
			cmd_delete = true;
	}

	if (cmd_delete)
	{
        CView* pView = pApp->GetActiveView();
        if (pView != NULL) pView->SendMessage(PM_CHANGE_PARAM_PROPERT, num_par_local, LPARAM(this));
	}

	Invalidate(false);
	return true;
}

void CPanelWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
}

void CPanelWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
    ASSERT(pFrm);
	if(nFlags == 9)
	{
		CDlgListOrder dlg;
		dlg.pPanel = this;
		dlg.DoModal();
	}
	else
	{
		if (!type_panel) 
		{
			CPoint pnt = point;
			ClientToScreen(&pnt);
			pFrm->PostMessage(PM_ADD_PARAM, (num_panel<<16) | 0xFFFF, (pnt.x<<16) | (pnt.y) );
		}
		else
		{
			CView* pView = pApp->GetActiveView();
			BOOL flag_size_save = m_flag_size;

			if (rect_add.PtInRect(point)) 
			{
				CPoint pnt = point;
				ClientToScreen(&pnt);
				pFrm->PostMessage(PM_ADD_PARAM, (num_panel<<16) | 0xFFFF, (pnt.x<<16) | (pnt.y));
			}
			else 
			{
				m_flag_size = !m_flag_size;
				if (pView != NULL) pView->PostMessage(PM_RECALC_SIZE_DGT_PANELS, 0, NULL);
			}
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void CPanelWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (rect_header.PtInRect(point)) 
	{
		CRect rect = rect_header;
		rect.left += 8;
		rect.right -= 1 + 14 + 1 + 15 + 1;
		rect.top -= 2;
		ClientToScreen(&rect);
		DlgEditText dlg(sNamePanel, rect, true, -1);
		if (dlg.DoModal() == IDOK)
		{
			sNamePanel = dlg.GetText();
			Invalidate(FALSE);
		}
	}
	CStatic::OnRButtonDown(nFlags, point);
}

BOOL CPanelWnd::GetInfoPanel(PanelDescription *plstParam)
{
	plstParam->snamePanel = sNamePanel;
	plstParam->m_params.clear();
	ParamInPanel pd;
	ParView *pParView;
	for(int i = 0; i < m_ParamArray.GetSize(); i++)
	{
		pParView = m_ParamArray[i];
		pd.m_param_num = pParView->pPrm->m_num_par;
		pd.m_min_val = pParView->f_minGraph; 
		pd.m_max_val = pParView->f_maxGraph; 
		pd.m_line_width = pParView->line_width;
		pd.m_color = pParView->colorGraph; 
		pd.text_color = pParView->color_font_dgt;
		pd.text_size = pParView->font_dgt.lfHeight;
		pd.font_weight = pParView->font_dgt.lfWeight;
		plstParam->m_params.push_back(pd);
	}
	return true;
}

int CPanelWnd::GetMaxSize() 
{
	return m_ParamArray.GetSize();
}

void CPanelWnd::ShowPanel(BOOL fl_visibles)
{
	UpdateVisible(fl_visibles);
}
void CPanelWnd::SetVisibilityFlag(BOOL fl_visible)
{
	m_flag_visible_field = fl_visible;
}

BOOL CPanelWnd::GetVisible() 
{	
	return m_flag_visible_field;
}

BOOL CPanelWnd::GetRectFieldGrf(CRect& rec) 
{	
    return m_parGraph.GetRectFieldGrf(rec);
}

UINT CPanelWnd::GetStartMarkerX() 
{	
    return m_parGraph.GetStartMarkerX();
}
void CPanelWnd::RedrawPanel(typePanel tp_p)
{
	Invalidate(false);
	if(tp_p == tp_grf) m_parGraph.FieldGraph.Invalidate(false);
}

void CPanelWnd::UpdateVisible(BOOL fl_visible)
{
    if(m_hWnd == NULL) return;
	if(m_pParentForGraph != NULL) 
	{
		if(m_parGraph.m_hWnd != NULL) 
		{
            if(!fl_visible) {m_parGraph.ShowWindow(SW_HIDE), ShowWindow(SW_HIDE);}
			else if(m_flag_visible_field) m_parGraph.ShowWindow(SW_SHOW), ShowWindow(SW_SHOW);// m_parGraph.Invalidate(true);}		
			else m_parGraph.ShowWindow(SW_HIDE), ShowWindow(SW_HIDE);
		}
	}

	ParView *pParView;
	for(int i = 0; i < m_ParamArray.GetSize(); i++)
	{
		pParView = m_ParamArray[i];
		if(pParView == NULL || pParView->m_dgtLabel == NULL) continue;

		if(!fl_visible) pParView->m_dgtLabel->SetShowDgtLabel(false);
		else if(m_flag_visible_field) pParView->m_dgtLabel->SetShowDgtLabel(true);
		else pParView->m_dgtLabel->SetShowDgtLabel(false);
	}

	if(!fl_visible) ShowWindow(SW_HIDE);
	else if(m_flag_visible_field) RecalcFieldSize(), ShowWindow(SW_SHOW);
	else ShowWindow(SW_HIDE);
}

void CPanelWnd::UpdateRectRange(int num_grf_panel, BOOL flRedraw)
{
	if(m_pParentForGraph != NULL && m_parGraph.m_hWnd != NULL) 
	{
		if(m_parGraph.num_self != num_grf_panel) m_parGraph.UpdateRectRangeExternal(flRedraw);
	}
}
void CPanelWnd::UpdateScrollRange(int num_grf_panel, int num_scroll)
{
    if(m_pParentForGraph != NULL && m_parGraph.m_hWnd != NULL) 
    {
        if(m_parGraph.num_self != num_grf_panel) m_parGraph.SetScroolGraphExternal(num_scroll);
    }
}

void CPanelWnd::ChangeGraphColor()
{
	m_parGraph.SetColorGreedGraph(g_color_interface.color_graph_greed);
	m_parGraph.SetColorDgtX(g_color_interface.color_graph_dgt);
	m_parGraph.SetColorDgtY(g_color_interface.color_graph_dgt);
	m_parGraph.SetColorFieldGraph(g_color_interface.color_graph_bk);
	m_parGraph.SetColorBk(g_color_interface.color_graph_dgt_bk);
	m_parGraph.SetStyleGreedGraph(g_StyleGreedGraph, 1, true);
}


void CPanelWnd::SetScaleTime(LPVOID pBufX, double ScaleXStart, double ScaleXEnd, BOOL flagTimeGlub, CString textGrf) 
{
	m_pBufX = pBufX;
	m_ScaleXStart = ScaleXStart;
	m_deltaX = ScaleXEnd - ScaleXStart;
	m_flagTimeGlub = flagTimeGlub;

	if(m_flagTimeGlub) m_parGraph.RenameHScale("В Р Е М Я", true, pBufX);
	else m_parGraph.RenameHScale("Г Л У Б И Н А   З А Б О Я", false, pBufX);

	m_parGraph.SetMinMaxTScale(m_ScaleXStart, m_ScaleXStart + m_deltaX);
	m_parGraph.SetTextGrf(textGrf, 0);
	m_parGraph.SetColorTextGrf(RGB(204*0.8,236*0.8,255*0.8));
}

void CPanelWnd::SetScaleXDiapazon(LPVOID pBufX, double ScaleXStart, double ScaleXEnd, BOOL flagTimeGlub, CString textGrf) 
{
	m_pBufX = pBufX;
	m_ScaleXStart = ScaleXStart;
	m_deltaX = ScaleXEnd - ScaleXStart;
	m_flagTimeGlub = flagTimeGlub;

	if(m_flagTimeGlub) m_parGraph.RenameHScale("В Р Е М Я", true, pBufX);
	else m_parGraph.RenameHScale("Г Л У Б И Н А   З А Б О Я", false, pBufX);

	m_parGraph.SetMinMaxXDiapazon(m_ScaleXStart, m_ScaleXStart + m_deltaX);
	m_parGraph.SetTextGrf(textGrf, 0);
	m_parGraph.SetColorTextGrf(RGB(204*0.8,236*0.8,255*0.8));
}

void CPanelWnd::SetTextGrfPanel(CString textGrf, BOOL flRedraw) 
{
	m_parGraph.SetTextGrf(textGrf, flRedraw);
}

void CPanelWnd::UpdateReper(BOOL flg) 
{
	m_parGraph.UpdateReper(flg);
}
void CPanelWnd::SynchronizeGraph(double min_x, double max_x) 
{
    m_parGraph.SynchronizeGraph(min_x, max_x);
}
VFRingBuf<double>* CPanelWnd::GetVXBuffer() 
{
	return m_parGraph.GetVXBuffer();
}

void CPanelWnd::ChangeScale(LPVOID pBufX, GRF_MINMAX& par, BOOL flagTimeGlub)
{
	m_flagTimeGlub = flagTimeGlub;
	if(par.flags_minmax.bitFlags.fl_buf_x) m_pBufX = pBufX;
	if(par.flags_minmax.bitFlags.fl_min_max) m_deltaX = par.Fmax - par.Fmin;

	if(m_flagTimeGlub) m_parGraph.ChangeScale("В Р Е М Я", true, m_pBufX, par);
	else m_parGraph.ChangeScale("Г Л У Б И Н А   З А Б О Я", false, m_pBufX, par);
	m_parGraph.SetColorTextGrf(RGB(204*0.8,236*0.8,255*0.8));
}
void CPanelWnd::GetMinMaxX(double& min, double& max)
{
    m_parGraph.GetMinMaxT(min, max);
}

void CPanelWnd::ClearParams()
{
	ParView *pParView;
	for(int i = 0; i < m_ParamArray.GetSize(); i++) 
	{
		if(m_ParamArray[i] != NULL) 
		{
			pParView = m_ParamArray[i];
			delete pParView->m_dgtLabel;
			pParView->m_dgtLabel = NULL;
			pParView->pPrm = NULL;
			delete pParView;
		}
	}
	m_ParamArray.RemoveAll();
}

int CPanelWnd::AddParam(ParamInPanel* pd, BOOL flg_redraw)
{
	int nKey = pd->m_param_num;
	CRect recGraph(0,0,30,30);
	CParam *pPar;
	if (!m_MapParam->Lookup(nKey, pPar))
		return -1;
	if(pPar == NULL) return -1;

	ParView *pParView = new ParView;
	pParView->pPrm = pPar;
	pParView->m_num_par_local = m_ParamArray.GetSize();
	pParView->colorGraph = pd->m_color;
	pParView->f_minGraph = (float)pd->m_min_val;
	pParView->f_maxGraph = (float)pd->m_max_val;
	pParView->line_width = pd->m_line_width;

	if (m_pParentForGraph != NULL)
	{
        if(pPar->pBufParam == NULL) pPar->Init();
		pParView->num_grf = m_parGraph.CreateBufYWithExtInpBuf(0, *(pPar->pBufParam));
		m_parGraph.SetMinMaxF(pParView->f_minGraph, pParView->f_maxGraph, pParView->num_grf);
		m_parGraph.SetThicknessDiagram(pParView->num_grf, pParView->line_width, true);
		m_parGraph.SetColorDiagram(pParView->num_grf, pParView->colorGraph, false);
		m_parGraph.SetNameGrf(pParView->num_grf, pPar->sGraphName);

		CHeaderGrf* dgtLabel = new CHeaderGrf;
		dgtLabel->Create(NULL, WS_CHILD, recGraph, this);
		dgtLabel->SetParamHeader(pParView, &m_parGraph);
		pParView->m_dgtLabel = dgtLabel;
	}
	else
	{
		pParView->color_font_dgt = pd->text_color;
		pParView->font_dgt.lfHeight = pd->text_size;
		pParView->font_dgt.lfWeight = pd->font_weight;

		COutParamPanel* dgtLabel = new COutParamPanel;
		dgtLabel->m_logfontN.lfHeight = pd->text_size;
		dgtLabel->m_logfontN.lfWeight = pd->font_weight;
		dgtLabel->Create(NULL, WS_CHILD, recGraph, this);
		dgtLabel->SetParamHeader(pParView, &m_parGraph);
		pParView->m_dgtLabel = dgtLabel;
	}

	m_ParamArray.Add(pParView);

	if (flg_redraw && m_pParentForGraph != NULL) 
	{
		RecalcFieldSize();
		//Invalidate(true);
		Invalidate(false);
		m_parGraph.FieldGraph.Invalidate(false);
	}
	return pParView->m_num_par_local;
}

void CPanelWnd::RequestData(COleDateTime begTime, COleDateTime endTime)
{
}

void CPanelWnd::InitPanelForDataDB(COleDateTime begTime, COleDateTime endTime, CString textGrf)
{

	int nn = m_ParamArray.GetSize();
	if(nn == 0) return;

	m_ScaleXStart = begTime.m_dt;
	m_deltaX = endTime.m_dt - begTime.m_dt;

	m_parGraph.SetMinMaxXDiapazon(m_ScaleXStart, m_ScaleXStart + m_deltaX);
	m_parGraph.SetTextGrf(textGrf, 0);
	m_parGraph.SetColorTextGrf(RGB(204*0.8,236*0.8,255*0.8));

	m_parGraph.ResetStyle(0x10000);
}

int CPanelWnd::AddActiveParam(CParamMap *pActive)
{
	int nn = m_ParamArray.GetSize();
	if(nn == 0) return pActive->GetCount();;

	ParView *pParView;
	CParam *pParI;
	for(int i = 0; i < nn; i++)
	{
		pParView = m_ParamArray[i];
		if(pParView == NULL) return pActive->GetCount();;

		if(!pActive->Lookup(pParView->pPrm->m_num_par, pParI)) 
			(*pActive)[pParView->pPrm->m_num_par] = pParView->pPrm;
	}

	return pActive->GetCount();
}

void CPanelWnd::RecalcFieldSize()
{
	CRect recHeadI, recHead;

	int mm = m_ParamArray.GetSize();
	if(mm == 0 || mm < 0) return;

	int j;

	GetClientRect(&recHead);

	if(m_pParentForGraph != NULL)
	{
        if(m_flag_orient)
        {
		    int widthI = recHead.Width();
		    int heightI = recHead.Height()/5;
		    int splitH = (recHead.Height() - 5*heightI)/2;

		    recHeadI.left = recHead.left;
		    recHeadI.right = recHead.right;

		    for(j = 0; j < mm; j++)
		    {
			    recHeadI.bottom = recHead.bottom - heightI * m_ParamArray[j]->m_num_par_local;
			    recHeadI.top = recHeadI.bottom -  heightI;
				m_ParamArray[j]->m_flag_orient = m_flag_orient;
				m_ParamArray[j]->m_dgtLabel->SetFontDgt();

			    m_ParamArray[j]->m_dgtLabel->SetWindowPos(&wndTop, 
				    recHeadI.left, recHeadI.top, recHeadI.Width(), recHeadI.Height(), 
				    SWP_SHOWWINDOW);
		    }
        }
        else
        {
            int widthI = recHead.Width()/5;
            int heightI = recHead.Height();
            int splitW = (recHead.Width() - 5*widthI)/2;

            recHeadI.top = recHead.top;
            recHeadI.bottom = recHead.bottom;

            for(j = 0; j < mm; j++)
            {
                recHeadI.right = recHead.right - widthI * m_ParamArray[j]->m_num_par_local;
                recHeadI.left = recHeadI.right -  widthI;
                m_ParamArray[j]->m_flag_orient = m_flag_orient;
				if(m_ParamArray[j]->m_dgtLabel == NULL) continue;

				m_ParamArray[j]->m_dgtLabel->SetFontDgt();

                m_ParamArray[j]->m_dgtLabel->SetWindowPos(&wndTop, 
                    recHeadI.left, recHeadI.top, recHeadI.Width(), recHeadI.Height(), 
                    SWP_SHOWWINDOW);
            }
        }
	}
	else
	{
		CRect recPanel;
		int n = m_ParamArray.GetSize();
		if(n == 0) return;

		GetClientRect(&recPanel);
		
		int widthI = recPanel.Width();
		int splitL = 5;

		recHeadI.left = splitL;
		recHeadI.right = recPanel.right - splitL;

		CSize sz;
		int heightI = 0;
		int cy = 0;
		for(int i = 0; i < n; i++)
		{
			sz = m_ParamArray[i]->m_dgtLabel->GetSizeText("P");
			heightI = sz.cy + 2;

			recHeadI.top = m_splitH + cy;
			recHeadI.bottom = recHeadI.top + heightI;
			m_ParamArray[i]->m_dgtLabel->SetWindowPos(&wndTop, 
				recHeadI.left, recHeadI.top, recHeadI.Width(), recHeadI.Height(), 
					SWP_SHOWWINDOW);

			cy += heightI;
		}
	}
}

CSize CPanelWnd::GetSizeDgtPanel()
{
	int n = m_ParamArray.GetSize();

	if(n <= 0 || !m_flag_size) return CSize(0, m_splitH);
	CSize sz(0,0), sz_0;
	for(int i = 0; i < n; i++)
	{
		sz_0 = m_ParamArray[i]->m_dgtLabel->GetSizeText("P");

		sz.cy += sz_0.cy;
		if(i < n-1) sz.cy += 2;//int(0.5 * sz_0.cy);
	}

	sz.cy += m_splitH;
	return sz;
}

extern void Daraw2Rect(CDC* pDC, CRect& rec, COLORREF clr);
void CPanelWnd::PrintGrf(CDC *pDC, CWnd *pWnd, float printScaleX, float printScaleY, 
	CSize *printShift)
{
	m_parGraph.DrawGrf(pDC, pWnd, printScaleX, printScaleY, printShift);

	CRect rec;
	GetClientRect(&rec);
	ClientToScreen(&rec);
	Daraw2Rect(pDC, rec, 0);

	CSize sz = *printShift;
	int n = m_ParamArray.GetSize();
	for(int i = 0; i < n; i++)
	{
		m_ParamArray[i]->m_dgtLabel->DrawHeader(pDC, pWnd, printScaleX, printScaleY, &sz);
	}
}

void CPanelWnd::LockDataForPrint(int lock_value)
{
    m_parGraph.loc_data_for_print = lock_value;
}

CPanelWnd::~CPanelWnd()
{
	ClearParams();
}

CString ParamInPanel::GetTextForSave()
{
	CString str;
	str.Format(" %d %s-%s %d %06X %06X %d %d |", 
		m_param_num, 
		BS_FloatWOZeros((float)m_min_val, 3), 
		BS_FloatWOZeros((float)m_max_val, 3), 
		m_line_width, 
		m_color,
		text_color,
		text_size,
		font_weight);
	return str;
}

bool ParamInPanel::FillFromText(CString text)
{
	int nn =  sscanf(text, "%d %lf-%lf %d %X %X %d %d", 
			&m_param_num,
			&m_min_val, &m_max_val,
			&m_line_width,
			&m_color, &text_color,
			&text_size,
			&font_weight);
	return nn == 8 ? true : false;
}

CString PanelDescription::GetTextForSave()
{
	CString str;
	str.Format("%s$ %d:", snamePanel, m_params.size());
	list<ParamInPanel>::iterator current = m_params.begin();
	while (current != m_params.end())
	{
		str += (*current).GetTextForSave();
		current++;
	}
	return str;
}

bool PanelDescription::FillFromText(CString text)
{
	m_params.clear();
	snamePanel = "Параметры";
	if (text.GetLength() == 0)
		return FALSE;

	int ind;

	ind = text.Find('$');
	if (ind < 0)
		return false;
	do
	{
		ind++;
		if (text.GetLength() <= ind)
			return false;
	} while (text.GetAt(ind) == '$');
	snamePanel = text.Left(ind - 1);

	int num;
	text = text.Right(text.GetLength() - ind);
	text.TrimLeft();
	if (sscanf(text, "%d", &num) != 1)
		return false;

	ind = text.Find(':');
	if (ind < 0)
		return false;
	text = text.Right(text.GetLength() - ind - 1);
	text.TrimLeft();

	ParamInPanel one_descrition;
	for (int i = 0; i < num; i++)
	{
		if (!one_descrition.FillFromText(text))
			return false;
		m_params.push_back(one_descrition);

		ind = text.Find('|');
		if (ind < 0)
			return false;
		text = text.Right(text.GetLength() - ind - 1);
		text.TrimLeft();
	}
	return true;
}

