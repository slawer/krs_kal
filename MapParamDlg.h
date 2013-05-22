#if !defined(AFX_MAPPARAMDLG_H__C67EA6A6_8FDC_4B37_AC5F_7135BF2258DB__INCLUDED_)
#define AFX_MAPPARAMDLG_H__C67EA6A6_8FDC_4B37_AC5F_7135BF2258DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapParamDlg.h : header file
//

#include "MainFrm.h"
#include "ExtListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// MapParamDlg dialog

class MapParamDlg : public CDialog
{
// Construction
public:
	MapParamDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(MapParamDlg)
	enum { IDD = IDD_DIALOG_MAP_PARAM };
	CExtListCtrl m_table_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MapParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//map<int, int> m_param_to_channel_map;

	void ChooseChannel(int param_num);
	void RebuildTable();
	void SetupLine(int item, CParam *param);
	void SetupLineColor(int item);

	int m_prev_selection;

	CMainFrame *m_frame;
	CParamMap m_AllParams;

	// Generated message map functions
	//{{AFX_MSG(MapParamDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonChange();
	afx_msg void OnDblClickListTable(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonAdd();
	afx_msg LRESULT OnListSelectionChanged(WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPPARAMDLG_H__C67EA6A6_8FDC_4B37_AC5F_7135BF2258DB__INCLUDED_)
