// =====================================================================
//  afxmsg.h  --  Message map machinery for the MFC-GCC clone.
//
//  Real MFC stores member-function pointers in the message map and casts
//  them between unrelated classes.  That is undefined behaviour which only
//  happens to work on the MSVC ABI, so this clone uses type-safe
//  *template thunks*: every ON_xxx macro instantiates a tiny static
//  trampoline with the exact handler signature.  All thunks share one
//  uniform prototype, so the dispatcher needs no casts at all.
//
//  The public syntax is 100% MFC compatible:
//
//      class CMyWnd : public CWnd {
//          DECLARE_MESSAGE_MAP()
//          afx_msg void OnPaint();
//          afx_msg void OnFileOpen();
//          afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
//      };
//      BEGIN_MESSAGE_MAP(CMyWnd, CWnd)
//          ON_WM_PAINT()
//          ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
//          ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
//      END_MESSAGE_MAP()
// =====================================================================
#ifndef __AFXMSG_H__
#define __AFXMSG_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif

class CCmdTarget;
class CWnd;
class CMenu;
class CDC;
class CPoint;
class CSize;
class CListBox;
struct CCmdUI;

// Helpers so that the thunks below do not need CWnd / CMenu / CDC to be
// complete when this header is parsed (they are defined in afxwnd.cpp).
CWnd* AFXAPI AfxMsgFromHandleWnd(HWND hWnd);
CMenu* AFXAPI AfxMsgFromHandleMenu(HMENU hMenu);
CDC* AFXAPI AfxMsgFromHandleDC(HDC hDC);
CListBox* AFXAPI AfxMsgFromHandleListBox(HWND hWnd);
HWND AFXAPI AfxMsgSafeHwnd(CCmdTarget* pTarget);

// Uniform prototype shared by every message-map thunk.
typedef LRESULT (*AFX_THUNK)(CCmdTarget* pTarget, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Signature ids (used by the dispatcher and for diagnostics).
enum AfxSigId
{
    AfxSig_end = 0,
    AfxSig_vv,        // void f()
    AfxSig_bv,        // BOOL f()
    AfxSig_uv,        // UINT f()
    AfxSig_hcv,       // HCURSOR f()
    AfxSig_vw,        // void f(UINT)
    AfxSig_vi,        // void f(int)
    AfxSig_vb,        // void f(BOOL)
    AfxSig_bb,        // BOOL f(BOOL)
    AfxSig_vT,        // void f(LPCTSTR)
    AfxSig_vup,       // void f(UINT_PTR)
    AfxSig_vwww,      // void f(UINT, UINT, UINT)
    AfxSig_vwwPt,     // void f(UINT, UINT, CPoint)
    AfxSig_vwPt,      // void f(UINT, CPoint)
    AfxSig_vwSz,      // void f(UINT, CSize)
    AfxSig_vwWb,      // void f(UINT, CWnd*, BOOL)
    AfxSig_vW,        // void f(CWnd*)
    AfxSig_vWww,      // void f(CWnd*, UINT, UINT)
    AfxSig_vwwW,      // void f(UINT, UINT, CWnd*)
    AfxSig_vwwH,      // void f(UINT, UINT, HMENU)
    AfxSig_vbw,       // void f(BOOL, UINT)
    AfxSig_vbd,       // void f(BOOL, DWORD)
    AfxSig_vwd,       // void f(UINT, DWORD)
    AfxSig_vul,       // void f(UINT, LONG)
    AfxSig_vxy,       // void f(int, int)
    AfxSig_vwii,      // void f(UINT, int, int)
    AfxSig_vPt,       // void f(CPoint)
    AfxSig_vWPt,      // void f(CWnd*, CPoint)
    AfxSig_vMMI,      // void f(MINMAXINFO*)
    AfxSig_vWP,       // void f(WINDOWPOS*)
    AfxSig_vHDROP,    // void f(HDROP)
    AfxSig_vRect,     // void f(LPRECT)
    AfxSig_vwRect,    // void f(UINT, LPRECT)
    AfxSig_vwStyle,   // void f(UINT, STYLESTRUCT*)
    AfxSig_vwT,       // void f(UINT, LPCTSTR)
    AfxSig_viMIS,     // void f(int, LPMEASUREITEMSTRUCT)
    AfxSig_viDIS,     // void f(int, LPDRAWITEMSTRUCT)
    AfxSig_vMwb,      // void f(CMenu*, UINT, BOOL)
    AfxSig_iCreate,   // int  f(LPCREATESTRUCT)
    AfxSig_iWww,      // int  f(CWnd*, UINT, UINT)
    AfxSig_iwLw,      // int  f(UINT, CListBox*, UINT)
    AfxSig_lPt,       // LRESULT f(CPoint)
    AfxSig_lwl,       // LRESULT f(WPARAM, LPARAM)
    AfxSig_lbwl,      // LRESULT f(BOOL, LPARAM)
    AfxSig_lwwM,      // LRESULT f(UINT, UINT, CMenu*)
    AfxSig_bwl,       // BOOL f(WPARAM, LPARAM)
    AfxSig_bDC,       // BOOL f(CDC*)
    AfxSig_bWww,      // BOOL f(CWnd*, UINT, UINT)
    AfxSig_bwSpt,     // BOOL f(UINT, short, CPoint)
    AfxSig_bWCD,      // BOOL f(CWnd*, COPYDATASTRUCT*)
    AfxSig_bHelp,     // BOOL f(HELPINFO*)
    AfxSig_hDCWw,     // HBRUSH f(CDC*, CWnd*, UINT)
    AfxSig_cmd,       // void f(CCmdUI*)
    AfxSig_vnId,      // void f(UINT)          -- command / control range
    AfxSig_bwId,      // BOOL f(UINT)          -- ON_COMMAND_EX
    AfxSig_notify,    // void f(NMHDR*, LRESULT*)
    AfxSig_notifyEx   // BOOL f(UINT, NMHDR*, LRESULT*)
};

struct AFX_MSGMAP_ENTRY
{
    UINT      nMessage;   // window message / WM_COMMAND / WM_NOTIFY, 0 == end of table
    UINT      nCode;      // control code or CN_COMMAND / CN_UPDATE_COMMAND_UI
    UINT      nID;        // control id or command id
    UINT      nLastID;    // end of an id range (0 == not a range)
    UINT      nSig;       // AfxSigId
    AFX_THUNK pfn;        // type-safe thunk
    UINT_PTR  nExtra;     // e.g. address of a registered-message variable
};

struct AFX_MSGMAP
{
    const AFX_MSGMAP*       lpBase;
    const AFX_MSGMAP_ENTRY* lpEntries;
};

struct AFX_CMDHANDLERINFO
{
    void* pmfTarget;      // unused, kept for source compatibility
    UINT  nID;
    int   nCode;
};

// MFC's generic colour message (winuser.h only has the WM_CTLCOLORxxx variants)
#ifndef WM_CTLCOLOR
#define WM_CTLCOLOR 0x0019
#endif

// marker used by ON_REGISTERED_MESSAGE (real id only known at run time)
#define AFX_MSGMAP_REGISTERED   0xFFFFFFFEu

// ---------------------------------------------------------------------
//  Type-safe thunks -- one per handler signature
//
//  AFX_MF() casts the handler pointer to "pointer to member of ThisClass".
//  That cast is needed because gcc types `&Derived::InheritedHandler` with
//  the class that *declares* the handler (CWnd), not the qualifying class,
//  and a static_cast of a pointer-to-member to a derived class is standard
//  C++ and safe here (single inheritance, no this-pointer adjustment).
// ---------------------------------------------------------------------
namespace afxmsg {

// Extracts the class that *declares* a member function pointer.  gcc types
// `&Derived::InheritedHandler` with the declaring class (e.g. CWnd), so the
// thunks below take the declaring class as a separate template parameter and
// static_cast the target pointer up to it -- always valid, no casts between
// unrelated classes and no undefined behaviour.
template <class PMF> struct PmfClass;
template <class B, class R, class... A> struct PmfClass<R (B::*)(A...)> { typedef B type; };

} // namespace afxmsg

#define AFX_BCLASS(fn) typename ::afxmsg::PmfClass<decltype(&ThisClass::fn)>::type

namespace afxmsg {

// void f()
template <class T, class B, void (B::*F)()> struct Th_vv {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)(); return 0; }
};
// BOOL f()
template <class T, class B, BOOL (B::*F)()> struct Th_bv {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)(); }
};
// UINT f()
template <class T, class B, UINT (B::*F)()> struct Th_uv {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM, LRESULT*)
    { return (LRESULT)(static_cast<B*>(static_cast<T*>(p))->*F)(); }
};
// HCURSOR f()
template <class T, class B, HCURSOR (B::*F)()> struct Th_hcv {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM, LRESULT*)
    { return (LRESULT)(static_cast<B*>(static_cast<T*>(p))->*F)(); }
};
// void f(UINT)
template <class T, class B, void (B::*F)(UINT)> struct Th_vw {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w); return 0; }
};
// void f(int)
template <class T, class B, void (B::*F)(int)> struct Th_vi {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((int)w); return 0; }
};
// void f(BOOL)
template <class T, class B, void (B::*F)(BOOL)> struct Th_vb {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((BOOL)w); return 0; }
};
// BOOL f(BOOL)
template <class T, class B, BOOL (B::*F)(BOOL)> struct Th_bb {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)((BOOL)w); }
};
// void f(LPCTSTR)
template <class T, class B, void (B::*F)(LPCTSTR)> struct Th_vT {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((LPCTSTR)l); return 0; }
};
// void f(UINT_PTR)
template <class T, class B, void (B::*F)(UINT_PTR)> struct Th_vup {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT_PTR)w); return 0; }
};
// void f(UINT, UINT, UINT)  -- WM_CHAR / WM_KEYDOWN / WM_KEYUP etc.
template <class T, class B, void (B::*F)(UINT, UINT, UINT)> struct Th_vwww {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, (UINT)LOWORD(l), (UINT)HIWORD(l)); return 0; }
};
// void f(UINT, LPARAM)  -- WM_SYSCOMMAND etc.
template <class T, class B, void (B::*F)(UINT, LPARAM)> struct Th_vulP {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, l); return 0; }
};
// void f(UINT, void*)  -- WM_INPUTLANGCHANGE etc.
template <class T, class B, void (B::*F)(UINT, void*)> struct Th_vwP {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, (void*)l); return 0; }
};
// void f(UINT nFlags, UINT nButton, CPoint pt)  -- XBUTTONxxx
template <class T, class B, void (B::*F)(UINT, UINT, CPoint)> struct Th_vwwPt {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { CPoint pt(GET_X_LPARAM(l), GET_Y_LPARAM(l));
      (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)LOWORD(w), (UINT)HIWORD(w), pt); return 0; }
};
// void f(UINT nFlags, CPoint point)
template <class T, class B, void (B::*F)(UINT, CPoint)> struct Th_vwPt {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { CPoint pt(GET_X_LPARAM(l), GET_Y_LPARAM(l));
      (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, pt); return 0; }
};
// void f(UINT bitsPerPixel, CSize sizeScreen)
template <class T, class B, void (B::*F)(UINT, CSize)> struct Th_vwSz {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { CSize sz(GET_X_LPARAM(l), GET_Y_LPARAM(l));
      (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, sz); return 0; }
};
// void f(UINT nState, CWnd* pWndOther, BOOL bMinimized)  -- WM_ACTIVATE
template <class T, class B, void (B::*F)(UINT, CWnd*, BOOL)> struct Th_vwWb {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)LOWORD(w), AfxMsgFromHandleWnd((HWND)l),
                               (BOOL)(HIWORD(w) ? TRUE : FALSE)); return 0; }
};
// void f(CWnd*)
template <class T, class B, void (B::*F)(CWnd*)> struct Th_vW {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)(AfxMsgFromHandleWnd((HWND)l)); return 0; }
};
// void f(CWnd* pWnd, UINT nHitTest, UINT message)  -- WM_SETCURSOR-ish
template <class T, class B, void (B::*F)(CWnd*, UINT, UINT)> struct Th_vWww {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)(AfxMsgFromHandleWnd((HWND)w), (UINT)LOWORD(l), (UINT)HIWORD(l)); return 0; }
};
// void f(UINT nSBCode, UINT nPos, CWnd* pScrollBar)  -- WM_HSCROLL / WM_VSCROLL
template <class T, class B, void (B::*F)(UINT, UINT, CWnd*)> struct Th_vwwW {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)LOWORD(w), (UINT)HIWORD(w), AfxMsgFromHandleWnd((HWND)l)); return 0; }
};
// void f(UINT nItemID, UINT nFlags, HMENU hSysMenu)  -- WM_MENUSELECT
template <class T, class B, void (B::*F)(UINT, UINT, HMENU)> struct Th_vwwH {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)LOWORD(w), (UINT)HIWORD(w), (HMENU)l); return 0; }
};
// void f(BOOL, UINT)
template <class T, class B, void (B::*F)(BOOL, UINT)> struct Th_vbw {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((BOOL)w, (UINT)l); return 0; }
};
// void f(BOOL, DWORD)
template <class T, class B, void (B::*F)(BOOL, DWORD)> struct Th_vbd {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((BOOL)w, (DWORD)l); return 0; }
};
// void f(UINT, DWORD)
template <class T, class B, void (B::*F)(UINT, DWORD)> struct Th_vwd {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)LOWORD(w), (DWORD)l); return 0; }
};
// void f(UINT, LONG)
template <class T, class B, void (B::*F)(UINT, LONG)> struct Th_vul {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, (LONG)l); return 0; }
};
// void f(int x, int y)
template <class T, class B, void (B::*F)(int, int)> struct Th_vxy {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((int)(short)LOWORD(l), (int)(short)HIWORD(l)); return 0; }
};
// void f(UINT nType, int cx, int cy)
template <class T, class B, void (B::*F)(UINT, int, int)> struct Th_vwii {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, (int)LOWORD(l), (int)HIWORD(l)); return 0; }
};
// void f(CPoint)
template <class T, class B, void (B::*F)(CPoint)> struct Th_vPt {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)(CPoint(l)); return 0; }
};
// void f(CWnd*, CPoint)  -- WM_CONTEXTMENU
template <class T, class B, void (B::*F)(CWnd*, CPoint)> struct Th_vWPt {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { CPoint pt(GET_X_LPARAM(l), GET_Y_LPARAM(l));
      (static_cast<B*>(static_cast<T*>(p))->*F)(AfxMsgFromHandleWnd((HWND)w), pt); return 0; }
};
// void f(MINMAXINFO*)
template <class T, class B, void (B::*F)(MINMAXINFO*)> struct Th_vMMI {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((MINMAXINFO*)l); return 0; }
};
// void f(WINDOWPOS*)
template <class T, class B, void (B::*F)(WINDOWPOS*)> struct Th_vWP {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((WINDOWPOS*)l); return 0; }
};
// void f(HDROP)
template <class T, class B, void (B::*F)(HDROP)> struct Th_vHDROP {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((HDROP)w); return 0; }
};
// void f(LPRECT)
template <class T, class B, void (B::*F)(LPRECT)> struct Th_vRect {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((LPRECT)l); return 0; }
};
// void f(UINT, LPRECT)  -- WM_SIZING / WM_MOVING
template <class T, class B, void (B::*F)(UINT, LPRECT)> struct Th_vwRect {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, (LPRECT)l); return 0; }
};
// void f(UINT, STYLESTRUCT*)  -- WM_STYLECHANGED
template <class T, class B, void (B::*F)(UINT, LPSTYLESTRUCT)> struct Th_vwStyle {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, (LPSTYLESTRUCT)l); return 0; }
};
// void f(UINT, LPCTSTR)  -- WM_SETTINGCHANGE
template <class T, class B, void (B::*F)(UINT, LPCTSTR)> struct Th_vwT {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, (LPCTSTR)l); return 0; }
};
// void f(int, LPMEASUREITEMSTRUCT)
template <class T, class B, void (B::*F)(int, LPMEASUREITEMSTRUCT)> struct Th_viMIS {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((int)w, (LPMEASUREITEMSTRUCT)l); return 0; }
};
// void f(int, LPDRAWITEMSTRUCT)
template <class T, class B, void (B::*F)(int, LPDRAWITEMSTRUCT)> struct Th_viDIS {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((int)w, (LPDRAWITEMSTRUCT)l); return 0; }
};
// void f(CMenu*, UINT, BOOL)  -- WM_INITMENUPOPUP
template <class T, class B, void (B::*F)(CMenu*, UINT, BOOL)> struct Th_vMwb {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)(AfxMsgFromHandleMenu((HMENU)w), (UINT)LOWORD(l),
                               (BOOL)(HIWORD(l) ? TRUE : FALSE)); return 0; }
};
// int f(LPCREATESTRUCT)
template <class T, class B, int (B::*F)(LPCREATESTRUCT)> struct Th_iCreate {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { return (LRESULT)(static_cast<B*>(static_cast<T*>(p))->*F)((LPCREATESTRUCT)l); }
};
// int f(CWnd*, UINT, UINT)  -- WM_MOUSEACTIVATE
template <class T, class B, int (B::*F)(CWnd*, UINT, UINT)> struct Th_iWww {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { return (LRESULT)(static_cast<B*>(static_cast<T*>(p))->*F)(AfxMsgFromHandleWnd((HWND)w),
                                               (UINT)LOWORD(l), (UINT)HIWORD(l)); }
};
// int f(UINT, CListBox*, UINT)  -- WM_CHARTOITEM / WM_VKEYTOITEM
template <class T, class B, int (B::*F)(UINT, CListBox*, UINT)> struct Th_iwLw {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { return (LRESULT)(static_cast<B*>(static_cast<T*>(p))->*F)((UINT)LOWORD(w),
              AfxMsgFromHandleListBox((HWND)l), (UINT)HIWORD(w)); }
};
// LRESULT f(CPoint)
template <class T, class B, LRESULT (B::*F)(CPoint)> struct Th_lPt {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)(CPoint(l)); }
};
// LRESULT f(WPARAM, LPARAM)
template <class T, class B, LRESULT (B::*F)(WPARAM, LPARAM)> struct Th_lwl {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)(w, l); }
};
// LRESULT f(BOOL, LPARAM)  -- WM_NCCALCSIZE
template <class T, class B, LRESULT (B::*F)(BOOL, LPARAM)> struct Th_lbwl {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)((BOOL)w, l); }
};
// LRESULT f(UINT, UINT, CMenu*)  -- WM_MENUCHAR
template <class T, class B, LRESULT (B::*F)(UINT, UINT, CMenu*)> struct Th_lwwM {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)LOWORD(w), (UINT)HIWORD(w), AfxMsgFromHandleMenu((HMENU)l)); }
};
// BOOL f(WPARAM, LPARAM)
template <class T, class B, BOOL (B::*F)(WPARAM, LPARAM)> struct Th_bwl {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)(w, l); }
};
// BOOL f(CDC*)
template <class T, class B, BOOL (B::*F)(CDC*)> struct Th_bDC {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)(AfxMsgFromHandleDC((HDC)w)); }
};
// BOOL f(CWnd*, UINT, UINT)  -- WM_SETCURSOR
template <class T, class B, BOOL (B::*F)(CWnd*, UINT, UINT)> struct Th_bWww {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)(AfxMsgFromHandleWnd((HWND)w), (UINT)LOWORD(l), (UINT)HIWORD(l)); }
};
// BOOL f(UINT nFlags, short zDelta, CPoint pt)  -- WM_MOUSEWHEEL
template <class T, class B, BOOL (B::*F)(UINT, short, CPoint)> struct Th_bwSpt {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { CPoint pt(GET_X_LPARAM(l), GET_Y_LPARAM(l));
      return (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)LOWORD(w), (short)HIWORD(w), pt); }
};
// BOOL f(CWnd*, COPYDATASTRUCT*)
template <class T, class B, BOOL (B::*F)(CWnd*, COPYDATASTRUCT*)> struct Th_bWCD {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)(AfxMsgFromHandleWnd((HWND)w), (COPYDATASTRUCT*)l); }
};
// BOOL f(HELPINFO*)
template <class T, class B, BOOL (B::*F)(HELPINFO*)> struct Th_bHelp {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)((HELPINFO*)l); }
};
// HBRUSH f(CDC*, CWnd*, UINT)  -- WM_CTLCOLOR
template <class T, class B, HBRUSH (B::*F)(CDC*, CWnd*, UINT)> struct Th_hDCWw {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { return (LRESULT)(static_cast<B*>(static_cast<T*>(p))->*F)(AfxMsgFromHandleDC((HDC)w),
              AfxMsgFromHandleWnd((HWND)LOWORD(l)), (UINT)HIWORD(l)); }
};
// void f(CCmdUI*)
template <class T, class B, void (B::*F)(CCmdUI*)> struct Th_cmd {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((CCmdUI*)l); return 0; }
};
// void f(UINT)  -- command / control ranges
template <class T, class B, void (B::*F)(UINT)> struct Th_vnId {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w); return 0; }
};
// BOOL f(UINT)  -- ON_COMMAND_EX
template <class T, class B, BOOL (B::*F)(UINT)> struct Th_bwId {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM, LRESULT*)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w); }
};
// void f(NMHDR*, LRESULT*)
template <class T, class B, void (B::*F)(NMHDR*, LRESULT*)> struct Th_notify {
    static LRESULT call(CCmdTarget* p, WPARAM, LPARAM l, LRESULT* pResult)
    { (static_cast<B*>(static_cast<T*>(p))->*F)((NMHDR*)l, pResult); return pResult ? *pResult : 0; }
};
// BOOL f(UINT, NMHDR*, LRESULT*)
template <class T, class B, BOOL (B::*F)(UINT, NMHDR*, LRESULT*)> struct Th_notifyEx {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT* pResult)
    { return (static_cast<B*>(static_cast<T*>(p))->*F)((UINT)w, (NMHDR*)l, pResult); }
};
// void f(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)  -- WM_MDIACTIVATE
template <class T, class B, void (B::*F)(BOOL, CWnd*, CWnd*)> struct Th_vbWW {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    {
        BOOL bActivate = ((HWND)l == AfxMsgSafeHwnd(p));
        (static_cast<B*>(static_cast<T*>(p))->*F)(bActivate, AfxMsgFromHandleWnd((HWND)l), AfxMsgFromHandleWnd((HWND)w));
        return 0;
    }
};
// void f(WPARAM, LPARAM)  -- ON_MESSAGE_VOID
template <class T, class B, void (B::*F)(WPARAM, LPARAM)> struct Th_vwl {
    static LRESULT call(CCmdTarget* p, WPARAM w, LPARAM l, LRESULT*)
    { (static_cast<B*>(static_cast<T*>(p))->*F)(w, l); return 0; }
};

} // namespace afxmsg

// ---------------------------------------------------------------------
//  Message map lookup / dispatch (implemented in mfc.cpp)
// ---------------------------------------------------------------------
const AFX_MSGMAP_ENTRY* AFXAPI AfxFindMessageEntry(const AFX_MSGMAP* lpMap, UINT nMsg,
                                                   UINT nCode, UINT nID);
LRESULT AFXAPI AfxDispatchMsgEntry(CCmdTarget* pTarget, const AFX_MSGMAP_ENTRY* lpEntry,
                                   WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// ---------------------------------------------------------------------
//  DECLARE / BEGIN / END_MESSAGE_MAP
// ---------------------------------------------------------------------
#define DECLARE_MESSAGE_MAP()                                                     \
public:                                                                           \
    static const AFX_MSGMAP messageMap;                                           \
    static const AFX_MSGMAP* PASCAL GetThisMessageMap();                          \
    virtual const AFX_MSGMAP* GetMessageMap() const;                              \
    static const AFX_MSGMAP_ENTRY* _GetMessageEntries();

#define BEGIN_MESSAGE_MAP(theClass, baseClass)                                    \
    const AFX_MSGMAP* theClass::GetMessageMap() const                             \
        { return &theClass::messageMap; }                                         \
    const AFX_MSGMAP* PASCAL theClass::GetThisMessageMap()                        \
        { return &theClass::messageMap; }                                         \
    const AFX_MSGMAP theClass::messageMap =                                       \
        { &baseClass::messageMap, theClass::_GetMessageEntries() };               \
    const AFX_MSGMAP_ENTRY* theClass::_GetMessageEntries()                        \
    {                                                                             \
        typedef theClass ThisClass;                                               \
        static_assert(sizeof(ThisClass*) > 0, "BEGIN_MESSAGE_MAP class");         \
        static const AFX_MSGMAP_ENTRY _messageEntries[] = {

#define END_MESSAGE_MAP()                                                         \
            { 0, 0, 0, 0, AfxSig_end, NULL, 0 }                                   \
        };                                                                        \
        return _messageEntries;                                                   \
    }

// ---------------------------------------------------------------------
//  Command / notification macros
// ---------------------------------------------------------------------
#define ON_COMMAND(id, fn) \
        { WM_COMMAND, CN_COMMAND, (UINT)(id), 0, AfxSig_vv, \
          &afxmsg::Th_vv<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_COMMAND_RANGE(id1, id2, fn) \
        { WM_COMMAND, CN_COMMAND, (UINT)(id1), (UINT)(id2), AfxSig_vnId, \
          &afxmsg::Th_vnId<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_COMMAND_EX(id, fn) \
        { WM_COMMAND, CN_COMMAND, (UINT)(id), 0, AfxSig_bwId, \
          &afxmsg::Th_bwId<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_COMMAND_EX_RANGE(id1, id2, fn) \
        { WM_COMMAND, CN_COMMAND, (UINT)(id1), (UINT)(id2), AfxSig_bwId, \
          &afxmsg::Th_bwId<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_UPDATE_COMMAND_UI(id, fn) \
        { WM_IDLEUPDATECMDUI, CN_UPDATE_COMMAND_UI, (UINT)(id), 0, AfxSig_cmd, \
          &afxmsg::Th_cmd<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_UPDATE_COMMAND_UI_RANGE(id1, id2, fn) \
        { WM_IDLEUPDATECMDUI, CN_UPDATE_COMMAND_UI, (UINT)(id1), (UINT)(id2), AfxSig_cmd, \
          &afxmsg::Th_cmd<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },

#define ON_CONTROL(wNotifyCode, id, fn) \
        { WM_COMMAND, (UINT)(wNotifyCode), (UINT)(id), 0, AfxSig_vv, \
          &afxmsg::Th_vv<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_CONTROL_RANGE(wNotifyCode, id1, id2, fn) \
        { WM_COMMAND, (UINT)(wNotifyCode), (UINT)(id1), (UINT)(id2), AfxSig_vnId, \
          &afxmsg::Th_vnId<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_CONTROL_EX(wNotifyCode, id, fn) \
        { WM_COMMAND, (UINT)(wNotifyCode), (UINT)(id), 0, AfxSig_bwId, \
          &afxmsg::Th_bwId<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },

#define ON_NOTIFY(wNotifyCode, id, fn) \
        { WM_NOTIFY, (UINT)(wNotifyCode), (UINT)(id), 0, AfxSig_notify, \
          &afxmsg::Th_notify<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_NOTIFY_RANGE(wNotifyCode, id1, id2, fn) \
        { WM_NOTIFY, (UINT)(wNotifyCode), (UINT)(id1), (UINT)(id2), AfxSig_notify, \
          &afxmsg::Th_notify<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_NOTIFY_EX(wNotifyCode, id, fn) \
        { WM_NOTIFY, (UINT)(wNotifyCode), (UINT)(id), 0, AfxSig_notifyEx, \
          &afxmsg::Th_notifyEx<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_NOTIFY_EX_RANGE(wNotifyCode, id1, id2, fn) \
        { WM_NOTIFY, (UINT)(wNotifyCode), (UINT)(id1), (UINT)(id2), AfxSig_notifyEx, \
          &afxmsg::Th_notifyEx<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },

#define ON_MESSAGE(message, fn) \
        { (UINT)(message), 0, 0, 0, AfxSig_lwl, \
          &afxmsg::Th_lwl<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_MESSAGE_VOID(message, fn) \
        { (UINT)(message), 0, 0, 0, AfxSig_lwl, \
          &afxmsg::Th_vwl<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, 0 },
#define ON_REGISTERED_MESSAGE(nMessageVariable, fn) \
        { AFX_MSGMAP_REGISTERED, 0, 0, 0, AfxSig_lwl, \
          &afxmsg::Th_lwl<ThisClass, AFX_BCLASS(fn), &ThisClass::fn>::call, (UINT_PTR)&(nMessageVariable) },
#define ON_THREAD_MESSAGE(message, fn)            ON_MESSAGE(message, fn)
#define ON_REGISTERED_THREAD_MESSAGE(v, fn)       ON_REGISTERED_MESSAGE(v, fn)

// ---------------------------------------------------------------------
//  ON_WM_xxx macros (handler names + signatures match MFC exactly)
// ---------------------------------------------------------------------
#define ON_WM_ACTIVATE()          { WM_ACTIVATE, 0, 0, 0, AfxSig_vwWb, &afxmsg::Th_vwWb<ThisClass, AFX_BCLASS(OnActivate), &ThisClass::OnActivate>::call, 0 },
#define ON_WM_ACTIVATEAPP()       { WM_ACTIVATEAPP, 0, 0, 0, AfxSig_vbd, &afxmsg::Th_vbd<ThisClass, AFX_BCLASS(OnActivateApp), &ThisClass::OnActivateApp>::call, 0 },
#define ON_WM_APPCOMMAND()        { WM_APPCOMMAND, 0, 0, 0, AfxSig_lwl, &afxmsg::Th_lwl<ThisClass, AFX_BCLASS(OnAppCommand), &ThisClass::OnAppCommand>::call, 0 },
#define ON_WM_CAPTURECHANGED()    { WM_CAPTURECHANGED, 0, 0, 0, AfxSig_vW, &afxmsg::Th_vW<ThisClass, AFX_BCLASS(OnCaptureChanged), &ThisClass::OnCaptureChanged>::call, 0 },
#define ON_WM_CANCELMODE()        { WM_CANCELMODE, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnCancelMode), &ThisClass::OnCancelMode>::call, 0 },
#define ON_WM_CHAR()              { WM_CHAR, 0, 0, 0, AfxSig_vwww, &afxmsg::Th_vwww<ThisClass, AFX_BCLASS(OnChar), &ThisClass::OnChar>::call, 0 },
#define ON_WM_CHARTOITEM()        { WM_CHARTOITEM, 0, 0, 0, AfxSig_iwLw, &afxmsg::Th_iwLw<ThisClass, AFX_BCLASS(OnCharToItem), &ThisClass::OnCharToItem>::call, 0 },
#define ON_WM_CHILDACTIVATE()     { WM_CHILDACTIVATE, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnChildActivate), &ThisClass::OnChildActivate>::call, 0 },
#define ON_WM_CLOSE()             { WM_CLOSE, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnClose), &ThisClass::OnClose>::call, 0 },
#define ON_WM_COMPACTING()        { WM_COMPACTING, 0, 0, 0, AfxSig_vw, &afxmsg::Th_vw<ThisClass, AFX_BCLASS(OnCompacting), &ThisClass::OnCompacting>::call, 0 },
#define ON_WM_CONTEXTMENU()       { WM_CONTEXTMENU, 0, 0, 0, AfxSig_vWPt, &afxmsg::Th_vWPt<ThisClass, AFX_BCLASS(OnContextMenu), &ThisClass::OnContextMenu>::call, 0 },
#define ON_WM_COPYDATA()          { WM_COPYDATA, 0, 0, 0, AfxSig_bWCD, &afxmsg::Th_bWCD<ThisClass, AFX_BCLASS(OnCopyData), &ThisClass::OnCopyData>::call, 0 },
#define ON_WM_CREATE()            { WM_CREATE, 0, 0, 0, AfxSig_iCreate, &afxmsg::Th_iCreate<ThisClass, AFX_BCLASS(OnCreate), &ThisClass::OnCreate>::call, 0 },
#define ON_WM_CTLCOLOR()          { WM_CTLCOLOR, 0, 0, 0, AfxSig_hDCWw, &afxmsg::Th_hDCWw<ThisClass, AFX_BCLASS(OnCtlColor), &ThisClass::OnCtlColor>::call, 0 },
#define ON_WM_DESTROY()           { WM_DESTROY, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnDestroy), &ThisClass::OnDestroy>::call, 0 },
#define ON_WM_DEVMODECHANGE()     { WM_DEVMODECHANGE, 0, 0, 0, AfxSig_vT, &afxmsg::Th_vT<ThisClass, AFX_BCLASS(OnDevModeChange), &ThisClass::OnDevModeChange>::call, 0 },
#define ON_WM_DISPLAYCHANGE()     { WM_DISPLAYCHANGE, 0, 0, 0, AfxSig_vwSz, &afxmsg::Th_vwSz<ThisClass, AFX_BCLASS(OnDisplayChange), &ThisClass::OnDisplayChange>::call, 0 },
#define ON_WM_DRAWITEM()          { WM_DRAWITEM, 0, 0, 0, AfxSig_viDIS, &afxmsg::Th_viDIS<ThisClass, AFX_BCLASS(OnDrawItem), &ThisClass::OnDrawItem>::call, 0 },
#define ON_WM_DROPFILES()         { WM_DROPFILES, 0, 0, 0, AfxSig_vHDROP, &afxmsg::Th_vHDROP<ThisClass, AFX_BCLASS(OnDropFiles), &ThisClass::OnDropFiles>::call, 0 },
#define ON_WM_ENABLE()            { WM_ENABLE, 0, 0, 0, AfxSig_vb, &afxmsg::Th_vb<ThisClass, AFX_BCLASS(OnEnable), &ThisClass::OnEnable>::call, 0 },
#define ON_WM_ENDSESSION()        { WM_ENDSESSION, 0, 0, 0, AfxSig_vb, &afxmsg::Th_vb<ThisClass, AFX_BCLASS(OnEndSession), &ThisClass::OnEndSession>::call, 0 },
#define ON_WM_ENTERSIZEMOVE()     { WM_ENTERSIZEMOVE, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnEnterSizeMove), &ThisClass::OnEnterSizeMove>::call, 0 },
#define ON_WM_ERASEBKGND()        { WM_ERASEBKGND, 0, 0, 0, AfxSig_bDC, &afxmsg::Th_bDC<ThisClass, AFX_BCLASS(OnEraseBkgnd), &ThisClass::OnEraseBkgnd>::call, 0 },
#define ON_WM_EXITSIZEMOVE()      { WM_EXITSIZEMOVE, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnExitSizeMove), &ThisClass::OnExitSizeMove>::call, 0 },
#define ON_WM_FONTCHANGE()        { WM_FONTCHANGE, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnFontChange), &ThisClass::OnFontChange>::call, 0 },
#define ON_WM_GETDLGCODE()        { WM_GETDLGCODE, 0, 0, 0, AfxSig_uv, &afxmsg::Th_uv<ThisClass, AFX_BCLASS(OnGetDlgCode), &ThisClass::OnGetDlgCode>::call, 0 },
#define ON_WM_GETMINMAXINFO()     { WM_GETMINMAXINFO, 0, 0, 0, AfxSig_vMMI, &afxmsg::Th_vMMI<ThisClass, AFX_BCLASS(OnGetMinMaxInfo), &ThisClass::OnGetMinMaxInfo>::call, 0 },
#define ON_WM_HELPINFO()          { WM_HELP, 0, 0, 0, AfxSig_bHelp, &afxmsg::Th_bHelp<ThisClass, AFX_BCLASS(OnHelpInfo), &ThisClass::OnHelpInfo>::call, 0 },
#define ON_WM_HSCROLL()           { WM_HSCROLL, 0, 0, 0, AfxSig_vwwW, &afxmsg::Th_vwwW<ThisClass, AFX_BCLASS(OnHScroll), &ThisClass::OnHScroll>::call, 0 },
#define ON_WM_ICONERASEBKGND()    { WM_ICONERASEBKGND, 0, 0, 0, AfxSig_bDC, &afxmsg::Th_bDC<ThisClass, AFX_BCLASS(OnIconEraseBkgnd), &ThisClass::OnIconEraseBkgnd>::call, 0 },
#define ON_WM_INITMENUPOPUP()     { WM_INITMENUPOPUP, 0, 0, 0, AfxSig_vMwb, &afxmsg::Th_vMwb<ThisClass, AFX_BCLASS(OnInitMenuPopup), &ThisClass::OnInitMenuPopup>::call, 0 },
#define ON_WM_INPUTLANGCHANGE()   { WM_INPUTLANGCHANGE, 0, 0, 0, AfxSig_vwd, &afxmsg::Th_vwP<ThisClass, AFX_BCLASS(OnInputLangChange), &ThisClass::OnInputLangChange>::call, 0 },
#define ON_WM_KILLFOCUS()         { WM_KILLFOCUS, 0, 0, 0, AfxSig_vW, &afxmsg::Th_vW<ThisClass, AFX_BCLASS(OnKillFocus), &ThisClass::OnKillFocus>::call, 0 },
#define ON_WM_KEYDOWN()           { WM_KEYDOWN, 0, 0, 0, AfxSig_vwww, &afxmsg::Th_vwww<ThisClass, AFX_BCLASS(OnKeyDown), &ThisClass::OnKeyDown>::call, 0 },
#define ON_WM_KEYUP()             { WM_KEYUP, 0, 0, 0, AfxSig_vwww, &afxmsg::Th_vwww<ThisClass, AFX_BCLASS(OnKeyUp), &ThisClass::OnKeyUp>::call, 0 },
#define ON_WM_LBUTTONDBLCLK()     { WM_LBUTTONDBLCLK, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnLButtonDblClk), &ThisClass::OnLButtonDblClk>::call, 0 },
#define ON_WM_LBUTTONDOWN()       { WM_LBUTTONDOWN, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnLButtonDown), &ThisClass::OnLButtonDown>::call, 0 },
#define ON_WM_LBUTTONUP()         { WM_LBUTTONUP, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnLButtonUp), &ThisClass::OnLButtonUp>::call, 0 },
#define ON_WM_MBUTTONDBLCLK()     { WM_MBUTTONDBLCLK, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnMButtonDblClk), &ThisClass::OnMButtonDblClk>::call, 0 },
#define ON_WM_MBUTTONDOWN()       { WM_MBUTTONDOWN, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnMButtonDown), &ThisClass::OnMButtonDown>::call, 0 },
#define ON_WM_MBUTTONUP()         { WM_MBUTTONUP, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnMButtonUp), &ThisClass::OnMButtonUp>::call, 0 },
#define ON_WM_MDIACTIVATE()       { WM_MDIACTIVATE, 0, 0, 0, AfxSig_vwWb, &afxmsg::Th_vbWW<ThisClass, AFX_BCLASS(OnMDIActivate), &ThisClass::OnMDIActivate>::call, 0 },
#define ON_WM_MEASUREITEM()       { WM_MEASUREITEM, 0, 0, 0, AfxSig_viMIS, &afxmsg::Th_viMIS<ThisClass, AFX_BCLASS(OnMeasureItem), &ThisClass::OnMeasureItem>::call, 0 },
#define ON_WM_MENUCHAR()          { WM_MENUCHAR, 0, 0, 0, AfxSig_lwwM, &afxmsg::Th_lwwM<ThisClass, AFX_BCLASS(OnMenuChar), &ThisClass::OnMenuChar>::call, 0 },
#define ON_WM_MENUSELECT()        { WM_MENUSELECT, 0, 0, 0, AfxSig_vwwH, &afxmsg::Th_vwwH<ThisClass, AFX_BCLASS(OnMenuSelect), &ThisClass::OnMenuSelect>::call, 0 },
#define ON_WM_MOUSEACTIVATE()     { WM_MOUSEACTIVATE, 0, 0, 0, AfxSig_iWww, &afxmsg::Th_iWww<ThisClass, AFX_BCLASS(OnMouseActivate), &ThisClass::OnMouseActivate>::call, 0 },
#define ON_WM_MOUSEHOVER()        { WM_MOUSEHOVER, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnMouseHover), &ThisClass::OnMouseHover>::call, 0 },
#define ON_WM_MOUSELEAVE()        { WM_MOUSELEAVE, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnMouseLeave), &ThisClass::OnMouseLeave>::call, 0 },
#define ON_WM_MOUSEMOVE()         { WM_MOUSEMOVE, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnMouseMove), &ThisClass::OnMouseMove>::call, 0 },
#define ON_WM_MOUSEWHEEL()        { WM_MOUSEWHEEL, 0, 0, 0, AfxSig_bwSpt, &afxmsg::Th_bwSpt<ThisClass, AFX_BCLASS(OnMouseWheel), &ThisClass::OnMouseWheel>::call, 0 },
#define ON_WM_MOVE()              { WM_MOVE, 0, 0, 0, AfxSig_vxy, &afxmsg::Th_vxy<ThisClass, AFX_BCLASS(OnMove), &ThisClass::OnMove>::call, 0 },
#define ON_WM_MOVING()            { WM_MOVING, 0, 0, 0, AfxSig_vwRect, &afxmsg::Th_vwRect<ThisClass, AFX_BCLASS(OnMoving), &ThisClass::OnMoving>::call, 0 },
#define ON_WM_NCACTIVATE()        { WM_NCACTIVATE, 0, 0, 0, AfxSig_bb, &afxmsg::Th_bb<ThisClass, AFX_BCLASS(OnNcActivate), &ThisClass::OnNcActivate>::call, 0 },
#define ON_WM_NCCALCSIZE()        { WM_NCCALCSIZE, 0, 0, 0, AfxSig_lbwl, &afxmsg::Th_lbwl<ThisClass, AFX_BCLASS(OnNcCalcSize), &ThisClass::OnNcCalcSize>::call, 0 },
#define ON_WM_NCHITTEST()         { WM_NCHITTEST, 0, 0, 0, AfxSig_lPt, &afxmsg::Th_lPt<ThisClass, AFX_BCLASS(OnNcHitTest), &ThisClass::OnNcHitTest>::call, 0 },
#define ON_WM_NCLBUTTONDBLCLK()   { WM_NCLBUTTONDBLCLK, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnNcLButtonDblClk), &ThisClass::OnNcLButtonDblClk>::call, 0 },
#define ON_WM_NCLBUTTONDOWN()     { WM_NCLBUTTONDOWN, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnNcLButtonDown), &ThisClass::OnNcLButtonDown>::call, 0 },
#define ON_WM_NCLBUTTONUP()       { WM_NCLBUTTONUP, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnNcLButtonUp), &ThisClass::OnNcLButtonUp>::call, 0 },
#define ON_WM_NCMOUSEMOVE()       { WM_NCMOUSEMOVE, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnNcMouseMove), &ThisClass::OnNcMouseMove>::call, 0 },
#define ON_WM_NCPAINT()           { WM_NCPAINT, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnNcPaint), &ThisClass::OnNcPaint>::call, 0 },
#define ON_WM_PAINT()             { WM_PAINT, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnPaint), &ThisClass::OnPaint>::call, 0 },
#define ON_WM_PARENTNOTIFY()      { WM_PARENTNOTIFY, 0, 0, 0, AfxSig_vwl, &afxmsg::Th_vwl<ThisClass, AFX_BCLASS(OnParentNotify), &ThisClass::OnParentNotify>::call, 0 },
#define ON_WM_POWER()             { WM_POWER, 0, 0, 0, AfxSig_vi, &afxmsg::Th_vi<ThisClass, AFX_BCLASS(OnPower), &ThisClass::OnPower>::call, 0 },
#define ON_WM_QUERYDRAGICON()     { WM_QUERYDRAGICON, 0, 0, 0, AfxSig_hcv, &afxmsg::Th_hcv<ThisClass, AFX_BCLASS(OnQueryDragIcon), &ThisClass::OnQueryDragIcon>::call, 0 },
#define ON_WM_QUERYNEWPALETTE()   { WM_QUERYNEWPALETTE, 0, 0, 0, AfxSig_bv, &afxmsg::Th_bv<ThisClass, AFX_BCLASS(OnQueryNewPalette), &ThisClass::OnQueryNewPalette>::call, 0 },
#define ON_WM_RBUTTONDBLCLK()     { WM_RBUTTONDBLCLK, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnRButtonDblClk), &ThisClass::OnRButtonDblClk>::call, 0 },
#define ON_WM_RBUTTONDOWN()       { WM_RBUTTONDOWN, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnRButtonDown), &ThisClass::OnRButtonDown>::call, 0 },
#define ON_WM_RBUTTONUP()         { WM_RBUTTONUP, 0, 0, 0, AfxSig_vwPt, &afxmsg::Th_vwPt<ThisClass, AFX_BCLASS(OnRButtonUp), &ThisClass::OnRButtonUp>::call, 0 },
#define ON_WM_SETCURSOR()         { WM_SETCURSOR, 0, 0, 0, AfxSig_bWww, &afxmsg::Th_bWww<ThisClass, AFX_BCLASS(OnSetCursor), &ThisClass::OnSetCursor>::call, 0 },
#define ON_WM_SETFOCUS()          { WM_SETFOCUS, 0, 0, 0, AfxSig_vW, &afxmsg::Th_vW<ThisClass, AFX_BCLASS(OnSetFocus), &ThisClass::OnSetFocus>::call, 0 },
#define ON_WM_SETTINGCHANGE()     { WM_SETTINGCHANGE, 0, 0, 0, AfxSig_vwT, &afxmsg::Th_vwT<ThisClass, AFX_BCLASS(OnSettingChange), &ThisClass::OnSettingChange>::call, 0 },
#define ON_WM_SHOWWINDOW()        { WM_SHOWWINDOW, 0, 0, 0, AfxSig_vbw, &afxmsg::Th_vbw<ThisClass, AFX_BCLASS(OnShowWindow), &ThisClass::OnShowWindow>::call, 0 },
#define ON_WM_SIZE()              { WM_SIZE, 0, 0, 0, AfxSig_vwii, &afxmsg::Th_vwii<ThisClass, AFX_BCLASS(OnSize), &ThisClass::OnSize>::call, 0 },
#define ON_WM_SIZING()            { WM_SIZING, 0, 0, 0, AfxSig_vwRect, &afxmsg::Th_vwRect<ThisClass, AFX_BCLASS(OnSizing), &ThisClass::OnSizing>::call, 0 },
#define ON_WM_STYLECHANGED()      { WM_STYLECHANGED, 0, 0, 0, AfxSig_vwStyle, &afxmsg::Th_vwStyle<ThisClass, AFX_BCLASS(OnStyleChanged), &ThisClass::OnStyleChanged>::call, 0 },
#define ON_WM_SYSCOLORCHANGE()    { WM_SYSCOLORCHANGE, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnSysColorChange), &ThisClass::OnSysColorChange>::call, 0 },
#define ON_WM_SYSCOMMAND()        { WM_SYSCOMMAND, 0, 0, 0, AfxSig_vul, &afxmsg::Th_vulP<ThisClass, AFX_BCLASS(OnSysCommand), &ThisClass::OnSysCommand>::call, 0 },
#define ON_WM_SYSKEYDOWN()        { WM_SYSKEYDOWN, 0, 0, 0, AfxSig_vwww, &afxmsg::Th_vwww<ThisClass, AFX_BCLASS(OnSysKeyDown), &ThisClass::OnSysKeyDown>::call, 0 },
#define ON_WM_SYSKEYUP()          { WM_SYSKEYUP, 0, 0, 0, AfxSig_vwww, &afxmsg::Th_vwww<ThisClass, AFX_BCLASS(OnSysKeyUp), &ThisClass::OnSysKeyUp>::call, 0 },
#define ON_WM_THEMECHANGED()      { WM_THEMECHANGED, 0, 0, 0, AfxSig_vv, &afxmsg::Th_vv<ThisClass, AFX_BCLASS(OnThemeChanged), &ThisClass::OnThemeChanged>::call, 0 },
#define ON_WM_TIMECHANGE()        { WM_TIMECHANGE, 0, 0, 0, AfxSig_vul, &afxmsg::Th_vul<ThisClass, AFX_BCLASS(OnTimeChange), &ThisClass::OnTimeChange>::call, 0 },
#define ON_WM_TIMER()             { WM_TIMER, 0, 0, 0, AfxSig_vup, &afxmsg::Th_vup<ThisClass, AFX_BCLASS(OnTimer), &ThisClass::OnTimer>::call, 0 },
#define ON_WM_VKEYTOITEM()        { WM_VKEYTOITEM, 0, 0, 0, AfxSig_iwLw, &afxmsg::Th_iwLw<ThisClass, AFX_BCLASS(OnVKeyToItem), &ThisClass::OnVKeyToItem>::call, 0 },
#define ON_WM_VSCROLL()           { WM_VSCROLL, 0, 0, 0, AfxSig_vwwW, &afxmsg::Th_vwwW<ThisClass, AFX_BCLASS(OnVScroll), &ThisClass::OnVScroll>::call, 0 },
#define ON_WM_WINDOWPOSCHANGED()  { WM_WINDOWPOSCHANGED, 0, 0, 0, AfxSig_vWP, &afxmsg::Th_vWP<ThisClass, AFX_BCLASS(OnWindowPosChanged), &ThisClass::OnWindowPosChanged>::call, 0 },
#define ON_WM_WINDOWPOSCHANGING() { WM_WINDOWPOSCHANGING, 0, 0, 0, AfxSig_vWP, &afxmsg::Th_vWP<ThisClass, AFX_BCLASS(OnWindowPosChanging), &ThisClass::OnWindowPosChanging>::call, 0 },
#define ON_WM_XBUTTONDBLCLK()     { WM_XBUTTONDBLCLK, 0, 0, 0, AfxSig_vwwPt, &afxmsg::Th_vwwPt<ThisClass, AFX_BCLASS(OnXButtonDblClk), &ThisClass::OnXButtonDblClk>::call, 0 },
#define ON_WM_XBUTTONDOWN()       { WM_XBUTTONDOWN, 0, 0, 0, AfxSig_vwwPt, &afxmsg::Th_vwwPt<ThisClass, AFX_BCLASS(OnXButtonDown), &ThisClass::OnXButtonDown>::call, 0 },
#define ON_WM_XBUTTONUP()         { WM_XBUTTONUP, 0, 0, 0, AfxSig_vwwPt, &afxmsg::Th_vwwPt<ThisClass, AFX_BCLASS(OnXButtonUp), &ThisClass::OnXButtonUp>::call, 0 },

// control notification shortcuts (exactly like MFC)
#define ON_BN_CLICKED(id, fn)             ON_CONTROL(BN_CLICKED, id, fn)
#define ON_BN_DOUBLECLICKED(id, fn)       ON_CONTROL(BN_DOUBLECLICKED, id, fn)
#define ON_BN_KILLFOCUS(id, fn)           ON_CONTROL(BN_KILLFOCUS, id, fn)
#define ON_BN_SETFOCUS(id, fn)            ON_CONTROL(BN_SETFOCUS, id, fn)
#define ON_EN_CHANGE(id, fn)              ON_CONTROL(EN_CHANGE, id, fn)
#define ON_EN_UPDATE(id, fn)              ON_CONTROL(EN_UPDATE, id, fn)
#define ON_EN_KILLFOCUS(id, fn)           ON_CONTROL(EN_KILLFOCUS, id, fn)
#define ON_EN_SETFOCUS(id, fn)            ON_CONTROL(EN_SETFOCUS, id, fn)
#define ON_EN_ERRSPACE(id, fn)            ON_CONTROL(EN_ERRSPACE, id, fn)
#define ON_EN_MAXTEXT(id, fn)             ON_CONTROL(EN_MAXTEXT, id, fn)
#define ON_EN_HSCROLL(id, fn)             ON_CONTROL(EN_HSCROLL, id, fn)
#define ON_EN_VSCROLL(id, fn)             ON_CONTROL(EN_VSCROLL, id, fn)
#define ON_LBN_SELCHANGE(id, fn)          ON_CONTROL(LBN_SELCHANGE, id, fn)
#define ON_LBN_DBLCLK(id, fn)             ON_CONTROL(LBN_DBLCLK, id, fn)
#define ON_LBN_SELCANCEL(id, fn)          ON_CONTROL(LBN_SELCANCEL, id, fn)
#define ON_CBN_SELCHANGE(id, fn)          ON_CONTROL(CBN_SELCHANGE, id, fn)
#define ON_CBN_DBLCLK(id, fn)             ON_CONTROL(CBN_DBLCLK, id, fn)
#define ON_CBN_EDITCHANGE(id, fn)         ON_CONTROL(CBN_EDITCHANGE, id, fn)
#define ON_CBN_EDITUPDATE(id, fn)         ON_CONTROL(CBN_EDITUPDATE, id, fn)
#define ON_CBN_DROPDOWN(id, fn)           ON_CONTROL(CBN_DROPDOWN, id, fn)
#define ON_STN_CLICKED(id, fn)            ON_CONTROL(STN_CLICKED, id, fn)

#endif /* __AFXMSG_H__ */
