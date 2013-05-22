#if !defined(_RING_BUFF)
#define _RING_BUFF

#include <afxtempl.h>
#include <math.h>

#define MAX_BUF_AVG 3600
template <class T>
void inline SwapData(T& t1, T& t2, BOOL flSwap)
{
    if(!flSwap) return;
    T tt = t1;
    t1 = t2;
    t2 = tt;
}

template <class T>
void inline OffsetData(T& t1, T& t2, bool flSwap)
{
    if(!flSwap) return;
    T tt = t1;
    t1 = t2;
    t2 = tt;
}

template <class T>
class TMarker
{
	static CFont fFontNameDef;
public:
    int base_position;
    int num_marker;
    int typeI;
    char typeCh;
    CString name;
    LOGFONT lfName;
	CFont *fFontName;
    COLORREF color, colorName, colorSave;
    T value;
    BOOL flag_select, flag_visible;
	CSize shift_text0;
	CSize shift_text;
	int num_buf;
    TMarker()
    {
        num_marker = -1;
        base_position = 0;
        name = "M";
        typeI = 0;
        typeCh = 'z';
        color = colorSave = RGB(0,255,0);
        colorName = 0;

        LOGFONT lfName0 = {
            -15, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
            OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
            VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
        };
        lfName = lfName0;

		if(!fFontNameDef.m_hObject) fFontNameDef.CreateFontIndirect(&lfName);
		fFontName = &fFontNameDef;
		
		value = 0;
        flag_select = 0;
        flag_visible = 0;
		shift_text0 = CSize(0,-30);
		shift_text = shift_text0;
		num_buf = -1;
    }
    TMarker(int pos_element0, int num_marker0, CString name0, int type0)
    {
        base_position = pos_element0;
        num_marker = num_marker0;
        typeI = type0;
        name = name0;

        LOGFONT lfName0 = {
            -15, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
            OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
            VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
        };
        lfName = lfName0;

		if(!fFontNameDef.m_hObject) fFontNameDef.CreateFontIndirect(&lfName);
		fFontName = &fFontNameDef;

        color = colorSave = RGB(0,0,0);
        value = 0;
        flag_select = 0;
        flag_visible = 0;
		shift_text0 = CSize(0,-30);
		shift_text = shift_text0;
		num_buf = -1;
}
    TMarker(int pos_element0, int num_marker0, CString name0, COLORREF clr_m, COLORREF clr_n, int type0)
    {
        base_position = pos_element0;
        num_marker = num_marker0;
        typeI = type0;
        name = name0;

        LOGFONT lfName0 = {
            -15, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
            OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
            VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
        };
        lfName = lfName0;

		if(!fFontNameDef.m_hObject) fFontNameDef.CreateFontIndirect(&lfName);
		fFontName = &fFontNameDef;

        color = clr_m;
        colorName = colorSave = clr_n;
        value = 0;
        flag_select = 0;
        flag_visible = 0;
		shift_text0 = CSize(0,-30);
		shift_text = shift_text0;
		num_buf = -1;
}
    TMarker(int pos_element0, int num_marker0, CString name0, COLORREF clr_m, COLORREF clr_n, T val, char type_ch)
    {
        base_position = pos_element0;
        num_marker = num_marker0;
        typeCh = type_ch;
        name = name0;

        LOGFONT lfName0 = {
            -15, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
            OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
            VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
        };
        lfName = lfName0;

		if(!fFontNameDef.m_hObject) fFontNameDef.CreateFontIndirect(&lfName);
		fFontName = &fFontNameDef;

        color = clr_m;
        colorName = colorSave = clr_n;
        value = val;
        flag_select = 0;
        flag_visible = 0;
		shift_text0 = CSize(0,-30);
		shift_text = shift_text0;
		num_buf = -1;
}
    TMarker(int pos_element0, int num_marker0, CString name0, int type0, char type_ch)
    {
        base_position = pos_element0;
        num_marker = num_marker0;
        typeI = type0;
        typeCh = type_ch;
        name = name0;

        LOGFONT lfName0 = {
            -15, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
            OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
            VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
        };
        lfName = lfName0;

		if(!fFontNameDef.m_hObject) fFontNameDef.CreateFontIndirect(&lfName);
		fFontName = &fFontNameDef;

        color = colorSave = RGB(0,0,0);
        value = 0;
        flag_select = 0;
        flag_visible = 0;
		shift_text0 = CSize(0,-30);
		shift_text = shift_text0;
		num_buf = -1;
}
    TMarker& operator=(TMarker& marker)
    {
        base_position = marker.base_position;
        num_marker = marker.num_marker;
        typeI = marker.typeI;
        typeCh = marker.typeCh;
        name = marker.name;
        lfName = marker.lfName;

		fFontNameDef.CreateFontIndirect(&lfName);
		fFontName = &fFontNameDef;

        color = marker.color;
        colorSave = marker.colorSave;
        colorName = marker.colorName;
        value = marker.value;
        flag_select = marker.flag_select;
        flag_visible = marker.flag_visible;
		shift_text0 = marker.shift_text0;
		num_buf = marker.num_buf;
		return *this;
    }
    TMarker(TMarker& marker)
    {
        operator=(marker);
    }
    TMarker(TMarker *marker)
    {
        operator=(*marker);
    }
    void ReplaceFontName(LOGFONT lfName0)
    {
        lfName =lfName0;

		fFontName = new CFont();
		fFontName->CreateFontIndirect(&lfName);
		
    }
    void ReplaceColorName(COLORREF clr)
    {
        colorName = clr;
    }
    COLORREF ReplaceColor(COLORREF clr, BOOL flg = true)
    {
        if(flg) colorSave = color;
        color = clr;
        return colorSave;
    }
    void RestoreColor()
    {
        color = colorSave;
    }
    void SetValue(T val)
    {
        value = val;
    }
    void GetValue(T val)
    {
        return value;
    }
    void SelectFrag(COLORREF color_select)
    {
        if(!flag_select) ReplaceColor(color_select, true);
        else RestoreColor();
        flag_select = !flag_select;
    }
    void SetTypeCh(char ch_type)
    {
        typeCh = ch_type;
    }
    inline char GetTypeCh()
    {
        return typeCh; 
    }

    inline void SetVisible(BOOL flg = false)
    {
        flag_visible = flg; 
    }

	inline void SetShiftText(int cx, int cy)
	{
		shift_text = shift_text0 + CSize(cx, cy);
	}
    ~TMarker()
    {
		if(fFontName != NULL && fFontName != &fFontNameDef) 
		{
			if(fFontName->m_hObject) fFontName->DeleteObject();
			fFontName = NULL;
		}
    }
};

template <class T> CFont TMarker<T>::fFontNameDef;

template <class T>
class TFragment : public TMarker<T>
{
	int base_position;
public:
    int begin, end, iterate_position;
    BOOL frag_locked;
	int qq;
public:
    int num;
    inline TFragment()
    {
        SetBase(0);
        begin = 0;
        end = -1;
        iterate_position = 0;
        frag_locked = false;
		qq = 0;
    }
    inline TFragment(int beg_BP, int end_BP, int base_pos, COLORREF clr) : TMarker<T>(base_pos, 0, "", 0)
    {
        begin = beg_BP;
        end = end_BP;
		SetBase(base_pos);
        iterate_position = 0;
        color = clr;
        frag_locked = false;
		qq = 0;
    }
    inline TFragment(int beg_BP, int end_BP, int base_pos, COLORREF clr, char type_ch) : TMarker<T>(base_pos, 0, "", 0, type_ch)
    {
        begin = beg_BP;
        end = end_BP;
		SetBase(base_pos);
        iterate_position = 0;
        color = clr;
        frag_locked = false;
		qq = 0;
    }
    inline TFragment(TFragment<T>& frament)
    {
        operator=(frament);
    }
    inline TFragment(TFragment<T> *frament)
    {
        ASSERT(frament);
        operator=(*frament);
    }
    inline TFragment<T>& operator=(TFragment<T>& fragment)
    {
        TMarker<T>::operator=(fragment);
        begin = fragment.begin;
        end = fragment.end;
		SetBase(fragment.base_position);
        iterate_position = fragment.iterate_position;
        frag_locked = fragment.frag_locked;
        num = fragment.num;
        return *this;
    }
    int inline GetSize()
    {
        return end - begin + 1;
    }
    int inline GetCurBP()
    {
        return end;
    }
    int inline GetCurGP()
    {
        return GetBase() + end - begin;
    }
    int inline SetBegBP(int beg_BP)
    {
		return begin = beg_BP >= 0 ? beg_BP : 0;
    }
    int inline GetBegBP()
    {
        return begin;
    }
    int inline SetEndBP(int end_BP)
    {
        return end = end_BP;
    }
    int inline SetEndGP(int end_GP)
    {
        return end = end_GP - GetBase();
    }
    void inline SetBase(int base_pos)
    {
 		base_position = base_pos;
    }
    int inline GetEndBP()
    {
        return end;
    }
    int inline GetEndGP()
    {
        return GetBase() + end - begin;
    }
	int inline GetBegGP()
	{
		return GetBase();
	}

	int inline GetBase()
    {
        return base_position;
    }
    int inline ConvGPtoBP(int index_GP)
    {
        return index_GP - GetBase() + begin; 
    }
    int inline ConvBPtoGP(int index_BP)
    {
        return index_BP + GetBase() - begin; 
    }
    int inline SetSize(int size)
    {
		end = (size <= 0) ? begin - 1 : begin + size - 1;
        return end;
    }
	int inline Clear()
	{
		int sz = GetSize();
		end = begin - 1;
		return sz;
	}
    int inline TestIndexBP(int index_BP)
    {
        int nn;
        if((index_BP >= begin) && (end >= index_BP)) nn = 0; 
        else if(index_BP > end) nn = 1; 
        else nn = -1;					
        return nn;
    }
    int inline TestIndexGP(int index_GP)
    {
        return TestIndexBP(ConvGPtoBP(index_GP));
    }
    int inline ShiftLocal(int shift)
    {
        int nn = begin;
        nn += shift;
        begin = (nn > 0) ? nn : 0;

        nn = end;
        nn += shift;
		end = (nn >= begin) ? nn : begin - 1;
		return GetSize();
    }
    void inline ShiftGlobal(int shift)
    {
        int nn = GetBase();
        nn += shift;
		SetBase((nn > 0) ? nn : 0);
    }
    int inline TestBasePos(int index_GP)
    {
        return (index_GP >= GetBase()) ? 1 : -1;
    }
    int inline SetIterPosLP(int index_LP)
    {
        return iterate_position = index_LP;
    }
    int inline SetIterPosBP(int index_BP)
    {
        return iterate_position = index_BP;
    }
    int inline SetIterPosBegFrag()
    {
        return iterate_position = begin;
    }
    int inline GetNext()
    {
        return (end >= ++iterate_position) ?  iterate_position : -1;
    }
    int CutRightPart(int index_BP)
    {
        int nn = GetSize();
        int b_p = index_BP - 1;
        end = (b_p >= begin) ? b_p : begin - 1;
        return nn - GetSize();
    }
    int CutLeftPart(int index_BP)
    {
        int nn = GetSize();
        int b_p = index_BP + 1;
		if(end >= b_p)begin = b_p;
		else
		{
			begin = end;
			end -= 1; 
		}
        return nn - GetSize();
    }
    int CutInnerPart(int index_begBP, int index_endBP)
    {
        int sz0 = GetSize();
        int sz = index_endBP - index_begBP + 1;
        end = (sz0 > sz) ? end - sz : begin - 1;
        return sz0 - GetSize();
    }
    BOOL inline LockFrag(BOOL fl_lock = true)
    {
        BOOL fl = frag_locked;
        frag_locked = fl_lock;
        return fl;
    }

    BOOL inline UnLockFrag()
    {
        BOOL fl = frag_locked;
        frag_locked = false;
        return fl;
    }

    void inline FinishFrag(BOOL flg_finished)
    {
        if(flg_finished) end--; 
    }

    ~TFragment()
    {
    }
};

template <class P>
class BufferExtendedParametrs
{
public:
    P Fmin, Fmax, FminDiapazon, FmaxDiapazon, maxF, deltaF, mouse_value;
    COLORREF colorDraw, colorBase;
    int thickness, mouse_index;
    CString sName, sRazmern;
    LOGFONT lfName, lfRazmern;
    CFont	fFontName, fFontRazmern;
    CPen pnGrf;
    BufferExtendedParametrs()
    {
        Fmin = 1;
        Fmax = 1;
        deltaF = 0;
        maxF = 1;
        FminDiapazon = 0;
        FmaxDiapazon = 0;
        sName = "Глубина";
        sRazmern = "Красавчег!";
        thickness = 1;
        colorDraw = RGB(200,200,255);
        colorBase = RGB(200,200,255);

        LOGFONT lfName0 = {
            -15, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
            OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
            VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
        };
        LOGFONT lfRazmern0 = {
            -15, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 
            OUT_TT_ONLY_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY,
            VARIABLE_PITCH | FF_SWISS, "MS Sans Serif"
        };
        lfName = lfName0;
        lfRazmern = lfRazmern0;

		if(fFontName.m_hObject) fFontName.DeleteObject();
        fFontName.CreateFontIndirect(&lfName);
		if(fFontRazmern.m_hObject) fFontRazmern.DeleteObject();
        fFontRazmern.CreateFontIndirect(&lfRazmern);

        pnGrf.CreatePen(PS_SOLID, thickness, colorDraw);
    }
    //------------------------------------------------------------------------
    BufferExtendedParametrs& operator=(BufferExtendedParametrs& ext_par)
    {
        Fmin = ext_par.Fmin;
        Fmax = ext_par.Fmax;
        deltaF = ext_par.deltaF;
        maxF = ext_par.maxF;
        FminDiapazon = ext_par.FminDiapazon;
        FmaxDiapazon = ext_par.FmaxDiapazon;
        sName = ext_par.sName;
        sRazmern = ext_par.sRazmern;
        thickness = ext_par.thickness;
        colorDraw = ext_par.colorDraw;
        colorBase = ext_par.colorBase;

        lfName = ext_par.lfName;
        lfRazmern = ext_par.lfRazmern;

        if(fFontName.m_hObject) fFontName.DeleteObject();
        if(fFontRazmern.m_hObject) fFontRazmern.DeleteObject();

		if(fFontName.m_hObject) fFontName.DeleteObject();
        fFontName.CreateFontIndirect(&lfName);
		if(fFontRazmern.m_hObject) fFontRazmern.DeleteObject();
        fFontRazmern.CreateFontIndirect(&lfRazmern);

        pnGrf.DeleteObject();
        pnGrf.CreatePen(PS_SOLID, thickness, color);
        return *this;
    }
    BufferExtendedParametrs(BufferExtendedParametrs& ext_par)
    {
        operator=(ext_par);
    }
    BufferExtendedParametrs(BufferExtendedParametrs *ext_par)
    {
        operator=(*ext_par);
    }
    COLORREF ReplacePenColor(COLORREF clr)
    {
        COLORREF cls = colorDraw;
        colorDraw = clr;
        pnGrf.DeleteObject();
        pnGrf.CreatePen(PS_SOLID, thickness, colorDraw);
        return cls;
    }
    void SetBaseColor(COLORREF clr)
    {
        colorBase = clr;
        ReplacePenColor(clr);
    }
    int ReplacePenThick(int thick)
    {
        int ts = thickness;
        thickness = thick;
        pnGrf.DeleteObject();
        pnGrf.CreatePen(PS_SOLID, thickness, colorDraw);
        return thick;
    }
    int GetPenThick()
    {
        return thickness;
    }
    void ReplaceFontName(LOGFONT lfName0)
    {
        lfName =lfName0;
        if(fFontName.m_hObject) fFontName.DeleteObject();
        fFontName.CreateFontIndirect(&lfName);
    }
    void ReplaceFontRazmern(LOGFONT lfRazmern0)
    {
        lfRazmern =lfRazmern0;
        if(fFontRazmern.m_hObject) fFontRazmern.DeleteObject();
        fFontRazmern.CreateFontIndirect(&lfRazmern);
    }
    P SetMinMax(P scFmin, P scFmax)
    {
        Fmin = scFmin;
        Fmax = scFmax;
        if(FminDiapazon > Fmin) FminDiapazon = Fmin;
        if(FmaxDiapazon < Fmax) FmaxDiapazon = Fmax;
        return (Fmax - Fmin);
    }
    P GetMinMax(P& scFmin, P& scFmax)
    {
        scFmin = Fmin;
        scFmax = Fmax;
        return (Fmax - Fmin);
    }
    P SetMinMaxDiapazon(P scFminS, P scFmaxS)
    {
        FminDiapazon = scFminS;
        FmaxDiapazon = scFmaxS;
        SetMinMax(scFminS, scFmaxS);
        deltaF = scFmaxS - scFminS;
        return deltaF;
    }
    P SetMinMaxDiapazonEX(P scFminS, P scFmaxS)
    {
        FminDiapazon = scFminS;
        FmaxDiapazon = scFmaxS;
        if(Fmin < FminDiapazon) Fmin = FminDiapazon;
        if(Fmax > FmaxDiapazon) Fmax = FmaxDiapazon;
        deltaF = scFmaxS - scFminS;
        return deltaF;
    }
    P GetMinMaxDiapazon(P& scFmin, P& scFmax)
    {
        scFmin = FminDiapazon;
        scFmax = FmaxDiapazon;
        deltaF = FmaxDiapazon - FminDiapazon;
        return deltaF;
    }
    void SaveMinMax()
    {
        FminDiapazon = Fmin;
        FmaxDiapazon = Fmax;
    }
    P GetMaxF()
    {
        return maxF;
    }
    P GetDiffMaxMin()
    {
        return Fmax - Fmin;
    }

    ~BufferExtendedParametrs()
    {
        if(fFontName.m_hObject) fFontName.DeleteObject();
        if(fFontRazmern.m_hObject) fFontRazmern.DeleteObject();
        pnGrf.DeleteObject();
    }
};

template <class T>
class RingBuf
{
protected:
    T *pData;
    int cuIndexL, cuIndexH, sizeBuff, sizeData;
    int iterate_position;
    BOOL data_locked;
	BOOL flag_auto_ring;

	RingBuf<T>* pBufAvg;
	int count_avg; 
	BOOL flg_avg; 
    int inline GetIndex(int num) const
    {
		VERIFY(sizeBuff);
        if(sizeData == 0) return 0;
        if(num < 0) return sizeBuff + num;
        if(num < sizeBuff) return num;
        return num%sizeBuff;
    }
    int inline ConvToLocal(int global_index)
    {
        int nn = global_index - GetBase(); 
        return nn >= 0 ? nn : -1;
    }
    int inline ConvToGlobal(int local_index)
    {
        int nn = local_index + GetBase(); 
        return nn < sizeBuff ? nn : -1;
    }
    int inline GetBeginGP()
    {
        return GetBase();
    }
    BOOL inline TestIndexGP(int index_GP)
    {
        int l_p = ConvToLocal(index_GP);
        if(l_p < 0) return false;
        return ((l_p >= 0) && (l_p <= GetCur())) ? TRUE : FALSE;
    }
    int inline SetSizeData(int sz)
    {
        if(data_locked) return sizeData;
        sizeData = (sz <= sizeBuff) ? sz : sizeBuff;
        if(curFrag) curFrag->SetEndBP(GetCur());
        return sizeData;
    }
    int inline GetNextIndex(int num)
    {
        return (num + 1)%sizeBuff;
    }
    int inline GetPrevIndex(int num)
    {
        int nn = (num > 0) ? num - 1 : sizeBuff - 1;
		return GetIndex(nn);
    }
    void inline CreateBuffer(int size, BOOL flag_constructor = false)
    {
        if(flag_constructor)
        {
            pData = (T*)new T[size];
            sizeBuff = size;
        }
        else 
        {
            if(size > sizeBuff)
            {
                if(pData != NULL ) delete pData;
                pData = (T*)new T[size];
                sizeBuff = size;
            }
            else
            {
                if(pData == NULL && size != 0) 
                {
                    pData = (T*)new T[size];
                    sizeBuff = size;
                }
            }
        }
    }
    void inline IncrementIndex(int& ind)
    {
        ind = (++ind)%sizeBuff; 
    }
    void inline DecrimentIndex(int& ind)
    {
		if(--ind < 0) ind = sizeBuff - 1;
    }
    BOOL inline IsBufferValidate()
    {
        if(pData == NULL || sizeBuff == 0) return false;
        return true;
    }
public:
    void inline ExtendDataIndexes(int nn)
    {
        if(data_locked) return;
        int nn1 = GetFreePositions();
        if(nn > nn1) nn = nn1;
        cuIndexH = GetIndex(cuIndexH + nn);
        SetSizeData(sizeData + nn);
    }
    int inline GetCur()
    {
        return (sizeData > 0) ? sizeData - 1 : 0;
    }
    int inline GetCurFree()
    {
        return sizeData;
    }
    int inline GetFreePositions()
    {
        return sizeBuff - sizeData;
    }
    int inline CopyData(int start_element, int count_elements, T* buf)
    {
		WaitForSingleObject(hMutex, INFINITE);
        if(!IsBufferValidate() || buf == NULL || start_element > sizeData - 1) return 0;

		WaitForSingleObject(hMutex, INFINITE);
		
		int nn;

        int max_right_data = sizeData - start_element;
        if(count_elements > max_right_data) nn = max_right_data;
        else nn = count_elements;

        int ind = GetIndex(cuIndexL + start_element);
        for(int i = 0; i < nn; i++)
        {
            buf[i] = pData[ind];
            IncrementIndex(ind);
        }
		ReleaseMutex(hMutex);
        return nn;
    }
    int inline PasteData(int start_element, 
        int count_elements, const T *data)
    {
		if(data_locked) return 0;
        if(!IsBufferValidate() || data == NULL || start_element > sizeBuff - 1) return 0;

		WaitForSingleObject(hMutex, INFINITE);

        int right_side_fragment = start_element + count_elements;
        int nn;
        if(right_side_fragment > sizeBuff) 
            nn = count_elements - (right_side_fragment - sizeBuff);
        else nn = count_elements;

        int ind = GetIndex(cuIndexL + start_element);
        for(int i = 0; i < nn; i++)
        {
            pData[ind] = data[i];
            IncrementIndex(ind);
        }

        int m_max = start_element + nn;
        if(m_max > sizeData)
        {
            SetSizeData(m_max);
            cuIndexH = GetIndex(cuIndexL + sizeData);
        }
		ReleaseMutex(hMutex);
        return nn;
    }
    int inline ShiftData(int start_element, int count_elements, int shift)
    {
        if(data_locked) return 0;
        if(!IsBufferValidate() || start_element > sizeData - 1 || count_elements == 0 || shift == 0) return 0;

		WaitForSingleObject(hMutex, INFINITE);

		int nn, ind1, ind2, sh;
        int dn = sizeBuff - sizeData;

        int max_right_data = sizeData - start_element;
        if(count_elements > max_right_data) nn = max_right_data;
        else nn = count_elements;

        if(shift > 0) 
        {
            int max_shift = sizeBuff - (start_element + nn);
            if(shift > max_shift) sh = max_shift; 
            else sh = shift;

            ind1 = GetIndex(cuIndexL + start_element + nn - 1);
            ind2 = GetIndex(ind1 + sh);
            for(int i = 0; i < nn; i++)
            {
                pData[ind2] = pData[ind1];
                ind1 = GetPrevIndex(ind1);
                ind2 = GetPrevIndex(ind2);
            }
            int m_data = start_element + nn + sh;
            if(m_data > sizeData) 
            {
                SetSizeData(m_data);
                cuIndexH = GetIndex(cuIndexL + sizeData);
            }
        }
        else 
        {
            shift = -shift;

            int max_shift = dn + start_element;
            if(shift > max_shift) sh = max_shift; 
            else sh = shift;

            ind1 = GetIndex(cuIndexL + start_element);
            ind2 = GetIndex(ind1 - sh);
            for(int i = 0; i < nn; i++)
            {
                pData[ind2] = pData[ind1];
                ind1 = GetNextIndex(ind1);
                ind2 = GetNextIndex(ind2);
            }
            int m_data = sh - start_element;
            if(m_data > 0) 
            {
                SetSizeData(sizeData + m_data);
                cuIndexL = GetIndex(cuIndexL - m_data);
            }
        }
		ReleaseMutex(hMutex);
        return sh;
    }
    int inline CreateSplit(int start_element, int size_split, const T *data = NULL)
    {
        if(data_locked) return 0;
        if(!IsBufferValidate() || start_element > sizeData - 1 || size_split == 0) return 0;
        int nn;

        if(start_element > sizeData - start_element)
        {
            nn = ShiftData(start_element, sizeData - start_element, size_split);
        }
        else
        {
            int sp = size_split;
            nn = ShiftData(0, start_element + 1, -sp);
        }
        if(data != 0) PasteData(start_element + 1, size_split, data);
        return nn;
    }
public:
    TFragment<T>* curFrag;
    int base_position;
    BOOL fl_extern_buf;
    T norm_max;
	HANDLE hMutex;
    RingBuf()
    {
		hMutex = 0;
        Clear();
        pData = NULL;
        sizeBuff = 0;
        fl_extern_buf = false;
		base_position = 0;
        curFrag = NULL;
		flag_auto_ring = false;

		count_avg = 0;
		pBufAvg = NULL;
		flg_avg = false;
    }
    RingBuf(int size_buf, int size_data, int base_pos, T *data = NULL)
    {
		hMutex = 0;
        curFrag = NULL;
        Clear();
        CreateBuffer(size_buf, true);
        if(data != NULL) 
        {
            memcpy(pData, data, size_data*sizeof(T));
            SetSizeData(size_data);
            cuIndexH = sizeData;
        }
        else SetSizeData(0);
        fl_extern_buf = false;
        base_position = base_pos;
		flag_auto_ring = false;

		count_avg = 0;
		pBufAvg = NULL;
		flg_avg = false;
   }
    RingBuf(RingBuf& pBuf)
    {
        operator=(pBuf);
    }
    RingBuf(RingBuf* pBuf)
    {
        ASSERT(pBuf);
        operator=(*pBuf);
    }

    RingBuf(int base_pos, RingBuf* pBuf)
    {
        ASSERT(pBuf);
        operator=(*pBuf);
        this->base_position = base_pos;
    }
    RingBuf& operator=(RingBuf& pBuf)
    {
        fl_extern_buf = pBuf.fl_extern_buf;
        if(fl_extern_buf) pData = pBuf.pData;
        else Init(pBuf.sizeBuff, pBuf.sizeData, pBuf.base_position, pBuf.pData);

        cuIndexL = pBuf.cuIndexL;
        cuIndexH = pBuf.cuIndexH;
        sizeBuff = pBuf.sizeBuff;
        sizeData = pBuf.sizeData;
        iterate_position = pBuf.iterate_position;
        base_position = pBuf.base_position;
        data_locked = pBuf.data_locked;
		flag_auto_ring = pBuf.flag_auto_ring;

		count_avg = 0;
		flg_avg = false;
		pBufAvg = NULL;
		return *this;
	}
	HANDLE InitMutex()
	{
		return hMutex = CreateMutex(NULL, FALSE, NULL);
	}

	void InitBufAvg(int n_avg, BOOL flg)
	{
		if(pBufAvg != NULL) delete pBufAvg;
		count_avg = MAX_BUF_AVG > n_avg ? n_avg : MAX_BUF_AVG - 1;//число усреднений меньше на 1 размера буфера, так быстрее алгоритм
		pBufAvg = new RingBuf<T>(MAX_BUF_AVG, 0, 0);
		flg_avg = flg;
		pBufAvg->flag_auto_ring = true;
	}

    void Init(int size_buf, int size_data, int base_pos, const T *data = NULL)
    {
		if(hMutex) WaitForSingleObject(hMutex, INFINITE);
        Clear();
        CreateBuffer(size_buf);
        if(data != NULL && size_data != 0) 
        {
            memcpy(pData, data, size_data*sizeof(T));
            SetSizeData(size_data);
        }
        else SetSizeData(0);
        base_position = base_pos; 
		if(hMutex) ReleaseMutex(hMutex);
    }
    void Init(int beg_index, int size_data, const T *data = NULL)
    {
        PasteData(beg_index, size_data, data);
    }
    void SetExternalBuf(int size_buf, int size_data, int base_pos, T *pBuf)
    {
        ASSERT(pBuf);
        pData = (T*)pBuf;
        Clear();
        sizeBuff = size_buf;
        if(pBuf != NULL) SetSizeData(size_data);
        cuIndexH = GetIndex(cuIndexH + GetCur());
        fl_extern_buf = TRUE;
        base_position = base_pos; 
    }

    void SetExternalBuf(int base_pos, RingBuf* pBuf)
    {
        ASSERT(pBuf);
        int flag_external = pBuf->fl_extern_buf;
        pBuf->fl_extern_buf = true;
        operator=(*pBuf);
        pBuf->fl_extern_buf = flag_external;
        base_position = base_pos; 
    }

    void SetExternalBuf(RingBuf* pBuf)
    {
        ASSERT(pBuf);
        int flag_external = pBuf->fl_extern_buf;
        pBuf->fl_extern_buf = true;
        operator=(*pBuf);
        pBuf->fl_extern_buf = flag_external;
    }
    T* SetExternalBuf(T *pBuf)
    {
        ASSERT(pBuf);
        T* sb = pData;
        pData = pBuf;
        fl_extern_buf = true;
        return sb;
    }
    void inline Clear()
    {
        cuIndexL = 0;
        cuIndexH = 0;
        iterate_position = 0;
        norm_max = 1;
        data_locked = false;
		flag_auto_ring = false;
        SetSizeData(0);
        if(curFrag != NULL) curFrag->Clear();
    }
    void inline Clear0()
    {
        curFrag = NULL;
        cuIndexL = 0;
        cuIndexH = 0;
        iterate_position = 0;
        norm_max = 1;
        data_locked = false;
		flag_auto_ring = false;
        SetSizeData(0);
    }

    BOOL inline LockData(BOOL fl_lock = true)
    {
        BOOL fl = data_locked; 
        data_locked = fl_lock;
        return fl;
    }
    BOOL inline UnLockData()
    {
        BOOL fl = data_locked; 
        data_locked = false;
        return fl;
    }
	BOOL inline SetFlagAutoRing(BOOL flg)
	{
		BOOL fl = flag_auto_ring; 
		flag_auto_ring = flg;
		return fl;
	}
    void inline SetPoint(int index_element, T data)
    {
        PasteData(index_element, 1, &data);
    }
	int inline AddOneElementToEnd_0(T data)
	{
		if(flg_avg && pBufAvg != NULL && count_avg > 1) 
		{
			int nn = pBufAvg->GetSizeData();
			if(nn < count_avg)
			{
				pBufAvg->AddOneElementToEnd_0(data);
				data = (GetLastData()*nn + data)/(nn + 1);
			}
			else
			{
				T first_data = pBufAvg->GetElement(nn - count_avg);
				pBufAvg->AddOneElementToEnd_0(data);
				data = (GetLastData()*count_avg - first_data + data)/count_avg;
			}
		}
		if(!flag_auto_ring)
		{
			if(sizeData == sizeBuff) return sizeData;
			SetSizeData(sizeData + 1);
		}
		else 
		{
			if(sizeData < sizeBuff) SetSizeData(sizeData + 1);
			else IncrementIndex(cuIndexL);
		}
		pData[cuIndexH] = data;
		IncrementIndex(cuIndexH);
		return sizeData;
	}
    int inline AddOneElementToEnd(T data)
    {
		if(data_locked || !IsBufferValidate()) return 0;
		if(hMutex) WaitForSingleObject(hMutex, INFINITE);
		int nn = AddOneElementToEnd_0(data);
		if(hMutex) ReleaseMutex(hMutex);
		return nn;
    }
	int inline AddOneElementToBegin_0(T data)
	{
		if(flg_avg && pBufAvg != NULL && count_avg > 1) 
		{
			int nn = pBufAvg->GetSizeData();
			if(nn < count_avg)
			{
				pBufAvg->AddOneElementToEnd_0(data);
				data = (GetFirstData()*nn + data)/(nn + 1);
			}
			else
			{
				T first_data = pBufAvg->GetElement(nn - count_avg);
				pBufAvg->AddOneElementToEnd_0(data);
				data = (GetFirstData()*count_avg - first_data + data)/count_avg;
			}
		}
		if(!flag_auto_ring)
		{
			if(sizeData == sizeBuff) return sizeData;
			SetSizeData(sizeData + 1);
		}
		else
		{
			if(sizeData < sizeBuff) SetSizeData(sizeData + 1);
			else DecrimentIndex(cuIndexH);
		}
		DecrimentIndex(cuIndexL);
		pData[cuIndexL] = data;

		return sizeData;
	}
    int inline AddOneElementToBegin(T data)
    {
		if(data_locked || !IsBufferValidate()) return 0;
		if(hMutex) WaitForSingleObject(hMutex, INFINITE);
		int nn = AddOneElementToBegin_0(data);
		if(hMutex) ReleaseMutex(hMutex);
		return nn;
    }
    int inline AddMultipleElementsToEnd(int count_elements, const T *data)
    {
		if(data_locked || !IsBufferValidate() || data == NULL) return 0;

		if(hMutex) WaitForSingleObject(hMutex, INFINITE);

		int nn = count_elements;
		if(!flag_auto_ring)
		{
			nn = GetFreePositions();
			if(count_elements <= nn) nn = count_elements;
		}
		for(int i = 0; i < nn; i++) 
		{
			T a = data[i];
			AddOneElementToEnd_0(a);
		}
		if(hMutex) ReleaseMutex(hMutex);
		return nn;
    }
    int inline AddMultipleElementsToBegin(int count_elements, const T *data)
    {
		if(data_locked || !IsBufferValidate() || data == NULL) return 0;

		if(hMutex) WaitForSingleObject(hMutex, INFINITE);

		int nn = count_elements;
		if(!flag_auto_ring)
		{
			nn = GetFreePositions();
			if(count_elements <= nn) nn = count_elements;
		}
		for(int i = 0; i < nn; i++) 
		{
			T a = data[i];
			AddOneElementToBegin_0(a);
		}
		if(hMutex) ReleaseMutex(hMutex);
        return nn;
    }
    int inline InsertData(int start_element, int count_elements, const T *data)
    {
        return CreateSplit(start_element, count_elements, data);
    }
    int RemoveMultipleElementsFromBegin(int count_elements, T* rem_elm = NULL)
    {
        if(data_locked) return 0;
		if(hMutex) WaitForSingleObject(hMutex, INFINITE);
        int nn;
        if(count_elements > sizeData) nn = sizeData;
        else nn = count_elements;

        if(rem_elm != NULL)
        {
            for(int i = 0; i < nn; i++)
            {
                rem_elm[i] = pData[cuIndexL];
                pData[cuIndexL] = 0;
                IncrementIndex(cuIndexL);
            }
        }
        else cuIndexL = GetIndex(cuIndexL + nn); 

        SetSizeData(sizeData - nn);
		if(hMutex) ReleaseMutex(hMutex);
        return nn;
    }
    int RemoveMultipleElementsFromEnd(int count_elements, T *rem_elm = NULL)
    {
        if(data_locked) return 0;
        if(sizeData == 0) return 0;
		if(hMutex) WaitForSingleObject(hMutex, INFINITE);
        int nn;
        if(count_elements > sizeData) nn = sizeData;
        else nn = count_elements;

        if(rem_elm != NULL)
        {
            for(int i = 0; i < nn; i++)
            {
                DecrimentIndex(cuIndexH);
                rem_elm[nn - i - 1] = pData[cuIndexH];
            }
        }
        else cuIndexH = GetIndex(cuIndexH - nn); 

        SetSizeData(sizeData - nn);
		if(hMutex) ReleaseMutex(hMutex);
        return nn;
    }
    int inline RemoveData(int start_element, int size_fragment, T* rem_elm = NULL)
    {
        if(data_locked || sizeData == 0) return 0;
        if(!IsBufferValidate() || start_element > sizeData - 1 || size_fragment == 0) return 0;
        int nn;

        if(rem_elm != 0) CopyData(start_element, size_fragment, rem_elm);

        if(start_element == 0) 
        {
            nn = RemoveMultipleElementsFromBegin(size_fragment);
        }
        else if(start_element + size_fragment >= sizeData) 
        {
            nn = RemoveMultipleElementsFromEnd(sizeData - start_element);
        }
        else if(start_element > sizeData - start_element)//правую часть сдвигаем влево 
        {
			if(hMutex) WaitForSingleObject(hMutex, INFINITE);
            int start_shift = start_element + size_fragment;
            int size_shift = sizeData - start_shift;
            nn = ShiftData(start_shift, size_shift, -size_fragment);
            SetSizeData(sizeData - nn);
            cuIndexH = GetIndex(cuIndexH - nn); 
			if(hMutex) ReleaseMutex(hMutex);
        }
        else
        {
			if(hMutex) WaitForSingleObject(hMutex, INFINITE);
            nn = ShiftData(0, start_element - 1, size_fragment);
            SetSizeData(sizeData - nn);
            cuIndexL = GetIndex(cuIndexL + nn); 
			if(hMutex) ReleaseMutex(hMutex);
        }
        return nn;
    }
    int inline GetBase()
    {
        return base_position;
    }
    T GetLastData()
    {
        return pData[GetPrevIndex(cuIndexH)];
    }
    int GetLastIndex()
    {
        return GetPrevIndex(cuIndexH);
    }
    int GetLastDI(T& value)
    {
        int nn = GetPrevIndex(cuIndexH);
        value = pData[nn]; 
        return nn;
    }
    T GetFirstData()
    {
        return pData[cuIndexL];
    }
    void GetFirstLast(T& value1, T& value2)
    {
        value1 = pData[cuIndexL];
        int nn = GetPrevIndex(cuIndexH);
        value2 = pData[nn];
    }
    T GetDiffLastFirst()
    {
        int nn = GetPrevIndex(cuIndexH);
        return pData[nn] - pData[cuIndexL];
    }
	T GetDiffLastData()
	{
		return GetLastData() - GetPrevLastData();
	}
    T GetElement(int num_element)
    {
        if(sizeData > num_element)
            return pData[GetIndex(cuIndexL + num_element)];
        else return 0;
    }
    T GetPrevLastData()
    {
        return pData[GetPrevIndex(cuIndexH - 1)];
    }
    int GetPrevLastIndex()
    {
        return GetPrevIndex(cuIndexH - 1);
    }
    int GetPrevLastDI(T& value)
    {
        int nn = GetPrevIndex(cuIndexH - 1);
        value = pData[nn]; 
        return nn;
    }
    int GetSizeBuf()
    {
        return sizeBuff;
    }
    int GetSizeData()
    {
        return sizeData;
    }
    void SetIterPos(int start_element)
    {
        iterate_position = start_element;
    }
    int inline GetNext(T& value)
    {
        int nn = iterate_position;
        value = GetElement(iterate_position++);
        return nn;
    }
    T GetPrev()
    {
        return GetElement(iterate_position--);
    }
    int FindIndex(int start_index, int end_index, T value)
    {
        if((start_index < 0) || (start_index > end_index) || (end_index < 0) || 
            (end_index > sizeData - 1) || (sizeData == 0)) return -1;
        //------------------------------------------------------------------------	
        //!!!!!!!!!!!!!!!!!!   EVG 19-04-07
        T min_val = GetFirstData();
        T max_val = GetLastData(); 
        if(value < min_val) return 0;//а вдруг
        if(value > max_val) return GetSizeData() - 1;//а вдруг
        //облом, рутина
        //вначале шаманство
        //		T k = (max_val - min_val)/sizeData;
        int index;// = value/k;
        int i_start = start_index, i_end = end_index;
        F i_val;
        do 
        {
            index = i_start + (i_end - i_start)/2; 
            i_val = pData[GetIndex(cuIndexL + index)];
            if(i_val > value) i_end = index; 
            else i_start = index;
        } 
        while(i_end - i_start > 1);

		if(value - i_start > 0.5) i_start++;

        return i_start;
    }
	int FindIndexRevers(T inp_value)
	{
		if(sizeData == 0) return -1;
		int start_index, end_index; 
		end_index = GetSizeData() - 1;
		if(curFrag->GetSize() < 2) return -1;

		T max_value = GetLastData();
		T dd = GetDiffLastData();
		int idapazon_search = end_index;
		if(dd != 0) idapazon_search = (max_value - inp_value)/dd;
		if(idapazon_search > end_index) idapazon_search = end_index;

		start_index = end_index - idapazon_search;

		T cur_val = 0;
		if(idapazon_search > 1 && start_index != 0)
		{
			do 
			{
				if(start_index == 0) 
					break;
				cur_val = GetElement(start_index);
				if(cur_val < inp_value) break;
				start_index -= idapazon_search/2;
				if(start_index < 0) start_index = 0;
			} while (1);
		}

		T min_val = GetFirstData();
		T max_val = GetLastData(); 
		if(inp_value < min_val) return 0;
		if(inp_value > max_val) return GetSizeData() - 1;//а вдруг
		int index;
		int i_start = start_index, i_end = end_index;
		F i_val;
		do 
		{
			index = i_start + (i_end - i_start)/2; 
			i_val = pData[GetIndex(cuIndexL + index)];
			if(i_val > inp_value) i_end = index; 
			else i_start = index;
		} 
		while(i_end - i_start > 1);

		return i_start;
	}
    T GetMax()
    {
        if(sizeData == 0) return 1;
        T fmax = -3.4e+38f, a;

        SetIterPos(0);
        T cur;
        for(int i = 0; i < sizeData; i++) 
        {
            GetNext(cur);
            a = T(fabs(cur));
            fmax = (fmax > a) ? fmax : a;
        }
        return fmax;
    }
    T GetMin()
    {
        if(sizeData == 0) return 1;
        T fmin = 3.4e+38f, a;

        SetIterPos(0);
        T cur;
        for(int i = 0; i < sizeData; i++) 
        {
            GetNext(cur);
            a = T(fabs(cur));
            fmin = (fmin < a) ? fmin : a;
        }
        return fmin;
    }
    T NormData()
    {
        if(data_locked) return 0;
        if(sizeData == 0) return 1;
        T fmax = GetMax();
        for(int i = 0; i < sizeData; i++) 
            pData[GetIndex(cuIndexL + i)] = pData[GetIndex(cuIndexL + i)]/fmax;
        norm_max = fmax;
        return fmax;
    }
    T UnNormData()
    {
        if(data_locked) return 0;
        if(sizeData == 0) return 1;
        for(int i = 0; i < sizeData; i++) 
            pData[GetIndex(cuIndexL + i)] = pData[GetIndex(cuIndexL + i)]*norm_max;
        return norm_max = 1;
    }
    void AddData(int size_data, T* data)
    {
        if(data_locked) return;
        UnNormData();
        AddMultipleElementsToEnd(size_data, data);
    }

	T GetAvgNLastElements(int n_elements, T* in_buf)
	{
		int nn = GetCur();
		if(nn < n_elements) n_elements = nn;
		
		nn = CopyData(GetCur() - n_elements + 1, n_elements, in_buf);
		if(nn == 0) return 0;
		T s = 0;
		for(int i = 0; i < nn; i++) s += in_buf[i];
		return s/nn;
	}

    ~RingBuf()
    {
        if(pData != NULL && !fl_extern_buf) delete pData;
		if(pBufAvg != NULL) delete pBufAvg;
		if(hMutex) CloseHandle(hMutex);
    }
};

template <class T>
class RStack : public RingBuf<T>
{
public:	
	RStack(int size_buf) : RingBuf<T>(size_buf, 0, 0, NULL)
	{
		flag_auto_ring = true;
	}

	BOOL empty()
	{
		return GetSizeData() > 0 ? true : false;
	}
	T pop()
	{
		T t = 0;
		if(GetSizeData()) RemoveMultipleElementsFromEnd(1, &t);
		return t;
	}
	int push(T value)
	{
		return AddOneElementToEnd(value);
	}
};

template <class T>
class RPipe : public RingBuf<T>
{
public:	
	RPipe(int size_buf) : RingBuf<T>(size_buf, 0, 0, NULL)
	{
		flag_auto_ring = true;
	}

	BOOL empty()
	{
		return GetSizeData() > 0 ? true : false;
	}
	T read()
	{
		T t = 0;
		if(GetSizeData()) RemoveMultipleElementsFromEnd(1, &t);
		return t;
	}
	int write(T value)
	{
		return AddOneElementToBegin(value);
	}
};

template <class T>
class RBuf : public RingBuf<T>
{
public:	
	RBuf(int size_buf) : RingBuf<T>(size_buf, 0, 0, NULL)
	{
		flag_auto_ring = true;
	}

	BOOL empty()
	{
		return GetSizeData() > 0 ? true : false;
	}

	RBuf<T>& operator<<(const T& data)
	{
		AddOneElementToEnd(T(data));
		return *this;
	}

	const RBuf<T>& operator>>(T& data) const
	{
		data = pData[GetIndex(cuIndexH - 1)];
		return *this;
	}

	T& operator[](int num_element) const
	{
		VERIFY(sizeData >= 0);
		if(num_element > sizeData - 1) num_element = sizeData - 1;
		return pData[GetIndex(cuIndexL + num_element)];
	}
};


typedef CList<int, int> FragmentSizeList;


template <class T>
class FRingBuf : public RingBuf<T>
{
    typedef CMap<int, int, TMarker<T>*, TMarker<T>*&> MarkerMap;
    typedef CTypedPtrList<CPtrList, TFragment<T>*> FragmentList;
    MarkerMap map_marker;
    int curent_num_marker, n_beg_iterFrag, n_end_iterFrag;
    BOOL flg_view_reper, flg_view_data;
public:	
    TFragment<T>* p_iterFrag;
    FragmentList lst_fragment;
    int begin_iterate_pos, end_iterate_pos;
    POSITION iter_posFrag;
    FragmentList lst_iter_frag;
    int nn_iter_frag;
    COLORREF color;
    BOOL fl_extern_buf;
	BOOL flag_cleared; 
	FRingBuf<T>* pBufBase;

    FRingBuf()
    {
        curent_num_marker = 0;
        fl_extern_buf = false;
        flg_view_reper = true;
        flg_view_data = true;
		flag_cleared = false;
		pBufBase = NULL;
		InitBufAvg(MAX_BUF_AVG, false);
    }

    FRingBuf(int base_pos, RingBuf<T>* pBuf) : RingBuf<T>(pBuf)
    {
        NewFragment(0, base_pos);
        curent_num_marker = 0;
        fl_extern_buf = fl_extern_buf = false;
        base_position = base_pos;
        flg_view_reper = true;
        flg_view_data = true;
		flag_cleared = false;
		pBufBase = NULL;
		InitBufAvg(MAX_BUF_AVG, false);
    }
    FRingBuf(int size_buf, int size_data, int base_pos, COLORREF clr, T *data=NULL) : 
    RingBuf<T>(size_buf, size_data, base_pos, data)
    {
        if(data != NULL && size_data != 0) NewFragment(0, GetCur() - 1, base_pos, clr);
        color = clr;
        curent_num_marker = 0;
        fl_extern_buf = fl_extern_buf = false;
        base_position = base_pos;
        flg_view_reper = true;
        flg_view_data = true;
		flag_cleared = false;
		pBufBase = NULL;
		InitBufAvg(MAX_BUF_AVG, false);
    }
    FRingBuf(FRingBuf& pBuf)
    {
        operator=(pBuf);
    }
    FRingBuf(FRingBuf *pBuf)
    {
        ASSERT(pBuf);
        operator=(*pBuf);
    }

    FRingBuf(int base_pos, FRingBuf *pBuf)
    {
        ASSERT(pBuf);
        operator=(*pBuf);
        base_position = base_pos;
    }
    FRingBuf& operator=(FRingBuf& pBuf)
    {
        RingBuf<T>::operator=(*((RingBuf<T>*)(&pBuf)));
        fl_extern_buf = pBuf.fl_extern_buf;

        curent_num_marker = pBuf.curent_num_marker;

        map_marker.RemoveAll();

        int mKey;
        TMarker<T> *m;
        POSITION pos = pBuf.map_marker.GetStartPosition();
        while (pos != NULL)
        {
            pBuf.map_marker.GetNextAssoc(pos, mKey, m);
            if(m != NULL) map_marker[mKey] = new TMarker<T>(m);
        }

        POSITION posFrag = pBuf.lst_fragment.GetHeadPosition();
        TFragment<T>* pFrag;
        lst_fragment.RemoveAll();
        while (posFrag != NULL)
        {
            pFrag = pBuf.lst_fragment.GetNext(posFrag);
            if(pFrag != NULL)
            {
                pFrag = new TFragment<T>(pFrag);
                lst_fragment.AddTail(pFrag);
            }
        }

        int nn = lst_fragment.GetCount();
        color = pBuf.color;
        flg_view_reper = pBuf.flg_view_reper;
        flg_view_data = pBuf.flg_view_data;

		flag_cleared = false;
		pBufBase = pBuf.pBufBase;

		return *this;
    }

    template <class InpData>
    FRingBuf& operator<<(InpData& data)
    {
        FRingBuf<T>::AddOneElementToEnd(T(data));
        return *this;
    }
    int inline GetBeginGP()
    {
        return base_position;
    }
    void Init(int size_buf, int size_data, int base_pos, const T *data = NULL)
    {
        NewFragment(base_pos); //???
        RingBuf<T>::Init(size_buf, size_data, base_pos, data);
    }

    void Init(int beg_index, int size_data, const T *data = NULL)
    {
        RingBuf<T>::Init(beg_index, size_data, data);
    }

    void Init(FRingBuf<T>* pBuf)
    {
        operator=(*pBuf);
    }

    BOOL SetExternalFlag(BOOL flag_external)
    {
        BOOL sfl = fl_extern_buf;
        fl_extern_buf = fl_extern_buf = flag_external;
        return sfl;
    }

    void SetExternalBuf(T *data)
    {
        RingBuf<T>::SetExternalBuf(data);
        fl_extern_buf = fl_extern_buf = TRUE;
    }

    void SetExternalBuf(int size_buf, int size_data, int base_pos, T *data)
    {
        RingBuf<T>::SetExternalBuf(size_buf, size_data, base_pos, data);
        fl_extern_buf = fl_extern_buf = TRUE;
    }
    int inline GetFreePositions()
    {
        return RingBuf<T>::GetFreePositions();
    }
    T inline GetCurData()
    {
        return RingBuf<T>::GetLastData();
    }
    T inline GetLastData()
    {
        return RingBuf<T>::GetLastData();
    }
    int inline GetCurIndex_GP()
    {
        return curFrag->GetCurGP();
    }
    int inline GetCurDI_GP(T& value)
    {
        value = RingBuf<T>::GetLastData();
        return curFrag->GetCurGP();
    }
    T GetFirstData()
    {
        return RingBuf<T>::GetFirstData();
    }
    T GetElement(int num_element)
    {
        return RingBuf<T>::GetElement(num_element);
    }
    int GetPrevLastDI_GP(T& value)
    {
        int sz = curFrag->GetSize();
        if(sz < 2) return -1;
        value = RingBuf<T>::GetPrevLastData();
        return curFrag->GetEndGP() - 1;
    }
    int GetSizeBuf()
    {
        return RingBuf<T>::GetSizeBuf();
    }
    int GetSizeData()
    {
        return RingBuf<T>::GetSizeData();
    }
    int inline FillListSizeFrg()
    {
        lst_iter_frag.RemoveAll();
        POSITION posFrag = lst_fragment.FindIndex(n_beg_iterFrag);
        iter_posFrag = posFrag;
        if(!posFrag) return 0;
        TFragment* pFrag = lst_fragment.GetAt(posFrag);
        while (posFrag != NULL)
        {
            lst_iter_frag.AddTail(pFrag);
            pFrag = lst_fragment.GetNext(posFrag);
        }
        return lst_iter_frag.GetCount();
    }

    int inline GetIterFrag()
    {
        ASSERT(p_iterFrag);
        if(!p_iterFrag->TestIndexGP(iterate_position)) FindeNextFragment(iter_posFrag);
        return p_iterFrag->GetBasePos(); 
    }

    int FindeIndexInFrag(int indexGP)
    {
        int num, sz;
        if((sz = GetSizeData()) == 0) return -1;
        TFragment<T>* pFrag;
        pFrag = FindeFragment(indexGP, 0, num);
        if(pFrag == NULL) return -1;
        num = pFrag->begin + (indexGP - pFrag->GetBase());
        if(num < 0) return -1;
        else if(num > sz - 1) return sz - 1;
        return num;
    }
    int SetIterPos(int beginGP, int endGP)
    {
        p_iterFrag = FindeFragment(endGP, 0, n_end_iterFrag);
        if(p_iterFrag != NULL) end_iterate_pos = p_iterFrag->ConvGPtoBP(endGP); 
        else
        {
            if (n_end_iterFrag == 0)
                return -1;
            else
            {
				n_end_iterFrag--;
                p_iterFrag = GetFragment(n_end_iterFrag);
                end_iterate_pos = p_iterFrag->GetEndBP();
            }
        }

        p_iterFrag = FindeFragment(beginGP, 0, n_beg_iterFrag, &iter_posFrag);
        if(p_iterFrag != NULL) begin_iterate_pos = p_iterFrag->ConvGPtoBP(beginGP);
        else
        {
            if(n_beg_iterFrag > n_end_iterFrag) 
                return -1;

            p_iterFrag = GetFragment(n_beg_iterFrag);
            begin_iterate_pos = p_iterFrag->GetBegBP();
            iter_posFrag = lst_fragment.FindIndex(n_beg_iterFrag);
            p_iterFrag = lst_fragment.GetNext(iter_posFrag);
        }
        p_iterFrag->SetIterPosBP(begin_iterate_pos);
        nn_iter_frag = n_end_iterFrag - n_beg_iterFrag + 1;

        RingBuf<T>::SetIterPos(begin_iterate_pos);
        iterate_position = begin_iterate_pos;
        return p_iterFrag->ConvBPtoGP(iterate_position);
    }
    int inline GetNext(T& value, BOOL fl_GP = true)
    {
        int cur_pos = RingBuf<T>::GetNext(value);
        if(cur_pos >= end_iterate_pos)
        {
            if(fl_GP) return -p_iterFrag->ConvBPtoGP(cur_pos);
            else return -cur_pos;
        }
        if(p_iterFrag->GetNext() < 0)
        {
            if(iter_posFrag == NULL) return -1;
            p_iterFrag = FindeNextFragment(iter_posFrag);
            p_iterFrag->SetIterPosBegFrag();
            if(fl_GP) return -p_iterFrag->ConvBPtoGP(cur_pos);
            else return -cur_pos;
        } 
        if(fl_GP) return p_iterFrag->ConvBPtoGP(cur_pos);
        else return cur_pos;
    }
    void inline SetPoint(int index_element, T data)
    {
        PasteData(index_element, 1, &data);
    }
    void inline SetElement(int index_element, T data)
    {
        SetPoint(index_element, data);
    }

    COLORREF GetCurFrgColor()
    {
        return p_iterFrag->color;
    }
    BOOL inline TestFragment(int num_frag)
    {
        int nn = lst_fragment.GetCount();
        if(nn == 0 || num_frag < 0) return FALSE;
        if(num_frag > nn - 1) return FALSE;
        return TRUE;
    }

private:
    TFragment<T>* NewFragment(int base_pos)
    {
        curFrag = new TFragment<T>(GetCur(), GetCur() - 1, base_pos, color);
        lst_fragment.AddTail(curFrag);
        curFrag->num = lst_fragment.GetCount() - 1;
        return curFrag;
    }
    TFragment<T>* NewFragment(int base_pos, COLORREF clr, BOOL flg = true)
    {
		int cur_pos = GetCur();
		if (!flg)
			cur_pos++;
        curFrag = new TFragment<T>(cur_pos, cur_pos - 1, base_pos, clr);
        lst_fragment.AddTail(curFrag);
        curFrag->num = lst_fragment.GetCount() - 1;
        return curFrag;
    }
    TFragment<T>* NewFragment(int base_pos, COLORREF clr, char type_ch)
    {
        curFrag = new TFragment<T>(GetCur(), GetCur() - 1, base_pos, clr, type_ch);
        lst_fragment.AddTail(curFrag);
        curFrag->num = lst_fragment.GetCount() - 1;
        return curFrag;
    }
    TFragment<T>* NewFragment(int begin, int end, int base_pos, COLORREF clr)
    {
        curFrag = new TFragment<T>(begin, end, base_pos, clr);
        lst_fragment.AddTail(curFrag);
        curFrag->num = lst_fragment.GetCount() - 1;
        return curFrag;
    }

public:
    TFragment<T>* AddFragment(int b_pos = -1)
    {
        int base_pos;
        if(b_pos >= 0) base_pos = b_pos;
        else base_pos = base_position;

        if(curFrag == NULL) NewFragment(base_pos);
        else
        {
            if(curFrag->GetSize() > 1) 
            {
                curFrag->FinishFrag(true);
                NewFragment(base_pos);
            }
            else curFrag->SetBase(base_pos);
        }
        return curFrag;
    }
    TFragment<T>* AddFragment(int b_pos, COLORREF clr, BOOL flg = true)
    {
        int base_pos;
        if(b_pos >= 0) base_pos = b_pos;
        else base_pos = base_position;

        if(curFrag == NULL) NewFragment(base_pos, clr, true);
        else
        {
            if(curFrag->GetSize() > 1) 
            {
                curFrag->FinishFrag(flg);
                NewFragment(base_pos, clr, flg);
            }
            else
            {
                curFrag->SetBase(base_pos);
                curFrag->ReplaceColor(clr);
            }
        }
        return curFrag;
    }
    TFragment<T>* AddFragment(int b_pos, COLORREF clr, char type_ch)
    {
        int base_pos;
        if(b_pos >= 0) base_pos = b_pos;
        else base_pos = base_position;

        if(curFrag == NULL) NewFragment(base_pos, clr, type_ch);
        else
        {
            if(curFrag->GetSize() != 0) NewFragment(base_pos, clr, type_ch);
            else
            {
                curFrag->SetBase(base_pos);
                curFrag->ReplaceColor(clr, false);
                curFrag->SetTypeCh(type_ch);
            }
        }
        return curFrag;
    }

    void FinishFrag(BOOL flg)
    {
        if(curFrag->GetSize() == 0) return;
        curFrag->FinishFrag(flg);
    }
    inline char GetTypeChCurFrag()
    {
        return curFrag->GetTypeCh();
    }
    inline void SetTypeChCurFrag(char type_ch)
    {
        curFrag->SetTypeCh(type_ch);
    }
    inline char SetSizeCurFrag(int sz_frag)
    {
        return curFrag->SetSize(sz_frag);
    }
    inline TFragment<T>* GetCurFrag()
    {
        return curFrag;
    }
    inline void ShiftCurFrag(int shift)
    {
        curFrag->ShiftLocal(shift);
    }
    COLORREF ReplaceColor(COLORREF clr, BOOL flg = false)
    {
        return curFrag->ReplaceColor(clr, flg);
    }

    TMarker<T>* GetMarker(int base_pos)
    {
        int mKey;
        TMarker<T> *m;
        POSITION pos = map_marker.GetStartPosition();
        while (pos != NULL)
        {
            map_marker.GetNextAssoc(pos, mKey, m);
            if(m != NULL) 
            {
                if(m->base_position == base_pos) return m;
            }
        }
        return NULL;
    }
    void RemoveMarker(int base_pos)
    {
        int mKey;
        TMarker<T> *m;
        POSITION pos = map_marker.GetStartPosition();
        while (pos != NULL)
        {
            map_marker.GetNextAssoc(pos, mKey, m);
            if(m != NULL) 
            {
                if(m->base_position == base_pos) 
                {
                    map_marker.RemoveKey(mKey);
                    return;
                }
            }
        }
    }
    void RemoveAllMarkerType(char ch_type)
    {
        int mKey;
        TMarker<T> *m;
        POSITION pos = map_marker.GetStartPosition();
        while (pos != NULL)
        {
            map_marker.GetNextAssoc(pos, mKey, m);
            if(m != NULL) 
            {
                if(m->typeCh == ch_type) map_marker.RemoveKey(mKey);
            }
        }
    }
    void SetVisibleMarker(int base_pos, BOOL flag_visible)
    {
        int mKey;
        TMarker<T> *m;
        POSITION pos = map_marker.GetStartPosition();
        while (pos != NULL)
        {
            map_marker.GetNextAssoc(pos, mKey, m);
            if(m != NULL) 
            {
                if(m->base_position == base_pos) 
                {
                    m->SetVisible(flag_visible);
                    return;
                }
            }
        }
    }
    void SetVisibleAllMarkerType(char ch_type, BOOL flag_visible)
    {
        int mKey;
        TMarker<T> *m;
        POSITION pos = map_marker.GetStartPosition();
        while (pos != NULL)
        {
            map_marker.GetNextAssoc(pos, mKey, m);
            if(m != NULL) 
            {
                if(m->typeCh == ch_type) m->SetVisible(flag_visible);
            }
        }
    }
    TFragment<T>* GetFragment(int num_frag)
    {
        if(!TestFragment(num_frag)) return NULL;
        POSITION posFrag = lst_fragment.FindIndex(num_frag);
        if(posFrag == NULL) return NULL;
        return lst_fragment.GetAt(posFrag);
    }
    int inline GetBasePosFragment(int num_frag)
    {
        TFragment<T>* pFrag = GetFragment(num_frag);
        if(pFrag == NULL) return -1;
        return pFrag->base_position;
    }
    void inline SetBasePosFragment(int num_frag, int b_pos = -1)
    {
        int base_pos;
        if(b_pos >= 0) base_pos = b_pos;
        else base_pos = base_position;

        TFragment<T>* pFrag = GetFragment(num_frag);
        if(pFrag != NULL) pFrag->SetBase(base_pos);
    }
    void inline SetBasePosCurFrag(int b_pos)
    {
        int base_pos;
        if(b_pos >= 0) base_pos = b_pos;
        else base_pos = base_position;

        if(curFrag != NULL) curFrag->SetBase(base_pos);
    }
    int inline GetBasePosCurFrag()
    {
        if(curFrag == NULL) return -1;
        return curFrag->GetBase();
    }
    int FindeFragment(int num_element, int start_fragment)
    {
        if(!TestFragment(start_fragment)) return -1;
        POSITION posFrag = lst_fragment.FindIndex(start_fragment);
        if(!posFrag) return -1;
        TFragment<T>* pFrag = lst_fragment.GetAt(posFrag);
        while (posFrag != NULL)
        {
            if(pFrag != NULL) 
            {
                if(pFrag->TestIndexBP(num_element)) return start_fragment;
                start_fragment++;
            }
            pFrag = lst_fragment.GetNext(posFrag);
        }
        return -1;
    }
    TFragment<T>*  FindeFragment(int num_element)
    {

        if(lst_fragment.GetCount() == 0) return NULL;
        if(lst_fragment.IsEmpty()) return NULL;
        POSITION posFrag = lst_fragment.GetHeadPosition();
        TFragment<T>* pFrag;
        while (posFrag != NULL)
        {
            pFrag = lst_fragment.GetNext(posFrag);
            if(pFrag != NULL) 
            {
                if(pFrag->TestIndexBP(num_element) == 0) return pFrag;
            }
        }
        return NULL;
    }
    TFragment<T>* FindeFragment(int& num_element, int start_fragment, 
        int& num_frag, POSITION* pos = 0)
    {
        if(!TestFragment(start_fragment)) return NULL;
        int kk = lst_fragment.GetCount();
        POSITION posFrag = lst_fragment.FindIndex(start_fragment);
        if(!posFrag) return NULL;
        TFragment<T>* pFrag;
        int mm = 0;
        if(pos != NULL) *pos = NULL;
        while (posFrag != NULL)
        {
            pFrag = lst_fragment.GetNext(posFrag);
            if(pFrag != NULL) 
            {
                int nn = pFrag->TestIndexGP(num_element);
                if(nn == -1) //break;//06-04-07 EVG
				{
					num_frag = pFrag->num;
					return NULL;
				}
                else if(nn == 0) 
                {
                    num_frag = pFrag->num;
                    if(pos != 0) *pos = posFrag;
                    return pFrag;
                }
            }
        }
		int n1 = curFrag->GetBegBP();
		int n2 = curFrag->GetEndBP();

		if(n2 >= n1) num_element = curFrag->ConvBPtoGP(n2);
		else num_element = curFrag->ConvBPtoGP(n1);

        num_frag = curFrag->num;
        return curFrag;
    }
    TFragment<T>* FindeNextFragment(POSITION& pos)
    {
        return lst_fragment.GetNext(pos);
    }

    void SetAllColor(COLORREF clr)
    {
        color = clr;

        int mKey;
        TMarker<T> *m;
        POSITION pos = map_marker.GetStartPosition();
        while (pos != NULL)
        {
            map_marker.GetNextAssoc(pos, mKey, m);
            if(m != NULL) m->ReplaceColor(clr, false);
        }

        POSITION posFrag = lst_fragment.GetHeadPosition();
        TFragment<T>* pFrag;
        while (posFrag != NULL)
        {
            pFrag = lst_fragment.GetNext(posFrag);
            if(pFrag != NULL) pFrag->ReplaceColor(clr, false);
        }
    }

    void SelectFrag(int num_element, COLORREF color_select)
    {
        TFragment<T> *pFrag = FindeFragment(num_element);
        if(pFrag == NULL) return;
        pFrag->SelectFrag(color_select);
    }
    void SelectChainFrags(int num_element, COLORREF color_select, char ch1, char ch2, char ch3)
    {
        TFragment<T> *pFragS = FindeFragment(num_element);
        if(pFragS == NULL) return;
        pFragS->SelectFrag(color_select);
        if(pFragS->typeCh == ch3) return;

        TFragment<T> *pFrag;
        POSITION posFrag, posFragS = lst_fragment.FindIndex(pFragS->num);

        BOOL flg = false;
        if(pFragS->typeCh == ch1)
        {
            posFrag = posFragS;
            pFrag = lst_fragment.GetNext(posFrag);
            while (posFrag != NULL)
            {
                pFrag = lst_fragment.GetNext(posFrag);
                if(pFrag->typeCh == ch3) 
                {
                    flg = true;
                    continue;
                }
                if(pFrag->typeCh == ch2) break;
                if(pFrag->typeCh == ch1) 
                {
                    if(flg)	
                    {
                        pFrag->SelectFrag(color_select);
                        flg = false;
                        break;
                    }
                }
            }
            posFrag = posFragS;
            pFrag = lst_fragment.GetPrev(posFrag);
            while (posFrag != NULL)
            {
                pFrag = lst_fragment.GetPrev(posFrag);
                if(pFrag->typeCh == ch3)
                {
                    flg = true;
                    continue;
                }
                if(pFrag->typeCh == ch2) break;
                if(pFrag->typeCh == ch1)
                    if(pFrag->typeCh == ch1) 
                    {
                        if(flg)	
                        {
                            pFrag->SelectFrag(color_select);
                            flg = false;
                            break;
                        }
                    }
            }
        }
        else
        {
            posFrag = posFragS;
            pFrag = lst_fragment.GetPrev(posFrag);
            while (posFrag != NULL)
            {
                pFrag = lst_fragment.GetPrev(posFrag);
                if(pFrag->typeCh == ch1 || pFrag->typeCh == ch2) break;
                pFrag->SelectFrag(color_select);
            }
            posFrag = posFragS;
            pFrag = lst_fragment.GetNext(posFrag);
            while (posFrag != NULL)
            {
                pFrag = lst_fragment.GetNext(posFrag);
                if(pFrag->typeCh == ch1 || pFrag->typeCh == ch2) break;
                pFrag->SelectFrag(color_select);
            }
        }
    }
    void UnSelectAllFrags()
    {
        POSITION posFrag = lst_fragment.GetHeadPosition();
        TFragment<T>* pFrag;
        while (posFrag != NULL)
        {
            pFrag = lst_fragment.GetNext(posFrag);
            if(pFrag != NULL)
            {
                if(pFrag->flag_select) pFrag->SelectFrag(0);
            }
        }
    }
    void RemoveFragment(int num_frag)
    {
        POSITION posFrag0 = lst_fragment.FindIndex(num_frag);
		POSITION posFrag = posFrag0;
        if(!posFrag) return;
		TFragment<T>* pFrag = lst_fragment.GetNext(posFrag);
		if(pFrag != NULL) delete pFrag;
		lst_fragment.RemoveAt(posFrag0);
    }
	int RemoveDataFragment(int num_frag)
	{
		if(!TestFragment(num_frag)) return 0;

		POSITION posFrag = lst_fragment.FindIndex(num_frag);
		if(posFrag == NULL) return 0;
		TFragment<T>* pFrag = lst_fragment.GetAt(posFrag);
		if(pFrag == NULL) return 0;

		int nn = 0;
		if(pFrag != NULL)
		{
			nn = RemoveData(pFrag->GetBegBP(), pFrag->GetSize());
			pFrag->Clear();
		}
		return nn;
	}
	int RemoveDataNFragments(int beg_frag, int end_frag)
	{
		int nn = lst_fragment.GetCount();
		if(nn == 0 || beg_frag > nn || end_frag > nn || beg_frag > end_frag || end_frag < 0) return 0;
		POSITION posFrag = lst_fragment.FindIndex(beg_frag);
		if(posFrag == NULL) return 0;
		TFragment<T>* pFrag;
		int size_data_for_remove = 0;

		pFrag = lst_fragment.GetAt(posFrag);
		if(pFrag == 0) return 0;

		int beg_index_for_remove = pFrag->GetBegBP();
		while (posFrag != NULL)
		{
			if(beg_frag++ > end_frag) break;
			pFrag = lst_fragment.GetNext(posFrag);
			if(pFrag != NULL && pFrag != curFrag) size_data_for_remove += pFrag->Clear();
		}
		nn = RemoveData(beg_index_for_remove, size_data_for_remove);
		ShiftAllFragment(-size_data_for_remove, beg_frag - 1);
		curFrag->SetBegBP(curFrag->GetBegBP() - nn);
		return nn;
	}

    void RemoveAllFragments()
    {
        if(lst_fragment.GetCount() == 0) return;
        POSITION posFrag = lst_fragment.GetHeadPosition();
        TFragment<T>* pFrag;
        while (posFrag != NULL)
        {
            pFrag = lst_fragment.GetNext(posFrag);
            if(pFrag != NULL) {delete pFrag; pFrag = NULL;}
        }
        lst_fragment.RemoveAll();
    }
	void RemoveAllZerosFragments()
	{
		if(lst_fragment.GetCount() == 0) return;
		TFragment<T>* pFrag;
		int new_num = 0;
		POSITION pos1, pos2;

		pos1 = lst_fragment.GetHeadPosition();
		while (pos1 != NULL)
		{
			pos2 = pos1;
			pFrag = lst_fragment.GetNext(pos1);
			if(pFrag != NULL && pFrag->GetSize() == 0 && pFrag != curFrag) 
			{
				lst_fragment.RemoveAt(pos2);
				delete pFrag; 
				pFrag = NULL;
			}
			else pFrag->num = new_num++;
		}
	}

    int CopyDataFragment(int num_frag, T* buf)
    {
        ASSERT(curBuf);
        TFragment<T>* pFrag = GetFragment(int num_frag);
        if(pFrag != NULL) return curBuf->CopyData(pFrag->GetBeg(), pFrag->GetSize(), buf);
        return 0;
    }
    TFragment<T>* CopyDataFragment(int num_frag, RingBuf<T>* r_buf)
    {
        TFragment<T>* pFrag = GetFragment(int num_frag);
        if(pFrag == NULL) return NULL;
        if(pFrag->GetSize() > 0)
        {
            T* data = new T[pFrag->GetSize()];
            curBuf->CopyData(pFrag->GetBeg(), pFrag->GetSize(), data);
            r_buf->NewFragment(pFrag->base_position);
            r_buf->AddMultipleElementsToEnd(pFrag->GetSize(), data);
            delete data;
        }
        return pFrag;
    }
    int CopyDataFragment(TFragment<T>* pFrag, FRingBuf<T>* r_buf)
    {
        int nn = 0;
        if(pFrag->GetSize() > 0)
        {
            T* data = new T[pFrag->GetSize()];
            nn = CopyData(pFrag->GetBegBP(), pFrag->GetSize(), data);
            r_buf->NewFragment(pFrag->GetBase());
            r_buf->AddMultipleElementsToEnd(pFrag->GetSize(), data);
            delete data;
        }
        return nn;
    }
    int RemoveDataFragment(TFragment<T>* pFrag, FRingBuf<T>* r_buf = NULL)
    {
        if(r_buf != NULL) CopyDataFragment(pFrag, r_buf);
        if(pFrag != NULL) return RemoveData(pFrag->GetBegBP(), pFrag->GetSize());
        return 0;
    }
	int CutRightDataCurFrag(int index_GP)
	{
		if(curFrag == NULL) return -1;
		if(curFrag->GetSize() <= 0) return -1;
		int cur_poz = curFrag->ConvGPtoBP(index_GP);
		int sz = GetSizeData() - cur_poz - 1;
		if(sz <= 0) return 0; 
		return RemoveMultipleElementsFromEnd(sz);
	}

	int CutRightPartFrag(TFragment<T>* pFrag, int indexBP)
    {
        int sz = pFrag->CutRightPart(indexBP);
        ShiftAllFragment(-sz, pFrag->num + 1);
		curFrag->SetBegBP(curFrag->GetBegBP() - sz);
        return  sz;
    }
	int CutDataRightPartFrag(TFragment<T>* pFrag, int indexBP)
	{
		int end_indexBP = pFrag->GetEndBP();
		int sz = end_indexBP - indexBP + 1;
		int nn = RemoveData(indexBP, sz);
		if(nn == 0) return 0;
		if(pFrag != curFrag) sz = CutRightPartFrag(pFrag, indexBP);
		return nn;
	}
    int CutLeftPartFrag(TFragment<T>* pFrag, int indexBP)
    {
        int sz = pFrag->CutLeftPart(indexBP);
        ShiftAllFragment(-sz, pFrag->num);
		curFrag->SetBegBP(curFrag->GetBegBP() - sz);
        return  sz;
    }
    int CutDataLeftPartFrag(TFragment<T>* pFrag, int indexBP)
    {
		int start_indexBP = pFrag->GetBegBP();
		int sz = indexBP - start_indexBP + 1;
        int nn = RemoveData(start_indexBP, sz);
		if(nn == 0) return 0;
		if(pFrag != curFrag) sz = CutLeftPartFrag(pFrag, indexBP);
        return nn;
    }
    int CutInnerPartFrag(TFragment<T>* pFrag, int index1, int index2)
    {
        int sz = pFrag->CutInnerPart(index1, index2);
        ShiftAllFragment(-sz, pFrag->num + 1);
		curFrag->SetBegBP(curFrag->GetBegBP() - sz);
        return  sz;
    }
	int CutDataInnerPartFrag(TFragment<T>* pFrag, int start_indexBP, int end_indexBP)
	{
		int sz = end_indexBP - start_indexBP + 1;
		int nn = RemoveData(start_indexBP, sz);
		if(nn == 0) return 0;
		if(pFrag != curFrag) sz = CutInnerPartFrag(pFrag, start_indexBP, end_indexBP);
		return nn;
	}
    void ShiftAllFragment(int shift, int start_fragment = 0)
    {
        if(!TestFragment(start_fragment)) return;
        int nn = lst_fragment.GetCount();
        POSITION posFrag = lst_fragment.FindIndex(start_fragment);
        if(!posFrag) return;
        TFragment<T>* pFrag;
        while (posFrag != NULL)
        {
            pFrag = lst_fragment.GetNext(posFrag);
            if(pFrag != NULL && pFrag != curFrag) pFrag->ShiftLocal(shift);
        }
    }
	void ShiftGlobalAllFragment(int shift, int start_fragment = 0)
	{
		if(!TestFragment(start_fragment)) return;
		int nn = lst_fragment.GetCount();
		if(start_fragment > nn - 1) return;
		POSITION posFrag = lst_fragment.FindIndex(start_fragment);
		if(!posFrag) return;
		TFragment<T>* pFrag;
		while (posFrag != NULL)
		{
			pFrag = lst_fragment.GetNext(posFrag);
			if(pFrag != NULL) pFrag->ShiftGlobal(shift);
		}
	}
    void AddData(int size_data, int b_pos, COLORREF clr, T* inBuf)
    {
        int base_pos = b_pos;
        if(base_pos >= 0) base_pos = b_pos;
        else base_pos = base_position;

        if(size_data <= 0 || !inBuf) return;
        NewFragment(GetCurFree(), GetCurFree(), base_pos, clr);
        RingBuf<T>::AddData(size_data, inBuf);
    }
    void AddData1(int size_data, T value)
    {
        if(size_data <= 0) return;
        NewFragment(GetCurFree(), GetCurFree(), base_pos, clr);
        RingBuf<T>::AddData(size_data, inBuf);
    }
    int inline RemoveStartDataP(float size_fragment_in_persent, int& ind1, int& ind2)
    {
        int max_ind = GetCur();
        T delta = GetDiffMaxMin()*size_fragment_in_persent/100.0f;
        ind1 = FindIndex(GetBase(), max_ind, GetMin0());
        if(ind1 < 0) return -1;
        ind2 = FindIndex(ind1, max_ind, GetMax0());
        if(ind2 < 0) ind2 = max_ind;
        return RemoveDataN(ind1, ind2);
    }
    //////////////////////////////////////////////////////////////////////////
	//проверено, работает
	int ClearBeginData(int num_point_for_delete)
	{
		if(num_point_for_delete == 0) return 0;
		int sz_data = GetSizeData();
		if(sz_data == 0) return 0;
		if(sz_data < num_point_for_delete) num_point_for_delete = sz_data;

		if(!map_marker.IsEmpty())
		{
			T value = GetElement(num_point_for_delete);
			int mKey;
			TMarker<T> *m;
			POSITION pos = map_marker.GetStartPosition();
			while (pos != NULL)
			{
				map_marker.GetNextAssoc(pos, mKey, m);
				if(m != NULL) 
				{
					if(m->value < value) map_marker.RemoveKey(mKey);
					else break;
				}
			}
		}
		RemoveDataN(0, num_point_for_delete - 1);
		return num_point_for_delete;
	}

	//проверено, работает
	TFragment<T>* GetFrag(int num_frag, POSITION* pos = 0)
	{
		POSITION posFrag = lst_fragment.FindIndex(num_frag);
		if(posFrag == NULL) return NULL;
		if(pos != 0) *pos = posFrag;
		return lst_fragment.GetAt(posFrag);
	}
	//проверено, работает
    int inline RemoveDataN(int beg_indGP, int end_indGP, T* rem_elm = NULL)
    {
        if(lst_fragment.GetCount() == 0) return 0;
        int num_beg_frag, num_end_frag;
		int start_indexBP, end_indexBP;
		int shift;

		TFragment<T> *pFragL, *pFragR, *pFrag;

		pFragL = FindeFragment(beg_indGP, 0, num_beg_frag);
		pFragR = FindeFragment(end_indGP, num_beg_frag, num_end_frag);

		int beg_index_for_remove, size_data_for_remove = 0; 
		if(pFragL == pFragR && pFragL != 0) 
		{
			start_indexBP = pFragL->ConvGPtoBP(beg_indGP);
			end_indexBP = pFragR->ConvGPtoBP(end_indGP);
			size_data_for_remove = pFragL->CutInnerPart(start_indexBP, end_indexBP);
			beg_index_for_remove = start_indexBP;
			num_beg_frag++;
		}
		else
		{
			if(pFragL == 0)
			{
				if((pFrag = GetFrag(num_beg_frag)) == 0) return 0;
				beg_index_for_remove = pFrag->GetBegBP();
				if(pFragR != 0)
				{
					end_indexBP = pFragR->ConvGPtoBP(end_indGP);
					size_data_for_remove = pFragR->CutLeftPart(end_indexBP);
				}
			}
			else 
			{
				start_indexBP = pFragL->ConvGPtoBP(beg_indGP);
				size_data_for_remove = pFragL->CutRightPart(start_indexBP);

				beg_index_for_remove = start_indexBP;
				num_beg_frag++;

				if(pFragR != 0)
				{
					end_indexBP = pFragR->ConvGPtoBP(end_indGP);
//					size_data_for_remove += CutLeftPartFrag(pFragR, end_indexBP);
					size_data_for_remove += pFragR->CutLeftPart(end_indexBP);
				}
			}

			num_end_frag--;
			if(num_end_frag >= num_beg_frag)//если есть фраги которые надо удалить целиком
			{
				POSITION posFrag = lst_fragment.FindIndex(num_beg_frag);
				VERIFY(posFrag);

				while (posFrag != NULL)
				{
					pFrag = lst_fragment.GetNext(posFrag);
					VERIFY(pFrag);
					if(pFrag->num > num_end_frag) break;
					if(pFrag != curFrag) size_data_for_remove += pFrag->Clear();
				}
			}
		}

		size_data_for_remove = RemoveData(beg_index_for_remove, size_data_for_remove, rem_elm);
		curFrag->SetBegBP(curFrag->GetBegBP() - size_data_for_remove);

		num_end_frag++;
		shift = -(end_indGP - beg_indGP + 1);
		int new_num = 0;

		POSITION pos1, pos2; 
		pos1 = lst_fragment.GetHeadPosition();
		while (pos1 != NULL)
		{
			pos2 = pos1;
			pFrag = lst_fragment.GetNext(pos1);
			VERIFY(pFrag);
			if(pFrag->GetSize() == 0 && pFrag != curFrag) 
			{
				lst_fragment.RemoveAt(pos2);
				delete pFrag; 
				pFrag = NULL;
			}
			else
			{
				if(pFrag->num >= num_end_frag) 
				{
					if(pFrag != curFrag) pFrag->ShiftLocal(-size_data_for_remove); 
					pFrag->ShiftGlobal(shift);
				}
				pFrag->num = new_num++;
			}
		}
        return size_data_for_remove;
    }

	double GetLifeTime(int beg_indGP, int end_indGP)
	{
		if(pBufBase == NULL) return 0;
		int num_beg_frag, num_end_frag;
		TFragment<T> *pFragL, *pFragR, *pFrag;

		pFragL = FindeFragment(beg_indGP, 0, num_beg_frag);
		pFragR = FindeFragment(end_indGP, num_beg_frag, num_end_frag);

		double life_time = 0;
		if(pFragL == pFragR && pFragL != 0) return pBufBase->GetElement(end_indGP) - pBufBase->GetElement(beg_indGP); 
		else
		{
			if(pFragL == 0)
			{
				if(pFragR != 0)
				{
					life_time = pBufBase->GetElement(end_indGP) - pBufBase->GetElement(pFragR->GetBegGP()); 
				}
			}
			else 
			{
				life_time = pBufBase->GetElement(pFragL->GetEndGP()) - pBufBase->GetElement(beg_indGP); 
				num_beg_frag++;

				if(pFragR != 0)
				{
					life_time += pBufBase->GetElement(end_indGP) - pBufBase->GetElement(pFragR->GetBegGP()); 
				}
			}

			num_end_frag--;
			if(num_end_frag >= num_beg_frag)
			{
				POSITION posFrag = lst_fragment.FindIndex(num_beg_frag);
				VERIFY(posFrag);

				while (posFrag != NULL)
				{
					pFrag = lst_fragment.GetNext(posFrag);
					VERIFY(pFrag);
					if(pFrag->num > num_end_frag) break;
					life_time += pBufBase->GetElement(pFrag->GetEndGP()) - pBufBase->GetElement(pFrag->GetBegGP());
				}
			}
		}
		return life_time;
	}

	double GetFullLifeTime()
	{
		int beg_indGP, end_indGP;
		if(pBufBase == NULL) return 0;
		beg_indGP = 0;
		end_indGP = pBufBase->GetSizeData() - 1;
		if(end_indGP <= 0) return 0.0;
		return GetLifeTime(beg_indGP, end_indGP);
	}
	double GetGetLastDeltaX()
	{
		int end_indGP;
		if(pBufBase == NULL) return 0;
		end_indGP = pBufBase->GetSizeData() - 2;
		if(end_indGP <= 0) return 0.0;
		return GetLifeTime(end_indGP - 1, end_indGP);
	}
	double GetDrv(double deltaX)
	{
		double dY, dX, X1, X2, Y1, Y2;
		int start_indexGP, end_indexGP;
		int start_indexBP, end_indexBP;

		if(GetSizeData() < 2 || curFrag->GetSize() < 2) return 0.0f;

		end_indexBP = curFrag->GetEndBP();

		end_indexGP = curFrag->GetEndGP();

		X2 = pBufBase->GetElement(end_indexGP);

		if(deltaX == 0.0f) start_indexGP = end_indexGP - 1;
		else start_indexGP = pBufBase->FindIndexRevers(X2 - deltaX);

		if(start_indexGP < 0) return 0;

		int num_beg_frag;
		TFragment<T> *pFragL = FindeFragment(start_indexGP, 0, num_beg_frag);
		if(pFragL != 0) start_indexBP = pFragL->ConvGPtoBP(start_indexGP);
		else
		{
			pFragL = GetFrag(num_beg_frag);
			if(pFragL == NULL) return 0;//куку, возврвщаем 0
			start_indexBP = pFragL->GetBegBP();
			start_indexGP = pFragL->ConvBPtoGP(start_indexBP);
		}
		X1 = pBufBase->GetElement(start_indexGP);

		Y1 = GetElement(start_indexBP);
		Y2 = GetElement(end_indexBP);

		dY = Y2 - Y1; 

		dX = X2 - X1;
		if(dX == 0) return 0;

		return dY/dX;
	}

	double GetDrvInv(double deltaY)
	{
		double dY, dX, X1, X2, Y1, Y2;
		int start_indexGP, end_indexGP;
		int start_indexBP, end_indexBP;

		if(GetSizeData() < 2 || curFrag->GetSize() < 2) return 0.0f;

		end_indexBP = curFrag->GetEndBP();

		end_indexGP = curFrag->GetEndGP();

		Y2 = GetElement(end_indexBP);

		if(deltaY == 0.0f) start_indexBP = end_indexBP - 1;
		else start_indexBP = FindIndexRevers(Y2 - deltaY);

		if(start_indexBP < 0) return 0;

		TFragment<T> *pFragL = FindeFragment(start_indexBP);
		if(pFragL == NULL) return 0;

		start_indexGP = pFragL->ConvBPtoGP(start_indexBP);
		if(start_indexGP < 0) return 0;
		X1 = pBufBase->GetElement(start_indexGP);
		Y1 = GetElement(start_indexBP);
		X2 = pBufBase->GetElement(end_indexGP);

		dY = Y2 - Y1; 

		dX = X2 - X1;
		if(dX == 0) return 0;

		return dY/dX;
	}

	int inline CopyData(int beg_indGP, int end_indGP, FRingBuf<T>* f_ringbuf = NULL)
	{
		if(lst_fragment.GetCount() == 0) return 0;
		int num_beg_frag, num_end_frag;
		int start_indexBP, end_indexBP;
		int shift;

		TFragment<T> *pFragL, *pFragR, *pFrag;

		pFragL = FindeFragment(beg_indGP, 0, num_beg_frag);
		pFragR = FindeFragment(end_indGP, num_beg_frag, num_end_frag);

		int beg_index_for_remove, size_data_for_remove = 0; 
		if(pFragL == pFragR && pFragL != 0) 
		{
			start_indexBP = pFragL->ConvGPtoBP(beg_indGP);
			end_indexBP = pFragR->ConvGPtoBP(end_indGP);
			size_data_for_remove = pFragL->CutInnerPart(start_indexBP, end_indexBP);
			beg_index_for_remove = start_indexBP;
			num_beg_frag++;
		}
		else
		{
			if(pFragL == 0)
			{
				if((pFrag = GetFrag(num_beg_frag)) == 0) return 0;
				beg_index_for_remove = pFrag->GetBegBP();
				if(pFragR != 0)
				{
					end_indexBP = pFragR->ConvGPtoBP(end_indGP);
					size_data_for_remove = pFragR->CutLeftPart(end_indexBP);
				}
			}
			else 
			{
				start_indexBP = pFragL->ConvGPtoBP(beg_indGP);
				size_data_for_remove = pFragL->CutRightPart(start_indexBP);

				beg_index_for_remove = start_indexBP;
				num_beg_frag++;

				if(pFragR != 0)
				{
					end_indexBP = pFragR->ConvGPtoBP(end_indGP);
					size_data_for_remove += pFragR->CutLeftPart(end_indexBP);
				}
			}

			num_end_frag--;
			if(num_end_frag >= num_beg_frag)
			{
				POSITION posFrag = lst_fragment.FindIndex(num_beg_frag);
				VERIFY(posFrag);

				while (posFrag != NULL)
				{
					pFrag = lst_fragment.GetNext(posFrag);
					VERIFY(pFrag);
					if(pFrag->num > num_end_frag) break;
					if(pFrag != curFrag) size_data_for_remove += pFrag->Clear();
				}
			}
		}

		size_data_for_remove = RemoveData(beg_index_for_remove, size_data_for_remove, rem_elm);
		curFrag->SetBegBP(curFrag->GetBegBP() - size_data_for_remove);

		num_end_frag++;
		shift = -(end_indGP - beg_indGP + 1);
		int new_num = 0;

		POSITION pos1, pos2; 
		pos1 = lst_fragment.GetHeadPosition();
		while (pos1 != NULL)
		{
			pos2 = pos1;
			pFrag = lst_fragment.GetNext(pos1);
			VERIFY(pFrag);
			if(pFrag->GetSize() == 0 && pFrag != curFrag) 
			{
				lst_fragment.RemoveAt(pos2);
				delete pFrag; 
				pFrag = NULL;
			}
			else
			{
				if(pFrag->num >= num_end_frag) 
				{
					if(pFrag != curFrag) pFrag->ShiftLocal(-size_data_for_remove); 
					pFrag->ShiftGlobal(shift);
				}
				pFrag->num = new_num++;
			}
		}
		return size_data_for_remove;
	}

    void GetMedianaFiltr(int iStep)
    {
        if(lst_fragment.GetCount() == 0) return;
        POSITION posFrag = lst_fragment.GetHeadPosition();
        TFragment<T>* pFrag;
        int sz_data = GetSizeData();
        if(sz_data <= 0) return;

        T *p_bufIn, *p_bufOut;
        int sz_frag;

        p_bufIn = new T[sz_data];
        p_bufOut = new T[sz_data];

        while (posFrag != NULL)
        {
            pFrag = lst_fragment.GetNext(posFrag);
            sz_frag = pFrag->GetSize();
            if(sz_frag > 0 && sz_frag >= iStep)
            {
                CopyData(pFrag->begin, sz_frag, p_bufIn);
                MedianFilter(p_bufIn, p_bufOut, sz_frag, iStep);
                PasteData(pFrag->begin, sz_frag, p_bufOut);
            }
        }

        delete p_bufIn;
        delete p_bufOut;
    }

    int GetSizeIterFrag()
    {
        if(p_iterFrag == NULL) return 0;
        return p_iterFrag->GetSize();
    }

	int GetTypeIterFrag()
	{
		if(p_iterFrag == NULL) return 0;
		return p_iterFrag->typeI;
	}

    ~FRingBuf()
    {
        RemoveAllFragments();
        DeleteAllMarker();
   }
    TMarker<T>* CreateMarker(int pos_element, CString name_m, int t_mark = 0)
    {
        TMarker<T> *pMark;
        if(map_marker.Lookup(curent_num_marker, pMark)) return NULL;

        TMarker<T> *m = new TMarker<T>(pos_element, curent_num_marker++, name_m, t_mark);
        if(pos_element >= 0 && pos_element <= sizeData - 1) m->base_position = pos_element;
        else m->base_position = sizeData - 1;

        map_marker[m->num_marker] = m;
        m->SetVisible(true);
        int mm = map_marker.GetCount();
        return m;
    }
    TMarker<T>* CreateMarker(int pos_element, CString name_m, COLORREF clr_m, COLORREF clr_n, int t_mark = 0)
    {
        TMarker<T> *pMark;
        if(map_marker.Lookup(curent_num_marker, pMark)) return NULL;

        TMarker<T> *m = new TMarker<T>(pos_element, curent_num_marker++, name_m, clr_m, clr_n, t_mark);
        if(pos_element >= 0 && pos_element <= sizeData - 1) m->base_position = pos_element;
        else m->base_position = sizeData - 1;

        map_marker[m->num_marker] = m;
        m->SetVisible(true);
        int mm = map_marker.GetCount();
        return m;
    }
    TMarker<T>* CreateMarker(int pos_element, CString name_m, COLORREF clr_m, COLORREF clr_n, T value, char ch_mark)
    {
        TMarker<T> *pMark;
        if(map_marker.Lookup(curent_num_marker, pMark)) return NULL;

        TMarker<T> *m = new TMarker<T>(pos_element, curent_num_marker++, name_m, clr_m, clr_n, value, ch_mark);
        if(pos_element >= 0 && (pos_element <= sizeData - 1)) m->base_position = pos_element;
        else m->base_position = sizeData - 1;

        map_marker[m->num_marker] = m;
        m->SetVisible(true);
        int mm = map_marker.GetCount();
        return m;
    }
    BOOL GetMarker(TMarker<T>& m, int num_marker)
    {
        TMarker *pMark;
        if(!map_marker.Lookup(num_marker, pMark)) return false;
        m = *(map_marker[num_marker]);
        return true;
    }
    int GetNumMarkers()
    {
        return map_marker.GetCount();
    }
    int GetTBLPtrMarkers(TMarker<T> **tbl_marker)
    {
        if(map_marker.GetCount() == 0) return 0; 
        int mKey;
        TMarker<T> *pMark;
        POSITION pos = map_marker.GetStartPosition();
        int index = 0;
        while (pos != NULL)
        {
            map_marker.GetNextAssoc(pos, mKey, pMark);
            tbl_marker[index++] = pMark;
        }
        return index;
    }
    int GetTBLNumMarkers(int *tbl_marker)
    {
        if(map_marker.GetCount() == 0) return 0; 
        int mKey;
        TMarker<T> *pMark;
        POSITION pos = map_marker.GetStartPosition();
        int index = 0;
        while (pos != NULL)
        {
            map_marker.GetNextAssoc(pos, mKey, pMark);
            tbl_marker[index++] = mKey;
        }
        return index;
    }
    BOOL DeleteMarker(int num_marker)
    {
        TMarker<T> *pMark;
        if(!map_marker.Lookup(num_marker, pMark)) return false;
        TMarker<T> *m = map_marker[num_marker];
        map_marker.RemoveKey(num_marker);
        delete m;
        return true;
    }
    void DeleteAllMarker()
    {
        if(map_marker.GetCount() == 0) return; 
        int mKey;
        TMarker<T> *pMark;
        POSITION pos = map_marker.GetStartPosition();
        while (pos != NULL)
        {
            map_marker.GetNextAssoc(pos, mKey, pMark);
            if(pMark != NULL) delete pMark;
        }
        map_marker.RemoveAll();
    }
    void Clear0()
    {
        RemoveAllFragments();
        DeleteAllMarker();
        RingBuf<T>::Clear0();
    }

    BOOL Get_flg_view_reper()
    {
        return flg_view_reper;
    }

    void Set_flg_view_reper(BOOL flg)
    {
        flg_view_reper = flg;
    }

    BOOL Get_flg_view_data()
    {
        return flg_view_data;
    }
    void Set_flg_view_data(BOOL flg)
    {
        flg_view_data = flg;
    }
};

template <class T>
class VFRingBuf : public BufferExtendedParametrs<T>
{
    TMarker<T> UMarker[4];
    FRingBuf<T>* curRBuf;
    BOOL fl_extern_buf;
public:
    int num_self;

    VFRingBuf()
    {
        fl_extern_buf = false;
        curRBuf = NULL;
        num_self = 0;
        UMarker[0].base_position = 0;
        UMarker[1].base_position = 0;
        UMarker[2].base_position = 0;
        UMarker[3].base_position = 0;
    }
    VFRingBuf(int base_pos, FRingBuf<T>& pBuf)
    {
        if(!flag_external) curRBuf = new FRingBuf<T>(base_pos, pBuf);
        else curRBuf = pBuf;
        curent_num_marker = 0;
        flg_view_data = true;
        curRBuf->fl_extern_buf = fl_extern_buf = flag_external;
        num_self = 0;
        UMarker[0].base_position = 0;
        UMarker[1].base_position = 0;
        UMarker[2].base_position = 0;
        UMarker[3].base_position = 0;
    }
    VFRingBuf(int base_pos, VFRingBuf<T>& pBuf)
    {
        operator=(pBuf);
        curRBuf->fl_extern_buf = fl_extern_buf = false;
        curRBuf->base_position = base_pos;
    }
    VFRingBuf(int base_pos, VFRingBuf<T>* pBuf)
    {
        operator=(*pBuf);
        curRBuf->fl_extern_buf = fl_extern_buf = false;
        curRBuf->base_position = base_pos;
    }
    VFRingBuf(int size_buf, int size_data, int base_pos, COLORREF clr, T *data = NULL)
    {
        curRBuf = new FRingBuf<T>(size_buf, size_data, base_pos, clr, data);
        SetBaseColor(clr);
        curRBuf->fl_extern_buf = fl_extern_buf = false;
        SetPosUMarker(1, 0);
        SetPosUMarker(2, curRBuf->GetCur());
        SetPosUMarker(0, 0); 
        SetPosUMarker(3, curRBuf->GetCur());
    }
    VFRingBuf(VFRingBuf<T>& pBuf)
    {
        operator=(pBuf);
    }
    VFRingBuf(FRingBuf<T>& pBuf)
    {
        curRBuf = new FRingBuf<T>(pBuf);
        fl_extern_buf = false;
        curRBuf = NULL;
        num_self = 0;
        UMarker[0].base_position = 0;
        UMarker[1].base_position = 0;
        UMarker[2].base_position = 0;
        UMarker[3].base_position = 0;
    }
    VFRingBuf(VFRingBuf<T>* pBuf)
    {
        operator=(*pBuf);
    }
    VFRingBuf(FRingBuf<T>* pBuf)
    {
        curRBuf = pBuf;
        fl_extern_buf = false;
        curRBuf = NULL;
        num_self = 0;
        UMarker[0].base_position = 0;
        UMarker[1].base_position = 0;
        UMarker[2].base_position = 0;
        UMarker[3].base_position = 0;
    }
    VFRingBuf<T>& operator=(VFRingBuf<T>& pBuf)
    {
        fl_extern_buf = pBuf.fl_extern_buf;
        if(fl_extern_buf) curRBuf = pBuf.curRBuf;
        else 
        {
            curRBuf = new FRingBuf<T>(pBuf.curRBuf);
            UMarker[0] = pBuf.UMarker[0];
            UMarker[1] = pBuf.UMarker[1];
            UMarker[2] = pBuf.UMarker[2];
            UMarker[3] = pBuf.UMarker[3];
        }
        return *this;
    }
    void Init(int size_buf, int size_data, int base_pos, T *data = NULL)
    {
        if(curRBuf == NULL) 
            curRBuf = new FRingBuf<T>(size_buf, size_data, base_pos, 0, data);
        else curRBuf->Init(size_buf, size_data, base_pos, data);
    }

    BOOL Init(int beg_index, int size_data, T *data = NULL)
    {
        if(curRBuf == NULL) return false;
        curRBuf->Init(beg_index, size_data, data);
        return true;
    }
    void Init(FRingBuf<T>& pBuf)
    {
        if(curRBuf == NULL) curRBuf = new FRingBuf<T>(pBuf);
        else curRBuf->Init(&pBuf);
    }
    void Init(FRingBuf<T>* pBuf)
    {
        if(curRBuf == NULL) curRBuf = new FRingBuf<T>(pBuf);
        else curRBuf->Init(pBuf);
    }
    void SetExternalBuf(int base_pos, FRingBuf<T>& pBuf)
    {
        curRBuf = &pBuf;
        curRBuf->fl_extern_buf = fl_extern_buf = TRUE;
        curRBuf->base_position = base_pos; 
    }
    int SetPosUMarker(int num_mark, int pos_ele)
    {
        if(curRBuf == NULL) return -1;
        if(num_mark >= 4) return 0;
        int nn = 0;
        if(curRBuf->GetSizeData() == 0) return 0;
        if(pos_ele >= 0 && pos_ele < (int)curRBuf->GetSizeData()) nn = pos_ele;

        switch(num_mark)
        {
        case 0:
            if(nn > UMarker[1].base_position) UMarker[1].base_position = nn;
        	break;
        case 1:
            if(nn < UMarker[0].base_position) nn = UMarker[0].base_position;
            break;
        case 2:
            if(nn > UMarker[3].base_position) nn = UMarker[3].base_position;
            break;
        case 3:
            if(nn < UMarker[2].base_position) UMarker[2].base_position = nn;
            break;
        default:
            break;
        }
        UMarker[num_mark].base_position = nn;
        return nn;
    }
    void SetPosUMarker2ToEndBuf()
    {
        if(curRBuf == NULL) return;
        UMarker[2].base_position = (int)curRBuf->GetSizeData() - 1;
    }
    void SetPosUMarker3ToEndBuf()
    {
        if(curRBuf == NULL) return;
        UMarker[3].base_position = (int)curRBuf->GetSizeData() - 1;
    }
    void SetPosUMarker23ToEndBuf()
    {
        if(curRBuf == NULL) return;
        int nn = (int)curRBuf->GetSizeData() - 1;
        if(nn < 0) nn = 0;
        UMarker[2].base_position = nn;
        UMarker[3].base_position = nn;
    }
    void ResetAllUMarkers()
    {
        UMarker[0].base_position = 0;
        UMarker[1].base_position = 0;
        UMarker[2].base_position = 0;
        UMarker[3].base_position = 0;
    }
    int GetPosUMarker(int num_mark)
    {
        return UMarker[num_mark].base_position;
    }
    T GetDataUMarker(int num_mark)
    {
        ASSERT(curRBuf);
        if(curRBuf->GetSizeData() == 0) return 0;
        return curRBuf->GetElement(UMarker[num_mark].base_position);
    }
    int GetDiffPosUMarker(int num_mark1, int num_mark2)
    {
        return UMarker[num_mark1].base_position - 
            UMarker[num_mark2].base_position + 1;
    }
    T NormData()
    {
        ASSERT(curRBuf);
        return maxF = curRBuf->NormData();
    }
    COLORREF ReplacePenColor(COLORREF clr)
    {
        return BufferExtendedParametrs<T>::ReplacePenColor(clr);
    }
    void SetBaseColor(COLORREF clr, BOOL flag_color = false)
    {
        BufferExtendedParametrs<T>::SetBaseColor(clr);
        if(flag_color && (curRBuf != NULL)) curRBuf->SetAllColor(clr);
    }
    int ReplacePenThick(int thick)
    {
        return BufferExtendedParametrs<T>::ReplacePenThick(thick);
    }
    int GetPenThick()
    {
        return BufferExtendedParametrs<T>::GetPenThick();
    }
    void AddData(int size_data, int base_pos, T* inBuf)
    {
        if(curRBuf == NULL) return;
        curRBuf->AddData(size_data, base_pos, colorBase, inBuf);
        NormData();
    }
    T SetMinMax(T scFmin, T scFmax)
    {
        return BufferExtendedParametrs<T>::SetMinMax(scFmin, scFmax);
    }
    T GetMinMax(T& scFmin, T& scFmax)
    {
        return BufferExtendedParametrs<T>::GetMinMax(scFmin, scFmax);
    }
    T SetMinMaxDiapazon(T scFminS, T scFmaxS)
    {
        return BufferExtendedParametrs<T>::SetMinMaxDiapazon(scFminS, scFmaxS);
    }
    T SetMinMaxDiapazonEX(T scFminS, T scFmaxS)
    {
        return BufferExtendedParametrs<T>::SetMinMaxDiapazonEX(scFminS, scFmaxS);
    }
    T SetDiapazonDefault()
    {
        return BufferExtendedParametrs<T>::SetMinMaxDiapazon(curRBuf->GetMin(), curRBuf->GetMax());
    }
    T SetDiapazonDefaultEX()
    {
        return BufferExtendedParametrs<T>::SetMinMaxDiapazonEX(curRBuf->GetMin(), curRBuf->GetMax());
    }
    T GetMinMaxDiapazon(T& scFminS, T& scFmaxS)
    {
        return BufferExtendedParametrs<T>::GetMinMaxDiapazon(scFminS, scFmaxS);
    }
    void CalcMinMaxDiapazon(T& scFminS, T& scFmaxS)
    {
        if(curRBuf == NULL) return;
        scFminS = curRBuf->GetMin();
        scFmaxS = curRBuf->GetMax();
    }
    T GetDiffMaxMin()
    {
        return BufferExtendedParametrs<T>::GetDiffMaxMin();
    }
    T GetMaxF()
    {
        return BufferExtendedParametrs<T>::maxF;
    }
    int FindIndexInViewRange(T value)
    {
        if(curRBuf == NULL) return 0;
        return curRBuf->FindIndex(GetPosUMarker(1), GetPosUMarker(2), value);
    }

    inline int GetNumMarkers()
    {
        if(curRBuf == NULL) return 0;
        return curRBuf->GetNumMarkers();
    }

    int GetTBLPtrMarkers(TMarker<T> **tbl_marker)
    {
        if(curRBuf == NULL) return 0;
        return curRBuf->GetTBLPtrMarkers(tbl_marker);
    }
    BOOL Get_flg_view_reper()
    {
        if(curRBuf == NULL) return 0;
        return curRBuf->Get_flg_view_reper();
    }
    BOOL Get_flg_view_data()
    {
        if(curRBuf == NULL) return 0;
        return curRBuf->Get_flg_view_data();
    }

    FRingBuf<T>* GetCurRBuf()
    {
        return curRBuf;
    }

    BOOL GetSizeData()
    {
        if(curRBuf == NULL) return 0;
        return curRBuf->GetSizeData();
    }
    int FindIndex(int start_index, int end_index, T value)
    {
        if(curRBuf == NULL) return -1;
        return curRBuf->FindIndex(start_index, end_index, value);
    }

    int Get_nn_iter_frag()
    {
        if(curRBuf == NULL) return -1;
        return curRBuf->nn_iter_frag;
    }

    int GetSizeIterFrag()
    {
        if(curRBuf == NULL) return 0;
        return curRBuf->GetSizeIterFrag();
    }

    TFragment<T>* GetCurFrag()
    {
        if(curRBuf == NULL) return 0;
        return curRBuf->GetCurFrag();
    }

    TMarker<T>* CreateMarker(int pos_element, CString name_m, int t_mark = 0)
    {
        if(curRBuf == NULL) return NULL;
        return curRBuf->CreateMarker(pos_element, name_m, t_mark);
    }

    TMarker<T>* CreateMarker(int pos_element, CString name_m, COLORREF clr_m, COLORREF clr_n, int t_mark = 0)
    {
        if(curRBuf == NULL) return NULL;
        return curRBuf->CreateMarker(pos_element, name_m, clr_m, clr_n, t_mark);
    }
    TMarker<T>* CreateMarker(int pos_element, CString name_m, COLORREF clr_m, COLORREF clr_n, T value, char ch_mark)
    {
        if(curRBuf == NULL) return NULL;
        return curRBuf->CreateMarker(pos_element, name_m, clr_m, clr_n, value, ch_mark);
    }

    void inline SetPoint(int index_element, T data)
    {
        if(curRBuf == NULL) return;
        curRBuf->SetPoint(index_element, data);
    }

    T GetLastData()
    {
        if(curRBuf == NULL) return 0;
        return GetElement(GetPosUMarker(3));
    }

    T GetFirstData()
    {
        if(curRBuf == NULL) return 0;
        return GetElement(GetPosUMarker(0));
    }

    int GetCur()
    {
        if(curRBuf == NULL) return 0;
        return curRBuf->GetCur();
    }
    int FindeIndexInFrag(int indexGP)
    {
        if(curRBuf == NULL) return 0;
        curRBuf->FindeIndexInFrag(indexGP);
    }

    void SetBase(int base_pos)
    {
        if(curRBuf != NULL) curRBuf->base_position = base_pos;
    }

    int GetBase()
    {
        if(curRBuf != NULL) return curRBuf->base_position;
        else return 0;
    }

    int inline AddOneElementToEnd(T data)
    {
        if(curRBuf == NULL) return 0;
        return AddOneElementToEnd(data);
    }

    T GetElement(int num_element)
    {
        if(curRBuf == NULL) return 0;
        return curRBuf->GetElement(num_element);
    }
    ~VFRingBuf()
    {
		if(fl_extern_buf) curRBuf = NULL;
		else
		{
			if(curRBuf != NULL) 
			{
				if(!curRBuf->fl_extern_buf) delete curRBuf;
				curRBuf = NULL;
			}
		}
    }
};
template <class F>
class GRData
{
    VFRingBuf<F> *pMain;
    VFRingBuf<F> *cur_iter_Buf;
    VFRingBuf<F> *p_tmpBuf;
    TFragment<F>* p_curFrag;
    FRingBuf<F> *pMain_Iter_curRBuf, *p_curent_Iter_curRBuf;
public:
    typedef CMap<int, int, VFRingBuf<F>*, VFRingBuf<F>*&> VFRingBufMap;
    VFRingBufMap map_buf;
    int	cur_num_frag, cur_nup_point;
    GRData()
    {
        pMain  = NULL;
        cur_iter_Buf = NULL;
    }
    GRData(int size_buf)
    {
        pMain = new VFRingBuf<F>(size_buf, 0, 0, FALSE);
        cur_iter_Buf = NULL;
    }
    GRData(int size_buf, int size_data, int base_pos, const F *x_data = NULL)
    {
        pMain = new VFRingBuf<F>(size_buf, size_data, base_pos, x_data);
        cur_iter_Buf = NULL;
    }
    GRData(const RingBuf<F>* x_Buf)
    {
        pMain = VFRingBuf<F>(x_Buf);
        cur_iter_Buf = NULL;
    }
    GRData(int base_pos, const FRingBuf<F>* x_Buf, BOOL flag_external = false)
    {
        pMain = VFRingBuf<F>(base_pos, x_Buf, flag_external);
        cur_iter_Buf = NULL;
    }
    void SetViewInitDataX()
    {
        if(pMain == NULL) return;
        pMain->SetPosUMarker(1, 0);
        pMain->SetPosUMarker23ToEndBuf();

        pMain->SetMinMax(pMain->GetDataUMarker(1), pMain->GetDataUMarker(2));
        pMain->SaveMinMax();
    }

    void InitX(int size_buf, int size_data, int base_pos, F* data = NULL)
    {
        if(pMain == NULL) pMain = new VFRingBuf<F>(size_buf, size_data, base_pos, 0, data);
        else pMain->Init(size_buf, size_data, base_pos, data);
        SetViewInitDataX();
    }
    void InitX(VFRingBuf<F>& pBuf)
    {
        if(pMain == NULL) pMain = new VFRingBuf<F>(pBuf);
        *pMain = pBuf;
        SetViewInitDataX();
    }
    void InitX(FRingBuf<F>& pBuf)
    {
        if(pMain == NULL) pMain = new VFRingBuf<F>(pBuf);
        else pMain->Init(pBuf);
        SetViewInitDataX();
    }
    void InitX(VFRingBuf<F>* pBuf)
    {
        if(pMain == NULL) pMain = new VFRingBuf<F>(pBuf);
        *pMain = *pBuf;
        SetViewInitDataX();
    }
    void InitX(FRingBuf<F>* pBuf)
    {
        if(pMain == NULL) pMain = new VFRingBuf<F>(pBuf);
        else pMain->Init(pBuf);
        SetViewInitDataX();
    }
    VFRingBuf<F>* InitY(int num_buf, int size_buf, int size_data, int base_pos, F* data = NULL)
    {
        if((p_tmpBuf = GetBuf(num_buf)) == NULL) return NULL;
        p_tmpBuf->Init(size_buf, size_data, base_pos, data);
        p_tmpBuf->NormData();
        return p_tmpBuf;
    }
    VFRingBuf<F>* InitY(int num_buf, int beg_index, int size_data, F* data = NULL)
    {
        if((p_tmpBuf = GetBuf(num_buf)) == NULL) return NULL;
        p_tmpBuf->Init(beg_index, size_data, data);
        p_tmpBuf->NormData();
        return p_tmpBuf;
    }
    VFRingBuf<F>* InitY(int num_buf, VFRingBuf<F>& pBuf)
    {
        if((p_tmpBuf = GetBuf(num_buf)) == NULL) return NULL;
        *p_tmpBuf = pBuf;
        p_tmpBuf->NormData();
        return p_tmpBuf;
    }
    VFRingBuf<F>* InitY(int num_buf, FRingBuf<F>& pBuf)
    {
        if((p_tmpBuf = GetBuf(num_buf)) == NULL) return NULL;
        p_tmpBuf->Init(pBuf);
        p_tmpBuf->NormData();
        return p_tmpBuf;
    }
    void SetExternalBufX(int base_pos, FRingBuf<F>& pBuf)
    {
        if(pMain == NULL) pMain = new VFRingBuf<F>;
        pMain->SetExternalBuf(base_pos, pBuf);
    }
    void SetExternalBufX(int base_pos, VFRingBuf<F>& pBuf)
    {
        if(pMain != NULL) delete pMain;
        pMain = &pBuf;
        pMain->SetBase(base_pos);
    }
    void SetExternalBufY(int base_pos, int num_buf, FRingBuf<F>& pBuf)
    {
        if((p_tmpBuf = GetBuf(num_buf)) == NULL) 
        {
            p_tmpBuf = new VFRingBuf<F>;
            p_tmpBuf->num_self = num_buf;
        }
        p_tmpBuf->SetExternalBuf(base_pos, pBuf);
        map_buf[num_buf] = p_tmpBuf;
        map_buf[num_buf]->SetBase(base_pos);

		FRingBuf<F>* bf = map_buf[num_buf]->GetCurRBuf();
		if(bf != NULL) bf->pBufBase = pMain->GetCurRBuf();
    }
    int SetExternalBufY(int base_pos, FRingBuf<F>& pBuf)
    {
        int num_buf = 0;
        num_buf = GetFreeNumBuf();
        p_tmpBuf = new VFRingBuf<F>;
        p_tmpBuf->num_self = num_buf;

        p_tmpBuf->SetExternalBuf(base_pos, pBuf);
        map_buf[num_buf] = p_tmpBuf;
        map_buf[num_buf]->SetBase(base_pos);
        return num_buf;
    }
    void SetExternalBufY(int base_pos, int num_buf, VFRingBuf<F>& pBuf)
    {
        if((p_tmpBuf = GetBuf(num_buf)) != NULL) delete p_tmpBuf;
        map_buf[num_buf] = &pBuf;
        map_buf[num_buf]->SetBase(base_pos);
    }
    GRData(GRData& pBuf)
    {
        operator=(pBuf);
    }
    GRData& operator=(GRData& pBuf)
    {
        DeleteAllBuffers();
        int nn = pBuf.map_buf.GetCount();
        if(nn != 0)
        {
            VFRingBuf<F> **m2 = (VFRingBuf<F>**)new LPVOID[nn];
            pBuf.GetTBLPtrBuffers(m2);
            for(int i = 0; i < nn; i++)
            {
                if(m2[i] != NULL) delete m2[i];
                CreateBuffer(*m2[i]);
            }
            delete m2;
        }
        if(pBuf.pMain != NULL) pMain = new VFRingBuf<F>(pBuf.pMain);
        else pMain = NULL;

        cur_iter_Buf = NULL;
        return *this;
    }
    VFRingBuf<F>* GetBuf(int num_buf)
    {
        if(num_buf < 0) return pMain;
        int nn = map_buf.GetCount();
        if(nn == 0) return NULL;
        if(!map_buf.Lookup(num_buf, p_tmpBuf)) return NULL;
        return p_tmpBuf;
    }
    int GetFreeNumBuf()
    {
        int num_buf = 0;
        if(map_buf.GetCount() != 0)
        {
            do {p_tmpBuf = GetBuf(num_buf++);
            } while(p_tmpBuf != NULL);
            num_buf--;
        }
        return num_buf;
    }
    int CreateBuffer(int base_pos)
    {
        int num_buf = 0;
        if(map_buf.GetCount() != 0)
        {
            do {p_tmpBuf = GetBuf(num_buf++);
            } while(p_tmpBuf != NULL);
            num_buf--;
        }
        map_buf[num_buf] = new VFRingBuf<F>(base_pos, *pMain);
        map_buf[num_buf]->num_self = num_buf;
        return num_buf;
    }
    int CreateBuffer(int size_buf, int size_data, int base_pos, COLORREF clr, F *f_data = NULL)
    {
        int num_buf = 0;
        if(map_buf.GetCount() != 0)
        {
            do {p_tmpBuf = GetBuf(num_buf++);
            } while(p_tmpBuf != NULL);
            num_buf--;
        }
        map_buf[num_buf] = new VFRingBuf<F>(size_buf, size_data, base_pos, clr, f_data);
        map_buf[num_buf]->NormData();
        map_buf[num_buf]->num_self = num_buf;
        return num_buf;
    }
    int CreateBuffer(VFRingBuf<F>& pBuf)
    {
        if(pMain == NULL) return -1;
        FRingBuf<F> *pMain_curRBuf = pMain->GetCurRBuf();
        if(pMain_curRBuf == NULL) return -1;

        int num_buf = 0;
        if(map_buf.GetCount() != 0)
        {
            do {p_tmpBuf = GetBuf(num_buf++);
            } while(p_tmpBuf != NULL);
            num_buf--;
        }
        map_buf[num_buf] = 
            new VFRingBuf<F>(pMain_curRBuf->GetBase() + pMain_curRBuf->GetCur(), pBuf);
        map_buf[num_buf]->num_self = num_buf;
        return num_buf;
    }
    int CreateBuffer(VFRingBuf<F>* pBuf)
    {
        if(pMain == NULL) return -1;

        int num_buf = 0;
        if(map_buf.GetCount() != 0)
        {
            do {p_tmpBuf = GetBuf(num_buf++);
            } while(p_tmpBuf != NULL);
            num_buf--;
        }
        map_buf[num_buf] = 
            new VFRingBuf<F>(pMain->GetBase() + pMain->GetCur(), pBuf);
        map_buf[num_buf]->num_self = num_buf;
        return num_buf;
    }
    int CreateBuffer(int base_pos, FRingBuf<F>& pBuf)
    {
        int num_buf = 0;
        if(map_buf.GetCount() != 0)
        {
            do {p_tmpBuf = GetBuf(num_buf++);
            } while(p_tmpBuf != NULL);
            num_buf--;
        }
        map_buf[num_buf] = new VFRingBuf<F>(base_pos, pBuf);
        map_buf[num_buf]->num_self = num_buf;
        return num_buf;
    }
    int CreateBuffer(FRingBuf<F>* pBuf)
    {
        int num_buf = 0;
        if(map_buf.GetCount() != 0)
        {
            do {p_tmpBuf = GetBuf(num_buf++);
            } while(p_tmpBuf != NULL);
            num_buf--;
        }
        map_buf[num_buf] = new VFRingBuf<F>(pBuf);
        map_buf[num_buf]->num_self = num_buf;
        return num_buf;
    }
    int CreateBufferInit(int size_buf, int size_data, int base_pos, COLORREF clr, F* f_data, F* x_data)
    {
        if(pMain != NULL) delete pMain;
        pMain = new VFRingBuf<F>(size_buf, size_data, base_pos, 0, x_data);
        return CreateBuffer(size_buf, size_data, 0, clr, f_data);
    }
    int CreateBufferInit(int base_pos, FRingBuf<F>* f_Buf, FRingBuf<F>* x_Buf)
    {
        if(pMain != NULL) delete pMain;
        pMain = new VFRingBuf<F>(base_pos, x_Buf);
        return CreateBuffer(base_pos, f_Buf);
    }
    int GetNumBuffers()
    {
        return map_buf.GetCount();
    }
    int GetTBLNumBuffers(int *tbl_buff)
    {
        if(map_buf.GetCount() == 0) return 0; 
        int mKey;
        VFRingBuf<F> *pBuf;
        POSITION pos = map_buf.GetStartPosition();
        int index = 0;
        while (pos != NULL)
        {
            map_buf.GetNextAssoc(pos, mKey, pBuf);
            tbl_buff[index++] = mKey;
        }
        return index;
    }
    int GetTBLPtrBuffers(VFRingBuf<F> **tbl_buff)
    {
        if(map_buf.GetCount() == 0) return 0; 
        int mKey;
        VFRingBuf<F> *pBuf;
        POSITION pos = map_buf.GetStartPosition();
        int index = 0;
        while (pos != NULL)
        {
            map_buf.GetNextAssoc(pos, mKey, pBuf);
            tbl_buff[index++] = pBuf;
        }
        return index;
    }
    int GetBufWithName(CString sName)
    {
        if(map_buf.GetCount() == 0) return -1; 
        int mKey;
        VFRingBuf<F> *pBuf;
        POSITION pos = map_buf.GetStartPosition();
        int index = 0;
        while (pos != NULL)
        {
            map_buf.GetNextAssoc(pos, mKey, pBuf);
            if(sName == pBuf->sName) return mKey;
        }
        return -1;
    }
    int inline CopyData(int num_buf, int start_element, int count_elements, F *data)
    {
        if(num_buf < 0) return -1;
        p_tmpBuf = GetBuf(num_buf);
        if(p_tmpBuf == NULL) return -1;
        return p_tmpBuf->CopyData(start_element, count_elements, data);
    }
    int inline PasteData0(int start_element, int count_elements, const F *data)
    {
        ASSERT(pMain);
        return pMain->PasteData(start_element, count_elements, data);
    }
    int inline PasteData(int num_buf, int start_element, int count_elements, const F *data)
    {
        if(num_buf < 0) return -1;
        p_tmpBuf = GetBuf(num_buf);
        if(p_tmpBuf == NULL) return -1;
        return p_tmpBuf->PasteData(start_element, count_elements, data);
    }
    int inline ShiftData(int num_buf, int start_element, int count_elements, int shift)
    {
        if(num_buf < 0) return -1;
        p_tmpBuf = GetBuf(num_buf);
        if(p_tmpBuf == NULL) return -1;
        return p_tmpBuf->ShiftData(start_element, count_elements, shift);
    }
    void AddPoint0(F x_data)
    {
        if(pMain == NULL) return;
        pMain->AddOneElementToEnd(x_data);
    }
    VFRingBuf<F>* AddToBuf(int num_buf, int count_elements, F* Ydata, F* Xdata)
    {
        if(num_buf < 0) return 0;
        FRingBuf<F> *pBuf_curRBuf = GetCurRBuf(num_buf);
        FRingBuf<F> *pMain_curRBuf = GetCurRBuf(-1);
        if(pBuf_curRBuf == NULL || pMain_curRBuf == NULL) return NULL;

        int nn1 = pBuf_curRBuf->GetFreePositions();
        int nn2 = pMain_curRBuf->GetFreePositions();
        if(nn1 == 0 || nn2 == 0) return 0;

        if(pBuf_curRBuf->GetCurIndex_GP() != pMain_curRBuf->GetCur()) pMain_curRBuf->AddFragment(pMain_curRBuf->GetCur(), 0, false); 

        if(count_elements == 1) 
        {
            pBuf_curRBuf->AddOneElementToEnd(*Ydata);
            if(Xdata != NULL) pMain_curRBuf->AddOneElementToEnd(*Xdata);
        }
        else 
        {
            if(count_elements > nn1) count_elements = nn1;
            if(count_elements > nn2) count_elements = nn2;
            pBuf_curRBuf->AddMultipleElementsToEnd(count_elements, Ydata);
            if(Xdata == NULL) pMain_curRBuf->ExtendDataIndexes(count_elements);
            else pMain_curRBuf->AddMultipleElementsToEnd(count_elements, Xdata);
        }

        return GetBuf(num_buf);
    }
    void AddData(int num_buf, int size_data, int base_pos, F* inBuf)
    {
        if(num_buf < 0) return;
        VFRingBuf<F> *pBuf = GetBuf(num_buf);
        pBuf->AddData(size_data, base_pos, inBuf);
    }
    BOOL RemoveBuffer(int num_buf)
    {
        if(map_buf.GetCount() == 0) return false;
        VFRingBuf<F> *pBuf;
        if(!map_buf.Lookup(num_buf, pBuf)) return false;
        map_buf.RemoveKey(num_buf);
        delete pBuf;
        return true;
    }
    void DeleteAllBuffers()
    {
        if (pMain != NULL)
        {
			delete pMain;
			pMain = NULL;
		}
        if (map_buf.GetCount() == 0)
			return; 
        int mKey;
        VFRingBuf<F> *pBuf;
        POSITION pos = map_buf.GetStartPosition();
        while (pos != NULL)
        {
            map_buf.GetNextAssoc(pos, mKey, pBuf);
            if (pBuf != NULL)
            {
				delete pBuf;
				pBuf = NULL;
			}
        }
        map_buf.RemoveAll();
    }
    void inline RemoveStartDataP(float size_fragment_in_persent)
    {
        int start_ind, end_index;
        pMain->curRBuf->RemoveStartDataP(size_fragment_in_persent, start_ind, end_index);
        if(map_buf.GetCount() == 0) return;
        int mKey;
        POSITION pos = map_buf.GetStartPosition();
        int index = 0;
        while (pos != NULL)
        {
            map_buf.GetNextAssoc(pos, mKey, p_tmpBuf);
            if(p_tmpBuf != NULL) p_tmpBuf->curRBuf->RemoveDataN(start_ind, end_index);
        }
    }
    BOOL ReplacePosMarker(double shift)
    {
        if(pMain == NULL) return false;
        F x_value;
        x_value = GetLast(pMain);
        if(x_value <= pMain->Fmax) return false;

        F df = (pMain->Fmax - pMain->Fmin);
        pMain->Fmin = x_value - df*(1.0 - shift);
        pMain->Fmax = x_value + df*shift;
		pMain->FmaxDiapazon = pMain->Fmax;
        int nn = pMain->FindIndex(pMain->GetPosUMarker(1), 
            pMain->GetPosUMarker(2), pMain->Fmin);
        pMain->SetPosUMarker(1, nn);
        return true;
    }
    BOOL ReplacePosMarker0(double shift)
    {
        if(pMain == NULL) return false;
        pMain->SetPosUMarker23ToEndBuf();
        F x_value;
        x_value = GetLast(pMain);

        F df = (pMain->Fmax - pMain->Fmin);
        pMain->Fmin = x_value - df*(1.0 - shift);
        pMain->Fmax = x_value + df*shift;
        pMain->FmaxDiapazon = pMain->Fmax;
        int nn = pMain->FindIndex(pMain->GetPosUMarker(1), 
            pMain->GetPosUMarker(2), pMain->Fmin);
        pMain->SetPosUMarker(1, nn);
        return true;
    }

    void SetCurColor(BOOL flag_color_frag)
    {
        if(flag_color_frag)
        {
            if(cur_iter_Buf == NULL) return;
            FRingBuf<F> *p_curRBuf = cur_iter_Buf->GetCurRBuf();
            if(p_curRBuf == NULL) return;
            COLORREF clr = p_curRBuf->GetCurFrgColor();
            if(cur_iter_Buf->colorDraw != clr) cur_iter_Buf->ReplacePenColor(clr);
        }
    }
    POINT SetIterPos(VFRingBuf<F> *pBuf, BOOL flag_color_marker)
    {
        POINT p_iter;
        p_iter.x = -1, p_iter.y = 0;
        if(pBuf == NULL || pMain == NULL) return p_iter;

        pMain_Iter_curRBuf = pMain->GetCurRBuf();
        if(pMain_Iter_curRBuf == NULL) return p_iter;

        p_curent_Iter_curRBuf = pBuf->GetCurRBuf();
        if(p_curent_Iter_curRBuf == NULL) return p_iter;

        cur_iter_Buf = pBuf;
 
        if(p_curent_Iter_curRBuf->lst_fragment.GetCount() == 0) return p_iter;
        int posUM0_1 = pMain->GetPosUMarker(1);
        int posUM0_2 = pMain->GetPosUMarker(2);

        int iter_pos = p_curent_Iter_curRBuf->SetIterPos(posUM0_1, posUM0_2);
        if(iter_pos >= 0) SetCurColor(flag_color_marker);
        if(iter_pos < 0) return p_iter;

        pMain_Iter_curRBuf->RingBuf<F>::SetIterPos(iter_pos);
        p_iter.x = iter_pos;
        p_iter.y = p_curent_Iter_curRBuf->p_iterFrag->ConvGPtoBP(iter_pos);
        return p_iter;
    }
    POINT inline GetNext0(F& val1, F& val2)
    {
        POINT p_iter;
        p_iter.x = -1, p_iter.y = 0;
        ASSERT(p_curent_Iter_curRBuf);
        ASSERT(pMain_Iter_curRBuf);

        p_iter.y = p_curent_Iter_curRBuf->GetNext(val1, false);
        p_iter.x = pMain_Iter_curRBuf->RingBuf<F>::GetNext(val2);
        if(p_iter.y < 0) 
        {
            int iter_pos = p_curent_Iter_curRBuf->p_iterFrag->GetBase();
            pMain_Iter_curRBuf->RingBuf<F>::SetIterPos(iter_pos);
        }
        return p_iter;
    }
    VFRingBuf<F>* SelectBuf(int num_buf)
    {
        if(num_buf < 0) return pMain;
        else return GetBuf(num_buf);
    }
    VFRingBuf<F>* GetSizeBuf(int num_buf, int& size_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        size_buf = p_tmpBuf->curRBuf->GetSizeBuf();
        return p_tmpBuf;
    }
    VFRingBuf<F>* GetSizeData(int num_buf, int& size_data)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        size_data = p_tmpBuf->GetSizeData();
        return p_tmpBuf;
    }
    int GetSizeData(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return -1;
        return p_tmpBuf->GetSizeData();
    }
    F SetMinMax(int num_buf, F scFmin, F scFmax)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->SetMinMax(scFmin, scFmax);
    }
    F GetMinMax(int num_buf, F& scFmin, F& scFmax)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->GetMinMax(scFmin, scFmax);
    }
    F SetMinMaxDiapazon(int num_buf, F scFminS, F scFmaxS)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        return p_tmpBuf->SetMinMaxDiapazon(scFminS, scFmaxS);
    }
    VFRingBuf<F>* SetMinMaxFullDiapazon(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        F scFminS, scFmaxS;
        GetMinMaxFullDiapazon(p_tmpBuf, scFminS, scFmaxS);
        p_tmpBuf->SetMinMaxDiapazonEX(scFminS, scFmaxS);
        return p_tmpBuf;
    }
    F GetMinMaxDiapazon(int num_buf, F& scFminS, F& scFmaxS)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->GetMinMaxDiapazon(scFminS, scFmaxS);
    }
    VFRingBuf<F>* GetMinMaxFullDiapazon(int num_buf, F& scFminS, F& scFmaxS)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        GetFirst(p_tmpBuf, scFminS);
        GetLast(p_tmpBuf, scFmaxS);
        return p_tmpBuf;
    }
    void GetMinMaxFullDiapazon(VFRingBuf<F>* pBuf, F& scFminS, F& scFmaxS)
    {
        if(pBuf == NULL) return;
        scFminS = GetFirst(pBuf);
        scFmaxS = GetLast(pBuf);
    }
    F GetDiffMaxMin(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        return p_tmpBuf->GetDiffMaxMin();
    }
    VFRingBuf<F>* GetDiffMaxMin(VFRingBuf<F>* pBuf, F& d_min_max)
    {
        if(pBuf == NULL) return NULL;
        d_min_max = pBuf->GetDiffMaxMin();
        return pBuf;
    }
    F GetMin0(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->GetMin0();
    }
    F GetMax0(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->GetMax0();
    }
    F GetMaxF(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->GetMaxF();
    }
    void SaveMinMax(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return;
        p_tmpBuf->SaveMinMax();
    }
    int GetThicknessDiagram(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->thickness;
    }
    int GetCur(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        return p_tmpBuf->GetCur();
    }
    F GetLast(VFRingBuf<F>* pBuf)
    {
        if(pBuf == NULL) return 0;
        return pBuf->GetLastData();
    }
    F GetLast(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->GetLastData();
    }
    F GetFirst(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->GetFirstData();
    }
    F GetFirst(VFRingBuf<F>* pBuf)
    {
        if(pBuf == NULL) return 0;
        return pBuf->GetFirstData();
    }
    VFRingBuf<F>* GetElement(int num_buf, int num_element, F& value)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        value = p_tmpBuf->GetElement(num_element);
        return p_tmpBuf;
    }
    int GetPrevLast(VFRingBuf<F>* pBuf, F& value)
    {
        if(pBuf == NULL) return NULL;
        return pBuf->curRBuf->GetPrevLast(value);
    }
    int GetPrevLast(int num_buf, F& value)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        return p_tmpBuf->curRBuf->GetPrevLast(value);
    }
    VFRingBuf<F>* GetFirstLast(int num_buf, F& value1, F& value2)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        value = p_tmpBuf->curRBuf->GetFirstLast(value1, value2);
        return p_tmpBuf;
    }
    VFRingBuf<F>* GetDiffLastFirst(int num_buf, F& f_delta)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        f_delta = p_tmpBuf->curRBuf->GetDiffLastFirst();
        return p_tmpBuf;
    }
    int GetLastDataPair(VFRingBuf<F> *pBuf, F& f_value, F& x_value)
    {
        if(pBuf == NULL || pMain == NULL) return -1;
        int nn;
        FRingBuf<F> *pMain_curRBuf = pMain->GetCurRBuf();
        FRingBuf<F> *pBuf_curRBuf = pBuf->GetCurRBuf();
        if(pMain_curRBuf == NULL || pBuf_curRBuf == NULL) return -1;

        if((nn = pBuf_curRBuf->GetCurDI_GP(f_value)) <= 0) return -1;
        int mm = pMain_curRBuf->GetCur();
        if(nn > mm)
        {
            pBuf_curRBuf->CutRightDataCurFrag(mm);
            f_value = pBuf_curRBuf->GetLastData();
            nn = mm;
        }
        x_value = pMain_curRBuf->GetElement(nn);
        return nn;
    }
    int GetPrevLastDataPair(VFRingBuf<F> *pBuf, F& f_value, F& x_value)
    {
        if(pBuf == NULL || pMain == NULL) return -1;
        int nn;
        FRingBuf<F> *pMain_curRBuf = pMain->GetCurRBuf();
        FRingBuf<F> *pBuf_curRBuf = pBuf->GetCurRBuf();
        if(pMain_curRBuf == NULL || pBuf_curRBuf == NULL) return -1;
 
        if((nn = pBuf_curRBuf->GetPrevLastDI_GP(f_value)) < 0) return -1;
        int mm = pMain_curRBuf->GetCur();
        if(nn > mm - 1) nn = mm - 1;
        x_value = pMain_curRBuf->GetElement(nn);
        return nn;
    }
    void SetViewRepers(BOOL flg_view_reper, int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return;
        FRingBuf<F> *pBuf_curRBuf = p_tmpBuf->GetCurRBuf();
        if(pBuf_curRBuf == NULL) return;
        pBuf_curRBuf->Set_flg_view_reper(flg_view_reper);
    }
    void SetViewData(BOOL flg_view_data, int num_buf)
    {
        if(num_buf < 0) return;
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return;
        FRingBuf<F> *pBuf_curRBuf = p_tmpBuf->GetCurRBuf();
        if(pBuf_curRBuf == NULL) return;
        pBuf_curRBuf->Set_flg_view_data(flg_view_data);
    }

    void SetPosViewMarkers(F min_value, F max_value)
    {
        if(pMain == NULL) return;
        int ind, sz_data = pMain->GetSizeData();
        if(sz_data == 0 || pMain == NULL) return;
        ind	= pMain->FindIndex(0, sz_data - 1, min_value);
        if(ind == -1) ind = 0; 
        pMain->SetPosUMarker(1, ind);
        ind = pMain->FindIndex(ind, sz_data - 1, max_value);
        if(ind == -1) ind = sz_data - 1; 
        pMain->SetPosUMarker(2, ind);
    }

    void SetPosViewMarkersDef()
    {
        if(pMain != NULL) SetPosViewMarkers(pMain->Fmin, pMain->Fmax);
    }

    inline F GetMouseValue(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->mouse_value;
    }

    inline CString GetName(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return "";
        return p_tmpBuf->sName;
    }

    inline void SetName(int num_buf, CString sName)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return;
        p_tmpBuf->sName = sName;
    }
    inline int GetNumMarkers(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->GetNumMarkers();
    }
    int GetTBLPtrMarkers(int num_buf, TMarker<F> **tbl_marker)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->GetTBLPtrMarkers(tbl_marker);
    }

    BOOL Get_flg_view_reper(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->Get_flg_view_reper();
    }
    BOOL Get_flg_view_data(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->Get_flg_view_data();
    }
    FRingBuf<F>* GetCurRBuf(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
        return p_tmpBuf->GetCurRBuf();
    }
    int GetPosUMarker(int num_buf, int num_mark)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return -1;
        return p_tmpBuf->GetPosUMarker(num_mark);
    }
    int SetPosUMarker(int num_buf, int num_mark, int pos_ele)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return -1;
        return p_tmpBuf->SetPosUMarker(num_mark, pos_ele);
    }
    void SetPosUMarker23ToEndBuf(int num_buf)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return;
        p_tmpBuf->SetPosUMarker23ToEndBuf();
    }
    int GetDiffPosUMarker(int num_buf, int num_mark1, int num_mark2)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return -1;
        return p_tmpBuf->GetDiffPosUMarker(num_mark1, num_mark2);
    }
    int FindIndex(int num_buf, int start_index, int end_index, F value)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return -1;
        return p_tmpBuf->FindIndex(start_index, end_index, value);
    }

    TMarker<F>* CreateMarker(int num_buf, int pos_element, CString name_m, int t_mark = 0)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        return p_tmpBuf->CreateMarker(pos_element, name_m, t_mark);
    }

    TMarker<F>* CreateMarker(int num_buf, int pos_element, CString name_m, COLORREF clr_m, COLORREF clr_n, int t_mark = 0)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
        return p_tmpBuf->CreateMarker(pos_element, name_m, clr_m, clr_n, t_mark);
    }
    TMarker<F>* CreateMarker(int num_buf, int pos_element, CString name_m, COLORREF clr_m, COLORREF clr_n, F value, char ch_mark)
    {
        if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return NULL;
		TMarker<F>* tmr = p_tmpBuf->CreateMarker(pos_element, name_m, clr_m, clr_n, value, ch_mark);
		if(tmr) tmr->num_buf = num_buf;
        return tmr;
    }

	int ClearBeginDataY(int num_point_for_delete)
	{
		if(pMain == NULL || num_point_for_delete == 0) return 0;
		FRingBuf<F> *pBuf = pMain->GetCurRBuf();
		if(pBuf == 0) return 0;
		int sz_data = pMain->GetSizeData();
		if(sz_data == 0) return 0;
		if(sz_data < num_point_for_delete) num_point_for_delete = sz_data;

		if(map_buf.GetCount() == 0) return num_point_for_delete;
		int mKey;
		POSITION pos = map_buf.GetStartPosition();
		int index = 0;
		VFRingBuf<F> *pVBuf;
		while (pos != NULL)
		{
			map_buf.GetNextAssoc(pos, mKey, pVBuf);
			if(pVBuf == NULL) continue;
			pBuf = pVBuf->GetCurRBuf();
			if(pBuf == NULL) continue;
			if(pBuf->flag_cleared) continue;
			pBuf->flag_cleared = true;
			pBuf->ClearBeginData(num_point_for_delete);
		}
		return num_point_for_delete;
	}
	int ClearBeginDataX(int num_point_for_delete)
	{
		if(pMain == NULL || num_point_for_delete == 0) return 0;
		FRingBuf<F> *pBuf = pMain->GetCurRBuf();
		if(pBuf == 0) return 0;
		int sz_data = pMain->GetSizeData();
		if(sz_data == 0) return 0;
		if(sz_data < num_point_for_delete) num_point_for_delete = sz_data;
		pBuf->ClearBeginData(num_point_for_delete);
		return num_point_for_delete;
	}

	void ResetFlagsClearedY()
	{
		if(pMain == NULL) return;
		FRingBuf<F> *pBuf = pMain->GetCurRBuf();
		if(pBuf == 0) return;

		if(map_buf.GetCount() == 0) return;
		int mKey;
		POSITION pos = map_buf.GetStartPosition();
		int index = 0;
		VFRingBuf<F> *pVBuf;
		while (pos != NULL)
		{
			map_buf.GetNextAssoc(pos, mKey, pVBuf);
			if(pVBuf != NULL) pVBuf->GetCurRBuf()->flag_cleared = false;
		}
	}
	int ClearBeginDataXY(int num_point_for_delete)
	{
		ClearBeginDataY(num_point_for_delete);
		int nn = ClearBeginDataX(num_point_for_delete);
		ResetFlagsClearedY();
		return nn;
	}

	double GetLifeTime(int num_buf, double start_time, double end_time)
	{
		if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
		FRingBuf<F>* pBuf = p_tmpBuf->GetCurRBuf();
		if(pBuf == 0) return 0;
		if(pMain == 0) return 0;

		int start_index = pMain->FindIndex(0, pMain->GetSizeData() - 1, start_time);
		if(start_index == -1) return 0;

		int end_index = pMain->FindIndex(start_index, pMain->GetSizeData() - 1, end_time);
		if(end_index == -1) return 0;

		return pBuf->GetLifeTime(start_index, end_index);
	}

	double GetFullLifeTime(int num_buf)
	{
		if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
		FRingBuf<F>* pBuf = p_tmpBuf->GetCurRBuf();
		if*(pBuf == 0) return 0;
		if(pMain == 0) return 0;

		return pBuf->GetFullLifeTime();
	}
	double GetDrv(int num_buf, double dX)
	{
		if((p_tmpBuf = SelectBuf(num_buf)) == NULL) return 0;
		FRingBuf<F>* pBuf = p_tmpBuf->GetCurRBuf();
		if(pBuf == 0) return 0;
		if(pMain == 0) return 0;

		return pBuf->GetDrv(dX);
	}
    ~GRData()
    {
        DeleteAllBuffers();
    }
};

#ifndef _SPOTPOSFUNC_H_
#define _SPOTPOSFUNC_H_

template <class X>
inline void Swap(X &x, X &y)
{
    X tmp = x;
    x = y;
    y = tmp;
}

template <class C>
C FindOrderedElement(C *a, int N, int k)
{
    C min = a[0];
    int indx;
    for (int i = 0; i < N; i++) {
        indx = i;
        min = a[i];
        for (int j = i+1; j < N; j++) {
            if (a[j] < min) {
                min = a[j];
                indx = j;
            }
        }
        Swap(a[i], a[indx]);
    }
    return a[k];
}
template <class X>
X GetMediaElement(X *pA, int N)
{
    return FindOrderedElement(pA, N, N / 2);
}

template <class T>
void MedianFilter(const T *pData_in, T *pData_out, int L, int iStep = 5)
{
    int iHalfStep = (iStep - 1) / 2;
    iStep = iHalfStep*2 + 1;

    for (int i=0; i<iHalfStep; i++) {
        pData_out[i] = pData_in[i];
        pData_out[L-1-i] = pData_in[L-1-i];
    }

    T *wPoints;
    int NPMax = iStep;
    wPoints = new T[NPMax];

    for (int j = iHalfStep; j < L  - iHalfStep; j++) {		
        int np = 0;

        for (int kx = j-iHalfStep; kx < j + iHalfStep + 1;  kx++) {
            wPoints[np] = pData_in[kx];
            np++;
        }

        pData_out[j] = GetMediaElement(wPoints, NPMax);
    }

    delete (wPoints);
}

#endif // _SPOTPOSFUNC_H_
#endif // _RING_BUFF
