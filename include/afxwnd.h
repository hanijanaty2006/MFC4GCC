// =====================================================================
//  afxwnd.h  --  CCmdTarget, CWnd, CMenu, CCmdUI for the MFC-GCC clone.
// =====================================================================
#ifndef __AFXWND_H__
#define __AFXWND_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif
#include "afxmsg.h"
#include "afxgdi.h"

class CView;
class CDocument;
class CDocTemplate;
class CFrameWnd;
class CDataExchange;
class CControlBar;
struct AFX_CMDHANDLERINFO;

#define AFX_IDW_PANE_FIRST      0xE900
#define AFX_IDW_PANE_LAST       0xE9FF
#define AFX_IDW_PANE_SAVE       0xE9FF

#define AFX_WM_IDLEUPDATECMDUI  (WM_USER + 0x100)

// The pseudo message used for command-UI updates (matches MFC's WM_IDLEUPDATECMDUI)
#ifndef WM_IDLEUPDATECMDUI
#define WM_IDLEUPDATECMDUI 0x0363
#endif
#ifndef WM_SETMESSAGESTRING
#define WM_SETMESSAGESTRING 0x0362
#endif
#ifndef WM_POPMESSAGESTRING
#define WM_POPMESSAGESTRING 0x0361
#endif
#ifndef WM_KICKIDLE
#define WM_KICKIDLE 0x036A
#endif
#ifndef WM_INITIALUPDATE
#define WM_INITIALUPDATE 0x036B
#endif
#ifndef WM_ACTIVATEVIEW
#define WM_ACTIVATEVIEW 0x036F
#endif
#ifndef WM_SIZEPARENT
#define WM_SIZEPARENT 0x0360
#endif
#ifndef WM_FLOATSTATUS
#define WM_FLOATSTATUS 0x0365
#endif
#ifndef WM_RECALCPARENT
#define WM_RECALCPARENT 0x0364
#endif
#ifndef WM_ACTIVATETOPLEVEL
#define WM_ACTIVATETOPLEVEL 0x0366
#endif
#ifndef WM_QUERYCENTERWND
#define WM_QUERYCENTERWND 0x0367
#endif
#ifndef WM_DISABLEMODAL
#define WM_DISABLEMODAL 0x0368
#endif
#ifndef WM_EXITHELPMODE
#define WM_EXITHELPMODE 0x037E
#endif
#ifndef WM_HELPPROMPTADDR
#define WM_HELPPROMPTADDR 0x0369
#endif
#ifndef WM_COMMANDHELP
#define WM_COMMANDHELP 0x037C
#endif
#ifndef WM_HELPHITTEST
#define WM_HELPHITTEST 0x037D
#endif

extern const RECT AFXAPI rectDefault;

// ---------------------------------------------------------------------
//  CCmdUI -- command/menu user-interface update
// ---------------------------------------------------------------------
struct CCmdUI
{
    CCmdUI();

    UINT    m_nID;
    UINT    m_nIndex;         // menu item index (or button index)
    UINT    m_nIndexMax;      // last index of the containing menu
    CMenu*  m_pMenu;          // NULL if not a menu
    CMenu*  m_pSubMenu;       // submenu that contains this item
    DWORD   m_nOtherContext;  // user defined

    void Enable(BOOL bOn = TRUE);
    void SetCheck(int nCheck = 1);
    void SetRadio(BOOL bOn = TRUE);
    void SetText(LPCTSTR lpszText);

    BOOL DoUpdate(CCmdTarget* pTarget, BOOL bDisableIfNoHandler);

    // implementation
    CCmdTarget* m_pOther;
    BOOL m_bEnableChanged;
    BOOL m_bContinueRouting;
};

// ---------------------------------------------------------------------
//  CCreateContext -- doc/view creation plumbing
// ---------------------------------------------------------------------
struct CCreateContext
{
    CRuntimeClass*  m_pNewViewClass;    // runtime class of view to create (or NULL)
    CView*          m_pCurrentView;     // currently active view (or NULL)
    CDocument*      m_pCurrentDoc;      // currently active document (or NULL)
    CDocTemplate*   m_pNewDocTemplate;  // template used to create the doc/frame
    CObject*        m_pLastView;        // original view (for splitters)
    CObject*        m_pLastFrame;       // original frame

    CCreateContext();
    void RememberCurrentView();
    void RememberCurrentFrame();
    void RememberCurrentDocument();
    CView* GetLastView();
    CFrameWnd* GetLastFrame();
    CDocument* GetLastDocument();
};

// ---------------------------------------------------------------------
//  CCmdTarget -- base for everything that can handle commands
// ---------------------------------------------------------------------
class CCmdTarget : public CObject
{
    DECLARE_DYNAMIC(CCmdTarget)
    DECLARE_MESSAGE_MAP()
public:
    CCmdTarget();
    virtual ~CCmdTarget();

    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

    // helper used by message-map dispatch
    BOOL DispatchThroughMap(const AFX_MSGMAP_ENTRY* lpEntry, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

// ---------------------------------------------------------------------
//  CMenu
// ---------------------------------------------------------------------
class CMenu : public CObject
{
    DECLARE_DYNAMIC(CMenu)
public:
    CMenu();
    virtual ~CMenu();

    HMENU m_hMenu;

    operator HMENU() const { return m_hMenu; }
    HMENU GetSafeHmenu() const { return m_hMenu; }
    BOOL Attach(HMENU hMenu);
    HMENU Detach();
    static CMenu* PASCAL FromHandle(HMENU hMenu);

    BOOL CreateMenu();
    BOOL CreatePopupMenu();
    BOOL LoadMenu(LPCTSTR lpszResourceName);
    BOOL LoadMenu(UINT nIDResource);
    BOOL LoadMenuIndirect(const void* lpMenuTemplate);
    BOOL DestroyMenu();

    CMenu* GetSubMenu(int nPos) const;
    BOOL DeleteMenu(UINT nPosition, UINT nFlags);
    BOOL RemoveMenu(UINT nPosition, UINT nFlags);
    BOOL AppendMenu(UINT nFlags, UINT_PTR nIDNewItem = 0, LPCTSTR lpszNewItem = NULL);
    BOOL AppendMenu(UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp);
    BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem = 0, LPCTSTR lpszNewItem = NULL);
    BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp);
    BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem = 0, LPCTSTR lpszNewItem = NULL);
    BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp);

    UINT GetMenuItemCount() const;
    UINT GetMenuItemID(int nPos) const;
    UINT GetMenuState(UINT nID, UINT nFlags) const;
    int GetMenuString(UINT nID, LPTSTR lpString, int nMaxCount, UINT nFlags) const;
    CString GetMenuString(UINT nID, UINT nFlags) const;
    BOOL GetMenuInfo(LPMENUINFO lpMenuInfo) const;
    BOOL SetMenuInfo(LPCMENUINFO lpMenuInfo);
    BOOL GetMenuItemInfo(UINT nItem, LPMENUITEMINFO lpMenuItemInfo, BOOL fByPos = FALSE);
    BOOL SetMenuItemInfo(UINT nItem, LPMENUITEMINFO lpMenuItemInfo, BOOL fByPos = FALSE);

    UINT EnableMenuItem(UINT nIDEnableItem, UINT nEnable);
    UINT CheckMenuItem(UINT nIDCheckItem, UINT nCheck);
    UINT CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags);
    BOOL SetDefaultItem(UINT uItem, BOOL fByPos = FALSE);
    UINT GetDefaultItem(UINT gmdiFlags = GMDI_USEDISABLED, BOOL fByPos = FALSE);
    BOOL SetMenuDefaultItem(UINT uItem = (UINT)-1, BOOL fByPos = FALSE) { return SetDefaultItem(uItem, fByPos); }
    BOOL SetMenuItemBitmaps(UINT nPosition, UINT nFlags, const CBitmap* pBmpUnchecked, const CBitmap* pBmpChecked);
    BOOL TrackPopupMenu(UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT lpRect = NULL);
    BOOL DrawMenuBar();

    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

    void AttachToMenuBar(HWND hWnd);
    void DetachFromMenuBar();
    HWND m_hMenuOwner;
};

// ---------------------------------------------------------------------
//  CWnd -- the big one
// ---------------------------------------------------------------------
class CWnd : public CCmdTarget
{
    DECLARE_DYNAMIC(CWnd)
    DECLARE_MESSAGE_MAP()

// Construction
public:
    CWnd();
    virtual ~CWnd();

    static CWnd* PASCAL FromHandle(HWND hWnd);
    static CWnd* PASCAL FromHandlePermanent(HWND hWnd);
    static void PASCAL DeleteTempMap();
    static BOOL PASCAL DestroyWindow(HWND hWnd);

    BOOL Attach(HWND hWndNew);
    HWND Detach();
    BOOL SubclassWindow(HWND hWnd);
    HWND UnsubclassWindow();
    BOOL SubclassDlgItem(UINT nID, CWnd* pParent);

    HWND GetSafeHwnd() const { return m_hWnd; }
    operator HWND() const { return m_hWnd; }
    DWORD GetStyle() const { return (DWORD)::GetWindowLongPtr(m_hWnd, GWL_STYLE); }
    DWORD GetExStyle() const { return (DWORD)::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE); }
    // NOTE: no GetWindowLongPtr()/SetWindowLongPtr() members -- winuser.h
    //       defines them as function-like macros.  Use:
    //         ::GetWindowLongPtr(pWnd->GetSafeHwnd(), nIndex)
    BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);
    BOOL ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);

    virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
                          DWORD dwStyle, int x, int y, int nWidth, int nHeight,
                          HWND hWndParent, HMENU nIDorHMenu, LPVOID lpParam = NULL);
    virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
                          DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
                          UINT nID, LPVOID lpParam = NULL);
    virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                        const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
    BOOL CreateControl(LPCTSTR lpszClass, LPCTSTR lpszWindowName, DWORD dwStyle,
                       const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpCreateStruct = NULL);
    BOOL CreateControl(LPCTSTR lpszClass, LPCTSTR lpszWindowName, DWORD dwStyle,
                       int x, int y, int cx, int cy, CWnd* pParentWnd, UINT nID,
                       LPVOID lpCreateStruct = NULL);
    virtual BOOL DestroyWindow();

    static CWnd* PASCAL GetDesktopWindow() { return FromHandle(::GetDesktopWindow()); }
    static CWnd* PASCAL GetForegroundWindow() { return FromHandle(::GetForegroundWindow()); }
    static CWnd* PASCAL GetActiveWindow() { return FromHandle(::GetActiveWindow()); }
    static CWnd* PASCAL GetCapture() { return FromHandle(::GetCapture()); }
    static CWnd* PASCAL GetFocus() { return FromHandle(::GetFocus()); }
    static CWnd* PASCAL FindWindow(LPCTSTR lpszClassName, LPCTSTR lpszWindowName);
    static CWnd* PASCAL WindowFromPoint(POINT point);
    static CWnd* PASCAL ChildWindowFromPoint(HWND hWndParent, POINT point);
    CWnd* ChildWindowFromPoint(POINT point) const;
    CWnd* ChildWindowFromPoint(POINT point, UINT nFlags) const;

// Attributes
    CWnd* GetParent() const;
    CWnd* SetParent(CWnd* pWndNewParent);
    CWnd* GetParentOwner() const;
    CWnd* GetTopLevelParent() const;
    CWnd* GetTopLevelFrame() const;
    CWnd* GetSafeOwner(CWnd* pParent = NULL, HWND* pWndTop = NULL) const;
    CWnd* GetWindow(UINT nCmd) const;
    CWnd* GetFirstChildWindow() const;
    CWnd* GetLastChildWindow() const;
    CWnd* GetNextWindow(UINT nCmd = GW_HWNDNEXT) const;
    CWnd* GetDescendantWindow(int nID, BOOL bOnlyPerm = FALSE) const;
    CWnd* GetDlgItem(int nID) const;
    int GetDlgCtrlID() const;
    int SetDlgCtrlID(int nID);
    BOOL IsChild(const CWnd* pWnd) const;
    BOOL IsWindow() const { return ::IsWindow(m_hWnd) ? TRUE : FALSE; }
    static BOOL PASCAL IsWindow(HWND hWnd) { return ::IsWindow(hWnd); }
    BOOL IsWindowVisible() const;
    BOOL IsWindowEnabled() const;
    BOOL IsIconic() const;
    BOOL IsZoomed() const;
    BOOL EnableWindow(BOOL bEnable = TRUE);
    BOOL IsDialogWindow() const;

    void GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const;
    void GetWindowText(CString& rString) const;
    int GetWindowTextLength() const;
    void SetWindowText(LPCTSTR lpszString);
    CString GetWindowTextString() const;

    void GetClientRect(LPRECT lpRect) const;
    void GetWindowRect(LPRECT lpRect) const;
    CRect GetClientRect() const;
    CRect GetWindowRect() const;

    void ScreenToClient(LPPOINT lpPoint) const;
    void ScreenToClient(LPRECT lpRect) const;
    void ClientToScreen(LPPOINT lpPoint) const;
    void ClientToScreen(LPRECT lpRect) const;
    void ScreenToClient(CPoint& point) const { ScreenToClient((LPPOINT)&point); }
    void ClientToScreen(CPoint& point) const { ClientToScreen((LPPOINT)&point); }
    CPoint ScreenToClient(CPoint point) const { ScreenToClient((LPPOINT)&point); return point; }
    CPoint ClientToScreen(CPoint point) const { ClientToScreen((LPPOINT)&point); return point; }
    CRect ScreenToClient(const CRect& rect) const { CRect r(rect); ScreenToClient((LPRECT)&r); return r; }
    CRect ClientToScreen(const CRect& rect) const { CRect r(rect); ClientToScreen((LPRECT)&r); return r; }
    int MapWindowPoints(CWnd* pWndTo, LPPOINT lpPoint, UINT nCount) const;
    int MapWindowPoints(CWnd* pWndTo, LPRECT lpRect) const;

    BOOL CenterWindow(CWnd* pAlternateOwner = NULL);

// Window state / geometry
    int ShowWindow(int nCmdShow);
    BOOL ShowWindowAsync(int nCmdShow);
    BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags);
    UINT ArrangeIconicWindows();
    void BringWindowToTop();
    BOOL MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
    BOOL MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);
    BOOL SetForegroundWindow();
    HWND SetActiveWindow();
    CWnd* SetFocus();
    BOOL LockWindowUpdate();
    static void UnlockWindowUpdate();
    BOOL RedrawWindow(LPCRECT lpRectUpdate = NULL, CRgn* prgnUpdate = NULL,
                      UINT nFlags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
    BOOL EnableScrollBar(int nSBFlags, UINT nArrowFlags = ESB_ENABLE_BOTH);
    UINT SetTimer(UINT_PTR nIDEvent, UINT nElapse,
                  void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD) = NULL);
    BOOL KillTimer(UINT_PTR nIDEvent);
    HDC GetDC() const;
    HDC GetWindowDC() const;
    int ReleaseDC(HDC hDC) const;
    CDC* GetDCEx(CRgn* prgnClip, DWORD flags);
    void Print(CDC* pDC, DWORD dwFlags) const;
    void PrintClient(CDC* pDC, DWORD dwFlags) const;
    void UpdateWindow();
    void SetRedraw(BOOL bRedraw = TRUE);
    BOOL GetUpdateRect(LPRECT lpRect, BOOL bErase = FALSE);
    int GetUpdateRgn(CRgn* pRgn, BOOL bErase = FALSE);
    void Invalidate(BOOL bErase = TRUE);
    void InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);
    void InvalidateRgn(CRgn* pRgn, BOOL bErase = TRUE);
    void ValidateRect(LPCRECT lpRect);
    void ValidateRgn(CRgn* pRgn);
    BOOL ShowScrollBar(int nBar, BOOL bShow = TRUE);
    void ScrollWindow(int xAmount, int yAmount, LPCRECT lpRect = NULL, LPCRECT lpClipRect = NULL);
    int ScrollWindowEx(int dx, int dy, LPCRECT lprcScroll, LPCRECT lprcClip,
                       CRgn* prgnUpdate, LPRECT lprcUpdate, UINT flags);
    BOOL FlashWindow(BOOL bInvert);
    BOOL SetWindowPlacement(const WINDOWPLACEMENT* lpwndpl);
    BOOL GetWindowPlacement(WINDOWPLACEMENT* lpwndpl) const;
    void SetIcon(HICON hIcon, BOOL bBigIcon);
    HICON GetIcon(BOOL bBigIcon) const;
    BOOL SetProp(LPCTSTR lpString, HANDLE hData);
    HANDLE GetProp(LPCTSTR lpString) const;
    HANDLE RemoveProp(LPCTSTR lpString);
    void DragAcceptFiles(BOOL bAccept = TRUE);
    CWnd* SetCapture();
    BOOL ReleaseCapture();

    BOOL OpenIcon();
    BOOL CloseWindow();

// Menus
    CMenu* GetMenu() const;
    BOOL SetMenu(CMenu* pMenu);
    CMenu* GetSystemMenu(BOOL bRevert = FALSE);
    BOOL DrawMenuBar();
    BOOL HiliteMenuItem(UINT nIDHiliteItem, UINT nHilite);

// Dialog data (works on dialogs, form views and any parent window)
    virtual void DoDataExchange(CDataExchange* pDX);
    BOOL UpdateData(BOOL bSaveAndValidate = TRUE);
    UINT GetDlgItemInt(int nID, BOOL* lpTrans = NULL, BOOL bSigned = TRUE) const;
    void SetDlgItemInt(int nID, UINT nValue, BOOL bSigned = TRUE);
    int GetDlgItemText(int nID, LPTSTR lpStr, int nMaxCount) const;
    void SetDlgItemText(int nID, LPCTSTR lpszString);
    void CheckDlgButton(int nIDButton, UINT nCheck);
    void CheckRadioButton(int nIDFirstButton, int nIDLastButton, int nIDCheckButton);
    UINT IsDlgButtonChecked(int nIDButton) const;
    LRESULT SendDlgItemMessage(int nID, UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
    CWnd* GetNextDlgGroupItem(CWnd* pWndCtl, BOOL bPrevious = FALSE) const;
    CWnd* GetNextDlgTabItem(CWnd* pWndCtl, BOOL bPrevious = FALSE) const;
    void NextDlgCtrl() const;
    void PrevDlgCtrl() const;
    void GotoDlgCtrl(CWnd* pWndCtl);

// Messaging
    LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const;
    BOOL PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const;
    BOOL SendNotifyMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
    BOOL SendChildNotifyLastMsg(LRESULT* pResult = NULL);
    LRESULT DefWindowProc();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Default();
    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void PreSubclassWindow();
    virtual void PostNcDestroy();
    virtual LRESULT OnToolHitTest(CPoint point, TOOLINFO* pInfo) const;
    virtual BOOL ContinueModal() { return TRUE; }
    virtual void EndModalLoop(int nResult);
    void SetFocusToThis() { ::SetFocus(m_hWnd); }

    // helpers used by the framework
    BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    static LRESULT PASCAL SendChildNotifyLastMsg(HWND hWnd, LRESULT* pResult = NULL);

// Implementation
public:
    HWND m_hWnd;          // main window handle
    HWND m_hWndOwner;     // owner (for subclassed windows)
protected:
    WNDPROC m_pfnSuper;   // original window proc when subclassing
    UINT    m_nMsgLast;
    WPARAM  m_wpLast;
    LPARAM  m_lpLast;
    UINT    m_nFlags;     // internal state flags
    BOOL    m_bDestroying;

public:
    // the window procedure shared by every window this framework creates
    friend LRESULT CALLBACK AfxWndProc(HWND, UINT, WPARAM, LPARAM);

    // framework housekeeping messages
    afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnPopMessageString(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnDestroy();
    afx_msg void OnNcDestroy();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnMouseHover(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
    afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnEnable(BOOL bEnable);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CWnd* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CWnd* pScrollBar);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
    afx_msg void OnCancelMode();
    afx_msg void OnCaptureChanged(CWnd* pWnd);
    afx_msg void OnDropFiles(HDROP dropInfo);
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszNewSection);
    afx_msg void OnDevModeChange(LPCTSTR lpDeviceName);
    afx_msg void OnDisplayChange(UINT bitsPerPixel, CSize sizeScreen);
    afx_msg void OnStyleChanged(UINT nStyleType, LPSTYLESTRUCT lpStyleStruct);
    afx_msg BOOL OnNcActivate(BOOL bActive);
    afx_msg LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam);
    afx_msg void OnNcPaint();
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
    afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
    afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
    afx_msg void OnEnterSizeMove();
    afx_msg void OnExitSizeMove();
    afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
    afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg BOOL OnHelpInfo(HELPINFO* lpHelpInfo);
    afx_msg UINT OnGetDlgCode();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg BOOL OnQueryNewPalette();
    afx_msg void OnFontChange();
    afx_msg void OnSysColorChange();
    afx_msg void OnThemeChanged();
    afx_msg void OnEndSession(BOOL bEnding);
    afx_msg void OnCompacting(UINT nRatio);
    afx_msg void OnPower(int powerCode);
    afx_msg void OnTimeChange(UINT wParam, LONG lParam);
    afx_msg void OnInputLangChange(UINT nFlags, void* hkl);
    afx_msg void OnChildActivate();
    afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
    afx_msg LRESULT OnAppCommand(WPARAM wParam, LPARAM lParam);
    afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
    afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
    afx_msg void OnParentNotify(WPARAM wParam, LPARAM lParam);
    afx_msg BOOL OnIconEraseBkgnd(CDC* pDC);
    afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
    afx_msg void OnXButtonUp(UINT nFlags, UINT nButton, CPoint point);
    afx_msg void OnXButtonDblClk(UINT nFlags, UINT nButton, CPoint point);
};

// window class registration helpers (MFC compatible)
LPCTSTR AFXAPI AfxRegisterWndClass(UINT nClassStyle, HCURSOR hCursor = NULL,
                                   HBRUSH hbrBackground = NULL, HICON hIcon = NULL);
BOOL AFXAPI AfxRegisterClass(const WNDCLASS* lpWndClass);
BOOL AFXAPI AfxDeferRegisterClass(DWORD fToRegister);

#define AFX_WNDCLASS(cs)                (0x00000001 | ((cs) << 16))
#define AFX_WNDREG()                    AFX_WNDCLASS(0)
#define AFX_WNDOLECONTROL_REG           0x00000002
#define AFX_WNDOLECONTROL2_REG          0x00000004
#define AFX_WNDOLECONTROL3_REG          0x00000008
#define AFX_WNDOLECTRLBAR_REG           0x00000010
#define AFX_WNDCOMBOBOX_REG             0x00000020
#define AFX_WNDCOMBOT_REG               0x00000040
#define AFX_WNDCOMBOCBT_REG             0x00000080

// internal HWND <-> CWnd bookkeeping (implemented in mfc.cpp)
void  AFXAPI AfxRegisterWnd(HWND hWnd, CWnd* pWnd);
void  AFXAPI AfxUnregisterWnd(HWND hWnd);
CWnd* AFXAPI AfxLookupWnd(HWND hWnd);
CWnd* AFXAPI AfxLookupWndPerm(HWND hWnd);
void  AFXAPI AfxAddTempWnd(CWnd* pWnd);
void  AFXAPI AfxRemoveTempWnd(HWND hWnd);
CWnd* AFXAPI AfxGetCreatingWnd();
void  AFXAPI AfxSetCreatingWnd(CWnd* pWnd);

LRESULT CALLBACK AfxWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

#endif /* __AFXWND_H__ */
