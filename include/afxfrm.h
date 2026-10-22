// =====================================================================
//  afxfrm.h  --  Frame windows and control bars for the MFC-GCC clone:
//                CControlBar, CFrameWnd, CMDIFrameWnd, CMDIChildWnd
// =====================================================================
#ifndef __AFXFRM_H__
#define __AFXFRM_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif
#include "afxwnd.h"
#include "afxdoc.h"
#include "afxres.h"

// --- frame window styles (MFC compatible) ----------------------------
#define FWS_SNAPTOBARS      0x00001000L
#define FWS_PREFIXTITLE     0x00004000L
#define FWS_ADDTOTITLE      0x00008000L

// --- control bar styles ---------------------------------------------
#define CBRS_ALIGN_LEFT     0x4000L
#define CBRS_ALIGN_TOP      0x1000L
#define CBRS_ALIGN_RIGHT    0x8000L
#define CBRS_ALIGN_BOTTOM   0x2000L
#define CBRS_ALIGN_ANY      0xF000L
#define CBRS_BORDER_LEFT    0x0100L
#define CBRS_BORDER_TOP     0x0200L
#define CBRS_BORDER_RIGHT   0x0400L
#define CBRS_BORDER_BOTTOM  0x0800L
#define CBRS_BORDER_ANY     0x0F00L
#define CBRS_BORDER_3D      0x0080L
#define CBRS_TOOLTIPS       0x0010L
#define CBRS_FLYBY          0x0020L
#define CBRS_SIZE_DYNAMIC   0x0040L
#define CBRS_SIZE_FIXED     0x0004L
#define CBRS_GRIPPER        0x0008L
#define CBRS_HIDE_INPLACE   0x0002L

// id range handled by CFrameWnd::OnBarCheck (View menu entries)
#define ID_VIEW_MIN         AFX_IDW_TOOLBAR
#define ID_VIEW_MAX         0xE8FF

#define CBRS_TOP            CBRS_ALIGN_TOP
#define CBRS_BOTTOM         CBRS_ALIGN_BOTTOM
#define CBRS_LEFT           CBRS_ALIGN_LEFT
#define CBRS_RIGHT          CBRS_ALIGN_RIGHT

// layout modes for CalcDynamicLayout
#define LM_HORZ             0
#define LM_VERT             1
#define LM_HORZDOCK         2
#define LM_VERTDOCK         3
#define LM_STRETCH          4
#define LM_COMMIT           5
#define LM_MRUWIDTH         0x1000
#define LM_LENGTHY          0x2000
#define LM_COMMITTED        0x4000


// ---------------------------------------------------------------------
//  CControlBar
// ---------------------------------------------------------------------
class CControlBar : public CWnd
{
    DECLARE_DYNAMIC(CControlBar)
public:
    CControlBar();
    virtual ~CControlBar();

    BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                const RECT& rect, CWnd* pParentWnd, UINT nID);
    BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, LPCTSTR lpszClassName,
                  LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, UINT nID);
    BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = 0,
                  DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
                  const RECT& rect = rectDefault, UINT nID = 0xFFFF);

    DWORD GetBarStyle();
    void SetBarStyle(DWORD dwStyle);
    BOOL IsVisible() const;
    BOOL IsFloating() const { return FALSE; }
    CFrameWnd* GetDockingFrame() const;
    void SetBorders(int left, int top, int right, int bottom);
    void SetBorders(LPCRECT lpBorders);
    void GetBorders(LPRECT lpBorders) const;
    int GetCount() const { return m_nCount; }

    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
    virtual CSize CalcLayout(DWORD dwMode, int nLength = -1);
    virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
    void SizeChildControl(int cx, int cy);
    void EraseNonClient();

    // simple "docking": where the frame puts this bar
    enum BarPosition { barTop, barBottom, barLeft, barRight };
    BarPosition GetBarPosition() const { return m_barPos; }
    void SetBarPosition(BarPosition pos) { m_barPos = pos; }

    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
    DECLARE_MESSAGE_MAP()

public:
    int m_cxLeft, m_cyTop, m_cxRight, m_cyBottom;
    int m_nCount;

protected:
    DWORD        m_dwStyle;
    BarPosition  m_barPos;
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

// ---------------------------------------------------------------------
//  CFrameWnd -- main/child frame window with doc/view + control bars
// ---------------------------------------------------------------------
class CFrameWnd : public CWnd
{
    DECLARE_DYNAMIC(CFrameWnd)
public:
    CFrameWnd();
    virtual ~CFrameWnd();

    // creation
    BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
                DWORD dwStyle = WS_OVERLAPPEDWINDOW,
                const RECT& rect = rectDefault,
                CWnd* pParentWnd = NULL, LPCTSTR lpszMenuName = NULL,
                DWORD dwExStyle = 0, CCreateContext* pContext = NULL);
    BOOL LoadFrame(UINT nIDResource,
                   DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
                   CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
    virtual BOOL CreateView(CCreateContext* pContext, UINT nID = AFX_IDW_PANE_FIRST);
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL DestroyWindow();
    virtual void PostNcDestroy();

    // view / document management
    CView* GetActiveView() const { return m_pViewActive; }
    void SetActiveView(CView* pViewNew, BOOL bNotify = TRUE);
    virtual void ActivateView(CView* pViewNew, BOOL bAdd = FALSE);
    CDocument* GetActiveDocument();
    void InitialUpdateFrame(CDocument* pDoc, BOOL bMakeVisible = TRUE);
    void UpdateFrameTitle(BOOL bAddToTitle);
    virtual void OnSetPreviewMode(BOOL bPreview, void* pState) { (void)bPreview; (void)pState; }

    // layout
    virtual void ActivateFrame(int nCmdShow = -1);
    CFrameWnd* GetParentFrame() const;
    virtual void RecalcLayout(BOOL bNotify = TRUE);
    virtual void DelayRecalcLayout(BOOL bNotify = TRUE);
    void AddControlBar(CControlBar* pBar, int nPos = -1);
    void RemoveControlBar(CControlBar* pBar);
    CControlBar* GetControlBar(UINT nID);
    void ShowControlBar(CControlBar* pBar, BOOL bShow, BOOL bDelay);
    int GetControlBarCount() const { return (int)m_listBars.size(); }

    // titles and messages
    void SetTitle(LPCTSTR lpszTitle);
    CString GetTitleString() const;
    void SetMessageText(LPCTSTR lpszText);
    void SetMessageText(UINT nID);
    virtual void GetMessageString(UINT nID, CString& rMessage) const;
    void EnableModeless(BOOL bEnable);
    void ShowOwnedPopups(BOOL bShow = TRUE);

    // accelerators
    BOOL LoadAccelTable(LPCTSTR lpszResourceName);
    BOOL LoadAccelTable(UINT nIDResource) { return LoadAccelTable(MAKEINTRESOURCE(nIDResource)); }
    void SetAcceleratorTable(HACCEL hAccel);
    HACCEL GetAcceleratorTable() const { return m_hAccelTable; }
    BOOL TranslateAcceleratorMessage(MSG* pMsg);

    // document helpers
    BOOL SaveAllModified();
    void CloseAllDocuments(BOOL bEndSession);

    // command routing
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    // handlers
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnPopMessageString(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
    afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateControlBarMenu(CCmdUI* pCmdUI);
    afx_msg void OnBarCheck(UINT nID);
    afx_msg void OnDestroy();

    DECLARE_MESSAGE_MAP()

    HACCEL m_hAccelTable;
    BOOL     m_bAutoMenuEnable;

protected:
    CView*   m_pViewActive;
    std::vector<CControlBar*> m_listBars;
    BOOL     m_bInRecalcLayout;
    CString  m_strTitle;
    CString  m_strMessage;
    int      m_nLastClientCx, m_nLastClientCy;
    CCreateContext m_context;
    UINT     m_nIDTracking;
    UINT     m_nIDLastMessage;

    void LayoutBars(int cxClient, int cyClient);
    void UpdateMenuItems(HMENU hMenu);
};

// ---------------------------------------------------------------------
//  CMDIFrameWnd / CMDIChildWnd
// ---------------------------------------------------------------------
class CMDIChildWnd;

class CMDIFrameWnd : public CFrameWnd
{
    DECLARE_DYNAMIC(CMDIFrameWnd)
public:
    CMDIFrameWnd();
    virtual ~CMDIFrameWnd();

    BOOL CreateClient(LPCREATESTRUCT lpCreateStruct, CMenu* pWindowMenu);
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpCreateStruct, CCreateContext* pContext);
    CMDIChildWnd* MDIGetActive(BOOL* pbMaximized = NULL) const;
    void MDIActivate(CWnd* pWndActivate);
    void MDIAddMenu(CMenu* pMenu, LPCTSTR lpszWindowMenuName = _T("&Window"));
    void MDISetMenu(CMenu* pFrameMenu, CMenu* pWindowMenu);
    void MDIRefreshMenu();
    void MDITile(int nType = MDITILE_HORIZONTAL);
    void MDICascade(int nType = 0);
    void MDIIconArrange();
    void MDIMaximize(CWnd* pWnd);
    void MDIRestore(CWnd* pWnd);
    void MDINext(CWnd* pWndCur = NULL, BOOL bPrevious = FALSE);
    CMDIChildWnd* CreateNewChild(CRuntimeClass* pClass, UINT nResource,
                                 CDocument* pDoc = NULL, CCreateContext* pContext = NULL);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    HWND GetMDIClientHandle() const { return m_hWndMDIClient; }
    int GetMDIChildCount() const;

    afx_msg void OnWindowNew();
    afx_msg void OnMDIWindowCmd(UINT nID);
    afx_msg void OnUpdateMDIWindowCmd(CCmdUI* pCmdUI);
    afx_msg void OnWindowTileHorz();
    afx_msg void OnWindowTileVert();
    afx_msg void OnWindowCascade();
    afx_msg void OnWindowArrangeIcons();
    afx_msg void OnUpdateWindowMenu(CCmdUI* pCmdUI);
    afx_msg LRESULT OnCommandHelpMDI(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

protected:
    HWND m_hWndMDIClient;
    void UpdateClientWindowMenu();
};

class CMDIChildWnd : public CFrameWnd
{
    DECLARE_DYNAMIC(CMDIChildWnd)
public:
    CMDIChildWnd();
    virtual ~CMDIChildWnd();

    BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
                DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                const RECT& rect = rectDefault,
                CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
    BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                   CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
    void ActivateFrame(int nCmdShow = -1);
    CMDIFrameWnd* GetMDIFrame() const;
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual BOOL DestroyWindow();
    virtual void OnSetFocus(CWnd* pOldWnd);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
    afx_msg int OnCreateMDI(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnWindowActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg LRESULT OnCommandHelpMDI(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSysCommandMDI(UINT nID, LPARAM lParam);
    afx_msg LRESULT OnMDISetMenu(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

protected:
    BOOL m_bPseudoInactive;
};

#endif /* __AFXFRM_H__ */
