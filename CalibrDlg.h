#if !defined(AFX_CALIBRDLG_H__1AF14F53_963A_4454_ADF8_CA4479D38817__INCLUDED_)
#define AFX_CALIBRDLG_H__1AF14F53_963A_4454_ADF8_CA4479D38817__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#pragma warning(disable:4786)
#include <list>
using namespace std;

#include "src\SERV.h"
#include "Indicator.h"

/////////////////////////////////////////////////////////////////////////////
// CalibrDlg dialog

class CalibrDlg : public CDialog
{
// Construction
public:
	CalibrDlg(HWND parent_hwnd, CParam* param, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CalibrDlg)
	enum { IDD = IDD_DIALOG_CALIBR };
	CListCtrl	m_table_list;
	Indicator	m_indicator_wait;
	int		m_combo_point_num;
	//}}AFX_DATA

	void ResetToDevice();
	void RebuildTable();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CalibrDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CalibrDlg)
	afx_msg void OnButtonSet();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnButtonReload();
	afx_msg void OnSaveTable();
	afx_msg void OnButtonZero();
	//}}AFX_MSG

	SERV_CalibrationTable::iterator CalcCurrentADSP();

	bool m_calibration_enabled;
	void EnableCalibration(bool ena);

	void DrawLabel(WORD val, CDC& dc, byte mode, int offset, bool last);
	void DrawLabels(byte mode, CDC& dc);
	LRESULT OnReady(WPARAM, LPARAM);

	int GraphX(int adsp);
	int GraphY(int val);

	CFont m_fx, m_fy;
	DWORD m_current_point_color;

	HWND m_parent_hwnd;

	SERV_CalibrationTable m_current_cal_table, m_saved_cal_table;
	WORD m_current_dsp, m_current_phys;

	CParam* m_param;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALIBRDLG_H__1AF14F53_963A_4454_ADF8_CA4479D38817__INCLUDED_)
