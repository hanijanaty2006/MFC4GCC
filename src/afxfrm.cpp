// =====================================================================
//  afxfrm.cpp -- CControlBar, CFrameWnd, CMDIFrameWnd, CMDIChildWnd
// =====================================================================
#include <afxwin.h>
#include <afxcmn.h>
#include <afxext.h>
#include <afxdlgs.h>
#include <afxres.h>

#define AFX_WS_DEFAULT_VIEW   (WS_CHILD | WS_VISIBLE | WS_BORDER)
#define AFX_IDM_FIRST_MDICHILD 0xFF00

// ---------------------------------------------------------------------
//  CControlBar
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CControlBar, CWnd)

CControlBar::CControlBar()
    : m_cxLeft(0), m_cyTop(0), m_cxRight(0), m_cyBottom(0), m_nCount(0),
      m_dwStyle(0), m_barPos(barTop)
{
}
CControlBar::~CControlBar() {}

BEGIN_MESSAGE_MAP(CControlBar, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

BOOL CControlBar::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                         const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    if (lpszClassName == NULL)
        lpszClassName = AfxRegisterWndClass(0, ::LoadCursor(NULL, IDC_ARROW),
                                            (HBRUSH)(COLOR_BTNFACE + 1), NULL);
    m_dwStyle = dwStyle;
    if (dwStyle & CBRS_BOTTOM) m_barPos = barBottom;
    else if (dwStyle & CBRS_LEFT) m_barPos = barLeft;
    else if (dwStyle & CBRS_RIGHT) m_barPos = barRight;
    else m_barPos = barTop;

    if (!CWnd::CreateEx(0, lpszClassName, lpszWindowName,
                        ((dwStyle & ~CBRS_ALIGN_ANY & ~CBRS_BORDER_ANY) | WS_CHILD |
                         WS_CLIPCHILDREN | WS_CLIPSIBLINGS),
                        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID))
        return FALSE;

    if (pParentWnd != NULL && pParentWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
        ((CFrameWnd*)pParentWnd)->AddControlBar(this);
    return TRUE;
}

BOOL CControlBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, LPCTSTR lpszClassName,
                           LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, UINT nID)
{
    (void)dwCtrlStyle;
    return Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID);
}

BOOL CControlBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle,
                           const RECT& rect, UINT nID)
{
    return CreateEx(pParentWnd, dwCtrlStyle, NULL, NULL, dwStyle, rect, nID);
}

DWORD CControlBar::GetBarStyle() { return m_dwStyle; }
void CControlBar::SetBarStyle(DWORD dwStyle)
{
    m_dwStyle = dwStyle;
    if (dwStyle & CBRS_BOTTOM) m_barPos = barBottom;
    else if (dwStyle & CBRS_LEFT) m_barPos = barLeft;
    else if (dwStyle & CBRS_RIGHT) m_barPos = barRight;
    else if (dwStyle & CBRS_TOP) m_barPos = barTop;
}

BOOL CControlBar::IsVisible() const
{
    return (::IsWindowVisible(m_hWnd) && (GetStyle() & WS_VISIBLE)) ? TRUE : FALSE;
}

CFrameWnd* CControlBar::GetDockingFrame() const
{
    CWnd* pParent = CWnd::GetParent();
    return (pParent != NULL && pParent->IsKindOf(RUNTIME_CLASS(CFrameWnd))) ? (CFrameWnd*)pParent : NULL;
}

void CControlBar::SetBorders(int left, int top, int right, int bottom)
{ m_cxLeft = left; m_cyTop = top; m_cxRight = right; m_cyBottom = bottom; }
void CControlBar::SetBorders(LPCRECT lpBorders)
{ m_cxLeft = lpBorders->left; m_cyTop = lpBorders->top; m_cxRight = lpBorders->right; m_cyBottom = lpBorders->bottom; }
void CControlBar::GetBorders(LPRECT lpBorders) const
{ lpBorders->left = m_cxLeft; lpBorders->top = m_cyTop; lpBorders->right = m_cxRight; lpBorders->bottom = m_cyBottom; }

CSize CControlBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
    CRect rc;
    GetWindowRect(&rc);
    if (bHorz) return CSize(bStretch ? 0x7FFF : rc.Width(), rc.Height());
    return CSize(rc.Width(), bStretch ? 0x7FFF : rc.Height());
}

CSize CControlBar::CalcLayout(DWORD dwMode, int nLength)
{
    (void)dwMode; (void)nLength;
    return CalcFixedLayout((dwMode & LM_STRETCH) != 0, (dwMode & LM_VERTDOCK) == 0);
}

CSize CControlBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
    return CalcLayout(dwMode, nLength);
}

void CControlBar::OnPaint() {}
BOOL CControlBar::OnEraseBkgnd(CDC* pDC)
{
    CRect rc;
    GetClientRect(&rc);
    pDC->FillSolidRect(&rc, ::GetSysColor(COLOR_BTNFACE));
    return TRUE;
}
void CControlBar::OnWindowPosChanged(WINDOWPOS* lpwndpos) { CWnd::OnWindowPosChanged(lpwndpos); }

BOOL CControlBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // forward control notifications (toolbar buttons etc.) to the frame
    CWnd* pParent = CWnd::GetParent();
    if (pParent != NULL)
        return (BOOL)pParent->SendMessage(WM_COMMAND, wParam, lParam);
    return CWnd::OnCommand(wParam, lParam);
}

// ---------------------------------------------------------------------
//  CFrameWnd
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CFrameWnd, CWnd)

CFrameWnd::CFrameWnd()
    : m_hAccelTable(NULL), m_bAutoMenuEnable(TRUE), m_pViewActive(NULL),
      m_bInRecalcLayout(FALSE), m_nLastClientCx(-1), m_nLastClientCy(-1),
      m_nIDTracking(0), m_nIDLastMessage(0)
{
}

CFrameWnd::~CFrameWnd()
{
    if (m_hAccelTable != NULL) { ::DestroyAcceleratorTable(m_hAccelTable); m_hAccelTable = NULL; }
}

BEGIN_MESSAGE_MAP(CFrameWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_WM_SETFOCUS()
    ON_WM_INITMENUPOPUP()
    ON_WM_DESTROY()
    ON_WM_SYSCOMMAND()
    ON_MESSAGE(WM_KICKIDLE, CFrameWnd::OnKickIdle)
    ON_MESSAGE(WM_IDLEUPDATECMDUI, CFrameWnd::OnIdleUpdateCmdUI)
    ON_MESSAGE(WM_SETMESSAGESTRING, CFrameWnd::OnSetMessageString)
    ON_MESSAGE(WM_POPMESSAGESTRING, CFrameWnd::OnPopMessageString)
    ON_MESSAGE(WM_COMMANDHELP, CFrameWnd::OnCommandHelp)
    ON_MESSAGE(WM_HELPHITTEST, CFrameWnd::OnHelpHitTest)
    ON_COMMAND_RANGE(ID_VIEW_MIN, ID_VIEW_MAX, CFrameWnd::OnBarCheck)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_MIN, ID_VIEW_MAX, CFrameWnd::OnUpdateControlBarMenu)
END_MESSAGE_MAP()

BOOL CFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
{
    if (cs.lpszClass == NULL) {
        cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
        if (cs.lpszClass == NULL) return FALSE;
    }
    // rectDefault == {0,0,0,0}: let Windows pick position and size
    if (cs.cx == 0 && cs.cy == 0) {
        cs.x = cs.y = CW_USEDEFAULT;
        cs.cx = cs.cy = CW_USEDEFAULT;
    }
    cs.style &= ~WS_BORDER;
    cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    cs.dwExStyle |= WS_EX_CONTROLPARENT;
    return CWnd::PreCreateWindow(cs);
}

BOOL CFrameWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
    // Title: the string table entry is the \n separated doc-template string,
    // field 0 is the window title.  Fall back to the application name.
    CString strTitle;
    if (!strTitle.LoadString(nIDResource)) {
        CWinApp* pApp = AfxGetApp();
        strTitle = (pApp != NULL && pApp->m_pszAppName != NULL) ? pApp->m_pszAppName : _T("Application");
    }
    int nSep = strTitle.Find('\n');
    if (nSep >= 0) strTitle = strTitle.Left(nSep);
    strTitle.Trim();
    if (strTitle.IsEmpty()) {
        CWinApp* pApp = AfxGetApp();
        strTitle = (pApp != NULL && pApp->m_pszAppName != NULL) ? pApp->m_pszAppName : _T("Application");
    }
    m_strTitle = strTitle;

    LPCTSTR lpszMenuName = MAKEINTRESOURCE(nIDResource);
    if (::FindResource(AfxGetResourceHandle(), lpszMenuName, RT_MENU) == NULL) lpszMenuName = NULL;

    if (!Create(NULL, strTitle, dwDefaultStyle, rectDefault, pParentWnd, lpszMenuName, 0, pContext))
        return FALSE;

    if (::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nIDResource), RT_ACCELERATOR) != NULL)
        LoadAccelTable(MAKEINTRESOURCE(nIDResource));

    UpdateFrameTitle(TRUE);
    return TRUE;
}

BOOL CFrameWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                       const RECT& rect, CWnd* pParentWnd, LPCTSTR lpszMenuName,
                       DWORD dwExStyle, CCreateContext* pContext)
{
    HMENU hMenu = NULL;
    if (lpszMenuName != NULL) hMenu = ::LoadMenu(AfxGetResourceHandle(), lpszMenuName);

    BOOL bResult = CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle,
                                  rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                                  pParentWnd ? pParentWnd->GetSafeHwnd() : NULL,
                                  (HMENU)hMenu, pContext);
    if (!bResult && hMenu != NULL) ::DestroyMenu(hMenu);
    return bResult;
}

int CFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;
    if (!OnCreateClient(lpCreateStruct, (CCreateContext*)lpCreateStruct->lpCreateParams))
        return -1;
    return 0;
}

BOOL CFrameWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
    (void)lpcs;
    if (pContext == NULL || pContext->m_pNewViewClass == NULL) return TRUE;
    return CreateView(pContext, AFX_IDW_PANE_FIRST);
}

BOOL CFrameWnd::CreateView(CCreateContext* pContext, UINT nID)
{
    if (pContext == NULL || pContext->m_pNewViewClass == NULL) return FALSE;
    CView* pView = (CView*)pContext->m_pNewViewClass->CreateObject();
    if (pView == NULL) { AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC); return FALSE; }
    if (!pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rectDefault, this, nID, pContext)) {
        delete pView;
        return FALSE;
    }
    return TRUE;
}

BOOL CFrameWnd::DestroyWindow()
{
    // remember: the view/document are destroyed with the frame
    return CWnd::DestroyWindow();
}

void CFrameWnd::PostNcDestroy()
{
    CWinApp* pApp = AfxGetApp();
    if (pApp != NULL && pApp->m_pMainWnd == this) {
        pApp->m_pMainWnd = NULL;
        AfxSetMainWndHandle(NULL);
        // last window closed => quit the message loop
        ::PostQuitMessage(0);
    }
    delete this;
}

void CFrameWnd::SetActiveView(CView* pViewNew, BOOL bNotify)
{
    CView* pViewActiveOld = m_pViewActive;
    if (pViewActiveOld == pViewNew) return;
    if (bNotify && pViewActiveOld != NULL) pViewActiveOld->SendMessage(WM_ACTIVATEVIEW, 0, 0);
    m_pViewActive = pViewNew;
    if (bNotify && pViewNew != NULL) pViewNew->SendMessage(WM_ACTIVATEVIEW, 1, 0);
    if (bNotify) {
        // update command routing / titles
        UpdateFrameTitle(FALSE);
    }
}

void CFrameWnd::ActivateView(CView* pViewNew, BOOL bAdd)
{
    (void)bAdd;
    SetActiveView(pViewNew, TRUE);
    if (pViewNew != NULL) pViewNew->SetFocus();
}

CDocument* CFrameWnd::GetActiveDocument()
{
    if (m_pViewActive != NULL) return m_pViewActive->GetDocument();
    return NULL;
}

void CFrameWnd::SetTitle(LPCTSTR lpszTitle) { m_strTitle = lpszTitle; UpdateFrameTitle(FALSE); }
CString CFrameWnd::GetTitleString() const { return m_strTitle; }

void CFrameWnd::UpdateFrameTitle(BOOL bAddToTitle)
{
    CString strDocTitle;
    CDocument* pDoc = GetActiveDocument();
    if (pDoc != NULL) {
        strDocTitle = pDoc->GetTitle();
        if (pDoc->IsModified()) strDocTitle += _T("*");
    }

    CString strAppName;
    CWinApp* pApp = AfxGetApp();
    if (pApp != NULL && pApp->m_pszAppName != NULL) strAppName = pApp->m_pszAppName;
    if (strAppName.IsEmpty()) strAppName = m_strTitle;

    CString strTitle;
    if (!bAddToTitle || strDocTitle.IsEmpty() || strDocTitle.CompareNoCase(strAppName) == 0)
        strTitle = strDocTitle.IsEmpty() ? strAppName : strDocTitle;
    else {
        strTitle.Format(_T("%s - %s"), (LPCTSTR)strDocTitle, (LPCTSTR)strAppName);
    }
    if (strTitle.IsEmpty()) strTitle = m_strTitle;

    SetWindowText(strTitle);
    m_strTitle = strTitle;
}

void CFrameWnd::InitialUpdateFrame(CDocument* pDoc, BOOL bMakeVisible)
{
    // tell every view of this document that lives in this frame
    if (pDoc != NULL) {
        POSITION pos = pDoc->GetFirstViewPosition();
        while (pos != NULL) {
            CView* pView = pDoc->GetNextView(pos);
            if (pView != NULL && pView->GetSafeHwnd() != NULL && pView->GetParentFrame() == this) {
                if (m_pViewActive == NULL) SetActiveView(pView, FALSE);
                pView->OnInitialUpdate();
            }
        }
    }
    if (m_pViewActive != NULL && m_pViewActive->GetDocument() == NULL && pDoc != NULL) {
        m_pViewActive->SetDocument(pDoc);
        pDoc->AddView(m_pViewActive);
    }
    RecalcLayout(TRUE);

    if (bMakeVisible) {
        int nCmdShow = SW_SHOWNORMAL;
        CWinApp* pApp = AfxGetApp();
        if (pApp != NULL) {
            if (pApp->m_nCmdShow != -1) nCmdShow = pApp->m_nCmdShow;
            pApp->m_nCmdShow = -1;
            if (pApp->m_pMainWnd == NULL) {
                pApp->m_pMainWnd = this;
                AfxSetMainWndHandle(GetSafeHwnd());
            }
        }
        ShowWindow(nCmdShow);
        UpdateWindow();
        SetForegroundWindow();
    }
}

// ---------------------------------------------------------------------
//  layout
// ---------------------------------------------------------------------
void CFrameWnd::AddControlBar(CControlBar* pBar, int nPos)
{
    if (pBar == NULL) return;
    for (size_t i = 0; i < m_listBars.size(); ++i)
        if (m_listBars[i] == pBar) return;
    if (nPos < 0 || nPos > (int)m_listBars.size()) m_listBars.push_back(pBar);
    else m_listBars.insert(m_listBars.begin() + nPos, pBar);
}

void CFrameWnd::RemoveControlBar(CControlBar* pBar)
{
    for (size_t i = 0; i < m_listBars.size(); ++i) {
        if (m_listBars[i] == pBar) { m_listBars.erase(m_listBars.begin() + i); break; }
    }
}

CControlBar* CFrameWnd::GetControlBar(UINT nID)
{
    for (size_t i = 0; i < m_listBars.size(); ++i) {
        CControlBar* pBar = m_listBars[i];
        if (pBar != NULL && pBar->GetSafeHwnd() != NULL &&
            (UINT)pBar->GetDlgCtrlID() == nID) return pBar;
    }
    return NULL;
}

void CFrameWnd::ShowControlBar(CControlBar* pBar, BOOL bShow, BOOL bDelay)
{
    (void)bDelay;
    if (pBar == NULL || pBar->GetSafeHwnd() == NULL) return;
    if (bShow == pBar->IsVisible()) { RecalcLayout(); return; }
    pBar->ShowWindow(bShow ? SW_SHOWNA : SW_HIDE);
    RecalcLayout();
}

void CFrameWnd::LayoutBars(int cxClient, int cyClient)
{
    int nTop = 0, nBottom = cyClient, nLeft = 0, nRight = cxClient;

    for (size_t i = 0; i < m_listBars.size(); ++i) {
        CControlBar* pBar = m_listBars[i];
        if (pBar == NULL || pBar->GetSafeHwnd() == NULL || !pBar->IsVisible()) continue;
        CSize sz = pBar->CalcDynamicLayout(0, LM_HORZDOCK);
        switch (pBar->GetBarPosition()) {
        case CControlBar::barTop:
            pBar->MoveWindow(0, nTop, cxClient, sz.cy, TRUE);
            pBar->SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(cxClient, sz.cy));
            nTop += sz.cy;
            break;
        case CControlBar::barBottom:
            nBottom -= sz.cy;
            if (nBottom < nTop) nBottom = nTop;
            pBar->MoveWindow(0, nBottom, cxClient, sz.cy, TRUE);
            pBar->SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(cxClient, sz.cy));
            break;
        case CControlBar::barLeft:
            pBar->MoveWindow(nLeft, nTop, sz.cx, cyClient - nTop, TRUE);
            nLeft += sz.cx;
            break;
        case CControlBar::barRight:
            nRight -= sz.cx;
            pBar->MoveWindow(nRight, nTop, sz.cx, cyClient - nTop, TRUE);
            break;
        }
    }

    if (m_pViewActive != NULL && m_pViewActive->GetSafeHwnd() != NULL) {
        int cx = nRight - nLeft, cy = nBottom - nTop;
        if (cx < 0) cx = 0;
        if (cy < 0) cy = 0;
        m_pViewActive->MoveWindow(nLeft, nTop, cx, cy, TRUE);
    }
}

void CFrameWnd::RecalcLayout(BOOL bNotify)
{
    (void)bNotify;
    if (m_bInRecalcLayout) return;
    m_bInRecalcLayout = TRUE;

    CRect rc;
    GetClientRect(&rc);
    LayoutBars(rc.Width(), rc.Height());

    m_bInRecalcLayout = FALSE;
}

void CFrameWnd::DelayRecalcLayout(BOOL bNotify)
{
    if (m_hWnd != NULL) PostMessage(WM_SIZE, 0, 0);
    (void)bNotify;
}

void CFrameWnd::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (nType == SIZE_MINIMIZED) return;
    LayoutBars(cx, cy);
}

void CFrameWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
    CWnd::OnWindowPosChanged(lpwndpos);
    // reposition the control bars and the view (RecalcLayout is re-entrancy safe)
    RecalcLayout();
}

void CFrameWnd::OnSetFocus(CWnd* pOldWnd)
{
    CWnd::OnSetFocus(pOldWnd);
    if (m_pViewActive != NULL && m_pViewActive->GetSafeHwnd() != NULL)
        m_pViewActive->SetFocus();
}

void CFrameWnd::OnClose()
{
    CDocument* pDoc = GetActiveDocument();
    if (pDoc != NULL) {
        // the document saves itself and destroys the frames that host its views
        pDoc->OnFileClose();
        if (m_hWnd == NULL || !::IsWindow(m_hWnd)) return;   // already gone
    }
    DestroyWindow();
}

void CFrameWnd::OnDestroy()
{
    // detach control bars
    for (size_t i = 0; i < m_listBars.size(); ++i) {
        CControlBar* pBar = m_listBars[i];
        if (pBar != NULL && pBar->GetSafeHwnd() != NULL) pBar->DestroyWindow();
    }
    m_listBars.clear();
    if (m_pViewActive != NULL && m_pViewActive->GetSafeHwnd() != NULL) {
        CView* pView = m_pViewActive;
        m_pViewActive = NULL;
        CDocument* pDoc = pView->GetDocument();
        if (pDoc != NULL) { pDoc->RemoveView(pView); }
        pView->DestroyWindow();
    }
    CWnd::OnDestroy();
}

void CFrameWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == SC_CLOSE) { OnClose(); return; }
    CWnd::OnSysCommand(nID, lParam);
}

void CFrameWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CWnd::OnContextMenu(pWnd, point);
}

// ---------------------------------------------------------------------
//  command routing
// ---------------------------------------------------------------------
BOOL CFrameWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // 1) the active view (which routes to its document)
    if (m_pViewActive != NULL && m_pViewActive->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
        return TRUE;

    // 2) the frame itself
    if (CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return TRUE;

    // 3) the application object
    CWinApp* pApp = AfxGetApp();
    if (pApp != NULL && pApp->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return TRUE;

    return FALSE;
}

BOOL CFrameWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return CWnd::OnCommand(wParam, lParam);
}

BOOL CFrameWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    return CWnd::OnNotify(wParam, lParam, pResult);
}

BOOL CFrameWnd::PreTranslateMessage(MSG* pMsg)
{
    if (m_hAccelTable != NULL && ::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

BOOL CFrameWnd::TranslateAcceleratorMessage(MSG* pMsg)
{
    if (m_hAccelTable != NULL && ::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
        return TRUE;
    return FALSE;
}

BOOL CFrameWnd::LoadAccelTable(LPCTSTR lpszResourceName)
{
    if (m_hAccelTable != NULL) { ::DestroyAcceleratorTable(m_hAccelTable); m_hAccelTable = NULL; }
    m_hAccelTable = ::LoadAccelerators(AfxGetResourceHandle(), lpszResourceName);
    return m_hAccelTable != NULL;
}

void CFrameWnd::SetAcceleratorTable(HACCEL hAccel)
{
    if (m_hAccelTable != NULL) ::DestroyAcceleratorTable(m_hAccelTable);
    m_hAccelTable = hAccel;
}

// ---------------------------------------------------------------------
//  menu / command UI updating
// ---------------------------------------------------------------------
void CFrameWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    CWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
    if (bSysMenu || pPopupMenu == NULL) return;

    CCmdUI state;
    state.m_pMenu = pPopupMenu;
    state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (UINT i = 0; i < state.m_nIndexMax; ++i) {
        state.m_nIndex = i;
        state.m_nID = pPopupMenu->GetMenuItemID(i);
        if (state.m_nID == 0) continue;                      // menu separator
        if (state.m_nID == (UINT)-1) {
            // submenu: recurse
            CMenu* pSubMenu = pPopupMenu->GetSubMenu(i);
            if (pSubMenu != NULL) OnInitMenuPopup(pSubMenu, i, FALSE);
            continue;
        }
        state.m_pSubMenu = NULL;
        state.DoUpdate(this, m_bAutoMenuEnable && state.m_nID < 0xF000);
    }
}

LRESULT CFrameWnd::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
    (void)wParam; (void)lParam;
    // refresh the control bars (tool bar buttons, status bar panes)
    for (size_t i = 0; i < m_listBars.size(); ++i) {
        CControlBar* pBar = m_listBars[i];
        if (pBar == NULL || pBar->GetSafeHwnd() == NULL || !pBar->IsVisible()) continue;
        pBar->SendMessage(WM_IDLEUPDATECMDUI, (WPARAM)0, (LPARAM)0);
    }
    return 0;
}

LRESULT CFrameWnd::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
    CWnd::OnKickIdle(wParam, lParam);
    OnIdleUpdateCmdUI(0, 0);
    return 0;
}

LRESULT CFrameWnd::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
    m_nIDLastMessage = (UINT)wParam;
    m_nIDTracking = (UINT)wParam;
    CString strMessage;
    if (wParam != 0) GetMessageString((UINT)wParam, strMessage);
    SetMessageText(strMessage.IsEmpty() ? NULL : (LPCTSTR)strMessage);
    (void)lParam;
    return (LRESULT)(INT_PTR)m_nIDLastMessage;
}

LRESULT CFrameWnd::OnPopMessageString(WPARAM wParam, LPARAM lParam)
{
    (void)wParam; (void)lParam;
    SetMessageText(m_nIDTracking);
    return 0;
}

LRESULT CFrameWnd::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
    (void)wParam; (void)lParam;
    AfxMessageBox(_T("No help is available for this command."), MB_ICONINFORMATION);
    return TRUE;
}

LRESULT CFrameWnd::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
    (void)wParam; (void)lParam;
    return 0;
}

void CFrameWnd::GetMessageString(UINT nID, CString& rMessage) const
{
    rMessage.Empty();
    if (nID == 0) return;
    // MFC convention: "<menu caption>\n<status bar prompt>" in the string table
    CString strPrompt;
    if (!strPrompt.LoadString(nID)) return;
    int nSep = strPrompt.Find('\n');
    rMessage = (nSep >= 0) ? strPrompt.Mid(nSep + 1) : strPrompt;
    rMessage.Trim();
}

void CFrameWnd::SetMessageText(LPCTSTR lpszText)
{
    CStatusBar* pBar = (CStatusBar*)GetControlBar(AFX_IDW_STATUS_BAR);
    if (pBar != NULL && pBar->GetSafeHwnd() != NULL)
        pBar->SetPaneText(0, lpszText ? lpszText : _T("Ready"), TRUE);
}

void CFrameWnd::SetMessageText(UINT nID)
{
    CString str;
    GetMessageString(nID, str);
    SetMessageText(str.IsEmpty() ? _T("Ready") : (LPCTSTR)str);
}

void CFrameWnd::EnableModeless(BOOL bEnable)
{
    if (m_hWnd != NULL) ::EnableWindow(m_hWnd, bEnable);
}

void CFrameWnd::ShowOwnedPopups(BOOL bShow)
{
    HWND hWndChild = ::GetWindow(m_hWnd, GW_HWNDNEXT);
    while (hWndChild != NULL) {
        HWND hWndNext = ::GetWindow(hWndChild, GW_HWNDNEXT);
        if (::GetWindow(hWndChild, GW_OWNER) == m_hWnd)
            ::ShowWindow(hWndChild, bShow ? SW_SHOWNOACTIVATE : SW_HIDE);
        hWndChild = hWndNext;
    }
}

BOOL CFrameWnd::SaveAllModified()
{
    CDocument* pDoc = GetActiveDocument();
    return (pDoc != NULL) ? pDoc->SaveModified() : TRUE;
}

void CFrameWnd::CloseAllDocuments(BOOL bEndSession)
{
    CDocument* pDoc = GetActiveDocument();
    if (pDoc != NULL) pDoc->OnFileClose();
    (void)bEndSession;
}

void CFrameWnd::OnUpdateControlBarMenu(CCmdUI* pCmdUI)
{
    CControlBar* pBar = GetControlBar(pCmdUI->m_nID);
    pCmdUI->Enable(pBar != NULL);
    if (pBar != NULL) pCmdUI->SetCheck(pBar->IsVisible() ? 1 : 0);
}

void CFrameWnd::OnBarCheck(UINT nID)
{
    CControlBar* pBar = GetControlBar(nID);
    if (pBar == NULL) return;
    ShowControlBar(pBar, !pBar->IsVisible(), TRUE);
}

void CFrameWnd::UpdateMenuItems(HMENU hMenu) { (void)hMenu; }
CFrameWnd* CFrameWnd::GetParentFrame() const { return (CFrameWnd*)CWnd::GetParent(); }

// ---------------------------------------------------------------------
//  CMDIFrameWnd
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CMDIFrameWnd, CFrameWnd)

CMDIFrameWnd::CMDIFrameWnd() : m_hWndMDIClient(NULL) {}
CMDIFrameWnd::~CMDIFrameWnd() {}

BEGIN_MESSAGE_MAP(CMDIFrameWnd, CFrameWnd)
    ON_COMMAND(ID_WINDOW_NEW, CMDIFrameWnd::OnWindowNew)
    ON_COMMAND(ID_WINDOW_CASCADE, CMDIFrameWnd::OnWindowCascade)
    ON_COMMAND(ID_WINDOW_TILE_HORZ, CMDIFrameWnd::OnWindowTileHorz)
    ON_COMMAND(ID_WINDOW_TILE_VERT, CMDIFrameWnd::OnWindowTileVert)
    ON_COMMAND(ID_WINDOW_ARRANGE, CMDIFrameWnd::OnWindowArrangeIcons)
    ON_COMMAND_RANGE(ID_WINDOW_MRU_CHILD1, ID_WINDOW_MRU_CHILD16, CMDIFrameWnd::OnMDIWindowCmd)
    ON_UPDATE_COMMAND_UI_RANGE(ID_WINDOW_MRU_CHILD1, ID_WINDOW_MRU_CHILD16, CMDIFrameWnd::OnUpdateMDIWindowCmd)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_CASCADE, CMDIFrameWnd::OnUpdateMDIWindowCmd)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_HORZ, CMDIFrameWnd::OnUpdateMDIWindowCmd)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_VERT, CMDIFrameWnd::OnUpdateMDIWindowCmd)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_ARRANGE, CMDIFrameWnd::OnUpdateMDIWindowCmd)
    ON_MESSAGE(WM_COMMANDHELP, CMDIFrameWnd::OnCommandHelpMDI)
END_MESSAGE_MAP()

BOOL CMDIFrameWnd::CreateClient(LPCREATESTRUCT lpCreateStruct, CMenu* pWindowMenu)
{
    (void)lpCreateStruct;
    CLIENTCREATESTRUCT ccs;
    ccs.hWindowMenu = (pWindowMenu != NULL) ? pWindowMenu->GetSafeHmenu() : NULL;
    ccs.idFirstChild = AFX_IDM_FIRST_MDICHILD;

    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | MDIS_ALLCHILDSTYLES;
    m_hWndMDIClient = ::CreateWindow(_T("MDIClient"), NULL, dwStyle, 0, 0, 0, 0,
                                     m_hWnd, (HMENU)(UINT_PTR)AFX_IDW_PANE_FIRST,
                                     AfxGetInstanceHandle(), &ccs);
    if (m_hWndMDIClient == NULL) return FALSE;
    ::SendMessage(m_hWndMDIClient, WM_MDISETMENU, (WPARAM)ccs.hWindowMenu, 0);
    return TRUE;
}

BOOL CMDIFrameWnd::OnCreateClient(LPCREATESTRUCT lpCreateStruct, CCreateContext* pContext)
{
    (void)pContext;
    // find the "Window" menu of the frame menu so MDI can append the child list
    CMenu* pMenu = GetMenu();
    CMenu* pWindowMenu = NULL;
    if (pMenu != NULL) {
        int nCount = (int)pMenu->GetMenuItemCount();
        for (int i = nCount - 1; i >= 0; --i) {
            CString str = pMenu->GetMenuString(i, MF_BYPOSITION);
            str.Remove('&');
            if (str.CompareNoCase(_T("Window")) == 0 || str.CompareNoCase(_T("&Window")) == 0) {
                pWindowMenu = pMenu->GetSubMenu(i);
                break;
            }
        }
    }
    if (!CreateClient(lpCreateStruct, pWindowMenu)) return FALSE;
    return TRUE;
}

CMDIChildWnd* CMDIFrameWnd::MDIGetActive(BOOL* pbMaximized) const
{
    if (m_hWndMDIClient == NULL) return NULL;
    BOOL bMax = FALSE;
    HWND hWnd = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMax);
    if (pbMaximized != NULL) *pbMaximized = bMax;
    if (hWnd == NULL) return NULL;
    CWnd* pWnd = CWnd::FromHandle(hWnd);
    return (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd))) ? (CMDIChildWnd*)pWnd : NULL;
}

void CMDIFrameWnd::MDIActivate(CWnd* pWndActivate)
{
    ::SendMessage(m_hWndMDIClient, WM_MDIACTIVATE,
                  (WPARAM)(pWndActivate ? pWndActivate->GetSafeHwnd() : NULL), 0);
}

void CMDIFrameWnd::MDIAddMenu(CMenu* pMenu, LPCTSTR lpszWindowMenuName)
{
    if (pMenu == NULL) return;
    int n = (int)pMenu->GetMenuItemCount();
    for (int i = n - 1; i >= 0; --i) {
        CString str = pMenu->GetMenuString(i, MF_BYPOSITION);
        str.Remove('&');
        if (str.CompareNoCase(lpszWindowMenuName) == 0) {
            ::SendMessage(m_hWndMDIClient, WM_MDISETMENU, 0, (LPARAM)pMenu->GetSubMenu(i)->GetSafeHmenu());
            MDIRefreshMenu();
            return;
        }
    }
}

void CMDIFrameWnd::MDISetMenu(CMenu* pFrameMenu, CMenu* pWindowMenu)
{
    ::SendMessage(m_hWndMDIClient, WM_MDISETMENU,
                  (WPARAM)(pFrameMenu ? pFrameMenu->GetSafeHmenu() : NULL),
                  (LPARAM)(pWindowMenu ? pWindowMenu->GetSafeHmenu() : NULL));
    DrawMenuBar();
}

void CMDIFrameWnd::MDIRefreshMenu() { ::SendMessage(m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0); DrawMenuBar(); }
void CMDIFrameWnd::MDITile(int nType) { ::SendMessage(m_hWndMDIClient, WM_MDITILE, nType, 0); }
void CMDIFrameWnd::MDICascade(int nType) { ::SendMessage(m_hWndMDIClient, WM_MDICASCADE, nType, 0); }
void CMDIFrameWnd::MDIIconArrange() { ::SendMessage(m_hWndMDIClient, WM_MDIICONARRANGE, 0, 0); }
void CMDIFrameWnd::MDIMaximize(CWnd* pWnd) { ::SendMessage(m_hWndMDIClient, WM_MDIMAXIMIZE, (WPARAM)(pWnd ? pWnd->GetSafeHwnd() : NULL), 0); }
void CMDIFrameWnd::MDIRestore(CWnd* pWnd) { ::SendMessage(m_hWndMDIClient, WM_MDIRESTORE, (WPARAM)(pWnd ? pWnd->GetSafeHwnd() : NULL), 0); }
void CMDIFrameWnd::MDINext(CWnd* pWndCur, BOOL bPrevious)
{
    HWND hWndCur = (pWndCur != NULL) ? pWndCur->GetSafeHwnd() : NULL;
    if (hWndCur == NULL) { CMDIChildWnd* pActive = MDIGetActive(); if (pActive) hWndCur = pActive->GetSafeHwnd(); }
    if (hWndCur == NULL) return;
    HWND hWndNext = ::GetWindow(hWndCur, bPrevious ? GW_HWNDPREV : GW_HWNDNEXT);
    if (hWndNext == NULL)
        hWndNext = ::GetWindow(hWndCur, bPrevious ? GW_HWNDLAST : GW_HWNDFIRST);
    if (hWndNext != NULL && hWndNext != hWndCur) MDIActivate(CWnd::FromHandle(hWndNext));
}

int CMDIFrameWnd::GetMDIChildCount() const
{
    if (m_hWndMDIClient == NULL) return 0;
    int n = 0;
    HWND hWndChild = ::GetWindow(m_hWndMDIClient, GW_CHILD);
    while (hWndChild != NULL) { ++n; hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT); }
    return n;
}

CMDIChildWnd* CMDIFrameWnd::CreateNewChild(CRuntimeClass* pClass, UINT nResource,
                                           CDocument* pDoc, CCreateContext* pContext)
{
    if (pClass == NULL) return NULL;
    CMDIChildWnd* pChild = (CMDIChildWnd*)pClass->CreateObject();
    if (pChild == NULL) return NULL;

    CCreateContext context;
    if (pContext != NULL) context = *pContext;
    if (pDoc != NULL) context.m_pCurrentDoc = pDoc;

    if (!pChild->LoadFrame(nResource, WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, this, &context)) {
        delete pChild;
        return NULL;
    }
    return pChild;
}

BOOL CMDIFrameWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // route to the active MDI child first
    CMDIChildWnd* pActive = MDIGetActive();
    if (pActive != NULL && pActive->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return TRUE;
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMDIFrameWnd::PreTranslateMessage(MSG* pMsg)
{
    if (m_hWndMDIClient != NULL && ::TranslateMDISysAccel(m_hWndMDIClient, pMsg))
        return TRUE;
    return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMDIFrameWnd::OnWindowNew() { /* handled by the doc template */ }
void CMDIFrameWnd::OnWindowTileHorz() { MDITile(MDITILE_HORIZONTAL); }
void CMDIFrameWnd::OnWindowTileVert() { MDITile(MDITILE_VERTICAL); }
void CMDIFrameWnd::OnWindowCascade() { MDICascade(0); }
void CMDIFrameWnd::OnWindowArrangeIcons() { MDIIconArrange(); }

void CMDIFrameWnd::OnMDIWindowCmd(UINT nID)
{
    int nIndex = (int)nID - (int)ID_WINDOW_MRU_CHILD1;
    if (nIndex < 0) return;
    HWND hWndChild = ::GetWindow(m_hWndMDIClient, GW_CHILD);
    while (hWndChild != NULL && nIndex-- > 0) hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
    if (hWndChild != NULL) MDIActivate(CWnd::FromHandle(hWndChild));
}

void CMDIFrameWnd::OnUpdateMDIWindowCmd(CCmdUI* pCmdUI)
{
    if (pCmdUI->m_nID == ID_WINDOW_CASCADE || pCmdUI->m_nID == ID_WINDOW_TILE_HORZ ||
        pCmdUI->m_nID == ID_WINDOW_TILE_VERT || pCmdUI->m_nID == ID_WINDOW_ARRANGE) {
        pCmdUI->Enable(GetMDIChildCount() > 0);
        return;
    }
    int nIndex = (int)pCmdUI->m_nID - (int)ID_WINDOW_MRU_CHILD1;
    HWND hWndChild = ::GetWindow(m_hWndMDIClient, GW_CHILD);
    while (hWndChild != NULL && nIndex-- > 0) hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
    pCmdUI->Enable(hWndChild != NULL);
    if (hWndChild != NULL && pCmdUI->m_pMenu != NULL) {
        TCHAR szTitle[256];
        ::GetWindowText(hWndChild, szTitle, 256);
        CString str;
        str.Format(_T("&%d %s"), (int)pCmdUI->m_nID - (int)ID_WINDOW_MRU_CHILD1 + 1, szTitle);
        pCmdUI->SetText(str);
    }
}

void CMDIFrameWnd::OnUpdateWindowMenu(CCmdUI* pCmdUI) { pCmdUI->Enable(TRUE); }
LRESULT CMDIFrameWnd::OnCommandHelpMDI(WPARAM wParam, LPARAM lParam) { return CFrameWnd::OnCommandHelp(wParam, lParam); }
void CMDIFrameWnd::UpdateClientWindowMenu() { MDIRefreshMenu(); }

LRESULT CMDIFrameWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (m_hWndMDIClient != NULL)
        return ::DefFrameProc(m_hWnd, m_hWndMDIClient, message, wParam, lParam);
    return CFrameWnd::DefWindowProc(message, wParam, lParam);
}

// ---------------------------------------------------------------------
//  CMDIChildWnd
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CMDIChildWnd, CFrameWnd)

CMDIChildWnd::CMDIChildWnd() : m_bPseudoInactive(FALSE) {}
CMDIChildWnd::~CMDIChildWnd() {}

BEGIN_MESSAGE_MAP(CMDIChildWnd, CFrameWnd)
    ON_WM_MDIACTIVATE()
    ON_MESSAGE(WM_COMMANDHELP, CMDIChildWnd::OnCommandHelpMDI)
END_MESSAGE_MAP()

BOOL CMDIChildWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                          const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext)
{
    if (pParentWnd == NULL) return FALSE;
    HWND hWndClient = pParentWnd->GetMDIClientHandle();
    if (hWndClient == NULL) return FALSE;

    if (lpszClassName == NULL)
        lpszClassName = AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);

    if (!CWnd::CreateEx(0, lpszClassName, lpszWindowName, dwStyle,
                        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                        hWndClient, (HMENU)(UINT_PTR)AFX_IDW_PANE_FIRST, pContext))
        return FALSE;
    return TRUE;
}

BOOL CMDIChildWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
    if (pParentWnd == NULL || !pParentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
        pParentWnd = GetTopLevelFrame();
        if (pParentWnd == NULL || !pParentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) return FALSE;
    }
    CMDIFrameWnd* pFrame = (CMDIFrameWnd*)pParentWnd;

    CString strTitle;
    if (pContext != NULL && pContext->m_pCurrentDoc != NULL)
        strTitle = pContext->m_pCurrentDoc->GetTitle();
    if (strTitle.IsEmpty() && !strTitle.LoadString(nIDResource)) {
        CWinApp* pApp = AfxGetApp();
        strTitle = (pApp != NULL && pApp->m_pszAppName != NULL) ? pApp->m_pszAppName : _T("Document");
    }
    m_strTitle = strTitle;

    if (!Create(NULL, strTitle, dwDefaultStyle, rectDefault, pFrame, pContext)) return FALSE;
    if (::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nIDResource), RT_ACCELERATOR) != NULL)
        LoadAccelTable(MAKEINTRESOURCE(nIDResource));
    UpdateFrameTitle(FALSE);
    return TRUE;
}

LRESULT CMDIChildWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_SIZE && wParam == SIZE_MINIMIZED)
        return CFrameWnd::DefWindowProc(message, wParam, lParam);
    return ::DefMDIChildProc(m_hWnd, message, wParam, lParam);
}

void CMDIChildWnd::ActivateFrame(int nCmdShow)
{
    CMDIFrameWnd* pFrame = GetMDIFrame();
    if (pFrame == NULL) return;
    if (nCmdShow == -1) nCmdShow = SW_SHOWNORMAL;
    if (nCmdShow == SW_SHOWMAXIMIZED) pFrame->MDIMaximize(this);
    else if (nCmdShow == SW_SHOWMINIMIZED) ::ShowWindow(m_hWnd, SW_SHOWMINIMIZED);
    else pFrame->MDIRestore(this);
    ::ShowWindow(m_hWnd, nCmdShow);
    pFrame->MDIActivate(this);
    SetForegroundWindow();
}

CMDIFrameWnd* CMDIChildWnd::GetMDIFrame() const
{
    CWnd* pClient = CWnd::GetParent();
    if (pClient == NULL) return NULL;
    CWnd* pFrame = pClient->GetParent();
    return (pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) ? (CMDIFrameWnd*)pFrame : NULL;
}

BOOL CMDIChildWnd::DestroyWindow() { return CWnd::DestroyWindow(); }

void CMDIChildWnd::OnSetFocus(CWnd* pOldWnd) { CFrameWnd::OnSetFocus(pOldWnd); }

BOOL CMDIChildWnd::PreTranslateMessage(MSG* pMsg) { return CFrameWnd::PreTranslateMessage(pMsg); }

void CMDIChildWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
    (void)pActivateWnd; (void)pDeactivateWnd;
    if (bActivate) UpdateFrameTitle(FALSE);
}

int CMDIChildWnd::OnCreateMDI(LPCREATESTRUCT lpCreateStruct) { return CFrameWnd::OnCreate(lpCreateStruct); }
void CMDIChildWnd::OnWindowActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{ CWnd::OnActivate(nState, pWndOther, bMinimized); }
LRESULT CMDIChildWnd::OnCommandHelpMDI(WPARAM wParam, LPARAM lParam) { return CFrameWnd::OnCommandHelp(wParam, lParam); }
void CMDIChildWnd::OnSysCommandMDI(UINT nID, LPARAM lParam) { CFrameWnd::OnSysCommand(nID, lParam); }
LRESULT CMDIChildWnd::OnMDISetMenu(WPARAM wParam, LPARAM lParam) { (void)wParam; (void)lParam; return 0; }

// CFrameWnd::ActivateFrame (SDI)
void CFrameWnd::ActivateFrame(int nCmdShow)
{
    if (nCmdShow == -1) {
        CWinApp* pApp = AfxGetApp();
        nCmdShow = (pApp != NULL && pApp->m_nCmdShow != -1) ? pApp->m_nCmdShow : SW_SHOWNORMAL;
    }
    ShowWindow(nCmdShow);
    SetForegroundWindow();
}

