#if !defined(AFX_DlgSelectChannel_H__F6FF4CB2_2217_4871_A9F6_91B62DC1BCE1__INCLUDED_)
#define AFX_DlgSelectChannel_H__F6FF4CB2_2217_4871_A9F6_91B62DC1BCE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DlgSelectChannel : public CDialog
{
public:
	DlgSelectChannel(CString name, bool allow_convertor, CWnd* pParent = NULL);   // standard constructor

	CImageList m_image_list;
	CBitmap m_bitmap_device, m_bitmap_channel, m_bitmap_cfg;

	bool m_allow_convertor;
	CString m_name;

	//{{AFX_DATA(DlgSelectChannel)
	enum { IDD = IDD_DIALOG_SELECT_CHANNEL };
	CTreeCtrl	m_channels_tree;
	//}}AFX_DATA

	HTREEITEM CheckDataOfChilds(HTREEITEM parent, DWORD data);

	int m_convertor_id;
	int m_channel_id;

	//{{AFX_VIRTUAL(DlgSelectChannel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(DlgSelectChannel)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeListSelectCalibrParam();
	afx_msg void OnSelchangingTreeDevices(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgSelectChannel_H__F6FF4CB2_2217_4871_A9F6_91B62DC1BCE1__INCLUDED_)
