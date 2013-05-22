#pragma once

class DlgDevicesMemory : public CDialog
{
	DECLARE_DYNAMIC(DlgDevicesMemory)

public:
	DlgDevicesMemory(bool download, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgDevicesMemory();

	enum { IDD = IDD_DIALOG_MEM };
	bool m_download;
	CProgressCtrl m_progress_devices, m_progress_memory;
	CListBox m_list_log;
	
	static CString m_cfg_name;
	
	void StartDevice(SERV_Device* device);
	void StartNext();
	void AddNegativeLog(CString log);
	void AddLog(CString log);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnMessage(WPARAM wp, LPARAM lp);
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

void DlgDevicesMemory_UploadMem();
void DlgDevicesMemory_DownloadMem();