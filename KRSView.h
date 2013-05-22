// KRSView.h : interface of the CKRSView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_KRSVIEW_H__43C34E4F_32CC_4D80_BB27_515B693A6685__INCLUDED_)
#define AFX_KRSVIEW_H__43C34E4F_32CC_4D80_BB27_515B693A6685__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Param.h"
#include "src/SERV.h"
#include "PanelArray.h"
#include "MainView.h"

class CKRSView : public CMainView
{
protected: // create from serialization only
	CKRSView();
	DECLARE_DYNCREATE(CKRSView)
    BOOL m_flag_init;

public:
	//{{AFX_DATA(CKRSView)
	enum{ IDD = IDD_KRS_FORM };
	//}}AFX_DATA

// Attributes
public:
// Operations
public:
// Overrides
public:
	virtual void ChangeaActiveListA(WPARAM wParam, LPARAM lParam);
	virtual void RButtonDown1A(WPARAM wParam, LPARAM  lParam);
	virtual double GetStartX(int num_list);
	virtual void SetStartX(double start_value, int num_list, BOOL flg);
	void ReloadDataView();
	BOOL ReadDataMessageA();

	int ClearBeginAllData();
	int TestSizeRingBuf();

// ClassWizard generated virtual function overrides
//{{AFX_VIRTUAL(CKRSView)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CKRSView();

// Generated message map functions
protected:
	afx_msg LRESULT OnReadDataMessage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSynchronizeGraph(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLButtonDown1(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
//    virtual void Serialize(CArchive& ar);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KRSVIEW_H__43C34E4F_32CC_4D80_BB27_515B693A6685__INCLUDED_)
