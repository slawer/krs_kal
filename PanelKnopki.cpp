// PanelKnopki.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "PanelKnopki.h"


#define MAX_LINE 20

IMPLEMENT_DYNAMIC(CPanelKnopki, CStatic)

CPanelKnopki::CPanelKnopki()
{
    knp_num = /*btn_active = */0;
    m_sz_panel = CSize(0,0);
    m_sz_btn = CSize(20,20);

    LOGFONT  logfontN = {
        -11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 
        OUT_STRING_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
        VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
    };

	if(m_btn_font.m_hObject) m_btn_font.DeleteObject();
    m_btn_font.CreateFontIndirect(&logfontN);
	m_pWndMsg = NULL;
}

CPanelKnopki::~CPanelKnopki()
{
    DeleteAllElements();
}

BEGIN_MESSAGE_MAP(CPanelKnopki, CStatic)
END_MESSAGE_MAP()



BOOL CPanelKnopki::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(m_pWndMsg != NULL)
	{
		int nn = nID - IDC_PANEL_KNOPKI - 1;
		{
			m_pWndMsg->PostMessage(PM_CHANGE_ACTIVE_LIST, nn, NULL);
		}
	}
	return CStatic::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

int CPanelKnopki::GetCount()
{
    return knp_num;
}

CSize CPanelKnopki::GetSize()
{
    return m_sz_panel;
}

void CPanelKnopki::DeleteAllElements()
{
    if(m_list_btn.GetCount() == 0) return; 
    int mKey;
    CButton *btn;
    POSITION pos = m_list_btn.GetStartPosition();
    while (pos != NULL)
    {
        m_list_btn.GetNextAssoc(pos, mKey, btn);
        if(btn != NULL) delete btn;
    }
    m_list_btn.RemoveAll();
}

CButton* CPanelKnopki::Add(int num_btn, CRect& rec_btn, int ID)
{
    CString str;
    str.Format("%d", num_btn+1);
    CButton *btn = new CButton;
    DWORD style = WS_CHILD|WS_VISIBLE|BS_PUSHLIKE|BS_AUTORADIOBUTTON|BS_CENTER|WS_TABSTOP;
    if (num_btn == 0)
        style |= WS_GROUP;
    btn->Create(str, style, rec_btn, this, ID);
    btn->SetFont(&m_btn_font);
    m_list_btn[m_list_btn.GetCount()] = btn;
    return btn;
}

CSize CPanelKnopki::Rebuild(int count_btn, CSize sz_btn, BOOL flg_create)
{
    if(flg_create) 
		DeleteAllElements();

    CRect rec, rec_btn;
    GetClientRect(&rec);
    const int c_slot = 1;
    int max_row = (rec.Width() - c_slot)/(sz_btn.cx + c_slot);
    if (max_row == 0) max_row = 1;

    int num_line = 0, num_row = 0;
    CButton *btn;
    for(int i = 0; i < count_btn; i++)
    {
        num_line = i/max_row;
        num_row = i - num_line * max_row;
        rec_btn = CRect(CPoint(num_row*(sz_btn.cx + c_slot) + c_slot, num_line*(sz_btn.cy + c_slot) + c_slot), sz_btn);  
        if(flg_create) btn = Add(i, rec_btn, IDC_PANEL_KNOPKI + i + 1);
        else 
        {
            btn = m_list_btn[i];
            if(btn != NULL) 
                btn->SetWindowPos(NULL, rec_btn.left, rec_btn.top, rec_btn.Width(), 
                rec_btn.Height(), SWP_NOZORDER);
        }
        if(btn != NULL) btn->ShowWindow(((num_line > MAX_LINE)?SW_HIDE:SW_SHOW));
    }
    knp_num = count_btn;
    m_sz_panel = 
        CSize((num_row + c_slot)*(sz_btn.cx + c_slot) + c_slot, (num_line + c_slot)*(sz_btn.cy + c_slot) + c_slot);
    return m_sz_panel;
}

CSize CPanelKnopki::Init(int count_btn, CSize sz_btn, CWnd *pWndMsg)
{
    if(m_hWnd == NULL) return CSize(0,0);
	m_pWndMsg = pWndMsg;
    CSize sz = Rebuild(count_btn, sz_btn, true);
    return sz;
}

CSize CPanelKnopki::RepositionBtn()
{
    if(m_hWnd == NULL || m_list_btn.GetCount() == 0) return CSize(0,0);
    CSize sz = Rebuild(knp_num, m_sz_btn, false);
    return sz;
}

void CPanelKnopki::PushBtn(int num_btn)
{
	if(m_hWnd == NULL || m_list_btn.GetCount() == 0 || num_btn > m_list_btn.GetCount()) return;
	CheckRadioButton(IDC_PANEL_KNOPKI + 1, IDC_PANEL_KNOPKI + m_list_btn.GetCount(), IDC_PANEL_KNOPKI + 1 + num_btn);
}