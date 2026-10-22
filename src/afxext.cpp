// =====================================================================
//  afxext.cpp -- CStatusBar / CToolBar / CDialogBar / CReBar
// =====================================================================
#include <afxwin.h>
#include <afxcmn.h>
#include <afxext.h>
#include <afxdlgs.h>
#include <afxres.h>

// ---------------------------------------------------------------------
//  CStatusBar
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CStatusBar, CControlBar)

CStatusBar::CStatusBar() : m_cyHeight(0), m_nLastWidth(-1), m_nLastPartsWidth(-1) {}
CStatusBar::~CStatusBar() {}

BEGIN_MESSAGE_MAP(CStatusBar, CControlBar)
    ON_MESSAGE(WM_IDLEUPDATECMDUI, CStatusBar::OnIdleUpdateCmdUI)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CStatusBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, UINT nID)
{
    if (!CControlBar::Create(NULL, NULL, dwStyle | CBRS_BOTTOM, rectDefault, pParentWnd, nID))
        return FALSE;

    DWORD dwCtrl = WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | dwCtrlStyle;
    if (!m_ctrl.Create(dwCtrl, CRect(0, 0, 0, 0), this, nID)) return FALSE;

    // figure out a sensible height from the system font
    CClientDC dc(this);
    CFont* pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
    CFont* pOld = dc.SelectObject(pFont);
    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);
    dc.SelectObject(pOld);
    m_cyHeight = tm.tmHeight + 7;
    if (m_cyHeight < 22) m_cyHeight = 22;
    return TRUE;
}

BOOL CStatusBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
    return CreateEx(pParentWnd, 0, dwStyle, nID);
}

BOOL CStatusBar::SetIndicators(const UINT* lpIDArray, int nIDCount)
{
    if (lpIDArray == NULL || nIDCount < 1) return FALSE;

    m_idArray.assign(lpIDArray, lpIDArray + nIDCount);
    m_styleArray.assign(nIDCount, SBPS_NORMAL);
    m_textArray.assign(nIDCount, CString());
    m_widthArray.assign(nIDCount, 0);

    // first pane stretches; the others size to their initial string
    CClientDC dc(this);
    CFont* pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
    CFont* pOld = dc.SelectObject(pFont);
    for (int i = 0; i < nIDCount; ++i) {
        UINT nID = lpIDArray[i];
        if (nID == ID_SEPARATOR) { m_widthArray[i] = -1; continue; }
        CString str;
        if (str.LoadString(nID)) { m_textArray[i] = str; m_widthArray[i] = dc.GetTextExtent(str).cx + 16; }
        else m_widthArray[i] = 80;
        if (m_widthArray[i] < 40) m_widthArray[i] = 40;
    }
    dc.SelectObject(pOld);
    if (nIDCount > 0 && m_widthArray[0] > 0) m_widthArray[0] = -1;   // pane 0 stretches

    RecalcParts(0);
    for (int i = 0; i < nIDCount; ++i) SetPaneText(i, m_textArray[i], FALSE);
    return TRUE;
}

void CStatusBar::RecalcParts(int cxTotal)
{
    if (m_idArray.empty() || m_ctrl.GetSafeHwnd() == NULL) return;
    if (cxTotal == m_nLastPartsWidth && cxTotal != 0) return;   // nothing changed
    m_nLastPartsWidth = cxTotal;
    int nCount = (int)m_idArray.size();
    if (cxTotal <= 0) {
        CRect rc;
        GetWindowRect(&rc);
        cxTotal = rc.Width();
    }
    if (nCount == 1) { int w = -1; m_ctrl.SetParts(1, &w); return; }

    // fixed width panes are laid out from the right; pane 0 stretches
    int nFixed = 0;
    for (int i = 1; i < nCount; ++i) nFixed += (m_widthArray[i] > 0) ? m_widthArray[i] : 100;

    std::vector<int> widths(nCount);
    int nLeft = cxTotal - nFixed;
    if (nLeft < 60) nLeft = 60;
    widths[0] = nLeft;
    for (int i = 1; i < nCount; ++i) {
        int w = (m_widthArray[i] > 0) ? m_widthArray[i] : 100;
        widths[i] = (i == nCount - 1) ? -1 : (widths[i - 1] + w);
    }
    m_ctrl.SetParts(nCount, &widths[0]);
}

void CStatusBar::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    SizeChildControl(cx, cy);
    if (cx != m_nLastWidth) { m_nLastWidth = cx; RecalcParts(cx); }
}

CSize CStatusBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
    (void)nLength; (void)dwMode;
    CRect rc;
    GetWindowRect(&rc);
    int cy = m_cyHeight > 0 ? m_cyHeight : rc.Height();
    return CSize(0x7FFF, cy);
}

int CStatusBar::CommandToIndex(UINT nID) const
{
    for (size_t i = 0; i < m_idArray.size(); ++i)
        if (m_idArray[i] == nID) return (int)i;
    return -1;
}

UINT CStatusBar::GetItemID(int nIndex) const
{
    if (nIndex < 0 || nIndex >= (int)m_idArray.size()) return 0;
    return m_idArray[nIndex];
}

void CStatusBar::GetItemRect(int nIndex, LPRECT lpRect) const { m_ctrl.GetRect(nIndex, lpRect); }

void CStatusBar::SetPaneInfo(int nIndex, UINT nID, UINT nStyle, int cxWidth)
{
    if (nIndex < 0 || nIndex >= (int)m_idArray.size()) return;
    m_idArray[nIndex] = nID;
    m_styleArray[nIndex] = nStyle;
    m_widthArray[nIndex] = cxWidth;
    CRect rc; GetWindowRect(&rc);
    RecalcParts(rc.Width());
}

void CStatusBar::GetPaneInfo(int nIndex, UINT& nID, UINT& nStyle, int& cxWidth) const
{
    if (nIndex < 0 || nIndex >= (int)m_idArray.size()) { nID = 0; nStyle = 0; cxWidth = 0; return; }
    nID = m_idArray[nIndex];
    nStyle = m_styleArray[nIndex];
    cxWidth = m_widthArray[nIndex];
}

UINT CStatusBar::GetPaneStyle(int nIndex) const
{
    if (nIndex < 0 || nIndex >= (int)m_styleArray.size()) return 0;
    return m_styleArray[nIndex];
}

void CStatusBar::SetPaneStyle(int nIndex, UINT nStyle)
{
    if (nIndex < 0 || nIndex >= (int)m_styleArray.size()) return;
    m_styleArray[nIndex] = nStyle;
}

BOOL CStatusBar::SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate)
{
    if (nIndex < 0 || nIndex >= (int)m_idArray.size()) return FALSE;
    if (m_ctrl.GetSafeHwnd() == NULL) return FALSE;
    m_textArray[nIndex] = lpszNewText ? lpszNewText : _T("");
    UINT nStyle = (m_styleArray[nIndex] & SBPS_DISABLED) ? SBT_NOBORDERS : 0;
    m_ctrl.SetText(m_textArray[nIndex], nIndex, (int)nStyle);
    if (bUpdate) m_ctrl.UpdateWindow();
    return TRUE;
}

CString CStatusBar::GetPaneText(int nIndex) const
{
    if (nIndex < 0 || nIndex >= (int)m_textArray.size()) return CString();
    return m_ctrl.GetText(nIndex, 0);
}

BOOL CStatusBar::SetPaneWidth(int nIndex, int nWidth)
{
    SetPaneInfo(nIndex, m_idArray[nIndex], m_styleArray[nIndex], nWidth);
    return TRUE;
}

void CStatusBar::SetPaneIcon(int nIndex, HICON hIcon, BOOL bUpdate)
{
    m_ctrl.SetIcon(nIndex, hIcon);
    if (bUpdate) m_ctrl.Invalidate(TRUE);
}

LRESULT CStatusBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
    (void)wParam; (void)lParam;
    if (IsWindowEnabled()) {
        CFrameWnd* pParent = (CFrameWnd*)CWnd::GetParent();
        if (pParent != NULL && pParent->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
            for (size_t i = 0; i < m_idArray.size(); ++i) {
                UINT nID = m_idArray[i];
                if (nID == 0 || nID == ID_SEPARATOR) continue;
                CCmdUI state;
                state.m_pOther = this;
                state.m_nID = nID;
                state.m_nIndex = (UINT)i;
                state.m_nIndexMax = (UINT)m_idArray.size();
                state.DoUpdate(pParent, TRUE);
            }
        }
    }
    return 0;
}

LRESULT CStatusBar::OnSetMinHeight(WPARAM wParam, LPARAM lParam)
{ (void)lParam; m_ctrl.SetMinHeight((int)wParam); return 0; }

void CStatusBar::OnDestroyBar() {}

// ---------------------------------------------------------------------
//  CToolBar
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CToolBar, CControlBar)

CToolBar::CToolBar()
    : m_cxImage(16), m_cyImage(15), m_cxButton(24), m_cyButton(22)
{
    m_dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_NODIVIDER | CCS_NOPARENTALIGN;
}
CToolBar::~CToolBar() {}

BEGIN_MESSAGE_MAP(CToolBar, CControlBar)
    ON_MESSAGE(WM_IDLEUPDATECMDUI, CToolBar::OnIdleUpdateCmdUI)
    ON_WM_SIZE()
    ON_NOTIFY_EX(TTN_NEEDTEXTW, 0, CToolBar::OnToolTipTextW)
    ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, CToolBar::OnToolTipTextA)
END_MESSAGE_MAP()

BOOL CToolBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, CRect rcBorders, UINT nID)
{
    m_dwCtrlStyle = dwCtrlStyle | TBSTYLE_TOOLTIPS | CCS_NODIVIDER | CCS_NOPARENTALIGN;
    SetBorders(rcBorders);
    if (!CControlBar::Create(NULL, NULL, dwStyle | CBRS_TOP, rectDefault, pParentWnd, nID))
        return FALSE;
    if (!m_ctrl.CreateEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | m_dwCtrlStyle,
                         CRect(0, 0, 0, 0), this, nID))
        return FALSE;
    m_ctrl.SendMessage(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    m_ctrl.SetButtonSize(CSize(m_cxButton, m_cyButton));
    m_ctrl.SetBitmapSize(CSize(m_cxImage, m_cyImage));
    return TRUE;
}

BOOL CToolBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
    return CreateEx(pParentWnd, m_dwCtrlStyle, dwStyle, CRect(0, 0, 0, 0), nID);
}

BOOL CToolBar::LoadBitmap(LPCTSTR lpszResourceName)
{
    HBITMAP hBitmap = ::LoadBitmap(AfxGetResourceHandle(), lpszResourceName);
    if (hBitmap == NULL) return FALSE;
    BITMAP bm;
    ::GetObject(hBitmap, sizeof(bm), &bm);
    if (bm.bmHeight > 0) { m_cyImage = bm.bmHeight; m_cxImage = bm.bmWidth / 8; }
    // add the whole bitmap strip using the classic "magic" transparent colour
    HIMAGELIST hTemp = ImageList_Create(m_cxImage, m_cyImage, ILC_COLOR32 | ILC_MASK, 8, 4);
    if (hTemp == NULL) { ::DeleteObject(hBitmap); return FALSE; }
    ImageList_AddMasked(hTemp, hBitmap, RGB(192, 192, 192));
    ::DeleteObject(hBitmap);
    m_imageList.DeleteImageList();
    m_imageList.m_hImageList = hTemp;
    m_imageList.m_bOwner = TRUE;
    m_ctrl.SetImageList(&m_imageList);
    m_ctrl.SetBitmapSize(CSize(m_cxImage, m_cyImage));
    return TRUE;
}

BOOL CToolBar::LoadBitmap(UINT nIDResource) { return LoadBitmap(MAKEINTRESOURCE(nIDResource)); }

BOOL CToolBar::LoadToolBar(UINT nIDResource) { return LoadToolBar(MAKEINTRESOURCE(nIDResource)); }

BOOL CToolBar::LoadStdImages()
{
    if (m_ctrl.GetSafeHwnd() == NULL) return FALSE;
    // ask comctl32 for its built-in colour toolbar bitmap
    int nBase = (int)m_ctrl.SendMessage(TB_LOADIMAGES, STD_FILENEW, (LPARAM)HINST_COMMCTRL);
    if (nBase < 0) return FALSE;

    struct { UINT nID; int nStd; } map[] = {
        { ID_FILE_NEW,            STD_FILENEW    },
        { ID_FILE_OPEN,           STD_FILEOPEN   },
        { ID_FILE_SAVE,           STD_FILESAVE   },
        { ID_FILE_SAVE_AS,        STD_FILESAVE   },
        { ID_FILE_PRINT,          STD_PRINT      },
        { ID_FILE_PRINT_PREVIEW,  STD_PRINTPRE   },
        { ID_FILE_PRINT_SETUP,    STD_PROPERTIES },
        { ID_EDIT_CUT,            STD_CUT        },
        { ID_EDIT_COPY,           STD_COPY       },
        { ID_EDIT_PASTE,          STD_PASTE      },
        { ID_EDIT_UNDO,           STD_UNDO       },
        { ID_EDIT_REDO,           STD_REDOW      },
        { ID_EDIT_FIND,           STD_FIND       },
        { ID_EDIT_REPLACE,        STD_REPLACE    },
        { ID_EDIT_CLEAR,          STD_DELETE     },
        { ID_EDIT_SELECT_ALL,     STD_DELETE     },
        { ID_HELP,                STD_HELP       },
        { ID_APP_ABOUT,           STD_HELP       },
        { ID_CONTEXT_HELP,        STD_HELP       },
        { ID_VIEW_TOOLBAR,        0              },
        { 0,                      0              }
    };

    for (size_t i = 0; i < m_idArray.size(); ++i) {
        UINT nID = m_idArray[i];
        if (nID == 0 || nID == ID_SEPARATOR) continue;
        int nImage = m_imageArray[i];
        for (size_t j = 0; map[j].nID != 0; ++j) {
            if (map[j].nID == nID) { nImage = nBase + map[j].nStd; break; }
        }
        m_imageArray[i] = nImage;
        TBBUTTONINFO tbi;
        memset(&tbi, 0, sizeof(tbi));
        tbi.cbSize = sizeof(tbi);
        tbi.dwMask = TBIF_IMAGE;
        tbi.iImage = nImage;
        m_ctrl.SetButtonInfo((int)i, &tbi);
    }
    m_cxImage = 16; m_cyImage = 16;
    m_ctrl.SetBitmapSize(CSize(16, 16));
    return TRUE;
}

BOOL CToolBar::LoadToolBar(LPCTSTR lpszResourceName)
{
    // Optional: a TOOLBAR resource compiled as raw data with the layout
    //   WORD wVersion; WORD wWidth; WORD wHeight; WORD wItemCount; WORD aItems[];
    HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), lpszResourceName, RT_RCDATA);
    if (hRsrc == NULL) {
        TRACE(_T("CToolBar::LoadToolBar - resource not found; use SetButtons()/LoadBitmap() instead.\n"));
        return FALSE;
    }
    HGLOBAL hGlobal = ::LoadResource(AfxGetResourceHandle(), hRsrc);
    if (hGlobal == NULL) return FALSE;
    WORD* pData = (WORD*)::LockResource(hGlobal);
    if (pData == NULL || pData[0] != 1) return FALSE;
    m_cxImage = pData[1];
    m_cyImage = pData[2];
    int nCount = pData[3];
    UINT* pIDs = new UINT[nCount + 1];
    for (int i = 0; i < nCount; ++i) pIDs[i] = pData[4 + i];
    BOOL bResult = SetButtons(pIDs, nCount);
    delete[] pIDs;
    return bResult;
}

BOOL CToolBar::SetButtons(const UINT* lpIDArray, int nIDCount)
{
    if (m_ctrl.GetSafeHwnd() == NULL) return FALSE;

    // remove what we have
    int nOld = m_ctrl.GetButtonCount();
    for (int i = nOld - 1; i >= 0; --i) m_ctrl.DeleteButton(i);
    m_idArray.clear();
    m_styleArray.clear();
    m_imageArray.clear();

    if (lpIDArray == NULL || nIDCount <= 0) return TRUE;

    std::vector<TBBUTTON> buttons;
    int nImage = 0;
    for (int i = 0; i < nIDCount; ++i) {
        TBBUTTON tb;
        memset(&tb, 0, sizeof(tb));
        UINT nID = lpIDArray[i];
        m_idArray.push_back(nID);
        if (nID == 0 || nID == ID_SEPARATOR) {
            tb.iBitmap = 0;
            tb.idCommand = 0;
            tb.fsStyle = BTNS_SEP;
            tb.fsState = 0;
            m_styleArray.push_back(TBBS_SEPARATOR);
            m_imageArray.push_back(-1);
        } else {
            tb.iBitmap = nImage++;
            tb.idCommand = nID;
            tb.fsStyle = BTNS_BUTTON;
            tb.fsState = TBSTATE_ENABLED;
            m_styleArray.push_back(TBBS_BUTTON);
            m_imageArray.push_back(tb.iBitmap);
        }
        buttons.push_back(tb);
    }
    if (!buttons.empty())
        m_ctrl.AddButtons((int)buttons.size(), &buttons[0]);
    m_ctrl.SendMessage(TB_AUTOSIZE, 0, 0);
    return TRUE;
}

int CToolBar::CommandToIndex(UINT nID) const { return m_ctrl.CommandToIndex(nID); }

UINT CToolBar::GetItemID(int nIndex) const
{
    if (nIndex < 0 || nIndex >= (int)m_idArray.size()) return 0;
    return m_idArray[nIndex];
}

UINT CToolBar::GetButtonStyle(int nIndex) const
{
    if (nIndex < 0 || nIndex >= (int)m_styleArray.size()) return 0;
    return m_styleArray[nIndex];
}

void CToolBar::SetButtonStyle(int nIndex, UINT nStyle)
{
    if (nIndex < 0 || nIndex >= (int)m_styleArray.size()) return;
    m_styleArray[nIndex] = nStyle;
    TBBUTTONINFO tbi;
    memset(&tbi, 0, sizeof(tbi));
    tbi.cbSize = sizeof(tbi);
    tbi.dwMask = TBIF_STYLE;
    tbi.fsStyle = (BYTE)(nStyle & 0xFF);
    m_ctrl.SetButtonInfo(nIndex, &tbi);
}

void CToolBar::GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const
{
    nID = GetItemID(nIndex);
    nStyle = GetButtonStyle(nIndex);
    iImage = (nIndex >= 0 && nIndex < (int)m_imageArray.size()) ? m_imageArray[nIndex] : -1;
}

void CToolBar::SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage)
{
    if (nIndex < 0 || nIndex >= (int)m_idArray.size()) return;
    m_idArray[nIndex] = nID;
    m_styleArray[nIndex] = nStyle;
    m_imageArray[nIndex] = iImage;
    TBBUTTONINFO tbi;
    memset(&tbi, 0, sizeof(tbi));
    tbi.cbSize = sizeof(tbi);
    tbi.dwMask = TBIF_COMMAND | TBIF_STYLE | TBIF_IMAGE;
    tbi.idCommand = nID;
    tbi.fsStyle = (BYTE)(nStyle & 0xFF);
    tbi.iImage = iImage;
    m_ctrl.SetButtonInfo(nIndex, &tbi);
}

void CToolBar::GetItemRect(int nIndex, LPRECT lpRect) const { m_ctrl.GetItemRect(nIndex, lpRect); }

void CToolBar::SetSizes(SIZE sizeButton, SIZE sizeImage)
{
    m_cxButton = sizeButton.cx; m_cyButton = sizeButton.cy;
    m_cxImage = sizeImage.cx;   m_cyImage = sizeImage.cy;
    if (m_ctrl.GetSafeHwnd() != NULL) {
        m_ctrl.SetButtonSize(sizeButton);
        m_ctrl.SetBitmapSize(sizeImage);
    }
}

void CToolBar::SetHeight(int cyHeight)
{
    m_cyButton = cyHeight;
    if (m_ctrl.GetSafeHwnd() != NULL) m_ctrl.SetButtonSize(CSize(m_cxButton, cyHeight));
}

CSize CToolBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
    (void)nLength; (void)dwMode;
    if (m_ctrl.GetSafeHwnd() == NULL) return CSize(0x7FFF, m_cyButton + 4);
    SIZE sz = { 0, 0 };
    if (!m_ctrl.GetMaxSize(&sz)) {
        // fall back: count buttons
        int n = m_ctrl.GetButtonCount();
        sz.cx = n * (m_cxButton + 2);
        sz.cy = m_cyButton + 4;
    }
    return CSize(0x7FFF, sz.cy > 0 ? sz.cy : (m_cyButton + 4));
}

void CToolBar::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    SizeChildControl(cx, cy);
}

void CControlBar::SizeChildControl(int cx, int cy)
{
    // the embedded common control always fills the bar
    CWnd* pChild = GetFirstChildWindow();
    if (pChild == NULL || pChild->GetSafeHwnd() == NULL) return;
    CRect rc;
    pChild->GetWindowRect(&rc);
    if (rc.Width() != cx || rc.Height() != cy)
        pChild->MoveWindow(0, 0, cx, cy, TRUE);
}

// (WM_PAINT is left to the default window procedure: it validates the
//  update region.  Intercepting it with an empty handler would cause an
//  endless WM_PAINT storm that starves timers and input.)

LRESULT CToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
    (void)wParam; (void)lParam;
    if (!IsWindowEnabled()) return 0;
    CFrameWnd* pParent = (CFrameWnd*)CWnd::GetParent();
    if (pParent == NULL || !pParent->IsKindOf(RUNTIME_CLASS(CFrameWnd))) return 0;

    for (size_t i = 0; i < m_idArray.size(); ++i) {
        UINT nID = m_idArray[i];
        if (nID == 0 || nID == ID_SEPARATOR) continue;
        CCmdUI state;
        state.m_pOther = this;
        state.m_nID = nID;
        state.m_nIndex = (UINT)i;
        state.m_nIndexMax = (UINT)m_idArray.size();
        state.DoUpdate(pParent, TRUE);
    }
    return 0;
}

// The toolbar control asks its parent for the button text.  MFC convention:
// the string resource holds "<menu caption>\n<status bar prompt>" and the
// prompt part becomes the tool tip.
static BOOL AfxToolBarTip(UINT_PTR nCmd, CString& rTip)
{
    rTip.Empty();
    if (nCmd == 0 || nCmd == (UINT_PTR)-1) return FALSE;
    CString strFull;
    if (!strFull.LoadString((UINT)nCmd)) return FALSE;
    int nSep = strFull.Find('\n');
    rTip = (nSep >= 0) ? strFull.Mid(nSep + 1) : strFull;
    rTip.Trim();
    return !rTip.IsEmpty();
}

BOOL CToolBar::OnToolTipTextW(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
    (void)nID;
    TOOLTIPTEXTW* pTTT = (TOOLTIPTEXTW*)pNMHDR;
    CString strTip;
    if (!AfxToolBarTip(pTTT->hdr.idFrom, strTip)) { *pResult = 0; return FALSE; }
    CStringW w(strTip);
    AfxStrCopy(pTTT->szText, sizeof(pTTT->szText) / sizeof(wchar_t), (LPCWSTR)w);
    pTTT->hinst = NULL;
    pTTT->uFlags = 0;
    *pResult = 0;
    return TRUE;
}

BOOL CToolBar::OnToolTipTextA(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
    (void)nID;
    TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
    CString strTip;
    if (!AfxToolBarTip(pTTT->hdr.idFrom, strTip)) { *pResult = 0; return FALSE; }
    CStringA a(strTip);
    AfxStrCopy(pTTT->szText, sizeof(pTTT->szText), (LPCSTR)a);
    pTTT->hinst = NULL;
    pTTT->uFlags = 0;
    *pResult = 0;
    return TRUE;
}

// ---------------------------------------------------------------------
//  CDialogBar
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CDialogBar, CControlBar)

CDialogBar::CDialogBar() : m_hDlgChild(NULL) { m_sizeDialog.cx = m_sizeDialog.cy = 0; }
CDialogBar::~CDialogBar() {}

BEGIN_MESSAGE_MAP(CDialogBar, CControlBar)
    ON_MESSAGE(WM_INITDIALOG, CDialogBar::OnInitDialogBar)
END_MESSAGE_MAP()

BOOL CDialogBar::Create(CWnd* pParentWnd, UINT nIDTemplate, DWORD dwStyle, UINT nID)
{
    return Create(pParentWnd, MAKEINTRESOURCE(nIDTemplate), dwStyle, nID);
}

BOOL CDialogBar::Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName, DWORD dwStyle, UINT nID)
{
    if (!CControlBar::Create(NULL, NULL, dwStyle, rectDefault, pParentWnd, nID)) return FALSE;

    // create the dialog template as a child of this bar
    m_hDlgChild = ::CreateDialogParam(AfxGetResourceHandle(), lpszTemplateName, m_hWnd,
                                      AfxDlgProc, (LPARAM)this);
    if (m_hDlgChild == NULL) return FALSE;

    RECT rc;
    ::GetWindowRect(m_hDlgChild, &rc);
    m_sizeDialog.cx = rc.right - rc.left;
    m_sizeDialog.cy = rc.bottom - rc.top;
    ::SetWindowPos(m_hDlgChild, NULL, 0, 0, m_sizeDialog.cx, m_sizeDialog.cy,
                   SWP_NOZORDER | SWP_NOACTIVATE);
    ::ShowWindow(m_hDlgChild, SW_SHOW);
    return TRUE;
}

LRESULT CDialogBar::OnInitDialogBar(WPARAM wParam, LPARAM lParam)
{
    (void)wParam; (void)lParam;
    // the child dialog is already subclassed by AfxDlgProc
    return TRUE;
}

void CDialogBar::DoDataExchange(CDataExchange* pDX)
{
    // route the exchange to the child dialog window
    CWnd* pDlg = CWnd::FromHandle(m_hDlgChild);
    if (pDlg != NULL) {
        CDataExchange dx(pDlg, pDX->m_bSaveAndValidate);
        CControlBar::DoDataExchange(&dx);
    }
}

BOOL CDialogBar::UpdateData(BOOL bSaveAndValidate)
{
    CWnd* pDlg = CWnd::FromHandle(m_hDlgChild);
    if (pDlg == NULL) return FALSE;
    return AfxDoDataExchange(pDlg, bSaveAndValidate);
}

CSize CDialogBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
    (void)nLength; (void)dwMode;
    if (m_barPos == barTop || m_barPos == barBottom) return CSize(0x7FFF, m_sizeDialog.cy + 4);
    return CSize(m_sizeDialog.cx + 4, 0x7FFF);
}

// ---------------------------------------------------------------------
//  CReBar
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CReBar, CControlBar)

CReBar::CReBar() {}
CReBar::~CReBar() {}

BEGIN_MESSAGE_MAP(CReBar, CControlBar)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CReBar::Create(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, UINT nID)
{
    if (!CControlBar::Create(NULL, NULL, dwStyle, rectDefault, pParentWnd, nID)) return FALSE;
    if (!m_rebar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwCtrlStyle,
                        CRect(0, 0, 0, 0), this, nID))
        return FALSE;
    return TRUE;
}

CSize CReBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
    (void)nLength; (void)dwMode;
    UINT nRows = m_rebar.GetRowCount();
    return CSize(0x7FFF, (int)(nRows * 26));
}

void CReBar::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (m_rebar.GetSafeHwnd() != NULL) m_rebar.MoveWindow(0, 0, cx, cy, TRUE);
}
