#pragma once

#include "krs.h"
#define HEIGHT_HEADER 100
extern PrinterParam prn;

typedef CTypedPtrArray<CObArray, CPanelWnd*> PanelArray;

class ParamOfView : public CStatic 
{
    CWnd* m_pParentForDgtPanel;
    CWnd* m_pParentForGrf;
    CParamMap* m_MapParam;

public:
    int num_grf_panel, num_dgt_panel;
    CParamMap* GetMapParam() { return m_MapParam; }
    PanelArray panArray;
    int m_active_grf[G_MAX_FIELDS_GRAPHS];
    int m_active_dgt[G_MAX_FIELDS_PANELS];
    float gTimeScale, gGlubScale, gTimeScalePrev, gGlubScalePrev;
    double begTime, begGlub, endTime, endGlub, gDeltaTime, gDeltaH;
    BOOL flg_nonstandart_scale;
	BOOL m_flag_orient;

    ParamOfView(int n_grf_panel, int n_dgt_panel)
    {
		m_pParentForDgtPanel = NULL;
		m_pParentForGrf = NULL;
		m_MapParam = NULL;

		num_grf_panel = n_grf_panel <= G_MAX_FIELDS_GRAPHS ? n_grf_panel : G_MAX_FIELDS_GRAPHS;
        num_dgt_panel = n_dgt_panel <= G_MAX_FIELDS_PANELS ? n_dgt_panel : G_MAX_FIELDS_PANELS;
        for(int i = 0; i < num_grf_panel; i++) m_active_grf[i] = 1;
        for(int i = 0; i < num_dgt_panel; i++) m_active_dgt[i] = 1;

        begGlub = 0;
        gTimeScale = 18000.0f, gGlubScale = 10.0f;
        gDeltaTime = 0.70820138033013791, gDeltaH = 2.1246040475089103;
        endGlub = begGlub + gDeltaH;
        gTimeScalePrev = gTimeScale;
        gGlubScalePrev = gGlubScale;
        flg_nonstandart_scale = false;

        m_pParentForDgtPanel = NULL;
        m_pParentForGrf = NULL;
		m_flag_orient = true;
    }

    void Create(CWnd* pParentForDgtPanel, 
        CWnd* pParentForGrf, LPVOID pBufX, CParamMap* m_mapPrm, typeView t_v)
    {
        int i;
        m_pParentForDgtPanel = pParentForDgtPanel;
        m_pParentForGrf = pParentForGrf;

//        panArray.RemoveAll();
		DeleteAllPanel();

//return;
        CRect recPanel(0,0,30,30);
        CPanelWnd *pPanel;
        for(i = 0; i < num_grf_panel; i++)
        {
            pPanel = new CPanelWnd;
            pPanel->m_pParentForPanel = pParentForDgtPanel;
            pPanel->m_pParentForGraph = pParentForGrf;
            pPanel->m_pBufX = pBufX;
            pPanel->m_MapParam = m_mapPrm;
            pPanel->t_view = t_v;
			pPanel->tp_panel = tp_grf;
            pPanel->m_flag_orient = m_flag_orient;
            panArray.Add(pPanel);
            pPanel->num_panel = panArray.GetSize() - 1;
            pPanel->Create(NULL, WS_CHILD|SS_WHITERECT, recPanel, pParentForGrf);
        }
        for(i = num_grf_panel; i < num_grf_panel + num_dgt_panel; i++)
        {
            pPanel = new CPanelWnd;
            pPanel->m_pParentForPanel = pParentForDgtPanel;
            pPanel->m_pParentForGraph = 0;
            pPanel->m_pBufX = 0;
            pPanel->m_MapParam = m_mapPrm;
			pPanel->t_view = t_v;
			pPanel->tp_panel = tp_dgt;
            panArray.Add(pPanel);
            pPanel->num_panel = panArray.GetSize() - 1;
            pPanel->m_flag_visible_field = true;
            pPanel->Create(NULL, WS_CHILD, recPanel, pParentForDgtPanel);
        }
        SetVisibilityFlagActiveList();
    }

	void ClearParams(int num_panel)
	{
		if (num_panel < 0 || num_panel >= panArray.GetSize()) return;
		panArray[num_panel]->ClearParams();
	}
	
	void SetupPanel(int num_panel, PanelDescription *panel_descr)
	{
		ClearParams(num_panel);
		SetNamePanel(num_panel, panel_descr->snamePanel);
		ParamInPanel pd;
		list<ParamInPanel>::iterator current_pd = panel_descr->m_params.begin();
		while (current_pd != panel_descr->m_params.end())
		{
			pd = *current_pd;
			AddParam(num_panel, &pd);
			current_pd++;
		}
	}
	
    void SetOrientPanelGrf(BOOL flag_orient)
    {
		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < num_grf_panel; i++)
        {
            panArray[i]->m_flag_orient = m_flag_orient;
        }
    }

    BOOL GetInfoPanel(int num_panel, PanelDescription *plstParam)
    {
        if(num_panel < 0 || num_panel >= panArray.GetSize()) return false;
        return panArray[num_panel]->GetInfoPanel(plstParam);
    }

    int GetNumPanel()
    {
        return panArray.GetSize();
    }
   void RedrawPanel(int num_panel, typePanel tp_p)
    {
        if(num_panel < 0 || num_panel >= panArray.GetSize()) return;
        if(tp_p == tp_grf)	panArray[num_panel]->RedrawPanel(tp_p);
        else if(tp_p == tp_dgt) panArray[num_grf_panel + num_panel]->RedrawPanel(tp_p);
    }

    void RedrawActiveList()
    {
		if(panArray.GetSize() == 0) return;
		for(int i = 0; i < num_grf_panel; i++) RedrawPanel(i, tp_grf);
        for(int i = 0; i < num_dgt_panel; i++) RedrawPanel(i, tp_dgt);
    }
    void ShowPanel(int num_panel, typePanel tp_p, BOOL flagShow)
    {
        if(num_panel < 0 || num_panel >= panArray.GetSize()) return;
        if(tp_p == tp_grf) panArray[num_panel]->ShowPanel(flagShow);
        else if(tp_p == tp_dgt) panArray[num_grf_panel + num_panel]->ShowPanel(flagShow);
    }
    void ShowDigitalPanels(BOOL flagShow)
    {
		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < num_dgt_panel; i++) ShowPanel(i, tp_dgt, flagShow);
    }
	void ShowGrfPanels(BOOL flagShow)
	{
		if(panArray.GetSize() == 0) return;
		for(int i = 0; i < num_grf_panel; i++) ShowPanel(i, tp_grf, flagShow);
	}
    void ShowPanelAll(BOOL flagShow)
    {
		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < num_grf_panel; i++) ShowPanel(i, tp_grf, flagShow);
        for(int i = 0; i < num_dgt_panel; i++) ShowPanel(i, tp_dgt, flagShow);
    }

    void SetVisibilityFlag(int num_panel, typePanel tp_p)
    {
        if(num_panel < 0 || num_panel >= panArray.GetSize()) return;
        if(tp_p == tp_grf)	panArray[num_panel]->SetVisibilityFlag(m_active_grf[num_panel]);
        else if(tp_p == tp_dgt) panArray[num_grf_panel + num_panel]->SetVisibilityFlag(m_active_dgt[num_panel]);
    }

    void SetVisibilityFlagDgt(BOOL flagShow)
    {
        for(int i = 0; i < num_dgt_panel; i++) 
        {
            m_active_dgt[i] = flagShow;
            panArray[num_grf_panel + i]->SetVisibilityFlag(flagShow);
        }
    }
    void SetVisibilityFlagAll()
    {
		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < num_grf_panel; i++) SetVisibilityFlag(i, tp_grf);
        for(int i = 0; i < num_dgt_panel; i++) SetVisibilityFlag(i, tp_dgt);
    }

    BOOL GetVisible(int num_panel, typePanel tp_p)
    {
        if(num_panel < 0 || num_panel >= panArray.GetSize()) return false;
        if(tp_p == tp_grf)	return panArray[num_panel]->GetVisible();
        else if(tp_p == tp_dgt) return panArray[num_grf_panel + num_panel]->GetVisible();
    }
    void ShowActiveList(BOOL flagShow)
    {
        ShowPanelAll(flagShow);
    }
    void SetVisibilityFlagActiveList()
    {
        SetVisibilityFlagAll();
    }

    void UpdateRectRangeAll(int n_grf_panel)
    {
        for(int i = 0; i < num_grf_panel; i++) panArray[i]->UpdateRectRange(n_grf_panel, true);
    }
    void UpdateScrollRangeAll(int n_grf_panel, int num_scroll)
    {
        for(int i = 0; i < num_grf_panel; i++) panArray[i]->UpdateScrollRange(n_grf_panel, num_scroll);
    }

    BOOL GetNumVisible(typePanel tp_p)
    {
        if(panArray.GetCount() == 0) return false;
        int i;
        int nn = 0;

        if(tp_p == tp_grf)
        {
            for(i = 0; i < num_grf_panel; i++)
            {
                if(panArray[i]->GetVisible()) nn++;
            }
        }
        else if(tp_p == tp_dgt)
        {
            for(i = num_grf_panel; i < num_grf_panel + num_dgt_panel; i++)
            {
                if(panArray[i]->GetVisible()) nn++;
            }
        }
        return nn;
    }

    void RecalcFieldSize(typeView t_v)
    {
		RecalcPanelsGrf(t_v);
		RecalcPanelsDgt(t_v);
    }

	int GetLengthFieldGrf()
	{
		if(m_pParentForGrf == NULL || m_pParentForGrf->m_hWnd == NULL) 
			return 0;
		CRect rec;
		int i = GetFirstNotEmptyPanel();
		if (panArray.GetSize() == 0 || panArray[0]->m_hWnd == NULL || i == -1 || !panArray[i]->GetRectFieldGrf(rec)) 
			return 0;
		if(m_flag_orient) return rec.Height();
		return rec.Width();
	}

	void RecalcPanelsGrf(typeView t_v)
	{
		if(m_pParentForGrf == NULL || m_pParentForGrf->m_hWnd == NULL) return;
		SetOrientPanelGrf(m_flag_orient);

		int slit_header = 2;
		CGraph* pFld;
		if(m_flag_orient)
		{
			int i, nn, h_scale;

			nn = GetNumVisible(tp_grf);
			if(nn == 0 || nn < 0 || nn > num_grf_panel) return;

			CRect recM;
			int header_start_y = 0;
			if(prn.pDC && prn.pDC->IsPrinting())
			{
				recM = prn.rec_prn;
				m_pParentForGrf->ScreenToClient(&recM);
				h_scale = (HEIGHT_HEADER*prn.k_y)*int(gflgDgtGraphPanel);
				header_start_y = recM.top;
				slit_header = 4*prn.k_y;
			}
			else
			{
				m_pParentForGrf->GetClientRect(&recM);
				h_scale = HEIGHT_HEADER*int(gflgDgtGraphPanel);
				header_start_y = 0;
				slit_header = 2;
			}

			if(recM.Width() == 0) return;
			recM.top += h_scale;

			int w_fld = recM.Width()/nn;

			int kk = 0, dw;
			for(i = 0; i < num_grf_panel; i++)
			{
				if(!panArray[i]->GetVisible()) continue;

				pFld = &panArray[i]->m_parGraph;
				if(pFld->m_hWnd == 0) return;

				if(i == (num_grf_panel - 1)) dw = recM.Width() - w_fld*nn;
				else dw = 0;

				CRect rec4 = CRect(CPoint(recM.left + kk*w_fld, recM.top+slit_header), CSize(w_fld + dw, recM.Height()));
				if(pFld->m_hWnd != NULL) pFld->SetWindowPos(&wndTop, rec4.left, rec4.top, rec4.Width(), rec4.Height(), SWP_HIDEWINDOW);
				if(panArray[i]->m_hWnd != NULL) panArray[i]->SetWindowPos(&wndTop, recM.left + kk*w_fld, header_start_y, w_fld + dw, 
					h_scale, 
					SWP_HIDEWINDOW);
				kk++;
			}
		}
		else
		{
			int i, nn, w_scale;

			nn = GetNumVisible(tp_grf);
			if(nn == 0 || nn < 0 || nn > num_grf_panel) return;

			int ph = 0;
			CRect recM;
			int header_start_x = 0;
			if(prn.pDC && prn.pDC->IsPrinting())
			{
				recM = prn.rec_prn;
				m_pParentForGrf->ScreenToClient(&recM);
				w_scale = (HEIGHT_HEADER*prn.k_x)*int(gflgDgtGraphPanel);
				header_start_x = recM.left;
				slit_header = 4*prn.k_x;
			}
			else
			{
				w_scale = HEIGHT_HEADER*int(gflgDgtGraphPanel);
				m_pParentForGrf->GetClientRect(&recM);
				header_start_x = 0;
				slit_header = 2;
			}

			if(recM.Height() == 0) return;
			recM.left += w_scale;

			int h_fld = recM.Height()/nn;

			int kk = 0, dh;
			for(i = 0; i < num_grf_panel; i++)
			{
				if(!panArray[i]->GetVisible()) continue;

				pFld = &panArray[i]->m_parGraph;
				if(pFld->m_hWnd == 0) return;

				if(i == (num_grf_panel - 1)) dh = recM.Height() - h_fld*nn;
				else dh = 0;

				if(pFld->m_hWnd != NULL) pFld->SetWindowPos(&wndTop, recM.left+slit_header, recM.top + kk*h_fld, recM.Width(), h_fld + dh, SWP_HIDEWINDOW);
				if(panArray[i]->m_hWnd != NULL) panArray[i]->SetWindowPos(&wndTop, header_start_x, recM.top + kk*h_fld, w_scale, h_fld + dh, SWP_HIDEWINDOW);
				kk++;
				pFld->UpdateGraph();
			}
		}
	}
    void RecalcPanelsDgt(typeView t_v)
    {
        int nn = GetNumVisible(tp_dgt);
        if(nn == 0 || nn < 0 || nn > num_grf_panel) return;

        CRect recM;
        m_pParentForGrf->GetClientRect(&recM);
        if(recM.Width() == 0) return;

        m_pParentForDgtPanel->GetClientRect(&recM);
        if(recM.Width() == 0) return;

        int w_fld = recM.Width();

        CSize szt;
        int splitPanel = 10;

        int dd = 1;
        if(t_v == t_real_time || t_v == gt_cement) dd = 0;
        int hh = 22 * (gflgIndRegimeBur && gflgViewPanelBur) + dd * 310 + gflgIndDataTime * 32;
        int i;
        for(i = num_grf_panel; i < nn + num_grf_panel; i++)
        {
            szt = panArray[i]->GetSizeDgtPanel();
            szt.cy += 0;
            hh +=  splitPanel;

            if(panArray[i]->m_hWnd != NULL) panArray[i]->SetWindowPos(&wndTop, 0, hh, w_fld, szt.cy, SWP_HIDEWINDOW);

            hh +=  szt.cy;
        }

        for(i = num_grf_panel; i < nn + num_grf_panel; i++) panArray[i]->RecalcFieldSize();

        m_pParentForDgtPanel->Invalidate(true);
    }

    void DrawGrf(CDC *pDC, CWnd *pWnd, float printScaleX, float printScaleY, 
        CSize *printShift)
    {
        int i, nn;

        nn = GetNumVisible(tp_grf);
        if(nn < 0 || nn > num_grf_panel) return;

        for(i = 0; i < num_grf_panel; i++)
            if(m_active_grf[i]) panArray[i]->PrintGrf(pDC, pWnd, printScaleX, printScaleY, printShift);
    }

    void LockDataForPrint(int lock_value)
    {
        int i, nn;

        nn = GetNumVisible(tp_grf);
        if(nn < 0 || nn > num_grf_panel) return;

        for(i = 0; i < nn; i++)
            panArray[i]->LockDataForPrint(lock_value);
    }

    void InitPanelForDataDB(COleDateTime begTime, COleDateTime endTime, CString textGrf)
    {
        int i, nn;

        nn = GetNumVisible(tp_grf);
        if(nn < 0 || nn > num_grf_panel) return;

        for(i = 0; i < nn; i++)
            panArray[i]->InitPanelForDataDB(begTime, endTime, textGrf);
    }

    int AddActiveParam(CParamMap *pActive)
    {
        for(int i = 0; i < num_grf_panel; i++) 
            panArray[i]->AddActiveParam(pActive);

        return pActive->GetCount();
    }
    //------------------------------------------------------------------------
    //	int AddParam(int num_panel, int nKey, BOOL flg_redraw = false) 
    int AddParam(int num_panel, ParamInPanel* pd, BOOL flg_redraw = false) 
    {
        if (num_panel < 0 || num_panel >= panArray.GetSize()) return -1;
        //return panArray[num_panel]->AddParam(nKey, flg_redraw);
        return panArray[num_panel]->AddParam(pd, flg_redraw);
    }

    CString GetNamePanel(int num_panel)
    {
        if(num_panel < 0 || num_panel >= panArray.GetSize()) return "";
        return panArray[num_panel]->sNamePanel;
    }

    void SetNamePanel(int num_panel, CString name)
    {
        if(num_panel < 0 || num_panel >= panArray.GetSize()) return;
        panArray[num_panel]->sNamePanel = name;
    }

    //------------------------------------------------------------------------
    void DeleteAllPanel()
    {
		int nn = panArray.GetSize();
        for(int i = 0; i < nn; i++)
		{
			panArray[i]->DestroyWindow();
			delete panArray[i];
		}
        panArray.RemoveAll();
    }

    void ChangeGraphColor()
    {
		int nn = panArray.GetSize();
		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < num_grf_panel; i++) panArray[i]->ChangeGraphColor();
    }

    void DeleteExtParam(int num)
    {
        for (int i = 0; i < panArray.GetSize(); i++) panArray[i]->DeleteParamByNum(num);
    }

    ~ParamOfView()
    {
        DeleteAllPanel();
    }

    //------------------------------------------------------------------------
    void SetScaleTime(LPVOID pBufX, 
        double m_ScaleXStart, double m_ScaleXEnd, BOOL m_flagTimeGlub, CString textGrf) 
    {
		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < num_grf_panel; i++) 
            panArray[i]->SetScaleTime(pBufX, m_ScaleXStart, m_ScaleXEnd, m_flagTimeGlub, textGrf);
    } 

    void SetScaleXDiapazon(LPVOID pBufX, 
        double m_ScaleXStart, double m_ScaleXEnd, BOOL m_flagTimeGlub, CString textGrf) 
    {
        //		for(int i = 0; i < panArray.GetSize(); i++) 
		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < num_grf_panel; i++) 
            panArray[i]->SetScaleXDiapazon(pBufX, m_ScaleXStart, m_ScaleXEnd, m_flagTimeGlub, textGrf);
    } 

    void SetTextAllGrfPanel(CString textGrf) 
    {
		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < num_grf_panel; i++) 
            panArray[i]->SetTextGrfPanel(textGrf, false);
    } 

    void SetTextGrfPanel(int num_panel, CString textGrf) 
    {
		if(panArray.GetSize() == 0) return;
        panArray[num_panel]->SetTextGrfPanel(textGrf);
    } 

    void UpdateReper(BOOL flg) 
    {
		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < num_grf_panel; i++) 
            panArray[i]->UpdateReper(flg);
    } 

    void ChangeScale(LPVOID pBufX, GRF_MINMAX& par, BOOL m_flagTimeGlub)
    {

		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < num_grf_panel; i++) 
            panArray[i]->ChangeScale(pBufX, par, m_flagTimeGlub);
    } 

    void UpdateInputData(int fl_update_grf)
    {
		if(panArray.GetSize() == 0) return;
        for(int i = 0; i < panArray.GetSize(); i++) 
            if(::SendMessage(panArray[i]->m_hWnd, PM_DATA_CHANGED, (int)fl_update_grf, NULL)) break;
    }

    void UpdateInputDataDB()
    {
        int nn = panArray.GetSize();
        for(int i = 0; i < panArray.GetSize(); i++) 
            ::PostMessage(panArray[i]->m_hWnd, PM_DATA_DB_CHANGED, 0, NULL);
    }
    void SetStyleGrf(ULONG64 style)
    {
        for(int i = 0; i < panArray.GetSize(); i++) 
            ::PostMessage(panArray[i]->m_hWnd, PM_SET_STYLE, WPARAM(style>>32), LPARAM(style));
    }
    void ResetStyleGrf(ULONG64 style)
    {
        for(int i = 0; i < panArray.GetSize(); i++) 
            ::PostMessage(panArray[i]->m_hWnd, PM_RESET_STYLE, WPARAM(style>>32), LPARAM(style));
    }
    void SetStyleGrfInstant(ULONG64 style, bool reset)
    {
        for(int i = 0; i < panArray.GetSize(); i++) 
            ::SendMessage(panArray[i]->m_hWnd, reset?PM_RESET_STYLE:PM_SET_STYLE, WPARAM(style>>32), LPARAM(style));
    }
    ULONG64 GetStyleGrf()
    {
        for(int i = 0; i < panArray.GetSize(); i++) panArray[i]->GetStyleGrf();
    }

    template <class type>
    void SubstituteBitStyleAllGrf(ULONG64 dwStyle, type value)
    {
        for(int i = 0; i < panArray.GetSize(); i++) panArray[i]->SubstituteBitStyleGrf(dwStyle, value);
    }
    template <class type>
    void SubstituteBitStyleGrf(int num_panel, ULONG64 dwStyle, type value, BOOL flRedraw)
    {
        panArray[num_panel]->SubstituteBitStyleGrf(dwStyle, value, flRedraw);
    }

	int GetFirstNotEmptyPanel()
	{
		int i = 0;
		while(panArray[i]->m_parGraph.FieldGraph.DATA.GetNumBuffers() == 0) 
		{
			i++;
			if (i >= num_grf_panel)
				return -1; //??
		}
		return i;
	}

    void GetMinMaxX(double& minf, double& maxf)
    {
		int i = GetFirstNotEmptyPanel();
		if (i == -1)
			return;
        panArray[i]->GetMinMaxX(minf, maxf);
    }

    UINT GetStartMarkerX(int num_panel)
    {
        if(num_panel < 0 || num_panel >= panArray.GetSize()) return false;
        return panArray[num_panel]->GetStartMarkerX();
    }

    void SynchronizeGraph(int num_panel)
    {
        if(num_panel < 0 || num_panel >= num_grf_panel) return;
        double min_x, max_x;
        ASSERT(panArray[num_panel]);
        panArray[num_panel]->GetMinMaxX(min_x, max_x);
        for(int i = 0; i < num_grf_panel; i++) 
            if(i != num_panel) panArray[i]->SynchronizeGraph(min_x, max_x);
    }
	VFRingBuf<double>* GetVXBuffer(int num_panel)
	{
//		if(num_panel < 0 || num_panel >= num_grf_panel) return NULL;
		if(num_panel < 0 || num_panel >= num_grf_panel || panArray.GetCount() == 0) return NULL;
		ASSERT(panArray[num_panel]);
		return panArray[num_panel]->GetVXBuffer();
	}
	void SetUMarkerPos(int marker_num, UINT pos)
	{
		if(panArray.GetCount() == 0) return;
		for(int i = 0; i < num_grf_panel; i++) 
		{
			panArray[i]->m_parGraph.FieldGraph.DATA.SetPosUMarker(-1, marker_num, pos);
		}
	}

	void ClearBeginDataY(int num_point_for_delete)
	{
		if(panArray.GetSize() == 0) return;
		for(int i = 0; i < num_grf_panel; i++) 
			panArray[i]->m_parGraph.ClearBeginDataY(num_point_for_delete);
	}
	void ResetFlagsClearedY()
	{
		if(panArray.GetSize() == 0) return;
		for(int i = 0; i < num_grf_panel; i++) 
			panArray[i]->m_parGraph.ResetFlagsClearedY();
	}

};

class CKRSView;

class CDataView : public CObject
{
	ParamOfView**	parListView;
	int	num_active_list;
public:
    int	num_lists;
    BOOL flag_time_glub;
    double scale_x_start, scale_x_end;
    CString name_type_view;

	int m_num_grf_panel, m_num_dgt_panel;
	
    CDataView(int n_lists, int n_grf_panel, int n_dgt_panel):
        parListView(NULL), m_num_grf_panel(n_grf_panel), m_num_dgt_panel(n_dgt_panel)
    {
        num_lists = n_lists <= G_MAX_VIEWS ? n_lists : G_MAX_VIEWS;
        if (num_lists > 0)
        {
            parListView = new ParamOfView*[num_lists];
            for(int i = 0; i < num_lists; i++) parListView[i] = new ParamOfView(m_num_grf_panel, m_num_dgt_panel);
        }
        SetNumActiveList(0);
        flag_time_glub = true;
        COleDateTime stm = COleDateTime::GetCurrentTime();	
        scale_x_start = stm.m_dt;	
        //????        scale_x_end = scale_x_start + CKRSView::GetLenX();
    }

    ~CDataView()
    {
        if (parListView)
        {
            for(int i = 0; i < num_lists; i++) delete parListView[i];
            delete parListView;
        }
    }

    ParamOfView* GetActive()
    {
        if(parListView == NULL) return NULL;
        if(num_active_list < 0 || num_active_list >= num_lists) num_active_list = 0;
        return parListView[num_active_list];
    }

	void SwapLists(int num_list1, int num_list2)
	{
		if(parListView == NULL || num_list1 < 0 || num_list1 > num_lists-1 || num_list2 < 0 || num_list2 > num_lists-1 || num_list1 == num_list2)
			return;
		ParamOfView* tmp = parListView[num_list1];
		parListView[num_list1] = parListView[num_list2];
		parListView[num_list2] = tmp;
	}

	void AddList()
	{
		ParamOfView** new_pov = new ParamOfView*[num_lists+1], **tmp;
		memcpy(new_pov, parListView, sizeof(ParamOfView*)*num_lists);
		new_pov[num_lists] = new ParamOfView(m_num_grf_panel, m_num_dgt_panel);
		tmp = parListView;
		parListView = new_pov;
		num_lists++;
		delete tmp;
	}

	void DeleteList(int num_list)
	{
		if(parListView == NULL || num_list < 0 || num_list > num_lists-1)
			return;

		num_lists--;
		if (num_lists > 0)
		{
			ParamOfView** new_pov = new ParamOfView*[num_lists], **tmp;
			int add = 0;
			for (int i = 0; i < num_lists + 1; i++)
			{
				if (i == num_list)
				{
					delete parListView[i];
					add = 1;
				}
				else
					new_pov[i-add] = parListView[i];
			}
			tmp = parListView;
			parListView = new_pov;
			delete tmp;
			if (num_active_list > num_list || num_active_list == num_lists)
				num_active_list--;
		}
		else
		{
			delete parListView[0];
			parListView = NULL;
			num_active_list = 0;
		}
	}

	ParamOfView* GetList(int n_lst)
	{
		if(parListView == NULL) return NULL;
		if(n_lst < 0 || n_lst > num_lists-1)
			return NULL;
		return parListView[n_lst];
	}

    void RecalcScale(CDC *pDC, CRect& rec)
    {
        ParamOfView *pV = GetActive();
        if (pV == NULL)
            return;
        float m_ScaleFactor, m_ScaleFactorG;

        pV->gDeltaTime = m_ScaleFactor*rec.Height();
        pV->gDeltaH = m_ScaleFactorG*rec.Height();
    }

	int GetNumActiveList()
	{
		return num_active_list;
	}
	void SetNumActiveList(int n_list)
	{
		num_active_list = n_list;
	}
	int GetNumLists()
	{
		return num_lists;
	}

};
