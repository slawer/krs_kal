#if !defined(AFX_NEWCORRESPONDENCEDLG_H__D6E7A805_CA22_4DCD_8A74_AA7208211087__INCLUDED_)
#define AFX_NEWCORRESPONDENCEDLG_H__D6E7A805_CA22_4DCD_8A74_AA7208211087__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewCorrespondenceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// NewCorrespondenceDlg dialog

class NewCorrespondenceDlg : public CDialog
{
public:
	NewCorrespondenceDlg(CParam* param, bool new_param, CWnd* pParent = NULL);   
	enum { IDD = IDD_DIALOG_NEW_CORRESPONDENCE };

	int m_channel_num, m_channel_conv; 
	int m_attr_channel_num, m_attr_channel_conv; 
	FORMULA_Formula* m_formula;
	int m_param_type;

	int m_line_width, m_digits;
	COLORREF m_line_color;

	CComboBox	m_combo_param_type;
	Indicator	m_ind_color;
	CComboBox	m_combo_control_type;
	int		m_num_par; 
	float	m_min_val, m_max_val, m_min_val_gr, m_max_val_gr, m_block_val, m_crush_val, m_interval_average, m_db_data_interval;
	double	m_db_time_interval;
	CString	m_name, m_file1_path, m_file2_path, m_file3_path, m_name_grf, m_mu;
	BOOL	m_store_to_db, m_active;

	void SetEditReadonly(UINT id, bool readonly);
	CParam* m_param;
	bool m_new_param;
	CMainFrame* m_frame;

	void BrowseFile(CString &path);
	void ClearFile(CString &path, CString name);

	void SetChannelText(UINT id, int conv_num, int channel_num);
	void UpdateParamInfo();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NewCorrespondenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OutputThick();
	void OutputDigits();

	// Generated message map functions
	//{{AFX_MSG(NewCorrespondenceDlg)
	afx_msg void OnButtonChoose();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonChooseNothing();
	afx_msg void OnButtonBrowse1();
	afx_msg void OnButtonBrowse2();
	afx_msg void OnButtonBrowse3();
	afx_msg void OnButtonChooseAttr();
	afx_msg void OnButtonChooseNothingAttr();
	afx_msg void OnButtonPlus();
	afx_msg void OnButtonMinus();
	afx_msg void OnStaticColor();
	afx_msg void OnComboParamType();
	afx_msg void OnButtonPlus2();
	afx_msg void OnButtonMinus2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedCheckActive();
public:
	afx_msg void OnBnClickedCheckDb();
public:
	afx_msg void OnBnClickedButtonClear1();
public:
	afx_msg void OnBnClickedButtonClear2();
public:
	afx_msg void OnBnClickedButtonClear3();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCORRESPONDENCEDLG_H__D6E7A805_CA22_4DCD_8A74_AA7208211087__INCLUDED_)
