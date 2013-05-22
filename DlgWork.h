// uic 19.08.2008

#pragma once
#include "afxwin.h"
#include "KRS.h"
#include "src\PRJ.h"

#define DlgWork_OPEN 1
#define DlgWork_NEW 2
#define DlgWork_IMPORT 4
#define DlgWork_EDIT 8

class DlgWork : public CDialog
{
	DECLARE_DYNAMIC(DlgWork)

public:
	DlgWork(CString title, byte mode, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgWork();

	enum { IDD = IDD_DIALOG_WORK };

	CComboBox m_combo_m;
	CComboBox m_combo_k;
	CComboBox m_combo_s;
	CComboBox m_combo_r;
	
	CComboBox m_combo_template;

	byte m_mode;
	CString m_title;
	bool m_failed_load, m_old_project_saved, m_new_project_saved;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	void AdjustCombos();
	void SelectChild(PRJ_Object* obj, CComboBox& combo);
	void AddChild(PRJ_Object* parent, PRJ_Object* obj, CComboBox& combo, CString initial_text);
	void RenameObject(PRJ_Object* obj, CComboBox& combo);
	void DeleteObject(PRJ_Object* object, CString class_name);
	void CopyFilesFromRoot();

	virtual BOOL OnInitDialog();

	afx_msg void OnCbnSelchangeComboM();
	afx_msg void OnBnClickedButtonAddM();
	afx_msg void OnBnClickedButtonEditM();
	afx_msg void OnBnClickedButtonDeleteM();
	afx_msg void OnCbnSelchangeComboK();
	afx_msg void OnBnClickedButtonAddK();
	afx_msg void OnBnClickedButtonEditK();
	afx_msg void OnBnClickedButtonDeleteK();
	afx_msg void OnCbnSelchangeComboS();
	afx_msg void OnBnClickedButtonAddS();
	afx_msg void OnBnClickedButtonEditS();
	afx_msg void OnBnClickedButtonDeleteS();
	afx_msg void OnCbnSelchangeComboR();
	afx_msg void OnBnClickedButtonAddR();
	afx_msg void OnBnClickedButtonEditR();
	afx_msg void OnBnClickedButtonDeleteR();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

////////////////////////////////////////////////////////////////////////////////
// end