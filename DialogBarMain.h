#pragma once


// CDialogBarMain

class CDialogBarMain : public CDialogBar
{
	DECLARE_DYNAMIC(CDialogBarMain)

    CPoint m_PointOld;
    CRect m_recBar;
    BOOL m_bFlagDown, m_bSplitCurs, m_bInflateBrder;
    HCURSOR m_CurNorm, m_CurHSpl, m_CurVSpl, m_oldCursor, m_curCursor;
    CRect m_rec_s;

    void SetCurCurssor(CPoint point);
    void SetSplitCurssor();
    void SetNormCurssor();
    void OnInvertTracker(CPoint point);
    void SetInitSize(LPVOID pFram);

public:
	CDialogBarMain();
	virtual ~CDialogBarMain();
    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
    virtual void ResizeOtherBars(){};
    virtual void Serialize(CArchive& ar);

protected:
	DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


