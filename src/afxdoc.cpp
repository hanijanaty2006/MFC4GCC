// =====================================================================
//  afxdoc.cpp -- Document/View implementation
// =====================================================================
#include <afxwin.h>
#include <afxcmn.h>
#include <afxext.h>
#include <afxdlgs.h>
#include <afxres.h>

// ---------------------------------------------------------------------
//  CPrintInfo
// ---------------------------------------------------------------------
CPrintInfo::CPrintInfo()
    : m_pPD(NULL), m_nCurPage(1), m_nNumPreviewPages(1), m_bPreview(FALSE),
      m_bContinuePrinting(TRUE), m_bDocObject(FALSE), m_nOffsetPage(0)
{
    m_pPD = new CPrintDialog(FALSE);
    m_rectDraw.SetRectEmpty();
}
CPrintInfo::~CPrintInfo() { delete m_pPD; m_pPD = NULL; }
void CPrintInfo::SetMinPage(UINT nMinPage) { if (m_pPD) m_pPD->m_pd.nMinPage = (WORD)nMinPage; }
void CPrintInfo::SetMaxPage(UINT nMaxPage) { if (m_pPD) m_pPD->m_pd.nMaxPage = (WORD)nMaxPage; }
UINT CPrintInfo::GetMinPage() const { return m_pPD ? m_pPD->m_pd.nMinPage : 1; }
UINT CPrintInfo::GetMaxPage() const { return m_pPD ? m_pPD->m_pd.nMaxPage : 0xFFFF; }
UINT CPrintInfo::GetFromPage() const { return m_pPD ? m_pPD->m_pd.nFromPage : 1; }
UINT CPrintInfo::GetToPage() const { return m_pPD ? m_pPD->m_pd.nToPage : 0xFFFF; }

// ---------------------------------------------------------------------
//  CView
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CView, CWnd)

CView::CView() : m_pDocument(NULL) {}
CView::~CView() {}

BEGIN_MESSAGE_MAP(CView, CWnd)
    ON_WM_PAINT()
    ON_WM_SETFOCUS()
    ON_WM_CREATE()
    ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
    ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, CView::OnUpdateFilePrint)
END_MESSAGE_MAP()

BOOL CView::PreCreateWindow(CREATESTRUCT& cs)
{
    if (cs.lpszClass == NULL)
        cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
    cs.style |= WS_CLIPCHILDREN;
    return CWnd::PreCreateWindow(cs);
}

int CView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;
    if (lpCreateStruct->lpCreateParams != NULL) {
        CCreateContext* pContext = (CCreateContext*)lpCreateStruct->lpCreateParams;
        if (pContext->m_pCurrentDoc != NULL) {
            m_pDocument = pContext->m_pCurrentDoc;
            m_pDocument->AddView(this);
        }
    }
    return 0;
}

void CView::PostNcDestroy()
{
    // a view deletes itself together with its window
    CDocument* pDoc = m_pDocument;
    m_pDocument = NULL;
    CWnd::PostNcDestroy();
    if (pDoc != NULL) pDoc->RemoveView(this);
    delete this;
}

void CView::OnPaint()
{
    CPaintDC dc(this);
    DoPrepareDC(&dc, NULL);
    OnDraw(&dc);
}

void CView::OnSetFocusView(CWnd* pOldWnd) { CWnd::OnSetFocus(pOldWnd); }

void CView::OnInitialUpdate()
{
    if (m_pDocument != NULL) {
        // nothing to do by default
    }
}

void CView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    (void)pSender; (void)lHint; (void)pHint;
    Invalidate(FALSE);
    UpdateWindow();
}

void CView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) { (void)pDC; (void)pInfo; }
void CView::OnPreparePrinting(CPrintInfo* pInfo) { (void)pInfo; }
void CView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) { (void)pDC; (void)pInfo; }
void CView::OnPrint(CDC* pDC, CPrintInfo* pInfo) { OnDraw(pDC); (void)pInfo; }
void CView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) { (void)pDC; (void)pInfo; }
void CView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView)
{ (void)pDC; (void)pInfo; (void)point; (void)pView; }

BOOL CView::DoPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
    OnPrepareDC(pDC, pInfo);
    return TRUE;
}

BOOL CView::DoPreparePrinting(CPrintInfo* pInfo)
{
    OnPreparePrinting(pInfo);
    return TRUE;
}

BOOL CView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    if (CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return TRUE;
    if (m_pDocument != NULL && m_pDocument->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return TRUE;
    return FALSE;
}

void CView::OnFilePrint()
{
    CPrintInfo info;
    if (!DoPreparePrinting(&info)) return;
    HDC hDC = info.m_pPD ? info.m_pPD->CreatePrinterDC() : NULL;
    if (hDC == NULL) { AfxMessageBox(_T("No printer available.")); return; }
    CDC dc;
    dc.Attach(hDC);
    dc.SetPrinting(TRUE);
    OnBeginPrinting(&dc, &info);
    int nFrom = (int)info.GetFromPage(), nTo = (int)info.GetToPage();
    if (nFrom < 1) nFrom = 1;
    int nMaxPage = (int)info.GetMaxPage();
    if (nMaxPage > 0 && nTo > nMaxPage) nTo = nMaxPage;
    if (nTo > nFrom + 99999) nTo = nFrom + 99999;
    dc.StartDoc(m_pDocument ? (LPCTSTR)m_pDocument->GetTitle() : _T("Document"));
    for (int nPage = nFrom; nPage <= nTo && info.m_bContinuePrinting; ++nPage) {
        info.m_nCurPage = nPage;
        if (dc.StartPage() < 0) break;
        info.m_rectDraw.SetRect(0, 0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));
        DoPrepareDC(&dc, &info);
        OnPrint(&dc, &info);
        dc.EndPage();
    }
    dc.EndDoc();
    OnEndPrinting(&dc, &info);
    dc.Detach();
    ::DeleteDC(hDC);
}

void CView::OnUpdateFilePrint(CCmdUI* pCmdUI) { pCmdUI->Enable(TRUE); }

// ---------------------------------------------------------------------
//  CScrollView
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CScrollView, CView)

CScrollView::CScrollView()
    : m_nMapMode(MM_TEXT), m_bCenter(FALSE), m_bTrack(FALSE), m_nTrackDir(0)
{
    m_sizeTotal.cx = m_sizeTotal.cy = 0;
    m_sizePage.cx = m_sizePage.cy = 0;
    m_sizeLine.cx = m_sizeLine.cy = 0;
    m_sizeTotalDev.cx = m_sizeTotalDev.cy = 0;
    m_sizePageDev.cx = m_sizePageDev.cy = 0;
    m_sizeLineDev.cx = m_sizeLineDev.cy = 0;
    m_sizeAllSingleLine.cx = m_sizeAllSingleLine.cy = 0;
    m_ptCenter.x = m_ptCenter.y = 0;
}
CScrollView::~CScrollView() {}

BEGIN_MESSAGE_MAP(CScrollView, CView)
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
END_MESSAGE_MAP()

void CScrollView::SetScaleToFitSize(SIZE sizeTotal)
{
    m_sizeTotal = sizeTotal;
    m_nMapMode = MM_ANISOTROPIC;
    m_sizePageDev = m_sizeAllSingleLine;
    m_sizeLineDev = m_sizeAllSingleLine;
    m_sizeTotalDev = sizeTotal;
    UpdateBars();
    Invalidate(TRUE);
}

void CScrollView::SetScrollSizes(int nMapMode, SIZE sizeTotal, const SIZE& sizePage, const SIZE& sizeLine)
{
    m_nMapMode = nMapMode;
    m_sizeTotal = sizeTotal;
    m_sizePage = sizePage;
    m_sizeLine = sizeLine;

    if (nMapMode == MM_TEXT) {
        m_sizeTotalDev = sizeTotal;
        m_sizePageDev = (sizePage.cx == -1 && sizePage.cy == -1) ? CSize(sizeTotal) : CSize(sizePage);
        m_sizeLineDev = (sizeLine.cx == -1 && sizeLine.cy == -1)
                          ? CSize(m_sizePageDev.cx / 10, m_sizePageDev.cy / 10) : CSize(sizeLine);
    } else {
        // metric map mode: let the DC convert logical units to device units
        CClientDC dc(this);
        dc.SetMapMode(nMapMode);
        SIZE sz = sizeTotal;
        ::LPtoDP(dc.GetSafeHdc(), (LPPOINT)&sz, 1);
        if (sz.cy < 0) sz.cy = -sz.cy;
        m_sizeTotalDev = sz;

        SIZE sp = sizePage;
        if (sp.cx == -1 && sp.cy == -1) sp = sz;
        else ::LPtoDP(dc.GetSafeHdc(), (LPPOINT)&sp, 1);
        if (sp.cy < 0) sp.cy = -sp.cy;
        m_sizePageDev = sp;

        SIZE sl = sizeLine;
        if (sl.cx == -1 && sl.cy == -1) { sl.cx = sp.cx / 10; sl.cy = sp.cy / 10; }
        else ::LPtoDP(dc.GetSafeHdc(), (LPPOINT)&sl, 1);
        if (sl.cy < 0) sl.cy = -sl.cy;
        m_sizeLineDev = sl;
    }
    if (m_sizePageDev.cx <= 0) m_sizePageDev.cx = 100;
    if (m_sizePageDev.cy <= 0) m_sizePageDev.cy = 100;
    if (m_sizeLineDev.cx <= 0) m_sizeLineDev.cx = 10;
    if (m_sizeLineDev.cy <= 0) m_sizeLineDev.cy = 10;
    UpdateBars();
}

CFrameWnd* CView::GetParentFrame() const
{
    CWnd* pParent = CWnd::GetParent();
    while (pParent != NULL && !pParent->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
        pParent = pParent->GetParent();
    return (CFrameWnd*)pParent;
}

void CScrollView::GetDeviceScrollSizes(int& nMapMode, SIZE& sizeTotal, SIZE& sizePage, SIZE& sizeLine) const
{
    nMapMode = m_nMapMode;
    sizeTotal = m_sizeTotalDev;
    sizePage = m_sizePageDev;
    sizeLine = m_sizeLineDev;
}

CSize CScrollView::GetDeviceScrollPosition() const
{
    SCROLLINFO si;
    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    int x = 0, y = 0;
    if (::GetScrollInfo(m_hWnd, SB_HORZ, &si)) x = si.nPos;
    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    if (::GetScrollInfo(m_hWnd, SB_VERT, &si)) y = si.nPos;
    return CSize(x, y);
}

void CScrollView::ScrollToPosition(POINT point)
{
    OnScrollBy(CSize(-point.x, -point.y), TRUE);
    UpdateWindow();
}

void CScrollView::CenterOnPoint(CPoint ptCenter)
{
    CRect rc;
    GetClientRect(&rc);
    int x = ptCenter.x - rc.Width() / 2;
    int y = ptCenter.y - rc.Height() / 2;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    ScrollToPosition(CPoint(x, y));
}

void CScrollView::UpdateBars()
{
    CRect rc;
    GetClientRect(&rc);
    int cx = rc.Width(), cy = rc.Height();

    SCROLLINFO si;
    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    si.nMin = 0;
    si.nMax = (m_sizeTotalDev.cx > 0) ? m_sizeTotalDev.cx - 1 : 0;
    si.nPage = (UINT)(m_sizePageDev.cx > 0 ? m_sizePageDev.cx : cx);
    si.fMask |= SIF_DISABLENOSCROLL;
    if (m_sizeTotalDev.cx > cx) { ::SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE); }
    else { si.nMax = 0; si.nPage = 1; ::SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE); }

    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL;
    si.nMin = 0;
    si.nMax = (m_sizeTotalDev.cy > 0) ? m_sizeTotalDev.cy - 1 : 0;
    si.nPage = (UINT)(m_sizePageDev.cy > 0 ? m_sizePageDev.cy : cy);
    if (m_sizeTotalDev.cy > cy) { ::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE); }
    else { si.nMax = 0; si.nPage = 1; ::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE); }
}

void CScrollView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    UpdateBars();
}

void CScrollView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
    CView::OnPrepareDC(pDC, pInfo);
    if (pDC == NULL) return;

    pDC->SetMapMode(m_nMapMode);
    CSize pos = GetDeviceScrollPosition();
    pDC->SetViewportOrg(-pos.cx, -pos.cy);
}

void CScrollView::OnPaint()
{
    CPaintDC dc(this);
    DoPrepareDC(&dc, NULL);
    CRect rcClip;
    dc.GetClipBox(&rcClip);
    dc.LPtoDP(&rcClip);
    OnDraw(&dc);
    // paint the area outside the document
    CRect rcClient;
    GetClientRect(&rcClient);
    if (m_sizeTotalDev.cx < rcClient.Width() || m_sizeTotalDev.cy < rcClient.Height()) {
        CBrush br;
        br.CreateSolidBrush(::GetSysColor(COLOR_APPWORKSPACE));
        if (m_sizeTotalDev.cx < rcClient.Width()) {
            CRect rc(m_sizeTotalDev.cx, 0, rcClient.Width(), rcClient.Height());
            dc.FillRect(&rc, &br);
        }
        if (m_sizeTotalDev.cy < rcClient.Height()) {
            CRect rc(0, m_sizeTotalDev.cy, m_sizeTotalDev.cx > rcClient.Width() ? m_sizeTotalDev.cx : rcClient.Width(), rcClient.Height());
            dc.FillRect(&rc, &br);
        }
    }
}

BOOL CScrollView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
    UINT nSBCode = LOWORD(nScrollCode);
    BOOL bHorz = (HIWORD(nScrollCode) != 0);
    CSize sizeScroll = GetDeviceScrollPosition();
    CSize sizeNew = sizeScroll;

    int nPage = bHorz ? m_sizePageDev.cx : m_sizePageDev.cy;
    int nLine = bHorz ? m_sizeLineDev.cx : m_sizeLineDev.cy;
    int nMax  = bHorz ? m_sizeTotalDev.cx : m_sizeTotalDev.cy;

    // note: SB_LINEUP == SB_LINELEFT, SB_PAGEUP == SB_PAGELEFT, SB_TOP == SB_LEFT, ...
    switch (nSBCode) {
    case SB_LINEUP:                       // SB_LINELEFT
        if (bHorz) sizeNew.cx -= nLine; else sizeNew.cy -= nLine;
        break;
    case SB_LINEDOWN:                     // SB_LINERIGHT
        if (bHorz) sizeNew.cx += nLine; else sizeNew.cy += nLine;
        break;
    case SB_PAGEUP:                       // SB_PAGELEFT
        if (bHorz) sizeNew.cx -= nPage; else sizeNew.cy -= nPage;
        break;
    case SB_PAGEDOWN:                     // SB_PAGERIGHT
        if (bHorz) sizeNew.cx += nPage; else sizeNew.cy += nPage;
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK: {
        SCROLLINFO si; memset(&si, 0, sizeof(si));
        si.cbSize = sizeof(si); si.fMask = SIF_TRACKPOS;
        ::GetScrollInfo(m_hWnd, bHorz ? SB_HORZ : SB_VERT, &si);
        if (bHorz) sizeNew.cx = si.nTrackPos; else sizeNew.cy = si.nTrackPos;
        (void)nPos;
        break; }
    case SB_TOP:                          // SB_LEFT
        if (bHorz) sizeNew.cx = 0; else sizeNew.cy = 0;
        break;
    case SB_BOTTOM:                       // SB_RIGHT
        if (bHorz) sizeNew.cx = nMax; else sizeNew.cy = nMax;
        break;
    default: return FALSE;
    }
    return OnScrollBy(sizeNew - sizeScroll, bDoScroll);
}

BOOL CScrollView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
    if (sizeScroll.cx == 0 && sizeScroll.cy == 0) return FALSE;
    CSize pos = GetDeviceScrollPosition();
    CSize newPos(pos.cx + sizeScroll.cx, pos.cy + sizeScroll.cy);
    if (newPos.cx < 0) newPos.cx = 0;
    if (newPos.cy < 0) newPos.cy = 0;
    if (m_sizeTotalDev.cx > 0 && newPos.cx > m_sizeTotalDev.cx - 1) newPos.cx = m_sizeTotalDev.cx - 1;
    if (m_sizeTotalDev.cy > 0 && newPos.cy > m_sizeTotalDev.cy - 1) newPos.cy = m_sizeTotalDev.cy - 1;

    if (!bDoScroll) return TRUE;

    int dx = pos.cx - newPos.cx, dy = pos.cy - newPos.cy;
    ScrollWindow(dx, dy);
    SCROLLINFO si;
    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si); si.fMask = SIF_POS; si.nPos = newPos.cx;
    ::SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
    si.nPos = newPos.cy;
    ::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
    UpdateWindow();
    return TRUE;
}

void CScrollView::OnHScroll(UINT nSBCode, UINT nPos, CWnd* pScrollBar)
{
    if (pScrollBar == NULL) OnScroll(MAKEWPARAM(nSBCode, 1), nPos);
    else CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CScrollView::OnVScroll(UINT nSBCode, UINT nPos, CWnd* pScrollBar)
{
    if (pScrollBar == NULL) OnScroll(MAKEWPARAM(nSBCode, 0), nPos);
    else CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CScrollView::OnMouseWheelScroll(UINT nFlags, short zDelta, CPoint pt)
{
    (void)nFlags; (void)pt;
    int nLines = 3;
    UINT nPage = (UINT)m_sizeLineDev.cy * nLines;
    OnScrollBy(CSize(0, -(zDelta / 120) * (int)nPage), TRUE);
    return TRUE;
}

void CScrollView::FillOutsideRect(CDC* pDC, CBrush* pBrush)
{
    CRect rc, rcClip;
    GetClientRect(&rc);
    pDC->GetClipBox(&rcClip);
    if (m_sizeTotalDev.cy < rc.bottom) {
        CRect rcFill(rc.left, m_sizeTotalDev.cy, rc.right, rc.bottom);
        pDC->FillRect(&rcFill, pBrush);
    }
    if (m_sizeTotalDev.cx < rc.right) {
        CRect rcFill(m_sizeTotalDev.cx, rcClip.top, rc.right, rcClip.bottom);
        pDC->FillRect(&rcFill, pBrush);
    }
}

// ---------------------------------------------------------------------
//  CDocument
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CDocument, CCmdTarget)

CDocument::CDocument() : m_pDocTemplate(NULL), m_bModified(FALSE), m_bAutoDelete(TRUE) {}
CDocument::~CDocument() {}

BEGIN_MESSAGE_MAP(CDocument, CCmdTarget)
    ON_COMMAND(ID_FILE_CLOSE, CDocument::OnFileClose)
    ON_COMMAND(ID_FILE_SAVE, CDocument::OnFileSave)
    ON_COMMAND(ID_FILE_SAVE_AS, CDocument::OnFileSaveAs)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, CDocument::OnUpdateFileSave)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, CDocument::OnUpdateFileSaveAs)
    ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, CDocument::OnUpdateFileClose)
END_MESSAGE_MAP()

void CDocument::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
    m_strPathName = lpszPathName;
    if (bAddToMRU) {
        CWinApp* pApp = AfxGetApp();
        if (pApp != NULL) pApp->AddToRecentFileList(lpszPathName);
    }
    // derive a title from the file name
    SetTitle(AfxGetFileTitle(lpszPathName));
}

void CDocument::SetTitle(LPCTSTR lpszTitle)
{
    m_strTitle = lpszTitle;
    OnChangedViewList();     // refresh frame titles
}

POSITION CDocument::GetFirstViewPosition() const { return m_viewList.GetHeadPosition(); }

CView* CDocument::GetNextView(POSITION& rPos) const
{
    if (rPos == NULL) return NULL;
    return (CView*)m_viewList.GetNext(rPos);
}

void CDocument::AddView(CView* pView)
{
    m_viewList.AddTail(pView);
    OnChangedViewList();
}

void CDocument::OnChangedViewList()
{
    if (m_viewList.GetCount() == 0) {
        if (m_pDocTemplate != NULL) m_pDocTemplate->RemoveDocument(this);
        else if (m_bAutoDelete) delete this;
        return;
    }
    // refresh the frame titles
    POSITION pos = GetFirstViewPosition();
    while (pos != NULL) {
        CView* pView = GetNextView(pos);
        CFrameWnd* pFrame = pView->GetParentFrame();
        if (pFrame != NULL) pFrame->UpdateFrameTitle(FALSE);
    }
}

void CDocument::UpdateAllViews(CView* pSender, LPARAM lHint, CObject* pHint)
{
    POSITION pos = GetFirstViewPosition();
    while (pos != NULL) {
        CView* pView = GetNextView(pos);
        if (pView != pSender) pView->OnUpdate(pSender, lHint, pHint);
    }
}

BOOL CDocument::OnNewDocument()
{
    DeleteContents();
    SetModifiedFlag(FALSE);
    return TRUE;
}

BOOL CDocument::OnOpenDocument(LPCTSTR lpszPathName)
{
    if (IsModified()) TRACE(_T("Warning: opening modified document\n"));

    CFile file;
    CFileException fe;
    if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe)) {
        TCHAR szErr[512];
        fe.GetErrorMessage(szErr, 512);
        AfxMessageBox(szErr, MB_ICONEXCLAMATION);
        return FALSE;
    }
    DeleteContents();
    CArchive loadArchive(&file, CArchive::load | CArchive::bNoFlushOnDelete);
    BOOL bResult = TRUE;
    try { Serialize(loadArchive); }
    catch (CException* e) { if (e) { e->ReportError(); e->Delete(); } bResult = FALSE; }
    catch (...) { bResult = FALSE; }
    loadArchive.Close();
    file.Close();

    if (bResult) {
        SetModifiedFlag(FALSE);
        SetPathName(lpszPathName, TRUE);
    }
    return bResult;
}

BOOL CDocument::OnSaveDocument(LPCTSTR lpszPathName)
{
    CFile file;
    CFileException fe;
    if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive, &fe)) {
        TCHAR szErr[512];
        fe.GetErrorMessage(szErr, 512);
        AfxMessageBox(szErr, MB_ICONEXCLAMATION);
        return FALSE;
    }
    CArchive saveArchive(&file, CArchive::store | CArchive::bNoFlushOnDelete);
    BOOL bResult = TRUE;
    try { Serialize(saveArchive); saveArchive.Flush(); }
    catch (CException* e) { if (e) { e->ReportError(); e->Delete(); } bResult = FALSE; }
    catch (...) { bResult = FALSE; }
    saveArchive.Close();
    file.Close();

    if (bResult) {
        SetModifiedFlag(FALSE);
        SetPathName(lpszPathName, TRUE);
    }
    return bResult;
}

BOOL CDocument::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
    CString newName = lpszPathName ? lpszPathName : _T("");
    if (newName.IsEmpty()) {
        newName = m_strPathName;
        if (newName.IsEmpty()) {
            // ask for a name
            CString strFilter;
            if (m_pDocTemplate != NULL) m_pDocTemplate->GetDocString(strFilter, CDocTemplate::fileNameFilterID);
            if (strFilter.IsEmpty()) strFilter = _T("All Files (*.*)|*.*||");
            CFileDialog dlg(FALSE, NULL, m_strTitle.IsEmpty() ? _T("untitled") : (LPCTSTR)m_strTitle,
                            OFN_HIDEREADONLY | (bReplace ? OFN_OVERWRITEPROMPT : 0), strFilter);
            if (dlg.DoModal() != IDOK) return FALSE;
            newName = dlg.GetPathName();
        }
    }
    CWaitCursor wait;
    return OnSaveDocument(newName);
}

BOOL CDocument::DoFileLoad()
{
    if (m_strPathName.IsEmpty()) return FALSE;
    return OnOpenDocument(m_strPathName);
}

BOOL CDocument::SaveModified()
{
    if (!IsModified()) return TRUE;
    CView* pView = NULL;
    POSITION pos = GetFirstViewPosition();
    if (pos != NULL) pView = GetNextView(pos);
    CFrameWnd* pFrame = (pView != NULL) ? pView->GetParentFrame() : NULL;

    CString strTitle = m_strTitle;
    CString prompt;
    prompt.Format(_T("Save changes to '%s'?"), (LPCTSTR)(strTitle.IsEmpty() ? _T("untitled") : strTitle));
    int nResult = AfxMessageBox(prompt, MB_YESNOCANCEL | MB_ICONQUESTION);
    if (nResult == IDCANCEL) return FALSE;
    if (nResult == IDNO) { SetModifiedFlag(FALSE); return TRUE; }
    (void)pFrame;
    return DoSave(NULL, TRUE);
}

void CDocument::DeleteContents() {}

void CDocument::CloseDocument()
{
    // destroy every frame that hosts one of our views (the frames tear the
    // views down, which in turn calls RemoveView() -> OnChangedViewList())
    std::vector<CFrameWnd*> frames;
    POSITION pos = GetFirstViewPosition();
    while (pos != NULL) {
        CView* pView = GetNextView(pos);
        CFrameWnd* pFrame = pView->GetParentFrame();
        if (pFrame != NULL) frames.push_back(pFrame);
    }
    for (size_t i = 0; i < frames.size(); ++i) {
        if (frames[i]->GetSafeHwnd() != NULL) frames[i]->DestroyWindow();
    }
    if (m_viewList.GetCount() == 0 && m_pDocTemplate == NULL && m_bAutoDelete)
        delete this;
}

void CDocument::PreCloseFrame() {}

void CDocument::Serialize(CArchive& ar) { CObject::Serialize(ar); }

void CDocument::OnFileClose()
{
    if (!SaveModified()) return;
    CloseDocument();
}

void CDocument::RemoveView(CView* pView)
{
    POSITION pos = m_viewList.Find(pView);
    if (pos != NULL) m_viewList.RemoveAt(pos);
    OnChangedViewList();
}

void CDocument::OnFileSave() { if (!DoSave(NULL, TRUE)) TRACE(_T("Save failed\n")); }
void CDocument::OnFileSaveAs() { if (!DoSave(NULL, FALSE)) TRACE(_T("Save As cancelled\n")); }
void CDocument::OnUpdateFileSave(CCmdUI* pCmdUI) { pCmdUI->Enable(TRUE); }
void CDocument::OnUpdateFileSaveAs(CCmdUI* pCmdUI) { pCmdUI->Enable(TRUE); }
void CDocument::OnUpdateFileClose(CCmdUI* pCmdUI) { pCmdUI->Enable(TRUE); }

// ---------------------------------------------------------------------
//  CDocTemplate
// ---------------------------------------------------------------------
CDocTemplate::CDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
                           CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
    : m_nIDResource(nIDResource), m_nIDServerResource(0), m_bAutoDelete(TRUE),
      m_pDocClass(pDocClass), m_pFrameClass(pFrameClass), m_pViewClass(pViewClass),
      m_pAttachedFactoryClass(NULL)
{
    memset(&m_strDocStrings, 0, sizeof(m_strDocStrings));
    LoadTemplate();
}

CDocTemplate::~CDocTemplate()
{
    // documents are deleted by CloseAllDocuments()
}

void CDocTemplate::LoadTemplate()
{
    // the string resource holds the \n separated doc-template strings:
    //   window title \n doc name \n file new name \n filter name \n filter ext \n
    //   shell file type \n shell new \n shell open \n shell open with \n shell DDE \n DDE topic
    CString str;
    if (m_nIDResource != 0) str.LoadString(m_nIDResource);
    if (str.IsEmpty()) return;

    CString fields[11];
    int nStart = 0;
    for (int i = 0; i < 11; ++i) {
        int nSep = str.Find('\n', nStart);
        if (nSep < 0) { fields[i] = str.Mid(nStart); nStart = str.GetLength(); }
        else { fields[i] = str.Mid(nStart, nSep - nStart); nStart = nSep + 1; }
    }
    m_strDocStrings.m_lpszClass          = NULL;
    m_strDocStrings.m_lpszWindowMenu     = NULL;
    m_strDocStrings.m_lpszFilterName     = fields[1].IsEmpty() ? NULL : _tcsdup((LPCTSTR)fields[1]);
    m_strDocStrings.m_lpszFileNewName    = fields[2].IsEmpty() ? NULL : _tcsdup((LPCTSTR)fields[2]);
    m_strDocStrings.m_lpszFilterExt      = fields[3].IsEmpty() ? NULL : _tcsdup((LPCTSTR)fields[3]);
    m_strDocStrings.m_lpszShellNew       = fields[6].IsEmpty() ? NULL : _tcsdup((LPCTSTR)fields[6]);
    m_strDocStrings.m_lpszShellOpen      = fields[7].IsEmpty() ? NULL : _tcsdup((LPCTSTR)fields[7]);
    m_strDocStrings.m_lpszShellPrint     = NULL;
    m_strDocStrings.m_lpszShellPrintTo   = NULL;
    m_strDocStrings.m_lpszShellDDE       = fields[9].IsEmpty() ? NULL : _tcsdup((LPCTSTR)fields[9]);
}

BOOL CDocTemplate::GetDocString(CString& rString, DocStringIndex index, HINSTANCE hInst) const
{
    (void)hInst;
    rString.Empty();
    CString str;
    if (m_nIDResource != 0) str.LoadString(m_nIDResource);
    if (str.IsEmpty()) return FALSE;
    switch (index) {
    // MFC substring layout: 0=window title, 1=filter name, 2=File/New name,
    //                       3=extension, 4=view name, 5=shell file type, ...
    case windowTitle:      return AfxExtractSubString(rString, str, 0, '\n') && !rString.IsEmpty();
    case fileNameFilterID: return AfxExtractSubString(rString, str, 1, '\n') && !rString.IsEmpty();
    case fileNewName:      return AfxExtractSubString(rString, str, 2, '\n') && !rString.IsEmpty();
    case fileExt:          return AfxExtractSubString(rString, str, 3, '\n') && !rString.IsEmpty();
    case shellFileTypeName:return AfxExtractSubString(rString, str, 5, '\n') && !rString.IsEmpty();
    case shellNewFileName: return AfxExtractSubString(rString, str, 6, '\n') && !rString.IsEmpty();
    case shellOpenCommand: return AfxExtractSubString(rString, str, 7, '\n') && !rString.IsEmpty();
    case shellOpenWithCmd: return AfxExtractSubString(rString, str, 8, '\n') && !rString.IsEmpty();
    case shellDDECmd:      return AfxExtractSubString(rString, str, 9, '\n') && !rString.IsEmpty();
    case shellDDETopic:    return AfxExtractSubString(rString, str, 10, '\n') && !rString.IsEmpty();
    default: break;
    }
    return FALSE;
}

CDocument* CDocTemplate::CreateNewDocument()
{
    if (m_pDocClass == NULL) return NULL;
    CDocument* pDoc = (CDocument*)m_pDocClass->CreateObject();
    if (pDoc == NULL) { AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC); return NULL; }
    AddDocument(pDoc);
    return pDoc;
}

CFrameWnd* CDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
{
    (void)pOther;
    if (m_pFrameClass == NULL) return NULL;
    CFrameWnd* pFrame = (CFrameWnd*)m_pFrameClass->CreateObject();
    if (pFrame == NULL) { AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC); return NULL; }

    CCreateContext context;
    context.m_pCurrentView = NULL;
    context.m_pCurrentDoc = pDoc;
    context.m_pNewViewClass = m_pViewClass;
    context.m_pNewDocTemplate = this;

    if (!pFrame->LoadFrame(m_nIDResource, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, &context)) {
        TRACE(_T("Warning: CDocTemplate couldn't create a frame.\n"));
        delete pFrame;
        return NULL;
    }
    return pFrame;
}

CFrameWnd* CDocTemplate::CreateNewFrameFromResource(CDocument* pDoc, CFrameWnd* pOther)
{ return CreateNewFrame(pDoc, pOther); }

void CDocTemplate::AddDocument(CDocument* pDoc)
{
    m_docList.AddTail(pDoc);
    pDoc->SetDocTemplate(this);
    CWinApp* pApp = AfxGetApp();
    if (pApp != NULL) pApp->EnableShellOpen();
}

void CDocTemplate::RemoveDocument(CDocument* pDoc)
{
    POSITION pos = m_docList.Find(pDoc);
    if (pos != NULL) m_docList.RemoveAt(pos);
}

POSITION CDocTemplate::GetFirstDocPosition() const { return m_docList.GetHeadPosition(); }

CDocument* CDocTemplate::GetNextDoc(POSITION& rPos) const
{
    if (rPos == NULL) return NULL;
    return (CDocument*)m_docList.GetNext(rPos);
}

void CDocTemplate::SaveAllModified()
{
    POSITION pos = GetFirstDocPosition();
    while (pos != NULL) {
        CDocument* pDoc = GetNextDoc(pos);
        if (!pDoc->SaveModified()) return;
    }
}

void CDocTemplate::CloseAllDocuments(BOOL bEndSession)
{
    POSITION pos = GetFirstDocPosition();
    while (pos != NULL) {
        CDocument* pDoc = GetNextDoc(pos);
        if (!pDoc->SaveModified()) return;
        if (bEndSession) pDoc->PreCloseFrame();
        pDoc->OnFileClose();
    }
}

CFrameWnd* CDocTemplate::GetFirstFrame()
{
    POSITION posDoc = GetFirstDocPosition();
    while (posDoc != NULL) {
        CDocument* pDoc = GetNextDoc(posDoc);
        POSITION posView = pDoc->GetFirstViewPosition();
        while (posView != NULL) {
            CView* pView = pDoc->GetNextView(posView);
            CFrameWnd* pFrame = pView->GetParentFrame();
            if (pFrame != NULL) return pFrame;
        }
    }
    return NULL;
}

BOOL CDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)
{
    rpDocMatch = NULL;
    if (lpszPathName == NULL) return FALSE;

    // already open?
    POSITION pos = GetFirstDocPosition();
    while (pos != NULL) {
        CDocument* pDoc = GetNextDoc(pos);
        if (pDoc->GetPathName().CompareNoCase(lpszPathName) == 0) { rpDocMatch = pDoc; return TRUE; }
    }
    // extension match
    CString strFilterExt;
    if (GetDocString(strFilterExt, fileExt) && !strFilterExt.IsEmpty()) {
        CString strExt = AfxGetExtension(lpszPathName);
        if (strExt.CompareNoCase(strFilterExt) == 0) return TRUE;
    }
    return FALSE;
}

// ---------------------------------------------------------------------
//  CSingleDocTemplate
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CSingleDocTemplate, CObject)

CSingleDocTemplate::CSingleDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
                                       CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
    : CDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass), m_pOnlyDoc(NULL)
{
}
CSingleDocTemplate::~CSingleDocTemplate() {}

CDocument* CSingleDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
    if (m_pOnlyDoc == NULL) {
        m_pOnlyDoc = CreateNewDocument();
        if (m_pOnlyDoc == NULL) return NULL;
        CFrameWnd* pFrame = CreateNewFrame(m_pOnlyDoc, NULL);
        if (pFrame == NULL) { m_pOnlyDoc = NULL; return NULL; }
        if (lpszPathName == NULL) {
            if (!m_pOnlyDoc->OnNewDocument()) { AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC); return NULL; }
            SetDefaultTitle(m_pOnlyDoc);
        } else {
            m_pOnlyDoc->SetPathName(lpszPathName, FALSE);
            if (!m_pOnlyDoc->OnOpenDocument(lpszPathName)) {
                m_pOnlyDoc->SetPathName(NULL, FALSE);
                if (!m_pOnlyDoc->OnNewDocument()) return NULL;
                SetDefaultTitle(m_pOnlyDoc);
                return NULL;
            }
            if (bAddToMRU) {
                CWinApp* pApp = AfxGetApp();
                if (pApp != NULL) pApp->AddToRecentFileList(lpszPathName);
            }
        }
        pFrame->InitialUpdateFrame(m_pOnlyDoc, TRUE);
        return m_pOnlyDoc;
    }

    // a document already exists
    CView* pView = NULL;
    POSITION pos = m_pOnlyDoc->GetFirstViewPosition();
    if (pos != NULL) pView = m_pOnlyDoc->GetNextView(pos);
    CFrameWnd* pFrame = (pView != NULL) ? pView->GetParentFrame() : NULL;

    if (!m_pOnlyDoc->SaveModified()) return NULL;

    m_pOnlyDoc->DeleteContents();
    if (lpszPathName == NULL) {
        if (!m_pOnlyDoc->OnNewDocument()) return NULL;
        SetDefaultTitle(m_pOnlyDoc);
    } else {
        m_pOnlyDoc->SetPathName(lpszPathName, FALSE);
        if (!m_pOnlyDoc->OnOpenDocument(lpszPathName)) {
            m_pOnlyDoc->SetPathName(NULL, FALSE);
            return NULL;
        }
        if (bAddToMRU) {
            CWinApp* pApp = AfxGetApp();
            if (pApp != NULL) pApp->AddToRecentFileList(lpszPathName);
        }
    }
    if (pFrame != NULL) {
        pFrame->SetActiveView(pView, FALSE);
        pFrame->InitialUpdateFrame(m_pOnlyDoc, TRUE);
    }
    return m_pOnlyDoc;
}

void CSingleDocTemplate::SetDefaultTitle(CDocument* pDocument)
{
    CString strTitle;
    if (!GetDocString(strTitle, windowTitle) || strTitle.IsEmpty()) {
        CWinApp* pApp = AfxGetApp();
        strTitle = (pApp != NULL && pApp->m_pszAppName != NULL) ? pApp->m_pszAppName : _T("Untitled");
    }
    pDocument->SetTitle(strTitle);
}

void CSingleDocTemplate::RemoveDocument(CDocument* pDoc)
{
    if (pDoc == m_pOnlyDoc) m_pOnlyDoc = NULL;
    CDocTemplate::RemoveDocument(pDoc);
    if (pDoc != NULL && pDoc->m_bAutoDelete) delete pDoc;
}

void CSingleDocTemplate::CloseAllDocuments(BOOL bEndSession) { CDocTemplate::CloseAllDocuments(bEndSession); }
void CSingleDocTemplate::SaveAllModified() { CDocTemplate::SaveAllModified(); }

// ---------------------------------------------------------------------
//  CMultiDocTemplate
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CMultiDocTemplate, CObject)

CMultiDocTemplate::CMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
                                     CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
    : CDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}
CMultiDocTemplate::~CMultiDocTemplate() {}

CDocument* CMultiDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
    CDocument* pDoc = CreateNewDocument();
    if (pDoc == NULL) return NULL;
    CFrameWnd* pFrame = CreateNewFrame(pDoc, NULL);
    if (pFrame == NULL) { pDoc->CloseDocument(); return NULL; }

    if (lpszPathName == NULL) {
        if (!pDoc->OnNewDocument()) { AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC); return NULL; }
        SetDefaultTitle(pDoc);
    } else {
        pDoc->SetPathName(lpszPathName, FALSE);
        if (!pDoc->OnOpenDocument(lpszPathName)) {
            pFrame->DestroyWindow();
            return NULL;
        }
        if (bAddToMRU) {
            CWinApp* pApp = AfxGetApp();
            if (pApp != NULL) pApp->AddToRecentFileList(lpszPathName);
        }
    }
    pFrame->InitialUpdateFrame(pDoc, TRUE);
    return pDoc;
}

void CMultiDocTemplate::SetDefaultTitle(CDocument* pDocument)
{
    CString strTitle;
    GetDocString(strTitle, fileNewName);
    if (strTitle.IsEmpty()) {
        CWinApp* pApp = AfxGetApp();
        strTitle = (pApp != NULL && pApp->m_pszAppName != NULL) ? CString(pApp->m_pszAppName) : CString(_T("Document"));
    }
    // number the documents: "Doc1", "Doc2", ...
    static int s_nDocCount = 0;
    ++s_nDocCount;
    CString strNumbered;
    strNumbered.Format(_T("%s%d"), (LPCTSTR)strTitle, s_nDocCount);
    pDocument->SetTitle(strNumbered);
}

void CMultiDocTemplate::RemoveDocument(CDocument* pDoc)
{
    CDocTemplate::RemoveDocument(pDoc);
    if (pDoc != NULL && pDoc->m_bAutoDelete) delete pDoc;
}

void CMultiDocTemplate::CloseAllDocuments(BOOL bEndSession) { CDocTemplate::CloseAllDocuments(bEndSession); }
void CMultiDocTemplate::SaveAllModified() { CDocTemplate::SaveAllModified(); }

// ---------------------------------------------------------------------
//  CDocManager
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CDocManager, CObject)

CPtrList* CDocManager::pStaticList = NULL;
BOOL CDocManager::bStaticInit = FALSE;
CDocManager* CDocManager::pStaticDocManager = NULL;

CDocManager::CDocManager()
{
    if (pStaticList != NULL) {
        // templates created before the app object existed
        POSITION pos = pStaticList->GetHeadPosition();
        while (pos != NULL) m_templateList.AddTail(pStaticList->GetNext(pos));
        delete pStaticList;
        pStaticList = NULL;
        pStaticDocManager = NULL;
    }
}

CDocManager::~CDocManager() {}

void CDocManager::AddDocTemplate(CDocTemplate* pTemplate)
{
    if (pTemplate == NULL) return;
    m_templateList.AddTail(pTemplate);
}

POSITION CDocManager::GetFirstDocTemplatePosition() const { return m_templateList.GetHeadPosition(); }

CDocTemplate* CDocManager::GetNextDocTemplate(POSITION& pos) const
{
    if (pos == NULL) return NULL;
    return (CDocTemplate*)m_templateList.GetNext(pos);
}

int CDocManager::GetDocumentCount()
{
    int nCount = 0;
    POSITION posT = GetFirstDocTemplatePosition();
    while (posT != NULL) {
        CDocTemplate* pTemplate = GetNextDocTemplate(posT);
        POSITION posD = pTemplate->GetFirstDocPosition();
        while (posD != NULL) { pTemplate->GetNextDoc(posD); ++nCount; }
    }
    return nCount;
}

CDocument* CDocManager::OpenDocumentFile(LPCTSTR lpszFileName)
{
    if (lpszFileName == NULL) { OnFileNew(); return NULL; }

    // find the template that matches the file type
    CDocTemplate* pBest = NULL;
    CDocument* pMatch = NULL;
    POSITION pos = GetFirstDocTemplatePosition();
    while (pos != NULL) {
        CDocTemplate* pTemplate = GetNextDocTemplate(pos);
        CDocument* pDoc = NULL;
        if (pTemplate->MatchDocType(lpszFileName, pDoc)) {
            pBest = pTemplate;
            pMatch = pDoc;
            if (pDoc != NULL) break;          // already open
        }
    }
    if (pMatch != NULL) {
        // activate the existing document
        POSITION posV = pMatch->GetFirstViewPosition();
        CView* pView = (posV != NULL) ? pMatch->GetNextView(posV) : NULL;
        CFrameWnd* pFrame = (pView != NULL) ? pView->GetParentFrame() : NULL;
        if (pFrame != NULL) {
            pFrame->ActivateFrame(SW_SHOW);
            pFrame->SetForegroundWindow();
        }
        return pMatch;
    }
    if (pBest == NULL) {
        // fall back to the first template
        pos = GetFirstDocTemplatePosition();
        if (pos != NULL) pBest = GetNextDocTemplate(pos);
    }
    if (pBest == NULL) {
        AfxMessageBox(_T("No document template is available to open this file."), MB_ICONEXCLAMATION);
        return NULL;
    }
    return pBest->OpenDocumentFile(lpszFileName, TRUE);
}

BOOL CDocManager::SaveAllModified()
{
    POSITION pos = GetFirstDocTemplatePosition();
    while (pos != NULL) {
        CDocTemplate* pTemplate = GetNextDocTemplate(pos);
        pTemplate->SaveAllModified();
    }
    return TRUE;
}

void CDocManager::CloseAllDocuments(BOOL bEndSession)
{
    POSITION pos = GetFirstDocTemplatePosition();
    while (pos != NULL) {
        CDocTemplate* pTemplate = GetNextDocTemplate(pos);
        pTemplate->CloseAllDocuments(bEndSession);
    }
}

int CDocManager::GetOpenDocumentCount() { return GetDocumentCount(); }

void CDocManager::OnFileNew()
{
    if (m_templateList.IsEmpty()) {
        TRACE(_T("No document templates registered.\n"));
        return;
    }
    CDocTemplate* pTemplate = NULL;
    if (m_templateList.GetCount() > 1) {
        // ask the user which document type to create
        CDocTemplate* pBest = NULL;
        int nBest = 0;
        POSITION pos = GetFirstDocTemplatePosition();
        while (pos != NULL) {
            CDocTemplate* p = GetNextDocTemplate(pos);
            CString strName;
            if (p->GetDocString(strName, CDocTemplate::fileNewName) && !strName.IsEmpty()) { ++nBest; pBest = p; }
        }
        pTemplate = (nBest == 1) ? pBest : (CDocTemplate*)m_templateList.GetHead();
    } else {
        pTemplate = (CDocTemplate*)m_templateList.GetHead();
    }
    if (pTemplate != NULL) pTemplate->OpenDocumentFile(NULL);
}

void CDocManager::OnFileOpen()
{
    CString strFilter;
    POSITION pos = GetFirstDocTemplatePosition();
    while (pos != NULL) {
        CDocTemplate* pTemplate = GetNextDocTemplate(pos);
        CString str;
        if (pTemplate->GetDocString(str, CDocTemplate::fileNameFilterID) && !str.IsEmpty()) {
            if (!strFilter.IsEmpty()) strFilter += _T("|");
            strFilter += str;
            CString strExt;
            if (pTemplate->GetDocString(strExt, CDocTemplate::fileExt) && !strExt.IsEmpty())
                strFilter += CString(_T("|*.")) + strExt + _T("|");
            else
                strFilter += _T("|*.*|");
        }
    }
    if (strFilter.IsEmpty()) strFilter = _T("All Files (*.*)|*.*||");

    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
    if (dlg.DoModal() != IDOK) return;
    OpenDocumentFile(dlg.GetPathName());
}

void CDocManager::RegisterShellFileTypes(BOOL bCompat) { (void)bCompat; }
void CDocManager::UnregisterShellFileTypes() {}

// ---------------------------------------------------------------------
//  Path helpers
// ---------------------------------------------------------------------
CString AFXAPI AfxGetExtension(LPCTSTR lpszPathName)
{
    LPCTSTR pDot = _tcsrchr(lpszPathName, '.');
    if (pDot == NULL) return CString();
    // ignore a dot that is part of the directory name
    LPCTSTR pSlash = _tcsrchr(lpszPathName, '\\');
    LPCTSTR pSlash2 = _tcsrchr(lpszPathName, '/');
    if (pSlash2 > pSlash) pSlash = pSlash2;
    if (pSlash != NULL && pDot < pSlash) return CString();
    return CString(pDot + 1);
}

CString AFXAPI AfxGetFileName(LPCTSTR lpszPathName)
{
    LPCTSTR p = lpszPathName;
    LPCTSTR pLast = p;
    while (*p != 0) { if (*p == '\\' || *p == '/') pLast = p + 1; ++p; }
    return CString(pLast);
}

CString AFXAPI AfxGetFileTitle(LPCTSTR lpszPathName)
{
    CString strName = AfxGetFileName(lpszPathName);
    int nDot = strName.ReverseFind('.');
    if (nDot > 0) return strName.Left(nDot);
    return strName;
}

void AFXAPI AfxFullPath(LPTSTR lpszPath, LPCTSTR lpszInput)
{
    if (::GetFullPathName(lpszInput, _MAX_PATH, lpszPath, NULL) == 0)
        AfxStrCopy(lpszPath, _MAX_PATH, lpszInput);
}

BOOL AFXAPI AfxOpenDocument(CDocument* pDoc, LPCTSTR lpszPathName)
{
    if (pDoc == NULL) return FALSE;
    return pDoc->OnOpenDocument(lpszPathName);
}
