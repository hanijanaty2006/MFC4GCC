// =====================================================================
//  afxapp.h  --  CWinThread / CWinApp / CCommandLineInfo for the
//                MFC-GCC clone.  This is what makes `CMyApp theApp;`
//  work exactly like a real MFC program.
// =====================================================================
#ifndef __AFXAPP_H__
#define __AFXAPP_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif
#include "afxwnd.h"

class CDocManager;
class CDocTemplate;
class CDocument;
class CFrameWnd;

typedef UINT (AFX_CDECL* AFX_THREADPROC)(LPVOID);

#define _AFX_MRU_COUNT 16

// ---------------------------------------------------------------------
//  CCommandLineInfo
// ---------------------------------------------------------------------
class CCommandLineInfo : public CObject
{
public:
    enum EShellCommand {
        FileNothing = -1, FileNew, FileOpen, FilePrint, FilePrintTo,
        FileDDE, AppUnregister, AppRegister, FilePrintPreview, FilePrintDirect
    };
    EShellCommand m_nShellCommand;
    BOOL          m_bRunEmbedded;
    BOOL          m_bRunAutomated;
    CString       m_strFileName;
    BOOL          m_bShowSplash;

    CCommandLineInfo();
    virtual ~CCommandLineInfo();
    virtual void ParseParam(LPCTSTR pszParam, BOOL bFlag, BOOL bLast);
    void ParseParamFlag(const TCHAR* pszParam);
    void ParseParamNotFlag(const TCHAR* pszParam);
    void ParseLast(BOOL bLast);
};

// ---------------------------------------------------------------------
//  CWinThread
// ---------------------------------------------------------------------
class CWinThread : public CCmdTarget
{
    DECLARE_DYNAMIC(CWinThread)
    DECLARE_MESSAGE_MAP()
public:
    CWinThread();
    virtual ~CWinThread();


    // overridables
    virtual BOOL InitInstance();
    virtual int  ExitInstance();
    virtual int  Run();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnIdle(LONG lCount);
    virtual BOOL IsIdleMessage(MSG* pMsg);
    virtual void OnFinished();

    // attributes
    CWnd*  m_pMainWnd;      // main window (usually a CFrameWnd or CDialog)
    CWnd*  m_pActiveWnd;    // active MDI child window
    BOOL   m_bAutoDelete;   // TRUE => 'delete this' when the thread ends
    DWORD  m_nThreadID;
    HANDLE m_hThread;
    MSG    m_msgCur;
    operator HANDLE() const { return m_hThread; }

    // operations
    BOOL CreateThread(DWORD dwCreateFlags = 0, UINT nStackSize = 0,
                      LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);
    int  ResumeThread();
    int  SuspendThread();
    BOOL PostThreadMessage(UINT message, WPARAM wParam, LPARAM lParam);
    BOOL SetThreadPriority(int nPriority);
    int  GetThreadPriority();
    BOOL DispatchThreadMessageEx(MSG* lpMsg);
    void DispatchThreadMessage(MSG* lpMsg);

    // implementation
    BOOL PumpMessage();     // returns FALSE when WM_QUIT is received

    // worker-thread plumbing
    AFX_THREADPROC m_pfnThreadProc;
    LPVOID         m_pThreadParams;

protected:
    BOOL  m_bIdle;          // TRUE when idle processing is needed
    LONG  m_nIdleCount;
    UINT  m_nStackSize;
    static UINT WINAPI ThreadEntry(LPVOID pParam);
    virtual UINT RunWorker();

    friend CWinThread* AFXAPI AfxBeginThread(AFX_THREADPROC, LPVOID, int, UINT, DWORD, LPSECURITY_ATTRIBUTES);
};

// ---------------------------------------------------------------------
//  CWinApp
// ---------------------------------------------------------------------
class CWinApp : public CWinThread
{
    DECLARE_DYNAMIC(CWinApp)
public:
    CWinApp(LPCTSTR lpszAppName = NULL);
    virtual ~CWinApp();

    // overridables
    virtual BOOL InitApplication();
    virtual BOOL InitInstance();
    virtual int  ExitInstance();
    virtual int  DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDHelp);
    virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
    virtual void ParseCommandLine(CCommandLineInfo& rCmdInfo);
    virtual BOOL ProcessShellCommand(CCommandLineInfo& rCmdInfo);
    virtual HCURSOR LoadCursor(UINT nID);
    virtual HICON LoadIcon(UINT nID);
    virtual BOOL LoadString(UINT nID, LPTSTR lpBuffer, int nMaxBufSize);

    // attributes
    HINSTANCE   m_hInstance;
    HINSTANCE   m_hPrevInstance;
    LPTSTR      m_lpCmdLine;
    int         m_nCmdShow;
    LPCTSTR     m_pszAppName;        // human readable name ("MyApp")
    LPCTSTR     m_pszRegistryKey;    // e.g. "MyCompany"  (used by profile funcs)
    LPCTSTR     m_pszProfileName;    // e.g. "MyApp"
    LPCTSTR     m_pszHelpFilePath;
    LPCTSTR     m_pszExeName;
    CDocManager* m_pDocManager;
    CCommandLineInfo m_cmdInfo;

    // registry-backed application profile
    void SetRegistryKey(LPCTSTR lpszRegistryKey);
    void SetRegistryKey(UINT nIDRegistryKey);
    UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
    BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
    CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL);
    BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
    BOOL GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes);
    BOOL WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);
    BOOL DelRegTree(LPCTSTR lpszKeyName);

    // document templates
    void AddDocTemplate(CDocTemplate* pTemplate);
    POSITION GetFirstDocTemplatePosition() const;
    CDocTemplate* GetNextDocTemplate(POSITION& pos) const;
    CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
    BOOL SaveAllModified();
    void CloseAllDocuments(BOOL bEndSession);
    int  GetOpenDocumentCount();
    void EnableShellOpen();
    void RegisterShellFileTypes(BOOL bCompat = FALSE);
    void UnregisterShellFileTypes();

    // misc
    void WinHelp(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
    void RestartApplication();
    void DoWaitCursor(int nCode);   // 0 = restore, 1 = wait cursor, -1 = previous
    BOOL WriteProfileSection(LPCTSTR lpszSection, LPCTSTR lpszValue);
    int  GetProfileSection(LPCTSTR lpszSection, LPTSTR lpReturnBuffer, DWORD nSize);

    // implementation
    void LoadStdProfileSettings(UINT nMaxMRU = _AFX_MRU_COUNT);
    void AddToRecentFileList(LPCTSTR lpszPathName);
    CString GetRecentFileListEntry(int nIndex) const;
    int  GetRecentFileListCount() const;
    void Enable3dControls();        // no-op (kept for source compatibility)
    void Enable3dControlsStatic();  // no-op
    void SetCurrentHandles();
    int  GetExitCode() const;

    CStringArray m_recentFileList;  // MRU file list
    UINT m_nNumRecentFiles;

    afx_msg void OnAppExit();
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFilePrintSetup();
    afx_msg void OnAppAbout();
    afx_msg void OnUpdateFileMruFile1(CCmdUI* pCmdUI);
    afx_msg void OnFileMruFile(UINT nID);

    DECLARE_MESSAGE_MAP()
};


// ---------------------------------------------------------------------
//  Globals / helper functions
// ---------------------------------------------------------------------
CWinThread* AFXAPI AfxBeginThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam,
                                  int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0,
                                  DWORD dwCreateFlags = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);
CWinThread* AFXAPI AfxBeginThread(CRuntimeClass* pThreadClass, int nPriority = THREAD_PRIORITY_NORMAL,
                                  UINT nStackSize = 0, DWORD dwCreateFlags = 0,
                                  LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);
void AFXAPI AfxEndThread(int nReturnCode = 0, BOOL bDelete = TRUE);
void AFXAPI AfxInitThreadState(CWinThread* pThread);

// the real entry points (implemented in mfcapp.cpp)
int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

// global state setters used by the framework itself
void AFXAPI AfxSetInstanceHandles(HINSTANCE hInstance, HINSTANCE hPrevInstance);
void AFXAPI AfxSetThread(CWinThread* pThread);
void AFXAPI AfxSetMainWndHandle(HWND hWnd);
void AFXAPI AfxSetAppObject(CWinApp* pApp);

// Sleep helper
void AFXAPI AfxSleep(unsigned int nMillis);

// convenience: run a modal dialog as the app's main window
void AFXAPI AfxPostInitDialog(CWnd* pDlg);

#endif /* __AFXAPP_H__ */
