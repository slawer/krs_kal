// DlgWaiting.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "DlgWaiting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_POS 5000

#define WM_ENFORCE_CLOSE (WM_USER + 1)

DlgWaiting::DlgWaiting(bool available_to_cancel, CWnd* pParent /*=NULL*/)
	: CDialog(DlgWaiting::IDD, pParent),
	m_available_to_cancel(available_to_cancel),
	m_inited(false)
{
	//{{AFX_DATA_INIT(DlgWaiting)
	//}}AFX_DATA_INIT
}

void DlgWaiting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgWaiting)
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_progress_bar);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgWaiting, CDialog)
	//{{AFX_MSG_MAP(DlgWaiting)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_MESSAGE(WM_ENFORCE_CLOSE, OnEnforceClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL DlgWaiting::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_progress_bar.SetRange32(0, MAX_POS);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_available_to_cancel);
	m_inited = true;
	return TRUE;
}

void DlgWaiting::Start(int total_time_in_millisec)
{
	m_progress_bar.SetPos(0);
	m_total_tick_count = total_time_in_millisec;
	m_passed_tick_count = 0;
	Resume();
}

void DlgWaiting::Pause()
{
	m_passed_tick_count += GetTickCount() - m_start_tick_count;
	KillTimer(1);
}

void DlgWaiting::Resume()
{
	m_start_tick_count = GetTickCount();
	SetTimer(1, 20, NULL);
}

void DlgWaiting::Stop()
{
	Pause();
	m_progress_bar.SetPos(INT_MAX);
}


void DlgWaiting::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		DWORD waited = GetTotalWitedTime();
		DWORD pos = waited*MAX_POS/GetTotalWitingTime();
		m_progress_bar.SetPos(pos);
		if (GetTotalWitingTime() - waited < 20)
			Stop();
	}
	else
		CDialog::OnTimer(nIDEvent);
}

void DlgWaiting::OnCancel() 
{
}

void DlgWaiting::OnButtonStop() 
{
	SignalToClose(IDCANCEL);
}

void DlgWaiting::SignalToClose(int id)
{
	if (!::IsWindow(m_hWnd))
		return;
	::PostMessage(m_hWnd, WM_ENFORCE_CLOSE, id, 0);
}

LRESULT DlgWaiting::OnEnforceClose(WPARAM id, LPARAM)
{
	if (!::IsWindow(m_hWnd))
		return -1;
	EndDialog(id);
	return +1;
}
