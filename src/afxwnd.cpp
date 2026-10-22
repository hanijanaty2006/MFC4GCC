// =====================================================================
//  afxwnd.cpp -- CCmdTarget, CWnd, CMenu, CCmdUI, the shared window
//                procedure and the HWND <-> CWnd maps.
// =====================================================================
#include <afxwin.h>
#include <afxcmn.h>
#include <afxext.h>
#include <map>
#include <vector>

// ---------------------------------------------------------------------
//  HWND <-> CWnd maps
// ---------------------------------------------------------------------
static std::map<HWND, CWnd*> s_permWnds;   // windows owned by a CWnd object
static std::map<HWND, CWnd*> s_tempWnds;   // temporary wrappers (MFC temp map)
static CRITICAL_SECTION s_wndLock;
static bool s_wndLockInit = false;

static void AfxWndLock()
{
    if (!s_wndLockInit) { ::InitializeCriticalSection(&s_wndLock); s_wndLockInit = true; }
    ::EnterCriticalSection(&s_wndLock);
}
static void AfxWndUnlock() { if (s_wndLockInit) ::LeaveCriticalSection(&s_wndLock); }

void AFXAPI AfxRegisterWnd(HWND hWnd, CWnd* pWnd)
{
    if (hWnd == NULL || pWnd == NULL) return;
    AfxWndLock();
    s_permWnds[hWnd] = pWnd;
    AfxWndUnlock();
}

void AFXAPI AfxUnregisterWnd(HWND hWnd)
{
    if (hWnd == NULL) return;
    AfxWndLock();
    s_permWnds.erase(hWnd);
    AfxWndUnlock();
}

CWnd* AFXAPI AfxLookupWndPerm(HWND hWnd)
{
    if (hWnd == NULL) return NULL;
    CWnd* p = NULL;
    AfxWndLock();
    std::map<HWND, CWnd*>::iterator it = s_permWnds.find(hWnd);
    if (it != s_permWnds.end()) p = it->second;
    AfxWndUnlock();
    return p;
}

CWnd* AFXAPI AfxLookupWnd(HWND hWnd)
{
    if (hWnd == NULL) return NULL;
    CWnd* p = AfxLookupWndPerm(hWnd);
    if (p != NULL) return p;
    AfxWndLock();
    std::map<HWND, CWnd*>::iterator it = s_tempWnds.find(hWnd);
    if (it != s_tempWnds.end()) p = it->second;
    AfxWndUnlock();
    return p;
}

void AFXAPI AfxAddTempWnd(CWnd* pWnd)
{
    if (pWnd == NULL || pWnd->GetSafeHwnd() == NULL) return;
    AfxWndLock();
    s_tempWnds[pWnd->GetSafeHwnd()] = pWnd;
    AfxWndUnlock();
}

void AFXAPI AfxRemoveTempWnd(HWND hWnd)
{
    if (hWnd == NULL) return;
    AfxWndLock();
    s_tempWnds.erase(hWnd);
    AfxWndUnlock();
}

// window currently being created (thread local, set by CWnd::CreateEx)
static DWORD s_tlsCreating = TLS_OUT_OF_INDEXES;

CWnd* AFXAPI AfxGetCreatingWnd()
{
    if (s_tlsCreating == TLS_OUT_OF_INDEXES) return NULL;
    return (CWnd*)::TlsGetValue(s_tlsCreating);
}

void AFXAPI AfxSetCreatingWnd(CWnd* pWnd)
{
    if (s_tlsCreating == TLS_OUT_OF_INDEXES) {
        s_tlsCreating = ::TlsAlloc();
        if (s_tlsCreating == TLS_OUT_OF_INDEXES) return;
    }
    ::TlsSetValue(s_tlsCreating, pWnd);
}

// ---------------------------------------------------------------------
//  The shared window procedure
// ---------------------------------------------------------------------
LRESULT CALLBACK AfxWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    CWnd* pWnd = AfxLookupWnd(hWnd);
    if (pWnd == NULL) {
        if (nMsg == WM_NCCREATE) {
            // the CWnd that asked for this window is remembered per-thread
            CWnd* pCreating = AfxGetCreatingWnd();
            if (pCreating != NULL && pCreating->GetSafeHwnd() == NULL) {
                AfxRegisterWnd(hWnd, pCreating);
                pCreating->m_hWnd = hWnd;
                pWnd = pCreating;
            }
        }
        if (pWnd == NULL) return ::DefWindowProc(hWnd, nMsg, wParam, lParam);
    }
    return pWnd->WindowProc(nMsg, wParam, lParam);
}

// ---------------------------------------------------------------------
//  Message map lookup / dispatch
// ---------------------------------------------------------------------
const AFX_MSGMAP_ENTRY* AFXAPI AfxFindMessageEntry(const AFX_MSGMAP* lpMap, UINT nMsg,
                                                   UINT nCode, UINT nID)
{
    while (lpMap != NULL) {
        const AFX_MSGMAP_ENTRY* lpEntry = lpMap->lpEntries;
        while (lpEntry != NULL && lpEntry->nMessage != 0) {
            if (lpEntry->nMessage == AFX_MSGMAP_REGISTERED) {
                UINT nRegistered = *(const UINT*)(lpEntry->nExtra);
                if (nRegistered == nMsg) return lpEntry;
            } else if (lpEntry->nMessage == nMsg && lpEntry->nCode == nCode &&
                       (lpEntry->nID == nID ||
                        (lpEntry->nLastID != 0 && nID >= lpEntry->nID && nID <= lpEntry->nLastID))) {
                return lpEntry;
            }
            ++lpEntry;
        }
        lpMap = lpMap->lpBase;
    }
    return NULL;
}

LRESULT AFXAPI AfxDispatchMsgEntry(CCmdTarget* pTarget, const AFX_MSGMAP_ENTRY* lpEntry,
                                   WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if (pTarget == NULL || lpEntry == NULL || lpEntry->pfn == NULL) return 0;
    return lpEntry->pfn(pTarget, wParam, lParam, pResult);
}

LPCSTR AFXAPI AfxGetSigName(UINT nSig)
{
    static const char* s_names[] = {
        "end","vv","bv","uv","hcv","vw","vi","vb","bb","vT","vup","vwww","vwwPt","vwPt",
        "vwSz","vwWb","vW","vWww","vwwW","vwwH","vbw","vbd","vwd","vul","vxy","vwii",
        "vPt","vWPt","vMMI","vWP","vHDROP","vRect","vwRect","vwStyle","vwT","viMIS",
        "viDIS","vMwb","iCreate","iWww","iwLw","lPt","lwl","lbwl","lwwM","bwl","bDC",
        "bWww","bwSpt","bWCD","bHelp","hDCWw","cmd","vnId","bwId","notify","notifyEx"
    };
    if (nSig < sizeof(s_names) / sizeof(s_names[0])) return s_names[nSig];
    return "unknown";
}

// ---------------------------------------------------------------------
//  Thunk helpers (CWnd / CMenu / CDC / CListBox are complete here)
// ---------------------------------------------------------------------
CWnd* AFXAPI AfxMsgFromHandleWnd(HWND hWnd) { return CWnd::FromHandle(hWnd); }
CMenu* AFXAPI AfxMsgFromHandleMenu(HMENU hMenu) { return CMenu::FromHandle(hMenu); }
CDC* AFXAPI AfxMsgFromHandleDC(HDC hDC) { return CDC::FromHandle(hDC); }
CListBox* AFXAPI AfxMsgFromHandleListBox(HWND hWnd) { return (CListBox*)CWnd::FromHandle(hWnd); }
HWND AFXAPI AfxMsgSafeHwnd(CCmdTarget* pTarget)
{
    if (pTarget == NULL || !pTarget->IsKindOf(RUNTIME_CLASS(CWnd))) return NULL;
    return ((CWnd*)pTarget)->GetSafeHwnd();
}

// ---------------------------------------------------------------------
//  CCmdTarget
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CCmdTarget, CObject)

static const AFX_MSGMAP_ENTRY s_emptyEntries[] = { { 0, 0, 0, 0, AfxSig_end, NULL, 0 } };
const AFX_MSGMAP CObject::messageMap = { NULL, s_emptyEntries };
const AFX_MSGMAP* PASCAL CObject::GetThisMessageMap() { return &CObject::messageMap; }

CCmdTarget::CCmdTarget() {}
CCmdTarget::~CCmdTarget() {}

BEGIN_MESSAGE_MAP(CCmdTarget, CObject)
END_MESSAGE_MAP()

BOOL CCmdTarget::DispatchThroughMap(const AFX_MSGMAP_ENTRY* lpEntry, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if (lpEntry == NULL || lpEntry->pfn == NULL) return FALSE;
    lpEntry->pfn(this, wParam, lParam, pResult);
    return TRUE;
}

BOOL CCmdTarget::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    if (pHandlerInfo != NULL) {
        // caller only wants to know whether somebody would handle it
        const AFX_MSGMAP* pMessageMap = GetMessageMap();
        UINT nMsg = (nCode == CN_UPDATE_COMMAND_UI) ? (UINT)WM_IDLEUPDATECMDUI : (UINT)WM_COMMAND;
        const AFX_MSGMAP_ENTRY* lpEntry = AfxFindMessageEntry(pMessageMap, nMsg, (UINT)nCode, nID);
        if (lpEntry == NULL) return FALSE;
        pHandlerInfo->nID = nID;
        pHandlerInfo->nCode = nCode;
        pHandlerInfo->pmfTarget = (void*)lpEntry->pfn;
        return TRUE;
    }

    const AFX_MSGMAP* pMessageMap = GetMessageMap();
    if (pMessageMap == NULL) return FALSE;

    if (nCode == CN_UPDATE_COMMAND_UI) {
        CCmdUI* pCmdUI = (CCmdUI*)pExtra;
        if (pCmdUI == NULL) return FALSE;
        const AFX_MSGMAP_ENTRY* lpEntry =
            AfxFindMessageEntry(pMessageMap, WM_IDLEUPDATECMDUI, (UINT)nCode, nID);
        if (lpEntry == NULL) return FALSE;
        lpEntry->pfn(this, 0, (LPARAM)pCmdUI, NULL);
        return TRUE;
    }

    {
        // CN_COMMAND (menu / accelerator / BN_CLICKED) as well as every other
        // control notification code (EN_UPDATE, CBN_SELCHANGE, LBN_DBLCLK, ...)
        const AFX_MSGMAP_ENTRY* lpEntry =
            AfxFindMessageEntry(pMessageMap, WM_COMMAND, (UINT)nCode, nID);
        if (lpEntry == NULL) return FALSE;
        BOOL bResult = TRUE;
        if (lpEntry->nSig == AfxSig_bwId) {
            // ON_COMMAND_EX / ON_CONTROL_EX: handler decides whether it handled it
            LRESULT lResult = lpEntry->pfn(this, (WPARAM)nID, (LPARAM)pExtra, NULL);
            bResult = (lResult != 0);
        } else {
            lpEntry->pfn(this, (WPARAM)nID, (LPARAM)pExtra, NULL);
        }
        return bResult;
    }
}

// ---------------------------------------------------------------------
//  CCmdUI
// ---------------------------------------------------------------------
CCmdUI::CCmdUI()
    : m_nID(0), m_nIndex(0), m_nIndexMax(0), m_pMenu(NULL), m_pSubMenu(NULL),
      m_nOtherContext(0), m_pOther(NULL), m_bEnableChanged(FALSE), m_bContinueRouting(FALSE)
{
}

void CCmdUI::Enable(BOOL bOn)
{
    m_bEnableChanged = TRUE;
    if (m_pMenu != NULL) {
        if (m_pMenu->m_hMenu == NULL) return;
        if (m_pMenu->EnableMenuItem(m_nID, MF_BYCOMMAND | (bOn ? MF_ENABLED : (MF_DISABLED | MF_GRAYED))) == (UINT)-1)
            ;   // not found: nothing to do
        return;
    }
    if (m_pOther != NULL) {
        // tool bar button / status bar pane
        if (m_pOther->IsKindOf(RUNTIME_CLASS(CToolBar))) {
            ((CToolBar*)m_pOther)->GetToolBarCtrl().EnableButton(m_nID, bOn);
            return;
        }
        if (m_pOther->IsKindOf(RUNTIME_CLASS(CStatusBar))) {
            int nIndex = ((CStatusBar*)m_pOther)->CommandToIndex(m_nID);
            if (nIndex >= 0) ((CStatusBar*)m_pOther)->GetStatusBarCtrl().EnablePane(nIndex, bOn);
            return;
        }
        if (m_pOther->IsKindOf(RUNTIME_CLASS(CWnd))) {
            ((CWnd*)m_pOther)->EnableWindow(bOn);
            return;
        }
    }
}

void CCmdUI::SetCheck(int nCheck)
{
    if (m_pMenu != NULL) {
        if (m_pMenu->m_hMenu == NULL) return;
        if (nCheck == 1) {
            m_pMenu->CheckMenuRadioItem(m_nID, m_nID, m_nID, MF_BYCOMMAND | MF_CHECKED);
        } else {
            m_pMenu->CheckMenuRadioItem(m_nID, m_nID, m_nID, MF_BYCOMMAND | MF_UNCHECKED);
        }
        return;
    }
    if (m_pOther != NULL && m_pOther->IsKindOf(RUNTIME_CLASS(CToolBar))) {
        ((CToolBar*)m_pOther)->GetToolBarCtrl().CheckButton(m_nID, nCheck == 1);
        return;
    }
    if (m_pOther != NULL && m_pOther->IsKindOf(RUNTIME_CLASS(CWnd))) {
        ((CWnd*)m_pOther)->SendMessage(BM_SETCHECK, nCheck != 0, 0);
    }
}

void CCmdUI::SetRadio(BOOL bOn)
{
    if (m_pMenu != NULL) {
        if (m_pMenu->m_hMenu == NULL) return;
        m_pMenu->CheckMenuRadioItem(m_nID, m_nID, m_nID,
                                    MF_BYCOMMAND | (bOn ? MF_CHECKED : MF_UNCHECKED));
        return;
    }
    SetCheck(bOn ? 1 : 0);
}

void CCmdUI::SetText(LPCTSTR lpszText)
{
    if (m_pMenu != NULL) {
        if (m_pMenu->m_hMenu == NULL) return;
        m_pMenu->ModifyMenu(m_nID, MF_BYCOMMAND | MF_STRING, m_nID, lpszText);
        return;
    }
    if (m_pOther != NULL) {
        if (m_pOther->IsKindOf(RUNTIME_CLASS(CToolBar))) {
            int nIndex = ((CToolBar*)m_pOther)->GetToolBarCtrl().CommandToIndex(m_nID);
            if (nIndex >= 0) ((CToolBar*)m_pOther)->GetToolBarCtrl().SetButtonText(nIndex, lpszText);
            return;
        }
        if (m_pOther->IsKindOf(RUNTIME_CLASS(CStatusBar))) {
            int nIndex = ((CStatusBar*)m_pOther)->CommandToIndex(m_nID);
            if (nIndex >= 0) ((CStatusBar*)m_pOther)->SetPaneText(nIndex, lpszText, TRUE);
            return;
        }
        if (m_pOther->IsKindOf(RUNTIME_CLASS(CWnd)))
            ((CWnd*)m_pOther)->SetWindowText(lpszText);
    }
}

BOOL CCmdUI::DoUpdate(CCmdTarget* pTarget, BOOL bDisableIfNoHandler)
{
    if (m_nID < 0xF000) {
        // menu or button: route the update command
        BOOL bAutoMenuEnable = TRUE;
        if (pTarget != NULL && pTarget->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
            bAutoMenuEnable = ((CFrameWnd*)pTarget)->m_bAutoMenuEnable;

        m_bEnableChanged = FALSE;
        BOOL bHandled = (pTarget != NULL) &&
                        pTarget->OnCmdMsg(m_nID, CN_UPDATE_COMMAND_UI, this, NULL);

        if (!bHandled) {
            // MFC semantics: when auto menu enable is on, an item that has no
            // ON_UPDATE_COMMAND_UI handler is disabled unless it does have an
            // ON_COMMAND handler.
            if (bDisableIfNoHandler) {
                AFX_CMDHANDLERINFO info;
                if (pTarget != NULL && pTarget->OnCmdMsg(m_nID, CN_COMMAND, NULL, &info))
                    Enable(TRUE);
                else
                    Enable(FALSE);
            } else {
                Enable(TRUE);
            }
        } else if (!m_bEnableChanged && (bAutoMenuEnable || m_pMenu != NULL)) {
            Enable(TRUE);
        }
    }
    m_bContinueRouting = FALSE;
    return TRUE;
}

// ---------------------------------------------------------------------
//  CCreateContext
// ---------------------------------------------------------------------
CCreateContext::CCreateContext()
    : m_pNewViewClass(NULL), m_pCurrentView(NULL), m_pCurrentDoc(NULL),
      m_pNewDocTemplate(NULL), m_pLastView(NULL), m_pLastFrame(NULL)
{
}
void CCreateContext::RememberCurrentView()   { m_pLastView = m_pCurrentView; }
void CCreateContext::RememberCurrentDocument() { m_pCurrentDoc = NULL; }
void CCreateContext::RememberCurrentFrame()  { m_pLastFrame = NULL; }
CView* CCreateContext::GetLastView()         { return (CView*)m_pLastView; }
CDocument* CCreateContext::GetLastDocument() { return NULL; }
CFrameWnd* CCreateContext::GetLastFrame()    { return (CFrameWnd*)m_pLastFrame; }

// ---------------------------------------------------------------------
//  CMenu
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CMenu, CObject)

CMenu::CMenu() : m_hMenu(NULL), m_hMenuOwner(NULL) {}

CMenu::~CMenu()
{
    if (m_hMenu != NULL) DestroyMenu();
}

BOOL CMenu::Attach(HMENU hMenu)
{
    if (hMenu == NULL) return FALSE;
    m_hMenu = hMenu;
    return TRUE;
}

HMENU CMenu::Detach()
{
    HMENU hMenu = m_hMenu;
    if (m_hMenuOwner != NULL) { AttachToMenuBar(NULL); }
    m_hMenu = NULL;
    return hMenu;
}

CMenu* CMenu::FromHandle(HMENU hMenu)
{
    if (hMenu == NULL) return NULL;
    static CMenu s_temp;          // MFC uses a temp map; a single wrapper is enough
    s_temp.m_hMenu = hMenu;
    return &s_temp;
}

BOOL CMenu::CreateMenu() { return Attach(::CreateMenu()); }
BOOL CMenu::CreatePopupMenu() { return Attach(::CreatePopupMenu()); }
BOOL CMenu::LoadMenu(LPCTSTR lpszResourceName) { return Attach(::LoadMenu(AfxGetResourceHandle(), lpszResourceName)); }
BOOL CMenu::LoadMenu(UINT nIDResource) { return Attach(::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(nIDResource))); }
BOOL CMenu::LoadMenuIndirect(const void* lpMenuTemplate) { return Attach(::LoadMenuIndirect(lpMenuTemplate)); }

BOOL CMenu::DestroyMenu()
{
    if (m_hMenuOwner != NULL) AttachToMenuBar(NULL);
    BOOL bResult = m_hMenu != NULL;
    if (bResult) bResult = ::DestroyMenu(m_hMenu) ? TRUE : FALSE;
    m_hMenu = NULL;
    return bResult;
}

CMenu* CMenu::GetSubMenu(int nPos) const { return FromHandle(::GetSubMenu(m_hMenu, nPos)); }
BOOL CMenu::DeleteMenu(UINT nPosition, UINT nFlags) { return ::DeleteMenu(m_hMenu, nPosition, nFlags); }
BOOL CMenu::RemoveMenu(UINT nPosition, UINT nFlags) { return ::RemoveMenu(m_hMenu, nPosition, nFlags); }
BOOL CMenu::AppendMenu(UINT nFlags, UINT_PTR nIDNewItem, LPCTSTR lpszNewItem)
{ return ::AppendMenu(m_hMenu, nFlags, nIDNewItem, lpszNewItem); }
BOOL CMenu::AppendMenu(UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp)
{ return ::AppendMenu(m_hMenu, nFlags, nIDNewItem, pBmp ? (LPCTSTR)(ULONG_PTR)pBmp->GetSafeHandle() : NULL); }
BOOL CMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem, LPCTSTR lpszNewItem)
{ return ::InsertMenu(m_hMenu, nPosition, nFlags, nIDNewItem, lpszNewItem); }
BOOL CMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp)
{ return ::InsertMenu(m_hMenu, nPosition, nFlags, nIDNewItem, pBmp ? (LPCTSTR)(ULONG_PTR)pBmp->GetSafeHandle() : NULL); }
BOOL CMenu::ModifyMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem, LPCTSTR lpszNewItem)
{ return ::ModifyMenu(m_hMenu, nPosition, nFlags, nIDNewItem, lpszNewItem); }
BOOL CMenu::ModifyMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp)
{ return ::ModifyMenu(m_hMenu, nPosition, nFlags, nIDNewItem, pBmp ? (LPCTSTR)(ULONG_PTR)pBmp->GetSafeHandle() : NULL); }

UINT CMenu::GetMenuItemCount() const { return (UINT)::GetMenuItemCount(m_hMenu); }
UINT CMenu::GetMenuItemID(int nPos) const { return ::GetMenuItemID(m_hMenu, nPos); }
UINT CMenu::GetMenuState(UINT nID, UINT nFlags) const { return ::GetMenuState(m_hMenu, nID, nFlags); }

int CMenu::GetMenuString(UINT nID, LPTSTR lpString, int nMaxCount, UINT nFlags) const
{ return ::GetMenuString(m_hMenu, nID, lpString, nMaxCount, nFlags); }

CString CMenu::GetMenuString(UINT nID, UINT nFlags) const
{
    TCHAR buf[512];
    int n = ::GetMenuString(m_hMenu, nID, buf, 512, nFlags);
    return (n > 0) ? CString(buf, n) : CString();
}

BOOL CMenu::GetMenuInfo(LPMENUINFO lpMenuInfo) const { return ::GetMenuInfo(m_hMenu, lpMenuInfo); }
BOOL CMenu::SetMenuInfo(LPCMENUINFO lpMenuInfo) { return ::SetMenuInfo(m_hMenu, lpMenuInfo); }
BOOL CMenu::GetMenuItemInfo(UINT nItem, LPMENUITEMINFO lpMenuItemInfo, BOOL fByPos)
{ return ::GetMenuItemInfo(m_hMenu, nItem, fByPos, lpMenuItemInfo); }
BOOL CMenu::SetMenuItemInfo(UINT nItem, LPMENUITEMINFO lpMenuItemInfo, BOOL fByPos)
{ return ::SetMenuItemInfo(m_hMenu, nItem, fByPos, lpMenuItemInfo); }

UINT CMenu::EnableMenuItem(UINT nIDEnableItem, UINT nEnable) { return ::EnableMenuItem(m_hMenu, nIDEnableItem, nEnable); }
UINT CMenu::CheckMenuItem(UINT nIDCheckItem, UINT nCheck) { return ::CheckMenuItem(m_hMenu, nIDCheckItem, nCheck); }
UINT CMenu::CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags)
{ return ::CheckMenuRadioItem(m_hMenu, nIDFirst, nIDLast, nIDItem, nFlags); }
BOOL CMenu::SetDefaultItem(UINT uItem, BOOL fByPos) { return ::SetMenuDefaultItem(m_hMenu, uItem, fByPos); }
UINT CMenu::GetDefaultItem(UINT gmdiFlags, BOOL fByPos) { return ::GetMenuDefaultItem(m_hMenu, fByPos, gmdiFlags); }

BOOL CMenu::SetMenuItemBitmaps(UINT nPosition, UINT nFlags, const CBitmap* pBmpUnchecked, const CBitmap* pBmpChecked)
{
    return ::SetMenuItemBitmaps(m_hMenu, nPosition, nFlags,
                                pBmpUnchecked ? (HBITMAP)pBmpUnchecked->GetSafeHandle() : NULL,
                                pBmpChecked ? (HBITMAP)pBmpChecked->GetSafeHandle() : NULL) != 0;
}

BOOL CMenu::TrackPopupMenu(UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT lpRect)
{
    return ::TrackPopupMenu(m_hMenu, nFlags, x, y, 0,
                            pWnd != NULL ? pWnd->GetSafeHwnd() : NULL, lpRect);
}

BOOL CMenu::DrawMenuBar() { return m_hMenuOwner ? ::DrawMenuBar(m_hMenuOwner) : FALSE; }

void CMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) { (void)lpDrawItemStruct; }
void CMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) { (void)lpMeasureItemStruct; }

void CMenu::AttachToMenuBar(HWND hWnd)
{
    if (m_hMenuOwner != NULL) ::DrawMenuBar(m_hMenuOwner);
    m_hMenuOwner = hWnd;
    if (hWnd != NULL) {
        ::SetMenu(hWnd, m_hMenu);
        ::DrawMenuBar(hWnd);
    }
}

void CMenu::DetachFromMenuBar()
{
    if (m_hMenuOwner != NULL) ::SetMenu(m_hMenuOwner, NULL);
    m_hMenuOwner = NULL;
}

// ---------------------------------------------------------------------
//  CWnd
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CWnd, CCmdTarget)

CWnd::CWnd()
    : m_hWnd(NULL), m_hWndOwner(NULL), m_pfnSuper(NULL), m_nMsgLast(0), m_wpLast(0),
      m_lpLast(0), m_nFlags(0), m_bDestroying(FALSE)
{
}

CWnd::~CWnd()
{
    if (m_hWnd != NULL && ::IsWindow(m_hWnd)) {
        AfxUnregisterWnd(m_hWnd);
        AfxRemoveTempWnd(m_hWnd);
    }
    m_hWnd = NULL;
}

CWnd* CWnd::FromHandlePermanent(HWND hWnd) { return AfxLookupWndPerm(hWnd); }

CWnd* CWnd::FromHandle(HWND hWnd)
{
    if (hWnd == NULL) return NULL;
    CWnd* pWnd = AfxLookupWnd(hWnd);
    if (pWnd != NULL) return pWnd;
    // create a temporary wrapper, deleted by DeleteTempMap() (from OnIdle)
    pWnd = new CWnd();
    pWnd->m_hWnd = hWnd;
    AfxAddTempWnd(pWnd);
    return pWnd;
}

void CWnd::DeleteTempMap()
{
    for (std::map<HWND, CWnd*>::iterator it = s_tempWnds.begin(); it != s_tempWnds.end(); ++it) {
        it->second->m_hWnd = NULL;
        delete it->second;
    }
    s_tempWnds.clear();
    CDC::DeleteTempMap();
}

BOOL CWnd::DestroyWindow(HWND hWnd) { return ::DestroyWindow(hWnd); }

BOOL CWnd::Attach(HWND hWndNew)
{
    if (hWndNew == NULL || !::IsWindow(hWndNew)) return FALSE;
    if (m_hWnd != NULL) Detach();
    m_hWnd = hWndNew;
    AfxRegisterWnd(hWndNew, this);
    return TRUE;
}

HWND CWnd::Detach()
{
    HWND hWnd = m_hWnd;
    if (hWnd != NULL) {
        AfxUnregisterWnd(hWnd);
        AfxRemoveTempWnd(hWnd);
    }
    m_hWnd = NULL;
    return hWnd;
}

BOOL CWnd::SubclassWindow(HWND hWnd)
{
    if (hWnd == NULL || !::IsWindow(hWnd)) return FALSE;
    if (m_hWnd != NULL) Detach();
    m_hWnd = hWnd;
    AfxRegisterWnd(hWnd, this);
    m_pfnSuper = (WNDPROC)::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&AfxWndProc);
    PreSubclassWindow();
    return TRUE;
}

HWND CWnd::UnsubclassWindow()
{
    HWND hWnd = m_hWnd;
    if (hWnd != NULL && m_pfnSuper != NULL) {
        ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)m_pfnSuper);
        m_pfnSuper = NULL;
    }
    m_hWnd = NULL;
    if (hWnd != NULL) AfxUnregisterWnd(hWnd);
    return hWnd;
}

BOOL CWnd::SubclassDlgItem(UINT nID, CWnd* pParent)
{
    if (pParent == NULL) return FALSE;
    HWND hWnd = ::GetDlgItem(pParent->GetSafeHwnd(), (int)nID);
    if (hWnd == NULL) return FALSE;
    return SubclassWindow(hWnd);
}

void CWnd::PreSubclassWindow() {}
void CWnd::PostNcDestroy() {}

// ---------------------------------------------------------------------
//  window class registration (MFC style auto-generated class names)
// ---------------------------------------------------------------------
struct AFX_WNDCLASS_ENTRY { LPCTSTR pszName; UINT nStyle; HCURSOR hCursor; HBRUSH hbr; HICON hIcon; };
static std::vector<CString> s_registeredNames;
static CRITICAL_SECTION s_clsLock;
static bool s_clsLockInit = false;

LPCTSTR AFXAPI AfxRegisterWndClass(UINT nClassStyle, HCURSOR hCursor, HBRUSH hbrBackground, HICON hIcon)
{
    if (!s_clsLockInit) { ::InitializeCriticalSection(&s_clsLock); s_clsLockInit = true; }

    TCHAR szName[128];
    _stprintf(szName, _T("Afx:%p:%x:%p:%p:%p"),
              (void*)AfxGetInstanceHandle(), nClassStyle, (void*)hCursor, (void*)hbrBackground, (void*)hIcon);

    ::EnterCriticalSection(&s_clsLock);
    for (size_t i = 0; i < s_registeredNames.size(); ++i) {
        if (s_registeredNames[i] == szName) {
            ::LeaveCriticalSection(&s_clsLock);
            return (LPCTSTR)s_registeredNames[i];
        }
    }
    WNDCLASS wnd;
    memset(&wnd, 0, sizeof(wnd));
    if (::GetClassInfo(AfxGetInstanceHandle(), szName, &wnd)) {
        s_registeredNames.push_back(CString(szName));
        ::LeaveCriticalSection(&s_clsLock);
        return (LPCTSTR)s_registeredNames.back();
    }
    wnd.style = nClassStyle;
    wnd.lpfnWndProc = &AfxWndProc;
    wnd.cbClsExtra = 0;
    wnd.cbWndExtra = 0;
    wnd.hInstance = AfxGetInstanceHandle();
    wnd.hIcon = hIcon;
    wnd.hCursor = hCursor ? hCursor : ::LoadCursor(NULL, IDC_ARROW);
    wnd.hbrBackground = hbrBackground;
    wnd.lpszMenuName = NULL;
    wnd.lpszClassName = szName;
    if (!::RegisterClass(&wnd)) {
        ::LeaveCriticalSection(&s_clsLock);
        return NULL;
    }
    s_registeredNames.push_back(CString(szName));
    ::LeaveCriticalSection(&s_clsLock);
    return (LPCTSTR)s_registeredNames.back();
}

BOOL AFXAPI AfxRegisterClass(const WNDCLASS* lpWndClass)
{
    WNDCLASS wnd = *lpWndClass;
    wnd.lpfnWndProc = (lpWndClass->lpfnWndProc != NULL) ? lpWndClass->lpfnWndProc : &AfxWndProc;
    wnd.hInstance = AfxGetInstanceHandle();
    if (wnd.hCursor == NULL) wnd.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    if (::RegisterClass(&wnd)) return TRUE;
    DWORD dwError = ::GetLastError();
    return (dwError == ERROR_CLASS_ALREADY_EXISTS);
}

BOOL AFXAPI AfxDeferRegisterClass(DWORD fToRegister)
{
    (void)fToRegister;
    // register the "everything" class used when no class name is supplied
    if (AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL) == NULL)
        return FALSE;
    return TRUE;
}

// ---------------------------------------------------------------------
//  CWnd -- creation
// ---------------------------------------------------------------------
BOOL CWnd::PreCreateWindow(CREATESTRUCT& cs)
{
    if (cs.lpszClass == NULL) {
        cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
        if (cs.lpszClass == NULL) return FALSE;
    }
    return TRUE;
}

BOOL CWnd::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                    int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU nIDorHMenu,
                    LPVOID lpParam)
{
    if (m_hWnd != NULL) return FALSE;

    CREATESTRUCT cs;
    memset(&cs, 0, sizeof(cs));
    cs.cx = nWidth; cs.cy = nHeight;
    cs.x = x; cs.y = y;
    cs.style = (int)dwStyle;
    cs.hwndParent = hWndParent;
    cs.hMenu = nIDorHMenu;
    cs.hInstance = AfxGetInstanceHandle();
    cs.lpszClass = lpszClassName;
    cs.lpszName = lpszWindowName;
    cs.dwExStyle = dwExStyle;
    cs.lpCreateParams = lpParam;

    if (!PreCreateWindow(cs)) return FALSE;

    // remember which CWnd is being created so AfxWndProc can hook WM_NCCREATE
    CWnd* pPrevCreating = AfxGetCreatingWnd();
    AfxSetCreatingWnd(this);

    HWND hWnd = ::CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, (DWORD)cs.style,
                                 cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu,
                                 cs.hInstance, cs.lpCreateParams);
    AfxSetCreatingWnd(pPrevCreating);

    if (hWnd == NULL) return FALSE;

    if (m_hWnd == NULL) {   // WM_NCCREATE hook did not run (e.g. foreign wndproc)
        m_hWnd = hWnd;
        AfxRegisterWnd(hWnd, this);
    }
    return TRUE;
}

BOOL CWnd::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                    const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
{
    return CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle,
                    rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                    pParentWnd != NULL ? pParentWnd->GetSafeHwnd() : NULL,
                    (HMENU)(UINT_PTR)nID, lpParam);
}

BOOL CWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                  const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
    return CreateEx(0, lpszClassName, lpszWindowName, dwStyle,
                    rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                    pParentWnd != NULL ? pParentWnd->GetSafeHwnd() : NULL,
                    (HMENU)(UINT_PTR)nID, (LPVOID)pContext);
}

BOOL CWnd::CreateControl(LPCTSTR lpszClass, LPCTSTR lpszWindowName, DWORD dwStyle,
                         const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpCreateStruct)
{
    return CreateEx(0, lpszClass, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpCreateStruct);
}

BOOL CWnd::CreateControl(LPCTSTR lpszClass, LPCTSTR lpszWindowName, DWORD dwStyle,
                         int x, int y, int cx, int cy, CWnd* pParentWnd, UINT nID, LPVOID lpCreateStruct)
{
    return CreateEx(0, lpszClass, lpszWindowName, dwStyle, x, y, cx, cy,
                    pParentWnd != NULL ? pParentWnd->GetSafeHwnd() : NULL,
                    (HMENU)(UINT_PTR)nID, lpCreateStruct);
}

BOOL CWnd::DestroyWindow()
{
    if (m_hWnd == NULL || !::IsWindow(m_hWnd)) return FALSE;
    return ::DestroyWindow(m_hWnd);
}

// ---------------------------------------------------------------------
//  CWnd -- message routing
// ---------------------------------------------------------------------
LRESULT CWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    m_nMsgLast = message; m_wpLast = wParam; m_lpLast = lParam;

    LRESULT lResult = 0;
    BOOL bHandled = OnWndMsg(message, wParam, lParam, &lResult);
    if (!bHandled)
        lResult = DefWindowProc(message, wParam, lParam);
    if (message == WM_PAINT) {
        // belt and braces: make sure the update region is always validated,
        // otherwise the queue never drains (wine is picky about this)
        ::ValidateRect(m_hWnd, NULL);
    }

    if (message == WM_NCDESTROY) {
        HWND hWnd = m_hWnd;
        m_hWnd = NULL;
        AfxUnregisterWnd(hWnd);
        PostNcDestroy();
    }
    return lResult;
}

LRESULT CWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (m_hWnd == NULL) return 0;
    if (m_pfnSuper != NULL) return ::CallWindowProc(m_pfnSuper, m_hWnd, message, wParam, lParam);
    return ::DefWindowProc(m_hWnd, message, wParam, lParam);
}

LRESULT CWnd::DefWindowProc()
{
    return DefWindowProc(m_nMsgLast, m_wpLast, m_lpLast);
}

LRESULT CWnd::Default() { return DefWindowProc(); }

BOOL CWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    LRESULT lResult = 0;
    const AFX_MSGMAP* pMessageMap = GetMessageMap();

    if (message == WM_COMMAND) {
        if (OnCommand(wParam, lParam)) { *pResult = 1; return TRUE; }
        return FALSE;
    }
    if (message == WM_NOTIFY) {
        if (OnNotify(wParam, lParam, &lResult)) { *pResult = lResult; return TRUE; }
        return FALSE;
    }

    const AFX_MSGMAP_ENTRY* lpEntry = AfxFindMessageEntry(pMessageMap, message, 0, 0);
    if (lpEntry != NULL) {
        lpEntry->pfn(this, wParam, lParam, &lResult);
        *pResult = lResult;
        return TRUE;
    }
    *pResult = 0;
    return FALSE;
}

BOOL CWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT nID = LOWORD(wParam);
    int nCode = HIWORD(wParam);
    if (nID == 0) return FALSE;
    // nCode == CN_COMMAND (0) covers menu items, accelerators and BN_CLICKED;
    // every other code is a control notification (EN_UPDATE, CBN_SELCHANGE, ...)
    if (OnCmdMsg(nID, nCode, (void*)lParam, NULL)) return TRUE;
    return FALSE;
}

BOOL CWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    NMHDR* pNMHDR = (NMHDR*)lParam;
    if (pNMHDR == NULL) return FALSE;
    UINT nCode = (UINT)pNMHDR->code;
    UINT nID = (wParam != 0) ? (UINT)wParam : (UINT)::GetDlgCtrlID(pNMHDR->hwndFrom);

    const AFX_MSGMAP* pMessageMap = GetMessageMap();
    const AFX_MSGMAP_ENTRY* lpEntry = AfxFindMessageEntry(pMessageMap, WM_NOTIFY, nCode, nID);
    if (lpEntry == NULL) lpEntry = AfxFindMessageEntry(pMessageMap, WM_NOTIFY, nCode, (UINT)wParam);
    if (lpEntry == NULL) lpEntry = AfxFindMessageEntry(pMessageMap, WM_NOTIFY, nCode, 0);   // wildcard id
    if (lpEntry == NULL) lpEntry = AfxFindMessageEntry(pMessageMap, WM_NOTIFY, (UINT)-1, nID);
    if (lpEntry == NULL) return FALSE;

    LRESULT lResult = 0;
    if (lpEntry->nSig == AfxSig_notifyEx)
        lResult = lpEntry->pfn(this, nID, (LPARAM)pNMHDR, &lResult);
    else
        lpEntry->pfn(this, nID, (LPARAM)pNMHDR, &lResult);

    if (pResult != NULL) *pResult = lResult;
    return TRUE;
}

BOOL CWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    return CCmdTarget::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CWnd::PreTranslateMessage(MSG* pMsg)
{
    (void)pMsg;
    return FALSE;
}

BOOL CWnd::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    (void)message; (void)wParam; (void)lParam; (void)pResult;
    return FALSE;
}

BOOL CWnd::SendChildNotifyLastMsg(LRESULT* pResult)
{
    return SendChildNotifyLastMsg(m_hWnd, pResult);
}

LRESULT CWnd::SendChildNotifyLastMsg(HWND hWnd, LRESULT* pResult)
{
    LRESULT lResult = 0;
    CWnd* pReflect = CWnd::FromHandle(hWnd);
    if (pReflect != NULL && pReflect->OnChildNotify(0, 0, 0, &lResult)) {
        if (pResult != NULL) *pResult = lResult;
        return TRUE;
    }
    return FALSE;
}

BOOL CWnd::SendNotifyMessage(UINT message, WPARAM wParam, LPARAM lParam)
{ return ::SendNotifyMessage(m_hWnd, message, wParam, lParam) != 0; }

LRESULT CWnd::SendMessage(UINT message, WPARAM wParam, LPARAM lParam) const
{ return ::SendMessage(m_hWnd, message, wParam, lParam); }

BOOL CWnd::PostMessage(UINT message, WPARAM wParam, LPARAM lParam) const
{ return ::PostMessage(m_hWnd, message, wParam, lParam); }

void CWnd::EndModalLoop(int nResult) { (void)nResult; }

// ---------------------------------------------------------------------
//  CWnd -- attributes
// ---------------------------------------------------------------------
BOOL CWnd::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
    DWORD dwStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_STYLE);
    dwStyle = (dwStyle & ~dwRemove) | dwAdd;
    ::SetWindowLongPtr(m_hWnd, GWL_STYLE, (LONG_PTR)dwStyle);
    if (nFlags != 0)
        ::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
                       SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE |
                       SWP_FRAMECHANGED | nFlags);
    return TRUE;
}

BOOL CWnd::ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
    DWORD dwExStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
    dwExStyle &= ~dwRemove;
    dwExStyle |= dwAdd;
    ::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, dwExStyle);
    if (nFlags != 0)
        ::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER |
                       SWP_NOACTIVATE | SWP_FRAMECHANGED | nFlags);
    return TRUE;
}

CWnd* CWnd::GetParent() const { return FromHandle(::GetParent(m_hWnd)); }
CWnd* CWnd::SetParent(CWnd* pWndNewParent)
{ return FromHandle(::SetParent(m_hWnd, pWndNewParent != NULL ? pWndNewParent->GetSafeHwnd() : NULL)); }

CWnd* CWnd::GetParentOwner() const
{
    if (m_hWndOwner != NULL) return FromHandle(m_hWndOwner);
    return FromHandle(::GetWindow(m_hWnd, GW_OWNER));
}

CWnd* CWnd::GetTopLevelParent() const
{
    HWND hWnd = m_hWnd, hWndParent;
    while ((hWndParent = ::GetParent(hWnd)) != NULL) hWnd = hWndParent;
    return FromHandle(hWnd);
}

CWnd* CWnd::GetTopLevelFrame() const { return GetTopLevelParent(); }

CWnd* CWnd::GetSafeOwner(CWnd* pParent, HWND* pWndTop) const
{
    HWND hWnd = (pParent != NULL) ? pParent->GetSafeHwnd() : ::GetParent(m_hWnd);
    if (hWnd == NULL) hWnd = ::GetActiveWindow();
    if (pWndTop != NULL) *pWndTop = hWnd;
    return FromHandle(hWnd);
}

CWnd* CWnd::GetWindow(UINT nCmd) const { return FromHandle(::GetWindow(m_hWnd, nCmd)); }
CWnd* CWnd::GetFirstChildWindow() const { return FromHandle(::GetWindow(m_hWnd, GW_CHILD)); }
CWnd* CWnd::GetLastChildWindow() const
{
    HWND hWndChild = ::GetWindow(m_hWnd, GW_CHILD);
    if (hWndChild == NULL) return NULL;
    HWND hWndLast = hWndChild;
    while ((hWndChild = ::GetWindow(hWndLast, GW_HWNDNEXT)) != NULL) hWndLast = hWndChild;
    return FromHandle(hWndLast);
}
CWnd* CWnd::GetNextWindow(UINT nCmd) const { return FromHandle(::GetWindow(m_hWnd, nCmd)); }

CWnd* CWnd::GetDescendantWindow(int nID, BOOL bOnlyPerm) const
{
    if (nID == 0) return NULL;
    HWND hWndChild = ::GetDlgItem(m_hWnd, nID);
    if (hWndChild != NULL) {
        if (bOnlyPerm) return FromHandlePermanent(hWndChild);
        return FromHandle(hWndChild);
    }
    // search the whole subtree
    hWndChild = ::GetWindow(m_hWnd, GW_CHILD);
    while (hWndChild != NULL) {
        CWnd* pWnd = FromHandle(hWndChild)->GetDescendantWindow(nID, bOnlyPerm);
        if (pWnd != NULL) return pWnd;
        hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
    }
    return NULL;
}

CWnd* CWnd::GetDlgItem(int nID) const { return FromHandle(::GetDlgItem(m_hWnd, nID)); }
int CWnd::GetDlgCtrlID() const { return ::GetDlgCtrlID(m_hWnd); }
int CWnd::SetDlgCtrlID(int nID) { return (int)::SetWindowLongPtr(m_hWnd, GWLP_ID, nID); }
BOOL CWnd::IsChild(const CWnd* pWnd) const
{ return ::IsChild(m_hWnd, pWnd != NULL ? pWnd->GetSafeHwnd() : NULL); }
BOOL CWnd::IsWindowVisible() const { return ::IsWindowVisible(m_hWnd); }
BOOL CWnd::IsWindowEnabled() const { return ::IsWindowEnabled(m_hWnd); }
BOOL CWnd::IsIconic() const { return ::IsIconic(m_hWnd); }
BOOL CWnd::IsZoomed() const { return ::IsZoomed(m_hWnd); }
BOOL CWnd::EnableWindow(BOOL bEnable) { return ::EnableWindow(m_hWnd, bEnable); }
BOOL CWnd::IsDialogWindow() const
{
    TCHAR szClass[64];
    if (::GetClassName(m_hWnd, szClass, 64) == 0) return FALSE;
    return (_tcsicmp(szClass, _T("#32770")) == 0);
}

void CWnd::GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
{ ::GetWindowText(m_hWnd, lpszStringBuf, nMaxCount); }

void CWnd::GetWindowText(CString& rString) const
{
    int nLen = ::GetWindowTextLength(m_hWnd);
    LPTSTR p = rString.GetBuffer(nLen + 1);
    ::GetWindowText(m_hWnd, p, nLen + 1);
    rString.ReleaseBuffer();
}

int CWnd::GetWindowTextLength() const { return ::GetWindowTextLength(m_hWnd); }
void CWnd::SetWindowText(LPCTSTR lpszString) { ::SetWindowText(m_hWnd, lpszString); }
CString CWnd::GetWindowTextString() const { CString s; GetWindowText(s); return s; }

void CWnd::GetClientRect(LPRECT lpRect) const { ::GetClientRect(m_hWnd, lpRect); }
void CWnd::GetWindowRect(LPRECT lpRect) const { ::GetWindowRect(m_hWnd, lpRect); }
CRect CWnd::GetClientRect() const { CRect r; ::GetClientRect(m_hWnd, &r); return r; }
CRect CWnd::GetWindowRect() const { CRect r; ::GetWindowRect(m_hWnd, &r); return r; }

void CWnd::ScreenToClient(LPPOINT lpPoint) const { ::ScreenToClient(m_hWnd, lpPoint); }
void CWnd::ScreenToClient(LPRECT lpRect) const { ::MapWindowPoints(NULL, m_hWnd, (LPPOINT)lpRect, 2); }
void CWnd::ClientToScreen(LPPOINT lpPoint) const { ::ClientToScreen(m_hWnd, lpPoint); }
void CWnd::ClientToScreen(LPRECT lpRect) const { ::MapWindowPoints(m_hWnd, NULL, (LPPOINT)lpRect, 2); }
int CWnd::MapWindowPoints(CWnd* pWndTo, LPPOINT lpPoint, UINT nCount) const
{ return ::MapWindowPoints(m_hWnd, pWndTo != NULL ? pWndTo->GetSafeHwnd() : NULL, lpPoint, nCount); }
int CWnd::MapWindowPoints(CWnd* pWndTo, LPRECT lpRect) const
{ return ::MapWindowPoints(m_hWnd, pWndTo != NULL ? pWndTo->GetSafeHwnd() : NULL, (LPPOINT)lpRect, 2); }

BOOL CWnd::CenterWindow(CWnd* pAlternateOwner)
{
    (void)pAlternateOwner;
    RECT rcDlg, rcArea;
    ::GetWindowRect(m_hWnd, &rcDlg);
    HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
    if (hMonitor != NULL) {
        MONITORINFO mi; mi.cbSize = sizeof(mi);
        ::GetMonitorInfo(hMonitor, &mi);
        rcArea = mi.rcWork;
    } else {
        rcArea.left = rcArea.top = 0;
        rcArea.right = ::GetSystemMetrics(SM_CXSCREEN);
        rcArea.bottom = ::GetSystemMetrics(SM_CYSCREEN);
    }
    int x = (rcArea.left + rcArea.right) / 2 - (rcDlg.right - rcDlg.left) / 2;
    int y = (rcArea.top + rcArea.bottom) / 2 - (rcDlg.bottom - rcDlg.top) / 2;
    if (x < rcArea.left) x = rcArea.left;
    if (y < rcArea.top) y = rcArea.top;
    return ::SetWindowPos(m_hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
}

int CWnd::ShowWindow(int nCmdShow) { return ::ShowWindow(m_hWnd, nCmdShow); }
BOOL CWnd::ShowWindowAsync(int nCmdShow) { return ::ShowWindowAsync(m_hWnd, nCmdShow); }
BOOL CWnd::SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
{
    return ::SetWindowPos(m_hWnd, pWndInsertAfter != NULL ? pWndInsertAfter->GetSafeHwnd() : NULL,
                          x, y, cx, cy, nFlags);
}
UINT CWnd::ArrangeIconicWindows() { return ::ArrangeIconicWindows(m_hWnd); }
void CWnd::BringWindowToTop() { ::BringWindowToTop(m_hWnd); }
BOOL CWnd::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{ return ::MoveWindow(m_hWnd, x, y, nWidth, nHeight, bRepaint); }
BOOL CWnd::MoveWindow(LPCRECT lpRect, BOOL bRepaint)
{ return ::MoveWindow(m_hWnd, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint); }
BOOL CWnd::SetForegroundWindow() { return ::SetForegroundWindow(m_hWnd); }
HWND CWnd::SetActiveWindow() { return ::SetActiveWindow(m_hWnd); }
CWnd* CWnd::SetFocus() { return FromHandle(::SetFocus(m_hWnd)); }
BOOL CWnd::LockWindowUpdate() { return ::LockWindowUpdate(m_hWnd); }
void CWnd::UnlockWindowUpdate() { ::LockWindowUpdate(NULL); }

BOOL CWnd::RedrawWindow(LPCRECT lpRectUpdate, CRgn* prgnUpdate, UINT nFlags)
{ return ::RedrawWindow(m_hWnd, lpRectUpdate, prgnUpdate ? (HRGN)prgnUpdate->GetSafeHandle() : NULL, nFlags); }

BOOL CWnd::EnableScrollBar(int nSBFlags, UINT nArrowFlags) { return ::EnableScrollBar(m_hWnd, nSBFlags, nArrowFlags); }

UINT CWnd::SetTimer(UINT_PTR nIDEvent, UINT nElapse, void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD))
{ return ::SetTimer(m_hWnd, nIDEvent, nElapse, lpfnTimer); }
BOOL CWnd::KillTimer(UINT_PTR nIDEvent) { return ::KillTimer(m_hWnd, nIDEvent); }

HDC CWnd::GetDC() const { return ::GetDC(m_hWnd); }
HDC CWnd::GetWindowDC() const { return ::GetWindowDC(m_hWnd); }
int CWnd::ReleaseDC(HDC hDC) const { return ::ReleaseDC(m_hWnd, hDC); }
CDC* CWnd::GetDCEx(CRgn* prgnClip, DWORD flags)
{ return CDC::FromHandle(::GetDCEx(m_hWnd, prgnClip ? (HRGN)prgnClip->GetSafeHandle() : NULL, flags)); }
void CWnd::Print(CDC* pDC, DWORD dwFlags) const { ::SendMessage(m_hWnd, WM_PRINT, (WPARAM)pDC->GetSafeHdc(), dwFlags); }
void CWnd::PrintClient(CDC* pDC, DWORD dwFlags) const { ::SendMessage(m_hWnd, WM_PRINTCLIENT, (WPARAM)pDC->GetSafeHdc(), dwFlags); }

void CWnd::UpdateWindow() { ::UpdateWindow(m_hWnd); }
void CWnd::SetRedraw(BOOL bRedraw) { ::SendMessage(m_hWnd, WM_SETREDRAW, bRedraw, 0); }
BOOL CWnd::GetUpdateRect(LPRECT lpRect, BOOL bErase) { return ::GetUpdateRect(m_hWnd, lpRect, bErase); }
int CWnd::GetUpdateRgn(CRgn* pRgn, BOOL bErase) { return ::GetUpdateRgn(m_hWnd, (HRGN)pRgn->GetSafeHandle(), bErase); }
void CWnd::Invalidate(BOOL bErase) { ::InvalidateRect(m_hWnd, NULL, bErase); }
void CWnd::InvalidateRect(LPCRECT lpRect, BOOL bErase) { ::InvalidateRect(m_hWnd, lpRect, bErase); }
void CWnd::InvalidateRgn(CRgn* pRgn, BOOL bErase) { ::InvalidateRgn(m_hWnd, (HRGN)pRgn->GetSafeHandle(), bErase); }
void CWnd::ValidateRect(LPCRECT lpRect) { ::ValidateRect(m_hWnd, lpRect); }
void CWnd::ValidateRgn(CRgn* pRgn) { ::ValidateRgn(m_hWnd, (HRGN)pRgn->GetSafeHandle()); }
BOOL CWnd::ShowScrollBar(int nBar, BOOL bShow) { return ::ShowScrollBar(m_hWnd, nBar, bShow); }
void CWnd::ScrollWindow(int xAmount, int yAmount, LPCRECT lpRect, LPCRECT lpClipRect)
{ ::ScrollWindow(m_hWnd, xAmount, yAmount, lpRect, lpClipRect); }
int CWnd::ScrollWindowEx(int dx, int dy, LPCRECT lprcScroll, LPCRECT lprcClip, CRgn* prgnUpdate, LPRECT lprcUpdate, UINT flags)
{ return ::ScrollWindowEx(m_hWnd, dx, dy, lprcScroll, lprcClip, prgnUpdate ? (HRGN)prgnUpdate->GetSafeHandle() : NULL, lprcUpdate, flags); }
BOOL CWnd::FlashWindow(BOOL bInvert) { return ::FlashWindow(m_hWnd, bInvert); }
BOOL CWnd::SetWindowPlacement(const WINDOWPLACEMENT* lpwndpl) { return ::SetWindowPlacement(m_hWnd, lpwndpl); }
BOOL CWnd::GetWindowPlacement(WINDOWPLACEMENT* lpwndpl) const { return ::GetWindowPlacement(m_hWnd, lpwndpl); }
void CWnd::SetIcon(HICON hIcon, BOOL bBigIcon) { ::SendMessage(m_hWnd, WM_SETICON, bBigIcon ? ICON_BIG : ICON_SMALL, (LPARAM)hIcon); }
HICON CWnd::GetIcon(BOOL bBigIcon) const { return (HICON)::SendMessage(m_hWnd, WM_GETICON, bBigIcon ? ICON_BIG : ICON_SMALL, 0); }
BOOL CWnd::SetProp(LPCTSTR lpString, HANDLE hData) { return ::SetProp(m_hWnd, lpString, hData); }
HANDLE CWnd::GetProp(LPCTSTR lpString) const { return ::GetProp(m_hWnd, lpString); }
HANDLE CWnd::RemoveProp(LPCTSTR lpString) { return ::RemoveProp(m_hWnd, lpString); }
void CWnd::DragAcceptFiles(BOOL bAccept) { ::DragAcceptFiles(m_hWnd, bAccept); }
CWnd* CWnd::SetCapture() { return FromHandle(::SetCapture(m_hWnd)); }
BOOL CWnd::ReleaseCapture() { return ::ReleaseCapture(); }
BOOL CWnd::OpenIcon() { return ::OpenIcon(m_hWnd); }
BOOL CWnd::CloseWindow() { return ::CloseWindow(m_hWnd); }

CMenu* CWnd::GetMenu() const { return CMenu::FromHandle(::GetMenu(m_hWnd)); }
BOOL CWnd::SetMenu(CMenu* pMenu) { return ::SetMenu(m_hWnd, pMenu != NULL ? pMenu->GetSafeHmenu() : NULL); }
CMenu* CWnd::GetSystemMenu(BOOL bRevert) { return CMenu::FromHandle(::GetSystemMenu(m_hWnd, bRevert)); }
BOOL CWnd::DrawMenuBar() { return ::DrawMenuBar(m_hWnd); }
BOOL CWnd::HiliteMenuItem(UINT nIDHiliteItem, UINT nHilite) { return ::HiliteMenuItem(m_hWnd, ::GetMenu(m_hWnd), nIDHiliteItem, nHilite); }

CWnd* CWnd::FindWindow(LPCTSTR lpszClassName, LPCTSTR lpszWindowName)
{ return FromHandle(::FindWindow(lpszClassName, lpszWindowName)); }
CWnd* CWnd::WindowFromPoint(POINT point) { return FromHandle(::WindowFromPoint(point)); }
CWnd* CWnd::ChildWindowFromPoint(HWND hWndParent, POINT point) { return FromHandle(::ChildWindowFromPoint(hWndParent, point)); }
CWnd* CWnd::ChildWindowFromPoint(POINT point) const { return FromHandle(::ChildWindowFromPoint(m_hWnd, point)); }
CWnd* CWnd::ChildWindowFromPoint(POINT point, UINT nFlags) const { return FromHandle(::ChildWindowFromPointEx(m_hWnd, point, nFlags)); }

// ---------------------------------------------------------------------
//  CWnd -- dialog helpers
// ---------------------------------------------------------------------
UINT CWnd::GetDlgItemInt(int nID, BOOL* lpTrans, BOOL bSigned) const
{ return ::GetDlgItemInt(m_hWnd, nID, lpTrans, bSigned); }
void CWnd::SetDlgItemInt(int nID, UINT nValue, BOOL bSigned) { ::SetDlgItemInt(m_hWnd, nID, nValue, bSigned); }
int CWnd::GetDlgItemText(int nID, LPTSTR lpStr, int nMaxCount) const { return ::GetDlgItemText(m_hWnd, nID, lpStr, nMaxCount); }
void CWnd::SetDlgItemText(int nID, LPCTSTR lpszString) { ::SetDlgItemText(m_hWnd, nID, lpszString); }
void CWnd::CheckDlgButton(int nIDButton, UINT nCheck) { ::CheckDlgButton(m_hWnd, nIDButton, nCheck); }
void CWnd::CheckRadioButton(int nIDFirstButton, int nIDLastButton, int nIDCheckButton)
{ ::CheckRadioButton(m_hWnd, nIDFirstButton, nIDLastButton, nIDCheckButton); }
UINT CWnd::IsDlgButtonChecked(int nIDButton) const { return ::IsDlgButtonChecked(m_hWnd, nIDButton); }
LRESULT CWnd::SendDlgItemMessage(int nID, UINT message, WPARAM wParam, LPARAM lParam)
{ return ::SendDlgItemMessage(m_hWnd, nID, message, wParam, lParam); }
CWnd* CWnd::GetNextDlgGroupItem(CWnd* pWndCtl, BOOL bPrevious) const
{ return FromHandle(::GetNextDlgGroupItem(m_hWnd, pWndCtl != NULL ? pWndCtl->GetSafeHwnd() : NULL, bPrevious)); }
CWnd* CWnd::GetNextDlgTabItem(CWnd* pWndCtl, BOOL bPrevious) const
{ return FromHandle(::GetNextDlgTabItem(m_hWnd, pWndCtl != NULL ? pWndCtl->GetSafeHwnd() : NULL, bPrevious)); }
void CWnd::NextDlgCtrl() const { ::SendMessage(m_hWnd, WM_NEXTDLGCTL, 0, 0); }
void CWnd::PrevDlgCtrl() const { ::SendMessage(m_hWnd, WM_NEXTDLGCTL, 1, 0); }
void CWnd::GotoDlgCtrl(CWnd* pWndCtl) { ::SetFocus(pWndCtl != NULL ? pWndCtl->GetSafeHwnd() : NULL); }

void CWnd::DoDataExchange(CDataExchange* pDX) { (void)pDX; }

BOOL CWnd::UpdateData(BOOL bSaveAndValidate)
{
    return AfxDoDataExchange(this, bSaveAndValidate);
}

LRESULT CWnd::OnToolHitTest(CPoint point, TOOLINFO* pInfo) const
{
    (void)point; (void)pInfo;
    return -1;
}

// ---------------------------------------------------------------------
//  CWnd default message handlers
// ---------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CWnd, CCmdTarget)
    ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
    ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
    ON_MESSAGE(WM_POPMESSAGESTRING, OnPopMessageString)
    ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
    ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

LRESULT CWnd::OnKickIdle(WPARAM, LPARAM) { return 0; }
LRESULT CWnd::OnSetMessageString(WPARAM, LPARAM) { return 0; }
LRESULT CWnd::OnPopMessageString(WPARAM, LPARAM) { return 0; }
LRESULT CWnd::OnCommandHelp(WPARAM, LPARAM) { return 0; }
LRESULT CWnd::OnHelpHitTest(WPARAM, LPARAM) { return 0; }

int CWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) { (void)lpCreateStruct; return 0; }
void CWnd::OnClose() {}
void CWnd::OnDestroy() {}
void CWnd::OnNcDestroy() {}
void CWnd::OnPaint() {}
BOOL CWnd::OnEraseBkgnd(CDC* pDC) { (void)pDC; return (BOOL)DefWindowProc(WM_ERASEBKGND, (WPARAM)pDC->GetSafeHdc(), 0); }
void CWnd::OnSize(UINT nType, int cx, int cy) { (void)nType; (void)cx; (void)cy; }
void CWnd::OnMove(int x, int y) { (void)x; (void)y; }
void CWnd::OnSizing(UINT fwSide, LPRECT pRect) { (void)fwSide; (void)pRect; }
void CWnd::OnMoving(UINT fwSide, LPRECT pRect) { (void)fwSide; (void)pRect; }
void CWnd::OnTimer(UINT_PTR nIDEvent) { (void)nIDEvent; }
void CWnd::OnSetFocus(CWnd* pOldWnd) { (void)pOldWnd; }
void CWnd::OnKillFocus(CWnd* pNewWnd) { (void)pNewWnd; }
void CWnd::OnLButtonDown(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
void CWnd::OnLButtonUp(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
void CWnd::OnLButtonDblClk(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
void CWnd::OnRButtonDown(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
void CWnd::OnRButtonUp(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
void CWnd::OnRButtonDblClk(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
void CWnd::OnMButtonDown(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
void CWnd::OnMButtonUp(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
void CWnd::OnMButtonDblClk(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
void CWnd::OnMouseMove(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
BOOL CWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) { (void)nFlags; (void)zDelta; (void)pt; return FALSE; }
void CWnd::OnMouseHover(UINT nFlags, CPoint point) { (void)nFlags; (void)point; }
void CWnd::OnMouseLeave() {}
BOOL CWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{ (void)pWnd; return (BOOL)DefWindowProc(WM_SETCURSOR, (WPARAM)(pWnd ? pWnd->GetSafeHwnd() : NULL), MAKELPARAM(nHitTest, message)); }
LRESULT CWnd::OnNcHitTest(CPoint point) { return DefWindowProc(WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y)); }
void CWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI) { (void)lpMMI; }
void CWnd::OnWindowPosChanging(WINDOWPOS* lpwndpos) { (void)lpwndpos; }
void CWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos) { (void)lpwndpos; }
void CWnd::OnShowWindow(BOOL bShow, UINT nStatus) { (void)bShow; (void)nStatus; }
void CWnd::OnEnable(BOOL bEnable) { (void)bEnable; }
HBRUSH CWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    (void)pWnd;
    return (HBRUSH)DefWindowProc(WM_CTLCOLOR + nCtlColor, (WPARAM)(pDC ? pDC->GetSafeHdc() : NULL), 0);
}
void CWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) { (void)pPopupMenu; (void)nIndex; (void)bSysMenu; }
LRESULT CWnd::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{ return DefWindowProc(WM_MENUCHAR, MAKELPARAM(nChar, nFlags), (LPARAM)(pMenu ? pMenu->GetSafeHmenu() : NULL)); }
void CWnd::OnSysCommand(UINT nID, LPARAM lParam) { DefWindowProc(WM_SYSCOMMAND, nID, lParam); }
void CWnd::OnContextMenu(CWnd* pWnd, CPoint point) { (void)pWnd; (void)point; }
void CWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) { (void)nChar; (void)nRepCnt; (void)nFlags; }
void CWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) { (void)nChar; (void)nRepCnt; (void)nFlags; }
void CWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) { (void)nChar; (void)nRepCnt; (void)nFlags; }
void CWnd::OnHScroll(UINT nSBCode, UINT nPos, CWnd* pScrollBar) { (void)nSBCode; (void)nPos; (void)pScrollBar; }
void CWnd::OnVScroll(UINT nSBCode, UINT nPos, CWnd* pScrollBar) { (void)nSBCode; (void)nPos; (void)pScrollBar; }
void CWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) { (void)nState; (void)pWndOther; (void)bMinimized; }
void CWnd::OnActivateApp(BOOL bActive, DWORD dwThreadID) { (void)bActive; (void)dwThreadID; }
void CWnd::OnCancelMode() {}
void CWnd::OnCaptureChanged(CWnd* pWnd) { (void)pWnd; }
void CWnd::OnDropFiles(HDROP dropInfo) { (void)dropInfo; }
void CWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszNewSection) { (void)uFlags; (void)lpszNewSection; }
void CWnd::OnDevModeChange(LPCTSTR lpDeviceName) { (void)lpDeviceName; }
void CWnd::OnDisplayChange(UINT bitsPerPixel, CSize sizeScreen) { (void)bitsPerPixel; (void)sizeScreen; }
void CWnd::OnStyleChanged(UINT nStyleType, LPSTYLESTRUCT lpStyleStruct) { (void)nStyleType; (void)lpStyleStruct; }
BOOL CWnd::OnNcActivate(BOOL bActive) { return (BOOL)DefWindowProc(WM_NCACTIVATE, bActive, 0); }
LRESULT CWnd::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam) { return DefWindowProc(WM_NCCALCSIZE, bCalcValidRects, lParam); }
void CWnd::OnNcPaint() {}
void CWnd::OnNcLButtonDown(UINT nHitTest, CPoint point) { (void)nHitTest; (void)point; }
void CWnd::OnNcLButtonUp(UINT nHitTest, CPoint point) { (void)nHitTest; (void)point; }
void CWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint point) { (void)nHitTest; (void)point; }
void CWnd::OnNcMouseMove(UINT nHitTest, CPoint point) { (void)nHitTest; (void)point; }
void CWnd::OnEnterSizeMove() {}
void CWnd::OnExitSizeMove() {}
int CWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{ (void)pDesktopWnd; (void)nHitTest; (void)message; return MA_ACTIVATE; }
BOOL CWnd::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) { (void)pWnd; (void)pCopyDataStruct; return FALSE; }
void CWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) { (void)nIDCtl; (void)lpMeasureItemStruct; }
void CWnd::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) { (void)nIDCtl; (void)lpDrawItemStruct; }
BOOL CWnd::OnHelpInfo(HELPINFO* lpHelpInfo) { (void)lpHelpInfo; return FALSE; }
UINT CWnd::OnGetDlgCode() { return 0; }
HCURSOR CWnd::OnQueryDragIcon() { return NULL; }
BOOL CWnd::OnQueryNewPalette() { return FALSE; }
void CWnd::OnFontChange() {}
void CWnd::OnSysColorChange() {}
void CWnd::OnThemeChanged() {}
void CWnd::OnEndSession(BOOL bEnding) { (void)bEnding; }
void CWnd::OnCompacting(UINT nRatio) { (void)nRatio; }
void CWnd::OnPower(int powerCode) { (void)powerCode; }
void CWnd::OnTimeChange(UINT wParam, LONG lParam) { (void)wParam; (void)lParam; }
void CWnd::OnInputLangChange(UINT nFlags, void* hkl) { (void)nFlags; (void)hkl; }
void CWnd::OnChildActivate() {}
void CWnd::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) { (void)nItemID; (void)nFlags; (void)hSysMenu; }
LRESULT CWnd::OnAppCommand(WPARAM wParam, LPARAM lParam) { (void)wParam; (void)lParam; return 0; }
int CWnd::OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex) { (void)nChar; (void)pListBox; (void)nIndex; return -1; }
int CWnd::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex) { (void)nKey; (void)pListBox; (void)nIndex; return -1; }
void CWnd::OnParentNotify(WPARAM wParam, LPARAM lParam) { (void)wParam; (void)lParam; }
BOOL CWnd::OnIconEraseBkgnd(CDC* pDC) { (void)pDC; return FALSE; }
void CWnd::OnXButtonDown(UINT nFlags, UINT nButton, CPoint point) { (void)nFlags; (void)nButton; (void)point; }
void CWnd::OnXButtonUp(UINT nFlags, UINT nButton, CPoint point) { (void)nFlags; (void)nButton; (void)point; }
void CWnd::OnXButtonDblClk(UINT nFlags, UINT nButton, CPoint point) { (void)nFlags; (void)nButton; (void)point; }

// ---------------------------------------------------------------------
//  AfxDelayDelete / misc globals
// ---------------------------------------------------------------------
void AFXAPI AfxDelayDelete(CWnd* pWnd)
{
    if (pWnd != NULL && pWnd->GetSafeHwnd() != NULL)
        ::PostMessage(pWnd->GetSafeHwnd(), WM_CLOSE, 0, 0);
}

CWnd* AFXAPI AfxGetMainWnd()
{
    CWinApp* pApp = AfxGetApp();
    if (pApp != NULL && pApp->m_pMainWnd != NULL) return pApp->m_pMainWnd;
    return CWnd::FromHandle(AfxGetMainHwnd());
}

CFrameWnd* AFXAPI AfxGetFrameWnd(CWnd* pWnd)
{
    while (pWnd != NULL) {
        if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) return (CFrameWnd*)pWnd;
        pWnd = pWnd->GetParent();
    }
    return NULL;
}

CWnd* AFXAPI AfxGetParentFrame(CWnd* pWnd)
{
    while (pWnd != NULL) {
        pWnd = pWnd->GetParent();
        if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) return pWnd;
    }
    return NULL;
}

HINSTANCE AFXAPI AfxFindResourceHandle(LPCTSTR lpszName, LPCTSTR lpszType)
{
    if (::FindResource(AfxGetResourceHandle(), lpszName, lpszType) != NULL)
        return AfxGetResourceHandle();
    return AfxGetInstanceHandle();
}

int AFXAPI AfxLoadString(UINT nID, LPTSTR lpszBuf)
{
    return ::LoadString(AfxGetResourceHandle(), nID, lpszBuf, 256);
}

CWinThread* AFXAPI AfxGetStaticModuleState() { return AfxGetThread(); }

// ---------------------------------------------------------------------
//  CWaitCursor
// ---------------------------------------------------------------------
CWaitCursor::CWaitCursor()
{
    CWinApp* pApp = AfxGetApp();
    m_hSaveCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
    (void)pApp;
}
CWaitCursor::~CWaitCursor() { Restore(); }
void CWaitCursor::Restore() { if (m_hSaveCursor != NULL) ::SetCursor(m_hSaveCursor); m_hSaveCursor = NULL; }

void AFXAPI AfxSleep(unsigned int nMillis) { ::Sleep(nMillis); }
