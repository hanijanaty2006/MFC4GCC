// =====================================================================
//  afxapp.cpp -- CWinThread / CWinApp / CCommandLineInfo / WinMain
// =====================================================================
#include <afxwin.h>
#include <afxcmn.h>
#include <afxext.h>
#include <afxdlgs.h>
#include <afxres.h>
#include <process.h>

// ---------------------------------------------------------------------
//  CCommandLineInfo
// ---------------------------------------------------------------------
CCommandLineInfo::CCommandLineInfo()
    : m_nShellCommand(FileNew), m_bRunEmbedded(FALSE), m_bRunAutomated(FALSE), m_bShowSplash(TRUE)
{
}
CCommandLineInfo::~CCommandLineInfo() {}

void CCommandLineInfo::ParseParam(LPCTSTR pszParam, BOOL bFlag, BOOL bLast)
{
    if (bFlag) ParseParamFlag(pszParam);
    else       ParseParamNotFlag(pszParam);
    ParseLast(bLast);
}

void CCommandLineInfo::ParseParamFlag(const TCHAR* pszParam)
{
    if (_tcsicmp(pszParam, _T("p")) == 0)              m_nShellCommand = FilePrint;
    else if (_tcsicmp(pszParam, _T("pt")) == 0) {      m_nShellCommand = FilePrintTo; }
    else if (_tcsicmp(pszParam, _T("pf")) == 0)        m_nShellCommand = FilePrintDirect;
    else if (_tcsicmp(pszParam, _T("dde")) == 0)       m_nShellCommand = FileDDE;
    else if (_tcsicmp(pszParam, _T("Embedding")) == 0) { m_bRunEmbedded = TRUE; m_nShellCommand = FileNew; }
    else if (_tcsicmp(pszParam, _T("Automation")) == 0){ m_bRunAutomated = TRUE; m_nShellCommand = FileNew; }
    else if (_tcsicmp(pszParam, _T("Reg")) == 0)       m_nShellCommand = AppRegister;
    else if (_tcsicmp(pszParam, _T("Unreg")) == 0 ||
             _tcsicmp(pszParam, _T("Unregister")) == 0) m_nShellCommand = AppUnregister;
    else if (_tcsicmp(pszParam, _T("noreg")) == 0)     { m_nShellCommand = FileNothing; }
    else if (_tcsicmp(pszParam, _T("?")) == 0 ||
             _tcsicmp(pszParam, _T("h")) == 0 ||
             _tcsicmp(pszParam, _T("help")) == 0)      m_nShellCommand = FileNothing;
}

void CCommandLineInfo::ParseParamNotFlag(const TCHAR* pszParam)
{
    // a bare file name on the command line => open it
    m_strFileName = pszParam;
    if (m_nShellCommand == FileNew) m_nShellCommand = FileOpen;
}

void CCommandLineInfo::ParseLast(BOOL bLast)
{
    if (bLast && m_nShellCommand == FileNew && m_strFileName.IsEmpty())
        m_nShellCommand = FileNew;
}

// ---------------------------------------------------------------------
//  CWinThread
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CWinThread, CCmdTarget)

CWinThread::CWinThread()
    : m_pMainWnd(NULL), m_pActiveWnd(NULL), m_bAutoDelete(FALSE), m_nThreadID(::GetCurrentThreadId()),
      m_hThread(NULL), m_pfnThreadProc(NULL), m_pThreadParams(NULL),
      m_bIdle(TRUE), m_nIdleCount(0), m_nStackSize(0)
{
    memset(&m_msgCur, 0, sizeof(m_msgCur));
    m_msgCur.hwnd = NULL;
}

CWinThread::~CWinThread()
{
    if (m_hThread != NULL) { ::CloseHandle(m_hThread); m_hThread = NULL; }
}

BEGIN_MESSAGE_MAP(CWinThread, CCmdTarget)
END_MESSAGE_MAP()

BOOL CWinThread::InitInstance() { return TRUE; }
int  CWinThread::ExitInstance() { return 0; }
void CWinThread::OnFinished() {}

BOOL CWinThread::IsIdleMessage(MSG* pMsg)
{
    switch (pMsg->message) {
    case WM_PAINT:
    case WM_NCPAINT:
    case WM_ERASEBKGND:
    case WM_SETCURSOR:
    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
    case WM_TIMER:
    case WM_WINDOWPOSCHANGED:
    case WM_NCCALCSIZE:
    case WM_MOUSEWHEEL:
    case WM_MOUSEHOVER:
        return FALSE;
    }
    if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) return TRUE;
    return TRUE;
}

BOOL CWinThread::OnIdle(LONG lCount)
{
    if (lCount == 0) {
        // clean up the temporary window / DC maps
        CWnd::DeleteTempMap();
        // tell the main window (and MDI child) to update their command UI
        if (m_pMainWnd != NULL && m_pMainWnd->GetSafeHwnd() != NULL)
            m_pMainWnd->SendMessage(WM_KICKIDLE);
        return TRUE;                 // there may be more idle work
    }
    return FALSE;
}

BOOL CWinThread::PumpMessage()
{
    if (!::GetMessage(&m_msgCur, NULL, 0, 0)) {
        // WM_QUIT: post it again so nested loops see it too
        ::PostQuitMessage((int)m_msgCur.wParam);
        return FALSE;
    }
    if (!PreTranslateMessage(&m_msgCur)) {
        ::TranslateMessage(&m_msgCur);
        ::DispatchMessage(&m_msgCur);
    }
    return TRUE;
}

int CWinThread::Run()
{
    BOOL bIdle = TRUE;
    LONG lIdleCount = 0;

    for (;;) {
        // idle processing while the queue is empty
        while (bIdle && !::PeekMessage(&m_msgCur, NULL, 0, 0, PM_NOREMOVE)) {
            if (!OnIdle(lIdleCount++)) bIdle = FALSE;
        }

        // pump messages until the queue is empty again
        do {
            if (!PumpMessage()) {
                ExitInstance();
                return 0;
            }
            if (IsIdleMessage(&m_msgCur)) { bIdle = TRUE; lIdleCount = 0; }
        } while (::PeekMessage(&m_msgCur, NULL, 0, 0, PM_NOREMOVE));
    }
}

BOOL CWinThread::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg == NULL || pMsg->hwnd == NULL) return FALSE;

    // give the app a chance to filter (common dialogs, print preview...)
    CWinApp* pApp = AfxGetApp();
    if (pApp != NULL && pApp->ProcessMessageFilter(-1, pMsg)) return TRUE;

    // modeless dialog keyboard handling
    if (m_pMainWnd != NULL && m_pMainWnd->GetSafeHwnd() != NULL) {
        HWND hWndMain = m_pMainWnd->GetSafeHwnd();
        if (m_pMainWnd->IsDialogWindow() && ::IsChild(hWndMain, pMsg->hwnd)) {
            if (::IsDialogMessage(hWndMain, pMsg)) return TRUE;
        }
        // MDI keyboard handling
        CWnd* pWnd = m_pMainWnd;
        if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
            HWND hWndClient = ((CMDIFrameWnd*)pWnd)->GetMDIClientHandle();
            if (hWndClient != NULL && ::IsChild(hWndClient, pMsg->hwnd)) {
                if (::TranslateMDISysAccel(hWndClient, pMsg)) return TRUE;
            }
        }
    }

    // walk up from the window that has the focus, letting each CWnd translate
    CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);
    while (pWnd != NULL) {
        if (pWnd->PreTranslateMessage(pMsg)) return TRUE;
        // frame accelerators
        if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
            if (((CFrameWnd*)pWnd)->TranslateAcceleratorMessage(pMsg)) return TRUE;
        }
        pWnd = pWnd->GetParent();
    }
    return FALSE;
}

BOOL CWinThread::CreateThread(DWORD dwCreateFlags, UINT nStackSize, LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
    m_nStackSize = nStackSize;
    UINT idThread = 0;
    HANDLE hThread = (HANDLE)::_beginthreadex(lpSecurityAttrs, nStackSize,
                                              (unsigned (__stdcall*)(void*))&CWinThread::ThreadEntry,
                                              this, dwCreateFlags | CREATE_SUSPENDED, &idThread);
    if (hThread == NULL) return FALSE;
    m_hThread = hThread;
    m_nThreadID = idThread;
    if ((dwCreateFlags & CREATE_SUSPENDED) == 0) ::ResumeThread(hThread);
    return TRUE;
}

UINT WINAPI CWinThread::ThreadEntry(LPVOID pParam)
{
    CWinThread* pThread = (CWinThread*)pParam;
    AfxInitThreadState(pThread);
    UINT nResult = pThread->RunWorker();
    pThread->OnFinished();
    if (pThread->m_bAutoDelete) delete pThread;
    return nResult;
}

UINT CWinThread::RunWorker()
{
    if (m_pfnThreadProc != NULL) return m_pfnThreadProc(m_pThreadParams);
    return 0;
}

int CWinThread::ResumeThread() { return ::ResumeThread(m_hThread); }
int CWinThread::SuspendThread() { return ::SuspendThread(m_hThread); }
BOOL CWinThread::PostThreadMessage(UINT message, WPARAM wParam, LPARAM lParam)
{ return ::PostThreadMessage(m_nThreadID, message, wParam, lParam); }
BOOL CWinThread::SetThreadPriority(int nPriority) { return ::SetThreadPriority(m_hThread, nPriority); }
int CWinThread::GetThreadPriority() { return ::GetThreadPriority(m_hThread); }

BOOL CWinThread::DispatchThreadMessageEx(MSG* lpMsg)
{
    if (m_pMainWnd != NULL && m_pMainWnd->PreTranslateMessage(lpMsg)) return TRUE;
    ::TranslateMessage(lpMsg);
    ::DispatchMessage(lpMsg);
    return FALSE;
}
void CWinThread::DispatchThreadMessage(MSG* lpMsg) { DispatchThreadMessageEx(lpMsg); }

void AFXAPI AfxInitThreadState(CWinThread* pThread) { AfxSetThread(pThread); }

void AFXAPI AfxEndThread(int nReturnCode, BOOL bDelete)
{
    (void)bDelete;
    ::_endthreadex((unsigned)nReturnCode);
}

CWinThread* AFXAPI AfxBeginThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam, int nPriority,
                                  UINT nStackSize, DWORD dwCreateFlags, LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
    CWinThread* pThread = new CWinThread();
    pThread->m_bAutoDelete = TRUE;
    pThread->m_pfnThreadProc = pfnThreadProc;
    pThread->m_pThreadParams = pParam;
    if (!pThread->CreateThread(dwCreateFlags | CREATE_SUSPENDED, nStackSize, lpSecurityAttrs)) {
        delete pThread;
        return NULL;
    }
    pThread->SetThreadPriority(nPriority);
    if ((dwCreateFlags & CREATE_SUSPENDED) == 0) pThread->ResumeThread();
    return pThread;
}

CWinThread* AFXAPI AfxBeginThread(CRuntimeClass* pThreadClass, int nPriority, UINT nStackSize,
                                  DWORD dwCreateFlags, LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
    if (pThreadClass == NULL || !pThreadClass->IsDerivedFrom(RUNTIME_CLASS(CWinThread))) return NULL;
    CWinThread* pThread = (CWinThread*)pThreadClass->CreateObject();
    if (pThread == NULL) return NULL;
    pThread->m_bAutoDelete = TRUE;
    if (!pThread->CreateThread(dwCreateFlags | CREATE_SUSPENDED, nStackSize, lpSecurityAttrs)) {
        delete pThread;
        return NULL;
    }
    pThread->SetThreadPriority(nPriority);
    if ((dwCreateFlags & CREATE_SUSPENDED) == 0) pThread->ResumeThread();
    return pThread;
}

// ---------------------------------------------------------------------
//  CWinApp
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CWinApp, CWinThread)

CWinApp::CWinApp(LPCTSTR lpszAppName)
    : m_hInstance(NULL), m_hPrevInstance(NULL), m_lpCmdLine(NULL), m_nCmdShow(SW_SHOWNORMAL),
      m_pszAppName(lpszAppName), m_pszRegistryKey(NULL), m_pszProfileName(NULL),
      m_pszHelpFilePath(NULL), m_pszExeName(NULL), m_pDocManager(NULL), m_nNumRecentFiles(4)
{
    if (m_pszAppName == NULL) {
        static TCHAR szName[MAX_PATH];
        DWORD n = ::GetModuleFileName(NULL, szName, MAX_PATH);
        if (n > 0) {
            // strip path + extension
            LPCTSTR p = szName;
            for (LPCTSTR q = szName; *q != 0; ++q)
                if (*q == '\\' || *q == '/') p = q + 1;
            static TCHAR szBase[MAX_PATH];
            AfxStrCopy(szBase, MAX_PATH, p);
            TCHAR* pDot = _tcsrchr(szBase, '.');
            if (pDot != NULL) *pDot = 0;
            m_pszAppName = szBase;
            m_pszExeName = szBase;
        }
    }
    if (m_pszExeName == NULL) m_pszExeName = m_pszAppName;
    m_pszProfileName = m_pszAppName;
    m_pDocManager = new CDocManager();
    AfxSetAppObject(this);
    AfxSetThread(this);
}

CWinApp::~CWinApp()
{
    delete m_pDocManager;
    m_pDocManager = NULL;
}

BEGIN_MESSAGE_MAP(CWinApp, CWinThread)
    ON_COMMAND(ID_APP_EXIT, CWinApp::OnAppExit)
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
    ON_COMMAND(ID_APP_ABOUT, CWinApp::OnAppAbout)
    ON_COMMAND_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, CWinApp::OnFileMruFile)
    ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, CWinApp::OnUpdateFileMruFile1)
END_MESSAGE_MAP()

BOOL CWinApp::InitApplication()
{
    LoadStdProfileSettings();
    return TRUE;
}

BOOL CWinApp::InitInstance() { return TRUE; }

int CWinApp::ExitInstance()
{
    CloseAllDocuments(TRUE);
    return 0;
}

int CWinApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDHelp)
{
    (void)nIDHelp;
    return AfxMessageBox(lpszPrompt, nType);
}

BOOL CWinApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
    (void)code; (void)lpMsg;
    return FALSE;
}

HCURSOR CWinApp::LoadCursor(UINT nID) { return ::LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(nID)); }
HICON CWinApp::LoadIcon(UINT nID) { return ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(nID)); }
BOOL CWinApp::LoadString(UINT nID, LPTSTR lpBuffer, int nMaxBufSize)
{ return ::LoadString(AfxGetResourceHandle(), nID, lpBuffer, nMaxBufSize) > 0; }

void CWinApp::WinHelp(DWORD_PTR dwData, UINT nCmd) { (void)dwData; (void)nCmd; }

void CWinApp::RestartApplication()
{
    TCHAR szPath[MAX_PATH * 2];
    ::GetModuleFileName(NULL, szPath, MAX_PATH);
    ::ShellExecute(NULL, _T("open"), szPath, m_lpCmdLine, NULL, SW_SHOWNORMAL);
    if (m_pMainWnd != NULL) m_pMainWnd->PostMessage(WM_CLOSE);
}

int CWinApp::GetExitCode() const
{
    if (m_hThread == NULL) return 0;
    DWORD dwCode = 0;
    ::GetExitCodeThread(m_hThread, &dwCode);
    return (int)dwCode;
}

void CWinApp::Enable3dControls() {}
void CWinApp::Enable3dControlsStatic() {}
void CWinApp::SetCurrentHandles() {}
void CWinApp::DoWaitCursor(int nCode)
{
    if (nCode == 0) ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
    else if (nCode > 0) ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}

// ---------------------------------------------------------------------
//  registry backed profile
// ---------------------------------------------------------------------
static CString AfxRegKeyPath(CWinApp* pApp, LPCTSTR lpszSection)
{
    CString strPath;
    if (pApp->m_pszRegistryKey != NULL && *pApp->m_pszRegistryKey != 0) {
        strPath.Format(_T("Software\\%s\\%s"), pApp->m_pszRegistryKey, pApp->m_pszProfileName);
        if (lpszSection != NULL && *lpszSection != 0) { strPath += _T("\\"); strPath += lpszSection; }
    } else {
        // no registry key: use an INI file next to the executable
        CString strIni;
        strIni.Format(_T("%s.ini"), pApp->m_pszProfileName ? pApp->m_pszProfileName : _T("app"));
        if (lpszSection != NULL) strIni = strIni;
        return strIni;   // (caller decides)
    }
    return strPath;
}

void CWinApp::SetRegistryKey(LPCTSTR lpszRegistryKey)
{
    m_pszRegistryKey = lpszRegistryKey;
    if (m_pszProfileName == NULL) m_pszProfileName = m_pszAppName;
    LoadStdProfileSettings();
}

void CWinApp::SetRegistryKey(UINT nIDRegistryKey)
{
    CString str;
    if (str.LoadString(nIDRegistryKey)) {
        // keep the string alive for the lifetime of the app
        TCHAR* p = new TCHAR[str.GetLength() + 1];
        AfxStrCopy(p, str.GetLength() + 1, (LPCTSTR)str);
        m_pszRegistryKey = p;
    }
}

BOOL CWinApp::GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes)
{
    *ppData = NULL; *pBytes = 0;
    if (m_pszRegistryKey == NULL) return FALSE;
    CString strPath = AfxRegKeyPath(this, lpszSection);
    HKEY hKey;
    if (::RegOpenKeyEx(HKEY_CURRENT_USER, strPath, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) return FALSE;
    DWORD dwType = 0, dwSize = 0;
    BOOL bResult = FALSE;
    if (::RegQueryValueEx(hKey, lpszEntry, NULL, &dwType, NULL, &dwSize) == ERROR_SUCCESS &&
        dwType == REG_BINARY && dwSize > 0) {
        *ppData = new BYTE[dwSize];
        if (::RegQueryValueEx(hKey, lpszEntry, NULL, &dwType, *ppData, &dwSize) == ERROR_SUCCESS) {
            *pBytes = dwSize;
            bResult = TRUE;
        } else {
            delete[] *ppData; *ppData = NULL; *pBytes = 0;
        }
    }
    ::RegCloseKey(hKey);
    return bResult;
}

BOOL CWinApp::WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes)
{
    if (m_pszRegistryKey == NULL) return FALSE;
    CString strPath = AfxRegKeyPath(this, lpszSection);
    HKEY hKey;
    if (::RegCreateKeyEx(HKEY_CURRENT_USER, strPath, 0, NULL, REG_OPTION_NON_VOLATILE,
                         KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) return FALSE;
    LONG lResult = ::RegSetValueEx(hKey, lpszEntry, 0, REG_BINARY, pData, nBytes);
    ::RegCloseKey(hKey);
    return lResult == ERROR_SUCCESS;
}

UINT CWinApp::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
    CString str = GetProfileString(lpszSection, lpszEntry, NULL);
    if (str.IsEmpty()) return (UINT)nDefault;
    return (UINT)AfxStrToInt(str);
}

BOOL CWinApp::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
{
    CString str;
    str.Format(_T("%d"), nValue);
    return WriteProfileString(lpszSection, lpszEntry, str);
}

CString CWinApp::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
    CString strDefault = lpszDefault ? lpszDefault : _T("");
    if (m_pszRegistryKey == NULL) return strDefault;
    CString strPath = AfxRegKeyPath(this, lpszSection);
    HKEY hKey;
    if (::RegOpenKeyEx(HKEY_CURRENT_USER, strPath, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return strDefault;
    TCHAR szBuf[4096];
    DWORD dwSize = sizeof(szBuf);
    DWORD dwType = 0;
    LONG lResult = ::RegQueryValueEx(hKey, lpszEntry, NULL, &dwType, (LPBYTE)szBuf, &dwSize);
    ::RegCloseKey(hKey);
    if (lResult != ERROR_SUCCESS || dwType != REG_SZ) return strDefault;
    szBuf[dwSize / sizeof(TCHAR)] = 0;
    return CString(szBuf);
}

BOOL CWinApp::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
    if (m_pszRegistryKey == NULL) return FALSE;
    CString strPath = AfxRegKeyPath(this, lpszSection);
    HKEY hKey;
    if (::RegCreateKeyEx(HKEY_CURRENT_USER, strPath, 0, NULL, REG_OPTION_NON_VOLATILE,
                         KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) return FALSE;
    LONG lResult;
    if (lpszValue == NULL) lResult = ::RegDeleteValue(hKey, lpszEntry);
    else lResult = ::RegSetValueEx(hKey, lpszEntry, 0, REG_SZ, (const BYTE*)lpszValue,
                                   (DWORD)((AfxStrLen(lpszValue) + 1) * sizeof(TCHAR)));
    ::RegCloseKey(hKey);
    return lResult == ERROR_SUCCESS;
}

BOOL CWinApp::DelRegTree(LPCTSTR lpszKeyName)
{
    CString strPath = AfxRegKeyPath(this, lpszKeyName);
    return ::RegDeleteTree(HKEY_CURRENT_USER, strPath) == ERROR_SUCCESS;
}

int CWinApp::GetProfileSection(LPCTSTR lpszSection, LPTSTR lpReturnBuffer, DWORD nSize)
{
    if (m_pszRegistryKey == NULL) return 0;
    CString strPath = AfxRegKeyPath(this, lpszSection);
    return (int)::RegGetValue(HKEY_CURRENT_USER, strPath, NULL, RRF_RT_ANY, NULL, lpReturnBuffer, &nSize);
}

BOOL CWinApp::WriteProfileSection(LPCTSTR lpszSection, LPCTSTR lpszValue)
{
    if (m_pszRegistryKey == NULL) return FALSE;
    CString strPath = AfxRegKeyPath(this, lpszSection);
    HKEY hKey;
    if (::RegCreateKeyEx(HKEY_CURRENT_USER, strPath, 0, NULL, REG_OPTION_NON_VOLATILE,
                         KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) return FALSE;
    LONG lResult = ::RegSetValueEx(hKey, NULL, 0, REG_MULTI_SZ, (const BYTE*)lpszValue,
                                   (DWORD)((AfxStrLen(lpszValue) + 2) * sizeof(TCHAR)));
    ::RegCloseKey(hKey);
    return lResult == ERROR_SUCCESS;
}

// ---------------------------------------------------------------------
//  MRU (most recently used) file list
// ---------------------------------------------------------------------
void CWinApp::LoadStdProfileSettings(UINT nMaxMRU)
{
    if (nMaxMRU > _AFX_MRU_COUNT) nMaxMRU = _AFX_MRU_COUNT;
    m_nNumRecentFiles = nMaxMRU;
    m_recentFileList.RemoveAll();
    for (UINT i = 0; i < nMaxMRU; ++i) {
        CString strEntry;
        strEntry.Format(_T("File%u"), i + 1);
        CString str = GetProfileString(_T("Recent File List"), strEntry, NULL);
        if (str.IsEmpty()) break;
        m_recentFileList.Add(str);
    }
}

void CWinApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
    if (lpszPathName == NULL || *lpszPathName == 0) return;
    if (m_nNumRecentFiles == 0) LoadStdProfileSettings();

    // remove any existing occurrence
    for (int i = 0; i < m_recentFileList.GetSize(); ++i) {
        if (m_recentFileList[i].CompareNoCase(lpszPathName) == 0) { m_recentFileList.RemoveAt(i); break; }
    }
    m_recentFileList.InsertAt(0, CString(lpszPathName));
    while (m_recentFileList.GetSize() > (int)m_nNumRecentFiles)
        m_recentFileList.RemoveAt(m_recentFileList.GetSize() - 1);

    // persist
    for (int i = 0; i < m_recentFileList.GetSize(); ++i) {
        CString strEntry;
        strEntry.Format(_T("File%d"), i + 1);
        WriteProfileString(_T("Recent File List"), strEntry, m_recentFileList[i]);
    }
    CString strEntry;
    strEntry.Format(_T("File%d"), m_recentFileList.GetSize() + 1);
    WriteProfileString(_T("Recent File List"), strEntry, NULL);
}

CString CWinApp::GetRecentFileListEntry(int nIndex) const
{
    if (nIndex < 0 || nIndex >= m_recentFileList.GetSize()) return CString();
    return m_recentFileList[nIndex];
}

int CWinApp::GetRecentFileListCount() const { return m_recentFileList.GetSize(); }

void CWinApp::OnUpdateFileMruFile1(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
    int nIndex = (int)pCmdUI->m_nID - (int)ID_FILE_MRU_FILE1;
    if (nIndex >= m_recentFileList.GetSize()) { pCmdUI->Enable(FALSE); return; }
    CString str = m_recentFileList[nIndex];
    CString strItem;
    strItem.Format(_T("&%d %s"), nIndex + 1, (LPCTSTR)str);
    pCmdUI->SetText(strItem);
}

void CWinApp::OnFileMruFile(UINT nID)
{
    int nIndex = (int)nID - (int)ID_FILE_MRU_FILE1;
    if (nIndex < 0 || nIndex >= m_recentFileList.GetSize()) return;
    CString strPath = m_recentFileList[nIndex];
    if (OpenDocumentFile(strPath) == NULL) m_recentFileList.RemoveAt(nIndex);
}

// ---------------------------------------------------------------------
//  document templates
// ---------------------------------------------------------------------
void CWinApp::AddDocTemplate(CDocTemplate* pTemplate)
{
    if (m_pDocManager == NULL) m_pDocManager = new CDocManager();
    m_pDocManager->AddDocTemplate(pTemplate);
}

POSITION CWinApp::GetFirstDocTemplatePosition() const { return m_pDocManager->GetFirstDocTemplatePosition(); }
CDocTemplate* CWinApp::GetNextDocTemplate(POSITION& pos) const { return m_pDocManager->GetNextDocTemplate(pos); }
CDocument* CWinApp::OpenDocumentFile(LPCTSTR lpszFileName) { return m_pDocManager->OpenDocumentFile(lpszFileName); }
BOOL CWinApp::SaveAllModified() { return m_pDocManager->SaveAllModified(); }
void CWinApp::CloseAllDocuments(BOOL bEndSession) { m_pDocManager->CloseAllDocuments(bEndSession); }
int CWinApp::GetOpenDocumentCount() { return m_pDocManager->GetOpenDocumentCount(); }
void CWinApp::EnableShellOpen() { RegisterShellFileTypes(TRUE); }

void CWinApp::RegisterShellFileTypes(BOOL bCompat) { (void)bCompat; m_pDocManager->RegisterShellFileTypes(bCompat); }
void CWinApp::UnregisterShellFileTypes() { m_pDocManager->UnregisterShellFileTypes(); }

void CWinApp::ParseCommandLine(CCommandLineInfo& rCmdInfo)
{
    CString strCmd(m_lpCmdLine ? m_lpCmdLine : _T(""));
    int nLen = strCmd.GetLength();
    int i = 0;
    while (i < nLen) {
        while (i < nLen && (strCmd[i] == ' ' || strCmd[i] == '\t')) ++i;
        if (i >= nLen) break;
        BOOL bFlag = FALSE;
        if (strCmd[i] == '-' || strCmd[i] == '/') { bFlag = TRUE; ++i; }
        int nStart = i;
        if (i < nLen && strCmd[i] == '"') {
            ++i; nStart = i;
            while (i < nLen && strCmd[i] != '"') ++i;
        } else {
            while (i < nLen && strCmd[i] != ' ' && strCmd[i] != '\t') ++i;
        }
        CString strParam = strCmd.Mid(nStart, i - nStart);
        if (!bFlag && i < nLen && strCmd[i] == '"') ++i;
        rCmdInfo.ParseParam(strParam, bFlag, (i >= nLen));
    }
}

BOOL CWinApp::ProcessShellCommand(CCommandLineInfo& rCmdInfo)
{
    BOOL bResult = TRUE;
    switch (rCmdInfo.m_nShellCommand) {
    case CCommandLineInfo::FileNew:
        OnFileNew();
        if (m_pMainWnd == NULL) bResult = FALSE;
        break;
    case CCommandLineInfo::FileOpen:
        if (OpenDocumentFile(rCmdInfo.m_strFileName) == NULL) bResult = FALSE;
        break;
    case CCommandLineInfo::FilePrint:
    case CCommandLineInfo::FilePrintDirect:
    case CCommandLineInfo::FilePrintTo:
        {
            CDocument* pDoc = OpenDocumentFile(rCmdInfo.m_strFileName);
            if (pDoc == NULL) bResult = FALSE;
            // printing itself is left to the view (OnFilePrint)
        }
        break;
    case CCommandLineInfo::FileDDE:
        bResult = TRUE;
        break;
    case CCommandLineInfo::AppRegister:
    case CCommandLineInfo::AppUnregister:
        bResult = TRUE;
        break;
    case CCommandLineInfo::FileNothing:
        bResult = TRUE;
        break;
    default:
        bResult = FALSE;
        break;
    }
    return bResult;
}

// ---------------------------------------------------------------------
//  Standard app commands
// ---------------------------------------------------------------------
void CWinApp::OnAppExit()
{
    if (m_pMainWnd != NULL) m_pMainWnd->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
    else ::PostQuitMessage(0);
}

void CWinApp::OnFileNew() { if (m_pDocManager != NULL) m_pDocManager->OnFileNew(); }
void CWinApp::OnFileOpen() { if (m_pDocManager != NULL) m_pDocManager->OnFileOpen(); }

void CWinApp::OnFilePrintSetup()
{
    CPrintDialog dlg(FALSE);
    dlg.DoModal();
}

void CWinApp::OnAppAbout()
{
    // use IDD_ABOUTBOX if the application provides one, else build a message
    // an application supplied about box wins over the built-in message
    if (::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(AFX_IDD_ABOUTBOX), RT_DIALOG) != NULL) {
        CDialog dlg(AFX_IDD_ABOUTBOX);
        dlg.DoModal();
        return;
    }
    if (::FindResource(AfxGetResourceHandle(), _T("IDD_ABOUTBOX"), RT_DIALOG) != NULL) {
        CDialog dlg(_T("IDD_ABOUTBOX"));
        dlg.DoModal();
        return;
    }
    CString str;
    TCHAR szExe[MAX_PATH];
    ::GetModuleFileName(NULL, szExe, MAX_PATH);
    str.Format(_T("%s\n\nBuilt with the MFC-for-GCC framework.\n"), m_pszAppName ? m_pszAppName : szExe);
    AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
}

// ---------------------------------------------------------------------
//  WinMain / wWinMain
// ---------------------------------------------------------------------
int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    AfxSetInstanceHandles(hInstance, hPrevInstance);
    AfxEnableVisualStyles();
    AfxInitCommonControls();
    AfxInitRichEdit2();

    CWinApp* pApp = AfxGetApp();
    if (pApp == NULL) {
        ::MessageBox(NULL,
                     _T("No CWinApp object was created before WinMain.\n")
                     _T("Declare one global CWinApp-derived object in your program."),
                     _T("MFC-GCC framework"), MB_ICONERROR | MB_OK);
        return 0;
    }

    AfxSetThread(pApp);
    pApp->m_hInstance = hInstance;
    pApp->m_hPrevInstance = hPrevInstance;
    pApp->m_lpCmdLine = lpCmdLine;
    pApp->m_nCmdShow = nCmdShow;
    pApp->m_hThread = ::GetCurrentThread();
    pApp->m_nThreadID = ::GetCurrentThreadId();
    AfxSetMainWndHandle(NULL);

    if (!pApp->InitApplication()) { pApp->ExitInstance(); return 0; }
    if (!pApp->InitInstance()) { pApp->ExitInstance(); return 0; }

    int nResult = pApp->Run();

    // tidy up: destroy any window that is still around
    if (pApp->m_pMainWnd != NULL && pApp->m_pMainWnd->GetSafeHwnd() != NULL)
        pApp->m_pMainWnd->DestroyWindow();
    CWnd::DeleteTempMap();

    int nExit = pApp->ExitInstance();
    AfxSetThread(NULL);
    return (nResult != 0) ? nResult : nExit;
}

extern "C" int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef UNICODE
    CStringW strCmd = lpCmdLine ? AfxMultiByteToWide(lpCmdLine).c_str() : L"";
    return AfxWinMain(hInstance, hPrevInstance, strCmd.GetBuffer(), nCmdShow);
#else
    return AfxWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#endif
}

extern "C" int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
#ifdef UNICODE
    return AfxWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#else
    CStringA strCmd = lpCmdLine ? AfxWideToMultiByte(lpCmdLine).c_str() : "";
    return AfxWinMain(hInstance, hPrevInstance, strCmd.GetBuffer(), nCmdShow);
#endif
}
