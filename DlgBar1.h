#if !defined(AFX_DLGBAR1_H__754AE475_C2DB_45FE_8FC0_2E21EF0CF890__INCLUDED_)
#define AFX_DLGBAR1_H__754AE475_C2DB_45FE_8FC0_2E21EF0CF890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Indicator.h"
#include "Indicftor.h"
#include "ExtListCtrl.h"
#include "DialogBarMain.h"
#include "PanelKnopki.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgBar1 dialog

class CDlgBar1 : public CDialogBarMain//CDialogBar
{
// Construction
public:
	CDlgBar1();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBar1)
	enum { IDD = IDD_DIALOG_PANEL0 };
	CIndicftor	m_Regim;
	CIndicftor	m_IndDataTime;
	Indicator	m_ind_shurf;
	Indicator	m_ind_load;
	Indicator	m_ind_pressure;
	Indicator	m_ind_weight;
	Indicator	m_ind_serv;
	Indicator	m_ind_conv;
	Indicator	m_ind_db;
	CPanelKnopki	m_PanelKnopki;
	//}}AFX_DATA

    void RecalcPosIndicators();
    void SetViewDiag();
    void SetIndParamBur();
    void SetIndDataTime();
    void SetIndConverter();
	void SetIndRegimeBur(BOOL flg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBar1)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDlgBar1();
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBar1)
   	afx_msg LRESULT OnMyInitDialog(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBAR1_H__754AE475_C2DB_45FE_8FC0_2E21EF0CF890__INCLUDED_)
