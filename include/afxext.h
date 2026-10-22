// =====================================================================
//  afxext.h  --  CControlBar derived bars for the MFC-GCC clone:
//                CStatusBar, CToolBar, CDialogBar, CReBar
// =====================================================================
#ifndef __AFXEXT_H__
#define __AFXEXT_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif
#include "afxwnd.h"
#include "afxcmn.h"
#include "afxfrm.h"
#include "afxres.h"

// status bar pane styles (MFC compatible)
#define SBPS_NORMAL     0x0000
#define SBPS_NOBORDERS  SBT_NOBORDERS
#define SBPS_POPOUT     SBT_POPOUT
#define SBPS_OWNERDRAW  SBT_OWNERDRAW
#define SBPS_DISABLED   0x04000000
#define SBPS_STRETCH    0x08000000

// toolbar button styles (MFC compatible)
#define TBBS_BUTTON     MAKELONG(TBSTYLE_BUTTON, 0)
#define TBBS_SEPARATOR  MAKELONG(TBSTYLE_SEP, 0)
#define TBBS_CHECKBOX   MAKELONG(TBSTYLE_CHECK, 0)
#define TBBS_GROUP      MAKELONG(TBSTYLE_GROUP, 0)
#define TBBS_CHECKGROUP (TBBS_GROUP | TBBS_CHECKBOX)
#define TBBS_DROPDOWN   MAKELONG(TBSTYLE_DROPDOWN, 0)
#define TBBS_AUTOSIZE   MAKELONG(TBSTYLE_AUTOSIZE, 0)
#define TBBS_NOPREFIX   MAKELONG(TBSTYLE_NOPREFIX, 0)
#define TBBS_CHECKED    MAKELONG(0, TBSTATE_CHECKED)
#define TBBS_PRESSED    MAKELONG(0, TBSTATE_PRESSED)
#define TBBS_DISABLED   MAKELONG(0, TBSTATE_ENABLED)
#define TBBS_INDETERMINATE MAKELONG(0, TBSTATE_INDETERMINATE)
#define TBBS_HIDDEN     MAKELONG(0, TBSTATE_HIDDEN)
#define TBBS_WRAPPED    MAKELONG(0, TBSTATE_WRAP)
#define TBBS_ELLIPSES   MAKELONG(0, TBSTATE_ELIPSES)
#define TBBS_MARKED     MAKELONG(0, TBSTATE_MARKED)

#ifndef ID_SEPARATOR
#define ID_SEPARATOR    0xFFFF
#endif

// ---------------------------------------------------------------------
//  CStatusBar
// ---------------------------------------------------------------------
class CStatusBar : public CControlBar
{
    DECLARE_DYNAMIC(CStatusBar)
public:
    CStatusBar();
    virtual ~CStatusBar();

    BOOL Create(CWnd* pParentWnd,
                DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
                UINT nID = AFX_IDW_STATUS_BAR);
    BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = 0,
                  DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
                  UINT nID = AFX_IDW_STATUS_BAR);

    BOOL SetIndicators(const UINT* lpIDArray, int nIDCount);
    int  CommandToIndex(UINT nID) const;
    UINT GetItemID(int nIndex) const;
    void GetItemRect(int nIndex, LPRECT lpRect) const;

    void SetPaneInfo(int nIndex, UINT nID, UINT nStyle, int cxWidth);
    void GetPaneInfo(int nIndex, UINT& nID, UINT& nStyle, int& cxWidth) const;
    UINT GetPaneStyle(int nIndex) const;
    void SetPaneStyle(int nIndex, UINT nStyle);
    BOOL SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE);
    CString GetPaneText(int nIndex) const;
    int  GetPaneCount() const { return (int)m_idArray.size(); }
    BOOL SetPaneWidth(int nIndex, int nWidth);
    void SetPaneIcon(int nIndex, HICON hIcon, BOOL bUpdate = TRUE);

    CStatusBarCtrl& GetStatusBarCtrl() const { return m_ctrl; }
    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);

    afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnSetMinHeight(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDestroyBar();
    DECLARE_MESSAGE_MAP()

protected:
    mutable CStatusBarCtrl m_ctrl;
    std::vector<UINT> m_idArray;
    std::vector<UINT> m_styleArray;
    std::vector<int>  m_widthArray;
    std::vector<CString> m_textArray;
    int m_cyHeight;
    int m_nLastWidth;
    int m_nLastPartsWidth;

    void UpdateAllPanes(BOOL bUpdateRects, BOOL bUpdateText);
    void RecalcParts(int cxTotal);
};

// ---------------------------------------------------------------------
//  CToolBar
// ---------------------------------------------------------------------
class CToolBar : public CControlBar
{
    DECLARE_DYNAMIC(CToolBar)
public:
    CToolBar();
    virtual ~CToolBar();

    BOOL Create(CWnd* pParentWnd,
                DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
                UINT nID = AFX_IDW_TOOLBAR);
    BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
                  DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
                  CRect rcBorders = CRect(0, 0, 0, 0),
                  UINT nID = AFX_IDW_TOOLBAR);

    BOOL LoadToolBar(LPCTSTR lpszResourceName);
    BOOL LoadToolBar(UINT nIDResource);
    // loads the standard comctl32 colour toolbar images and maps every button
    // id to its matching STD_* image -- handy when you have no bitmap resource
    BOOL LoadStdImages();
    BOOL LoadBitmap(LPCTSTR lpszResourceName);
    BOOL LoadBitmap(UINT nIDResource);
    BOOL SetButtons(const UINT* lpIDArray, int nIDCount);

    int  CommandToIndex(UINT nID) const;
    UINT GetItemID(int nIndex) const;
    UINT GetButtonStyle(int nIndex) const;
    void SetButtonStyle(int nIndex, UINT nStyle);
    void SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage);
    void GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const;
    void GetItemRect(int nIndex, LPRECT lpRect) const;
    int  GetButtonCount() const { return (int)m_idArray.size(); }
    void SetSizes(SIZE sizeButton, SIZE sizeImage);
    void SetHeight(int cyHeight);

    CToolBarCtrl& GetToolBarCtrl() const { return m_ctrl; }
    CImageList& GetImageList() { return m_imageList; }
    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);

    afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnToolTipTextW(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg BOOL OnToolTipTextA(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

protected:
    mutable CToolBarCtrl m_ctrl;
    CImageList      m_imageList;
    CImageList      m_imageListDisabled;
    std::vector<UINT> m_idArray;
    std::vector<UINT> m_styleArray;
    std::vector<int>  m_imageArray;
    int m_cxImage, m_cyImage;
    int m_cxButton, m_cyButton;
    DWORD m_dwCtrlStyle;
};

// ---------------------------------------------------------------------
//  CDialogBar -- a control bar built from a dialog template
// ---------------------------------------------------------------------
class CDialogBar : public CControlBar
{
    DECLARE_DYNAMIC(CDialogBar)
public:
    CDialogBar();
    virtual ~CDialogBar();

    BOOL Create(CWnd* pParentWnd, UINT nIDTemplate, DWORD dwStyle, UINT nID);
    BOOL Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName, DWORD dwStyle, UINT nID);
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
    BOOL UpdateData(BOOL bSaveAndValidate = TRUE);

    afx_msg LRESULT OnInitDialogBar(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

protected:
    HWND m_hDlgChild;
    CSize m_sizeDialog;
};

// ---------------------------------------------------------------------
//  CReBar -- rebar control bar
// ---------------------------------------------------------------------
class CReBar : public CControlBar
{
    DECLARE_DYNAMIC(CReBar)
public:
    CReBar();
    virtual ~CReBar();
    BOOL Create(CWnd* pParentWnd, DWORD dwCtrlStyle = RBS_BANDBORDERS,
                DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
                                WS_CLIPCHILDREN | CBRS_TOP,
                UINT nID = AFX_IDW_REBAR);
    CReBarCtrl& GetReBarCtrl() const { return m_rebar; }
    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
protected:
    mutable CReBarCtrl m_rebar;
};

#endif /* __AFXEXT_H__ */
