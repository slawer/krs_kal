#pragma once

class DlgCalibrNew : public CDialog
{
	DECLARE_DYNAMIC(DlgCalibrNew)

public:
	DlgCalibrNew(CParam *param, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgCalibrNew();

	void Draw(CDC& dc);

	enum { IDD = IDD_DIALOG_CALIBR_NEW };
	
	void ReadTable();
	void WriteTable();

	void ResetToDevice();
	void RebuildTable();
	
protected:
	WORD m_current_dsp, m_current_phys;
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	LRESULT OnReady(WPARAM, LPARAM);
	LRESULT OnRebuilt(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAdd();
public:
	afx_msg void OnBnClickedButtonReset();
public:
	afx_msg void OnBnClickedButtonSave();
public:
	afx_msg void OnBnClickedRadio1();
public:
	afx_msg void OnBnClickedRadio2();
public:
	afx_msg void OnBnClickedRadio3();
public:
	afx_msg void OnBnClickedButtonClear();
public:
	afx_msg void OnBnClickedButtonKeep();
public:
	afx_msg void OnBnClickedCheckCalcOuter();
public:
	afx_msg void OnBnClickedButtonCfg();
public:
	afx_msg void OnBnClickedCheckScale();
};
