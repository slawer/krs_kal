#if !defined(AFX_DLGBARH_H__E1BE4257_ADAB_406F_B4D2_3FA56C857605__INCLUDED_)
#define AFX_DLGBARH_H__E1BE4257_ADAB_406F_B4D2_3FA56C857605__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDLgBarH : public CDialogBar
{
public:
	CDLgBarH();

	enum { IDD = IDD_DIALOG_PANEL_H };

public:
	int m_type_view;

	void HideShowTimeDeepnessButtons(bool visible);
	void EnableViewSwitchers(BOOL enable);

	void OutputWk(float dWk, bool send);
	void OutputNKBK(int dkbk);
	void OutTextNKBNK();
	void OutputTalblock(float dT);
    void SetViewRealArch();
    void SetViewTimeGlub();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual ~CDLgBarH();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateEnable(CCmdUI* pCmdUi);
	afx_msg void OnButtonWkMinus();
	afx_msg void OnButtonWkPlus();
	afx_msg void OnButtonNull();
	afx_msg void OnButtonNull2Minus();
	afx_msg void OnButtonNull2Plus();
	afx_msg void OnButtonLbtMinus();
	afx_msg void OnButtonLbtPlus();
	afx_msg void OnRadioArchive();
	afx_msg void OnRadioRealTime();
    afx_msg void OnRadioCement();
	afx_msg void OnRadioOtchet();
	afx_msg void OnButtonKnbk();
	afx_msg void OnStaticDeltaTB();
	afx_msg void OnSTATICWk();
	afx_msg void OnStaticNkbk();
	afx_msg void OnButtonStartStop();
	afx_msg LRESULT OnMyInitDialog(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#endif
