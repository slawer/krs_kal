#pragma once

class DlgHardCfg : public CDialog
{
	DECLARE_DYNAMIC(DlgHardCfg)

public:
	DlgHardCfg(bool save, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgHardCfg();

	CString m_cfg_name;
	bool m_save;
	enum { IDD = IDD_DIALOG_HARD_CFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void OnOK();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLbnSelchangeListCfgs();
};
