// =====================================================================
//  afxmt.h  --  MFC style synchronisation objects for the MFC-GCC clone.
//               Implemented with the Win32 primitives (the MinGW g++
//               "win32" thread model has no std::mutex/std::thread).
// =====================================================================
#ifndef __AFXMT_H__
#define __AFXMT_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif

#define AFX_SYNC_TIMEOUT_INFINITE   ((DWORD)-1)

// ---------------------------------------------------------------------
//  CSyncObject -- common base
// ---------------------------------------------------------------------
class CSyncObject : public CObject
{
    DECLARE_DYNAMIC(CSyncObject)
public:
    CSyncObject(LPCTSTR pszName);
    virtual ~CSyncObject();

    operator HANDLE() const { return m_hObject; }
    HANDLE GetSafeHandle() const { return m_hObject; }
    virtual BOOL Lock(DWORD dwTimeout = AFX_SYNC_TIMEOUT_INFINITE);
    virtual BOOL Unlock();
    CString GetObjectName() const { return m_strName; }

    HANDLE  m_hObject;
    CString m_strName;
};

// ---------------------------------------------------------------------
//  CCriticalSection
// ---------------------------------------------------------------------
class CCriticalSection : public CSyncObject
{
    DECLARE_DYNAMIC(CCriticalSection)
public:
    CCriticalSection();
    CCriticalSection(LPCTSTR pszName);
    virtual ~CCriticalSection();

    virtual BOOL Lock(DWORD dwTimeout = AFX_SYNC_TIMEOUT_INFINITE);
    virtual BOOL Lock();
    virtual BOOL Unlock();
    void Enter()  { ::EnterCriticalSection(&m_sect); }
    void Leave()  { ::LeaveCriticalSection(&m_sect); }
    BOOL TryEnter(){ return ::TryEnterCriticalSection(&m_sect) ? TRUE : FALSE; }
    operator CRITICAL_SECTION*() { return &m_sect; }
    LPCRITICAL_SECTION GetHandle() { return &m_sect; }

protected:
    CRITICAL_SECTION m_sect;
};

// ---------------------------------------------------------------------
//  CSemaphore
// ---------------------------------------------------------------------
class CSemaphore : public CSyncObject
{
    DECLARE_DYNAMIC(CSemaphore)
public:
    CSemaphore(LONG lInitialCount = 1, LONG lMaxCount = 1, LPCTSTR pszName = NULL,
               LPSECURITY_ATTRIBUTES lpsaAttributes = NULL);
    virtual ~CSemaphore() {}
    BOOL Unlock();
    BOOL Unlock(LONG lCount, LONG* lpPrevCount = NULL);
    virtual BOOL Lock(DWORD dwTimeout = AFX_SYNC_TIMEOUT_INFINITE);
};

// ---------------------------------------------------------------------
//  CMutex
// ---------------------------------------------------------------------
class CMutex : public CSyncObject
{
    DECLARE_DYNAMIC(CMutex)
public:
    CMutex(BOOL bInitiallyOwn = FALSE, LPCTSTR pszName = NULL,
           LPSECURITY_ATTRIBUTES lpsaAttributes = NULL);
    virtual ~CMutex() {}
    BOOL Unlock();
    virtual BOOL Lock(DWORD dwTimeout = AFX_SYNC_TIMEOUT_INFINITE);
};

// ---------------------------------------------------------------------
//  CEvent
// ---------------------------------------------------------------------
class CEvent : public CSyncObject
{
    DECLARE_DYNAMIC(CEvent)
public:
    CEvent(BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE, LPCTSTR pszName = NULL,
           LPSECURITY_ATTRIBUTES lpsaAttributes = NULL);
    virtual ~CEvent() {}
    BOOL SetEvent();
    BOOL PulseEvent();
    BOOL ResetEvent();
    BOOL Unlock();
    virtual BOOL Lock(DWORD dwTimeout = AFX_SYNC_TIMEOUT_INFINITE);
};

// ---------------------------------------------------------------------
//  CSingleLock / CMultiLock
// ---------------------------------------------------------------------
class CSingleLock
{
public:
    CSingleLock(CSyncObject* pObject, BOOL bInitialLock = FALSE);
    ~CSingleLock();
    BOOL Lock(DWORD dwTimeout = AFX_SYNC_TIMEOUT_INFINITE);
    BOOL Lock(LONGLONG dwTimeout);
    BOOL Unlock();
    BOOL IsLocked() const { return m_bAcquired ? TRUE : FALSE; }
    operator BOOL() const { return m_bAcquired ? TRUE : FALSE; }

protected:
    CSyncObject* m_pObject;
    BOOL         m_bAcquired;
};

class CMultiLock
{
public:
    enum { AFX_MAX_WAIT_OBJECTS = 64 };
    CMultiLock(CSyncObject* const* ppObjects, DWORD dwCount, BOOL bInitialLock = FALSE);
    ~CMultiLock();
    DWORD Lock(DWORD dwTimeout = AFX_SYNC_TIMEOUT_INFINITE, BOOL bWaitForAll = TRUE,
               BOOL bIgnoreAlerts = TRUE);
    DWORD Lock(LONGLONG dwTimeout, BOOL bWaitForAll = TRUE);
    BOOL Unlock();
    BOOL IsLocked(DWORD dwItem) const;
    operator BOOL() const { return m_bHaveLocked ? TRUE : FALSE; }

protected:
    CSyncObject* m_pObjects[AFX_MAX_WAIT_OBJECTS];
    BOOL         m_bLocked[AFX_MAX_WAIT_OBJECTS];
    DWORD        m_dwCount;
    BOOL         m_bHaveLocked;
};

#endif /* __AFXMT_H__ */
