// =====================================================================
//  afxdlgs.cpp -- common dialog wrappers + property sheets
// =====================================================================
#include <afxwin.h>
#include <afxcmn.h>
#include <afxext.h>
#include <afxdlgs.h>
#include <afxres.h>
#include <prsht.h>

// ---------------------------------------------------------------------
//  CFileDialog
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CFileDialog, CDialog)

UINT_PTR CALLBACK AfxCommDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

CFileDialog::CFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
                         DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd)
    : CDialog(), m_bOpenFileDialog(bOpenFileDialog), m_bMultiSelect(FALSE)
{
    memset(&m_ofn, 0, sizeof(m_ofn));
    memset(m_szFileBuffer, 0, sizeof(m_szFileBuffer));
    memset(m_szFileTitle, 0, sizeof(m_szFileTitle));
    m_ofn.lStructSize = sizeof(m_ofn);
    m_ofn.hwndOwner = (pParentWnd != NULL) ? pParentWnd->GetSafeHwnd() : AfxGetMainHwnd();
    m_ofn.hInstance = AfxGetResourceHandle();

    // build the filter: MFC uses '|' separated strings, Win32 uses '\0'
    m_strFilter = (lpszFilter != NULL) ? lpszFilter : _T("All Files (*.*)|*.*||");
    LPTSTR pFilter = m_strFilter.GetBuffer();
    for (int i = 0; i < m_strFilter.GetLength(); ++i)
        if (pFilter[i] == '|') pFilter[i] = 0;
    m_strFilter.ReleaseBuffer(m_strFilter.GetLength());
    m_ofn.lpstrFilter = (LPCTSTR)m_strFilter;

    m_ofn.lpstrFile = m_szFileBuffer;
    m_ofn.nMaxFile = (sizeof(m_szFileBuffer) / sizeof(TCHAR)) - 1;
    m_ofn.lpstrFileTitle = m_szFileTitle;
    m_ofn.nMaxFileTitle = _MAX_PATH;
    m_ofn.lpstrDefExt = lpszDefExt;
    m_ofn.Flags = dwFlags | OFN_ENABLEHOOK | OFN_EXPLORER;
    m_ofn.lpfnHook = AfxCommDlgProc;
    m_ofn.lCustData = 0;         // set in DoModal()

    if (lpszFileName != NULL) AfxStrCopy(m_szFileBuffer, m_ofn.nMaxFile, lpszFileName);
    if ((dwFlags & OFN_ALLOWMULTISELECT) != 0) m_bMultiSelect = TRUE;
}

CFileDialog::~CFileDialog() {}

BEGIN_MESSAGE_MAP(CFileDialog, CDialog)
END_MESSAGE_MAP()

INT_PTR CFileDialog::DoModal()
{
    m_bModal = TRUE;
    if (m_ofn.hwndOwner == NULL) m_ofn.hwndOwner = AfxGetMainHwnd();
    m_ofn.lCustData = (LPARAM)this;

    BOOL bResult;
    if (m_bOpenFileDialog) bResult = ::GetOpenFileName(&m_ofn);
    else                   bResult = ::GetSaveFileName(&m_ofn);

    if (m_hWnd != NULL) { AfxUnregisterWnd(m_hWnd); m_hWnd = NULL; }
    m_bModal = FALSE;
    return bResult ? IDOK : IDCANCEL;
}

UINT_PTR CALLBACK AfxCommDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    (void)wParam;
    CFileDialog* pDlg = (CFileDialog*)::GetWindowLongPtr(hDlg, DWLP_USER);
    if (message == WM_INITDIALOG) {
        OPENFILENAME* pOFN = (OPENFILENAME*)lParam;
        pDlg = (CFileDialog*)(pOFN ? pOFN->lCustData : 0);
        ::SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)pDlg);
        if (pDlg != NULL) {
            pDlg->m_hWnd = hDlg;
            AfxRegisterWnd(hDlg, pDlg);
        }
        return TRUE;
    }
    if (pDlg == NULL) return FALSE;

    if (message == WM_NOTIFY) {
        OFNOTIFY* pNotify = (OFNOTIFY*)lParam;
        if (pNotify == NULL || pNotify->lpOFN == NULL) return FALSE;
        switch (pNotify->hdr.code) {
        case CDN_FILEOK:
            if (pDlg->OnFileNameOK()) { ::SetWindowLongPtr(hDlg, DWLP_MSGRESULT, -1); return TRUE; }
            break;
        case CDN_FOLDERCHANGE:
            pDlg->OnFolderChange();
            break;
        case CDN_SELCHANGE:
            pDlg->OnFileName();
            break;
        case CDN_TYPECHANGE:
            pDlg->OnTypeChange();
            break;
        case CDN_INITDONE:
            break;
        case CDN_SHAREVIOLATION:
            pDlg->OnShareViolation(pNotify->pszFile);
            break;
        default: break;
        }
        return FALSE;
    }
    if (message == WM_DESTROY) {
        if (pDlg->m_hWnd == hDlg) { pDlg->m_hWnd = NULL; AfxUnregisterWnd(hDlg); }
        return FALSE;
    }
    return FALSE;
}

void CFileDialog::OnFileName() {}
void CFileDialog::OnFolderChange() {}
void CFileDialog::OnLBSelChangedNotify(UINT nCodeBox, UINT nCurSel, UINT nPrevSel)
{ (void)nCodeBox; (void)nCurSel; (void)nPrevSel; }
void CFileDialog::OnTypeChange() {}
UINT CFileDialog::OnShareViolation(LPCTSTR lpszPathName) { (void)lpszPathName; return 0; }
BOOL CFileDialog::OnFileNameOK() { return FALSE; }
void CFileDialog::OnOK() {}
void CFileDialog::OnCancel() {}

CString CFileDialog::GetPathName() const
{
    if (!m_bMultiSelect) return CString(m_ofn.lpstrFile);
    // multi-select: "dir\0file1\0file2\0\0"
    POSITION pos = GetStartPosition();
    if (pos == NULL) return CString(m_ofn.lpstrFile);
    return GetNextPathName(pos);
}

CString CFileDialog::GetFileName() const { return CString(m_szFileTitle); }
CString CFileDialog::GetFileExt() const  { return CString(m_ofn.lpstrFile + m_ofn.nFileExtension); }
CString CFileDialog::GetFileTitle() const { return AfxGetFileTitle(m_ofn.lpstrFile); }
CString CFileDialog::GetSpec() const
{
    CString str = m_ofn.lpstrFile;
    int nSlash = str.ReverseFind('\\');
    int nSlash2 = str.ReverseFind('/');
    if (nSlash2 > nSlash) nSlash = nSlash2;
    if (nSlash < 0) return str;
    return str.Mid(nSlash + 1);
}

CString CFileDialog::GetFolderPath() const
{
    CString strPath = m_ofn.lpstrFile;
    int nSlash = strPath.ReverseFind('\\');
    int nSlash2 = strPath.ReverseFind('/');
    if (nSlash2 > nSlash) nSlash = nSlash2;
    if (nSlash < 0) return CString();
    return strPath.Left(nSlash);
}

BOOL CFileDialog::GetReadOnlyPref() const { return (m_ofn.Flags & OFN_READONLY) != 0; }

POSITION CFileDialog::GetStartPosition() const
{
    LPCTSTR p = m_ofn.lpstrFile;
    if (p == NULL || *p == 0) return NULL;
    return (POSITION)p;
}

CString CFileDialog::GetNextPathName(POSITION& pos) const
{
    LPCTSTR p = (LPCTSTR)pos;
    if (p == NULL || *p == 0) { pos = NULL; return CString(); }

    CString strToken = p;
    p += strToken.GetLength() + 1;
    pos = (*p == 0) ? NULL : (POSITION)p;

    if (!m_bMultiSelect) return strToken;

    // multi-select: the first token is the folder, the rest are file names
    if (strToken.Find('\\') >= 0 || strToken.Find('/') >= 0 || strToken.Find(':') >= 0) {
        m_strLastDir = strToken;
        if (pos == NULL) return strToken;         // only one file selected
        return GetNextPathName(pos);
    }
    if (m_strLastDir.IsEmpty()) return strToken;
    TCHAR chLast = m_strLastDir[m_strLastDir.GetLength() - 1];
    if (chLast == '\\' || chLast == '/') return m_strLastDir + strToken;
    return m_strLastDir + _T("\\") + strToken;
}

// ---------------------------------------------------------------------
//  CColorDialog
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CColorDialog, CDialog)

static COLORREF s_aCustomColors[16] = {
    RGB(255,255,255), RGB(255,255,255), RGB(255,255,255), RGB(255,255,255),
    RGB(255,255,255), RGB(255,255,255), RGB(255,255,255), RGB(255,255,255),
    RGB(255,255,255), RGB(255,255,255), RGB(255,255,255), RGB(255,255,255),
    RGB(255,255,255), RGB(255,255,255), RGB(255,255,255), RGB(255,255,255)
};

COLORREF* CColorDialog::GetCustomColors() { return s_aCustomColors; }

CColorDialog::CColorDialog(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd) : CDialog()
{
    memset(&m_cc, 0, sizeof(m_cc));
    m_cc.lStructSize = sizeof(m_cc);
    m_cc.hwndOwner = (pParentWnd != NULL) ? pParentWnd->GetSafeHwnd() : AfxGetMainHwnd();
    m_cc.lpCustColors = (LPDWORD)s_aCustomColors;
    m_cc.rgbResult = clrInit;
    m_cc.Flags = dwFlags | CC_RGBINIT | CC_FULLOPEN;
}

COLORREF CColorDialog::GetColor() { return s_aCustomColors[0]; }

INT_PTR CColorDialog::DoModal()
{
    m_bModal = TRUE;
    if (m_cc.hwndOwner == NULL) m_cc.hwndOwner = AfxGetMainHwnd();
    BOOL bResult = ::ChooseColor(&m_cc);
    m_bModal = FALSE;
    return bResult ? IDOK : IDCANCEL;
}

// ---------------------------------------------------------------------
//  CFontDialog
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CFontDialog, CDialog)

CFontDialog::CFontDialog(const LOGFONT* lplfInitial, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd)
    : CDialog()
{
    memset(&m_cf, 0, sizeof(m_cf));
    memset(&m_lf, 0, sizeof(m_lf));
    m_cf.lStructSize = sizeof(m_cf);
    m_cf.hwndOwner = (pParentWnd != NULL) ? pParentWnd->GetSafeHwnd() : AfxGetMainHwnd();
    m_cf.hInstance = AfxGetResourceHandle();
    m_cf.Flags = dwFlags;
    if (pdcPrinter != NULL) { m_cf.hDC = pdcPrinter->GetSafeHdc(); m_cf.Flags |= CF_PRINTERFONTS; }
    if (lplfInitial != NULL) {
        memcpy(&m_lf, lplfInitial, sizeof(LOGFONT));
        m_cf.lpLogFont = &m_lf;
        m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
    } else {
        m_cf.lpLogFont = &m_lf;
    }
    m_cf.nSizeMin = 0;
    m_cf.nSizeMax = 0;
}

INT_PTR CFontDialog::DoModal()
{
    m_bModal = TRUE;
    if (m_cf.hwndOwner == NULL) m_cf.hwndOwner = AfxGetMainHwnd();
    BOOL bResult = ::ChooseFont(&m_cf);
    m_bModal = FALSE;
    return bResult ? IDOK : IDCANCEL;
}

void CFontDialog::GetCurrentFont(LOGFONT* lplf)
{
    if (lplf == NULL) return;
    if (m_cf.Flags & CF_INITTOLOGFONTSTRUCT) memcpy(lplf, &m_lf, sizeof(LOGFONT));
    else if (m_cf.lpLogFont != NULL) memcpy(lplf, m_cf.lpLogFont, sizeof(LOGFONT));
}

// ---------------------------------------------------------------------
//  CPrintDialog
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CPrintDialog, CDialog)

CPrintDialog::CPrintDialog(BOOL bPrintSetupOnly, DWORD dwFlags, CWnd* pParentWnd) : CDialog(), m_bHandleDC(FALSE)
{
    memset(&m_pd, 0, sizeof(m_pd));
    m_pd.lStructSize = sizeof(m_pd);
    m_pd.hwndOwner = (pParentWnd != NULL) ? pParentWnd->GetSafeHwnd() : AfxGetMainHwnd();
    m_pd.hInstance = AfxGetResourceHandle();
    m_pd.Flags = dwFlags;
    if (bPrintSetupOnly) m_pd.Flags |= PD_PRINTSETUP;
    else m_pd.Flags |= PD_RETURNDC;
    m_pd.nFromPage = 1;
    m_pd.nToPage = 1;
    m_pd.nMinPage = 1;
    m_pd.nMaxPage = 0xFFFF;
    m_pd.nCopies = 1;
}

CPrintDialog::~CPrintDialog()
{
    if (m_bHandleDC && m_pd.hDC != NULL) ::DeleteDC(m_pd.hDC);
    if (m_pd.hDevMode != NULL) ::GlobalFree(m_pd.hDevMode);
    if (m_pd.hDevNames != NULL) ::GlobalFree(m_pd.hDevNames);
}

BEGIN_MESSAGE_MAP(CPrintDialog, CDialog)
END_MESSAGE_MAP()

INT_PTR CPrintDialog::DoModal()
{
    m_bModal = TRUE;
    if (m_pd.hwndOwner == NULL) m_pd.hwndOwner = AfxGetMainHwnd();
    BOOL bResult = ::PrintDlg(&m_pd);
    m_bModal = FALSE;
    return bResult ? IDOK : IDCANCEL;
}

BOOL CPrintDialog::GetDefaults()
{
    m_pd.Flags |= PD_RETURNDEFAULT;
    BOOL b = ::PrintDlg(&m_pd);
    m_pd.Flags &= ~PD_RETURNDEFAULT;
    return b;
}

void CPrintDialog::AttachPrinter(HDC hDC, BOOL bHandles)
{
    m_pd.hDC = hDC;
    m_bHandleDC = bHandles;
}

HDC CPrintDialog::GetPrinterDC() const { return m_pd.hDC; }

HDC CPrintDialog::CreatePrinterDC()
{
    m_pd.Flags |= PD_RETURNDC;
    if (::PrintDlg(&m_pd)) return m_pd.hDC;
    return NULL;
}

int CPrintDialog::GetCopies() const { return (int)m_pd.nCopies; }
BOOL CPrintDialog::PrintSelection() const { return (m_pd.Flags & PD_SELECTION) != 0; }
BOOL CPrintDialog::PrintRange() const { return (m_pd.Flags & PD_PAGENUMS) != 0; }
BOOL CPrintDialog::PrintAll() const { return !PrintRange() && !PrintSelection(); }
int CPrintDialog::GetFromPage() const { return (PrintRange() ? (int)m_pd.nFromPage : 1); }
int CPrintDialog::GetToPage() const { return (PrintRange() ? (int)m_pd.nToPage : 0xFFFF); }

DEVMODE* CPrintDialog::GetDevMode() const
{
    if (m_pd.hDevMode == NULL) return NULL;
    return (DEVMODE*)::GlobalLock(m_pd.hDevMode);
}

CString CPrintDialog::GetDeviceName() const
{
    if (m_pd.hDevNames == NULL) return CString();
    DEVNAMES* pdn = (DEVNAMES*)::GlobalLock(m_pd.hDevNames);
    if (pdn == NULL) return CString();
    CString str((LPCTSTR)pdn + pdn->wDeviceOffset);
    ::GlobalUnlock(m_pd.hDevNames);
    return str;
}

CString CPrintDialog::GetDriverName() const
{
    if (m_pd.hDevNames == NULL) return CString();
    DEVNAMES* pdn = (DEVNAMES*)::GlobalLock(m_pd.hDevNames);
    if (pdn == NULL) return CString();
    CString str((LPCTSTR)pdn + pdn->wDriverOffset);
    ::GlobalUnlock(m_pd.hDevNames);
    return str;
}

CString CPrintDialog::GetPortName() const
{
    if (m_pd.hDevNames == NULL) return CString();
    DEVNAMES* pdn = (DEVNAMES*)::GlobalLock(m_pd.hDevNames);
    if (pdn == NULL) return CString();
    CString str((LPCTSTR)pdn + pdn->wOutputOffset);
    ::GlobalUnlock(m_pd.hDevNames);
    return str;
}

BOOL CPrintDialog::DoPrintDialog(CDocument* pDoc, CView* pView)
{
    if (pView == NULL && pDoc != NULL) {
        POSITION pos = pDoc->GetFirstViewPosition();
        if (pos != NULL) pView = pDoc->GetNextView(pos);
    }
    if (pView == NULL) return FALSE;
    CPrintInfo info;
    *info.m_pPD = *this;
    pView->OnPreparePrinting(&info);
    if (DoModal() != IDOK) return FALSE;
    pView->OnFilePrint();
    return TRUE;
}

// ---------------------------------------------------------------------
//  CPageSetupDialog
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CPageSetupDialog, CDialog)

CPageSetupDialog::CPageSetupDialog(DWORD dwFlags, CWnd* pParentWnd) : CDialog()
{
    memset(&m_psd, 0, sizeof(m_psd));
    m_psd.lStructSize = sizeof(m_psd);
    m_psd.hwndOwner = (pParentWnd != NULL) ? pParentWnd->GetSafeHwnd() : AfxGetMainHwnd();
    m_psd.hInstance = AfxGetResourceHandle();
    m_psd.Flags = dwFlags;
    m_psd.ptPaperSize.x = 21590;   // 8.5" in 1/1000 inch units
    m_psd.ptPaperSize.y = 27940;   // 11"
}

INT_PTR CPageSetupDialog::DoModal()
{
    m_bModal = TRUE;
    BOOL bResult = ::PageSetupDlg(&m_psd);
    m_bModal = FALSE;
    return bResult ? IDOK : IDCANCEL;
}

void CPageSetupDialog::GetMargins(LPRECT lpRectMargins, LPRECT lpRectMinMargins) const
{
    if (lpRectMargins != NULL) ::CopyRect(lpRectMargins, &m_psd.rtMargin);
    if (lpRectMinMargins != NULL) ::CopyRect(lpRectMinMargins, &m_psd.rtMinMargin);
}

CSize CPageSetupDialog::GetPaperSize() const { return CSize(m_psd.ptPaperSize.x, m_psd.ptPaperSize.y); }

void CPageSetupDialog::GetDevMode(LPDEVMODE* ppDevMode) const
{
    if (ppDevMode == NULL) return;
    if (m_psd.hDevMode == NULL) { *ppDevMode = NULL; return; }
    *ppDevMode = (LPDEVMODE)::GlobalLock(m_psd.hDevMode);
}

// ---------------------------------------------------------------------
//  CFindReplaceDialog
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CFindReplaceDialog, CDialog)

CFindReplaceDialog::CFindReplaceDialog() : CDialog(), m_bTerminating(FALSE)
{
    memset(&m_fr, 0, sizeof(m_fr));
    memset(m_szFindWhat, 0, sizeof(m_szFindWhat));
    memset(m_szReplaceWith, 0, sizeof(m_szReplaceWith));
    m_fr.lStructSize = sizeof(m_fr);
    m_fr.lpstrFindWhat = m_szFindWhat;
    m_fr.wFindWhatLen = (sizeof(m_szFindWhat) / sizeof(TCHAR)) - 1;
    m_fr.lpstrReplaceWith = m_szReplaceWith;
    m_fr.wReplaceWithLen = (sizeof(m_szReplaceWith) / sizeof(TCHAR)) - 1;
}

CFindReplaceDialog::~CFindReplaceDialog() {}

BEGIN_MESSAGE_MAP(CFindReplaceDialog, CDialog)
END_MESSAGE_MAP()

UINT CFindReplaceDialog::GetFindReplaceMsg()
{
    static UINT s_nMsg = 0;
    if (s_nMsg == 0) s_nMsg = ::RegisterWindowMessage(FINDMSGSTRING);
    return s_nMsg;
}

BOOL CFindReplaceDialog::Create(BOOL bFindDialogOnly, LPCTSTR lpszFindWhat,
                                LPCTSTR lpszReplaceWith, DWORD dwFlags, CWnd* pParentWnd)
{
    m_bModal = FALSE;
    AfxStrCopy(m_szFindWhat, m_fr.wFindWhatLen, lpszFindWhat ? lpszFindWhat : _T(""));
    AfxStrCopy(m_szReplaceWith, m_fr.wReplaceWithLen, lpszReplaceWith ? lpszReplaceWith : _T(""));
    m_fr.Flags = dwFlags;
    m_fr.hwndOwner = (pParentWnd != NULL) ? pParentWnd->GetSafeHwnd() : AfxGetMainHwnd();
    m_fr.hInstance = AfxGetResourceHandle();
    m_fr.lCustData = (LPARAM)this;

    HWND hWnd = bFindDialogOnly ? ::FindText(&m_fr) : ::ReplaceText(&m_fr);
    if (hWnd == NULL) return FALSE;
    m_hWnd = hWnd;
    AfxRegisterWnd(hWnd, this);
    return TRUE;
}

CFindReplaceDialog* CFindReplaceDialog::GetNotifier(LPARAM lParam)
{
    FINDREPLACE* pfr = (FINDREPLACE*)lParam;
    if (pfr == NULL) return NULL;
    return (CFindReplaceDialog*)pfr->lCustData;
}

void CFindReplaceDialog::PostNcDestroy()
{
    m_bTerminating = TRUE;
    CDialog::PostNcDestroy();
    delete this;      // MFC deletes modeless find/replace dialogs
}

// ---------------------------------------------------------------------
//  CPropertyPage / CPropertySheet
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CPropertyPage, CDialog)
IMPLEMENT_DYNAMIC(CPropertySheet, CWnd)

CPropertyPage::CPropertyPage(UINT nIDTemplate, UINT nIDCaption, DWORD dwSize) : CDialog(nIDTemplate)
{
    memset(&m_psp, 0, sizeof(m_psp));
    m_psp.dwSize = dwSize;
    m_psp.dwFlags = PSP_USEICONID;
    m_psp.hInstance = AfxGetResourceHandle();
    m_psp.pszTemplate = MAKEINTRESOURCE(nIDTemplate);
    if (nIDCaption != 0) {
        m_psp.dwFlags |= PSP_USETITLE;
        m_psp.pszTitle = MAKEINTRESOURCE(nIDCaption);
    }
    m_pParentSheet = NULL;
}

CPropertyPage::CPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption, DWORD dwSize)
    : CDialog(lpszTemplateName)
{
    memset(&m_psp, 0, sizeof(m_psp));
    m_psp.dwSize = dwSize;
    m_psp.hInstance = AfxGetResourceHandle();
    m_psp.pszTemplate = lpszTemplateName;
    if (nIDCaption != 0) {
        m_psp.dwFlags |= PSP_USETITLE;
        m_psp.pszTitle = MAKEINTRESOURCE(nIDCaption);
    }
    m_pParentSheet = NULL;
}

CPropertyPage::CPropertyPage() : CDialog(), m_pParentSheet(NULL)
{
    memset(&m_psp, 0, sizeof(m_psp));
    m_psp.dwSize = sizeof(m_psp);
}

CPropertyPage::~CPropertyPage() {}

void CPropertyPage::Construct(UINT nIDTemplate, UINT nIDCaption)
{
    m_nIDTemplate = nIDTemplate;
    memset(&m_psp, 0, sizeof(m_psp));
    m_psp.dwSize = sizeof(m_psp);
    m_psp.hInstance = AfxGetResourceHandle();
    m_psp.pszTemplate = MAKEINTRESOURCE(nIDTemplate);
    if (nIDCaption != 0) { m_psp.dwFlags |= PSP_USETITLE; m_psp.pszTitle = MAKEINTRESOURCE(nIDCaption); }
}

void CPropertyPage::Construct(LPCTSTR lpszTemplateName, UINT nIDCaption)
{
    m_lpszTemplateName = lpszTemplateName;
    memset(&m_psp, 0, sizeof(m_psp));
    m_psp.dwSize = sizeof(m_psp);
    m_psp.hInstance = AfxGetResourceHandle();
    m_psp.pszTemplate = lpszTemplateName;
    if (nIDCaption != 0) { m_psp.dwFlags |= PSP_USETITLE; m_psp.pszTitle = MAKEINTRESOURCE(nIDCaption); }
}

void CPropertyPage::SetModified(BOOL bChanged)
{
    if (m_pParentSheet != NULL && m_pParentSheet->GetSafeHwnd() != NULL)
        ::SendMessage(m_pParentSheet->GetSafeHwnd(), bChanged ? PSM_CHANGED : PSM_UNCHANGED, (WPARAM)m_hWnd, 0);
}

LRESULT CPropertyPage::Apply()
{
    if (m_pParentSheet == NULL) return PSNRET_NOERROR;
    return ::SendMessage(m_pParentSheet->GetSafeHwnd(), PSM_APPLY, 0, 0);
}

LRESULT CPropertyPage::QuerySiblings(WPARAM wParam, LPARAM lParam)
{
    if (m_pParentSheet == NULL) return 0;
    return ::SendMessage(m_pParentSheet->GetSafeHwnd(), PSM_QUERYSIBLINGS, wParam, lParam);
}

void CPropertyPage::CancelToClose()
{
    if (m_pParentSheet != NULL)
        ::SendMessage(m_pParentSheet->GetSafeHwnd(), PSM_CANCELTOCLOSE, 0, 0);
}

void CPropertyPage::SetWizardButtons(DWORD dwFlags)
{
    if (m_pParentSheet != NULL)
        ::PostMessage(m_pParentSheet->GetSafeHwnd(), PSM_SETWIZBUTTONS, 0, (LPARAM)dwFlags);
}

BOOL CPropertyPage::OnApply() { return TRUE; }
void CPropertyPage::OnReset() {}
BOOL CPropertyPage::OnQueryCancel() { return TRUE; }
BOOL CPropertyPage::OnWizardBack() { return TRUE; }
BOOL CPropertyPage::OnWizardFinish() { return TRUE; }
LRESULT CPropertyPage::OnWizardNext() { return 0; }
LRESULT CPropertyPage::OnWizardFinishEx() { return 0; }
BOOL CPropertyPage::OnSetActive() { return TRUE; }
BOOL CPropertyPage::OnKillActive() { return TRUE; }
void CPropertyPage::OnHelp() {}

// the property page window procedure
static INT_PTR CALLBACK AfxPropPageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CPropertyPage* pPage = NULL;
    if (message == WM_INITDIALOG) {
        PROPSHEETPAGE* pPsp = (PROPSHEETPAGE*)lParam;
        pPage = (CPropertyPage*)(pPsp ? pPsp->lParam : 0);
        if (pPage != NULL) {
            pPage->m_hWnd = hWnd;
            AfxRegisterWnd(hWnd, pPage);
        }
    } else {
        pPage = (CPropertyPage*)CWnd::FromHandlePermanent(hWnd);
        if (pPage != NULL && !pPage->IsKindOf(RUNTIME_CLASS(CPropertyPage))) pPage = NULL;
    }
    if (pPage == NULL) return FALSE;

    LRESULT lResult = 0;
    switch (message) {
    case WM_NOTIFY: {
        NMHDR* pNMHDR = (NMHDR*)lParam;
        switch (pNMHDR->code) {
        case PSN_SETACTIVE:
            return pPage->OnSetActive() ? TRUE : FALSE;
        case PSN_KILLACTIVE:
            return pPage->OnKillActive() ? TRUE : FALSE;
        case PSN_APPLY:
            return pPage->OnApply() ? PSNRET_NOERROR : PSNRET_INVALID;
        case PSN_RESET:
            pPage->OnReset();
            return TRUE;
        case PSN_QUERYCANCEL:
            return pPage->OnQueryCancel() ? TRUE : FALSE;
        case PSN_WIZBACK: {
            BOOL b = pPage->OnWizardBack();
            return b ? 0 : -1;
        }
        case PSN_WIZNEXT:
            return pPage->OnWizardNext();
        case PSN_WIZFINISH:
            return pPage->OnWizardFinish() ? FALSE : TRUE;
        case PSN_HELP:
            pPage->OnHelp();
            return TRUE;
        default: break;
        }
        break;
    }
    case WM_DESTROY:
        if (pPage->m_hWnd == hWnd) { pPage->m_hWnd = NULL; AfxUnregisterWnd(hWnd); }
        return FALSE;
    default: break;
    }

    if (pPage->OnWndMsg(message, wParam, lParam, &lResult)) return (INT_PTR)lResult;
    return FALSE;
}

int CALLBACK AfxPropSheetCallback(HWND hWnd, UINT message, LPARAM lParam)
{
    switch (message) {
    case PSCB_INITIALIZED:
        return TRUE;
    case PSCB_PRECREATE: {
        // add the standard styles
        LPDLGTEMPLATE pTemplate = (LPDLGTEMPLATE)lParam;
        if (pTemplate != NULL) pTemplate->style |= DS_SETFONT | WS_CLIPCHILDREN;
        return TRUE;
    }
    default: break;
    }
    (void)hWnd;
    return 0;
}

// ---------------------------------------------------------------------
//  CPropertySheet
// ---------------------------------------------------------------------
CPropertySheet::CPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
    : m_dwFlags(0), m_iSelectPage((int)iSelectPage), m_bWizard(FALSE)
{
    (void)pParentWnd;
    CString str;
    str.LoadString(nIDCaption);
    m_strCaption = str;
}

CPropertySheet::CPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
    : m_dwFlags(0), m_strCaption(pszCaption ? pszCaption : _T("")),
      m_iSelectPage((int)iSelectPage), m_bWizard(FALSE)
{
    (void)pParentWnd;
}

CPropertySheet::CPropertySheet()
    : m_dwFlags(0), m_iSelectPage(0), m_bWizard(FALSE)
{
}

CPropertySheet::~CPropertySheet() {}

void CPropertySheet::Construct(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
{
    CString str;
    str.LoadString(nIDCaption);
    m_strCaption = str;
    m_iSelectPage = (int)iSelectPage;
    (void)pParentWnd;
}

void CPropertySheet::Construct(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
{
    m_strCaption = pszCaption ? pszCaption : _T("");
    m_iSelectPage = (int)iSelectPage;
    (void)pParentWnd;
}

int CPropertySheet::AddPage(CPropertyPage* pPage)
{
    if (pPage == NULL) return -1;
    pPage->m_pParentSheet = this;
    m_pages.push_back(pPage);
    return (int)m_pages.size() - 1;
}

void CPropertySheet::RemovePage(CPropertyPage* pPage)
{
    for (size_t i = 0; i < m_pages.size(); ++i) {
        if (m_pages[i] == pPage) { pPage->m_pParentSheet = NULL; m_pages.erase(m_pages.begin() + i); return; }
    }
}

void CPropertySheet::RemovePage(int nPage)
{
    if (nPage < 0 || nPage >= (int)m_pages.size()) return;
    m_pages[nPage]->m_pParentSheet = NULL;
    m_pages.erase(m_pages.begin() + nPage);
}

CPropertyPage* CPropertySheet::GetPage(int nPage) const
{
    if (nPage < 0 || nPage >= (int)m_pages.size()) return NULL;
    return m_pages[nPage];
}

int CPropertySheet::GetActiveIndex() const
{
    if (m_hWnd == NULL) return m_iSelectPage;
    HWND hTab = ::GetDlgItem(m_hWnd, 0x3020);
    if (hTab == NULL) return m_iSelectPage;
    return (int)::SendMessage(hTab, TCM_GETCURSEL, 0, 0);
}

CPropertyPage* CPropertySheet::GetActivePage() const { return GetPage(GetActiveIndex()); }

void CPropertySheet::SetActivePage(int nPage)
{
    if (m_hWnd == NULL) { m_iSelectPage = nPage; return; }
    ::SendMessage(m_hWnd, PSM_SETCURSEL, nPage, 0);
}

void CPropertySheet::SetActivePage(CPropertyPage* pPage)
{
    for (size_t i = 0; i < m_pages.size(); ++i)
        if (m_pages[i] == pPage) { SetActivePage((int)i); return; }
}

void CPropertySheet::SetTitle(LPCTSTR lpszText, UINT nStyle)
{
    m_strCaption = lpszText;
    if (m_hWnd != NULL) ::SendMessage(m_hWnd, PSM_SETTITLE, nStyle, (LPARAM)(LPCTSTR)m_strCaption);
}

void CPropertySheet::SetWizardMode() { m_bWizard = TRUE; m_dwFlags |= PSH_WIZARD; }
void CPropertySheet::EnableApplyButton(BOOL bEnable)
{
    if (m_hWnd != NULL) ::SendMessage(m_hWnd, PSM_CHANGED, (WPARAM)::GetDlgItem(m_hWnd, 0x3021), bEnable ? 0 : 1);
}
void CPropertySheet::SetFinishText(LPCTSTR lpszText)
{
    HWND hFinish = ::GetDlgItem(m_hWnd, 0x3025);
    if (hFinish != NULL) {
        ::SetWindowText(hFinish, lpszText);
        ::EnableWindow(hFinish, TRUE);
    }
}
void CPropertySheet::SetDefaultButton() {}
void CPropertySheet::EnableHelp() { m_dwFlags |= PSH_HASHELP; }
void CPropertySheet::Apply() { if (m_hWnd != NULL) ::SendMessage(m_hWnd, PSM_APPLY, 0, 0); }
void CPropertySheet::PressButton(int nButton) { if (m_hWnd != NULL) ::PostMessage(m_hWnd, PSM_PRESSBUTTON, nButton, 0); }
HWND CPropertySheet::GetTabControl() const { return ::GetDlgItem(m_hWnd, 0x3020); }
void CPropertySheet::MapDialogRect(LPRECT lpRect) const { ::MapDialogRect(m_hWnd, lpRect); }

void CPropertySheet::BuildPropPageArray()
{
    m_hPages.clear();
    for (size_t i = 0; i < m_pages.size(); ++i) {
        CPropertyPage* pPage = m_pages[i];
        pPage->m_psp.pfnDlgProc = AfxPropPageProc;
        pPage->m_psp.lParam = (LPARAM)pPage;
        pPage->m_psp.hInstance = AfxGetResourceHandle();
        if (pPage->m_psp.pszTemplate == NULL) {
            if (pPage->m_nIDTemplate != 0) pPage->m_psp.pszTemplate = MAKEINTRESOURCE(pPage->m_nIDTemplate);
            else pPage->m_psp.pszTemplate = pPage->m_lpszTemplateName;
        }
        HPROPSHEETPAGE hPage = ::CreatePropertySheetPage(&pPage->m_psp);
        if (hPage != NULL) m_hPages.push_back(hPage);
    }
}

HPROPSHEETPAGE* CPropertySheet::GetPageHandles()
{
    BuildPropPageArray();
    return m_hPages.empty() ? NULL : &m_hPages[0];
}

INT_PTR CPropertySheet::DoModal()
{
    BuildPropPageArray();
    if (m_hPages.empty()) return -1;

    PROPSHEETHEADER psh;
    memset(&psh, 0, sizeof(psh));
    psh.dwSize = sizeof(psh);
    psh.dwFlags = PSH_PROPSHEETPAGE | m_dwFlags | (m_bWizard ? PSH_WIZARD : 0);
    psh.hwndParent = AfxGetMainHwnd();
    psh.hInstance = AfxGetResourceHandle();
    psh.pszCaption = (LPCTSTR)m_strCaption;
    psh.nPages = (UINT)m_hPages.size();
    psh.nStartPage = (UINT)((m_iSelectPage >= 0 && m_iSelectPage < (int)m_hPages.size()) ? m_iSelectPage : 0);
    psh.phpage = &m_hPages[0];
    psh.pfnCallback = AfxPropSheetCallback;

    INT_PTR nResult = ::PropertySheet(&psh);
    m_hWnd = NULL;
    for (size_t i = 0; i < m_pages.size(); ++i) {
        if (m_pages[i]->m_hWnd != NULL) { AfxUnregisterWnd(m_pages[i]->m_hWnd); m_pages[i]->m_hWnd = NULL; }
    }
    m_hPages.clear();
    return nResult;
}

BOOL CPropertySheet::Create(CWnd* pParentWnd, DWORD dwStyle, DWORD dwExStyle)
{
    BuildPropPageArray();
    if (m_hPages.empty()) return FALSE;

    PROPSHEETHEADER psh;
    memset(&psh, 0, sizeof(psh));
    psh.dwSize = sizeof(psh);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_MODELESS | m_dwFlags;
    psh.hwndParent = (pParentWnd != NULL) ? pParentWnd->GetSafeHwnd() : AfxGetMainHwnd();
    psh.hInstance = AfxGetResourceHandle();
    psh.pszCaption = (LPCTSTR)m_strCaption;
    psh.nPages = (UINT)m_hPages.size();
    psh.nStartPage = (UINT)((m_iSelectPage >= 0 && m_iSelectPage < (int)m_hPages.size()) ? m_iSelectPage : 0);
    psh.phpage = &m_hPages[0];
    psh.pfnCallback = AfxPropSheetCallback;

    HWND hWnd = (HWND)::PropertySheet(&psh);
    if (hWnd == NULL) return FALSE;
    m_hWnd = hWnd;
    AfxRegisterWnd(hWnd, this);
    if (dwStyle != (DWORD)-1) {
        LONG_PTR style = ::GetWindowLongPtr(hWnd, GWL_STYLE);
        style = (style & ~(DWORD)0xFFFF) | (dwStyle & 0xFFFF);
        ::SetWindowLongPtr(hWnd, GWL_STYLE, style);
    }
    if (dwExStyle != 0) {
        LONG_PTR ex = ::GetWindowLongPtr(hWnd, GWL_EXSTYLE);
        ::SetWindowLongPtr(hWnd, GWL_EXSTYLE, ex | dwExStyle);
    }
    return TRUE;
}

void CPropertySheet::EndDialog(int nEndID)
{
    if (m_hWnd != NULL) ::SendMessage(m_hWnd, PSM_PRESSBUTTON, PSBTN_FINISH, 0);
    (void)nEndID;
}
