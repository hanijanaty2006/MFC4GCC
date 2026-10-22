// =====================================================================
//  afx.h  --  Core of the "MFC for GCC" framework.
//
//  A source-compatible clone of the parts of the Microsoft Foundation
//  Classes that a normal Win32 application needs, implemented on top of
//  the raw Win32 API plus the C++11 standard library, so that it builds
//  with g++ (MinGW-w64) / GCC.  Nothing here needs MSVC, the MFC headers
//  or the MFC DLLs.
//
//  Usage:  #include <afxwin.h>
// =====================================================================
#ifndef __AFX_H__
#define __AFX_H__
#pragma once

#if !defined(WINVER)
#define WINVER 0x0601
#endif
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0601
#endif
#if !defined(_WIN32_IE)
#define _WIN32_IE 0x0800
#endif
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#if !defined(NTDDI_VERSION)
#define NTDDI_VERSION 0x06010000
#endif
#ifndef _COMMCTRL_
#define _COMMCTRL_H_INCLUDE_COMMON  // (harmless, keeps ordering stable)
#endif

#ifndef _WIN32
#error "This framework requires a Win32 target (build with x86_64-w64-mingw32-g++ or native MinGW g++)."
#endif

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <tchar.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cctype>
#include <cwctype>
#include <ctime>
#include <cstdint>
#include <new>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <utility>

// windowsx.h is deliberately NOT included: it defines macros (SubclassWindow,
// CopyRgn, ...) that collide with MFC member names.  Win32 macros that clash
// with MFC members are undone here.
#ifdef GetNextWindow
#undef GetNextWindow          // macro alias for GetWindow(); CWnd::GetNextWindow wins
#endif
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))
#endif

// MFC's magic default sizes / rectangles (defined in afxcore.cpp)

// ---------------------------------------------------------------------
//  Basic MFC-isms
// ---------------------------------------------------------------------
#define AFXAPI
#define AFX_CDECL
#define AFX_INLINE inline
#define AFX_DATADEF
#define AFX_COMDAT __declspec(selectany)

#ifndef PASCAL
#define PASCAL __stdcall
#endif
#ifndef CALLBACK
#define CALLBACK __stdcall
#endif

#ifndef afx_msg
#define afx_msg         // MFC marker for message-handler members
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// command routing codes understood by the message map machinery
#define CN_COMMAND              0
#define CN_UPDATE_COMMAND_UI    1
#define CN_OLECOMMAND           2
#define CN_OLEVERB              3

// MFC magic default sizes / rectangles (defined in afxcore.cpp)
extern const RECT AFXAPI rectDefault;
extern const SIZE AFXAPI sizeDefault;

// ---------------------------------------------------------------------
//  Character / string helper overloads (char + wchar_t)
// ---------------------------------------------------------------------
AFX_INLINE size_t AfxStrLen(const char* p)      { return strlen(p); }
AFX_INLINE size_t AfxStrLen(const wchar_t* p)   { return wcslen(p); }
AFX_INLINE int AfxStrCmp(const char* a, const char* b)        { return strcmp(a, b); }
AFX_INLINE int AfxStrCmp(const wchar_t* a, const wchar_t* b)  { return wcscmp(a, b); }
AFX_INLINE int AfxStrCmpI(const char* a, const char* b)       { return _stricmp(a, b); }
AFX_INLINE int AfxStrCmpI(const wchar_t* a, const wchar_t* b) { return _wcsicmp(a, b); }
AFX_INLINE int AfxStrCmpN(const char* a, const char* b, size_t n)        { return strncmp(a, b, n); }
AFX_INLINE int AfxStrCmpN(const wchar_t* a, const wchar_t* b, size_t n)  { return wcsncmp(a, b, n); }
AFX_INLINE const char* AfxStrChr(const char* p, int c)        { return strchr(p, c); }
AFX_INLINE const wchar_t* AfxStrChr(const wchar_t* p, wchar_t c) { return wcschr(p, c); }
AFX_INLINE const char* AfxStrRChr(const char* p, int c)       { return strrchr(p, c); }
AFX_INLINE const wchar_t* AfxStrRChr(const wchar_t* p, wchar_t c) { return wcsrchr(p, c); }
AFX_INLINE const char* AfxStrStr(const char* h, const char* n)         { return strstr(h, n); }
AFX_INLINE const wchar_t* AfxStrStr(const wchar_t* h, const wchar_t* n){ return wcsstr(h, n); }
AFX_INLINE size_t AfxStrSpn(const char* p, const char* s)     { return strspn(p, s); }
AFX_INLINE size_t AfxStrSpn(const wchar_t* p, const wchar_t* s){ return wcsspn(p, s); }
AFX_INLINE size_t AfxStrCSpn(const char* p, const char* s)    { return strcspn(p, s); }
AFX_INLINE size_t AfxStrCSpn(const wchar_t* p, const wchar_t* s){ return wcscspn(p, s); }
AFX_INLINE char AfxToUpper(char c)        { return (char)toupper((unsigned char)c); }
AFX_INLINE wchar_t AfxToUpper(wchar_t c)  { return (wchar_t)towupper(c); }
AFX_INLINE char AfxToLower(char c)        { return (char)tolower((unsigned char)c); }
AFX_INLINE wchar_t AfxToLower(wchar_t c)  { return (wchar_t)towlower(c); }
AFX_INLINE bool AfxIsSpace(char c)        { return isspace((unsigned char)c) != 0; }
AFX_INLINE bool AfxIsSpace(wchar_t c)     { return iswspace(c) != 0; }
AFX_INLINE int AfxStrColl(const char* a, const char* b)        { return strcoll(a, b); }
AFX_INLINE int AfxStrColl(const wchar_t* a, const wchar_t* b)  { return wcscoll(a, b); }
AFX_INLINE int AfxStrToInt(const char* p)      { return atoi(p); }
AFX_INLINE int AfxStrToInt(const wchar_t* p)   { return _wtoi(p); }
AFX_INLINE long AfxStrToLong(const char* p)    { return strtol(p, NULL, 10); }
AFX_INLINE long AfxStrToLong(const wchar_t* p) { return wcstol(p, NULL, 10); }
AFX_INLINE double AfxStrToDouble(const char* p)    { return atof(p); }
AFX_INLINE double AfxStrToDouble(const wchar_t* p) { return _wtof(p); }

// empty string literal for either character type
AFX_INLINE const char*    AfxEmptyStr(const char*)    { static const char e = 0; return &e; }
AFX_INLINE const wchar_t* AfxEmptyStr(const wchar_t*) { static const wchar_t e = L'\0'; return &e; }

// printf-style formatting primitives (ANSI + Unicode overloads)
AFX_INLINE int AfxSprintfLenV(const char* pszFormat, va_list args)
{ return vsnprintf(NULL, 0, pszFormat, args); }
AFX_INLINE int AfxSprintfLenV(const wchar_t* pszFormat, va_list args)
{ return _vscwprintf(pszFormat, args); }

AFX_INLINE int AfxSprintfV(char* pszBuf, size_t nMaxLen, const char* pszFormat, va_list args)
{
    if (nMaxLen == 0 || pszBuf == NULL) return -1;
    int n = vsnprintf(pszBuf, nMaxLen, pszFormat, args);
    pszBuf[nMaxLen - 1] = 0;
    return n;
}
AFX_INLINE int AfxSprintfV(wchar_t* pszBuf, size_t nMaxLen, const wchar_t* pszFormat, va_list args)
{
    if (nMaxLen == 0 || pszBuf == NULL) return -1;
    int n = _vsnwprintf(pszBuf, nMaxLen, pszFormat, args);
    if (n < 0 || (size_t)n >= nMaxLen) pszBuf[nMaxLen - 1] = 0;
    return n;
}

// narrow <-> wide conversions
std::wstring AFXAPI AfxMultiByteToWide(const char* psz, int nLen = -1, UINT cp = CP_ACP);
std::string  AFXAPI AfxWideToMultiByte(const wchar_t* psz, int nLen = -1, UINT cp = CP_ACP);

// safe string copy helper (no _s dependency)
template <class Ch>
AFX_INLINE void AfxStrCopy(Ch* pszDest, size_t nDestSize, const Ch* pszSrc)
{
    if (pszDest == NULL || nDestSize == 0) return;
    size_t i = 0;
    if (pszSrc != NULL) {
        for (; i + 1 < nDestSize && pszSrc[i] != 0; ++i) pszDest[i] = pszSrc[i];
    }
    pszDest[i] = 0;
}

// forward declarations
class CObject;
class CWnd;
class CCmdTarget;
class CWinThread;
class CWinApp;
class CFrameWnd;
class CDialog;
class CView;
class CDocument;
class CDocTemplate;
class CFile;
class CArchive;
class CException;
class CFileException;
class CUserException;
template <class Ch> class CStringT;
typedef CStringT<char>    CStringA;
typedef CStringT<wchar_t> CStringW;

// ---------------------------------------------------------------------
//  Global handle helpers
// ---------------------------------------------------------------------
HINSTANCE AFXAPI AfxGetInstanceHandle();
HINSTANCE AFXAPI AfxGetResourceHandle();
HINSTANCE AFXAPI AfxSetResourceHandle(HINSTANCE hInst);
HWND      AFXAPI AfxGetMainHwnd();
CWinApp*  AFXAPI AfxGetApp();
CWinThread* AFXAPI AfxGetThread();

// ---------------------------------------------------------------------
//  Diagnostics
// ---------------------------------------------------------------------
int AFXAPI AfxTrace(LPCSTR pszFile, int nLine, LPCTSTR pszFormat, ...);
void AFXAPI AfxAssertFailed(LPCSTR pszFile, int nLine, LPCSTR pszExpr);
void AFXAPI AfxAssertValidObject(const CObject* pOb, LPCSTR pszFile, int nLine);

// TRACE("value = %d\n", n);   -- printf style, like MFC
#define TRACE(...)      ::AfxTrace(__FILE__, __LINE__, __VA_ARGS__)

#ifdef _DEBUG
#define ASSERT(f)   do { if (!(f)) ::AfxAssertFailed(__FILE__, __LINE__, #f); } while (0)
#define ASSERT_VALID(pObj)  ::AfxAssertValidObject(pObj, __FILE__, __LINE__)
#define DEBUG_ONLY(f)   (f)
#else
#define ASSERT(f)           ((void)0)
#define ASSERT_VALID(pObj)  ((void)0)
#define DEBUG_ONLY(f)       ((void)0)
#endif

#ifdef _DEBUG
#define VERIFY(f)   ASSERT(f)
#else
#define VERIFY(f)   ((void)(f))
#endif

// ---------------------------------------------------------------------
//  CRuntimeClass  --  MFC style runtime type information
// ---------------------------------------------------------------------
struct CRuntimeClass
{
    LPCSTR                  m_lpszClassName;
    int                     m_nObjectSize;
    UINT                    m_wSchema;
    CObject* (PASCAL*       m_pfnCreateObject)();     // NULL => abstract
    CRuntimeClass*          m_pBaseClass;
    CRuntimeClass*          m_pNextClass;             // linked list of all classes

    CObject* CreateObject() { return (m_pfnCreateObject != NULL) ? (*m_pfnCreateObject)() : NULL; }
    BOOL IsDerivedFrom(const CRuntimeClass* pBaseClass) const;
    static CRuntimeClass* PASCAL FromName(LPCSTR lpszClassName);
    void AssertValid() const;
};

struct AFX_CLASSINIT { AFX_CLASSINIT(CRuntimeClass* pNewClass); };

#define RUNTIME_CLASS(class_name) ((CRuntimeClass*)(&class_name::class##class_name))

// NOTE: the runtime-class descriptors cannot be 'const' -- AFX_CLASSINIT links
// them into a list during static initialisation, and gcc/MinGW puts const
// namespace-scope objects in read-only memory (MSVC happens not to).
#define DECLARE_DYNAMIC(class_name)                                             \
public:                                                                         \
    static CRuntimeClass class##class_name;                                     \
    static CRuntimeClass* PASCAL GetThisClass();                                \
    virtual CRuntimeClass* GetRuntimeClass() const;

#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew)   \
    static char _lpsz_##class_name[] = #class_name;                             \
    CRuntimeClass class_name::class##class_name = {                             \
        _lpsz_##class_name, sizeof(class_name), wSchema, pfnNew,                \
        RUNTIME_CLASS(base_class_name), NULL };                                 \
    CRuntimeClass* PASCAL class_name::GetThisClass()                            \
        { return &class_name::class##class_name; }                              \
    CRuntimeClass* class_name::GetRuntimeClass() const                          \
        { return const_cast<CRuntimeClass*>(&class_name::class##class_name); }  \
    static const AFX_CLASSINIT _init_##class_name(                              \
        &class_name::class##class_name);

#define DECLARE_DYNCREATE(class_name) \
    DECLARE_DYNAMIC(class_name) \
    static CObject* PASCAL CreateObject();

#define DECLARE_SERIAL(class_name) \
    DECLARE_DYNCREATE(class_name) \
    virtual void Serialize(CArchive& ar); \
    friend CArchive& AFXAPI operator>>(CArchive& ar, class_name*& pOb);

#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
    _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, NULL)

#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
    CObject* PASCAL class_name::CreateObject() { return new class_name; } \
    _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, &class_name::CreateObject)

#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema) \
    CObject* PASCAL class_name::CreateObject() { return new class_name; } \
    _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, &class_name::CreateObject) \
    CArchive& AFXAPI operator>>(CArchive& ar, class_name*& pOb) \
        { pOb = (class_name*)ar.ReadObject(RUNTIME_CLASS(class_name)); return ar; }

// ---------------------------------------------------------------------
//  CObject  --  root of the class hierarchy
// ---------------------------------------------------------------------
struct AFX_MSGMAP;      // defined in afxmsg.h (needed by the root message map)

class CObject
{
public:
    // root of every message map chain (defined in afxwnd.cpp)
    static const AFX_MSGMAP messageMap;
    static const AFX_MSGMAP* PASCAL GetThisMessageMap();

    static CRuntimeClass classCObject;
    static CRuntimeClass* PASCAL GetThisClass() { return &CObject::classCObject; }

    virtual ~CObject() {}
    BOOL IsKindOf(const CRuntimeClass* pClass) const;
    virtual CRuntimeClass* GetRuntimeClass() const { return &CObject::classCObject; }
    virtual void Serialize(CArchive& ar);
#ifdef _DEBUG
    virtual void AssertValid() const {}
    virtual void Dump(void* pFile) const { (void)pFile; }
#endif
protected:
    CObject() {}
    CObject(const CObject&) {}
    CObject& operator=(const CObject&) { return *this; }
};

AFX_INLINE BOOL CObject::IsKindOf(const CRuntimeClass* pClass) const
{
    for (CRuntimeClass* p = GetRuntimeClass(); p != NULL; p = p->m_pBaseClass)
        if (p == pClass) return TRUE;
    return FALSE;
}

// ---------------------------------------------------------------------
//  CStringT / CStringA / CStringW / CString
//
//  Derived from std::basic_string so that it interoperates with the
//  C++11 standard library, with the classic MFC API layered on top.
// ---------------------------------------------------------------------
// character-width conversion helpers (defined after the class template,
// declared here so that two-phase lookup inside CStringT finds them)
AFX_INLINE std::basic_string<char>    AfxStrConv(const std::basic_string<wchar_t>& s);
AFX_INLINE std::basic_string<wchar_t> AfxStrConv(const std::basic_string<char>& s);

// argument normalisation for Format(): CString/std::string decay to a pointer.
// declared here (before the class template) so that two-phase lookup finds them.
template <class T> AFX_INLINE const T& AfxFmtConv(const T& v);
AFX_INLINE const char*    AfxFmtConv(const CStringT<char>& s);
AFX_INLINE const wchar_t* AfxFmtConv(const CStringT<wchar_t>& s);
AFX_INLINE const char*    AfxFmtConv(const std::string& s);
AFX_INLINE const wchar_t* AfxFmtConv(const std::wstring& s);
AFX_INLINE int            AfxFmtConv(bool b);

template <class Ch>
class CStringT : public std::basic_string<Ch>
{
public:
    typedef std::basic_string<Ch>   BaseType;
    typedef Ch*                     PXSTR;
    typedef const Ch*               PCXSTR;
    typedef CStringT<Ch>            ThisType;

    enum { AFX_FORMAT_STACK = 512 };

    // --- construction -------------------------------------------------
    CStringT() : BaseType() {}
    CStringT(const ThisType& s) : BaseType(s) {}
    CStringT(const BaseType& s) : BaseType(s) {}
    CStringT(PCXSTR psz) : BaseType(psz ? psz : EmptyLit()) {}
    CStringT(PCXSTR psz, int nLen) : BaseType(psz ? psz : EmptyLit(), nLen > 0 ? (size_t)nLen : 0) {}
    CStringT(Ch ch, int nRepeat = 1) : BaseType(nRepeat > 0 ? (size_t)nRepeat : 0, ch) {}
    explicit CStringT(UINT nID) { LoadString(nID); }
    // converting constructor from the other character width
    template <class Other,
              class = typename std::enable_if<!std::is_same<Other, Ch>::value>::type>
    CStringT(const CStringT<Other>& other) : BaseType(AfxStrConv(other)) {}
    // converting constructor from a raw string of the other character width
    template <class Other,
              class = typename std::enable_if<!std::is_same<Other, Ch>::value>::type>
    CStringT(const Other* psz)
        : BaseType(psz != NULL ? AfxStrConv(std::basic_string<Other>(psz))
                               : std::basic_string<Ch>()) {}

    // --- assignment ---------------------------------------------------
    ThisType& operator=(const ThisType& s) { BaseType::operator=(s); return *this; }
    ThisType& operator=(const BaseType& s) { BaseType::operator=(s); return *this; }
    ThisType& operator=(PCXSTR psz)        { BaseType::assign(psz ? psz : EmptyLit()); return *this; }
    ThisType& operator=(Ch ch)             { BaseType::assign(1, ch); return *this; }

    // --- MFC attribute accessors --------------------------------------
    int GetLength() const                 { return (int)BaseType::size(); }
    BOOL IsEmpty() const                  { return BaseType::empty() ? TRUE : FALSE; }
    void Empty()                          { BaseType::clear(); }
    Ch GetAt(int nIndex) const            { return BaseType::operator[]((size_t)nIndex); }
    Ch operator[](int nIndex) const       { return BaseType::operator[]((size_t)nIndex); }
    Ch& operator[](int nIndex)            { return BaseType::operator[]((size_t)nIndex); }
    void SetAt(int nIndex, Ch ch)         { BaseType::operator[]((size_t)nIndex) = ch; }
    operator PCXSTR() const               { return BaseType::c_str(); }
    PCXSTR GetString() const              { return BaseType::c_str(); }
    PXSTR GetBuffer()                     { return &BaseType::operator[](0); }
    PXSTR GetBuffer(int nMinBufLength);
    PXSTR GetBufferSetLength(int nNewLength);
    void ReleaseBuffer(int nNewLength = -1);
    void ReleaseBufferSetLength(int nNewLength) { ReleaseBuffer(nNewLength); }
    void FreeExtra()                      { BaseType::shrink_to_fit(); }
    int SetString(PCXSTR psz)             { BaseType::assign(psz ? psz : EmptyLit()); return GetLength(); }
    int SetString(PCXSTR psz, int nLen)   { BaseType::assign(psz ? psz : EmptyLit(), (size_t)nLen); return GetLength(); }
    void Truncate(int nNewLength)         { if (nNewLength < GetLength()) BaseType::resize((size_t)nNewLength); }

    static PCXSTR EmptyLit()              { return AfxEmptyStr((Ch*)NULL); }

    // --- string operations --------------------------------------------
    ThisType Mid(int nFirst) const             { return SafeSub(nFirst, GetLength()); }
    ThisType Mid(int nFirst, int nCount) const { return SafeSub(nFirst, nCount); }
    ThisType Left(int nCount) const            { return SafeSub(0, nCount); }
    ThisType Right(int nCount) const           { return SafeSub(GetLength() - nCount, nCount); }
    ThisType SpanIncluding(PCXSTR pszCharSet) const
        { return BaseType::substr(0, AfxStrSpn(BaseType::c_str(), pszCharSet ? pszCharSet : EmptyLit())); }
    ThisType SpanExcluding(PCXSTR pszCharSet) const
        { return BaseType::substr(0, AfxStrCSpn(BaseType::c_str(), pszCharSet ? pszCharSet : EmptyLit())); }

    int Find(Ch ch) const                     { return FindCh(ch, 0); }
    int Find(Ch ch, int nStart) const         { return FindCh(ch, nStart); }
    int Find(PCXSTR pszSub) const             { return FindStr(pszSub, 0); }
    int Find(PCXSTR pszSub, int nStart) const { return FindStr(pszSub, nStart); }
    int ReverseFind(Ch ch) const
        { size_t i = BaseType::rfind(ch); return (i == BaseType::npos) ? -1 : (int)i; }
    int Replace(PCXSTR pszOld, PCXSTR pszNew);
    int Remove(Ch ch);
    int Insert(int nIndex, Ch ch);
    int Insert(int nIndex, PCXSTR psz);
    int Delete(int nIndex, int nCount = 1);

    ThisType& MakeUpper();
    ThisType& MakeLower();
    ThisType& Trim();
    ThisType& TrimLeft();
    ThisType& TrimRight();
    ThisType& Trim(Ch chTarget);
    ThisType& TrimLeft(Ch chTarget);
    ThisType& TrimRight(Ch chTarget);

    int Compare(PCXSTR psz) const       { return AfxStrCmp(BaseType::c_str(), psz ? psz : EmptyLit()); }
    int CompareNoCase(PCXSTR psz) const { return AfxStrCmpI(BaseType::c_str(), psz ? psz : EmptyLit()); }
    int Collate(PCXSTR psz) const       { return AfxStrColl(BaseType::c_str(), psz ? psz : EmptyLit()); }

    ThisType Tokenize(PCXSTR pszTokens, int& iStart) const;

    // --- printf style formatting ---------------------------------------
    void FormatV(PCXSTR pszFormat, va_list args);
    template <class... Args> void Format(PCXSTR pszFormat, Args&&... args)
        { FormatRaw(pszFormat, AfxFmtConv(args)...); }
    template <class... Args> void AppendFormat(PCXSTR pszFormat, Args&&... args)
    {
        ThisType tmp;
        tmp.FormatRaw(pszFormat, AfxFmtConv(args)...);
        BaseType::append(tmp);
    }
    void Format(UINT nFormatID, ...);

    // --- resources ------------------------------------------------------
    BOOL LoadString(UINT nID);
    BOOL LoadString(HINSTANCE hInst, UINT nID);

protected:
    void FormatRaw(PCXSTR pszFormat, ...);

    ThisType SafeSub(int nFirst, int nCount) const
    {
        int nLen = GetLength();
        if (nFirst < 0) { nCount += nFirst; nFirst = 0; }
        if (nFirst >= nLen || nCount <= 0) return ThisType();
        if (nFirst + nCount > nLen) nCount = nLen - nFirst;
        return ThisType(BaseType::substr((size_t)nFirst, (size_t)nCount));
    }
    int FindCh(Ch ch, int nStart) const
    {
        if (nStart < 0) nStart = 0;
        if (nStart > GetLength()) return -1;
        size_t i = BaseType::find(ch, (size_t)nStart);
        return (i == BaseType::npos) ? -1 : (int)i;
    }
    int FindStr(PCXSTR pszSub, int nStart) const
    {
        if (nStart < 0) nStart = 0;
        if (pszSub == NULL || *pszSub == 0) return -1;
        size_t i = BaseType::find(pszSub, (size_t)nStart);
        return (i == BaseType::npos) ? -1 : (int)i;
    }
};

// character-width conversion helpers used by the converting constructor
AFX_INLINE std::basic_string<char>    AfxStrConv(const std::basic_string<wchar_t>& s) { return AfxWideToMultiByte(s.c_str()); }
AFX_INLINE std::basic_string<wchar_t> AfxStrConv(const std::basic_string<char>& s)    { return AfxMultiByteToWide(s.c_str()); }

// argument normalisation so that CString can be passed to %s in Format()
template <class T> AFX_INLINE const T& AfxFmtConv(const T& v) { return v; }
AFX_INLINE const char*    AfxFmtConv(const CStringT<char>& s)    { return s.c_str(); }
AFX_INLINE const wchar_t* AfxFmtConv(const CStringT<wchar_t>& s) { return s.c_str(); }
AFX_INLINE const char*    AfxFmtConv(const std::string& s)       { return s.c_str(); }
AFX_INLINE const wchar_t* AfxFmtConv(const std::wstring& s)      { return s.c_str(); }
AFX_INLINE int            AfxFmtConv(bool b)                     { return b ? 1 : 0; }

template <class Ch> typename CStringT<Ch>::PXSTR CStringT<Ch>::GetBuffer(int nMinBufLength)
{
    if ((int)BaseType::size() < nMinBufLength) BaseType::resize((size_t)nMinBufLength);
    return &BaseType::operator[](0);
}

template <class Ch> typename CStringT<Ch>::PXSTR CStringT<Ch>::GetBufferSetLength(int nNewLength)
{
    BaseType::resize(nNewLength > 0 ? (size_t)nNewLength : 0);
    return &BaseType::operator[](0);
}

template <class Ch> void CStringT<Ch>::ReleaseBuffer(int nNewLength)
{
    if (nNewLength < 0) nNewLength = (int)AfxStrLen(BaseType::c_str());
    BaseType::resize((size_t)nNewLength);
}

template <class Ch> int CStringT<Ch>::Replace(PCXSTR pszOld, PCXSTR pszNew)
{
    if (pszOld == NULL || *pszOld == 0) return 0;
    if (pszNew == NULL) pszNew = EmptyLit();
    size_t nOldLen = AfxStrLen(pszOld), nNewLen = AfxStrLen(pszNew);
    int nReplaced = 0;
    size_t pos = 0;
    while ((pos = BaseType::find(pszOld, pos)) != BaseType::npos) {
        BaseType::replace(pos, nOldLen, pszNew, nNewLen);
        pos += (nNewLen ? nNewLen : 1);
        ++nReplaced;
    }
    return nReplaced;
}

template <class Ch> int CStringT<Ch>::Remove(Ch ch)
{
    ThisType out;
    out.reserve(BaseType::size());
    int nRemoved = 0;
    for (size_t i = 0; i < BaseType::size(); ++i) {
        if (BaseType::operator[](i) == ch) ++nRemoved;
        else out.push_back(BaseType::operator[](i));
    }
    BaseType::swap(out);
    return nRemoved;
}

template <class Ch> int CStringT<Ch>::Insert(int nIndex, Ch ch)
{
    if (nIndex < 0) nIndex = 0;
    if (nIndex > GetLength()) nIndex = GetLength();
    BaseType::insert((size_t)nIndex, 1, ch);
    return GetLength();
}

template <class Ch> int CStringT<Ch>::Insert(int nIndex, PCXSTR psz)
{
    if (nIndex < 0) nIndex = 0;
    if (nIndex > GetLength()) nIndex = GetLength();
    BaseType::insert((size_t)nIndex, psz ? psz : EmptyLit());
    return GetLength();
}

template <class Ch> int CStringT<Ch>::Delete(int nIndex, int nCount)
{
    if (nIndex < 0 || nIndex >= GetLength()) return GetLength();
    if (nCount < 0) nCount = 0;
    BaseType::erase((size_t)nIndex, (size_t)nCount);
    return GetLength();
}

template <class Ch> typename CStringT<Ch>::ThisType& CStringT<Ch>::MakeUpper()
{
    for (size_t i = 0; i < BaseType::size(); ++i)
        BaseType::operator[](i) = AfxToUpper(BaseType::operator[](i));
    return *this;
}

template <class Ch> typename CStringT<Ch>::ThisType& CStringT<Ch>::MakeLower()
{
    for (size_t i = 0; i < BaseType::size(); ++i)
        BaseType::operator[](i) = AfxToLower(BaseType::operator[](i));
    return *this;
}

template <class Ch> typename CStringT<Ch>::ThisType& CStringT<Ch>::TrimLeft()
{
    size_t i = 0;
    while (i < BaseType::size() && AfxIsSpace(BaseType::operator[](i))) ++i;
    if (i) BaseType::erase(0, i);
    return *this;
}

template <class Ch> typename CStringT<Ch>::ThisType& CStringT<Ch>::TrimRight()
{
    size_t n = BaseType::size();
    while (n > 0 && AfxIsSpace(BaseType::operator[](n - 1))) --n;
    BaseType::resize(n);
    return *this;
}

template <class Ch> typename CStringT<Ch>::ThisType& CStringT<Ch>::Trim()
{ TrimRight(); TrimLeft(); return *this; }

template <class Ch> typename CStringT<Ch>::ThisType& CStringT<Ch>::TrimLeft(Ch chTarget)
{
    size_t i = 0;
    while (i < BaseType::size() && BaseType::operator[](i) == chTarget) ++i;
    if (i) BaseType::erase(0, i);
    return *this;
}

template <class Ch> typename CStringT<Ch>::ThisType& CStringT<Ch>::TrimRight(Ch chTarget)
{
    size_t n = BaseType::size();
    while (n > 0 && BaseType::operator[](n - 1) == chTarget) --n;
    BaseType::resize(n);
    return *this;
}

template <class Ch> typename CStringT<Ch>::ThisType& CStringT<Ch>::Trim(Ch chTarget)
{ TrimRight(chTarget); TrimLeft(chTarget); return *this; }

template <class Ch> typename CStringT<Ch>::ThisType CStringT<Ch>::Tokenize(PCXSTR pszTokens, int& iStart) const
{
    ThisType result;
    if (iStart < 0 || pszTokens == NULL) return result;
    int nLen = GetLength();
    while (iStart < nLen && AfxStrChr(pszTokens, BaseType::operator[]((size_t)iStart)) != NULL) ++iStart;
    if (iStart >= nLen) { iStart = -1; return result; }
    int nEnd = iStart;
    while (nEnd < nLen && AfxStrChr(pszTokens, BaseType::operator[](nEnd)) == NULL) ++nEnd;
    result = BaseType::substr((size_t)iStart, (size_t)(nEnd - iStart));
    iStart = (nEnd < nLen) ? nEnd + 1 : -1;
    return result;
}

template <class Ch> void CStringT<Ch>::FormatV(PCXSTR pszFormat, va_list args)
{
    Ch stackBuf[AFX_FORMAT_STACK];
    va_list argsCopy;
    va_copy(argsCopy, args);
    int nNeed = AfxSprintfLenV(pszFormat, argsCopy);
    va_end(argsCopy);
    if (nNeed < 0) { BaseType::clear(); return; }
    if (nNeed < AFX_FORMAT_STACK) {
        AfxSprintfV(stackBuf, (size_t)AFX_FORMAT_STACK, pszFormat, args);
        BaseType::assign(stackBuf);
    } else {
        std::vector<Ch> buf((size_t)nNeed + 1);
        AfxSprintfV(&buf[0], buf.size(), pszFormat, args);
        BaseType::assign(&buf[0], (size_t)nNeed);
    }
}

template <class Ch> void CStringT<Ch>::FormatRaw(PCXSTR pszFormat, ...)
{
    va_list args;
    va_start(args, pszFormat);
    FormatV(pszFormat, args);
    va_end(args);
}

template <class Ch> void CStringT<Ch>::Format(UINT nFormatID, ...)
{
    ThisType fmt;
    fmt.LoadString(nFormatID);
    va_list args;
    va_start(args, nFormatID);
    FormatV(fmt.c_str(), args);
    va_end(args);
}

// string-resource loading: overloaded helper avoids illegal in-template branches
AFX_INLINE BOOL AfxLoadStringImpl(HINSTANCE hInst, UINT nID, std::basic_string<char>& s)
{
    char buf[2048];
    int n = ::LoadStringA(hInst, nID, buf, (int)sizeof(buf));
    s.assign(buf, n > 0 ? (size_t)n : 0);
    return n > 0 ? TRUE : FALSE;
}
AFX_INLINE BOOL AfxLoadStringImpl(HINSTANCE hInst, UINT nID, std::basic_string<wchar_t>& s)
{
    wchar_t buf[2048];
    int n = ::LoadStringW(hInst, nID, buf, 2048);
    s.assign(buf, n > 0 ? (size_t)n : 0);
    return n > 0 ? TRUE : FALSE;
}

template <class Ch> BOOL CStringT<Ch>::LoadString(HINSTANCE hInst, UINT nID)
{ return AfxLoadStringImpl(hInst, nID, static_cast<BaseType&>(*this)); }

template <class Ch> BOOL CStringT<Ch>::LoadString(UINT nID)
{ return LoadString(AfxGetResourceHandle(), nID); }

// AfxExtractSubString -- MFC compatible
template <class Ch>
AFX_INLINE BOOL AfxExtractSubStringT(CStringT<Ch>& rString, const Ch* pszString,
                                     int iSubstring, Ch chSep)
{
    rString.Empty();
    if (pszString == NULL) return FALSE;
    int nLen = (int)AfxStrLen(pszString);
    int i = 0, j = 0, nCur = 0;
    while (j <= nLen) {
        if (j == nLen || pszString[j] == chSep) {
            if (nCur == iSubstring) { rString = CStringT<Ch>(pszString + i, j - i); return TRUE; }
            ++nCur;
            i = j + 1;
        }
        ++j;
    }
    return FALSE;
}

AFX_INLINE BOOL AfxExtractSubString(CStringA& rString, const char* pszString,
                                    int iSubstring, char chSep = '\n')
{ return AfxExtractSubStringT(rString, pszString, iSubstring, chSep); }
AFX_INLINE BOOL AfxExtractSubString(CStringW& rString, const wchar_t* pszString,
                                    int iSubstring, wchar_t chSep = L'\n')
{ return AfxExtractSubStringT(rString, pszString, iSubstring, chSep); }

#ifdef UNICODE
typedef CStringW CString;
#define AFX_LPCTSTR_OF(s)  (LPCWSTR)(s)
#else
typedef CStringA CString;
#define AFX_LPCTSTR_OF(s)  (LPCSTR)(s)
#endif

// ---------------------------------------------------------------------
//  Exceptions
// ---------------------------------------------------------------------
int AFXAPI AfxMessageBox(LPCTSTR lpszText, UINT nType = MB_OK, UINT nIDHelp = 0);
int AFXAPI AfxMessageBox(UINT nIDPrompt, UINT nType = MB_OK, UINT nIDHelp = 0);

class CException
{
public:
    virtual ~CException() {}
    virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL)
    {
        if (lpszError != NULL && nMaxError > 0) lpszError[0] = 0;
        if (pnHelpContext != NULL) *pnHelpContext = 0;
        return FALSE;
    }
    virtual int ReportError(UINT nType = MB_OK, DWORD dwMessageID = 0);
    void Delete() { delete this; }
};

class CUserException : public CException
{
public:
    CUserException() {}
};

class CMemoryException : public CException
{
public:
    CMemoryException() {}
};

class CSimpleException : public CException
{
public:
    CString m_strMessage;
    CSimpleException(LPCTSTR psz = NULL) : m_strMessage(psz ? psz : _T("")) {}
    virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL)
    {
        if (pnHelpContext != NULL) *pnHelpContext = 0;
        AfxStrCopy(lpszError, nMaxError, (LPCTSTR)m_strMessage);
        return TRUE;
    }
};

class CFileException : public CException
{
public:
    enum { none, genericException, fileNotFound, badPath, tooManyOpenFiles, accessDenied,
           invalidFile, removeCurrentDir, directoryFull, badSeek, hardIO, sharingViolation,
           lockViolation, diskFull, endOfFile };
    int     m_cause;
    LONG    m_lOsError;
    CString m_strFileName;

    CFileException(int cause = none, LONG lOsError = -1, LPCTSTR pszFileName = NULL)
        : m_cause(cause), m_lOsError(lOsError), m_strFileName(pszFileName ? pszFileName : _T("")) {}

    static int PASCAL OsErrorToException(LONG lOsError);
    static void PASCAL ThrowOsError(LONG lOsError, LPCTSTR pszFileName = NULL);
    virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL);
};

void AFXAPI AfxThrowUserException();
void AFXAPI AfxThrowMemoryException();
void AFXAPI AfxThrowFileException(int cause, LONG lOsError = -1, LPCTSTR pszFileName = NULL);
void AFXAPI AfxThrowSimpleException(LPCTSTR pszMessage);

// ---------------------------------------------------------------------
//  CTime / CTimeSpan
// ---------------------------------------------------------------------
class CTimeSpan
{
public:
    CTimeSpan() : m_timeSpan(0) {}
    CTimeSpan(time_t t) : m_timeSpan(t) {}
    CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs)
        : m_timeSpan(((time_t)lDays * 24 + nHours) * 3600 + (time_t)nMins * 60 + nSecs) {}
    time_t GetTotalDays() const    { return m_timeSpan / 86400; }
    time_t GetTotalHours() const   { return m_timeSpan / 3600; }
    time_t GetTotalMinutes() const { return m_timeSpan / 60; }
    time_t GetTotalSeconds() const { return m_timeSpan; }
    LONG GetDays() const           { return (LONG)(m_timeSpan / 86400); }
    LONG GetHours() const          { return (LONG)((m_timeSpan / 3600) % 24); }
    LONG GetMinutes() const        { return (LONG)((m_timeSpan / 60) % 60); }
    LONG GetSeconds() const        { return (LONG)(m_timeSpan % 60); }
    time_t GetTimeSpan() const     { return m_timeSpan; }
    CString Format(LPCTSTR pszFormat) const;
    CTimeSpan operator+(CTimeSpan o) const { return CTimeSpan(m_timeSpan + o.m_timeSpan); }
    CTimeSpan operator-(CTimeSpan o) const { return CTimeSpan(m_timeSpan - o.m_timeSpan); }
    CTimeSpan& operator+=(CTimeSpan o) { m_timeSpan += o.m_timeSpan; return *this; }
    CTimeSpan& operator-=(CTimeSpan o) { m_timeSpan -= o.m_timeSpan; return *this; }
    BOOL operator==(CTimeSpan o) const { return m_timeSpan == o.m_timeSpan; }
    BOOL operator!=(CTimeSpan o) const { return m_timeSpan != o.m_timeSpan; }
    BOOL operator<(CTimeSpan o) const  { return m_timeSpan < o.m_timeSpan; }
    BOOL operator>(CTimeSpan o) const  { return m_timeSpan > o.m_timeSpan; }
    BOOL operator<=(CTimeSpan o) const { return m_timeSpan <= o.m_timeSpan; }
    BOOL operator>=(CTimeSpan o) const { return m_timeSpan >= o.m_timeSpan; }
protected:
    time_t m_timeSpan;
};

class CTime
{
public:
    CTime() : m_time(0) {}
    CTime(time_t t) : m_time(t) {}
    CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST = -1);
    static CTime PASCAL GetCurrentTime() { return CTime(::time(NULL)); }
    time_t GetTime() const { return m_time; }
    int GetYear() const;   int GetMonth() const;  int GetDay() const;
    int GetHour() const;   int GetMinute() const; int GetSecond() const;
    int GetDayOfWeek() const;
    CString Format(LPCTSTR pszFormat) const;
    CString FormatGmt(LPCTSTR pszFormat) const;
    CString FormatDateTime(LPCTSTR pszDateFmt = NULL, LPCTSTR pszTimeFmt = NULL) const;
    struct tm* GetGmtTm(struct tm* ptm = NULL) const;
    struct tm* GetLocalTm(struct tm* ptm = NULL) const;
    CTime operator+(CTimeSpan s) const { return CTime(m_time + s.GetTimeSpan()); }
    CTime operator-(CTimeSpan s) const { return CTime(m_time - s.GetTimeSpan()); }
    CTimeSpan operator-(CTime t) const { return CTimeSpan(m_time - t.m_time); }
    CTime& operator+=(CTimeSpan s) { m_time += s.GetTimeSpan(); return *this; }
    CTime& operator-=(CTimeSpan s) { m_time -= s.GetTimeSpan(); return *this; }
    BOOL operator==(CTime t) const { return m_time == t.m_time; }
    BOOL operator!=(CTime t) const { return m_time != t.m_time; }
    BOOL operator<(CTime t) const  { return m_time < t.m_time; }
    BOOL operator>(CTime t) const  { return m_time > t.m_time; }
    BOOL operator<=(CTime t) const { return m_time <= t.m_time; }
    BOOL operator>=(CTime t) const { return m_time >= t.m_time; }
protected:
    time_t m_time;
};

// ---------------------------------------------------------------------
//  CFile / CStdioFile / CFileStatus
// ---------------------------------------------------------------------
struct CFileStatus
{
    CTime   m_ctime;
    CTime   m_mtime;
    CTime   m_atime;
    LONG    m_size;
    BYTE    m_attribute;
    BYTE    m_padding;
    CString m_szFullName;
    enum Attribute { normal = 0x00, readOnly = 0x01, hidden = 0x02, system = 0x04,
                     volume = 0x08, directory = 0x10, archive = 0x20 };
};

class CFile : public CObject
{
    DECLARE_DYNAMIC(CFile)
public:
    enum Attribute { normal = 0x00, readOnly = 0x01, hidden = 0x02, system = 0x04,
                     volume = 0x08, directory = 0x10, archive = 0x20 };
    enum OpenFlags {
        modeRead        = 0x0000,
        modeWrite       = 0x0001,
        modeReadWrite   = 0x0002,
        shareCompat     = 0x0000,
        shareExclusive  = 0x0010,
        shareDenyWrite  = 0x0020,
        shareDenyRead   = 0x0030,
        shareDenyNone   = 0x0040,
        modeNoTruncate  = 0x0100,
        modeCreate      = 0x1000,
        typeText        = 0x4000,
        typeBinary      = (int)0x8000,
        osNoBuffer      = 0x00100000
    };

    CFile();
    CFile(HANDLE hFile);
    CFile(LPCTSTR lpszFileName, UINT nOpenFlags);
    virtual ~CFile();

    virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL);
    virtual void Close();
    virtual void Abort();
    virtual void Flush();
    virtual UINT_PTR Read(void* lpBuf, UINT_PTR nCount);
    virtual void Write(const void* lpBuf, UINT_PTR nCount);
    virtual LONG Seek(LONG lOff, UINT nFrom);
    LONG SeekToBegin()   { return Seek(0, FILE_BEGIN); }
    LONG SeekToEnd()     { return Seek(0, FILE_END); }
    LONG GetPosition() const;
    void SetPosition(LONG pos) { Seek(pos, FILE_BEGIN); }
    DWORD GetLength() const;
    void SetLength(DWORD dwNewLen);
    virtual BOOL LockRange(DWORD dwPos, DWORD dwCount);
    virtual BOOL UnlockRange(DWORD dwPos, DWORD dwCount);

    HANDLE GetSafeHandle() const { return m_hFile; }
    void Attach(HANDLE hFile);
    HANDLE Detach();
    CString GetFileName() const { return m_strFileName; }
    CString GetFileTitle() const;
    CString GetFilePath() const;

    virtual BOOL GetStatus(CFileStatus& rStatus) const;
    virtual void SetStatus(const CFileStatus& status);
    static BOOL PASCAL GetStatus(LPCTSTR lpszFileName, CFileStatus& rStatus);
    static void PASCAL SetStatus(LPCTSTR lpszFileName, const CFileStatus& status);
    static void PASCAL Rename(LPCTSTR lpszOldName, LPCTSTR lpszNewName);
    static void PASCAL Remove(LPCTSTR lpszFileName);

    BOOL ReadString(CString& rString);
    void WriteString(LPCTSTR lpsz);

    HANDLE  m_hFile;
protected:
    CString m_strFileName;
    BOOL    m_bCloseOnDelete;
};

class CStdioFile : public CFile
{
public:
    CStdioFile();
    CStdioFile(FILE* pOpenStream);
    CStdioFile(LPCTSTR lpszFileName, UINT nOpenFlags);
    virtual ~CStdioFile();

    BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL);
    virtual LPTSTR ReadString(LPTSTR lpsz, UINT nMax);
    virtual BOOL ReadString(CString& rString);
    virtual void WriteString(LPCTSTR lpsz);
    FILE* m_pStream;
};

// ---------------------------------------------------------------------
//  CArchive -- simple MFC-style serialisation
// ---------------------------------------------------------------------
class CArchive
{
public:
    enum Mode { store = 0, load = 1, bNoFlushOnDelete = 2, bNoByteSwap = 4 };
    CArchive(CFile* pFile, UINT nMode);
    ~CArchive();

    BOOL IsLoading() const { return (m_nMode & load) != 0; }
    BOOL IsStoring() const { return (m_nMode & load) == 0; }
    CFile* GetFile() const { return m_pFile; }
    void Close();
    void Flush();

    CObject* ReadObject(const CRuntimeClass* pClass);
    void WriteObject(const CObject* pOb);

    CArchive& operator<<(BYTE by)    { Write(&by, 1); return *this; }
    CArchive& operator<<(WORD w)     { Write(&w, 2); return *this; }
    CArchive& operator<<(short s)    { Write(&s, 2); return *this; }
    CArchive& operator<<(LONG l)     { Write(&l, 4); return *this; }
    CArchive& operator<<(DWORD dw)   { Write(&dw, 4); return *this; }
    CArchive& operator<<(int i)      { Write(&i, (UINT)sizeof(int)); return *this; }
    CArchive& operator<<(UINT u)     { Write(&u, (UINT)sizeof(UINT)); return *this; }
    CArchive& operator<<(float f)    { Write(&f, 4); return *this; }
    CArchive& operator<<(double d)   { Write(&d, 8); return *this; }
    CArchive& operator<<(LONGLONG v) { Write(&v, 8); return *this; }
    CArchive& operator<<(const CStringA& str);
    CArchive& operator<<(const CStringW& str);
    CArchive& operator<<(const CObject* pOb) { WriteObject(pOb); return *this; }
    CArchive& operator<<(const CObject& ob)  { WriteObject(&ob); return *this; }
    CArchive& operator<<(POINT pt) { Write(&pt, (UINT)sizeof(pt)); return *this; }
    CArchive& operator<<(SIZE sz)  { Write(&sz, (UINT)sizeof(sz)); return *this; }
    CArchive& operator<<(RECT rc)  { Write(&rc, (UINT)sizeof(rc)); return *this; }

    CArchive& operator>>(BYTE& by)    { Read(&by, 1); return *this; }
    CArchive& operator>>(WORD& w)     { Read(&w, 2); return *this; }
    CArchive& operator>>(short& s)    { Read(&s, 2); return *this; }
    CArchive& operator>>(LONG& l)     { Read(&l, 4); return *this; }
    CArchive& operator>>(DWORD& dw)   { Read(&dw, 4); return *this; }
    CArchive& operator>>(int& i)      { Read(&i, (UINT)sizeof(int)); return *this; }
    CArchive& operator>>(UINT& u)     { Read(&u, (UINT)sizeof(UINT)); return *this; }
    CArchive& operator>>(float& f)    { Read(&f, 4); return *this; }
    CArchive& operator>>(double& d)   { Read(&d, 8); return *this; }
    CArchive& operator>>(LONGLONG& v) { Read(&v, 8); return *this; }
    CArchive& operator>>(CStringA& str);
    CArchive& operator>>(CStringW& str);
    CArchive& operator>>(CObject*& pOb) { pOb = ReadObject(NULL); return *this; }
    CArchive& operator>>(POINT& pt) { Read(&pt, (UINT)sizeof(pt)); return *this; }
    CArchive& operator>>(SIZE& sz)  { Read(&sz, (UINT)sizeof(sz)); return *this; }
    CArchive& operator>>(RECT& rc)  { Read(&rc, (UINT)sizeof(rc)); return *this; }

    UINT Read(void* lpBuf, UINT nMax);
    void Write(const void* lpBuf, UINT nCount);

    UINT m_nMode;
protected:
    CFile* m_pFile;
};


// ---------------------------------------------------------------------
//  CSize / CPoint / CRect  (MFC compatible, derived from the Win32 structs)
// ---------------------------------------------------------------------
class CSize : public tagSIZE
{
public:
    CSize() { cx = cy = 0; }
    CSize(int initCX, int initCY) { cx = initCX; cy = initCY; }
    CSize(SIZE initSize) { cx = initSize.cx; cy = initSize.cy; }
    CSize(POINT initPt)  { cx = initPt.x; cy = initPt.y; }
    CSize(DWORD dwSize)  { cx = (short)LOWORD(dwSize); cy = (short)HIWORD(dwSize); }

    BOOL operator==(SIZE sz) const { return (cx == sz.cx && cy == sz.cy); }
    BOOL operator!=(SIZE sz) const { return (cx != sz.cx || cy != sz.cy); }
    void operator=(SIZE sz)  { *(tagSIZE*)this = sz; }
    void operator=(const CSize& sz) { *(tagSIZE*)this = *(const tagSIZE*)&sz; }
    void operator+=(SIZE sz) { cx += sz.cx; cy += sz.cy; }
    void operator-=(SIZE sz) { cx -= sz.cx; cy -= sz.cy; }

    CSize operator+(SIZE sz) const { return CSize(cx + sz.cx, cy + sz.cy); }
    CSize operator-(SIZE sz) const { return CSize(cx - sz.cx, cy - sz.cy); }
    CSize operator-() const        { return CSize(-cx, -cy); }

    operator LPSIZE()       { return this; }
    operator LPSIZE() const { return (LPSIZE)this; }
};

class CPoint : public tagPOINT
{
public:
    CPoint() { x = y = 0; }
    CPoint(int initX, int initY) { x = initX; y = initY; }
    CPoint(POINT initPt) { *(tagPOINT*)this = initPt; }
    CPoint(SIZE initSize){ x = initSize.cx; y = initSize.cy; }
    CPoint(DWORD dwPoint){ x = (short)LOWORD(dwPoint); y = (short)HIWORD(dwPoint); }

    void Offset(int xOffset, int yOffset) { x += xOffset; y += yOffset; }
    void Offset(POINT point)  { x += point.x; y += point.y; }
    void Offset(SIZE size)    { x += size.cx; y += size.cy; }

    BOOL operator==(POINT point) const { return (x == point.x && y == point.y); }
    BOOL operator!=(POINT point) const { return !(x == point.x && y == point.y); }
    void operator=(POINT point) { *(tagPOINT*)this = point; }
    void operator=(const CPoint& point) { *(tagPOINT*)this = *(const tagPOINT*)&point; }
    void operator+=(SIZE size)  { x += size.cx; y += size.cy; }
    void operator-=(SIZE size)  { x -= size.cx; y -= size.cy; }
    void operator+=(POINT point){ x += point.x; y += point.y; }
    void operator-=(POINT point){ x -= point.x; y -= point.y; }

    CPoint operator+(SIZE size) const  { return CPoint(x + size.cx, y + size.cy); }
    CPoint operator+(POINT point) const{ return CPoint(x + point.x, y + point.y); }
    CPoint operator-(SIZE size) const  { return CPoint(x - size.cx, y - size.cy); }
    CPoint operator-() const           { return CPoint(-x, -y); }
    CSize operator-(POINT point) const { return CSize(x - point.x, y - point.y); }

    operator LPPOINT()       { return this; }
    operator LPPOINT() const { return (LPPOINT)this; }
};

// CPoint/CRect share the memory layout of POINT/RECT; this helper does the
// one and only type-pun in the framework, isolated behind a pragma.
AFX_INLINE CPoint& AfxRectPointRef(tagRECT* pRect, int nWhich)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
    CPoint* pPoints = reinterpret_cast<CPoint*>(pRect);
#pragma GCC diagnostic pop
    return pPoints[nWhich];
}

class CRect : public tagRECT
{
public:
    CRect() { left = top = right = bottom = 0; }
    CRect(int l, int t, int r, int b) { left = l; top = t; right = r; bottom = b; }
    CRect(const RECT& srcRect) { *(tagRECT*)this = srcRect; }
    CRect(LPCRECT lpSrcRect)   { *(tagRECT*)this = *lpSrcRect; }
    CRect(POINT point, SIZE size)
        { right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }
    CRect(POINT topLeft, POINT bottomRight)
        { left = topLeft.x; top = topLeft.y; right = bottomRight.x; bottom = bottomRight.y; }

    int Width() const  { return right - left; }
    int Height() const { return bottom - top; }
    CSize Size() const { return CSize(Width(), Height()); }
    CPoint& TopLeft()       { return AfxRectPointRef(this, 0); }
    CPoint& BottomRight()   { return AfxRectPointRef(this, 1); }
    const CPoint& TopLeft() const     { return AfxRectPointRef(const_cast<CRect*>(this), 0); }
    const CPoint& BottomRight() const { return AfxRectPointRef(const_cast<CRect*>(this), 1); }
    CPoint CenterPoint() const { return CPoint((left + right) / 2, (top + bottom) / 2); }

    BOOL IsRectEmpty() const  { return ::IsRectEmpty(this); }
    BOOL IsRectNull() const   { return (left == 0 && right == 0 && top == 0 && bottom == 0); }
    BOOL PtInRect(POINT point) const { return ::PtInRect(this, point); }

    void SetRect(int x1, int y1, int x2, int y2) { ::SetRect(this, x1, y1, x2, y2); }
    void SetRect(POINT topLeft, POINT bottomRight)
        { ::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y); }
    void SetRectEmpty() { ::SetRectEmpty(this); }
    void CopyRect(LPCRECT lpSrcRect) { ::CopyRect(this, lpSrcRect); }
    BOOL EqualRect(LPCRECT lpRect) const { return ::EqualRect(this, lpRect); }

    void InflateRect(int x, int y) { ::InflateRect(this, x, y); }
    void InflateRect(SIZE size)    { ::InflateRect(this, size.cx, size.cy); }
    void InflateRect(LPCRECT lpRect)
        { left -= lpRect->left; top -= lpRect->top; right += lpRect->right; bottom += lpRect->bottom; }
    void InflateRect(int l, int t, int r, int b)
        { left -= l; top -= t; right += r; bottom += b; }
    void DeflateRect(int x, int y) { ::InflateRect(this, -x, -y); }
    void DeflateRect(SIZE size)    { ::InflateRect(this, -size.cx, -size.cy); }
    void DeflateRect(LPCRECT lpRect)
        { left += lpRect->left; top += lpRect->top; right -= lpRect->right; bottom -= lpRect->bottom; }
    void DeflateRect(int l, int t, int r, int b)
        { left += l; top += t; right -= r; bottom -= b; }

    void OffsetRect(int x, int y) { ::OffsetRect(this, x, y); }
    void OffsetRect(SIZE size)    { ::OffsetRect(this, size.cx, size.cy); }
    void OffsetRect(POINT point)  { ::OffsetRect(this, point.x, point.y); }
    void NormalizeRect()
    {
        if (left > right)  { int t = left;  left = right; right = t; }
        if (top > bottom)  { int t = top;   top = bottom; bottom = t; }
    }

    BOOL IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2) { return ::IntersectRect(this, lpRect1, lpRect2); }
    BOOL UnionRect(LPCRECT lpRect1, LPCRECT lpRect2)     { return ::UnionRect(this, lpRect1, lpRect2); }
    BOOL SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2)
        { return ::SubtractRect(this, lpRectSrc1, lpRectSrc2); }

    void operator=(const RECT& srcRect) { ::CopyRect(this, &srcRect); }
    void operator=(const CRect& srcRect){ ::CopyRect(this, &srcRect); }
    BOOL operator==(const RECT& rect) const { return ::EqualRect(this, &rect); }
    BOOL operator!=(const RECT& rect) const { return !::EqualRect(this, &rect); }
    void operator+=(POINT point) { ::OffsetRect(this, point.x, point.y); }
    void operator+=(SIZE size)   { ::OffsetRect(this, size.cx, size.cy); }
    void operator+=(LPCRECT lpRect) { InflateRect(lpRect); }
    void operator-=(POINT point) { ::OffsetRect(this, -point.x, -point.y); }
    void operator-=(SIZE size)   { ::OffsetRect(this, -size.cx, -size.cy); }
    void operator-=(LPCRECT lpRect) { DeflateRect(lpRect); }
    void operator&=(const RECT& rect) { ::IntersectRect(this, this, &rect); }
    void operator|=(const RECT& rect) { ::UnionRect(this, this, &rect); }

    CRect operator+(POINT point) const { return CRect(left + point.x, top + point.y, right + point.x, bottom + point.y); }
    CRect operator-(POINT point) const { return CRect(left - point.x, top - point.y, right - point.x, bottom - point.y); }
    CRect operator+(LPCRECT lpRect) const { CRect r(*this); r.InflateRect(lpRect); return r; }
    CRect operator+(SIZE size) const { return CRect(left + size.cx, top + size.cy, right + size.cx, bottom + size.cy); }
    CRect operator-(SIZE size) const { return CRect(left - size.cx, top - size.cy, right - size.cx, bottom - size.cy); }
    CRect operator-(LPCRECT lpRect) const { CRect r(*this); r.DeflateRect(lpRect); return r; }
    CRect operator&(const RECT& rect) const { CRect r; ::IntersectRect(&r, this, &rect); return r; }
    CRect operator|(const RECT& rect) const { CRect r; ::UnionRect(&r, this, &rect); return r; }

    operator LPRECT()       { return this; }
    operator LPCRECT() const{ return this; }
};

#include "afxtempl.h"
#include "afxmt.h"

#endif /* __AFX_H__ */
