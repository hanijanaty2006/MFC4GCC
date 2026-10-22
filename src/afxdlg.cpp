// =====================================================================
//  afxdlg.cpp -- CDialog, the shared dialog procedure and DDX/DDV
// =====================================================================
#include <afxwin.h>
#include <afxcmn.h>
#include <afxext.h>
#include <afxdlgs.h>
#include <cmath>

// ---------------------------------------------------------------------
//  CDataExchange
// ---------------------------------------------------------------------
CDataExchange::CDataExchange(CWnd* pDlg, BOOL bSaveAndValidate)
    : m_bSaveAndValidate(bSaveAndValidate), m_pDlgWnd(pDlg),
      m_hWndLastControl(NULL), m_bEditLastControl(FALSE)
{
}

HWND CDataExchange::PrepareCtrl(int nIDC)
{
    ASSERT(nIDC != 0);
    CWnd* pWnd = m_pDlgWnd->GetDlgItem(nIDC);
    m_hWndLastControl = (pWnd != NULL) ? pWnd->GetSafeHwnd() : NULL;
    m_bEditLastControl = FALSE;
    return m_hWndLastControl;
}

HWND CDataExchange::PrepareEditCtrl(int nIDC)
{
    ASSERT(nIDC != 0);
    CWnd* pWnd = m_pDlgWnd->GetDlgItem(nIDC);
    m_hWndLastControl = (pWnd != NULL) ? pWnd->GetSafeHwnd() : NULL;
    m_bEditLastControl = TRUE;
    return m_hWndLastControl;
}

void CDataExchange::Fail()
{
    if (m_hWndLastControl != NULL) ::SetFocus(m_hWndLastControl);
    AfxThrowUserException();
}

BOOL AFXAPI AfxDoDataExchange(CWnd* pWnd, BOOL bSaveAndValidate)
{
    if (pWnd == NULL || pWnd->GetSafeHwnd() == NULL) return FALSE;
    CDataExchange dx(pWnd, bSaveAndValidate);
    try {
        pWnd->DoDataExchange(&dx);
    } catch (CUserException&) {
        return FALSE;
    } catch (CException* e) {
        if (e != NULL) { e->ReportError(); e->Delete(); }
        return FALSE;
    } catch (...) {
        return FALSE;
    }
    return TRUE;
}

// ---------------------------------------------------------------------
//  The shared dialog procedure
// ---------------------------------------------------------------------
INT_PTR CALLBACK AfxDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_INITDIALOG) {
        // lParam is the CDialog* we passed to DialogBox[Indirect]Param
        CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);
        if (pWnd == NULL && lParam != 0) {
            CWnd* pNew = (CWnd*)lParam;
            if (pNew->IsKindOf(RUNTIME_CLASS(CWnd)) && pNew->GetSafeHwnd() == NULL) {
                pNew->m_hWnd = hWnd;
                AfxRegisterWnd(hWnd, pNew);
            }
        }
    }

    CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);
    if (pWnd == NULL) return FALSE;

    LRESULT lResult = 0;
    if (pWnd->OnWndMsg(message, wParam, lParam, &lResult))
        return (INT_PTR)lResult;

    // not handled: let the default dialog procedure do its job
    // (tab navigation, default button, ESC/ENTER ...)
    return FALSE;
}

// ---------------------------------------------------------------------
//  CDialog
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CDialog, CWnd)

CDialog::CDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
    : m_lpszTemplateName(lpszTemplateName), m_nIDTemplate(0), m_hDialogTemplate(NULL),
      m_lpDialogTemplate(NULL), m_lpDialogInit(NULL), m_pParentWnd(pParentWnd),
      m_nIDHelp(0), m_nModalResult(0), m_bModal(FALSE)
{
}

CDialog::CDialog(UINT nIDTemplate, CWnd* pParentWnd)
    : m_lpszTemplateName(NULL), m_nIDTemplate(nIDTemplate), m_hDialogTemplate(NULL),
      m_lpDialogTemplate(NULL), m_lpDialogInit(NULL), m_pParentWnd(pParentWnd),
      m_nIDHelp(nIDTemplate), m_nModalResult(0), m_bModal(FALSE)
{
}

CDialog::CDialog()
    : m_lpszTemplateName(NULL), m_nIDTemplate(0), m_hDialogTemplate(NULL),
      m_lpDialogTemplate(NULL), m_lpDialogInit(NULL), m_pParentWnd(NULL),
      m_nIDHelp(0), m_nModalResult(0), m_bModal(FALSE)
{
}

CDialog::~CDialog()
{
    if (m_hDialogTemplate != NULL) { ::GlobalFree(m_hDialogTemplate); m_hDialogTemplate = NULL; }
    if (m_hWnd != NULL) { AfxUnregisterWnd(m_hWnd); m_hWnd = NULL; }
}

BEGIN_MESSAGE_MAP(CDialog, CWnd)
    ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
    ON_MESSAGE(WM_SETFONT, HandleSetFont)
    ON_COMMAND(IDOK, CDialog::OnOK)
    ON_COMMAND(IDCANCEL, CDialog::OnCancel)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

LRESULT CDialog::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
    PreSubclassWindow();

    // centre the dialog if the template asks for it
    if (CheckAutoCenter()) CenterWindow(m_pParentWnd);

    // Attach the DDX_Control members and push the initial member values
    // into the controls *before* the user's OnInitDialog() runs - this is
    // what makes m_tree/m_list/... usable inside OnInitDialog, exactly as
    // in MFC.
    UpdateData(FALSE);

    BOOL bResult = OnInitDialog();

    if (bResult) {
        // set the dialog font if the template did not specify one
        HFONT hFont = (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0);
        if (hFont == NULL) {
            hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
            ::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, 0);
        }
    }
    (void)wParam; (void)lParam;
    return bResult;
}

LRESULT CDialog::HandleSetFont(WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = DefWindowProc(WM_SETFONT, wParam, lParam);
    if (CheckAutoCenter()) CenterWindow(m_pParentWnd);
    return lResult;
}

BOOL CDialog::CheckAutoCenter()
{
    // DS_CENTER / DS_CENTERMOUSE in the template?
    if (m_lpDialogTemplate != NULL)
        return (m_lpDialogTemplate->style & (DS_CENTER | DS_CENTERMOUSE)) != 0;
    HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(),
                                 m_nIDTemplate ? MAKEINTRESOURCE(m_nIDTemplate) : (LPCTSTR)m_lpszTemplateName,
                                 RT_DIALOG);
    if (hRsrc != NULL) {
        HGLOBAL hGlobal = ::LoadResource(AfxGetResourceHandle(), hRsrc);
        if (hGlobal != NULL) {
            DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)::LockResource(hGlobal);
            BOOL b = (pTemplate->style & (DS_CENTER | DS_CENTERMOUSE)) != 0;
            return b;
        }
    }
    return TRUE;      // default: centre on the parent
}

BOOL CDialog::OnInitDialog() { return TRUE; }

void CDialog::OnOK()
{
    if (!UpdateData(TRUE)) {
        TRACE(_T("UpdateData failed during dialog termination.\n"));
        return;
    }
    EndDialog(IDOK);
}

void CDialog::OnCancel() { EndDialog(IDCANCEL); }

void CDialog::OnClose()
{
    if (m_bModal) EndDialog(IDCANCEL);
    else DestroyWindow();
}

INT_PTR CDialog::DoModal()
{
    m_bModal = TRUE;
    m_nModalResult = 0;
    HINSTANCE hInst = AfxGetResourceHandle();

    HWND hWndParent = NULL;
    if (m_pParentWnd != NULL) hWndParent = m_pParentWnd->GetSafeHwnd();
    else if (AfxGetMainHwnd() != NULL) hWndParent = AfxGetMainHwnd();
    else hWndParent = ::GetActiveWindow();

    if (m_hWnd != NULL) { AfxUnregisterWnd(m_hWnd); m_hWnd = NULL; }

    INT_PTR nResult;
    if (m_hDialogTemplate != NULL) {
        LPCDLGTEMPLATE pTemplate = (LPCDLGTEMPLATE)::GlobalLock(m_hDialogTemplate);
        nResult = ::DialogBoxIndirectParam(hInst, pTemplate, hWndParent, AfxDlgProc, (LPARAM)this);
        ::GlobalUnlock(m_hDialogTemplate);
    } else if (m_lpDialogTemplate != NULL) {
        nResult = ::DialogBoxIndirectParam(hInst, m_lpDialogTemplate, hWndParent, AfxDlgProc, (LPARAM)this);
    } else if (m_nIDTemplate != 0) {
        nResult = ::DialogBoxParam(hInst, MAKEINTRESOURCE(m_nIDTemplate), hWndParent, AfxDlgProc, (LPARAM)this);
    } else {
        nResult = ::DialogBoxParam(hInst, m_lpszTemplateName, hWndParent, AfxDlgProc, (LPARAM)this);
    }

    if (nResult == -1) {
        CString str;
        str.Format(_T("DialogBox failed (error %lu). Check the dialog template."), ::GetLastError());
        AfxMessageBox(str);
        nResult = IDCANCEL;
    }
    m_hWnd = NULL;
    m_bModal = FALSE;
    return nResult;
}

BOOL CDialog::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
    if (pParentWnd != NULL) m_pParentWnd = pParentWnd;
    m_lpszTemplateName = lpszTemplateName;
    m_nIDTemplate = 0;
    m_bModal = FALSE;
    HWND hWndParent = (m_pParentWnd != NULL) ? m_pParentWnd->GetSafeHwnd() : AfxGetMainHwnd();
    HWND hWnd = ::CreateDialogParam(AfxGetResourceHandle(), lpszTemplateName, hWndParent,
                                    AfxDlgProc, (LPARAM)this);
    if (hWnd == NULL) return FALSE;
    if (m_hWnd == NULL) { m_hWnd = hWnd; AfxRegisterWnd(hWnd, this); }
    ::ShowWindow(hWnd, SW_SHOW);
    return TRUE;
}

BOOL CDialog::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
    if (pParentWnd != NULL) m_pParentWnd = pParentWnd;
    m_nIDTemplate = nIDTemplate;
    m_lpszTemplateName = NULL;
    m_bModal = FALSE;
    HWND hWndParent = (m_pParentWnd != NULL) ? m_pParentWnd->GetSafeHwnd() : AfxGetMainHwnd();
    HWND hWnd = ::CreateDialogParam(AfxGetResourceHandle(), MAKEINTRESOURCE(nIDTemplate), hWndParent,
                                    AfxDlgProc, (LPARAM)this);
    if (hWnd == NULL) return FALSE;
    if (m_hWnd == NULL) { m_hWnd = hWnd; AfxRegisterWnd(hWnd, this); }
    ::ShowWindow(hWnd, SW_SHOW);
    return TRUE;
}

BOOL CDialog::CreateIndirect(LPCDLGTEMPLATE lpDialogTemplate, CWnd* pParentWnd, void* lpInit)
{
    if (pParentWnd != NULL) m_pParentWnd = pParentWnd;
    m_lpDialogTemplate = lpDialogTemplate;
    m_lpDialogInit = lpInit;
    m_bModal = FALSE;
    HWND hWndParent = (m_pParentWnd != NULL) ? m_pParentWnd->GetSafeHwnd() : AfxGetMainHwnd();
    HWND hWnd = ::CreateDialogIndirectParam(AfxGetResourceHandle(), lpDialogTemplate, hWndParent,
                                            AfxDlgProc, (LPARAM)this);
    if (hWnd == NULL) return FALSE;
    if (m_hWnd == NULL) { m_hWnd = hWnd; AfxRegisterWnd(hWnd, this); }
    ::ShowWindow(hWnd, SW_SHOW);
    return TRUE;
}

void CDialog::EndDialog(INT_PTR nResult)
{
    m_nModalResult = (int)nResult;
    if (m_bModal) ::EndDialog(m_hWnd, nResult);
    else {
        ::SetWindowLongPtr(m_hWnd, DWLP_MSGRESULT, (LONG_PTR)nResult);
        DestroyWindow();
    }
}

void CDialog::MapDialogRect(LPRECT lpRect) const { ::MapDialogRect(m_hWnd, lpRect); }
void CDialog::NextDlgCtrl() const { ::SendMessage(m_hWnd, WM_NEXTDLGCTL, 0, 0); }
void CDialog::PrevDlgCtrl() const { ::SendMessage(m_hWnd, WM_NEXTDLGCTL, 1, 0); }
void CDialog::PostNcDestroy() { CWnd::PostNcDestroy(); }

BOOL CDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && m_bModal) {
        // ESC closes a modal dialog unless a control wants the key
        HWND hWndFocus = ::GetFocus();
        TCHAR szClass[64] = { 0 };
        if (hWndFocus != NULL) ::GetClassName(hWndFocus, szClass, 64);
        if (_tcsicmp(szClass, _T("Edit")) != 0) { OnCancel(); return TRUE; }
    }
    return CWnd::PreTranslateMessage(pMsg);
}

BOOL CDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (CWnd::OnCommand(wParam, lParam)) return TRUE;
    return FALSE;
}

BOOL CDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if (CWnd::OnNotify(wParam, lParam, pResult)) return TRUE;
    return FALSE;
}

BOOL CDialog::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    if (CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return TRUE;
    return FALSE;
}

HBRUSH CDialog::OnCtlColorDlg(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{ return CWnd::OnCtlColor(pDC, pWnd, nCtlColor); }
LRESULT CDialog::OnNcHitTestDlg(CPoint point) { return CWnd::OnNcHitTest(point); }
void CDialog::OnSysCommandDlg(UINT nID, LPARAM lParam) { CWnd::OnSysCommand(nID, lParam); }

// ---------------------------------------------------------------------
//  DDX_Text and friends
// ---------------------------------------------------------------------
void AFXAPI AfxSetWindowText(HWND hWndCtrl, LPCTSTR lpszText)
{
    ::SetWindowText(hWndCtrl, lpszText);
    // make sure the parent sees EN_UPDATE
    ::SendMessage(::GetParent(hWndCtrl), WM_COMMAND,
                  MAKEWPARAM(::GetDlgCtrlID(hWndCtrl), EN_UPDATE), (LPARAM)hWndCtrl);
}

int AFXAPI AfxGetWindowText(HWND hWndCtrl, LPTSTR lpszText, int nMaxLen)
{
    return (int)::SendMessage(hWndCtrl, WM_GETTEXT, nMaxLen, (LPARAM)lpszText);
}

BOOL AFXAPI AfxDlgDataExchangeError(CDataExchange* pDX, UINT nIDC, UINT nIDS)
{
    CString str;
    if (!str.LoadString(nIDS)) str = _T("Invalid data entered.");
    AfxMessageBox(str, MB_ICONEXCLAMATION | MB_OK);
    if (pDX != NULL) { pDX->PrepareEditCtrl((int)nIDC); pDX->Fail(); }
    return FALSE;
}

// generic integer DDX
template <class T>
static void AfxDDX_TextInt(CDataExchange* pDX, int nIDC, T& value, UINT nIDS, LPCTSTR pszFormat)
{
    HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
    if (pDX->m_bSaveAndValidate) {
        TCHAR szText[64];
        if (AfxGetWindowText(hWndCtrl, szText, 64) == 0) {
            // an empty field is treated as 0 unless the value is required
            value = 0;
            return;
        }
        TCHAR* pEnd = NULL;
        LONGLONG n = (LONGLONG)_tcstoi64(szText, &pEnd, 10);
        if (pEnd == szText || (*pEnd != 0 && !AfxIsSpace(*pEnd))) {
            AfxDlgDataExchangeError(pDX, nIDC, nIDS);
            return;
        }
        value = (T)n;
    } else {
        CString str;
        str.Format(pszFormat, value);
        AfxSetWindowText(hWndCtrl, str);
    }
}

template <class T>
static void AfxDDX_TextUInt(CDataExchange* pDX, int nIDC, T& value, UINT nIDS, LPCTSTR pszFormat)
{
    HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
    if (pDX->m_bSaveAndValidate) {
        TCHAR szText[64];
        if (AfxGetWindowText(hWndCtrl, szText, 64) == 0) { value = 0; return; }
        TCHAR* pEnd = NULL;
        unsigned long long n = _tcstoui64(szText, &pEnd, 10);
        if (pEnd == szText || (*pEnd != 0 && !AfxIsSpace(*pEnd))) {
            AfxDlgDataExchangeError(pDX, nIDC, nIDS);
            return;
        }
        value = (T)n;
    } else {
        CString str;
        str.Format(pszFormat, value);
        AfxSetWindowText(hWndCtrl, str);
    }
}

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, BYTE& value)      { AfxDDX_TextUInt(pDX, nIDC, value, AFX_IDP_PARSE_BYTE, _T("%u")); }
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, short& value)     { AfxDDX_TextInt(pDX, nIDC, value, AFX_IDP_PARSE_INT, _T("%hd")); }
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, int& value)       { AfxDDX_TextInt(pDX, nIDC, value, AFX_IDP_PARSE_INT, _T("%d")); }
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, UINT& value)      { AfxDDX_TextUInt(pDX, nIDC, value, AFX_IDP_PARSE_UINT, _T("%u")); }
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, long& value)      { AfxDDX_TextInt(pDX, nIDC, value, AFX_IDP_PARSE_INT, _T("%ld")); }
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, DWORD& value)     { AfxDDX_TextUInt(pDX, nIDC, value, AFX_IDP_PARSE_UINT, _T("%lu")); }
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, LONGLONG& value)  { AfxDDX_TextInt(pDX, nIDC, value, AFX_IDP_PARSE_INT, _T("%I64d")); }

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, float& value)
{
    HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
    if (pDX->m_bSaveAndValidate) {
        TCHAR szText[64];
        if (AfxGetWindowText(hWndCtrl, szText, 64) == 0) { value = 0.0f; return; }
        TCHAR* pEnd = NULL;
        double d = _tcstod(szText, &pEnd);
        if (pEnd == szText || (*pEnd != 0 && !AfxIsSpace(*pEnd))) {
            AfxDlgDataExchangeError(pDX, nIDC, AFX_IDP_PARSE_REAL);
            return;
        }
        value = (float)d;
    } else {
        CString str;
        str.Format(_T("%g"), value);
        AfxSetWindowText(hWndCtrl, str);
    }
}

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, double& value)
{
    HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
    if (pDX->m_bSaveAndValidate) {
        TCHAR szText[64];
        if (AfxGetWindowText(hWndCtrl, szText, 64) == 0) { value = 0.0; return; }
        TCHAR* pEnd = NULL;
        double d = _tcstod(szText, &pEnd);
        if (pEnd == szText || (*pEnd != 0 && !AfxIsSpace(*pEnd))) {
            AfxDlgDataExchangeError(pDX, nIDC, AFX_IDP_PARSE_REAL);
            return;
        }
        value = d;
    } else {
        CString str;
        str.Format(_T("%g"), value);
        AfxSetWindowText(hWndCtrl, str);
    }
}

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, CString& value)
{
    HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
    if (pDX->m_bSaveAndValidate) {
        int nLen = ::GetWindowTextLength(hWndCtrl);
        LPTSTR p = value.GetBuffer(nLen + 1);
        int n = ::GetWindowText(hWndCtrl, p, nLen + 1);
        p[n] = 0;
        value.ReleaseBuffer(n);
    } else {
        ::SetWindowText(hWndCtrl, (LPCTSTR)value);
    }
}

void AFXAPI DDX_Check(CDataExchange* pDX, int nIDC, int& value)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    if (pDX->m_bSaveAndValidate) {
        int nCheck = (int)::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0);
        if (nCheck < BST_UNCHECKED || nCheck > BST_INDETERMINATE) nCheck = BST_UNCHECKED;
        value = nCheck;
    } else {
        if (value < BST_UNCHECKED || value > BST_INDETERMINATE) value = BST_UNCHECKED;
        ::SendMessage(hWndCtrl, BM_SETCHECK, value, 0);
    }
}

void AFXAPI DDX_Check(CDataExchange* pDX, int nIDC, bool& value)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    if (pDX->m_bSaveAndValidate)
        value = (::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0) == BST_CHECKED);
    else
        ::SendMessage(hWndCtrl, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
}

void AFXAPI DDX_Radio(CDataExchange* pDX, int nIDC, int& value)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    if (!pDX->m_bSaveAndValidate) value = -1;
    do {
        if (pDX->m_bSaveAndValidate) {
            if (::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0) != BST_UNCHECKED) {
                ASSERT(value == -1);   // only one button may be checked
                value = (int)::GetWindowLongPtr(hWndCtrl, GWLP_ID) - nIDC;
            }
        } else {
            ::SendMessage(hWndCtrl, BM_SETCHECK,
                          ((int)::GetWindowLongPtr(hWndCtrl, GWLP_ID) - nIDC == value) ? BST_CHECKED : BST_UNCHECKED, 0);
        }
        hWndCtrl = ::GetWindow(hWndCtrl, GW_HWNDNEXT);
    } while (hWndCtrl != NULL &&
             (::GetWindowLongPtr(hWndCtrl, GWL_STYLE) & WS_GROUP) == 0 &&
             ::SendMessage(hWndCtrl, WM_GETDLGCODE, 0, 0) & DLGC_RADIOBUTTON);
}

// --- list / combo helpers -------------------------------------------
void AFXAPI DDX_CBIndex(CDataExchange* pDX, int nIDC, int& index)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    index = pDX->m_bSaveAndValidate ? (int)::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0) : index;
    if (!pDX->m_bSaveAndValidate) ::SendMessage(hWndCtrl, CB_SETCURSEL, index, 0);
}

void AFXAPI DDX_LBIndex(CDataExchange* pDX, int nIDC, int& index)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    index = pDX->m_bSaveAndValidate ? (int)::SendMessage(hWndCtrl, LB_GETCURSEL, 0, 0) : index;
    if (!pDX->m_bSaveAndValidate) ::SendMessage(hWndCtrl, LB_SETCURSEL, index, 0);
}

void AFXAPI DDX_CBString(CDataExchange* pDX, int nIDC, CString& value)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    if (pDX->m_bSaveAndValidate) {
        int nSel = (int)::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0);
        if (nSel == CB_ERR) {
            // fall back to whatever the user typed
            int nLen = (int)::SendMessage(hWndCtrl, WM_GETTEXTLENGTH, 0, 0);
            LPTSTR p = value.GetBuffer(nLen + 1);
            int n = ::GetWindowText(hWndCtrl, p, nLen + 1);
            p[n] = 0;
            value.ReleaseBuffer(n);
        } else {
            int nLen = (int)::SendMessage(hWndCtrl, CB_GETLBTEXTLEN, nSel, 0);
            LPTSTR p = value.GetBuffer(nLen + 1);
            int n = (int)::SendMessage(hWndCtrl, CB_GETLBTEXT, nSel, (LPARAM)p);
            p[n > 0 ? n : 0] = 0;
            value.ReleaseBuffer(n > 0 ? n : 0);
        }
    } else {
        int nSel = (int)::SendMessage(hWndCtrl, CB_FINDSTRING, (WPARAM)-1, (LPARAM)(LPCTSTR)value);
        ::SendMessage(hWndCtrl, CB_SETCURSEL, nSel, 0);
        if (nSel == CB_ERR) ::SetWindowText(hWndCtrl, value);
    }
}

void AFXAPI DDX_CBStringExact(CDataExchange* pDX, int nIDC, CString& value)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    if (pDX->m_bSaveAndValidate) {
        int nSel = (int)::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0);
        int nLen = (int)::SendMessage(hWndCtrl, CB_GETLBTEXTLEN, nSel, 0);
        if (nLen == CB_ERR) nLen = 0;
        LPTSTR p = value.GetBuffer(nLen + 1);
        int n = (int)::SendMessage(hWndCtrl, CB_GETLBTEXT, nSel, (LPARAM)p);
        p[n > 0 ? n : 0] = 0;
        value.ReleaseBuffer(n > 0 ? n : 0);
    } else {
        int nSel = (int)::SendMessage(hWndCtrl, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(LPCTSTR)value);
        ::SendMessage(hWndCtrl, CB_SETCURSEL, nSel, 0);
    }
}

int AFXAPI DDX_CBError(CDataExchange* pDX, int nIDC, UINT nIDCErrorString)
{
    (void)nIDCErrorString;
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    return (int)::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0);
}

void AFXAPI DDX_LBString(CDataExchange* pDX, int nIDC, CString& value)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    if (pDX->m_bSaveAndValidate) {
        int nSel = (int)::SendMessage(hWndCtrl, LB_GETCURSEL, 0, 0);
        if (nSel == LB_ERR) value.Empty();
        else {
            int nLen = (int)::SendMessage(hWndCtrl, LB_GETTEXTLEN, nSel, 0);
            LPTSTR p = value.GetBuffer(nLen + 1);
            int n = (int)::SendMessage(hWndCtrl, LB_GETTEXT, nSel, (LPARAM)p);
            p[n > 0 ? n : 0] = 0;
            value.ReleaseBuffer(n > 0 ? n : 0);
        }
    } else {
        int nSel = (int)::SendMessage(hWndCtrl, LB_FINDSTRING, (WPARAM)-1, (LPARAM)(LPCTSTR)value);
        ::SendMessage(hWndCtrl, LB_SETCURSEL, nSel, 0);
    }
}

void AFXAPI DDX_LBStringExact(CDataExchange* pDX, int nIDC, CString& value)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    if (pDX->m_bSaveAndValidate) {
        int nSel = (int)::SendMessage(hWndCtrl, LB_GETCURSEL, 0, 0);
        int nLen = (int)::SendMessage(hWndCtrl, LB_GETTEXTLEN, nSel, 0);
        if (nLen == LB_ERR) nLen = 0;
        LPTSTR p = value.GetBuffer(nLen + 1);
        int n = (int)::SendMessage(hWndCtrl, LB_GETTEXT, nSel, (LPARAM)p);
        p[n > 0 ? n : 0] = 0;
        value.ReleaseBuffer(n > 0 ? n : 0);
    } else {
        int nSel = (int)::SendMessage(hWndCtrl, LB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(LPCTSTR)value);
        ::SendMessage(hWndCtrl, LB_SETCURSEL, nSel, 0);
    }
}

int AFXAPI DDX_LBError(CDataExchange* pDX, int nIDC, UINT nIDCErrorString)
{
    (void)nIDCErrorString;
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    return (int)::SendMessage(hWndCtrl, LB_GETCURSEL, 0, 0);
}

void AFXAPI DDX_Control(CDataExchange* pDX, int nIDC, CWnd& rControl)
{
    if (!::IsWindow(rControl.GetSafeHwnd())) {
        HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
        if (hWndCtrl == NULL) {
            CString str;
            str.Format(_T("DDX_Control: control id %d not found."), nIDC);
            AfxMessageBox(str);
            AfxThrowUserException();
        }
        if (!rControl.SubclassWindow(hWndCtrl)) {
            AfxTrace(__FILE__, __LINE__, _T("DDX_Control: subclassing failed\n"));
            AfxThrowUserException();
        }
    }
}

// ---------------------------------------------------------------------
//  DDV_ validation helpers
// ---------------------------------------------------------------------
void AFXAPI DDV_MinMaxByte(CDataExchange* pDX, BYTE const& value, BYTE minVal, BYTE maxVal)
{ if (pDX->m_bSaveAndValidate && (value < minVal || value > maxVal)) AfxDlgDataExchangeError(pDX, 0, AFX_IDP_PARSE_BYTE); }
void AFXAPI DDV_MinMaxShort(CDataExchange* pDX, short const& value, short minVal, short maxVal)
{ if (pDX->m_bSaveAndValidate && (value < minVal || value > maxVal)) AfxDlgDataExchangeError(pDX, 0, AFX_IDP_PARSE_INT_RANGE); }
void AFXAPI DDV_MinMaxInt(CDataExchange* pDX, int const& value, int minVal, int maxVal)
{ if (pDX->m_bSaveAndValidate && (value < minVal || value > maxVal)) AfxDlgDataExchangeError(pDX, 0, AFX_IDP_PARSE_INT_RANGE); }
void AFXAPI DDV_MinMaxUInt(CDataExchange* pDX, UINT const& value, UINT minVal, UINT maxVal)
{ if (pDX->m_bSaveAndValidate && (value < minVal || value > maxVal)) AfxDlgDataExchangeError(pDX, 0, AFX_IDP_PARSE_UINT); }
void AFXAPI DDV_MinMaxLong(CDataExchange* pDX, long const& value, long minVal, long maxVal)
{ if (pDX->m_bSaveAndValidate && (value < minVal || value > maxVal)) AfxDlgDataExchangeError(pDX, 0, AFX_IDP_PARSE_INT_RANGE); }
void AFXAPI DDV_MinMaxFloat(CDataExchange* pDX, float const& value, float minVal, float maxVal)
{ if (pDX->m_bSaveAndValidate && (value < minVal || value > maxVal)) AfxDlgDataExchangeError(pDX, 0, AFX_IDP_PARSE_REAL_RANGE); }
void AFXAPI DDV_MinMaxDouble(CDataExchange* pDX, double const& value, double minVal, double maxVal)
{ if (pDX->m_bSaveAndValidate && (value < minVal || value > maxVal)) AfxDlgDataExchangeError(pDX, 0, AFX_IDP_PARSE_REAL_RANGE); }

void AFXAPI DDV_MaxChars(CDataExchange* pDX, CString const& value, int nChars)
{
    if (pDX->m_bSaveAndValidate && value.GetLength() > nChars) {
        CString str;
        str.Format(_T("Please enter no more than %d characters."), nChars);
        AfxMessageBox(str, MB_ICONEXCLAMATION | MB_OK);
        pDX->Fail();
    }
}
