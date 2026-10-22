// =====================================================================
//  afxcmn.cpp -- common control support code (initialisation, rich edit,
//                tool tips, date/time helpers) + runtime classes
// =====================================================================
#include <afxwin.h>
#include <afxcmn.h>
#include <afxext.h>
#include <afxdlgs.h>
#include <afxres.h>

// ---------------------------------------------------------------------
//  runtime class information for the control wrappers
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CImageList, CObject)
IMPLEMENT_DYNAMIC(CButton, CWnd)
IMPLEMENT_DYNAMIC(CEdit, CWnd)
IMPLEMENT_DYNAMIC(CStatic, CWnd)
IMPLEMENT_DYNAMIC(CListBox, CWnd)
IMPLEMENT_DYNAMIC(CComboBox, CWnd)
IMPLEMENT_DYNAMIC(CTabCtrl, CWnd)
IMPLEMENT_DYNAMIC(CListCtrl, CWnd)
IMPLEMENT_DYNAMIC(CTreeCtrl, CWnd)
IMPLEMENT_DYNAMIC(CHeaderCtrl, CWnd)
IMPLEMENT_DYNAMIC(CProgressCtrl, CWnd)
IMPLEMENT_DYNAMIC(CSliderCtrl, CWnd)
IMPLEMENT_DYNAMIC(CSpinButtonCtrl, CWnd)
IMPLEMENT_DYNAMIC(CToolTipCtrl, CWnd)
IMPLEMENT_DYNAMIC(CStatusBarCtrl, CWnd)
IMPLEMENT_DYNAMIC(CToolBarCtrl, CWnd)
IMPLEMENT_DYNAMIC(CRichEditCtrl, CWnd)
IMPLEMENT_DYNAMIC(CDateTimeCtrl, CWnd)
IMPLEMENT_DYNAMIC(CMonthCalCtrl, CWnd)
IMPLEMENT_DYNAMIC(CHotKeyCtrl, CWnd)
IMPLEMENT_DYNAMIC(CIPAddressCtrl, CWnd)
IMPLEMENT_DYNAMIC(CReBarCtrl, CWnd)

// ---------------------------------------------------------------------
//  initialisation helpers
// ---------------------------------------------------------------------
static bool s_bCommonControlsInit = false;

BOOL AFXAPI AfxInitCommonControls()
{
    if (s_bCommonControlsInit) return TRUE;
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES | ICC_TAB_CLASSES |
                ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_UPDOWN_CLASS |
                ICC_HOTKEY_CLASS | ICC_ANIMATE_CLASS | ICC_WIN95_CLASSES |
                ICC_DATE_CLASSES | ICC_USEREX_CLASSES | ICC_COOL_CLASSES |
                ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS |
                ICC_NATIVEFNTCTL_CLASS | ICC_LINK_CLASS | ICC_STANDARD_CLASSES;
    s_bCommonControlsInit = ::InitCommonControlsEx(&icc) ? TRUE : FALSE;
    if (!s_bCommonControlsInit) ::InitCommonControls();
    return TRUE;
}

static ULONG_PTR s_ulActivationCookie = 0;

void AFXAPI AfxEnableVisualStyles()
{
    // activate comctl32 v6 for the whole process
    static bool s_bDone = false;
    if (s_bDone) return;
    s_bDone = true;

    OSVERSIONINFOEX ver;
    memset(&ver, 0, sizeof(ver));
    ver.dwOSVersionInfoSize = sizeof(ver);
    ver.dwMajorVersion = 6;
    DWORDLONG condMask = 0;
    VER_SET_CONDITION(condMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    if (::VerifyVersionInfo(&ver, VER_MAJORVERSION, condMask)) {
        TCHAR szPath[MAX_PATH];
        ::GetModuleFileName(NULL, szPath, MAX_PATH);
        ACTCTX actCtx;
        memset(&actCtx, 0, sizeof(actCtx));
        actCtx.cbSize = sizeof(actCtx);
        actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_SET_PROCESS_DEFAULT;
        actCtx.lpSource = szPath;
        actCtx.lpResourceName = MAKEINTRESOURCE(1);      // the standard manifest id
        HANDLE hActCtx = ::CreateActCtx(&actCtx);
        if (hActCtx != INVALID_HANDLE_VALUE) {
            if (!::ActivateActCtx(hActCtx, &s_ulActivationCookie)) s_ulActivationCookie = 0;
        }
    }
    AfxInitCommonControls();
}

void AFXAPI AfxSetWindowTheme(HWND hWnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList)
{
    typedef HRESULT (WINAPI *PFNSETWINDOWTHEME)(HWND, LPCWSTR, LPCWSTR);
    HMODULE hUx = ::LoadLibrary(_T("uxtheme.dll"));
    if (hUx == NULL) return;
    PFNSETWINDOWTHEME pfn = (PFNSETWINDOWTHEME)::GetProcAddress(hUx, "SetWindowTheme");
    if (pfn != NULL) pfn(hWnd, pszSubAppName, pszSubIdList);
    ::FreeLibrary(hUx);
}

void AFXAPI AfxEnableDPIServiceAwareness()
{
    typedef BOOL (WINAPI *PFNSETDPI)(void);
    HMODULE hUser = ::GetModuleHandle(_T("user32.dll"));
    if (hUser == NULL) return;
    PFNSETDPI pfn = (PFNSETDPI)::GetProcAddress(hUser, "SetProcessDPIAware");
    if (pfn != NULL) pfn();
}

void AFXAPI AfxInitRichEdit()
{
    static bool s_bDone = false;
    if (s_bDone) return;
    s_bDone = true;
    AfxInitCommonControls();
    ::LoadLibrary(_T("riched20.dll"));
}

void AFXAPI AfxInitRichEdit2()
{
    static bool s_bDone = false;
    if (s_bDone) return;
    s_bDone = true;
    AfxInitCommonControls();
    if (::LoadLibrary(_T("msftedit.dll")) == NULL) ::LoadLibrary(_T("riched20.dll"));
}

// ---------------------------------------------------------------------
//  CRichEditCtrl
// ---------------------------------------------------------------------
BOOL CRichEditCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    AfxInitRichEdit2();
    // MSFTEDIT_CLASS is always a wide string, even in ANSI builds
    CString strClass(MSFTEDIT_CLASS);
    if (::GetModuleHandle(_T("msftedit.dll")) == NULL) {
        AfxInitRichEdit();
        strClass = CString(RICHEDIT_CLASS);
    }
    LPCTSTR pszClass = (LPCTSTR)strClass;
    return CreateEx(WS_EX_CLIENTEDGE, pszClass, NULL, dwStyle, rect.left, rect.top,
                    rect.right - rect.left, rect.bottom - rect.top,
                    pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
}

void CRichEditCtrl::GetTextRange(long nFirst, long nLast, CString& refString) const
{
    TEXTRANGE tr;
    tr.chrg.cpMin = nFirst;
    tr.chrg.cpMax = nLast;
    int nLen = (int)::SendMessage(m_hWnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
    if (nLen <= 0) { refString.Empty(); return; }
    LPTSTR p = refString.GetBuffer(nLen + 1);
    tr.lpstrText = p;
    nLen = (int)::SendMessage(m_hWnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
    p[nLen] = 0;
    refString.ReleaseBuffer(nLen);
}

void CRichEditCtrl::GetSelText(CString& refString) const
{
    CHARRANGE cr;
    ::SendMessage(m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
    GetTextRange(cr.cpMin, cr.cpMax, refString);
}

// ---------------------------------------------------------------------
//  CToolTipCtrl
// ---------------------------------------------------------------------
BOOL CToolTipCtrl::AddTool(CWnd* pWnd, UINT nIDText, LPCRECT lpRectTool, UINT_PTR nIDTool)
{
    CString str;
    str.LoadString(nIDText);
    return AddTool(pWnd, (LPCTSTR)str, lpRectTool, nIDTool);
}

BOOL CToolTipCtrl::AddTool(CWnd* pWnd, LPCTSTR lpszText, LPCRECT lpRectTool, UINT_PTR nIDTool)
{
    TOOLINFO ti;
    memset(&ti, 0, sizeof(ti));
    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_SUBCLASS | ((lpRectTool != NULL) ? 0 : TTF_IDISHWND);
    ti.hwnd = pWnd ? pWnd->GetSafeHwnd() : NULL;
    ti.uId = (lpRectTool != NULL) ? nIDTool : (UINT_PTR)ti.hwnd;
    if (lpRectTool != NULL) ::CopyRect(&ti.rect, lpRectTool);
    ti.lpszText = (LPTSTR)lpszText;
    return (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

void CToolTipCtrl::DelTool(CWnd* pWnd, UINT_PTR nIDTool)
{
    TOOLINFO ti;
    memset(&ti, 0, sizeof(ti));
    ti.cbSize = sizeof(ti);
    ti.hwnd = pWnd ? pWnd->GetSafeHwnd() : NULL;
    ti.uId = nIDTool;
    ::SendMessage(m_hWnd, TTM_DELTOOL, 0, (LPARAM)&ti);
}

BOOL CToolTipCtrl::HitTest(CWnd* pWnd, CPoint point, LPTOOLINFO lpInfo) const
{
    TT_HITTESTINFO hti;
    memset(&hti, 0, sizeof(hti));
    hti.hwnd = pWnd ? pWnd->GetSafeHwnd() : NULL;
    hti.pt = point;
    memcpy(&hti.ti, lpInfo, sizeof(TOOLINFO));
    return (BOOL)::SendMessage(m_hWnd, TTM_HITTEST, 0, (LPARAM)&hti);
}

void CToolTipCtrl::GetText(CString& rText, CWnd* pWnd, UINT_PTR nIDTool) const
{
    TOOLINFO ti;
    memset(&ti, 0, sizeof(ti));
    ti.cbSize = sizeof(ti);
    ti.hwnd = pWnd ? pWnd->GetSafeHwnd() : NULL;
    ti.uId = nIDTool;
    TCHAR buf[1024];
    ti.lpszText = buf;
    ::SendMessage(m_hWnd, TTM_GETTEXT, 1024, (LPARAM)&ti);
    rText = buf;
}

void CToolTipCtrl::SetToolRect(LPCRECT lpRect, CWnd* pWnd, UINT_PTR nIDTool)
{
    TOOLINFO ti;
    memset(&ti, 0, sizeof(ti));
    ti.cbSize = sizeof(ti);
    ti.hwnd = pWnd ? pWnd->GetSafeHwnd() : NULL;
    ti.uId = nIDTool;
    ::CopyRect(&ti.rect, lpRect);
    ::SendMessage(m_hWnd, TTM_NEWTOOLRECT, 0, (LPARAM)&ti);
}

void CToolTipCtrl::UpdateTipText(LPCTSTR lpszText, CWnd* pWnd, UINT_PTR nIDTool)
{
    TOOLINFO ti;
    memset(&ti, 0, sizeof(ti));
    ti.cbSize = sizeof(ti);
    ti.hwnd = pWnd ? pWnd->GetSafeHwnd() : NULL;
    ti.uId = nIDTool;
    ti.lpszText = (LPTSTR)lpszText;
    ::SendMessage(m_hWnd, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
}

void CToolTipCtrl::UpdateTipText(UINT nIDText, CWnd* pWnd, UINT_PTR nIDTool)
{
    CString str;
    str.LoadString(nIDText);
    UpdateTipText((LPCTSTR)str, pWnd, nIDTool);
}

// ---------------------------------------------------------------------
//  CToolBarCtrl
// ---------------------------------------------------------------------
void CToolBarCtrl::SetButtonText(int nIndex, LPCTSTR lpszString)
{
    TBBUTTONINFO tbi;
    memset(&tbi, 0, sizeof(tbi));
    tbi.cbSize = sizeof(tbi);
    tbi.dwMask = TBIF_TEXT;
    tbi.pszText = (LPTSTR)lpszString;
    ::SendMessage(m_hWnd, TB_SETBUTTONINFO, nIndex, (LPARAM)&tbi);
}

DWORD CToolBarCtrl::GetButtonStyle(int nIndex) const
{
    TBBUTTONINFO tbi;
    memset(&tbi, 0, sizeof(tbi));
    tbi.cbSize = sizeof(tbi);
    tbi.dwMask = TBIF_STYLE;
    ::SendMessage(m_hWnd, TB_GETBUTTONINFO, nIndex, (LPARAM)&tbi);
    return tbi.fsStyle;
}

void CToolBarCtrl::SetButtonStyle(int nIndex, DWORD dwStyle)
{
    TBBUTTONINFO tbi;
    memset(&tbi, 0, sizeof(tbi));
    tbi.cbSize = sizeof(tbi);
    tbi.dwMask = TBIF_STYLE;
    tbi.fsStyle = (BYTE)dwStyle;
    ::SendMessage(m_hWnd, TB_SETBUTTONINFO, nIndex, (LPARAM)&tbi);
}

// ---------------------------------------------------------------------
//  CDateTimeCtrl / CMonthCalCtrl (CTime <-> SYSTEMTIME helpers)
// ---------------------------------------------------------------------
static void AfxTimeToSystemTime(const CTime& t, SYSTEMTIME& st)
{
    memset(&st, 0, sizeof(st));
    st.wYear = (WORD)t.GetYear();
    st.wMonth = (WORD)t.GetMonth();
    st.wDay = (WORD)t.GetDay();
    st.wHour = (WORD)t.GetHour();
    st.wMinute = (WORD)t.GetMinute();
    st.wSecond = (WORD)t.GetSecond();
}

static CTime AfxSystemTimeToTime(const SYSTEMTIME& st)
{
    return CTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

DWORD CDateTimeCtrl::GetRange(CTime* pMinRange, CTime* pMaxRange) const
{
    SYSTEMTIME st[2];
    DWORD dw = (DWORD)::SendMessage(m_hWnd, DTM_GETRANGE, 0, (LPARAM)st);
    if (dw & GDTR_MIN && pMinRange) *pMinRange = AfxSystemTimeToTime(st[0]);
    if (dw & GDTR_MAX && pMaxRange) *pMaxRange = AfxSystemTimeToTime(st[1]);
    return dw;
}

BOOL CDateTimeCtrl::SetRange(const CTime* pMinRange, const CTime* pMaxRange)
{
    SYSTEMTIME st[2];
    DWORD dw = 0;
    if (pMinRange) { AfxTimeToSystemTime(*pMinRange, st[0]); dw |= GDTR_MIN; }
    if (pMaxRange) { AfxTimeToSystemTime(*pMaxRange, st[1]); dw |= GDTR_MAX; }
    return (BOOL)::SendMessage(m_hWnd, DTM_SETRANGE, dw, (LPARAM)st);
}

DWORD CDateTimeCtrl::GetTime(CTime& timeDest) const
{
    SYSTEMTIME st;
    DWORD dw = (DWORD)::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
    timeDest = AfxSystemTimeToTime(st);
    return dw;
}

BOOL CDateTimeCtrl::SetTime(const CTime* pTimeNew) { return SetTime(GDT_VALID, pTimeNew); }

BOOL CDateTimeCtrl::SetTime(DWORD dwFlags, const CTime* pTimeNew)
{
    SYSTEMTIME st;
    memset(&st, 0, sizeof(st));
    if (pTimeNew != NULL) AfxTimeToSystemTime(*pTimeNew, st);
    return (BOOL)::SendMessage(m_hWnd, DTM_SETSYSTEMTIME, dwFlags, (LPARAM)&st);
}

CFont* CDateTimeCtrl::GetMonthCalFont() const
{
    return CFont::FromHandle((HFONT)::SendMessage(m_hWnd, DTM_GETMCFONT, 0, 0));
}

BOOL CMonthCalCtrl::GetCurSel(CTime& refTime) const
{
    SYSTEMTIME st;
    if (!::SendMessage(m_hWnd, MCM_GETCURSEL, 0, (LPARAM)&st)) return FALSE;
    refTime = AfxSystemTimeToTime(st);
    return TRUE;
}

BOOL CMonthCalCtrl::SetCurSel(const CTime& refTime)
{
    SYSTEMTIME st;
    AfxTimeToSystemTime(refTime, st);
    return (BOOL)::SendMessage(m_hWnd, MCM_SETCURSEL, 0, (LPARAM)&st);
}

int CMonthCalCtrl::GetMonthRange(CTime& refMinRange, CTime& refMaxRange) const
{
    SYSTEMTIME st[2];
    int n = (int)::SendMessage(m_hWnd, MCM_GETMONTHRANGE, 0, (LPARAM)st);
    refMinRange = AfxSystemTimeToTime(st[0]);
    refMaxRange = AfxSystemTimeToTime(st[1]);
    return n;
}

BOOL CMonthCalCtrl::SetMonthRange(DWORD dwFlags, const CTime& refMinRange, const CTime& refMaxRange)
{
    (void)dwFlags; (void)refMinRange; (void)refMaxRange;
    return FALSE;      // the month calendar has no set-range message
}

BOOL CMonthCalCtrl::SetSelRange(const CTime& pMinRange, const CTime& pMaxRange)
{
    SYSTEMTIME st[2];
    AfxTimeToSystemTime(pMinRange, st[0]);
    AfxTimeToSystemTime(pMaxRange, st[1]);
    return (BOOL)::SendMessage(m_hWnd, MCM_SETSELRANGE, 0, (LPARAM)st);
}

BOOL CMonthCalCtrl::GetSelRange(CTime& refMinRange, CTime& refMaxRange) const
{
    SYSTEMTIME st[2];
    ::SendMessage(m_hWnd, MCM_GETSELRANGE, 0, (LPARAM)st);
    refMinRange = AfxSystemTimeToTime(st[0]);
    refMaxRange = AfxSystemTimeToTime(st[1]);
    return TRUE;
}

BOOL CMonthCalCtrl::GetToday(CTime& refTime) const
{
    SYSTEMTIME st;
    ::SendMessage(m_hWnd, MCM_GETTODAY, 0, (LPARAM)&st);
    refTime = AfxSystemTimeToTime(st);
    return TRUE;
}

void CMonthCalCtrl::GetToday(CTime* lpTimeInstance) const
{
    if (lpTimeInstance != NULL) GetToday(*lpTimeInstance);
}

void CMonthCalCtrl::SetToday(const CTime& refTime)
{
    SYSTEMTIME st;
    AfxTimeToSystemTime(refTime, st);
    ::SendMessage(m_hWnd, MCM_SETTODAY, 0, (LPARAM)&st);
}
