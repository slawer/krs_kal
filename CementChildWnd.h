#pragma once


// CCementChildWnd frame

class CCementChildWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CCementChildWnd)
protected:
	CCementChildWnd();           // protected constructor used by dynamic creation
	virtual ~CCementChildWnd();

public:
    // Overrides
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual void ActivateFrame(int nCmdShow = -1);
};


