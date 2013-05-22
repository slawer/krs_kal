#pragma once


// CDlgInterface dialog

class CDlgInterface : public CDialog
{
	DECLARE_DYNAMIC(CDlgInterface)

public:
	CDlgInterface(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInterface();

// Dialog Data
	enum { IDD = IDD_INTERFACE };
	int		m_flgViewMarkers;
	int		m_flgViewPanelBur;
	int		m_flgViewDiag;
	int     m_flgViewDgtRT;
	int     m_flgViewDgtBD;
	int     m_flgViewRealArch;
	int     m_flgViewTimeGlub;
	int		m_flgDataBegEnd;
	int		m_gflgGrfStupenki;
	int		m_gflgScrollAll;
	int		m_gflgIndParBur;
	int		m_gflgIndHighPanel;
	int		m_gflgDgtGraphPanel;
	int		m_gflgIndDataTime;
	int     m_gflgIndConverter;
	int     m_gflgIndRegimeBur;
	int     m_gflgSelectOnlyY;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckViewReper();
	afx_msg void OnCheckViewPanelBur();
	afx_msg void OnCheckViewDiag();
	afx_msg void OnCheckViewDgtRT();
	afx_msg void OnCheckViewDgtBD();
	afx_msg void OnCheckViewRealArch();
	afx_msg void OnCheckViewTimeGlub();
	afx_msg void OnCheckDataBegEnd();
	afx_msg void OnCheckGrfStupenki();
	afx_msg void OnCheckScrollAll();
	afx_msg void OnCheckParamBur();
	afx_msg void OnCheckHighPanel();
	afx_msg void OnCheckGraphDigit();
	afx_msg void OnCheckIndDataTime();
	afx_msg void OnCheckIndConverter();
	afx_msg void OnCheckIndRegimeBur();
	afx_msg void OnCheckSelectOnlyY();
public:
	afx_msg void OnBnClickedButtonColorInterface();
};
