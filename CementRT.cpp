#include "stdafx.h"
#include "resource.h"
#include "KRS.h"
#include "MainFrm.h"
#include "CementDoc.h"
#include "CementRT.h"

extern double TIME_current_time;

IMPLEMENT_DYNCREATE(CCementRT, CCementView)

CCementRT::CCementRT() : CCementView()
{
    type_view = t_cement_rt;
    m_flag_init = false;
}

CCementRT::~CCementRT()
{
}

void CCementRT::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCementRT, CCementView)
    ON_MESSAGE(PM_DATAACP, OnReadDataMessage)
    ON_MESSAGE(PM_SYNCHRONIZE_GRAPH, OnSynchronizeGraph)
    ON_MESSAGE(PM_EDITPOINT, OnLButtonDown1)
    ON_WM_SETFOCUS()
	ON_WM_TIMER()
	ON_WM_CREATE()
END_MESSAGE_MAP()

LRESULT CCementRT::OnReadDataMessage(WPARAM wParam, LPARAM lParam)
{
    if (m_parOfView == NULL) return false;
//------------------------------------------------
    if(TIME_current_time == 0) return false;
//------------------------------------------------
	ParamOfView *pList;
//-----------------------------------
//........................................................
	if ((pList = m_parOfView->GetList(STAGES_GetExecutingStage()-1)) != NULL)
		pList->UpdateInputData(1);
//---------------------------------------------
    STAGES_EvaluateCalculations();
    return true;
}

LRESULT CCementRT::OnSynchronizeGraph(WPARAM wParam, LPARAM lParam)
{
	int num_work_stage = STAGES_GetExecutingStage() - 1;
	if(pApp->IsActiveView(type_view) && m_parOfView->GetNumActiveList() == num_work_stage)
		m_parOfView->GetList(num_work_stage)->SynchronizeGraph(int(wParam));
    return true;
}

LRESULT CCementRT::OnLButtonDown1(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 150258)
    {
        flag_scroll = lParam;
        SetParXScale(m_parOfView->GetNumActiveList());
    }
    return true;
}

void CCementRT::OnInitialUpdate()
{
    if(m_flag_init) return;
    m_flag_init = true;

	m_ptmData	= pgtmData;
	m_pcurGlub	= pgcurGlub;
	CCementView::OnInitialUpdate();

    KRS_CheckFinishInitialization();
}

void CCementRT::OnSetFocus(CWnd* pOldWnd)
{
    CFormView::OnSetFocus(pOldWnd);
    ASSERT(pFrm);
    CheckRegim();
    CheckGrf();
    pFrm->ChangeTypeView((int)type_view, 2);
    SetVisibilityPanelEtap();
}

void CCementRT::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		if (!KRS_running)
			STAGES_InvalidateLists();
	}

	if (nIDEvent == 10 && gflgDataReaded != 0)
	{
		KillTimer(nIDEvent);
	}

	CCementView::OnTimer(nIDEvent);
}

int CCementRT::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCementView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
