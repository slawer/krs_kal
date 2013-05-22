#pragma once

#include "Indicator.h"

class DlgDigitalPannelSettings : public CDialog
{
	DECLARE_DYNAMIC(DlgDigitalPannelSettings)

public:
	DlgDigitalPannelSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgDigitalPannelSettings();

// Dialog Data
	enum { IDD = IDD_DIALOG_DIGITAL_PANNEL_SETTINGS };
	int m_cmd_num;
	CString m_param_name;
	DWORD m_color;
	Indicator m_ind_color;
	LOGFONT m_font;
	int m_param_num;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnStaticColor();
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButtonChooseFont();
};
