// =====================================================================
//  afxwin.h  --  The single header MFC programmers include.
//
//      #include <afxwin.h>     // window/application framework
//      #include <afxext.h>     // status bar, tool bar, rebar
//      #include <afxcmn.h>     // common controls
//      #include <afxdlgs.h>    // common dialogs
//
//  Everything below is a source-compatible clone of the corresponding
//  MFC classes, implemented on top of the plain Win32 API and the C++11
//  standard library so that it compiles with g++ / MinGW-w64.
// =====================================================================
#ifndef __AFXWIN_H__
#define __AFXWIN_H__
#pragma once

#include "afx.h"          // core: CString, CObject, collections, files, time
#include "afxmsg.h"       // message maps
#include "afxgdi.h"       // CDC, CPen, CBrush, CFont, CBitmap, CRgn
#include "afxwnd.h"       // CCmdTarget, CWnd, CMenu, CCmdUI
#include "afxapp.h"       // CWinThread, CWinApp
#include "afxdoc.h"       // CDocument, CView, CDocTemplate, CDocManager
#include "afxdlg.h"       // CDialog + DDX/DDV
#include "afxfrm.h"       // CControlBar, CFrameWnd, MDI

// ---------------------------------------------------------------------
//  Global helper functions (MFC compatible)
// ---------------------------------------------------------------------
int AFXAPI AfxMessageBox(LPCTSTR lpszText, UINT nType, UINT nIDHelp);
int AFXAPI AfxMessageBox(UINT nIDPrompt, UINT nType, UINT nIDHelp);

LPTSTR AFXAPI AfxGetAppName();
CWnd*  AFXAPI AfxGetMainWnd();
CWnd*  AFXAPI AfxGetParentFrame(CWnd* pWnd);
CFrameWnd* AFXAPI AfxGetFrameWnd(CWnd* pWnd);
HINSTANCE AFXAPI AfxFindResourceHandle(LPCTSTR lpszName, LPCTSTR lpszType);
int AFXAPI AfxLoadString(UINT nID, LPTSTR lpszBuf);
BOOL AFXAPI AfxInitExtensionModule(void* module, HINSTANCE hInst);
void AFXAPI AfxTermExtensionModule(void* module);
void AFXAPI AfxDelayDelete(CWnd* pWnd);
CWinThread* AFXAPI AfxGetStaticModuleState();   // compatibility stub

// helper: show a wait cursor / hourglass while doing work
class CWaitCursor
{
public:
    CWaitCursor();
    ~CWaitCursor();
    void Restore();
protected:
    HCURSOR m_hSaveCursor;
};

// MFC's "trace" of a message map entry (diagnostics)
LPCSTR AFXAPI AfxGetSigName(UINT nSig);

#endif /* __AFXWIN_H__ */
