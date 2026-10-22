// =====================================================================
//  afxcmn.h  --  Common control wrappers for the MFC-GCC clone.
//                Implemented inline on top of the comctl32 messages, so
//                no extra object files are needed for these classes.
//
//                comctl32 v6 (visual styles) is activated by the
//                framework itself, see AfxEnableVisualStyles().
// =====================================================================
#ifndef __AFXCMN_H__
#define __AFXCMN_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif
#include "afxwnd.h"
#include <richedit.h>

class CDC;
class CFont;
class CBitmap;
class CToolTipCtrl;
class CListCtrl;
class CTreeCtrl;

#define AFX_CMN_INLINE AFX_INLINE

// ---------------------------------------------------------------------
//  Constants that some MinGW-w64 commctrl/winuser headers do not define
// ---------------------------------------------------------------------
#ifndef EM_GETCARETINDEX
#define EM_GETCARETINDEX        0x00DF
#endif
#ifndef EM_SETCUEBANNER
#define EM_SETCUEBANNER         (ECM_FIRST + 1)
#endif
#ifndef LVM_SETORIGIN
#define LVM_SETORIGIN           (LVM_FIRST + 43)
#endif
#ifndef LVM_GETSELECTEDCOUNT
#define LVM_GETSELECTEDCOUNT    (LVM_FIRST + 50)
#endif
// MinGW calls the treeview base TV_FIRST instead of TVM_FIRST
#ifndef TV_FIRST
#define TV_FIRST                0x1100
#endif
#ifndef TVM_FIRST
#define TVM_FIRST               TV_FIRST
#endif
#ifndef TVM_SETLINESIZE
#define TVM_SETLINESIZE         (TV_FIRST + 40)
#endif
#ifndef TVM_GETLINESIZE
#define TVM_GETLINESIZE         (TV_FIRST + 41)
#endif
#ifndef TVM_SETEXTENDEDSTYLE
#define TVM_SETEXTENDEDSTYLE    (TV_FIRST + 44)
#endif
#ifndef TVM_GETEXTENDEDSTYLE
#define TVM_GETEXTENDEDSTYLE    (TV_FIRST + 45)
#endif
#ifndef TB_SETOWNER
#define TB_SETOWNER             (WM_USER + 48)
#endif
#ifndef TB_GETOWNER
#define TB_GETOWNER             (WM_USER + 49)
#endif
#ifndef MCM_GETFIRSTDOW
#define MCM_GETFIRSTDOW         MCM_GETFIRSTDAYOFWEEK
#endif
#ifndef MCM_SETFIRSTDOW
#define MCM_SETFIRSTDOW         MCM_SETFIRSTDAYOFWEEK
#endif
#ifndef HDM_GETHOTDIVIDER
#define HDM_GETHOTDIVIDER       (HDM_FIRST + 20)
#endif
#ifndef TCM_GETEXTENDEDSTYLE
#define TCM_GETEXTENDEDSTYLE    (TCM_FIRST + 53)
#endif
#ifndef TCM_SETEXTENDEDSTYLE
#define TCM_SETEXTENDEDSTYLE    (TCM_FIRST + 52)
#endif
#ifndef TB_GETEXTENDEDSTYLE
#define TB_GETEXTENDEDSTYLE     (WM_USER + 79)
#endif
#ifndef TB_SETEXTENDEDSTYLE
#define TB_SETEXTENDEDSTYLE     (WM_USER + 44)
#endif
#ifndef TB_GETPADDING
#define TB_GETPADDING           (WM_USER + 86)
#endif
#ifndef TB_SETPADDING
#define TB_SETPADDING           (WM_USER + 87)
#endif
#ifndef TB_SETLISTGAP
#define TB_SETLISTGAP           (WM_USER + 96)
#endif
#ifndef TB_GETLISTGAP
#define TB_GETLISTGAP           (WM_USER + 97)
#endif
#ifndef PBM_GETRANGE
#define PBM_GETRANGE            (WM_USER + 7)
#endif
#ifndef PBM_GETPOS
#define PBM_GETPOS              (WM_USER + 8)
#endif
#ifndef PBM_SETBARCOLOR
#define PBM_SETBARCOLOR         (WM_USER + 9)
#endif
#ifndef PBM_GETBARCOLOR
#define PBM_GETBARCOLOR         (WM_USER + 14)
#endif
#ifndef PBM_SETSTATE
#define PBM_SETSTATE            (WM_USER + 16)
#endif
#ifndef PBM_GETSTATE
#define PBM_GETSTATE            (WM_USER + 17)
#endif
#ifndef UDM_GETRANGE32
#define UDM_GETRANGE32          (UDM_FIRST + 102)
#endif
#ifndef UDM_SETRANGE32
#define UDM_SETRANGE32          (UDM_FIRST + 101)
#endif
#ifndef UDM_GETPOS32
#define UDM_GETPOS32            (UDM_FIRST + 104)
#endif
#ifndef UDM_SETPOS32
#define UDM_SETPOS32            (UDM_FIRST + 103)
#endif
#if !defined(TT_HITTESTINFO) && defined(TTHITTESTINFO)
typedef TTHITTESTINFO TT_HITTESTINFO;
#endif
#ifndef RB_FIRST
#define RB_FIRST                0x2000
#endif
#ifndef RB_GETPARENT
#define RB_GETPARENT            (RB_FIRST + 22)
#endif
#ifndef SB_SETTIPTEXT
#define SB_SETTIPTEXT           (WM_USER + 17)
#endif
#ifndef ECM_FIRST
#define ECM_FIRST               0x1500
#endif


// ---------------------------------------------------------------------
//  CImageList
// ---------------------------------------------------------------------
class CImageList : public CObject
{
    DECLARE_DYNAMIC(CImageList)
public:
    CImageList() : m_bOwner(TRUE), m_hImageList(NULL) {}
    virtual ~CImageList() { if (m_hImageList != NULL && m_bOwner) ImageList_Destroy(m_hImageList); }
    BOOL m_bOwner;      // FALSE for wrappers handed out by FromHandle()

    HIMAGELIST m_hImageList;

    operator HIMAGELIST() const { return m_hImageList; }
    HIMAGELIST GetSafeHandle() const { return m_hImageList; }
    HIMAGELIST Detach() { HIMAGELIST h = m_hImageList; m_hImageList = NULL; return h; }
    BOOL Attach(HIMAGELIST h) { if (m_hImageList) ImageList_Destroy(m_hImageList); m_hImageList = h; return h != NULL; }
    static CImageList* PASCAL FromHandle(HIMAGELIST h);
    void DeleteImageList() { if (m_hImageList && m_bOwner) { ImageList_Destroy(m_hImageList); } m_hImageList = NULL; }

    BOOL Create(int cx, int cy, UINT nFlags, int nInitial, int nGrow)
    {
        DeleteImageList();
        m_hImageList = ImageList_Create(cx, cy, nFlags, nInitial, nGrow);
        return m_hImageList != NULL;
    }
    BOOL Create(UINT nBitmapID, int cx, int nGrow, COLORREF crMask)
    {
        DeleteImageList();
        m_hImageList = ImageList_LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nBitmapID),
                                           cx, nGrow, crMask, IMAGE_BITMAP, LR_DEFAULTCOLOR);
        return m_hImageList != NULL;
    }
    BOOL Create(LPCTSTR lpszBitmapID, int cx, int nGrow, COLORREF crMask)
    {
        DeleteImageList();
        m_hImageList = ImageList_LoadImage(AfxGetResourceHandle(), lpszBitmapID,
                                           cx, nGrow, crMask, IMAGE_BITMAP, LR_DEFAULTCOLOR);
        return m_hImageList != NULL;
    }
    int GetImageCount() const { return ImageList_GetImageCount(m_hImageList); }
    BOOL SetImageCount(UINT uNewCount) { return ImageList_SetImageCount(m_hImageList, uNewCount); }
    int Add(CBitmap* pbmImage, CBitmap* pbmMask)
        { return ImageList_Add(m_hImageList, (HBITMAP)*pbmImage, pbmMask ? (HBITMAP)*pbmMask : NULL); }
    int Add(CBitmap* pbmImage, COLORREF crMask)
        { return ImageList_AddMasked(m_hImageList, (HBITMAP)*pbmImage, crMask); }
    int Add(HICON hIcon) { return ImageList_AddIcon(m_hImageList, hIcon); }
    BOOL Remove(int nImage) { return ImageList_Remove(m_hImageList, nImage); }
    BOOL RemoveAll() { return ImageList_RemoveAll(m_hImageList); }
    BOOL Replace(int nImage, CBitmap* pbmImage, CBitmap* pbmMask)
        { return ImageList_Replace(m_hImageList, nImage, (HBITMAP)*pbmImage, pbmMask ? (HBITMAP)*pbmMask : NULL); }
    BOOL Replace(int nImage, HICON hIcon) { return ImageList_ReplaceIcon(m_hImageList, nImage, hIcon) != -1; }
    int ReplaceIcon(int nImage, HICON hIcon) { return ImageList_ReplaceIcon(m_hImageList, nImage, hIcon); }
    HICON ExtractIcon(int nImage) { return ImageList_ExtractIcon(NULL, m_hImageList, nImage); }
    BOOL Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle);
    BOOL Draw(CDC* pDC, int nImage, int x, int y, UINT nStyle);
    BOOL SetBkColor(COLORREF crBk) { return ImageList_SetBkColor(m_hImageList, crBk) != CLR_NONE; }
    COLORREF GetBkColor() const { return ImageList_GetBkColor(m_hImageList); }
    BOOL SetOverlayImage(int nImage, int nOverlay) { return ImageList_SetOverlayImage(m_hImageList, nImage, nOverlay); }
    BOOL GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const
        { return ImageList_GetImageInfo(m_hImageList, nImage, pImageInfo); }
    BOOL BeginDrag(int nImage, POINT ptHotSpot) { return ImageList_BeginDrag(m_hImageList, nImage, ptHotSpot.x, ptHotSpot.y); }
    BOOL EndDrag() { ImageList_EndDrag(); return TRUE; }
    BOOL DragEnter(CWnd* pWndLock, POINT point) { return ImageList_DragEnter(pWndLock->GetSafeHwnd(), point.x, point.y); }
    BOOL DragLeave(CWnd* pWndLock) { return ImageList_DragLeave(pWndLock->GetSafeHwnd()); }
    BOOL DragMove(POINT point) { return ImageList_DragMove(point.x, point.y); }
    BOOL DragShowNolock(BOOL bShow = TRUE) { return ImageList_DragShowNolock(bShow); }
    BOOL SetDragCursorImage(int nDrag, POINT ptHotSpot)
        { return ImageList_SetDragCursorImage(m_hImageList, nDrag, ptHotSpot.x, ptHotSpot.y); }
    BOOL SetIconSize(int cx, int cy) { return ImageList_SetIconSize(m_hImageList, cx, cy); }
    BOOL GetIconSize(int* cx, int* cy) const { return ImageList_GetIconSize(m_hImageList, cx, cy); }
};

// ---------------------------------------------------------------------
//  CButton
// ---------------------------------------------------------------------
class CButton : public CWnd
{
    DECLARE_DYNAMIC(CButton)
public:
    CButton() {}
    virtual ~CButton() {}

    BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, _T("BUTTON"), lpszCaption, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    UINT GetState() const { return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0); }
    void SetState(BOOL bHighlight) { ::SendMessage(m_hWnd, BM_SETSTATE, bHighlight, 0); }
    int GetCheck() const { return (int)::SendMessage(m_hWnd, BM_GETCHECK, 0, 0); }
    void SetCheck(int nCheck) { ::SendMessage(m_hWnd, BM_SETCHECK, nCheck, 0); }
    UINT GetButtonStyle() const { return (UINT)(::GetWindowLong(m_hWnd, GWL_STYLE) & 0xFFFF); }
    void SetButtonStyle(UINT nStyle, BOOL bRedraw = TRUE)
        { ::SendMessage(m_hWnd, BM_SETSTYLE, nStyle, MAKELPARAM(bRedraw ? TRUE : FALSE, 0)); }
    HICON SetIcon(HICON hIcon) { return (HICON)::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon); }
    HICON GetIcon() const { return (HICON)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_ICON, 0); }
    HBITMAP SetBitmap(HBITMAP hBitmap) { return (HBITMAP)::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap); }
    HBITMAP GetBitmap() const { return (HBITMAP)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_BITMAP, 0); }
    void Click() { ::SendMessage(m_hWnd, BM_CLICK, 0, 0); }
    BOOL GetIdealSize(LPSIZE lpSize) const { return (BOOL)::SendMessage(m_hWnd, BCM_GETIDEALSIZE, 0, (LPARAM)lpSize); }
    BOOL SetDontClick(BOOL bDontClick) { ::SendMessage(m_hWnd, BM_SETDONTCLICK, (WPARAM)bDontClick, 0); return TRUE; }
};

// ---------------------------------------------------------------------
//  CEdit
// ---------------------------------------------------------------------
class CEdit : public CWnd
{
    DECLARE_DYNAMIC(CEdit)
public:
    CEdit() {}
    virtual ~CEdit() {}

    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    BOOL CanUndo() const { return (BOOL)::SendMessage(m_hWnd, EM_CANUNDO, 0, 0); }
    int GetLineCount() const { return (int)::SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0); }
    void SetModify(BOOL bModified = TRUE) { ::SendMessage(m_hWnd, EM_SETMODIFY, bModified, 0); }
    BOOL GetModify() const { return (BOOL)::SendMessage(m_hWnd, EM_GETMODIFY, 0, 0); }
    void SetSel(DWORD dwSelection, BOOL bNoScroll = FALSE)
        { ::SendMessage(m_hWnd, EM_SETSEL, LOWORD(dwSelection), HIWORD(dwSelection)); (void)bNoScroll; }
    void SetSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE)
        { ::SendMessage(m_hWnd, EM_SETSEL, nStartChar, nEndChar); (void)bNoScroll; }
    void GetSel(int& nStartChar, int& nEndChar) const
        { ::SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&nStartChar, (LPARAM)&nEndChar); }
    void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE)
        { ::SendMessage(m_hWnd, EM_REPLACESEL, bCanUndo, (LPARAM)lpszNewText); }
    void SetPasswordChar(TCHAR ch) { ::SendMessage(m_hWnd, EM_SETPASSWORDCHAR, ch, 0); }
    TCHAR GetPasswordChar() const { return (TCHAR)::SendMessage(m_hWnd, EM_GETPASSWORDCHAR, 0, 0); }
    int GetFirstVisibleLine() const { return (int)::SendMessage(m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0); }
    int LineFromChar(int nIndex = -1) const { return (int)::SendMessage(m_hWnd, EM_LINEFROMCHAR, nIndex, 0); }
    int LineIndex(int nLine = -1) const { return (int)::SendMessage(m_hWnd, EM_LINEINDEX, nLine, 0); }
    int LineLength(int nLine = -1) const { return (int)::SendMessage(m_hWnd, EM_LINELENGTH, nLine, 0); }
    void LineScroll(int nLines, int nChars = 0) { ::SendMessage(m_hWnd, EM_LINESCROLL, nChars, nLines); }
    int GetLine(int nIndex, LPTSTR lpBuffer) const
        { *(WORD*)lpBuffer = (WORD)::SendMessage(m_hWnd, EM_LINELENGTH, ::SendMessage(m_hWnd, EM_LINEINDEX, nIndex, 0), 0);
          return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpBuffer); }
    int GetLine(int nIndex, LPTSTR lpBuffer, int nMaxLength) const
        { *(WORD*)lpBuffer = (WORD)nMaxLength;
          int n = (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpBuffer);
          lpBuffer[n] = 0; return n; }
    void EmptyUndoBuffer() { ::SendMessage(m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0); }
    BOOL FmtLines(BOOL bAddEOL) { return (BOOL)::SendMessage(m_hWnd, EM_FMTLINES, bAddEOL, 0); }
    void LimitText(int nChars = 0) { ::SendMessage(m_hWnd, EM_LIMITTEXT, nChars, 0); }
    int SetLimitText(UINT nMax) { return (int)::SendMessage(m_hWnd, EM_SETLIMITTEXT, nMax, 0); }
    UINT GetLimitText() const { return (UINT)::SendMessage(m_hWnd, EM_GETLIMITTEXT, 0, 0); }
    CPoint PosFromChar(UINT nChar) const
        { DWORD dw = (DWORD)::SendMessage(m_hWnd, EM_POSFROMCHAR, nChar, 0); return CPoint(dw); }
    int CharFromPos(CPoint pt) const
        { return (int)::SendMessage(m_hWnd, EM_CHARFROMPOS, 0, MAKELPARAM(pt.x, pt.y)); }
    void SetMargins(UINT nLeft, UINT nRight)
        { ::SendMessage(m_hWnd, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELONG(nLeft, nRight)); }
    DWORD GetMargins() const { return (DWORD)::SendMessage(m_hWnd, EM_GETMARGINS, 0, 0); }
    BOOL SetReadOnly(BOOL bReadOnly = TRUE) { return !::SendMessage(m_hWnd, EM_SETREADONLY, bReadOnly, 0); }
    int GetCaretIndex() const { return (int)::SendMessage(m_hWnd, EM_GETCARETINDEX, 0, 0); }
    BOOL Undo() { return (BOOL)::SendMessage(m_hWnd, EM_UNDO, 0, 0); }
    BOOL Clear() { ::SendMessage(m_hWnd, WM_CLEAR, 0, 0); return TRUE; }
    BOOL Cut()   { ::SendMessage(m_hWnd, WM_CUT, 0, 0); return TRUE; }
    BOOL Copy()  { ::SendMessage(m_hWnd, WM_COPY, 0, 0); return TRUE; }
    BOOL Paste() { ::SendMessage(m_hWnd, WM_PASTE, 0, 0); return TRUE; }
    HLOCAL GetHandle() const { return (HLOCAL)::SendMessage(m_hWnd, EM_GETHANDLE, 0, 0); }
    void SetHandle(HLOCAL hBuffer) { ::SendMessage(m_hWnd, EM_SETHANDLE, (WPARAM)hBuffer, 0); }
    void SetCueBannerText(LPCTSTR lpszText, BOOL bKeepFocus = FALSE)
    {
#ifdef UNICODE
        ::SendMessage(m_hWnd, EM_SETCUEBANNER, bKeepFocus ? TRUE : FALSE, (LPARAM)(LPCWSTR)lpszText);
#else
        // EM_SETCUEBANNER always takes a wide string
        std::wstring w = AfxMultiByteToWide(lpszText);
        ::SendMessage(m_hWnd, EM_SETCUEBANNER, bKeepFocus ? TRUE : FALSE, (LPARAM)w.c_str());
#endif
    }
};

// ---------------------------------------------------------------------
//  CStatic
// ---------------------------------------------------------------------
class CStatic : public CWnd
{
    DECLARE_DYNAMIC(CStatic)
public:
    CStatic() {}
    virtual ~CStatic() {}
    BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = (UINT)-1)
    {
        return CreateEx(0, _T("STATIC"), lpszText, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    HICON SetIcon(HICON hIcon) { return (HICON)::SendMessage(m_hWnd, STM_SETICON, (WPARAM)hIcon, 0); }
    HICON GetIcon() const { return (HICON)::SendMessage(m_hWnd, STM_GETICON, 0, 0); }
    HBITMAP SetBitmap(HBITMAP hBitmap) { return (HBITMAP)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap); }
    HBITMAP GetBitmap() const { return (HBITMAP)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_BITMAP, 0); }
    HCURSOR SetCursor(HCURSOR hCursor) { return (HCURSOR)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_CURSOR, (LPARAM)hCursor); }
    HCURSOR GetCursor() { return (HCURSOR)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_CURSOR, 0); }
    HENHMETAFILE SetEnhMetaFile(HENHMETAFILE hMetaFile)
        { return (HENHMETAFILE)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_ENHMETAFILE, (LPARAM)hMetaFile); }
    HENHMETAFILE GetEnhMetaFile() const
        { return (HENHMETAFILE)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_ENHMETAFILE, 0); }
};

// ---------------------------------------------------------------------
//  CListBox
// ---------------------------------------------------------------------
class CListBox : public CWnd
{
    DECLARE_DYNAMIC(CListBox)
public:
    CListBox() {}
    virtual ~CListBox() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(WS_EX_CLIENTEDGE, _T("LISTBOX"), NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    int GetCount() const { return (int)::SendMessage(m_hWnd, LB_GETCOUNT, 0, 0); }
    int GetHorizontalExtent() const { return (int)::SendMessage(m_hWnd, LB_GETHORIZONTALEXTENT, 0, 0); }
    void SetHorizontalExtent(int cxExtent) { ::SendMessage(m_hWnd, LB_SETHORIZONTALEXTENT, cxExtent, 0); }
    int GetTopIndex() const { return (int)::SendMessage(m_hWnd, LB_GETTOPINDEX, 0, 0); }
    int SetTopIndex(int nIndex) { return (int)::SendMessage(m_hWnd, LB_SETTOPINDEX, nIndex, 0); }
    int GetItemHeight(int nIndex) const { return (int)::SendMessage(m_hWnd, LB_GETITEMHEIGHT, nIndex, 0); }
    int SetItemHeight(int nIndex, UINT cyItemHeight)
        { return (int)::SendMessage(m_hWnd, LB_SETITEMHEIGHT, nIndex, MAKELPARAM(cyItemHeight, 0)); }
    int GetCaretIndex() const { return (int)::SendMessage(m_hWnd, LB_GETCARETINDEX, 0, 0); }
    int SetCaretIndex(int nIndex, BOOL bScroll = TRUE)
        { return (int)::SendMessage(m_hWnd, LB_SETCARETINDEX, nIndex, MAKELPARAM(bScroll, 0)); }
    DWORD_PTR GetItemData(int nIndex) const { return ::SendMessage(m_hWnd, LB_GETITEMDATA, nIndex, 0); }
    int SetItemData(int nIndex, DWORD_PTR dwItemData)
        { return (int)::SendMessage(m_hWnd, LB_SETITEMDATA, nIndex, (LPARAM)dwItemData); }
    void* GetItemDataPtr(int nIndex) const { return (void*)GetItemData(nIndex); }
    int SetItemDataPtr(int nIndex, void* pData) { return SetItemData(nIndex, (DWORD_PTR)pData); }
    int GetCurSel() const { return (int)::SendMessage(m_hWnd, LB_GETCURSEL, 0, 0); }
    int SetCurSel(int nSelect) { return (int)::SendMessage(m_hWnd, LB_SETCURSEL, nSelect, 0); }
    LCID GetLocale() const { return (LCID)::SendMessage(m_hWnd, LB_GETLOCALE, 0, 0); }
    LCID SetLocale(LCID nNewLocale) { return (LCID)::SendMessage(m_hWnd, LB_SETLOCALE, nNewLocale, 0); }
    int GetSel(int nIndex) const { return (int)::SendMessage(m_hWnd, LB_GETSEL, nIndex, 0); }
    int GetSelCount() const { return (int)::SendMessage(m_hWnd, LB_GETSELCOUNT, 0, 0); }
    int GetSelItems(int nMaxItems, LPINT rgIndex) const
        { return (int)::SendMessage(m_hWnd, LB_GETSELITEMS, nMaxItems, (LPARAM)rgIndex); }
    int GetText(int nIndex, LPTSTR lpszBuffer) const
        { return (int)::SendMessage(m_hWnd, LB_GETTEXT, nIndex, (LPARAM)lpszBuffer); }
    void GetText(int nIndex, CString& rString) const
    {
        int nLen = GetTextLen(nIndex);
        if (nLen == LB_ERR) { rString.Empty(); return; }
        LPTSTR p = rString.GetBuffer(nLen + 1);
        GetText(nIndex, p);
        rString.ReleaseBuffer();
    }
    int GetTextLen(int nIndex) const { return (int)::SendMessage(m_hWnd, LB_GETTEXTLEN, nIndex, 0); }
    void SetColumnWidth(int cxWidth) { ::SendMessage(m_hWnd, LB_SETCOLUMNWIDTH, cxWidth, 0); }
    void SetTabStops(int nTabStops, LPINT rgTabStops)
        { ::SendMessage(m_hWnd, LB_SETTABSTOPS, nTabStops, (LPARAM)rgTabStops); }
    void SetTabStops() { ::SendMessage(m_hWnd, LB_SETTABSTOPS, 0, 0); }
    void SetTabStops(const int& cxEachStop) { ::SendMessage(m_hWnd, LB_SETTABSTOPS, 1, (LPARAM)&cxEachStop); }
    UINT ItemFromPoint(CPoint pt, BOOL& bOutside) const
    {
        DWORD dw = (DWORD)::SendMessage(m_hWnd, LB_ITEMFROMPOINT, 0, MAKELPARAM(pt.x, pt.y));
        bOutside = (BOOL)HIWORD(dw);
        return (UINT)LOWORD(dw);
    }
    void SetAnchorIndex(int nIndex) { ::SendMessage(m_hWnd, LB_SETANCHORINDEX, nIndex, 0); }
    int GetAnchorIndex() const { return (int)::SendMessage(m_hWnd, LB_GETANCHORINDEX, 0, 0); }
    int AddString(LPCTSTR lpszItem) { return (int)::SendMessage(m_hWnd, LB_ADDSTRING, 0, (LPARAM)lpszItem); }
    int DeleteString(UINT nIndex) { return (int)::SendMessage(m_hWnd, LB_DELETESTRING, nIndex, 0); }
    int InsertString(int nIndex, LPCTSTR lpszString) { return (int)::SendMessage(m_hWnd, LB_INSERTSTRING, nIndex, (LPARAM)lpszString); }
    void ResetContent() { ::SendMessage(m_hWnd, LB_RESETCONTENT, 0, 0); }
    int Dir(UINT attr, LPCTSTR lpszWildCard) { return (int)::SendMessage(m_hWnd, LB_DIR, attr, (LPARAM)lpszWildCard); }
    int FindString(int nStartAfter, LPCTSTR lpszItem) const
        { return (int)::SendMessage(m_hWnd, LB_FINDSTRING, nStartAfter, (LPARAM)lpszItem); }
    int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
        { return (int)::SendMessage(m_hWnd, LB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind); }
    int SelectString(int nStartAfter, LPCTSTR lpszString)
        { return (int)::SendMessage(m_hWnd, LB_SELECTSTRING, nStartAfter, (LPARAM)lpszString); }
    int SetSel(int nIndex, BOOL bSelect = TRUE) { return (int)::SendMessage(m_hWnd, LB_SETSEL, bSelect, nIndex); }
    int SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem)
        { return (int)::SendMessage(m_hWnd, LB_SELITEMRANGE, bSelect, MAKELPARAM(nFirstItem, nLastItem)); }
};

// ---------------------------------------------------------------------
//  CComboBox
// ---------------------------------------------------------------------
class CComboBox : public CWnd
{
    DECLARE_DYNAMIC(CComboBox)
public:
    CComboBox() {}
    virtual ~CComboBox() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, _T("COMBOBOX"), NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    int GetCount() const { return (int)::SendMessage(m_hWnd, CB_GETCOUNT, 0, 0); }
    int GetCurSel() const { return (int)::SendMessage(m_hWnd, CB_GETCURSEL, 0, 0); }
    int SetCurSel(int nSelect) { return (int)::SendMessage(m_hWnd, CB_SETCURSEL, nSelect, 0); }
    LCID GetLocale() const { return (LCID)::SendMessage(m_hWnd, CB_GETLOCALE, 0, 0); }
    LCID SetLocale(LCID nNewLocale) { return (LCID)::SendMessage(m_hWnd, CB_SETLOCALE, nNewLocale, 0); }
    int GetTopIndex() const { return (int)::SendMessage(m_hWnd, CB_GETTOPINDEX, 0, 0); }
    int SetTopIndex(int nIndex) { return (int)::SendMessage(m_hWnd, CB_SETTOPINDEX, nIndex, 0); }
    void SetHorizontalExtent(UINT cxExtent) { ::SendMessage(m_hWnd, CB_SETHORIZONTALEXTENT, cxExtent, 0); }
    UINT GetHorizontalExtent() const { return (UINT)::SendMessage(m_hWnd, CB_GETHORIZONTALEXTENT, 0, 0); }
    void SetDroppedWidth(UINT nWidth) { ::SendMessage(m_hWnd, CB_SETDROPPEDWIDTH, nWidth, 0); }
    UINT GetDroppedWidth() const { return (UINT)::SendMessage(m_hWnd, CB_GETDROPPEDWIDTH, 0, 0); }
    DWORD GetEditSel() const { return (DWORD)::SendMessage(m_hWnd, CB_GETEDITSEL, 0, 0); }
    BOOL LimitText(int nMaxChars) { return (BOOL)::SendMessage(m_hWnd, CB_LIMITTEXT, nMaxChars, 0); }
    BOOL SetEditSel(int nStartChar, int nEndChar)
        { return (BOOL)::SendMessage(m_hWnd, CB_SETEDITSEL, 0, MAKELPARAM(nStartChar, nEndChar)); }
    DWORD_PTR GetItemData(int nIndex) const { return ::SendMessage(m_hWnd, CB_GETITEMDATA, nIndex, 0); }
    int SetItemData(int nIndex, DWORD_PTR dwItemData)
        { return (int)::SendMessage(m_hWnd, CB_SETITEMDATA, nIndex, (LPARAM)dwItemData); }
    void* GetItemDataPtr(int nIndex) const { return (void*)GetItemData(nIndex); }
    int SetItemDataPtr(int nIndex, void* pData) { return SetItemData(nIndex, (DWORD_PTR)pData); }
    int GetLBText(int nIndex, LPTSTR lpszText) const
        { return (int)::SendMessage(m_hWnd, CB_GETLBTEXT, nIndex, (LPARAM)lpszText); }
    void GetLBText(int nIndex, CString& rString) const
    {
        int nLen = GetLBTextLen(nIndex);
        if (nLen == CB_ERR) { rString.Empty(); return; }
        LPTSTR p = rString.GetBuffer(nLen + 1);
        GetLBText(nIndex, p);
        rString.ReleaseBuffer();
    }
    int GetLBTextLen(int nIndex) const { return (int)::SendMessage(m_hWnd, CB_GETLBTEXTLEN, nIndex, 0); }
    int SetItemHeight(int nIndex, UINT cyItemHeight)
        { return (int)::SendMessage(m_hWnd, CB_SETITEMHEIGHT, nIndex, MAKELPARAM(cyItemHeight, 0)); }
    int GetItemHeight(int nIndex) const { return (int)::SendMessage(m_hWnd, CB_GETITEMHEIGHT, nIndex, 0); }
    int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
        { return (int)::SendMessage(m_hWnd, CB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind); }
    int SetExtendedUI(BOOL bExtended = TRUE) { return (int)::SendMessage(m_hWnd, CB_SETEXTENDEDUI, bExtended, 0); }
    BOOL GetExtendedUI() const { return (BOOL)::SendMessage(m_hWnd, CB_GETEXTENDEDUI, 0, 0); }
    void GetDroppedControlRect(LPRECT lprect) const { ::SendMessage(m_hWnd, CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)lprect); }
    BOOL GetDroppedState() const { return (BOOL)::SendMessage(m_hWnd, CB_GETDROPPEDSTATE, 0, 0); }
    void ShowDropDown(BOOL bShowIt = TRUE) { ::SendMessage(m_hWnd, CB_SHOWDROPDOWN, bShowIt, 0); }
    int AddString(LPCTSTR lpszString) { return (int)::SendMessage(m_hWnd, CB_ADDSTRING, 0, (LPARAM)lpszString); }
    int DeleteString(UINT nIndex) { return (int)::SendMessage(m_hWnd, CB_DELETESTRING, nIndex, 0); }
    int InsertString(int nIndex, LPCTSTR lpszString) { return (int)::SendMessage(m_hWnd, CB_INSERTSTRING, nIndex, (LPARAM)lpszString); }
    void ResetContent() { ::SendMessage(m_hWnd, CB_RESETCONTENT, 0, 0); }
    int Dir(UINT attr, LPCTSTR lpszWildCard) { return (int)::SendMessage(m_hWnd, CB_DIR, attr, (LPARAM)lpszWildCard); }
    int FindString(int nStartAfter, LPCTSTR lpszString) const
        { return (int)::SendMessage(m_hWnd, CB_FINDSTRING, nStartAfter, (LPARAM)lpszString); }
    int SelectString(int nStartAfter, LPCTSTR lpszString)
        { return (int)::SendMessage(m_hWnd, CB_SELECTSTRING, nStartAfter, (LPARAM)lpszString); }
    void Clear() { ::SendMessage(m_hWnd, WM_CLEAR, 0, 0); }
    void Copy()  { ::SendMessage(m_hWnd, WM_COPY, 0, 0); }
    void Cut()   { ::SendMessage(m_hWnd, WM_CUT, 0, 0); }
    void Paste() { ::SendMessage(m_hWnd, WM_PASTE, 0, 0); }
};

// ---------------------------------------------------------------------
//  CTabCtrl
// ---------------------------------------------------------------------
class CTabCtrl : public CWnd
{
    DECLARE_DYNAMIC(CTabCtrl)
public:
    CTabCtrl() {}
    virtual ~CTabCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, WC_TABCONTROL, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    CImageList* GetImageList() const { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TCM_GETIMAGELIST, 0, 0)); }
    CImageList* SetImageList(CImageList* pImageList)
        { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TCM_SETIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }
    int GetItemCount() const { return (int)::SendMessage(m_hWnd, TCM_GETITEMCOUNT, 0, 0); }
    BOOL GetItem(int nItem, TC_ITEM* pTabCtrlItem) const
        { return (BOOL)::SendMessage(m_hWnd, TCM_GETITEM, nItem, (LPARAM)pTabCtrlItem); }
    BOOL SetItem(int nItem, TC_ITEM* pTabCtrlItem)
        { return (BOOL)::SendMessage(m_hWnd, TCM_SETITEM, nItem, (LPARAM)pTabCtrlItem); }
    BOOL InsertItem(int nItem, TC_ITEM* pTabCtrlItem)
        { return (BOOL)::SendMessage(m_hWnd, TCM_INSERTITEM, nItem, (LPARAM)pTabCtrlItem); }
    BOOL InsertItem(int nItem, LPCTSTR lpszItem)
    {
        TC_ITEM ti; ZeroMemory(&ti, sizeof(ti));
        ti.mask = TCIF_TEXT; ti.pszText = (LPTSTR)lpszItem;
        return InsertItem(nItem, &ti);
    }
    BOOL InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
    {
        TC_ITEM ti; ZeroMemory(&ti, sizeof(ti));
        ti.mask = TCIF_TEXT | TCIF_IMAGE; ti.pszText = (LPTSTR)lpszItem; ti.iImage = nImage;
        return InsertItem(nItem, &ti);
    }
    BOOL InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, int nImage = -1, LPARAM lParam = 0)
    {
        TC_ITEM ti; ZeroMemory(&ti, sizeof(ti));
        ti.mask = nMask; ti.pszText = (LPTSTR)lpszItem; ti.iImage = nImage; ti.lParam = lParam;
        return InsertItem(nItem, &ti);
    }
    BOOL DeleteItem(int nItem) { return (BOOL)::SendMessage(m_hWnd, TCM_DELETEITEM, nItem, 0); }
    BOOL DeleteAllItems() { return (BOOL)::SendMessage(m_hWnd, TCM_DELETEALLITEMS, 0, 0); }
    void AdjustRect(BOOL bLarger, LPRECT lpRect) { ::SendMessage(m_hWnd, TCM_ADJUSTRECT, bLarger, (LPARAM)lpRect); }
    void SetItemSize(CSize size) { ::SendMessage(m_hWnd, TCM_SETITEMSIZE, 0, MAKELPARAM(size.cx, size.cy)); }
    void RemoveImage(int nImage) { ::SendMessage(m_hWnd, TCM_REMOVEIMAGE, nImage, 0); }
    int SetCurSel(int nItem) { return (int)::SendMessage(m_hWnd, TCM_SETCURSEL, nItem, 0); }
    int GetCurSel() const { return (int)::SendMessage(m_hWnd, TCM_GETCURSEL, 0, 0); }
    int HitTest(TC_HITTESTINFO* pHitTestInfo) const
        { return (int)::SendMessage(m_hWnd, TCM_HITTEST, 0, (LPARAM)pHitTestInfo); }
    void SetCurFocus(int nItem) { ::SendMessage(m_hWnd, TCM_SETCURFOCUS, nItem, 0); }
    int GetCurFocus() const { return (int)::SendMessage(m_hWnd, TCM_GETCURFOCUS, 0, 0); }
    int GetRowCount() const { return (int)::SendMessage(m_hWnd, TCM_GETROWCOUNT, 0, 0); }
    void SetMinTabWidth(int cx) { ::SendMessage(m_hWnd, TCM_SETMINTABWIDTH, 0, cx); }
    DWORD GetExtendedStyle() { return (DWORD)::SendMessage(m_hWnd, TCM_GETEXTENDEDSTYLE, 0, 0); }
    DWORD SetExtendedStyle(DWORD dwNewStyle, DWORD dwExMask = 0)
        { return (DWORD)::SendMessage(m_hWnd, TCM_SETEXTENDEDSTYLE, dwExMask, dwNewStyle); }
    void DeselectAll(int fExcludeFocus) { ::SendMessage(m_hWnd, TCM_DESELECTALL, fExcludeFocus, 0); }
    BOOL HighlightItem(int nItem, BOOL fHighlight = TRUE)
        { return (BOOL)::SendMessage(m_hWnd, TCM_HIGHLIGHTITEM, nItem, MAKELPARAM(fHighlight, 0)); }
    void SetItemExtra(int nBytes) { ::SendMessage(m_hWnd, TCM_SETITEMEXTRA, nBytes, 0); }
    BOOL SetPadding(CSize size) { ::SendMessage(m_hWnd, TCM_SETPADDING, 0, MAKELPARAM(size.cx, size.cy)); return TRUE; }
    BOOL GetItemText(int nItem, CString& rText) const
    {
        TCHAR buf[512];
        TC_ITEM ti; ZeroMemory(&ti, sizeof(ti));
        ti.mask = TCIF_TEXT; ti.pszText = buf; ti.cchTextMax = 512;
        if (!GetItem(nItem, &ti)) return FALSE;
        rText = buf;
        return TRUE;
    }
    BOOL SetItemText(int nItem, LPCTSTR lpszText)
    {
        TC_ITEM ti; ZeroMemory(&ti, sizeof(ti));
        ti.mask = TCIF_TEXT; ti.pszText = (LPTSTR)lpszText;
        return SetItem(nItem, &ti);
    }
};

// ---------------------------------------------------------------------
//  CListCtrl
// ---------------------------------------------------------------------
class CListCtrl : public CWnd
{
    DECLARE_DYNAMIC(CListCtrl)
public:
    CListCtrl() {}
    virtual ~CListCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    COLORREF GetBkColor() const { return (COLORREF)::SendMessage(m_hWnd, LVM_GETBKCOLOR, 0, 0); }
    BOOL SetBkColor(COLORREF cr) { return (BOOL)::SendMessage(m_hWnd, LVM_SETBKCOLOR, 0, cr); }
    CImageList* GetImageList(int nImageList) const
        { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, LVM_GETIMAGELIST, nImageList, 0)); }
    CImageList* SetImageList(CImageList* pImageList, int nImageList)
        { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, LVM_SETIMAGELIST, nImageList,
                     (LPARAM)(pImageList ? pImageList->GetSafeHandle() : NULL))); }
    int GetItemCount() const { return (int)::SendMessage(m_hWnd, LVM_GETITEMCOUNT, 0, 0); }
    BOOL GetItem(LVITEM* pItem) const { return (BOOL)::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)pItem); }
    BOOL SetItem(const LVITEM* pItem) { return (BOOL)::SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)pItem); }
    BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem, int nImage,
                 UINT nState, UINT nStateMask, LPARAM lParam)
    {
        LVITEM li; ZeroMemory(&li, sizeof(li));
        li.mask = nMask; li.iItem = nItem; li.iSubItem = nSubItem;
        li.pszText = (LPTSTR)lpszItem; li.iImage = nImage;
        li.state = nState; li.stateMask = nStateMask; li.lParam = lParam;
        return SetItem(&li);
    }
    UINT GetCallbackMask() const { return (UINT)::SendMessage(m_hWnd, LVM_GETCALLBACKMASK, 0, 0); }
    BOOL SetCallbackMask(UINT nMask) { return (BOOL)::SendMessage(m_hWnd, LVM_SETCALLBACKMASK, nMask, 0); }
    int GetNextItem(int nItem, UINT nFlags) const { return (int)::SendMessage(m_hWnd, LVM_GETNEXTITEM, nItem, MAKELPARAM(nFlags, 0)); }
    BOOL GetItemPosition(int nItem, LPPOINT lpPoint) const
        { return (BOOL)::SendMessage(m_hWnd, LVM_GETITEMPOSITION, nItem, (LPARAM)lpPoint); }
    BOOL SetItemPosition(int nItem, POINT pt)
        { return (BOOL)::SendMessage(m_hWnd, LVM_SETITEMPOSITION, nItem, MAKELPARAM(pt.x, pt.y)); }
    BOOL SetItemPosition32(int nItem, int x, int y)
        { POINTL ptl = { x, y }; ::SendMessage(m_hWnd, LVM_SETITEMPOSITION32, nItem, (LPARAM)&ptl); return TRUE; }
    BOOL GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const
        { ((LPRECT)lpRect)->left = nCode; return (BOOL)::SendMessage(m_hWnd, LVM_GETITEMRECT, nItem, (LPARAM)lpRect); }
    BOOL SetItemState(int nItem, LVITEM* pItem) { return (BOOL)::SendMessage(m_hWnd, LVM_SETITEMSTATE, nItem, (LPARAM)pItem); }
    BOOL SetItemState(int nItem, UINT nState, UINT nMask)
    {
        LVITEM li; ZeroMemory(&li, sizeof(li));
        li.state = nState; li.stateMask = nMask;
        return SetItemState(nItem, &li);
    }
    UINT GetItemState(int nItem, UINT nMask) const
        { return (UINT)::SendMessage(m_hWnd, LVM_GETITEMSTATE, nItem, nMask); }
    CString GetItemText(int nItem, int nSubItem) const
    {
        TCHAR buf[1024];
        LVITEM li; ZeroMemory(&li, sizeof(li));
        li.iSubItem = nSubItem; li.pszText = buf; li.cchTextMax = 1024;
        LRESULT n = ::SendMessage(m_hWnd, LVM_GETITEMTEXT, nItem, (LPARAM)&li);
        if (n <= 0) return CString();
        return CString(buf, (int)n);
    }
    int GetItemText(int nItem, int nSubItem, LPTSTR lpszText, int nLen) const
    {
        LVITEM li; ZeroMemory(&li, sizeof(li));
        li.iSubItem = nSubItem; li.pszText = lpszText; li.cchTextMax = nLen;
        return (int)::SendMessage(m_hWnd, LVM_GETITEMTEXT, nItem, (LPARAM)&li);
    }
    BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
        { return SetItem(nItem, nSubItem, LVIF_TEXT, lpszText, 0, 0, 0, 0); }
    void SetItemCount(int nItems) { ::SendMessage(m_hWnd, LVM_SETITEMCOUNT, nItems, 0); }
    DWORD_PTR GetItemData(int nItem) const
    {
        LVITEM li; ZeroMemory(&li, sizeof(li));
        li.mask = LVIF_PARAM; li.iItem = nItem;
        if (!GetItem(&li)) return 0;
        return li.lParam;
    }
    BOOL SetItemData(int nItem, DWORD_PTR dwData)
        { return SetItem(nItem, 0, LVIF_PARAM, NULL, 0, 0, 0, (LPARAM)dwData); }
    BOOL DeleteItem(int nItem) { return (BOOL)::SendMessage(m_hWnd, LVM_DELETEITEM, nItem, 0); }
    BOOL DeleteAllItems() { return (BOOL)::SendMessage(m_hWnd, LVM_DELETEALLITEMS, 0, 0); }
    int GetSelectedCount() const { return (int)::SendMessage(m_hWnd, LVM_GETSELECTEDCOUNT, 0, 0); }
    CPoint GetItemSpacing(BOOL fSmall = TRUE) const
        { return CPoint((DWORD)::SendMessage(m_hWnd, LVM_GETITEMSPACING, fSmall, 0)); }
    BOOL GetColumn(int nCol, LVCOLUMN* pColumn) const { return (BOOL)::SendMessage(m_hWnd, LVM_GETCOLUMN, nCol, (LPARAM)pColumn); }
    BOOL SetColumn(int nCol, const LVCOLUMN* pColumn) { return (BOOL)::SendMessage(m_hWnd, LVM_SETCOLUMN, nCol, (LPARAM)pColumn); }
    int InsertColumn(int nCol, const LVCOLUMN* pColumn) { return (int)::SendMessage(m_hWnd, LVM_INSERTCOLUMN, nCol, (LPARAM)pColumn); }
    int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1)
    {
        LVCOLUMN col; ZeroMemory(&col, sizeof(col));
        col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        col.fmt = nFormat; col.cx = nWidth; col.pszText = (LPTSTR)lpszColumnHeading; col.iSubItem = nSubItem;
        return InsertColumn(nCol, &col);
    }
    BOOL DeleteColumn(int nCol) { return (BOOL)::SendMessage(m_hWnd, LVM_DELETECOLUMN, nCol, 0); }
    int GetColumnWidth(int nCol) const { return (int)::SendMessage(m_hWnd, LVM_GETCOLUMNWIDTH, nCol, 0); }
    BOOL SetColumnWidth(int nCol, int cx) { return (BOOL)::SendMessage(m_hWnd, LVM_SETCOLUMNWIDTH, nCol, MAKELPARAM(cx, 0)); }
    BOOL GetViewRect(LPRECT lpRect) const { return (BOOL)::SendMessage(m_hWnd, LVM_GETVIEWRECT, 0, (LPARAM)lpRect); }
    COLORREF GetTextColor() const { return (COLORREF)::SendMessage(m_hWnd, LVM_GETTEXTCOLOR, 0, 0); }
    BOOL SetTextColor(COLORREF cr) { return (BOOL)::SendMessage(m_hWnd, LVM_SETTEXTCOLOR, 0, cr); }
    COLORREF GetTextBkColor() const { return (COLORREF)::SendMessage(m_hWnd, LVM_GETTEXTBKCOLOR, 0, 0); }
    BOOL SetTextBkColor(COLORREF cr) { return (BOOL)::SendMessage(m_hWnd, LVM_SETTEXTBKCOLOR, 0, cr); }
    int GetTopIndex() const { return (int)::SendMessage(m_hWnd, LVM_GETTOPINDEX, 0, 0); }
    int GetCountPerPage() const { return (int)::SendMessage(m_hWnd, LVM_GETCOUNTPERPAGE, 0, 0); }
    BOOL GetOrigin(LPPOINT lpPoint) const { return (BOOL)::SendMessage(m_hWnd, LVM_GETORIGIN, 0, (LPARAM)lpPoint); }
    BOOL SetOrigin(POINT pt) { return (BOOL)::SendMessage(m_hWnd, LVM_SETORIGIN, 0, MAKELPARAM(pt.x, pt.y)); }
    int GetSelectionMark() const { return (int)::SendMessage(m_hWnd, LVM_GETSELECTIONMARK, 0, 0); }
    int SetSelectionMark(int nIndex) { return (int)::SendMessage(m_hWnd, LVM_SETSELECTIONMARK, 0, nIndex); }
    BOOL Update(int nItem) { return (BOOL)::SendMessage(m_hWnd, LVM_UPDATE, nItem, 0); }
    BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData)
        { return (BOOL)::SendMessage(m_hWnd, LVM_SORTITEMS, dwData, (LPARAM)pfnCompare); }
    CEdit* GetEditControl() const { return (CEdit*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, LVM_GETEDITCONTROL, 0, 0)); }
    CWnd* GetHeaderCtrl() const { return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, LVM_GETHEADER, 0, 0)); }
    BOOL SetColumnOrderArray(int iCount, LPINT piArray) { return (BOOL)::SendMessage(m_hWnd, LVM_SETCOLUMNORDERARRAY, iCount, (LPARAM)piArray); }
    int* GetColumnOrderArray(int iCount = 0)
    {
        if (iCount == 0) iCount = GetItemCount();
        int* pArray = new int[iCount];
        if (::SendMessage(m_hWnd, LVM_GETCOLUMNORDERARRAY, iCount, (LPARAM)pArray)) return pArray;
        delete[] pArray;
        return NULL;
    }
    CSize SetIconSpacing(CSize size)
        { return CSize((DWORD)::SendMessage(m_hWnd, LVM_SETICONSPACING, 0, MAKELPARAM(size.cx, size.cy))); }
    CSize SetIconSpacing(int cx, int cy)
        { return CSize((DWORD)::SendMessage(m_hWnd, LVM_SETICONSPACING, 0, MAKELPARAM(cx, cy))); }
    CSize ApproximateViewRect(CSize size = CSize(-1, -1), int iCount = -1) const
        { return CSize((DWORD)::SendMessage(m_hWnd, LVM_APPROXIMATEVIEWRECT, iCount, MAKELPARAM(size.cx, size.cy))); }
    DWORD GetExtendedStyle() const { return (DWORD)::SendMessage(m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0); }
    DWORD SetExtendedStyle(DWORD dwNewStyle, DWORD dwExMask = 0)
        { return (DWORD)::SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, dwExMask, dwNewStyle); }
    BOOL GetSubItemRect(int iItem, int iSubItem, int nArea, CRect& ref) const
    {
        ref.SetRect(nArea, iSubItem, 0, 0);
        return (BOOL)::SendMessage(m_hWnd, LVM_GETSUBITEMRECT, iItem, (LPARAM)&ref);
    }
    int SubItemHitTest(LVHITTESTINFO* pInfo) const { return (int)::SendMessage(m_hWnd, LVM_SUBITEMHITTEST, 0, (LPARAM)pInfo); }
    int HitTest(LVHITTESTINFO* pInfo) const { return (int)::SendMessage(m_hWnd, LVM_HITTEST, 0, (LPARAM)pInfo); }
    int HitTest(CPoint pt, UINT* pFlags = NULL) const
    {
        LVHITTESTINFO hti; ZeroMemory(&hti, sizeof(hti));
        hti.pt = pt;
        int n = HitTest(&hti);
        if (pFlags != NULL) *pFlags = hti.flags;
        return n;
    }
    int InsertItem(const LVITEM* pItem) { return (int)::SendMessage(m_hWnd, LVM_INSERTITEM, 0, (LPARAM)pItem); }
    int InsertItem(int nItem, LPCTSTR lpszItem)
        { return InsertItem(LVIF_TEXT, nItem, lpszItem, 0, 0, 0, 0); }
    int InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
        { return InsertItem(LVIF_TEXT | LVIF_IMAGE, nItem, lpszItem, 0, 0, nImage, 0); }
    int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam)
    {
        LVITEM li; ZeroMemory(&li, sizeof(li));
        li.mask = nMask; li.iItem = nItem; li.iSubItem = 0;
        li.pszText = (LPTSTR)lpszItem; li.state = nState; li.stateMask = nStateMask;
        li.iImage = nImage; li.lParam = lParam;
        return InsertItem(&li);
    }
    BOOL EnsureVisible(int nItem, BOOL bPartialOK)
        { return (BOOL)::SendMessage(m_hWnd, LVM_ENSUREVISIBLE, nItem, MAKELPARAM(bPartialOK, 0)); }
    int FindItem(LVFINDINFO* pFindInfo, int nStart = -1) const
        { return (int)::SendMessage(m_hWnd, LVM_FINDITEM, nStart, (LPARAM)pFindInfo); }
    void RedrawItems(int nFirst, int nLast) { ::SendMessage(m_hWnd, LVM_REDRAWITEMS, nFirst, nLast); }
    BOOL Scroll(CSize size) { return (BOOL)::SendMessage(m_hWnd, LVM_SCROLL, size.cx, size.cy); }
    void EnableGroupView(BOOL fEnable) { ::SendMessage(m_hWnd, LVM_ENABLEGROUPVIEW, fEnable, 0); }
    int InsertGroup(int index, LVGROUP* pGroup) { return (int)::SendMessage(m_hWnd, LVM_INSERTGROUP, index, (LPARAM)pGroup); }
    void RemoveAllGroups() { ::SendMessage(m_hWnd, LVM_REMOVEALLGROUPS, 0, 0); }
    void RemoveGroup(int iGroupID) { ::SendMessage(m_hWnd, LVM_REMOVEGROUP, iGroupID, 0); }
    void MoveGroup(int iGroupID, int toIndex) { ::SendMessage(m_hWnd, LVM_MOVEGROUP, iGroupID, toIndex); }
    void MoveItemToGroup(int idItemFrom, int idGroupTo) { ::SendMessage(m_hWnd, LVM_MOVEITEMTOGROUP, idItemFrom, idGroupTo); }
    BOOL SortGroups(PFNLVGROUPCOMPARE pGroupCompare, LPVOID plv)
        { return (BOOL)::SendMessage(m_hWnd, LVM_SORTGROUPS, (WPARAM)pGroupCompare, (LPARAM)plv); }
    void SetGroupMetrics(LVGROUPMETRICS* pGroupMetrics) { ::SendMessage(m_hWnd, LVM_SETGROUPMETRICS, 0, (LPARAM)pGroupMetrics); }
    void GetGroupMetrics(LVGROUPMETRICS* pGroupMetrics) const { ::SendMessage(m_hWnd, LVM_GETGROUPMETRICS, 0, (LPARAM)pGroupMetrics); }
};

// ---------------------------------------------------------------------
//  CTreeCtrl
// ---------------------------------------------------------------------
class CTreeCtrl : public CWnd
{
    DECLARE_DYNAMIC(CTreeCtrl)
public:
    CTreeCtrl() {}
    virtual ~CTreeCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    CImageList* GetImageList(int nImageList) const
        { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TVM_GETIMAGELIST, nImageList, 0)); }
    CImageList* SetImageList(CImageList* pImageList, int nImageListType)
        { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TVM_SETIMAGELIST, nImageListType,
                    (LPARAM)(pImageList ? pImageList->GetSafeHandle() : NULL))); }
    UINT GetCount() const { return (UINT)::SendMessage(m_hWnd, TVM_GETCOUNT, 0, 0); }
    UINT GetVisibleCount() const { return (UINT)::SendMessage(m_hWnd, TVM_GETVISIBLECOUNT, 0, 0); }
    HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode) const
        { return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, nCode, (LPARAM)hItem); }
    HTREEITEM GetChildItem(HTREEITEM hItem) const { return GetNextItem(hItem, TVGN_CHILD); }
    HTREEITEM GetNextSiblingItem(HTREEITEM hItem) const { return GetNextItem(hItem, TVGN_NEXT); }
    HTREEITEM GetPrevSiblingItem(HTREEITEM hItem) const { return GetNextItem(hItem, TVGN_PREVIOUS); }
    HTREEITEM GetParentItem(HTREEITEM hItem) const { return GetNextItem(hItem, TVGN_PARENT); }
    HTREEITEM GetFirstVisibleItem() const { return GetNextItem(NULL, TVGN_FIRSTVISIBLE); }
    HTREEITEM GetNextVisibleItem(HTREEITEM hItem) const { return GetNextItem(hItem, TVGN_NEXTVISIBLE); }
    HTREEITEM GetPrevVisibleItem(HTREEITEM hItem) const { return GetNextItem(hItem, TVGN_PREVIOUSVISIBLE); }
    HTREEITEM GetSelectedItem() const { return GetNextItem(NULL, TVGN_CARET); }
    HTREEITEM GetDropHilightItem() const { return GetNextItem(NULL, TVGN_DROPHILITE); }
    HTREEITEM GetRootItem() const { return GetNextItem(NULL, TVGN_ROOT); }
    BOOL GetItem(TVITEM* pItem) const { return (BOOL)::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)pItem); }
    BOOL SetItem(TVITEM* pItem) { return (BOOL)::SendMessage(m_hWnd, TVM_SETITEM, 0, (LPARAM)pItem); }
    BOOL SetItem(HTREEITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage,
                 UINT nState, UINT nStateMask, LPARAM lParam)
    {
        TVITEM tvi; ZeroMemory(&tvi, sizeof(tvi));
        tvi.hItem = hItem; tvi.mask = nMask; tvi.pszText = (LPTSTR)lpszItem;
        tvi.iImage = nImage; tvi.iSelectedImage = nSelectedImage;
        tvi.state = nState; tvi.stateMask = nStateMask; tvi.lParam = lParam;
        return SetItem(&tvi);
    }
    CString GetItemText(HTREEITEM hItem) const
    {
        TCHAR buf[1024];
        TVITEM tvi; ZeroMemory(&tvi, sizeof(tvi));
        tvi.hItem = hItem; tvi.mask = TVIF_TEXT; tvi.pszText = buf; tvi.cchTextMax = 1024;
        if (!GetItem(&tvi)) return CString();
        return CString(buf);
    }
    BOOL SetItemText(HTREEITEM hItem, LPCTSTR lpszItem)
        { return SetItem(hItem, TVIF_TEXT, lpszItem, 0, 0, 0, 0, 0); }
    int GetItemImage(HTREEITEM hItem, int& nImage, int& nSelectedImage) const
    {
        TVITEM tvi; ZeroMemory(&tvi, sizeof(tvi));
        tvi.hItem = hItem; tvi.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        if (!GetItem(&tvi)) return 0;
        nImage = tvi.iImage; nSelectedImage = tvi.iSelectedImage;
        return 1;
    }
    BOOL SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage)
        { return SetItem(hItem, TVIF_IMAGE | TVIF_SELECTEDIMAGE, NULL, nImage, nSelectedImage, 0, 0, 0); }
    DWORD_PTR GetItemData(HTREEITEM hItem) const
    {
        TVITEM tvi; ZeroMemory(&tvi, sizeof(tvi));
        tvi.hItem = hItem; tvi.mask = TVIF_HANDLE | TVIF_PARAM;
        if (!GetItem(&tvi)) return 0;
        return tvi.lParam;
    }
    BOOL SetItemData(HTREEITEM hItem, DWORD_PTR dwData)
        { return SetItem(hItem, TVIF_PARAM, NULL, 0, 0, 0, 0, (LPARAM)dwData); }
    BOOL ItemHasChildren(HTREEITEM hItem) const { return GetChildItem(hItem) != NULL; }
    UINT GetItemState(HTREEITEM hItem, UINT nStateMask) const
        { return (UINT)::SendMessage(m_hWnd, TVM_GETITEMSTATE, (WPARAM)hItem, (LPARAM)nStateMask); }
    BOOL SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask)
        { return SetItem(hItem, TVIF_STATE, NULL, 0, 0, nState, nStateMask, 0); }
    BOOL GetItemRect(HTREEITEM hItem, LPRECT lpRect, BOOL bTextOnly = FALSE) const
        { ((LPRECT)lpRect)->left = bTextOnly; return (BOOL)::SendMessage(m_hWnd, TVM_GETITEMRECT, 0, (LPARAM)lpRect); }
    BOOL SetInsertMark(HTREEITEM hItem, BOOL fAfter = TRUE)
        { return (BOOL)::SendMessage(m_hWnd, TVM_SETINSERTMARK, fAfter, (LPARAM)hItem); }
    HTREEITEM InsertItem(LPTVINSERTSTRUCT lpInsertStruct)
        { return (HTREEITEM)::SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)lpInsertStruct); }
    HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage,
                         UINT nState, UINT nStateMask, LPARAM lParam,
                         HTREEITEM hParent, HTREEITEM hInsertAfter)
    {
        TVINSERTSTRUCT tvis; ZeroMemory(&tvis, sizeof(tvis));
        tvis.hParent = hParent; tvis.hInsertAfter = hInsertAfter;
        tvis.item.mask = nMask; tvis.item.pszText = (LPTSTR)lpszItem;
        tvis.item.iImage = nImage; tvis.item.iSelectedImage = nSelectedImage;
        tvis.item.state = nState; tvis.item.stateMask = nStateMask; tvis.item.lParam = lParam;
        return InsertItem(&tvis);
    }
    HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST)
        { return InsertItem(TVIF_TEXT, lpszItem, 0, 0, 0, 0, 0, hParent, hInsertAfter); }
    HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage,
                         HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST)
        { return InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE, lpszItem, nImage, nSelectedImage, 0, 0, 0, hParent, hInsertAfter); }
    BOOL DeleteItem(HTREEITEM hItem) { return (BOOL)::SendMessage(m_hWnd, TVM_DELETEITEM, 0, (LPARAM)hItem); }
    BOOL DeleteAllItems() { return DeleteItem(TVI_ROOT); }
    BOOL Expand(HTREEITEM hItem, UINT nCode) { return (BOOL)::SendMessage(m_hWnd, TVM_EXPAND, nCode, (LPARAM)hItem); }
    BOOL Select(HTREEITEM hItem, UINT nCode) { return (BOOL)::SendMessage(m_hWnd, TVM_SELECTITEM, nCode, (LPARAM)hItem); }
    BOOL SelectItem(HTREEITEM hItem) { return Select(hItem, TVGN_CARET); }
    BOOL SelectDropTarget(HTREEITEM hItem) { return Select(hItem, TVGN_DROPHILITE); }
    BOOL SelectSetFirstVisible(HTREEITEM hItem) { return Select(hItem, TVGN_FIRSTVISIBLE); }
    CEdit* GetEditControl() const { return (CEdit*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_GETEDITCONTROL, 0, 0)); }
    CEdit* EditLabel(HTREEITEM hItem)
        { return (CEdit*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_EDITLABEL, 0, (LPARAM)hItem)); }
    HTREEITEM HitTest(CPoint pt, UINT* pFlags = NULL) const
    {
        TVHITTESTINFO hti; ZeroMemory(&hti, sizeof(hti));
        hti.pt = pt;
        HTREEITEM h = (HTREEITEM)::SendMessage(m_hWnd, TVM_HITTEST, 0, (LPARAM)&hti);
        if (pFlags != NULL) *pFlags = hti.flags;
        return h;
    }
    HTREEITEM HitTest(TVHITTESTINFO* pHitTestInfo) const
        { return (HTREEITEM)::SendMessage(m_hWnd, TVM_HITTEST, 0, (LPARAM)pHitTestInfo); }
    BOOL SortChildren(HTREEITEM hItem) { return (BOOL)::SendMessage(m_hWnd, TVM_SORTCHILDREN, 0, (LPARAM)hItem); }
    BOOL EnsureVisible(HTREEITEM hItem) { return (BOOL)::SendMessage(m_hWnd, TVM_ENSUREVISIBLE, 0, (LPARAM)hItem); }
    BOOL SortChildrenCB(LPTVSORTCB pSort) { return (BOOL)::SendMessage(m_hWnd, TVM_SORTCHILDRENCB, 0, (LPARAM)pSort); }
    BOOL SetItemHeight(short cyHeight) { return (BOOL)::SendMessage(m_hWnd, TVM_SETITEMHEIGHT, cyHeight, 0); }
    short GetItemHeight() const { return (short)::SendMessage(m_hWnd, TVM_GETITEMHEIGHT, 0, 0); }
    BOOL SetIndent(int nIndent) { ::SendMessage(m_hWnd, TVM_SETINDENT, nIndent, 0); return TRUE; }
    int GetIndent() const { return (int)::SendMessage(m_hWnd, TVM_GETINDENT, 0, 0); }
    COLORREF GetBkColor() const { return (COLORREF)::SendMessage(m_hWnd, TVM_GETBKCOLOR, 0, 0); }
    COLORREF SetBkColor(COLORREF clr) { return (COLORREF)::SendMessage(m_hWnd, TVM_SETBKCOLOR, 0, clr); }
    COLORREF GetTextColor() const { return (COLORREF)::SendMessage(m_hWnd, TVM_GETTEXTCOLOR, 0, 0); }
    COLORREF SetTextColor(COLORREF clr) { return (COLORREF)::SendMessage(m_hWnd, TVM_SETTEXTCOLOR, 0, clr); }
    DWORD GetExtendedStyle() const { return (DWORD)::SendMessage(m_hWnd, TVM_GETEXTENDEDSTYLE, 0, 0); }
    DWORD SetExtendedStyle(DWORD dwStyle, DWORD dwMask = 0)
        { return (DWORD)::SendMessage(m_hWnd, TVM_SETEXTENDEDSTYLE, dwMask, dwStyle); }
    BOOL SetLineSize(int iWidth) { return (BOOL)::SendMessage(m_hWnd, TVM_SETLINESIZE, 0, iWidth); }
    int GetLineSize() const { return (int)::SendMessage(m_hWnd, TVM_GETLINESIZE, 0, 0); }
};

// ---------------------------------------------------------------------
//  CHeaderCtrl
// ---------------------------------------------------------------------
class CHeaderCtrl : public CWnd
{
    DECLARE_DYNAMIC(CHeaderCtrl)
public:
    CHeaderCtrl() {}
    virtual ~CHeaderCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, WC_HEADER, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    int GetItemCount() const { return (int)::SendMessage(m_hWnd, HDM_GETITEMCOUNT, 0, 0); }
    BOOL GetItem(int nPos, HD_ITEM* pHeaderItem) { return (BOOL)::SendMessage(m_hWnd, HDM_GETITEM, nPos, (LPARAM)pHeaderItem); }
    BOOL SetItem(int nPos, HD_ITEM* pHeaderItem) { return (BOOL)::SendMessage(m_hWnd, HDM_SETITEM, nPos, (LPARAM)pHeaderItem); }
    int InsertItem(int nPos, HD_ITEM* pHeaderItem) { return (int)::SendMessage(m_hWnd, HDM_INSERTITEM, nPos, (LPARAM)pHeaderItem); }
    BOOL DeleteItem(int nPos) { return (BOOL)::SendMessage(m_hWnd, HDM_DELETEITEM, nPos, 0); }
    BOOL Layout(HD_LAYOUT* pHeaderLayout) { return (BOOL)::SendMessage(m_hWnd, HDM_LAYOUT, 0, (LPARAM)pHeaderLayout); }
    int OrderToIndex(int nOrder) const { return (int)::SendMessage(m_hWnd, HDM_ORDERTOINDEX, nOrder, 0); }
    CImageList* SetImageList(CImageList* pImageList)
        { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, HDM_SETIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }
    CImageList* GetImageList() const { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, HDM_GETIMAGELIST, 0, 0)); }
    BOOL GetItemRect(int nIndex, LPRECT lpRect, UINT nFlag) const
        { return (BOOL)::SendMessage(m_hWnd, HDM_GETITEMRECT, nIndex, MAKELPARAM(nFlag, (LPARAM)lpRect)); }
    int GetOrderArray(LPINT lpnArray, int nSize = -1) const
        { if (nSize < 0) nSize = GetItemCount(); return (int)::SendMessage(m_hWnd, HDM_GETORDERARRAY, nSize, (LPARAM)lpnArray); }
    BOOL SetOrderArray(int nSize, LPINT lpnArray) { return (BOOL)::SendMessage(m_hWnd, HDM_SETORDERARRAY, nSize, (LPARAM)lpnArray); }
    int HitTest(LPHDHITTESTINFO pInfo) const { return (int)::SendMessage(m_hWnd, HDM_HITTEST, 0, (LPARAM)pInfo); }
    BOOL SetHotDivider(CPoint pt) { return (BOOL)::SendMessage(m_hWnd, HDM_SETHOTDIVIDER, TRUE, MAKELPARAM(pt.x, pt.y)); }
    BOOL SetHotDivider(int nIndex) { return (BOOL)::SendMessage(m_hWnd, HDM_SETHOTDIVIDER, FALSE, nIndex); }
    int GetHotDivider() const { return (int)::SendMessage(m_hWnd, HDM_GETHOTDIVIDER, 0, 0); }
};

// ---------------------------------------------------------------------
//  CProgressCtrl
// ---------------------------------------------------------------------
class CProgressCtrl : public CWnd
{
    DECLARE_DYNAMIC(CProgressCtrl)
public:
    CProgressCtrl() {}
    virtual ~CProgressCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, PROGRESS_CLASS, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    int OffsetPos(int nPos) { return (int)::SendMessage(m_hWnd, PBM_DELTAPOS, nPos, 0); }
    int SetPos(int nPos) { return (int)::SendMessage(m_hWnd, PBM_SETPOS, nPos, 0); }
    int SetStep(int nStep) { return (int)::SendMessage(m_hWnd, PBM_SETSTEP, nStep, 0); }
    int StepIt() { return (int)::SendMessage(m_hWnd, PBM_STEPIT, 0, 0); }
    void SetRange(short nLower, short nUpper) { ::SendMessage(m_hWnd, PBM_SETRANGE, 0, MAKELPARAM(nLower, nUpper)); }
    void SetRange32(int nLower, int nUpper) { ::SendMessage(m_hWnd, PBM_SETRANGE32, nLower, nUpper); }
    void GetRange(int& nLower, int& nUpper) const
    {
        nLower = (int)::SendMessage(m_hWnd, PBM_GETRANGE, TRUE, 0);
        nUpper = (int)::SendMessage(m_hWnd, 0, 0, 0);
    }
    int GetPos() const { return (int)::SendMessage(m_hWnd, PBM_GETPOS, 0, 0); }
    int SetBarColor(COLORREF cr) { return (int)::SendMessage(m_hWnd, PBM_SETBARCOLOR, 0, cr); }
    COLORREF GetBarColor() const { return (COLORREF)::SendMessage(m_hWnd, PBM_GETBARCOLOR, 0, 0); }
    int SetBkColor(COLORREF cr) { return (int)::SendMessage(m_hWnd, PBM_SETBKCOLOR, 0, cr); }
    COLORREF GetBkColor() const { return (COLORREF)::SendMessage(m_hWnd, PBM_GETBKCOLOR, 0, 0); }
    BOOL SetMarquee(BOOL fMarqueeMode, UINT nInterval = 30)
        { return (BOOL)::SendMessage(m_hWnd, PBM_SETMARQUEE, fMarqueeMode, nInterval); }
    void SetState(int iState) { ::SendMessage(m_hWnd, PBM_SETSTATE, iState, 0); }
    int GetState() const { return (int)::SendMessage(m_hWnd, PBM_GETSTATE, 0, 0); }
};

// ---------------------------------------------------------------------
//  CSliderCtrl
// ---------------------------------------------------------------------
class CSliderCtrl : public CWnd
{
    DECLARE_DYNAMIC(CSliderCtrl)
public:
    CSliderCtrl() {}
    virtual ~CSliderCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, TRACKBAR_CLASS, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    int GetLineSize() const { return (int)::SendMessage(m_hWnd, TBM_GETLINESIZE, 0, 0); }
    int SetLineSize(int nSize) { return (int)::SendMessage(m_hWnd, TBM_SETLINESIZE, 0, nSize); }
    int GetPageSize() const { return (int)::SendMessage(m_hWnd, TBM_GETPAGESIZE, 0, 0); }
    int SetPageSize(int nSize) { return (int)::SendMessage(m_hWnd, TBM_SETPAGESIZE, 0, nSize); }
    int GetRangeMax() const { return (int)::SendMessage(m_hWnd, TBM_GETRANGEMAX, 0, 0); }
    int GetRangeMin() const { return (int)::SendMessage(m_hWnd, TBM_GETRANGEMIN, 0, 0); }
    void SetRangeMin(int nMin, BOOL bRedraw = FALSE) { ::SendMessage(m_hWnd, TBM_SETRANGEMIN, bRedraw, nMin); }
    void SetRangeMax(int nMax, BOOL bRedraw = FALSE) { ::SendMessage(m_hWnd, TBM_SETRANGEMAX, bRedraw, nMax); }
    void SetRange(int nMin, int nMax, BOOL bRedraw = FALSE) { ::SendMessage(m_hWnd, TBM_SETRANGE, bRedraw, MAKELPARAM(nMin, nMax)); }
    void ClearSel(BOOL bRedraw = FALSE) { ::SendMessage(m_hWnd, TBM_CLEARSEL, bRedraw, 0); }
    void GetChannelRect(LPRECT lprc) const { ::SendMessage(m_hWnd, TBM_GETCHANNELRECT, 0, (LPARAM)lprc); }
    void GetThumbRect(LPRECT lprc) const { ::SendMessage(m_hWnd, TBM_GETTHUMBRECT, 0, (LPARAM)lprc); }
    int GetPos() const { return (int)::SendMessage(m_hWnd, TBM_GETPOS, 0, 0); }
    void SetPos(int nPos) { ::SendMessage(m_hWnd, TBM_SETPOS, TRUE, nPos); }
    void ClearTics(BOOL bRedraw = FALSE) { ::SendMessage(m_hWnd, TBM_CLEARTICS, bRedraw, 0); }
    UINT GetNumTics() const { return (UINT)::SendMessage(m_hWnd, TBM_GETNUMTICS, 0, 0); }
    DWORD* GetTicArray() const { return (DWORD*)::SendMessage(m_hWnd, TBM_GETPTICS, 0, 0); }
    int GetTic(int nTic) const { return (int)::SendMessage(m_hWnd, TBM_GETTIC, nTic, 0); }
    int GetTicPos(int nTic) const { return (int)::SendMessage(m_hWnd, TBM_GETTICPOS, nTic, 0); }
    BOOL SetTic(int nTic) { ::SendMessage(m_hWnd, TBM_SETTIC, 0, nTic); return TRUE; }
    void SetTicFreq(int nFreq) { ::SendMessage(m_hWnd, TBM_SETTICFREQ, nFreq, 0); }
    int GetSelStart() const { return (int)::SendMessage(m_hWnd, TBM_GETSELSTART, 0, 0); }
    int GetSelEnd() const { return (int)::SendMessage(m_hWnd, TBM_GETSELEND, 0, 0); }
    void SetSelection(int nMin, int nMax) { SetSelStart(nMin); SetSelEnd(nMax); }
    void SetSelStart(int nMin) { ::SendMessage(m_hWnd, TBM_SETSELSTART, TRUE, nMin); }
    void SetSelEnd(int nMax) { ::SendMessage(m_hWnd, TBM_SETSELEND, TRUE, nMax); }
    void SetThumbLength(int cyThumb, BOOL bRedraw = TRUE) { ::SendMessage(m_hWnd, TBM_SETTHUMBLENGTH, cyThumb, bRedraw); }
    int GetThumbLength() const { return (int)::SendMessage(m_hWnd, TBM_GETTHUMBLENGTH, 0, 0); }
    void SetBuddy(CWnd* pWndLeft, BOOL bLeft = TRUE) { ::SendMessage(m_hWnd, TBM_SETBUDDY, bLeft, (LPARAM)pWndLeft->GetSafeHwnd()); }
    CWnd* GetBuddy(BOOL fLocation = TRUE) const { return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TBM_GETBUDDY, fLocation, 0)); }
    CToolTipCtrl* GetToolTips() const { return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TBM_GETTOOLTIPS, 0, 0)); }
    void SetToolTips(CWnd* pTipWnd) { ::SendMessage(m_hWnd, TBM_SETTOOLTIPS, (WPARAM)pTipWnd->GetSafeHwnd(), 0); }
    void SetTipSide(int nLocation) { ::SendMessage(m_hWnd, TBM_SETTIPSIDE, nLocation, 0); }
};

// ---------------------------------------------------------------------
//  CSpinButtonCtrl
// ---------------------------------------------------------------------
class CSpinButtonCtrl : public CWnd
{
    DECLARE_DYNAMIC(CSpinButtonCtrl)
public:
    CSpinButtonCtrl() {}
    virtual ~CSpinButtonCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, UPDOWN_CLASS, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    void SetAccel(UINT nAccel, UDACCEL* pAccel) { ::SendMessage(m_hWnd, UDM_SETACCEL, nAccel, (LPARAM)pAccel); }
    UINT GetAccel(UINT cAccels, UDACCEL* pAccels) const { return (UINT)::SendMessage(m_hWnd, UDM_GETACCEL, cAccels, (LPARAM)pAccels); }
    void SetBase(UINT nBase) { ::SendMessage(m_hWnd, UDM_SETBASE, nBase, 0); }
    UINT GetBase() const { return (UINT)::SendMessage(m_hWnd, UDM_GETBASE, 0, 0); }
    CWnd* SetBuddy(CWnd* pWndBuddy) { return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, UDM_SETBUDDY, (WPARAM)pWndBuddy->GetSafeHwnd(), 0)); }
    CWnd* GetBuddy(BOOL fWrap = TRUE) const { return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, UDM_GETBUDDY, fWrap, 0)); }
    int SetPos(int nPos) { return (short)LOWORD(::SendMessage(m_hWnd, UDM_SETPOS, 0, MAKELPARAM(nPos, 0))); }
    int GetPos() const { return (short)LOWORD(::SendMessage(m_hWnd, UDM_GETPOS, 0, 0)); }
    void SetRange(int nLower, int nUpper) { ::SendMessage(m_hWnd, UDM_SETRANGE, 0, MAKELPARAM(nUpper, nLower)); }
    void SetRange32(int nLower, int nUpper) { ::SendMessage(m_hWnd, UDM_SETRANGE32, nLower, nUpper); }
    void GetRange(int& lower, int& upper) const
    {
        DWORD r = (DWORD)::SendMessage(m_hWnd, UDM_GETRANGE32, 0, 0);
        lower = (int)(short)LOWORD(r); upper = (int)(short)HIWORD(r);
    }
    DWORD GetPos32(int* pError = NULL) const { return (DWORD)::SendMessage(m_hWnd, UDM_GETPOS32, 0, (LPARAM)pError); }
    DWORD SetPos32(int nPos) { return (DWORD)::SendMessage(m_hWnd, UDM_SETPOS32, 0, nPos); }
};

// ---------------------------------------------------------------------
//  CToolTipCtrl
// ---------------------------------------------------------------------
class CToolTipCtrl : public CWnd
{
    DECLARE_DYNAMIC(CToolTipCtrl)
public:
    CToolTipCtrl() {}
    virtual ~CToolTipCtrl() {}
    BOOL Create(CWnd* pParentWnd, DWORD dwStyle = 0)
    {
        return CreateEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
                        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | dwStyle,
                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, NULL);
    }
    BOOL AddTool(CWnd* pWnd, UINT nIDText, LPCRECT lpRectTool = NULL, UINT_PTR nIDTool = 0);
    BOOL AddTool(CWnd* pWnd, LPCTSTR lpszText = LPSTR_TEXTCALLBACK, LPCRECT lpRectTool = NULL, UINT_PTR nIDTool = 0);
    void DelTool(CWnd* pWnd, UINT_PTR nIDTool = 0);
    BOOL HitTest(CWnd* pWnd, CPoint point, LPTOOLINFO lpInfo) const;
    void GetText(CString& rText, CWnd* pWnd, UINT_PTR nIDTool = 0) const;
    BOOL GetToolInfo(LPTOOLINFO lpToolInfo, CWnd* pWnd, UINT_PTR nIDTool = 0) const
    {
        lpToolInfo->hwnd = pWnd ? pWnd->GetSafeHwnd() : NULL;
        lpToolInfo->uId = nIDTool;
        return (BOOL)::SendMessage(m_hWnd, TTM_GETTOOLINFO, 0, (LPARAM)lpToolInfo);
    }
    void SetToolInfo(LPTOOLINFO lpToolInfo) { ::SendMessage(m_hWnd, TTM_SETTOOLINFO, 0, (LPARAM)lpToolInfo); }
    void SetToolRect(LPCRECT lpRect, CWnd* pWnd, UINT_PTR nIDTool = 0);
    void RelayEvent(MSG* pMsg) { ::SendMessage(m_hWnd, TTM_RELAYEVENT, 0, (LPARAM)pMsg); }
    void UpdateTipText(LPCTSTR lpszText, CWnd* pWnd, UINT_PTR nIDTool = 0);
    void UpdateTipText(UINT nIDText, CWnd* pWnd, UINT_PTR nIDTool = 0);
    int GetToolCount() const { return (int)::SendMessage(m_hWnd, TTM_GETTOOLCOUNT, 0, 0); }
    void Activate(BOOL bActivate) { ::SendMessage(m_hWnd, TTM_ACTIVATE, bActivate, 0); }
    void Pop() { ::SendMessage(m_hWnd, TTM_POP, 0, 0); }
    void SetDelayTime(UINT nDelay) { ::SendMessage(m_hWnd, TTM_SETDELAYTIME, TTDT_INITIAL, nDelay); }
    int GetDelayTime(DWORD dwDuration) const { return (int)::SendMessage(m_hWnd, TTM_GETDELAYTIME, dwDuration, 0); }
    void SetMaxTipWidth(int nWidth) { ::SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, nWidth); }
    int GetMaxTipWidth() const { return (int)::SendMessage(m_hWnd, TTM_GETMAXTIPWIDTH, 0, 0); }
    void SetTipBkColor(COLORREF clr) { ::SendMessage(m_hWnd, TTM_SETTIPBKCOLOR, clr, 0); }
    void SetTipTextColor(COLORREF clr) { ::SendMessage(m_hWnd, TTM_SETTIPTEXTCOLOR, clr, 0); }
    COLORREF GetTipBkColor() const { return (COLORREF)::SendMessage(m_hWnd, TTM_GETTIPBKCOLOR, 0, 0); }
    COLORREF GetTipTextColor() const { return (COLORREF)::SendMessage(m_hWnd, TTM_GETTIPTEXTCOLOR, 0, 0); }
    BOOL GetCurrentTool(LPTOOLINFO lpToolInfo) const { return (BOOL)::SendMessage(m_hWnd, TTM_GETCURRENTTOOL, 0, (LPARAM)lpToolInfo); }
    void TrackActivate(LPTOOLINFO lpToolInfo, BOOL bActivate) { ::SendMessage(m_hWnd, TTM_TRACKACTIVATE, bActivate, (LPARAM)lpToolInfo); }
    void TrackPosition(int xPos, int yPos) { ::SendMessage(m_hWnd, TTM_TRACKPOSITION, 0, MAKELPARAM(xPos, yPos)); }
    void Update() { ::SendMessage(m_hWnd, TTM_UPDATE, 0, 0); }
};

// ---------------------------------------------------------------------
//  CStatusBarCtrl
// ---------------------------------------------------------------------
class CStatusBarCtrl : public CWnd
{
    DECLARE_DYNAMIC(CStatusBarCtrl)
public:
    CStatusBarCtrl() {}
    virtual ~CStatusBarCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, STATUSCLASSNAME, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    BOOL SetText(LPCTSTR lpszText, int nPane, int nType)
        { return (BOOL)::SendMessage(m_hWnd, SB_SETTEXT, nPane | nType, (LPARAM)lpszText); }
    int GetText(int nPane, LPTSTR lpszText, int nType) const
        { return (int)::SendMessage(m_hWnd, SB_GETTEXT, nPane | nType, (LPARAM)lpszText); }
    CString GetText(int nPane, int nType, int* pLen = NULL) const
    {
        int nLen = GetTextLength(nPane, NULL);
        if (pLen) *pLen = nLen;
        if (nLen == 0) return CString();
        CString s;
        LPTSTR p = s.GetBuffer(nLen + 1);
        GetText(nPane, p, nType);
        s.ReleaseBuffer();
        return s;
    }
    int GetTextLength(int nPane, int* pType = NULL) const
        { return (int)::SendMessage(m_hWnd, SB_GETTEXTLENGTH, nPane, (LPARAM)pType); }
    BOOL SetParts(int nParts, int* pWidths) { return (BOOL)::SendMessage(m_hWnd, SB_SETPARTS, nParts, (LPARAM)pWidths); }
    int GetParts(int nParts, int* pParts) const { return (int)::SendMessage(m_hWnd, SB_GETPARTS, nParts, (LPARAM)pParts); }
    BOOL GetBorders(int* pBorders) const { return (BOOL)::SendMessage(m_hWnd, SB_GETBORDERS, 0, (LPARAM)pBorders); }
    BOOL GetBorders(int& nHorz, int& nVert, int& nSpacing) const
    {
        int borders[3];
        BOOL b = GetBorders(borders);
        nHorz = borders[0]; nVert = borders[1]; nSpacing = borders[2];
        return b;
    }
    void SetMinHeight(int nMin) { ::SendMessage(m_hWnd, SB_SETMINHEIGHT, nMin, 0); }
    BOOL SetSimple(BOOL bSimple = TRUE) { return (BOOL)::SendMessage(m_hWnd, SB_SIMPLE, bSimple, 0); }
    BOOL IsSimple() const { return (BOOL)::SendMessage(m_hWnd, SB_ISSIMPLE, 0, 0); }
    BOOL SetTipText(LPCTSTR lpstrText) { return (BOOL)::SendMessage(m_hWnd, SB_SETTIPTEXT, 0, (LPARAM)lpstrText); }
    void GetRect(int nPane, LPRECT lpRect) const { ::SendMessage(m_hWnd, SB_GETRECT, nPane, (LPARAM)lpRect); }
    HICON SetIcon(int nPane, HICON hIcon) { return (HICON)::SendMessage(m_hWnd, SB_SETICON, nPane, (LPARAM)hIcon); }
    HICON GetIcon(int nPane) const { return (HICON)::SendMessage(m_hWnd, SB_GETICON, nPane, 0); }
    // status bar panes have no real "disabled" look: keep the text as it is
    void EnablePane(int nPaneIndex, BOOL bEnable) { (void)nPaneIndex; (void)bEnable; }
    void SetPaneText(int nPaneIndex, LPCTSTR lpszText) { SetText(lpszText, nPaneIndex, 0); }
};

// ---------------------------------------------------------------------
//  CToolBarCtrl
// ---------------------------------------------------------------------
class CToolBarCtrl : public CWnd
{
    DECLARE_DYNAMIC(CToolBarCtrl)
public:
    CToolBarCtrl() {}
    virtual ~CToolBarCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, TOOLBARCLASSNAME, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    BOOL IsButtonEnabled(int nID) const { return (BOOL)::SendMessage(m_hWnd, TB_ISBUTTONENABLED, nID, 0); }
    BOOL IsButtonChecked(int nID) const { return (BOOL)::SendMessage(m_hWnd, TB_ISBUTTONCHECKED, nID, 0); }
    BOOL IsButtonPressed(int nID) const { return (BOOL)::SendMessage(m_hWnd, TB_ISBUTTONPRESSED, nID, 0); }
    BOOL IsButtonHidden(int nID) const { return (BOOL)::SendMessage(m_hWnd, TB_ISBUTTONHIDDEN, nID, 0); }
    BOOL IsButtonIndeterminate(int nID) const { return (BOOL)::SendMessage(m_hWnd, TB_ISBUTTONINDETERMINATE, nID, 0); }
    UINT GetButtonState(int nID) const { return (UINT)::SendMessage(m_hWnd, TB_GETSTATE, nID, 0); }
    BOOL SetButtonState(int nID, UINT nState) { return (BOOL)::SendMessage(m_hWnd, TB_SETSTATE, nID, MAKELPARAM(nState, 0)); }
    BOOL SetCheck(int nID, BOOL bCheck = TRUE) { return SetButtonState(nID, bCheck ? TBSTATE_CHECKED : 0); }
    BOOL EnableButton(int nID, BOOL bEnable = TRUE)
        { return (BOOL)::SendMessage(m_hWnd, TB_ENABLEBUTTON, nID, MAKELPARAM(bEnable, 0)); }
    BOOL CheckButton(int nID, BOOL bCheck = TRUE)
        { return (BOOL)::SendMessage(m_hWnd, TB_CHECKBUTTON, nID, MAKELPARAM(bCheck, 0)); }
    BOOL PressButton(int nID, BOOL bPress = TRUE)
        { return (BOOL)::SendMessage(m_hWnd, TB_PRESSBUTTON, nID, MAKELPARAM(bPress, 0)); }
    BOOL HideButton(int nID, BOOL bHide = TRUE)
        { return (BOOL)::SendMessage(m_hWnd, TB_HIDEBUTTON, nID, MAKELPARAM(bHide, 0)); }
    BOOL Indeterminate(int nID, BOOL bIndeterminate = TRUE)
        { return (BOOL)::SendMessage(m_hWnd, TB_INDETERMINATE, nID, MAKELPARAM(bIndeterminate, 0)); }
    BOOL GetButton(int nIndex, LPTBBUTTON lpButton) const
        { return (BOOL)::SendMessage(m_hWnd, TB_GETBUTTON, nIndex, (LPARAM)lpButton); }
    int GetButtonCount() const { return (int)::SendMessage(m_hWnd, TB_BUTTONCOUNT, 0, 0); }
    BOOL GetItemRect(int nIndex, LPRECT lpRect) const
        { return (BOOL)::SendMessage(m_hWnd, TB_GETITEMRECT, nIndex, (LPARAM)lpRect); }
    void SetButtonStructSize(int nSize) { ::SendMessage(m_hWnd, TB_BUTTONSTRUCTSIZE, nSize, 0); }
    BOOL SetButtonSize(CSize size) { return (BOOL)::SendMessage(m_hWnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(size.cx, size.cy)); }
    BOOL SetBitmapSize(CSize size) { return (BOOL)::SendMessage(m_hWnd, TB_SETBITMAPSIZE, 0, MAKELPARAM(size.cx, size.cy)); }
    CImageList* GetImageList() const { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TB_GETIMAGELIST, 0, 0)); }
    CImageList* SetImageList(CImageList* pImageList)
        { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TB_SETIMAGELIST, 0, (LPARAM)(pImageList ? pImageList->GetSafeHandle() : NULL))); }
    CImageList* GetDisabledImageList() const { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TB_GETDISABLEDIMAGELIST, 0, 0)); }
    CImageList* SetDisabledImageList(CImageList* pImageList)
        { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)(pImageList ? pImageList->GetSafeHandle() : NULL))); }
    CImageList* GetHotImageList() const { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TB_GETHOTIMAGELIST, 0, 0)); }
    CImageList* SetHotImageList(CImageList* pImageList)
        { return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TB_SETHOTIMAGELIST, 0, (LPARAM)(pImageList ? pImageList->GetSafeHandle() : NULL))); }
    int AddBitmap(int nNumButtons, UINT nID)
        { return (int)::SendMessage(m_hWnd, TB_ADDBITMAP, nNumButtons, (LPARAM)&nID); }
    int AddBitmap(int nNumButtons, CBitmap* pBitmap)
    {
        TBADDBITMAP tbab; tbab.hInst = NULL; tbab.nID = (UINT_PTR)pBitmap->GetSafeHandle();
        return (int)::SendMessage(m_hWnd, TB_ADDBITMAP, nNumButtons, (LPARAM)&tbab);
    }
    BOOL AddButtons(int nNumButtons, LPTBBUTTON lpButtons)
        { return (BOOL)::SendMessage(m_hWnd, TB_ADDBUTTONS, nNumButtons, (LPARAM)lpButtons); }
    BOOL InsertButton(int nIndex, LPTBBUTTON lpButton)
        { return (BOOL)::SendMessage(m_hWnd, TB_INSERTBUTTON, nIndex, (LPARAM)lpButton); }
    BOOL DeleteButton(int nIndex) { return (BOOL)::SendMessage(m_hWnd, TB_DELETEBUTTON, nIndex, 0); }
    UINT GetButtonID(int nIndex) const
    {
        TBBUTTON tb; ZeroMemory(&tb, sizeof(tb));
        if (!::SendMessage(m_hWnd, TB_GETBUTTON, nIndex, (LPARAM)&tb)) return 0;
        return (UINT)tb.idCommand;
    }
    int CommandToIndex(UINT nID) const { return (int)::SendMessage(m_hWnd, TB_COMMANDTOINDEX, nID, 0); }
    BOOL SetRows(int nRows, BOOL bLarger, int* pnRows)
        { return (BOOL)::SendMessage(m_hWnd, TB_SETROWS, MAKEWPARAM(nRows, bLarger), (LPARAM)pnRows); }
    int GetRows() const { return (int)::SendMessage(m_hWnd, TB_GETROWS, 0, 0); }
    void SetCmdID(int nIndex, UINT nID) { ::SendMessage(m_hWnd, TB_SETCMDID, nIndex, nID); }
    UINT GetBitmapFlags() const { return (UINT)::SendMessage(m_hWnd, TB_GETBITMAPFLAGS, 0, 0); }
    CToolTipCtrl* GetToolTips() const { return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TB_GETTOOLTIPS, 0, 0)); }
    void SetToolTips(CToolTipCtrl* pTip) { ::SendMessage(m_hWnd, TB_SETTOOLTIPS, (WPARAM)(pTip ? pTip->GetSafeHwnd() : NULL), 0); }
    CWnd* SetOwner(CWnd* pWnd) { return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TB_SETOWNER, (WPARAM)(pWnd ? pWnd->GetSafeHwnd() : NULL), 0)); }
    CWnd* GetOwner() const { return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TB_GETOWNER, 0, 0)); }
    void GetButtonText(int nIndex, CString& rString) const
    {
        TCHAR buf[256];
        int n = (int)::SendMessage(m_hWnd, TB_GETBUTTONTEXT, nIndex, (LPARAM)buf);
        rString = (n > 0) ? CString(buf, n) : CString();
    }
    void SetButtonText(int nIndex, LPCTSTR lpszString);
    void SetButtonWidth(int cxMin, int cxMax) { ::SendMessage(m_hWnd, TB_SETBUTTONWIDTH, 0, MAKELPARAM(cxMin, cxMax)); }
    DWORD GetStyle() const { return (DWORD)::SendMessage(m_hWnd, TB_GETSTYLE, 0, 0); }
    void SetStyle(DWORD dwStyle) { ::SendMessage(m_hWnd, TB_SETSTYLE, 0, dwStyle); }
    DWORD GetButtonStyle(int nIndex) const;
    void SetButtonStyle(int nIndex, DWORD dwStyle);
    void GetButtonInfo(int nIndex, TBBUTTONINFO* ptbbi) const { ::SendMessage(m_hWnd, TB_GETBUTTONINFO, nIndex, (LPARAM)ptbbi); }
    BOOL SetButtonInfo(int nIndex, TBBUTTONINFO* ptbbi) { return (BOOL)::SendMessage(m_hWnd, TB_SETBUTTONINFO, nIndex, (LPARAM)ptbbi); }
    BOOL SetMaxTextRows(int nMax) { return (BOOL)::SendMessage(m_hWnd, TB_SETMAXTEXTROWS, nMax, 0); }
    int GetMaxTextRows() const { return (int)::SendMessage(m_hWnd, TB_GETTEXTROWS, 0, 0); }
    BOOL SetIndent(int nIndent) { return (BOOL)::SendMessage(m_hWnd, TB_SETINDENT, nIndent, 0); }
    DWORD GetExtendedStyle() const { return (DWORD)::SendMessage(m_hWnd, TB_GETEXTENDEDSTYLE, 0, 0); }
    DWORD SetExtendedStyle(DWORD dwExStyle) { return (DWORD)::SendMessage(m_hWnd, TB_SETEXTENDEDSTYLE, 0, dwExStyle); }
    BOOL GetHotItem(int& nIndex) const { nIndex = (int)::SendMessage(m_hWnd, TB_GETHOTITEM, 0, 0); return nIndex != -1; }
    BOOL SetHotItem(int nIndex) { return (int)::SendMessage(m_hWnd, TB_SETHOTITEM, nIndex, 0) != -1; }
    void Customize() { ::SendMessage(m_hWnd, TB_CUSTOMIZE, 0, 0); }
    void AutoSize() { ::SendMessage(m_hWnd, TB_AUTOSIZE, 0, 0); }
    int HitTest(LPPOINT ppt) const { return (int)::SendMessage(m_hWnd, TB_HITTEST, 0, (LPARAM)ppt); }
    BOOL MapAccelerator(TCHAR chAccel, UINT* pIDBtn) const
        { return (BOOL)::SendMessage(m_hWnd, TB_MAPACCELERATOR, (WPARAM)chAccel, (LPARAM)pIDBtn); }
    BOOL MarkButton(int nID, BOOL fHighlight = TRUE)
        { return (BOOL)::SendMessage(m_hWnd, TB_MARKBUTTON, nID, MAKELPARAM(fHighlight, 0)); }
    BOOL MoveButton(UINT nOldPos, UINT nNewPos) { return (BOOL)::SendMessage(m_hWnd, TB_MOVEBUTTON, nOldPos, nNewPos); }
    BOOL GetMaxSize(LPSIZE pSize) const { return (BOOL)::SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)pSize); }
    void GetPadding(LPSIZE psizePadding) const
    {
        DWORD dw = (DWORD)::SendMessage(m_hWnd, TB_GETPADDING, 0, 0);
        psizePadding->cx = (short)LOWORD(dw); psizePadding->cy = (short)HIWORD(dw);
    }
    void SetPadding(int cx, int cy) { ::SendMessage(m_hWnd, TB_SETPADDING, 0, MAKELPARAM(cx, cy)); }
    void SetListGap(int nListGap) { ::SendMessage(m_hWnd, TB_SETLISTGAP, nListGap, 0); }
    int GetListGap() const { return (int)::SendMessage(m_hWnd, TB_GETLISTGAP, 0, 0); }
};

// ---------------------------------------------------------------------
//  CRichEditCtrl  (riched20 / msftedit based)
// ---------------------------------------------------------------------
class CRichEditCtrl : public CWnd
{
    DECLARE_DYNAMIC(CRichEditCtrl)
public:
    CRichEditCtrl() {}
    virtual ~CRichEditCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

    BOOL CanUndo() const { return (BOOL)::SendMessage(m_hWnd, EM_CANUNDO, 0, 0); }
    int GetLineCount() const { return (int)::SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0); }
    BOOL GetModify() const { return (BOOL)::SendMessage(m_hWnd, EM_GETMODIFY, 0, 0); }
    void SetModify(BOOL bModified = TRUE) { ::SendMessage(m_hWnd, EM_SETMODIFY, bModified, 0); }
    void GetRect(LPRECT lpRect) const { ::SendMessage(m_hWnd, EM_GETRECT, 0, (LPARAM)lpRect); }
    void GetSel(long& nStartChar, long& nEndChar) const { ::SendMessage(m_hWnd, EM_EXGETSEL, 0, (LPARAM)&nStartChar); nEndChar = nStartChar; }
    void GetSel(CHARRANGE& cr) const { ::SendMessage(m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr); }
    void SetSel(long nStartChar, long nEndChar) { CHARRANGE cr = { nStartChar, nEndChar }; ::SendMessage(m_hWnd, EM_EXSETSEL, 0, (LPARAM)&cr); }
    void SetSel(CHARRANGE& cr) { ::SendMessage(m_hWnd, EM_EXSETSEL, 0, (LPARAM)&cr); }
    void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE) { ::SendMessage(m_hWnd, EM_REPLACESEL, bCanUndo, (LPARAM)lpszNewText); }
    void SetReadOnly(BOOL bReadOnly = TRUE) { ::SendMessage(m_hWnd, EM_SETREADONLY, bReadOnly, 0); }
    int GetFirstVisibleLine() const { return (int)::SendMessage(m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0); }
    void EmptyUndoBuffer() { ::SendMessage(m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0); }
    void LimitText(long nChars = 0) { ::SendMessage(m_hWnd, EM_EXLIMITTEXT, 0, nChars); }
    long LineFromChar(long nIndex = -1) const { return (long)::SendMessage(m_hWnd, EM_EXLINEFROMCHAR, 0, nIndex); }
    int LineIndex(int nLine = -1) const { return (int)::SendMessage(m_hWnd, EM_LINEINDEX, nLine, 0); }
    int LineLength(int nLine = -1) const { return (int)::SendMessage(m_hWnd, EM_LINELENGTH, nLine, 0); }
    void LineScroll(int nLines, int nChars = 0) { ::SendMessage(m_hWnd, EM_LINESCROLL, nChars, nLines); }
    CPoint PosFromChar(UINT nChar) const { POINTL ptl; ::SendMessage(m_hWnd, EM_POSFROMCHAR, (WPARAM)&ptl, nChar); return CPoint(ptl.x, ptl.y); }
    int CharFromPos(CPoint pt) const { POINTL ptl = { pt.x, pt.y }; return (int)::SendMessage(m_hWnd, EM_CHARFROMPOS, 0, (LPARAM)&ptl); }
    long GetTextLength() const { return (long)::SendMessage(m_hWnd, WM_GETTEXTLENGTH, 0, 0); }
    long GetEventMask() const { return (long)::SendMessage(m_hWnd, EM_GETEVENTMASK, 0, 0); }
    long SetEventMask(long dwEventMask) { return (long)::SendMessage(m_hWnd, EM_SETEVENTMASK, 0, dwEventMask); }
    COLORREF SetBackgroundColor(COLORREF cr) { return (COLORREF)::SendMessage(m_hWnd, EM_SETBKGNDCOLOR, 0, cr); }
    COLORREF GetBackgroundColor() const { return (COLORREF)::SendMessage(m_hWnd, EM_SETBKGNDCOLOR, 1, 0); }
    DWORD GetDefaultCharFormat(CHARFORMAT& cf) const { return (DWORD)::SendMessage(m_hWnd, EM_GETCHARFORMAT, 0, (LPARAM)&cf); }
    BOOL SetDefaultCharFormat(CHARFORMAT& cf) { return (BOOL)::SendMessage(m_hWnd, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf); }
    DWORD GetSelectionCharFormat(CHARFORMAT& cf) const { return (DWORD)::SendMessage(m_hWnd, EM_GETCHARFORMAT, 1, (LPARAM)&cf); }
    BOOL SetSelectionCharFormat(CHARFORMAT& cf) { return (BOOL)::SendMessage(m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf); }
    BOOL SetWordCharFormat(CHARFORMAT& cf) { return (BOOL)::SendMessage(m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf); }
    DWORD GetParaFormat(PARAFORMAT& pf) const { return (DWORD)::SendMessage(m_hWnd, EM_GETPARAFORMAT, 0, (LPARAM)&pf); }
    BOOL SetParaFormat(PARAFORMAT& pf) { return (BOOL)::SendMessage(m_hWnd, EM_SETPARAFORMAT, 0, (LPARAM)&pf); }
    long StreamIn(int nFormat, EDITSTREAM& es) { return (long)::SendMessage(m_hWnd, EM_STREAMIN, nFormat, (LPARAM)&es); }
    long StreamOut(int nFormat, EDITSTREAM& es) { return (long)::SendMessage(m_hWnd, EM_STREAMOUT, nFormat, (LPARAM)&es); }
    BOOL Undo() { return (BOOL)::SendMessage(m_hWnd, EM_UNDO, 0, 0); }
    void Redo() { ::SendMessage(m_hWnd, EM_REDO, 0, 0); }
    BOOL Clear() { ::SendMessage(m_hWnd, WM_CLEAR, 0, 0); return TRUE; }
    BOOL Cut()   { ::SendMessage(m_hWnd, WM_CUT, 0, 0); return TRUE; }
    BOOL Copy()  { ::SendMessage(m_hWnd, WM_COPY, 0, 0); return TRUE; }
    BOOL Paste() { ::SendMessage(m_hWnd, WM_PASTE, 0, 0); return TRUE; }
    void HideSelection(BOOL bHide, BOOL bChangeStyle) { ::SendMessage(m_hWnd, EM_HIDESELECTION, bHide, bChangeStyle); }
    void RequestResize() { ::SendMessage(m_hWnd, EM_REQUESTRESIZE, 0, 0); }
    long FindText(DWORD dwFlags, FINDTEXT* pFindText) const { return (long)::SendMessage(m_hWnd, EM_FINDTEXT, dwFlags, (LPARAM)pFindText); }
    long FindTextEx(DWORD dwFlags, FINDTEXTEX* pFindText) const { return (long)::SendMessage(m_hWnd, EM_FINDTEXTEX, dwFlags, (LPARAM)pFindText); }
    long FormatRange(FORMATRANGE* pfr, BOOL bDisplay = TRUE) { return (long)::SendMessage(m_hWnd, EM_FORMATRANGE, bDisplay, (LPARAM)pfr); }
    void SetAutoURLDetect(BOOL bAutoDetect = TRUE) { ::SendMessage(m_hWnd, EM_AUTOURLDETECT, bAutoDetect, 0); }
    BOOL GetAutoURLDetect() const { return (BOOL)::SendMessage(m_hWnd, EM_GETAUTOURLDETECT, 0, 0); }
    int SetUndoLimit(int nUndoLimit) { return (int)::SendMessage(m_hWnd, EM_SETUNDOLIMIT, nUndoLimit, 0); }
    BOOL GetZoom(int& nNumerator, int& nDenominator) const { return (BOOL)::SendMessage(m_hWnd, EM_GETZOOM, (WPARAM)&nNumerator, (LPARAM)&nDenominator); }
    BOOL SetZoom(int nNumerator, int nDenominator) { return (BOOL)::SendMessage(m_hWnd, EM_SETZOOM, nNumerator, nDenominator); }
    BOOL SetZoomOff() { return (BOOL)::SendMessage(m_hWnd, EM_SETZOOM, 0, 0); }
    void ShowScrollBar(int nBarType, BOOL bShow = TRUE) { ::SendMessage(m_hWnd, EM_SHOWSCROLLBAR, nBarType, bShow); }
    void GetTextRange(long nFirst, long nLast, CString& refString) const;
    void GetSelText(CString& refString) const;
    BOOL SetTargetDevice(HDC hDC, long lLineWidth) { return (BOOL)::SendMessage(m_hWnd, EM_SETTARGETDEVICE, (WPARAM)hDC, lLineWidth); }
    BOOL SetTargetDevice(CDC& dc, long lLineWidth) { return SetTargetDevice((HDC)dc, lLineWidth); }
};

// ---------------------------------------------------------------------
//  CDateTimeCtrl / CMonthCalCtrl / CHotKeyCtrl / CIPAddressCtrl
// ---------------------------------------------------------------------
class CDateTimeCtrl : public CWnd
{
    DECLARE_DYNAMIC(CDateTimeCtrl)
public:
    CDateTimeCtrl() {}
    virtual ~CDateTimeCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, DATETIMEPICK_CLASS, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    DWORD GetRange(CTime* pMinRange, CTime* pMaxRange) const;
    BOOL SetRange(const CTime* pMinRange, const CTime* pMaxRange);
    DWORD GetTime(CTime& timeDest) const;
    BOOL SetTime(const CTime* pTimeNew);
    BOOL SetTime(DWORD dwFlags, const CTime* pTimeNew = NULL);
    CWnd* GetMonthCalCtrl() const { return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, DTM_GETMONTHCAL, 0, 0)); }
    CFont* GetMonthCalFont() const;
    COLORREF SetMonthCalColor(int iColor, COLORREF ref) { return (COLORREF)::SendMessage(m_hWnd, DTM_SETMCCOLOR, iColor, ref); }
    COLORREF GetMonthCalColor(int iColor) const { return (COLORREF)::SendMessage(m_hWnd, DTM_GETMCCOLOR, iColor, 0); }
    DWORD SetFormat(LPCTSTR pstrFormat) { return (DWORD)::SendMessage(m_hWnd, DTM_SETFORMAT, 0, (LPARAM)pstrFormat); }
};

class CMonthCalCtrl : public CWnd
{
    DECLARE_DYNAMIC(CMonthCalCtrl)
public:
    CMonthCalCtrl() {}
    virtual ~CMonthCalCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, MONTHCAL_CLASS, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    COLORREF GetColor(int nRegion) const { return (COLORREF)::SendMessage(m_hWnd, MCM_GETCOLOR, nRegion, 0); }
    COLORREF SetColor(int nRegion, COLORREF ref) { return (COLORREF)::SendMessage(m_hWnd, MCM_SETCOLOR, nRegion, ref); }
    BOOL GetCurSel(CTime& refTime) const;
    BOOL SetCurSel(const CTime& refTime);
    int GetFirstDOW(int* pnDOW = NULL) const
    {
        int n = (int)::SendMessage(m_hWnd, MCM_GETFIRSTDOW, 0, 0);
        if (pnDOW) *pnDOW = n;
        return n;
    }
    int SetFirstDOW(int nDOW) { return (int)::SendMessage(m_hWnd, MCM_SETFIRSTDOW, 0, nDOW); }
    DWORD GetMaxSelCount() const { return (DWORD)::SendMessage(m_hWnd, MCM_GETMAXSELCOUNT, 0, 0); }
    BOOL SetMaxSelCount(DWORD nMax) { return (BOOL)::SendMessage(m_hWnd, MCM_SETMAXSELCOUNT, 0, nMax); }
    int GetMonthRange(CTime& refMinRange, CTime& refMaxRange) const;
    BOOL SetMonthRange(DWORD dwFlags, const CTime& refMinRange, const CTime& refMaxRange);
    BOOL SetDayState(int nMonths, LPMONTHDAYSTATE pDayStateArray)
        { return (BOOL)::SendMessage(m_hWnd, MCM_SETDAYSTATE, nMonths, (LPARAM)pDayStateArray); }
    DWORD HitTest(MCHITTESTINFO* pMCHITTESTINFO) { return (DWORD)::SendMessage(m_hWnd, MCM_HITTEST, 0, (LPARAM)pMCHITTESTINFO); }
    BOOL SetSelRange(const CTime& pMinRange, const CTime& pMaxRange);
    BOOL GetSelRange(CTime& refMinRange, CTime& refMaxRange) const;
    BOOL GetToday(CTime& refTime) const;
    void GetToday(CTime* lpTimeInstance) const;
    void SetToday(const CTime& refTime);
    BOOL GetMinReqRect(LPRECT pRect) const { return (BOOL)::SendMessage(m_hWnd, MCM_GETMINREQRECT, 0, (LPARAM)pRect); }
    int GetMonthDelta() const { return (int)::SendMessage(m_hWnd, MCM_GETMONTHDELTA, 0, 0); }
    int SetMonthDelta(int iDelta) { return (int)::SendMessage(m_hWnd, MCM_SETMONTHDELTA, iDelta, 0); }
};

class CHotKeyCtrl : public CWnd
{
    DECLARE_DYNAMIC(CHotKeyCtrl)
public:
    CHotKeyCtrl() {}
    virtual ~CHotKeyCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, HOTKEY_CLASS, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    DWORD GetHotKey() const { return (DWORD)::SendMessage(m_hWnd, HKM_GETHOTKEY, 0, 0); }
    void GetHotKey(WORD& wVirtualKeyCode, WORD& wModifiers) const
    {
        DWORD dw = GetHotKey();
        wVirtualKeyCode = LOWORD(dw); wModifiers = HIWORD(dw);
    }
    void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers) { ::SendMessage(m_hWnd, HKM_SETHOTKEY, MAKEWORD(wVirtualKeyCode, wModifiers), 0); }
    void SetRules(WORD wInvalidComb, WORD wModifiers) { ::SendMessage(m_hWnd, HKM_SETRULES, wInvalidComb, MAKELPARAM(wModifiers, 0)); }
};

class CIPAddressCtrl : public CWnd
{
    DECLARE_DYNAMIC(CIPAddressCtrl)
public:
    CIPAddressCtrl() {}
    virtual ~CIPAddressCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, WC_IPADDRESS, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    BOOL IsBlank() const { return (BOOL)::SendMessage(m_hWnd, IPM_ISBLANK, 0, 0); }
    void ClearAddress() { ::SendMessage(m_hWnd, IPM_CLEARADDRESS, 0, 0); }
    int GetAddress(BYTE& nField0, BYTE& nField1, BYTE& nField2, BYTE& nField3) const
    {
        DWORD dw = 0;
        int n = (int)::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM)&dw);
        nField0 = (BYTE)FIRST_IPADDRESS(dw); nField1 = (BYTE)SECOND_IPADDRESS(dw);
        nField2 = (BYTE)THIRD_IPADDRESS(dw);  nField3 = (BYTE)FOURTH_IPADDRESS(dw);
        return n;
    }
    int GetAddress(DWORD& dwAddress) const { return (int)::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM)&dwAddress); }
    void SetAddress(DWORD dwAddress) { ::SendMessage(m_hWnd, IPM_SETADDRESS, 0, dwAddress); }
    void SetAddress(BYTE nField0, BYTE nField1, BYTE nField2, BYTE nField3)
        { SetAddress(MAKEIPADDRESS(nField0, nField1, nField2, nField3)); }
    void SetFieldFocus(WORD wField) { ::SendMessage(m_hWnd, IPM_SETFOCUS, wField, 0); }
    void SetFieldRange(int nField, BYTE nLower, BYTE nUpper)
        { ::SendMessage(m_hWnd, IPM_SETRANGE, nField, MAKEIPRANGE(nLower, nUpper)); }
};

class CReBarCtrl : public CWnd
{
    DECLARE_DYNAMIC(CReBarCtrl)
public:
    CReBarCtrl() {}
    virtual ~CReBarCtrl() {}
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
    {
        return CreateEx(0, REBARCLASSNAME, NULL, dwStyle, rect.left, rect.top,
                        rect.right - rect.left, rect.bottom - rect.top,
                        pParentWnd ? pParentWnd->GetSafeHwnd() : NULL, (HMENU)(UINT_PTR)nID);
    }
    UINT GetBandCount() const { return (UINT)::SendMessage(m_hWnd, RB_GETBANDCOUNT, 0, 0); }
    BOOL GetBandInfo(UINT uBand, REBARBANDINFO* prbbi) const { return (BOOL)::SendMessage(m_hWnd, RB_GETBANDINFO, uBand, (LPARAM)prbbi); }
    BOOL SetBandInfo(UINT uBand, REBARBANDINFO* prbbi) { return (BOOL)::SendMessage(m_hWnd, RB_SETBANDINFO, uBand, (LPARAM)prbbi); }
    UINT GetRowCount() const { return (UINT)::SendMessage(m_hWnd, RB_GETROWCOUNT, 0, 0); }
    UINT GetRowHeight(UINT uRow) const { return (UINT)::SendMessage(m_hWnd, RB_GETROWHEIGHT, uRow, 0); }
    UINT IDToIndex(UINT uBandID) const { return (UINT)::SendMessage(m_hWnd, RB_IDTOINDEX, uBandID, 0); }
    CWnd* GetToolTips() const { return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, RB_GETTOOLTIPS, 0, 0)); }
    void SetToolTips(CWnd* pTip) { ::SendMessage(m_hWnd, RB_SETTOOLTIPS, (WPARAM)(pTip ? pTip->GetSafeHwnd() : NULL), 0); }
    BOOL InsertBand(UINT uIndex, REBARBANDINFO* prbbi) { return (BOOL)::SendMessage(m_hWnd, RB_INSERTBAND, uIndex, (LPARAM)prbbi); }
    BOOL DeleteBand(UINT uIndex) { return (BOOL)::SendMessage(m_hWnd, RB_DELETEBAND, uIndex, 0); }
    void BeginDrag(UINT uBand, DWORD dwPos = (DWORD)-1) { ::SendMessage(m_hWnd, RB_BEGINDRAG, uBand, dwPos); }
    void EndDrag() { ::SendMessage(m_hWnd, RB_ENDDRAG, 0, 0); }
    void DragMove(DWORD dwPos = (DWORD)-1) { ::SendMessage(m_hWnd, RB_DRAGMOVE, 0, dwPos); }
    void GetBarInfo(REBARINFO* prbi) const { ::SendMessage(m_hWnd, RB_GETBARINFO, 0, (LPARAM)prbi); }
    void SetBarInfo(REBARINFO* prbi) { ::SendMessage(m_hWnd, RB_SETBARINFO, 0, (LPARAM)prbi); }
    BOOL MoveBand(UINT uFrom, UINT uTo) { return (BOOL)::SendMessage(m_hWnd, RB_MOVEBAND, uFrom, uTo); }
    void MaximizeBand(UINT uBand, BOOL fIdeal = TRUE) { ::SendMessage(m_hWnd, RB_MAXIMIZEBAND, uBand, fIdeal); }
    void MinimizeBand(UINT uBand) { ::SendMessage(m_hWnd, RB_MINIMIZEBAND, uBand, 0); }
    void ShowBand(UINT uBand, BOOL fShow = TRUE) { ::SendMessage(m_hWnd, RB_SHOWBAND, uBand, fShow); }
    void GetRect(UINT uBand, LPRECT prc) const { ::SendMessage(m_hWnd, RB_GETRECT, uBand, (LPARAM)prc); }
    void SetOwner(CWnd* pWnd) { ::SendMessage(m_hWnd, RB_SETPARENT, (WPARAM)(pWnd ? pWnd->GetSafeHwnd() : NULL), 0); }
    CWnd* GetOwner() const { return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, RB_GETPARENT, 0, 0)); }
    void SetPalette(HPALETTE hPal) { ::SendMessage(m_hWnd, RB_SETPALETTE, 0, (LPARAM)hPal); }
    HPALETTE GetPalette() const { return (HPALETTE)::SendMessage(m_hWnd, RB_GETPALETTE, 0, 0); }
    BOOL HitTest(RBHITTESTINFO* prbht) { return (BOOL)::SendMessage(m_hWnd, RB_HITTEST, 0, (LPARAM)prbht); }
    void GetBandBorders(UINT uBand, LPRECT prc) const { ::SendMessage(m_hWnd, RB_GETBANDBORDERS, uBand, (LPARAM)prc); }
    COLORREF GetTextColor() const { return (COLORREF)::SendMessage(m_hWnd, RB_GETTEXTCOLOR, 0, 0); }
    COLORREF SetTextColor(COLORREF clr) { return (COLORREF)::SendMessage(m_hWnd, RB_SETTEXTCOLOR, 0, clr); }
    COLORREF GetBkColor() const { return (COLORREF)::SendMessage(m_hWnd, RB_GETBKCOLOR, 0, 0); }
    COLORREF SetBkColor(COLORREF clr) { return (COLORREF)::SendMessage(m_hWnd, RB_SETBKCOLOR, 0, clr); }
};

// ---------------------------------------------------------------------
//  Misc helpers (implemented in mfc.cpp)
// ---------------------------------------------------------------------
void AFXAPI AfxInitRichEdit();         // loads riched20.dll  (RichEdit 2.0/3.0)
void AFXAPI AfxInitRichEdit2();        // loads msftedit.dll  (RichEdit 4.1)
BOOL AFXAPI AfxInitCommonControls();   // InitCommonControlsEx with everything
void AFXAPI AfxEnableVisualStyles();   // activate comctl32 v6 themes
void AFXAPI AfxSetWindowTheme(HWND hWnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
void AFXAPI AfxEnableDPIServiceAwareness();

#endif /* __AFXCMN_H__ */
