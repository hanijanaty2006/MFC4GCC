// =====================================================================
//  afxdoc.h  --  Document/View architecture for the MFC-GCC clone:
//                CDocument, CView, CScrollView, CDocTemplate,
//                CSingleDocTemplate, CMultiDocTemplate, CDocManager
// =====================================================================
#ifndef __AFXDOC_H__
#define __AFXDOC_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif
#include "afxwnd.h"

class CPrintDialog;
class CPreviewView;

// ---------------------------------------------------------------------
//  CPrintInfo -- describes a print job (used by the CView print hooks)
// ---------------------------------------------------------------------
struct CPrintInfo
{
    CPrintDialog* m_pPD;              // print dialog used
    int   m_nCurPage;                 // current page during printing
    int   m_nNumPreviewPages;         // number of preview pages
    BOOL  m_bPreview;                 // TRUE while print-previewing
    BOOL  m_bContinuePrinting;        // set to FALSE to abort
    BOOL  m_bDocObject;
    UINT  m_nOffsetPage;              // offset for page numbering
    CRect m_rectDraw;                 // rectangle of the current page

    CPrintInfo();
    ~CPrintInfo();
    void SetMinPage(UINT nMinPage);
    void SetMaxPage(UINT nMaxPage);
    UINT GetMinPage() const;
    UINT GetMaxPage() const;
    UINT GetFromPage() const;
    UINT GetToPage() const;
};

// ---------------------------------------------------------------------
//  CView
// ---------------------------------------------------------------------
class CView : public CWnd
{
    DECLARE_DYNAMIC(CView)
public:
    CView();
    virtual ~CView();

    CDocument* GetDocument() const { return m_pDocument; }
    void SetDocument(CDocument* pDoc) { m_pDocument = pDoc; }
    CFrameWnd* GetParentFrame() const;

    // overridables
    virtual void OnDraw(CDC* pDC) = 0;
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
    virtual void OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

    BOOL DoPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
    BOOL DoPreparePrinting(CPrintInfo* pInfo);

    virtual void PostNcDestroy();

    afx_msg void OnPaint();
    afx_msg void OnFilePrint();
    afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSetFocusView(CWnd* pOldWnd);

    DECLARE_MESSAGE_MAP()

protected:
    CDocument* m_pDocument;
};

// ---------------------------------------------------------------------
//  CScrollView -- a view with automatic scroll bars
// ---------------------------------------------------------------------
class CScrollView : public CView
{
    DECLARE_DYNAMIC(CScrollView)
public:
    CScrollView();
    virtual ~CScrollView();

    void SetScaleToFitSize(SIZE sizeTotal);
    void SetScrollSizes(int nMapMode, SIZE sizeTotal,
                        const SIZE& sizePage = sizeDefault, const SIZE& sizeLine = sizeDefault);
    CSize GetTotalSize() const   { return m_sizeTotal; }
    CSize GetTotalDevSize() const{ return m_sizeTotalDev; }
    CSize GetTotalLogSize() const{ return m_sizeTotal; }
    CSize GetDeviceScrollPosition() const;
    void GetDeviceScrollSizes(int& nMapMode, SIZE& sizeTotal, SIZE& sizePage, SIZE& sizeLine) const;
    void ScrollToPosition(POINT point);
    void FillOutsideRect(CDC* pDC, CBrush* pBrush);
    void CenterOnPoint(CPoint ptCenter);
    BOOL IsTracking() const { return m_bTrack; }
    virtual void OnDraw(CDC* pDC) = 0;
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CWnd* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CWnd* pScrollBar);
    afx_msg BOOL OnMouseWheelScroll(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnPaint();

    DECLARE_MESSAGE_MAP()

protected:
    virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
    virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
    void UpdateBars();

    CSize m_sizeTotal;      // total size in logical units
    CSize m_sizePage;       // page size
    CSize m_sizeLine;       // line size
    CSize m_sizeTotalDev;   // total size in device units
    CSize m_sizePageDev;
    CSize m_sizeLineDev;
    CSize m_sizeAllSingleLine;
    int   m_nMapMode;
    BOOL  m_bCenter;
    BOOL  m_bTrack;
    int   m_nTrackDir;
    POINT m_ptCenter;
};

// ---------------------------------------------------------------------
//  CDocument
// ---------------------------------------------------------------------
class CDocument : public CCmdTarget
{
    DECLARE_DYNAMIC(CDocument)
public:
    CDocument();
    virtual ~CDocument();

    // attributes
    const CString& GetPathName() const { return m_strPathName; }
    void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
    const CString& GetTitle() const { return m_strTitle; }
    void SetTitle(LPCTSTR lpszTitle);
    CDocTemplate* GetDocTemplate() const { return m_pDocTemplate; }
    void SetDocTemplate(CDocTemplate* pTemplate) { m_pDocTemplate = pTemplate; }
    BOOL IsModified() const { return m_bModified; }
    void SetModifiedFlag(BOOL bModified = TRUE) { m_bModified = bModified; }

    // overridables
    virtual BOOL OnNewDocument();
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
    virtual void DeleteContents();
    virtual void CloseDocument();
    virtual void Serialize(CArchive& ar);
    virtual void OnChangedViewList();
    virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);

    // operations
    void UpdateAllViews(CView* pSender, LPARAM lHint = 0, CObject* pHint = NULL);
    POSITION GetFirstViewPosition() const;
    CView* GetNextView(POSITION& rPos) const;
    void AddView(CView* pView);
    void RemoveView(CView* pView);
    BOOL SaveModified();
    BOOL DoFileLoad();
    void PreCloseFrame();

    afx_msg void OnFileClose();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);

    DECLARE_MESSAGE_MAP()

protected:
    CString        m_strTitle;
    CString        m_strPathName;
    CDocTemplate*  m_pDocTemplate;
    CPtrList       m_viewList;
    BOOL           m_bModified;
    BOOL           m_bAutoDelete;

    friend class CDocTemplate;
    friend class CSingleDocTemplate;
    friend class CMultiDocTemplate;
};

// ---------------------------------------------------------------------
//  CDocTemplate
// ---------------------------------------------------------------------
class CDocTemplate : public CObject
{
public:
    // document type strings (loaded from the string table, MFC layout)
    struct DocString
    {
        LPCTSTR m_lpszClass;
        LPCTSTR m_lpszWindowMenu;
        LPCTSTR m_lpszFileNewName;
        LPCTSTR m_lpszFilterName;
        LPCTSTR m_lpszFilterExt;
        LPCTSTR m_lpszShellNew;
        LPCTSTR m_lpszShellOpen;
        LPCTSTR m_lpszShellPrint;
        LPCTSTR m_lpszShellPrintTo;
        LPCTSTR m_lpszShellDDE;
    };
    enum DocStringIndex
    {
        windowTitle,        // default window title
        fileNewName,        // name shown under File/New
        fileExt,            // default extension (without the dot)
        fileNameFilterID,   // filter shown in the file dialogs
        fileNewViewName,    // unused
        shellFileTypeName,  // shell file type name
        shellNewFileName,   // shell new command
        shellOpenCommand,   // shell open command
        shellOpenWithCmd,   // shell open-with command
        shellDDECmd,        // shell DDE command
        shellDDETopic       // shell DDE topic
    };

    CDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
                 CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
    virtual ~CDocTemplate();

    // overridables
    virtual CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther);
    virtual CDocument* CreateNewDocument();
    virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE) = 0;
    virtual void SaveAllModified();
    virtual void CloseAllDocuments(BOOL bEndSession);
    virtual BOOL MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch);
    virtual CFrameWnd* CreateNewFrameFromResource(CDocument* pDoc, CFrameWnd* pOther);
    virtual void SetContainerInfo(UINT nIDOleInPlaceContainer) { (void)nIDOleInPlaceContainer; }
    virtual void SetDefaultTitle(CDocument* pDocument) = 0;

    // operations
    void AddDocument(CDocument* pDoc);
    virtual void RemoveDocument(CDocument* pDoc);
    POSITION GetFirstDocPosition() const;
    CDocument* GetNextDoc(POSITION& rPos) const;
    BOOL GetDocString(CString& rString, DocStringIndex index, HINSTANCE hInst = NULL) const;
    void LoadTemplate();
    CFrameWnd* GetFirstFrame();

    // attributes
    UINT        m_nIDResource;          // IDR_MAINFRAME style resource
    UINT        m_nIDServerResource;
    DocString   m_strDocStrings;
    BOOL        m_bAutoDelete;
    CRuntimeClass* m_pDocClass;
    CRuntimeClass* m_pFrameClass;
    CRuntimeClass* m_pViewClass;
    CRuntimeClass* m_pAttachedFactoryClass;

protected:
    CPtrList    m_docList;
    CString     m_strDefaultFileName;   // not used, kept for compatibility
};

// ---------------------------------------------------------------------
//  CSingleDocTemplate / CMultiDocTemplate
// ---------------------------------------------------------------------
class CSingleDocTemplate : public CDocTemplate
{
    DECLARE_DYNAMIC(CSingleDocTemplate)
public:
    CSingleDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
                       CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
    virtual ~CSingleDocTemplate();

    virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
    virtual void SetDefaultTitle(CDocument* pDocument);
    virtual void CloseAllDocuments(BOOL bEndSession);
    virtual void SaveAllModified();
    virtual void RemoveDocument(CDocument* pDoc);
    CDocument* GetDocument() const { return m_pOnlyDoc; }

protected:
    CDocument* m_pOnlyDoc;
};

class CMultiDocTemplate : public CDocTemplate
{
    DECLARE_DYNAMIC(CMultiDocTemplate)
public:
    CMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
                      CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
    virtual ~CMultiDocTemplate();

    virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
    virtual void SetDefaultTitle(CDocument* pDocument);
    virtual void CloseAllDocuments(BOOL bEndSession);
    virtual void SaveAllModified();
    virtual void RemoveDocument(CDocument* pDoc);
    int GetDocumentCount() { return m_docList.GetCount(); }
};

// ---------------------------------------------------------------------
//  CDocManager
// ---------------------------------------------------------------------
class CDocManager : public CObject
{
    DECLARE_DYNAMIC(CDocManager)
public:
    CDocManager();
    virtual ~CDocManager();

    virtual void AddDocTemplate(CDocTemplate* pTemplate);
    virtual POSITION GetFirstDocTemplatePosition() const;
    virtual CDocTemplate* GetNextDocTemplate(POSITION& pos) const;
    virtual void RegisterShellFileTypes(BOOL bCompat);
    virtual void UnregisterShellFileTypes();
    virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
    virtual BOOL SaveAllModified();
    virtual void CloseAllDocuments(BOOL bEndSession);
    virtual int GetOpenDocumentCount();
    virtual BOOL OnDDECommand(LPTSTR lpszCommand) { (void)lpszCommand; return FALSE; }
    virtual void OnFileNew();
    virtual void OnFileOpen();

    CPtrList m_templateList;
    int GetDocumentCount();

    static CPtrList* pStaticList;       // used before the app object exists
    static BOOL bStaticInit;
    static CDocManager* pStaticDocManager;
};

// helper used by CWinApp::ProcessShellCommand
BOOL AFXAPI AfxOpenDocument(CDocument* pDoc, LPCTSTR lpszPathName);
CString AFXAPI AfxGetExtension(LPCTSTR lpszPathName);
CString AFXAPI AfxGetFileTitle(LPCTSTR lpszPathName);
CString AFXAPI AfxGetFileName(LPCTSTR lpszPathName);
void AFXAPI AfxFullPath(LPTSTR lpszPath, LPCTSTR lpszInput);

#endif /* __AFXDOC_H__ */
