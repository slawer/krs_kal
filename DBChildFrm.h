#pragma once


// CDBChildFrm frame

class CDBChildFrm : public CMDIChildWnd
{
    DECLARE_DYNCREATE(CDBChildFrm)
protected:
    CDBChildFrm();
	virtual ~CDBChildFrm();

public:
// Overrides
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    DECLARE_MESSAGE_MAP()
public:
	virtual void ActivateFrame(int nCmdShow = -1);
};


