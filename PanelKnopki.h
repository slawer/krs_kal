#pragma once

#define PM_KNOPKI_PANEL   	    WM_USER + 4000

typedef CMap<int, int, CButton*, CButton*&> MapButtons;
// CPanelKnopki

class CPanelKnopki : public CStatic
{
	DECLARE_DYNAMIC(CPanelKnopki)

	CWnd* m_pWndMsg;
public:
    int GetCount();
    CSize GetSize();
    void DeleteAllElements();
    CButton* Add(int num_btn, CRect& rec_btn, int ID);
    CSize Rebuild(int count_btn, CSize sz_btn, BOOL flg_create);
    CSize Init(int count_btn, CSize sz_btn, CWnd *pWndMsg);
    CSize RepositionBtn();
	void PushBtn(int num_btn);

    MapButtons m_list_btn;
    int knp_num;//, btn_active;
    CSize m_sz_panel, m_sz_btn;
    CFont m_btn_font;

public:
	CPanelKnopki();
	virtual ~CPanelKnopki();
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};


