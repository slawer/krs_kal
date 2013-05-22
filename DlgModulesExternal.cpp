// DlgModulesExternal.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "BaseDoc.h"
#include "DlgModulesExternal.h"


// CDlgModulesExternal dialog

IMPLEMENT_DYNAMIC(CDlgModulesExternal, CDialog)

CDlgModulesExternal::CDlgModulesExternal(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModulesExternal::IDD, pParent)
{
}

CDlgModulesExternal::~CDlgModulesExternal()
{
}

void CDlgModulesExternal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODULES, m_list_modules);
}


BEGIN_MESSAGE_MAP(CDlgModulesExternal, CDialog)
    ON_MESSAGE(PM_MENU_LIST_EXTERNAL, OnMenuListExternal)
	ON_BN_CLICKED(IDC_BUTTON_MODULE_DELETE, &CDlgModulesExternal::OnBnClickedButtonModuleDelete)
    ON_BN_CLICKED(IDOK, &CDlgModulesExternal::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgModulesExternal message handlers

LRESULT CDlgModulesExternal::OnMenuListExternal(WPARAM wParam, LPARAM lParam)
{
    BaseDoc* pDoc = (BaseDoc*)(pApp->GetActiveView()->GetDocument());

    if(pDoc->m_ListModuleExternal.GetCount() > 9) return true;
    int num_line = (wParam >> 8) & 0xFF;
    int num_col = wParam & 0xFF;
    BOOL flg_new = false;
    CString sName, sName1;
    int num;

    if(num_line < 0xFF)
    {
        sName = m_list_modules.GetItemText(num_line, 2);
        num = sName.ReverseFind('\\');
        sName = sName.Left(num);
    }
    else
    {
        num_line = m_list_modules.GetItemCount();
        flg_new = true;
    }

    CFileDialog dlgFile(
        TRUE,
        _T(".exe"),
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T("Access Files (*.exe)|*.exe|All Files (*.*)|*.*||"));
    dlgFile.m_ofn.lpstrTitle = "Выбрать путь к модулю";
    dlgFile.m_ofn.lpstrInitialDir = sName;

    char str[1024];
    GetCurrentDirectory(1024, str);
    if(dlgFile.DoModal() == IDOK) 
    {
        sName = dlgFile.m_ofn.lpstrFile;

		CString num_str;
		int num_elements = m_list_modules.GetItemCount();
		num_str.Format("%d", num_elements + 1);
		int index = m_list_modules.InsertItem(num_elements, num_str);
		sName1 = sName;
		num = sName1.ReverseFind('\\');
		sName1.Delete(0, num+1);
		num = sName1.ReverseFind('.');
		sName1 = sName1.Left(num);
		m_list_modules.SetItemText(index, 1, sName1);
		m_list_modules.SetItemText(index, 2, sName);
		ChangeList();
    }
    SetCurrentDirectory(str);
    return true;
}

void CDlgModulesExternal::RebuildList()
{
    BaseDoc* pDoc = (BaseDoc*)(pApp->GetActiveView()->GetDocument());

    m_list_modules.DeleteAllItems();

    CString cs1;
    for(int i = 0; i < pDoc->m_ListModuleExternal.GetCount(); i++)
    {
        int col = 1; 
        cs1.Format("%d", i + 1);
        int index = m_list_modules.InsertItem(i, cs1);
        m_list_modules.SetItemText(index, col++, pDoc->m_ListModuleExternal.GetName(i));
        m_list_modules.SetItemText(index, col++, pDoc->m_ListModuleExternal.GetName2(i));
    }
}
BOOL CDlgModulesExternal::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(false);

    m_list_modules.m_lb_dbl_click_msg = PM_MENU_LIST_EXTERNAL;
    m_list_modules.m_messages_wnd = this;
    m_list_modules.SetColStyle(1, 1);
    m_list_modules.SetColStyle(2, 1);
    m_list_modules.SetListFont(GetFont());
    m_list_modules.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    CRect rec;
    m_list_modules.GetClientRect(&rec);

    int w0 = 25;
    int w1 = (rec.Width() - w0)/4;
    int w2 = rec.Width() - w0 - w1;
    m_list_modules.InsertColumn(0,"Путь",LVCFMT_CENTER,w2, 2);
    m_list_modules.InsertColumn(0,"Название",LVCFMT_CENTER,w1, 1);
    m_list_modules.InsertColumn(0,"№",LVCFMT_CENTER,w0, 0);

    RebuildList();
	return TRUE; 
}

void CDlgModulesExternal::OnBnClickedButtonModuleDelete()
{
    int nn = m_list_modules.GetCurItem();
	if(nn < 0) return;
	m_list_modules.DeleteItem(nn);
	ChangeList();
}

void CDlgModulesExternal::ChangeList()
{
    BaseDoc* pDoc = (BaseDoc*)(pApp->GetActiveView()->GetDocument());

	pDoc->m_ListModuleExternal.Init();
	m_list_modules.GetItemCount();
	for(int i = 0; i < m_list_modules.GetItemCount(); i++)
	{
		pDoc->m_ListModuleExternal.Add(m_list_modules.GetItemText(i, 1), m_list_modules.GetItemText(i, 2), 0);
	}
}

void CDlgModulesExternal::OnBnClickedOk()
{
	ChangeList();
    OnOK();
}
