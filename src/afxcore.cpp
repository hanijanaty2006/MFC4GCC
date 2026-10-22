// =====================================================================
//  afxcore.cpp -- core implementation: global state, diagnostics,
//                 CRuntimeClass, CFile, CArchive, CTime, sync objects.
// =====================================================================
#include <afxwin.h>
#include <afxmt.h>
#include <afxres.h>
#include <io.h>
#include <direct.h>
#include <sys/stat.h>

// ---------------------------------------------------------------------
//  Global state
// ---------------------------------------------------------------------
namespace afxstate {
    HINSTANCE   s_hInstance     = NULL;
    HINSTANCE   s_hResource     = NULL;
    CWinApp*    s_pApp          = NULL;
    CWinThread* s_pThread       = NULL;
    HWND        s_hMainWnd      = NULL;
    bool        s_bExiting      = false;
}

HINSTANCE AFXAPI AfxGetInstanceHandle()   { return afxstate::s_hInstance; }
HINSTANCE AFXAPI AfxGetResourceHandle()   { return afxstate::s_hResource ? afxstate::s_hResource : afxstate::s_hInstance; }
HINSTANCE AFXAPI AfxSetResourceHandle(HINSTANCE hInst)
{
    HINSTANCE hOld = afxstate::s_hResource;
    afxstate::s_hResource = hInst;
    return hOld;
}
CWinApp*  AFXAPI AfxGetApp()              { return afxstate::s_pApp; }

void AFXAPI AfxSetInstanceHandles(HINSTANCE hInstance, HINSTANCE hPrevInstance)
{
    afxstate::s_hInstance = hInstance;
    if (afxstate::s_hResource == NULL) afxstate::s_hResource = hInstance;
    (void)hPrevInstance;
}
void AFXAPI AfxSetThread(CWinThread* pThread) { afxstate::s_pThread = pThread; }
void AFXAPI AfxSetMainWndHandle(HWND hWnd) { afxstate::s_hMainWnd = hWnd; }
void AFXAPI AfxSetAppObject(CWinApp* pApp) { afxstate::s_pApp = pApp; }
CWinThread* AFXAPI AfxGetThread()         { return afxstate::s_pThread; }
HWND AFXAPI AfxGetMainHwnd()              { return afxstate::s_hMainWnd; }

LPTSTR AFXAPI AfxGetAppName()
{
    CWinApp* pApp = AfxGetApp();
    return pApp ? (LPTSTR)pApp->m_pszAppName : NULL;
}

// ---------------------------------------------------------------------
//  MFC magic constants
// ---------------------------------------------------------------------
const RECT AFXAPI rectDefault = { 0, 0, 0, 0 };
const SIZE AFXAPI sizeDefault = { -1, -1 };

// ---------------------------------------------------------------------
//  Character conversions
// ---------------------------------------------------------------------
std::wstring AFXAPI AfxMultiByteToWide(const char* psz, int nLen, UINT cp)
{
    if (psz == NULL) return std::wstring();
    if (nLen < 0) nLen = (int)strlen(psz);
    if (nLen == 0) return std::wstring();
    int nChars = ::MultiByteToWideChar(cp, 0, psz, nLen, NULL, 0);
    if (nChars <= 0) return std::wstring();
    std::wstring w((size_t)nChars, L'\0');
    ::MultiByteToWideChar(cp, 0, psz, nLen, &w[0], nChars);
    return w;
}

std::string AFXAPI AfxWideToMultiByte(const wchar_t* psz, int nLen, UINT cp)
{
    if (psz == NULL) return std::string();
    if (nLen < 0) nLen = (int)wcslen(psz);
    if (nLen == 0) return std::string();
    int nBytes = ::WideCharToMultiByte(cp, 0, psz, nLen, NULL, 0, NULL, NULL);
    if (nBytes <= 0) return std::string();
    std::string s((size_t)nBytes, '\0');
    ::WideCharToMultiByte(cp, 0, psz, nLen, &s[0], nBytes, NULL, NULL);
    return s;
}

// ---------------------------------------------------------------------
//  printf helpers
// ---------------------------------------------------------------------
int AFXAPI AfxSprintfLen(const char* pszFormat, ...)
{
    va_list args; va_start(args, pszFormat);
    int n = vsnprintf(NULL, 0, pszFormat, args);
    va_end(args);
    return n;
}
int AFXAPI AfxSprintfLen(const wchar_t* pszFormat, ...)
{
    va_list args; va_start(args, pszFormat);
    int n = _vscwprintf(pszFormat, args);
    va_end(args);
    return n;
}
int AFXAPI AfxSprintfTo(char* pszBuf, size_t nMaxLen, const char* pszFormat, ...)
{
    va_list args; va_start(args, pszFormat);
    int n = AfxSprintfV(pszBuf, nMaxLen, pszFormat, args);
    va_end(args);
    return n;
}
int AFXAPI AfxSprintfTo(wchar_t* pszBuf, size_t nMaxLen, const wchar_t* pszFormat, ...)
{
    va_list args; va_start(args, pszFormat);
    int n = AfxSprintfV(pszBuf, nMaxLen, pszFormat, args);
    va_end(args);
    return n;
}

// ---------------------------------------------------------------------
//  Diagnostics
// ---------------------------------------------------------------------
int AFXAPI AfxTrace(LPCSTR pszFile, int nLine, LPCTSTR pszFormat, ...)
{
    if (pszFormat == NULL) return 0;
    TCHAR szBuf[2048];
    va_list args; va_start(args, pszFormat);
    AfxSprintfV(szBuf, 2048, pszFormat, args);
    va_end(args);

    TCHAR szFull[2304];
#ifdef _DEBUG
    AfxSprintfTo(szFull, 2304, _T("%hs(%d) : %s"), pszFile ? pszFile : "", nLine, szBuf);
#else
    (void)pszFile; (void)nLine;
    AfxStrCopy(szFull, 2304, szBuf);
#endif
    ::OutputDebugString(szFull);
#ifdef _AFX_TRACE_TO_STDERR
    fputs(AfxWideToMultiByte(szFull).c_str(), stderr);
    fflush(stderr);
#endif
    return 0;
}

void AFXAPI AfxAssertFailed(LPCSTR pszFile, int nLine, LPCSTR pszExpr)
{
    char szBuf[512];
    _snprintf(szBuf, sizeof(szBuf), "ASSERT failed\nFile: %s\nLine: %d\nExpression: %s",
              pszFile ? pszFile : "?", nLine, pszExpr ? pszExpr : "?");
    szBuf[sizeof(szBuf) - 1] = 0;
    ::OutputDebugStringA(szBuf);
#ifdef _DEBUG
    int nResult = ::MessageBoxA(::GetActiveWindow(), szBuf,
                                "MFC-GCC Assertion Failed (Abort / Retry / Ignore)",
                                MB_ABORTRETRYIGNORE | MB_ICONHAND | MB_TASKMODAL);
    if (nResult == IDRETRY) ::DebugBreak();
    if (nResult == IDABORT) ::abort();
#endif
}

void AFXAPI AfxAssertValidObject(const CObject* pOb, LPCSTR pszFile, int nLine)
{
    if (pOb == NULL) {
#ifdef _DEBUG
        char szBuf[256];
        _snprintf(szBuf, sizeof(szBuf), "ASSERT_VALID called with NULL pointer\nFile: %s\nLine: %d", pszFile, nLine);
        szBuf[sizeof(szBuf) - 1] = 0;
        ::OutputDebugStringA(szBuf);
#endif
        return;
    }
    CRuntimeClass* pClass = pOb->GetRuntimeClass();
    if (pClass == NULL) AfxAssertFailed(pszFile, nLine, "GetRuntimeClass() == NULL");
}

int AFXAPI AfxMessageBox(LPCTSTR lpszText, UINT nType, UINT nIDHelp)
{
    (void)nIDHelp;
    HWND hWnd = AfxGetMainHwnd();
    if (hWnd == NULL) hWnd = ::GetActiveWindow();
    if (lpszText == NULL) lpszText = _T("");
    int nResult = ::MessageBox(hWnd, lpszText, AfxGetAppName() ? AfxGetAppName() : _T(""), nType);
    if (nResult == 0) nResult = IDABORT;
    return nResult;
}

int AFXAPI AfxMessageBox(UINT nIDPrompt, UINT nType, UINT nIDHelp)
{
    CString strText;
    if (!strText.LoadString(nIDPrompt)) {
        // prompt string not found: show a generic message
        return AfxMessageBox(_T("Error: invalid message id."), nType, nIDHelp);
    }
    return AfxMessageBox((LPCTSTR)strText, nType, nIDHelp);
}

int CException::ReportError(UINT nType, DWORD dwMessageID)
{
    (void)dwMessageID;
    TCHAR szBuf[512];
    if (GetErrorMessage(szBuf, 512))
        return AfxMessageBox(szBuf, nType);
    return AfxMessageBox(_T("Unknown Error."), nType);
}

void AFXAPI AfxThrowUserException()   { throw CUserException(); }
void AFXAPI AfxThrowMemoryException() { throw CMemoryException(); }
void AFXAPI AfxThrowFileException(int cause, LONG lOsError, LPCTSTR pszFileName)
{ throw CFileException(cause, lOsError, pszFileName); }
void AFXAPI AfxThrowSimpleException(LPCTSTR pszMessage) { throw CSimpleException(pszMessage); }

int CFileException::OsErrorToException(LONG lOsError)
{
    switch (lOsError) {
    case 0:                 return none;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_DRIVE:
    case ERROR_NO_MORE_FILES:
    case ERROR_BAD_NET_NAME:
    case ERROR_BAD_NETPATH:
    case ERROR_INVALID_NAME:return fileNotFound;
    case ERROR_ACCESS_DENIED:
    case ERROR_INVALID_ACCESS:
    case ERROR_SHARING_VIOLATION:
    case ERROR_NETWORK_ACCESS_DENIED:
    case ERROR_LOCK_VIOLATION:
    case ERROR_DRIVE_LOCKED:return accessDenied;
    case ERROR_DISK_FULL:   return diskFull;
    case ERROR_INVALID_HANDLE:
    case ERROR_INVALID_TARGET_HANDLE:
    case ERROR_DIRECT_ACCESS_HANDLE: return invalidFile;
    case ERROR_SEEK_ON_DEVICE:
    case ERROR_NEGATIVE_SEEK: return badSeek;
    case ERROR_TOO_MANY_OPEN_FILES: return tooManyOpenFiles;
    case ERROR_NOT_DOS_DISK:
    case ERROR_NOT_READY:
    case ERROR_BAD_UNIT:
    case ERROR_OPEN_FAILED:
    case ERROR_IO_DEVICE:
    case ERROR_SEEK:        return hardIO;
    case ERROR_BAD_FORMAT:
    case ERROR_INVALID_BLOCK:
    case ERROR_INVALID_DATA:return genericException;
    default:                return genericException;
    }
}

void CFileException::ThrowOsError(LONG lOsError, LPCTSTR pszFileName)
{ throw CFileException(OsErrorToException(lOsError), lOsError, pszFileName); }

BOOL CFileException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext)
{
    if (pnHelpContext != NULL) *pnHelpContext = 0;
    if (lpszError == NULL || nMaxError == 0) return FALSE;
    lpszError[0] = 0;
    TCHAR szMessage[256];
    DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    DWORD n = ::FormatMessage(dwFlags, NULL, (DWORD)m_lOsError,
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              szMessage, 256, NULL);
    if (n == 0) AfxStrCopy(szMessage, 256, _T("Unrecognized error"));
    // strip trailing newline
    size_t len = AfxStrLen(szMessage);
    while (len > 0 && (szMessage[len - 1] == '\r' || szMessage[len - 1] == '\n')) szMessage[--len] = 0;

    if (!m_strFileName.IsEmpty())
        AfxSprintfTo(lpszError, nMaxError, _T("%s\n(%s)"), szMessage, (LPCTSTR)m_strFileName);
    else
        AfxStrCopy(lpszError, nMaxError, szMessage);
    return TRUE;
}

// ---------------------------------------------------------------------
//  CRuntimeClass registry
// ---------------------------------------------------------------------
static CRuntimeClass* s_pFirstRuntimeClass = NULL;
static CRITICAL_SECTION s_rtcsLock;
static bool s_rtcsInited = false;

static void AfxLockRtc()
{
    if (!s_rtcsInited) { ::InitializeCriticalSection(&s_rtcsLock); s_rtcsInited = true; }
    ::EnterCriticalSection(&s_rtcsLock);
}
static void AfxUnlockRtc() { if (s_rtcsInited) ::LeaveCriticalSection(&s_rtcsLock); }

AFX_CLASSINIT::AFX_CLASSINIT(CRuntimeClass* pNewClass)
{
    AfxLockRtc();
    pNewClass->m_pNextClass = s_pFirstRuntimeClass;
    s_pFirstRuntimeClass = pNewClass;
    AfxUnlockRtc();
}

BOOL CRuntimeClass::IsDerivedFrom(const CRuntimeClass* pBaseClass) const
{
    for (const CRuntimeClass* pClass = this; pClass != NULL; pClass = pClass->m_pBaseClass)
        if (pClass == pBaseClass) return TRUE;
    return FALSE;
}

CRuntimeClass* CRuntimeClass::FromName(LPCSTR lpszClassName)
{
    if (lpszClassName == NULL) return NULL;
    CRuntimeClass* pResult = NULL;
    AfxLockRtc();
    for (CRuntimeClass* pClass = s_pFirstRuntimeClass; pClass != NULL; pClass = pClass->m_pNextClass) {
        if (strcmp(lpszClassName, pClass->m_lpszClassName) == 0) { pResult = pClass; break; }
    }
    AfxUnlockRtc();
    return pResult;
}

void CRuntimeClass::AssertValid() const
{
    if (m_lpszClassName == NULL || AfxStrLen(m_lpszClassName) == 0)
        AfxAssertFailed(__FILE__, __LINE__, "CRuntimeClass::m_lpszClassName");
    if (m_nObjectSize <= 0)
        AfxAssertFailed(__FILE__, __LINE__, "CRuntimeClass::m_nObjectSize");
}

static char _lpsz_CObject[] = "CObject";
CRuntimeClass CObject::classCObject =
    { _lpsz_CObject, sizeof(CObject), 0xFFFF, NULL, NULL, NULL };
static const AFX_CLASSINIT _init_CObject(&CObject::classCObject);

void CObject::Serialize(CArchive& ar)
{
    // base class does nothing; derived classes override
    (void)ar;
}

// ---------------------------------------------------------------------
//  small time helpers
// ---------------------------------------------------------------------
static void afx_gmtime(const time_t* t, struct tm* out)
{
    const struct tm* p = ::gmtime(t);
    if (p != NULL) *out = *p; else memset(out, 0, sizeof(*out));
}
static void afx_localtime(const time_t* t, struct tm* out)
{
    const struct tm* p = ::localtime(t);
    if (p != NULL) *out = *p; else memset(out, 0, sizeof(*out));
}
#define gmtime_r_afx(t, out)    afx_gmtime((t), (out))
#define localtime_r_afx(t, out) afx_localtime((t), (out))

static time_t AfxFileTimeToTime(ULONGLONG ft)
{
    // FILETIME: 100ns intervals since 1601-01-01 UTC
    if (ft < 116444736000000000ULL) return 0;
    return (time_t)((ft - 116444736000000000ULL) / 10000000ULL);
}

// ---------------------------------------------------------------------
//  CTime / CTimeSpan
// ---------------------------------------------------------------------
CTime::CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST)
{
    struct tm atm;
    atm.tm_sec = nSec; atm.tm_min = nMin; atm.tm_hour = nHour;
    atm.tm_mday = nDay; atm.tm_mon = nMonth - 1; atm.tm_year = nYear - 1900;
    atm.tm_isdst = nDST;
    m_time = ::mktime(&atm);
}

struct tm* CTime::GetGmtTm(struct tm* ptm) const
{
    static struct tm s_tm;
    if (ptm == NULL) ptm = &s_tm;
    struct tm* pTmp = ptm;
    gmtime_r_afx(&m_time, pTmp);
    return pTmp;
}

struct tm* CTime::GetLocalTm(struct tm* ptm) const
{
    static struct tm s_tm;
    if (ptm == NULL) ptm = &s_tm;
    localtime_r_afx(&m_time, ptm);
    return ptm;
}

int CTime::GetYear() const      { struct tm t; localtime_r_afx(&m_time, &t); return t.tm_year + 1900; }
int CTime::GetMonth() const     { struct tm t; localtime_r_afx(&m_time, &t); return t.tm_mon + 1; }
int CTime::GetDay() const       { struct tm t; localtime_r_afx(&m_time, &t); return t.tm_mday; }
int CTime::GetHour() const      { struct tm t; localtime_r_afx(&m_time, &t); return t.tm_hour; }
int CTime::GetMinute() const    { struct tm t; localtime_r_afx(&m_time, &t); return t.tm_min; }
int CTime::GetSecond() const    { struct tm t; localtime_r_afx(&m_time, &t); return t.tm_sec; }
int CTime::GetDayOfWeek() const { struct tm t; localtime_r_afx(&m_time, &t); return t.tm_wday + 1; }

CString CTime::Format(LPCTSTR pszFormat) const
{
    struct tm tmVal;
    localtime_r_afx(&m_time, &tmVal);
    TCHAR szBuf[512];
    size_t n = _tcsftime(szBuf, 512, pszFormat, &tmVal);
    if (n == 0) return CString();
    return CString(szBuf, (int)n);
}

CString CTime::FormatGmt(LPCTSTR pszFormat) const
{
    struct tm tmVal;
    gmtime_r_afx(&m_time, &tmVal);
    TCHAR szBuf[512];
    size_t n = _tcsftime(szBuf, 512, pszFormat, &tmVal);
    if (n == 0) return CString();
    return CString(szBuf, (int)n);
}

CString CTime::FormatDateTime(LPCTSTR pszDateFmt, LPCTSTR pszTimeFmt) const
{
    CString str;
    str += Format(pszDateFmt ? pszDateFmt : _T("%Y-%m-%d"));
    str += _T(" ");
    str += Format(pszTimeFmt ? pszTimeFmt : _T("%H:%M:%S"));
    return str;
}

CString CTimeSpan::Format(LPCTSTR pszFormat) const
{
    struct tm tmVal;
    memset(&tmVal, 0, sizeof(tmVal));
    time_t t = m_timeSpan;
    gmtime_r_afx(&t, &tmVal);
    TCHAR szBuf[512];
    size_t n = _tcsftime(szBuf, 512, pszFormat, &tmVal);
    if (n == 0) return CString();
    return CString(szBuf, (int)n);
}

// ---------------------------------------------------------------------
//  CFile
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CFile, CObject)

CFile::CFile() : m_hFile(INVALID_HANDLE_VALUE), m_bCloseOnDelete(FALSE) {}

CFile::CFile(HANDLE hFile) : m_hFile(hFile), m_bCloseOnDelete(FALSE) {}

CFile::CFile(LPCTSTR lpszFileName, UINT nOpenFlags)
    : m_hFile(INVALID_HANDLE_VALUE), m_strFileName(lpszFileName), m_bCloseOnDelete(TRUE)
{
    CFileException e;
    if (!Open(lpszFileName, nOpenFlags, &e))
        throw e;
}

CFile::~CFile()
{
    if (m_hFile != INVALID_HANDLE_VALUE && m_bCloseOnDelete) Close();
}

BOOL CFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
    m_strFileName = lpszFileName;
    if (pError != NULL) pError->m_cause = CFileException::none;

    DWORD dwAccess = 0, dwShare = 0, dwCreate = 0;
    switch (nOpenFlags & 3) {
    case modeRead:      dwAccess = GENERIC_READ; break;
    case modeWrite:     dwAccess = GENERIC_WRITE; break;
    case modeReadWrite: dwAccess = GENERIC_READ | GENERIC_WRITE; break;
    }
    switch (nOpenFlags & (shareExclusive | shareDenyWrite | shareDenyRead | shareDenyNone)) {
    case shareExclusive:  dwShare = 0; break;
    case shareDenyWrite:  dwShare = FILE_SHARE_READ; break;
    case shareDenyRead:   dwShare = FILE_SHARE_WRITE; break;
    case shareDenyNone:   dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE; break;
    default:              dwShare = FILE_SHARE_READ; break;
    }
    if (nOpenFlags & modeCreate)
        dwCreate = (nOpenFlags & modeNoTruncate) ? OPEN_ALWAYS : CREATE_ALWAYS;
    else
        dwCreate = OPEN_EXISTING;

    DWORD dwFlags = FILE_ATTRIBUTE_NORMAL;
    if (nOpenFlags & osNoBuffer) dwFlags |= FILE_FLAG_NO_BUFFERING;

    HANDLE hFile = ::CreateFile(lpszFileName, dwAccess, dwShare, NULL, dwCreate, dwFlags, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD dwErr = ::GetLastError();
        if (pError != NULL) {
            pError->m_cause = CFileException::OsErrorToException((LONG)dwErr);
            pError->m_lOsError = (LONG)dwErr;
            pError->m_strFileName = lpszFileName;
            return FALSE;
        }
        CFileException::ThrowOsError((LONG)dwErr, lpszFileName);
    }
    if (m_hFile != INVALID_HANDLE_VALUE && m_bCloseOnDelete) ::CloseHandle(m_hFile);
    m_hFile = hFile;
    m_bCloseOnDelete = TRUE;
    return TRUE;
}

void CFile::Close()
{
    if (m_hFile != INVALID_HANDLE_VALUE) {
        ::FlushFileBuffers(m_hFile);
        if (m_bCloseOnDelete) ::CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
    m_bCloseOnDelete = FALSE;
}

void CFile::Abort() { if (m_hFile != INVALID_HANDLE_VALUE) { ::CloseHandle(m_hFile); m_hFile = INVALID_HANDLE_VALUE; } m_bCloseOnDelete = FALSE; }
void CFile::Flush() { if (m_hFile != INVALID_HANDLE_VALUE) ::FlushFileBuffers(m_hFile); }

UINT_PTR CFile::Read(void* lpBuf, UINT_PTR nCount)
{
    if (nCount == 0) return 0;
    DWORD nRead = 0;
    if (!::ReadFile(m_hFile, lpBuf, (DWORD)nCount, &nRead, NULL)) {
        DWORD dwErr = ::GetLastError();
        if (dwErr == ERROR_HANDLE_EOF) return 0;
        CFileException::ThrowOsError((LONG)dwErr, m_strFileName);
    }
    return nRead;
}

void CFile::Write(const void* lpBuf, UINT_PTR nCount)
{
    if (nCount == 0) return;
    DWORD nWritten = 0;
    if (!::WriteFile(m_hFile, lpBuf, (DWORD)nCount, &nWritten, NULL) || nWritten != (DWORD)nCount)
        CFileException::ThrowOsError((LONG)::GetLastError(), m_strFileName);
}

LONG CFile::Seek(LONG lOff, UINT nFrom)
{
    DWORD dwMoveMethod;
    switch (nFrom) {
    case FILE_BEGIN:   dwMoveMethod = FILE_BEGIN; break;
    case FILE_CURRENT: dwMoveMethod = FILE_CURRENT; break;
    case FILE_END:     dwMoveMethod = FILE_END; break;
    default: AfxThrowSimpleException(_T("CFile::Seek bad origin")); return -1;
    }
    LONG lResult = (LONG)::SetFilePointer(m_hFile, lOff, NULL, dwMoveMethod);
    if (lResult == (LONG)INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR)
        CFileException::ThrowOsError((LONG)::GetLastError(), m_strFileName);
    return lResult;
}

LONG CFile::GetPosition() const
{
    LONG lPos = (LONG)::SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
    if (lPos == (LONG)INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR) return -1;
    return lPos;
}

DWORD CFile::GetLength() const { return ::GetFileSize(m_hFile, NULL); }

void CFile::SetLength(DWORD dwNewLen)
{
    LONG lOldPos = GetPosition();
    Seek((LONG)dwNewLen, FILE_BEGIN);
    ::SetEndOfFile(m_hFile);
    Seek(lOldPos, FILE_BEGIN);
}

BOOL CFile::LockRange(DWORD dwPos, DWORD dwCount)
{
    OVERLAPPED ov; ZeroMemory(&ov, sizeof(ov));
    ov.Offset = dwPos; ov.OffsetHigh = 0;
    return ::LockFile(m_hFile, ov.Offset, 0, dwCount, 0) ? TRUE : FALSE;
}

BOOL CFile::UnlockRange(DWORD dwPos, DWORD dwCount)
{
    OVERLAPPED ov; ZeroMemory(&ov, sizeof(ov));
    ov.Offset = dwPos; ov.OffsetHigh = 0;
    return ::UnlockFile(m_hFile, ov.Offset, 0, dwCount, 0) ? TRUE : FALSE;
}

void CFile::Attach(HANDLE hFile) { m_hFile = hFile; m_bCloseOnDelete = TRUE; }

HANDLE CFile::Detach()
{
    HANDLE h = m_hFile;
    m_hFile = INVALID_HANDLE_VALUE;
    m_bCloseOnDelete = FALSE;
    return h;
}

CString CFile::GetFileTitle() const
{
    LPCTSTR p = m_strFileName;
    LPCTSTR pTitle = p;
    while (*p != 0) { if (*p == '\\' || *p == '/' || *p == ':') pTitle = p + 1; ++p; }
    return CString(pTitle);
}

CString CFile::GetFilePath() const
{
    TCHAR szFull[_MAX_PATH];
    DWORD n = ::GetFullPathName(m_strFileName, _MAX_PATH, szFull, NULL);
    if (n == 0 || n > _MAX_PATH) return m_strFileName;
    return CString(szFull);
}

BOOL CFile::GetStatus(CFileStatus& rStatus) const { return GetStatus(m_strFileName, rStatus); }

void CFile::SetStatus(const CFileStatus& status) { SetStatus(m_strFileName, status); }

BOOL CFile::GetStatus(LPCTSTR lpszFileName, CFileStatus& rStatus)
{
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!::GetFileAttributesEx(lpszFileName, GetFileExInfoStandard, &fad)) return FALSE;
    rStatus = CFileStatus();
    ULARGE_INTEGER uli;
    uli.LowPart = fad.ftCreationTime.dwLowDateTime; uli.HighPart = fad.ftCreationTime.dwHighDateTime;
    rStatus.m_ctime = CTime(AfxFileTimeToTime(uli.QuadPart));
    uli.LowPart = fad.ftLastWriteTime.dwLowDateTime; uli.HighPart = fad.ftLastWriteTime.dwHighDateTime;
    rStatus.m_mtime = CTime(AfxFileTimeToTime(uli.QuadPart));
    uli.LowPart = fad.ftLastAccessTime.dwLowDateTime; uli.HighPart = fad.ftLastAccessTime.dwHighDateTime;
    rStatus.m_atime = CTime(AfxFileTimeToTime(uli.QuadPart));
    rStatus.m_size = (LONG)fad.nFileSizeLow;
    rStatus.m_attribute = (BYTE)fad.dwFileAttributes;
    rStatus.m_szFullName = lpszFileName;
    return TRUE;
}

void CFile::SetStatus(LPCTSTR lpszFileName, const CFileStatus& status)
{
    ::SetFileAttributes(lpszFileName, status.m_attribute);
}

void CFile::Rename(LPCTSTR lpszOldName, LPCTSTR lpszNewName)
{
    if (!::MoveFileEx(lpszOldName, lpszNewName, MOVEFILE_REPLACE_EXISTING))
        CFileException::ThrowOsError((LONG)::GetLastError(), lpszOldName);
}

void CFile::Remove(LPCTSTR lpszFileName)
{
    if (!::DeleteFile(lpszFileName))
        CFileException::ThrowOsError((LONG)::GetLastError(), lpszFileName);
}

BOOL CFile::ReadString(CString& rString)
{
    rString.Empty();
    TCHAR ch;
    DWORD nRead = 0;
    BOOL bAny = FALSE;
    while (::ReadFile(m_hFile, &ch, sizeof(TCHAR), &nRead, NULL) && nRead == sizeof(TCHAR)) {
        bAny = TRUE;
        if (ch == '\n') break;
        if (ch != '\r') rString += ch;
    }
    return bAny;
}

void CFile::WriteString(LPCTSTR lpsz)
{
    Write(lpsz, (UINT_PTR)(AfxStrLen(lpsz) * sizeof(TCHAR)));
    TCHAR nl = '\n';
    Write(&nl, sizeof(TCHAR));
}

// ---------------------------------------------------------------------
//  CStdioFile
// ---------------------------------------------------------------------
CStdioFile::CStdioFile() : m_pStream(NULL) { m_hFile = INVALID_HANDLE_VALUE; }
CStdioFile::CStdioFile(FILE* pOpenStream) : m_pStream(pOpenStream) { m_hFile = INVALID_HANDLE_VALUE; }
CStdioFile::CStdioFile(LPCTSTR lpszFileName, UINT nOpenFlags) : m_pStream(NULL)
{
    CFileException e;
    if (!Open(lpszFileName, nOpenFlags, &e)) throw e;
}
CStdioFile::~CStdioFile() { if (m_pStream != NULL) fclose(m_pStream); }

BOOL CStdioFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
    m_strFileName = lpszFileName;
    const TCHAR* pszMode;
    UINT nFlags = nOpenFlags & (modeRead | modeWrite | modeReadWrite | modeCreate | modeNoTruncate);
    BOOL bText = (nOpenFlags & typeBinary) == 0;
    if (nFlags & modeCreate)        pszMode = bText ? _T("w+t") : _T("w+b");
    else if ((nFlags & 3) == modeReadWrite) pszMode = bText ? _T("r+t") : _T("r+b");
    else if ((nFlags & 3) == modeWrite)     pszMode = bText ? _T("w+t") : _T("w+b");
    else                                    pszMode = bText ? _T("rt") : _T("rb");

    m_pStream = _tfopen(lpszFileName, pszMode);
    if (m_pStream == NULL) {
        LONG err = (LONG)::GetLastError();
        if (pError != NULL) { pError->m_cause = CFileException::OsErrorToException(err); pError->m_lOsError = err; return FALSE; }
        CFileException::ThrowOsError(err, lpszFileName);
    }
    m_hFile = (HANDLE)_get_osfhandle(_fileno(m_pStream));
    return TRUE;
}

LPTSTR CStdioFile::ReadString(LPTSTR lpsz, UINT nMax)
{
    if (m_pStream == NULL) return NULL;
    return _fgetts(lpsz, (int)nMax, m_pStream);
}

BOOL CStdioFile::ReadString(CString& rString)
{
    rString.Empty();
    TCHAR szBuf[4096];
    if (ReadString(szBuf, 4096) == NULL) return FALSE;
    rString = szBuf;
    rString.TrimRight(_T('\r'));
    rString.TrimRight(_T('\n'));
    return TRUE;
}

void CStdioFile::WriteString(LPCTSTR lpsz)
{
    if (m_pStream != NULL) { _fputts(lpsz, m_pStream); _fputts(_T("\n"), m_pStream); fflush(m_pStream); }
}

// ---------------------------------------------------------------------
//  CArchive
// ---------------------------------------------------------------------
CArchive::CArchive(CFile* pFile, UINT nMode) : m_nMode(nMode), m_pFile(pFile) {}
CArchive::~CArchive() { Close(); }
void CArchive::Close() { if (m_pFile != NULL) { m_pFile->Flush(); m_pFile = NULL; } }
void CArchive::Flush() { if (m_pFile != NULL) m_pFile->Flush(); }

UINT CArchive::Read(void* lpBuf, UINT nMax)
{
    UINT_PTR n = m_pFile->Read(lpBuf, nMax);
    if (n != nMax) throw CFileException(CFileException::endOfFile, 0, m_pFile->GetFileName());
    return (UINT)n;
}

void CArchive::Write(const void* lpBuf, UINT nCount) { m_pFile->Write(lpBuf, nCount); }

CArchive& CArchive::operator<<(const CStringA& str)
{
    UINT nLen = (UINT)str.GetLength();
    *this << nLen;
    if (nLen) Write((LPCSTR)str, nLen);
    return *this;
}
CArchive& CArchive::operator<<(const CStringW& str)
{
    UINT nLen = (UINT)str.GetLength();
    *this << nLen;
    if (nLen) Write((LPCWSTR)str, nLen * sizeof(wchar_t));
    return *this;
}
CArchive& CArchive::operator>>(CStringA& str)
{
    UINT nLen = 0; *this >> nLen;
    if (nLen == 0) { str.Empty(); return *this; }
    char* p = str.GetBuffer((int)nLen + 1);
    Read(p, nLen);
    p[nLen] = 0;
    str.ReleaseBuffer((int)nLen);
    return *this;
}
CArchive& CArchive::operator>>(CStringW& str)
{
    UINT nLen = 0; *this >> nLen;
    if (nLen == 0) { str.Empty(); return *this; }
    wchar_t* p = str.GetBuffer((int)nLen + 1);
    Read(p, nLen * sizeof(wchar_t));
    p[nLen] = 0;
    str.ReleaseBuffer((int)nLen);
    return *this;
}

void CArchive::WriteObject(const CObject* pOb)
{
    if (pOb == NULL) { WORD w = 0; Write(&w, sizeof(w)); return; }
    CRuntimeClass* pClass = pOb->GetRuntimeClass();
    CStringA name(pClass ? pClass->m_lpszClassName : "CObject");
    *this << name;
    WORD schema = (WORD)(pClass ? pClass->m_wSchema : 0xFFFF);
    Write(&schema, sizeof(schema));
    const_cast<CObject*>(pOb)->Serialize(*this);
}

CObject* CArchive::ReadObject(const CRuntimeClass* pClass)
{
    CStringA name;
    *this >> name;
    if (name.IsEmpty()) return NULL;
    WORD schema = 0;
    Read(&schema, sizeof(schema));
    CRuntimeClass* pLoaded = CRuntimeClass::FromName(name);
    if (pLoaded == NULL) return NULL;
    if (pClass != NULL && !pLoaded->IsDerivedFrom(pClass)) return NULL;
    CObject* pOb = pLoaded->CreateObject();
    if (pOb == NULL) return NULL;
    const_cast<CObject*>(pOb)->Serialize(*this);
    return pOb;
}

// ---------------------------------------------------------------------
//  Synchronisation objects
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CSyncObject, CObject)
IMPLEMENT_DYNAMIC(CCriticalSection, CSyncObject)
IMPLEMENT_DYNAMIC(CSemaphore, CSyncObject)
IMPLEMENT_DYNAMIC(CMutex, CSyncObject)
IMPLEMENT_DYNAMIC(CEvent, CSyncObject)

CSyncObject::CSyncObject(LPCTSTR pszName) : m_hObject(NULL), m_strName(pszName ? pszName : _T("")) {}

CSyncObject::~CSyncObject()
{
    if (m_hObject != NULL) { ::CloseHandle(m_hObject); m_hObject = NULL; }
}

BOOL CSyncObject::Lock(DWORD dwTimeout)
{
    if (m_hObject == NULL) return FALSE;
    DWORD dwResult = ::WaitForSingleObject(m_hObject, dwTimeout);
    return (dwResult == WAIT_OBJECT_0 || dwResult == WAIT_ABANDONED) ? TRUE : FALSE;
}

BOOL CSyncObject::Unlock() { return TRUE; }

CCriticalSection::CCriticalSection() : CSyncObject(NULL) { ::InitializeCriticalSection(&m_sect); }
CCriticalSection::CCriticalSection(LPCTSTR pszName) : CSyncObject(pszName) { ::InitializeCriticalSection(&m_sect); }
CCriticalSection::~CCriticalSection() { ::DeleteCriticalSection(&m_sect); }
BOOL CCriticalSection::Lock(DWORD dwTimeout) { (void)dwTimeout; Enter(); return TRUE; }
BOOL CCriticalSection::Lock() { Enter(); return TRUE; }
BOOL CCriticalSection::Unlock() { Leave(); return TRUE; }

CSemaphore::CSemaphore(LONG lInitialCount, LONG lMaxCount, LPCTSTR pszName, LPSECURITY_ATTRIBUTES lpsa)
    : CSyncObject(pszName)
{
    m_hObject = ::CreateSemaphore(lpsa, lInitialCount, lMaxCount, pszName);
}
BOOL CSemaphore::Unlock() { return ::ReleaseSemaphore(m_hObject, 1, NULL) ? TRUE : FALSE; }
BOOL CSemaphore::Unlock(LONG lCount, LONG* lpPrevCount) { return ::ReleaseSemaphore(m_hObject, lCount, lpPrevCount) ? TRUE : FALSE; }
BOOL CSemaphore::Lock(DWORD dwTimeout) { return CSyncObject::Lock(dwTimeout); }

CMutex::CMutex(BOOL bInitiallyOwn, LPCTSTR pszName, LPSECURITY_ATTRIBUTES lpsa) : CSyncObject(pszName)
{ m_hObject = ::CreateMutex(lpsa, bInitiallyOwn, pszName); }
BOOL CMutex::Unlock() { return ::ReleaseMutex(m_hObject) ? TRUE : FALSE; }
BOOL CMutex::Lock(DWORD dwTimeout) { return CSyncObject::Lock(dwTimeout); }

CEvent::CEvent(BOOL bInitiallyOwn, BOOL bManualReset, LPCTSTR pszName, LPSECURITY_ATTRIBUTES lpsa)
    : CSyncObject(pszName)
{ m_hObject = ::CreateEvent(lpsa, bManualReset, bInitiallyOwn, pszName); }
BOOL CEvent::SetEvent()   { return ::SetEvent(m_hObject) ? TRUE : FALSE; }
BOOL CEvent::PulseEvent() { return ::PulseEvent(m_hObject) ? TRUE : FALSE; }
BOOL CEvent::ResetEvent() { return ::ResetEvent(m_hObject) ? TRUE : FALSE; }
BOOL CEvent::Unlock()     { return SetEvent(); }
BOOL CEvent::Lock(DWORD dwTimeout) { return CSyncObject::Lock(dwTimeout); }

CSingleLock::CSingleLock(CSyncObject* pObject, BOOL bInitialLock)
    : m_pObject(pObject), m_bAcquired(FALSE)
{
    if (bInitialLock) Lock();
}
CSingleLock::~CSingleLock() { if (m_bAcquired) Unlock(); }
BOOL CSingleLock::Lock(DWORD dwTimeout)
{
    if (m_pObject == NULL) return FALSE;
    m_bAcquired = m_pObject->Lock(dwTimeout);
    return m_bAcquired;
}
BOOL CSingleLock::Lock(LONGLONG dwTimeout) { return Lock((DWORD)dwTimeout); }
BOOL CSingleLock::Unlock()
{
    if (m_pObject == NULL) return FALSE;
    BOOL bRet = m_pObject->Unlock();
    m_bAcquired = FALSE;
    return bRet;
}

CMultiLock::CMultiLock(CSyncObject* const* ppObjects, DWORD dwCount, BOOL bInitialLock)
    : m_dwCount(dwCount > AFX_MAX_WAIT_OBJECTS ? AFX_MAX_WAIT_OBJECTS : dwCount), m_bHaveLocked(FALSE)
{
    for (DWORD i = 0; i < m_dwCount; ++i) { m_pObjects[i] = ppObjects[i]; m_bLocked[i] = FALSE; }
    if (bInitialLock) Lock();
}
CMultiLock::~CMultiLock() { if (m_bHaveLocked) Unlock(); }

DWORD CMultiLock::Lock(DWORD dwTimeout, BOOL bWaitForAll, BOOL bIgnoreAlerts)
{
    (void)bIgnoreAlerts;
    HANDLE handles[AFX_MAX_WAIT_OBJECTS];
    for (DWORD i = 0; i < m_dwCount; ++i) handles[i] = (HANDLE)(*m_pObjects[i]);
    DWORD dwResult = ::WaitForMultipleObjects(m_dwCount, handles, bWaitForAll, dwTimeout);
    if (dwResult >= WAIT_OBJECT_0 && dwResult < WAIT_OBJECT_0 + m_dwCount) {
        m_bHaveLocked = TRUE;
        if (bWaitForAll) for (DWORD i = 0; i < m_dwCount; ++i) m_bLocked[i] = TRUE;
        else m_bLocked[dwResult - WAIT_OBJECT_0] = TRUE;
    }
    return dwResult;
}
DWORD CMultiLock::Lock(LONGLONG dwTimeout, BOOL bWaitForAll) { return Lock((DWORD)dwTimeout, bWaitForAll); }

BOOL CMultiLock::Unlock()
{
    for (DWORD i = 0; i < m_dwCount; ++i) {
        if (m_bLocked[i]) { m_pObjects[i]->Unlock(); m_bLocked[i] = FALSE; }
    }
    m_bHaveLocked = FALSE;
    return TRUE;
}
BOOL CMultiLock::IsLocked(DWORD dwItem) const { return dwItem < m_dwCount ? m_bLocked[dwItem] : FALSE; }
