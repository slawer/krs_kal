#if !defined(AFX_DLGCONFIGDEVICES_H__5C92E684_0BB4_4832_8FBC_1D43180474A7__INCLUDED_)
#define AFX_DLGCONFIGDEVICES_H__5C92E684_0BB4_4832_8FBC_1D43180474A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConfigDevices.h : header file
//

#include <SERV.h>

/////////////////////////////////////////////////////////////////////////////
// DlgConfigDevices dialog

class DlgConfigDevices : public CDialog
{
// Construction
public:
	DlgConfigDevices(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DlgConfigDevices)
	enum { IDD = IDD_DIALOG_CONFIG_DEVICES };
	CTreeCtrl	m_devices_tree;
	//}}AFX_DATA

	SERV_DeviceCfg* m_tmp_cfg;

	CImageList m_image_list;
	CBitmap m_bitmap_device, m_bitmap_channel;

	void EnableEditDelete(BOOL enable);
	void ResetCfgName();
	void RebuildTree();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgConfigDevices)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgConfigDevices)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangingTreeDevices(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonAddDevice();
	afx_msg void OnButtonAddChannel();
	virtual void OnCancel();
	afx_msg void OnSaveConfig();
	afx_msg void OnButtonEdit();
	afx_msg void OnLoadConfig();
	afx_msg void OnButtonDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONFIGDEVICES_H__5C92E684_0BB4_4832_8FBC_1D43180474A7__INCLUDED_)
