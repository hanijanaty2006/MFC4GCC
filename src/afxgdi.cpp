// =====================================================================
//  afxgdi.cpp -- GDI wrapper implementation (CGdiObject, CPen, CBrush,
//                CFont, CBitmap, CRgn, CDC and friends)
// =====================================================================
#include <afxwin.h>
#include <afxcmn.h>
#include <afxext.h>
#include <map>
#include <vector>

// ---------------------------------------------------------------------
//  GDI handle <-> wrapper registry
// ---------------------------------------------------------------------
static std::map<HGDIOBJ, CGdiObject*> s_gdiMap;
static CRITICAL_SECTION s_gdiLock;
static bool s_gdiLockInit = false;

static void AfxGdiLock()
{
    if (!s_gdiLockInit) { ::InitializeCriticalSection(&s_gdiLock); s_gdiLockInit = true; }
    ::EnterCriticalSection(&s_gdiLock);
}
static void AfxGdiUnlock() { if (s_gdiLockInit) ::LeaveCriticalSection(&s_gdiLock); }

void AFXAPI AfxRegisterGdi(HGDIOBJ hObject, CGdiObject* pObject)
{
    if (hObject == NULL) return;
    AfxGdiLock();
    s_gdiMap[hObject] = pObject;
    AfxGdiUnlock();
}

void AFXAPI AfxUnregisterGdi(HGDIOBJ hObject)
{
    if (hObject == NULL) return;
    AfxGdiLock();
    s_gdiMap.erase(hObject);
    AfxGdiUnlock();
}

CGdiObject* AFXAPI AfxLookupGdi(HGDIOBJ hObject)
{
    if (hObject == NULL) return NULL;
    CGdiObject* p = NULL;
    AfxGdiLock();
    std::map<HGDIOBJ, CGdiObject*>::iterator it = s_gdiMap.find(hObject);
    if (it != s_gdiMap.end()) p = it->second;
    AfxGdiUnlock();
    return p;
}

// ---------------------------------------------------------------------
//  CGdiObject
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CGdiObject, CObject)

CGdiObject::~CGdiObject()
{
    if (m_hObject != NULL) DeleteObject();
}

BOOL CGdiObject::Attach(HGDIOBJ hObject)
{
    if (m_hObject != NULL) DeleteObject();
    m_hObject = hObject;
    if (m_hObject != NULL) AfxRegisterGdi(m_hObject, this);
    return m_hObject != NULL;
}

HGDIOBJ CGdiObject::Detach()
{
    HGDIOBJ h = m_hObject;
    if (h != NULL) AfxUnregisterGdi(h);
    m_hObject = NULL;
    return h;
}

BOOL CGdiObject::DeleteObject()
{
    if (m_hObject == NULL) return FALSE;
    AfxUnregisterGdi(m_hObject);
    BOOL b = TRUE;
    if (!m_bStock) b = ::DeleteObject(m_hObject) ? TRUE : FALSE;
    m_hObject = NULL;
    return b;
}

int CGdiObject::GetObject(int nCount, LPVOID lpObject) const
{
    if (m_hObject == NULL) return 0;
    return ::GetObject(m_hObject, nCount, lpObject);
}

UINT CGdiObject::GetObjectType() const
{
    if (m_hObject == NULL) return 0;
    return ::GetObjectType(m_hObject);
}

BOOL CGdiObject::CreateStockObject(int nIndex)
{
    BOOL b = Attach(::GetStockObject(nIndex));
    m_bStock = TRUE;
    return b;
}

BOOL CGdiObject::UnrealizeObject()
{
    if (m_hObject == NULL) return FALSE;
    return ::UnrealizeObject(m_hObject);
}

// ---------------------------------------------------------------------
//  CPen
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CPen, CGdiObject)

CPen::CPen() {}
CPen::CPen(int nPenStyle, int nWidth, COLORREF crColor) { CreatePen(nPenStyle, nWidth, crColor); }
CPen::CPen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush, int nStyleCount, const DWORD* lpStyle)
{ CreatePen(nPenStyle, nWidth, pLogBrush, nStyleCount, lpStyle); }
CPen::~CPen() {}

BOOL CPen::CreatePen(int nPenStyle, int nWidth, COLORREF crColor)
{
    return Attach(::CreatePen(nPenStyle, nWidth, crColor));
}

BOOL CPen::CreatePen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush, int nStyleCount, const DWORD* lpStyle)
{
    return Attach(::ExtCreatePen((DWORD)nPenStyle, (DWORD)nWidth, pLogBrush, (DWORD)nStyleCount, lpStyle));
}

BOOL CPen::CreatePenIndirect(LPLOGPEN lpLogPen)
{
    return Attach(::CreatePenIndirect(lpLogPen));
}

int CPen::GetLogPen(LOGPEN* pLogPen)
{
    return GetObject(sizeof(LOGPEN), pLogPen);
}

// ---------------------------------------------------------------------
//  CBrush
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CBrush, CGdiObject)

CBrush::CBrush() {}
CBrush::CBrush(COLORREF crColor) { CreateSolidBrush(crColor); }
CBrush::CBrush(int nIndex, COLORREF crColor) { CreateHatchBrush(nIndex, crColor); }
CBrush::CBrush(CBitmap* pBitmap) { CreatePatternBrush(pBitmap); }
CBrush::~CBrush() {}

BOOL CBrush::CreateSolidBrush(COLORREF crColor) { return Attach(::CreateSolidBrush(crColor)); }
BOOL CBrush::CreateHatchBrush(int nIndex, COLORREF crColor) { return Attach(::CreateHatchBrush(nIndex, crColor)); }
BOOL CBrush::CreatePatternBrush(CBitmap* pBitmap) { return Attach(::CreatePatternBrush((HBITMAP)pBitmap->GetSafeHandle())); }
BOOL CBrush::CreateBrushIndirect(const LOGBRUSH* lpLogBrush) { return Attach(::CreateBrushIndirect(lpLogBrush)); }

BOOL CBrush::CreateDIBPatternBrush(HGLOBAL hPackedDIB, UINT nUsage)
{
    LPVOID lp = ::GlobalLock(hPackedDIB);
    if (lp == NULL) return FALSE;
    HBRUSH h = ::CreateDIBPatternBrushPt(lp, nUsage);
    ::GlobalUnlock(hPackedDIB);
    return Attach(h);
}

BOOL CBrush::CreateSysColorBrush(int nIndex)
{
    // system brushes are owned by Windows: never delete them
    if (m_hObject != NULL) DeleteObject();
    m_hObject = ::GetSysColorBrush(nIndex);
    m_bStock = TRUE;
    if (m_hObject != NULL) AfxRegisterGdi(m_hObject, this);
    return m_hObject != NULL;
}

int CBrush::GetLogBrush(LOGBRUSH* pLogBrush) { return GetObject(sizeof(LOGBRUSH), pLogBrush); }

// ---------------------------------------------------------------------
//  CFont
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CFont, CGdiObject)

CFont::CFont() {}
CFont::~CFont() {}

BOOL CFont::CreateFontIndirect(const LOGFONT* lpLogFont) { return Attach(::CreateFontIndirect(lpLogFont)); }

BOOL CFont::CreateFont(int nHeight, int nWidth, int nEscapement, int nOrientation, int nWeight,
                       BYTE bItalic, BYTE bUnderline, BYTE cStrikeOut, BYTE nCharSet,
                       BYTE nOutPrecision, BYTE nClipPrecision, BYTE nQuality,
                       BYTE nPitchAndFamily, LPCTSTR lpszFacename)
{
    return Attach(::CreateFont(nHeight, nWidth, nEscapement, nOrientation, nWeight,
                               bItalic, bUnderline, cStrikeOut, nCharSet, nOutPrecision,
                               nClipPrecision, nQuality, nPitchAndFamily, lpszFacename));
}

BOOL CFont::CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, CDC* pDC)
{
    LOGFONT logFont;
    memset(&logFont, 0, sizeof(logFont));
    logFont.lfCharSet = DEFAULT_CHARSET;
    logFont.lfHeight = nPointSize;                 // converted below
    logFont.lfWeight = FW_NORMAL;
    logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    if (lpszFaceName != NULL) AfxStrCopy(logFont.lfFaceName, LF_FACESIZE, lpszFaceName);
    return CreatePointFontIndirect(&logFont, pDC);
}

BOOL CFont::CreatePointFontIndirect(const LOGFONT* lpLogFont, CDC* pDC)
{
    LOGFONT lf = *lpLogFont;
    // lfHeight is in tenths of a point: convert to device units
    HDC hDC = (pDC != NULL) ? pDC->GetSafeHdc() : ::GetDC(NULL);
    int nHeight = -::MulDiv(lf.lfHeight, ::GetDeviceCaps(hDC, LOGPIXELSY), 720);
    if (pDC == NULL) ::ReleaseDC(NULL, hDC);
    lf.lfHeight = nHeight;
    return CreateFontIndirect(&lf);
}

int CFont::GetLogFont(LOGFONT* pLogFont) { return GetObject(sizeof(LOGFONT), pLogFont); }

// ---------------------------------------------------------------------
//  CBitmap
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CBitmap, CGdiObject)

CBitmap::CBitmap() {}
CBitmap::~CBitmap() {}

BOOL CBitmap::LoadBitmap(LPCTSTR lpszResourceName)
{
    return Attach(::LoadBitmap(AfxGetResourceHandle(), lpszResourceName));
}
BOOL CBitmap::LoadBitmap(UINT nIDResource)
{
    return Attach(::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(nIDResource)));
}
BOOL CBitmap::LoadOEMBitmap(UINT nIDBitmap)
{
    return Attach(::LoadBitmap(NULL, MAKEINTRESOURCE(nIDBitmap)));
}
BOOL CBitmap::LoadMappedBitmap(UINT nIdBitmap, UINT nFlags, LPCOLORMAP lpColorMap, int nNumColors)
{
    return Attach(::CreateMappedBitmap(AfxGetResourceHandle(), (int)nIdBitmap, (UINT)nFlags,
                                       lpColorMap, nNumColors));
}
BOOL CBitmap::CreateCompatibleBitmap(CDC* pDC, int nWidth, int nHeight)
{
    return Attach(::CreateCompatibleBitmap(pDC->GetSafeHdc(), nWidth, nHeight));
}
BOOL CBitmap::CreateDiscardableBitmap(CDC* pDC, int nWidth, int nHeight)
{
    return Attach(::CreateDiscardableBitmap(pDC->GetSafeHdc(), nWidth, nHeight));
}
BOOL CBitmap::CreateBitmap(int nWidth, int nHeight, UINT nPlanes, UINT nBitcount, const void* lpBits)
{
    return Attach(::CreateBitmap(nWidth, nHeight, nPlanes, nBitcount, lpBits));
}
BOOL CBitmap::CreateBitmapIndirect(LPBITMAP lpBitmap) { return Attach(::CreateBitmapIndirect(lpBitmap)); }

HBITMAP CBitmap::CreateDIBitmap(CDC* pDC, LPBITMAPINFOHEADER lpbmih, DWORD fdwInit,
                                LPCVOID lpbInit, LPBITMAPINFO lpbmi, UINT fuUsage)
{
    HBITMAP h = ::CreateDIBitmap(pDC->GetSafeHdc(), lpbmih, fdwInit, lpbInit, lpbmi, fuUsage);
    if (h != NULL) Attach(h);
    return h;
}

BOOL CBitmap::CreateDIBSection(CDC* pDC, const BITMAPINFO* pbmi, UINT iUsage,
                               void** ppvBits, HANDLE hSection, DWORD dwOffset)
{
    return Attach(::CreateDIBSection(pDC ? pDC->GetSafeHdc() : NULL, pbmi, iUsage,
                                     ppvBits, hSection, dwOffset));
}

int CBitmap::GetBitmap(BITMAP* pBitMap) { return GetObject(sizeof(BITMAP), pBitMap); }

// ---------------------------------------------------------------------
//  CRgn
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CRgn, CGdiObject)

CRgn::CRgn() {}
CRgn::~CRgn() {}

BOOL CRgn::CreateRectRgn(int x1, int y1, int x2, int y2) { return Attach(::CreateRectRgn(x1, y1, x2, y2)); }
BOOL CRgn::CreateRectRgnIndirect(LPCRECT lpRect) { return Attach(::CreateRectRgnIndirect(lpRect)); }
BOOL CRgn::CreateEllipticRgn(int x1, int y1, int x2, int y2) { return Attach(::CreateEllipticRgn(x1, y1, x2, y2)); }
BOOL CRgn::CreateEllipticRgnIndirect(LPCRECT lpRect) { return Attach(::CreateEllipticRgnIndirect(lpRect)); }
BOOL CRgn::CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode) { return Attach(::CreatePolygonRgn(lpPoints, nCount, nMode)); }
BOOL CRgn::CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyMode)
{ return Attach(::CreatePolyPolygonRgn(lpPoints, lpPolyCounts, nCount, nPolyMode)); }
BOOL CRgn::CreateRoundRectRgn(int x1, int y1, int x2, int y2, int x3, int y3) { return Attach(::CreateRoundRectRgn(x1, y1, x2, y2, x3, y3)); }
BOOL CRgn::CreateFromPath(CDC* pDC) { return Attach(::PathToRegion(pDC->GetSafeHdc())); }
BOOL CRgn::CreateFromData(const XFORM* lpXForm, HRGN hRgn)
{
    DWORD dwSize = ::GetRegionData(hRgn, 0, NULL);
    if (dwSize == 0) return FALSE;
    std::vector<BYTE> buf(dwSize);
    if (::GetRegionData(hRgn, dwSize, (LPRGNDATA)&buf[0]) == 0) return FALSE;
    return Attach(::ExtCreateRegion(lpXForm, dwSize, (LPRGNDATA)&buf[0]));
}

int CRgn::CombineRgn(CRgn* pRgn1, CRgn* pRgn2, int nCombineMode)
{ return ::CombineRgn((HRGN)m_hObject, (HRGN)pRgn1->GetSafeHandle(), (HRGN)pRgn2->GetSafeHandle(), nCombineMode); }
int CRgn::CombineRgn(CRgn* pRgn1, CRgn* pRgn2, int nCombineMode) const
{ return ::CombineRgn((HRGN)m_hObject, (HRGN)pRgn1->GetSafeHandle(), (HRGN)pRgn2->GetSafeHandle(), nCombineMode); }
int CRgn::CopyRgn(CRgn* pRgnSrc) { return ::CombineRgn((HRGN)m_hObject, (HRGN)pRgnSrc->GetSafeHandle(), NULL, RGN_COPY); }
int CRgn::EqualRgn(CRgn* pRgn) const { return ::EqualRgn((HRGN)m_hObject, (HRGN)pRgn->GetSafeHandle()); }
int CRgn::OffsetRgn(POINT point) { return ::OffsetRgn((HRGN)m_hObject, point.x, point.y); }
int CRgn::OffsetRgn(int x, int y) { return ::OffsetRgn((HRGN)m_hObject, x, y); }
int CRgn::GetRgnBox(LPRECT lpRect) const { return ::GetRgnBox((HRGN)m_hObject, lpRect); }
BOOL CRgn::PtInRegion(int x, int y) const { return ::PtInRegion((HRGN)m_hObject, x, y); }
BOOL CRgn::PtInRegion(POINT point) const { return ::PtInRegion((HRGN)m_hObject, point.x, point.y); }
BOOL CRgn::RectInRegion(LPCRECT lpRect) const { return ::RectInRegion((HRGN)m_hObject, lpRect); }
int CRgn::GetRegionData(LPRGNDATA lpRgnData, int nDataSize) const
{ return ::GetRegionData((HRGN)m_hObject, (DWORD)nDataSize, lpRgnData); }

// ---------------------------------------------------------------------
//  CDC
// ---------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CDC, CObject)

// DC wrappers: permanent map (CDC objects that called Attach()) plus a
// temporary map, exactly like MFC's FromHandle()/DeleteTempMap() pair.
static std::map<HDC, CDC*> s_permDCs;
static std::map<HDC, CDC*> s_tempDCs;

CDC* CDC::FromHandle(HDC hDC)
{
    if (hDC == NULL) return NULL;
    std::map<HDC, CDC*>::iterator ip = s_permDCs.find(hDC);
    if (ip != s_permDCs.end()) return ip->second;
    std::map<HDC, CDC*>::iterator it = s_tempDCs.find(hDC);
    if (it != s_tempDCs.end()) return it->second;
    CDC* pDC = new CDC();
    pDC->m_hDC = hDC;
    pDC->m_hAttribDC = hDC;
    s_tempDCs[hDC] = pDC;
    return pDC;
}

void CDC::DeleteTempMap()
{
    for (std::map<HDC, CDC*>::iterator it = s_tempDCs.begin(); it != s_tempDCs.end(); ++it) {
        it->second->m_hDC = NULL;
        it->second->m_hAttribDC = NULL;
        delete it->second;
    }
    s_tempDCs.clear();
}

CDC::CDC() : m_hDC(NULL), m_hAttribDC(NULL), m_bPrinting(FALSE) {}

CDC::~CDC()
{
    if (m_hDC != NULL) {
        std::map<HDC, CDC*>::iterator it = s_tempDCs.find(m_hDC);
        if (it != s_tempDCs.end()) s_tempDCs.erase(it);
    }
    DeleteDC();
}

BOOL CDC::Attach(HDC hDC)
{
    if (hDC == NULL) return FALSE;
    if (m_hDC != NULL) Detach();
    m_hDC = hDC;
    if (m_hAttribDC == NULL) m_hAttribDC = hDC;
    s_permDCs[hDC] = this;
    return TRUE;
}

HDC CDC::Detach()
{
    HDC h = m_hDC;
    if (h != NULL) s_permDCs.erase(h);
    m_hDC = NULL;
    m_hAttribDC = NULL;
    return h;
}

CWnd* CDC::GetWindow() const { return CWnd::FromHandle(::WindowFromDC(m_hAttribDC)); }

BOOL CDC::CreateDC(LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName, LPCTSTR lpszOutput, const void* lpInitData)
{
    return Attach(::CreateDC(lpszDriverName, lpszDeviceName, lpszOutput, (const DEVMODE*)lpInitData));
}

BOOL CDC::CreateIC(LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName, LPCTSTR lpszOutput, const void* lpInitData)
{
    return Attach(::CreateIC(lpszDriverName, lpszDeviceName, lpszOutput, (const DEVMODE*)lpInitData));
}

BOOL CDC::CreateCompatibleDC(CDC* pDC)
{
    return Attach(::CreateCompatibleDC(pDC != NULL ? pDC->GetSafeHdc() : NULL));
}

BOOL CDC::DeleteDC()
{
    if (m_hDC == NULL) return FALSE;
    BOOL b = ::DeleteDC(m_hDC) ? TRUE : FALSE;
    m_hDC = NULL;
    m_hAttribDC = NULL;
    return b;
}

int CDC::GetDeviceCaps(int nIndex) const { return ::GetDeviceCaps(GetAttribDC(), nIndex); }

CSize CDC::GetTextExtent(LPCTSTR lpszString, int nCount) const
{
    SIZE sz = { 0, 0 };
    if (nCount < 0) nCount = (int)AfxStrLen(lpszString);
    ::GetTextExtentPoint32(GetAttribDC(), lpszString, nCount, &sz);
    return CSize(sz);
}

CSize CDC::GetTextExtent(const CString& str) const { return GetTextExtent((LPCTSTR)str, str.GetLength()); }

CSize CDC::GetOutputTextExtent(LPCTSTR lpszString, int nCount) const
{
    SIZE sz = { 0, 0 };
    if (nCount < 0) nCount = (int)AfxStrLen(lpszString);
    ::GetTextExtentPoint32(m_hDC, lpszString, nCount, &sz);
    return CSize(sz);
}

CSize CDC::GetTabbedTextExtent(LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions) const
{
    DWORD dw = ::GetTabbedTextExtent(GetAttribDC(), lpszString, nCount, nTabPositions, lpnTabStopPositions);
    return CSize(dw);
}

CSize CDC::GetOutputTabbedTextExtent(LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions) const
{
    DWORD dw = ::GetTabbedTextExtent(m_hDC, lpszString, nCount, nTabPositions, lpnTabStopPositions);
    return CSize(dw);
}

CString CDC::GetTextFace() const
{
    TCHAR buf[LF_FACESIZE];
    int n = ::GetTextFace(GetAttribDC(), LF_FACESIZE, buf);
    return (n > 0) ? CString(buf) : CString();
}

int CDC::GetTextFace(int nCount, LPTSTR lpszData) const { return ::GetTextFace(GetAttribDC(), nCount, lpszData); }
BOOL CDC::GetTextMetrics(LPTEXTMETRIC lpMetrics) const { return ::GetTextMetrics(GetAttribDC(), lpMetrics); }
BOOL CDC::GetOutputTextMetrics(LPTEXTMETRIC lpMetrics) const { return ::GetTextMetrics(m_hDC, lpMetrics); }
int CDC::GetTextAlign() const { return ::GetTextAlign(GetAttribDC()); }
int CDC::SetTextAlign(UINT nFlags) { return ::SetTextAlign(m_hDC, nFlags); }
int CDC::GetTextCharacterExtra() const { return ::GetTextCharacterExtra(GetAttribDC()); }
int CDC::SetTextCharacterExtra(int nCharExtra) { return ::SetTextCharacterExtra(m_hDC, nCharExtra); }
int CDC::GetMapMode() const { return ::GetMapMode(GetAttribDC()); }
int CDC::SetMapMode(int nMapMode) { return ::SetMapMode(m_hDC, nMapMode); }
int CDC::GetGraphicsMode() const { return ::GetGraphicsMode(GetAttribDC()); }
int CDC::SetGraphicsMode(int iMode) { return ::SetGraphicsMode(m_hDC, iMode); }

COLORREF CDC::GetTextColor() const { return ::GetTextColor(GetAttribDC()); }
COLORREF CDC::SetTextColor(COLORREF crColor) { return ::SetTextColor(m_hDC, crColor); }
COLORREF CDC::GetBkColor() const { return ::GetBkColor(GetAttribDC()); }
COLORREF CDC::SetBkColor(COLORREF crColor) { return ::SetBkColor(m_hDC, crColor); }
int CDC::GetBkMode() const { return ::GetBkMode(GetAttribDC()); }
int CDC::SetBkMode(int nBkMode) { return ::SetBkMode(m_hDC, nBkMode); }
int CDC::GetPolyFillMode() const { return ::GetPolyFillMode(GetAttribDC()); }
int CDC::SetPolyFillMode(int nPolyFillMode) { return ::SetPolyFillMode(m_hDC, nPolyFillMode); }
int CDC::GetROP2() const { return ::GetROP2(GetAttribDC()); }
int CDC::SetROP2(int nDrawMode) { return ::SetROP2(m_hDC, nDrawMode); }
int CDC::GetStretchBltMode() const { return ::GetStretchBltMode(GetAttribDC()); }
int CDC::SetStretchBltMode(int nStretchMode) { return ::SetStretchBltMode(m_hDC, nStretchMode); }
CPoint CDC::GetBrushOrg() const { POINT pt; ::GetBrushOrgEx(GetAttribDC(), &pt); return CPoint(pt); }
CPoint CDC::SetBrushOrg(int nXOrg, int nYOrg) { POINT pt; ::SetBrushOrgEx(m_hDC, nXOrg, nYOrg, &pt); return CPoint(pt); }
CPoint CDC::SetBrushOrg(POINT point) { POINT pt; ::SetBrushOrgEx(m_hDC, point.x, point.y, &pt); return CPoint(pt); }
int CDC::GetArcDirection() const { return ::GetArcDirection(GetAttribDC()); }
int CDC::SetArcDirection(int nArcDirection) { return ::SetArcDirection(m_hDC, nArcDirection); }
int CDC::SaveDC() { return ::SaveDC(m_hDC); }
BOOL CDC::RestoreDC(int nSavedDC) { return ::RestoreDC(m_hDC, nSavedDC); }

int CDC::ExtSelectClipRgn(CRgn* pRgn, int nMode) { return ::ExtSelectClipRgn(m_hDC, pRgn ? (HRGN)pRgn->GetSafeHandle() : NULL, nMode); }
int CDC::GetClipRgn(CRgn* pRgn) const { return ::GetClipRgn(m_hDC, (HRGN)pRgn->GetSafeHandle()); }
int CDC::ExcludeClipRect(int x1, int y1, int x2, int y2) { return ::ExcludeClipRect(m_hDC, x1, y1, x2, y2); }
int CDC::ExcludeClipRect(LPCRECT lpRect) { return ::ExcludeClipRect(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom); }
int CDC::IntersectClipRect(int x1, int y1, int x2, int y2) { return ::IntersectClipRect(m_hDC, x1, y1, x2, y2); }
int CDC::IntersectClipRect(LPCRECT lpRect) { return ::IntersectClipRect(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom); }
int CDC::OffsetClipRgn(SIZE size) { return ::OffsetClipRgn(m_hDC, size.cx, size.cy); }
int CDC::OffsetClipRgn(int x, int y) { return ::OffsetClipRgn(m_hDC, x, y); }
int CDC::GetClipBox(LPRECT lpRect) const { return ::GetClipBox(GetAttribDC(), lpRect); }
BOOL CDC::PtVisible(POINT point) const { return ::PtVisible(GetAttribDC(), point.x, point.y); }
BOOL CDC::RectVisible(LPCRECT lpRect) const { return ::RectVisible(GetAttribDC(), lpRect); }

// --- GDI object selection -------------------------------------------
CGdiObject* CDC::SelectObject(CGdiObject* pObject)
{
    if (pObject == NULL || pObject->GetSafeHandle() == NULL) return NULL;
    HGDIOBJ hOld = ::SelectObject(m_hDC, pObject->GetSafeHandle());
    if (m_hAttribDC != m_hDC) ::SelectObject(m_hAttribDC, pObject->GetSafeHandle());
    return AfxLookupGdi(hOld);
}

CGdiObject* CDC::SelectStockObject(int nIndex)
{
    HGDIOBJ hOld = ::SelectObject(m_hDC, ::GetStockObject(nIndex));
    if (m_hAttribDC != m_hDC) ::SelectObject(m_hAttribDC, ::GetStockObject(nIndex));
    return AfxLookupGdi(hOld);
}

CPen* CDC::SelectObject(CPen* pPen)             { return (CPen*)SelectObject((CGdiObject*)pPen); }
CBrush* CDC::SelectObject(CBrush* pBrush)       { return (CBrush*)SelectObject((CGdiObject*)pBrush); }
CFont* CDC::SelectObject(CFont* pFont)          { return (CFont*)SelectObject((CGdiObject*)pFont); }
CBitmap* CDC::SelectObject(CBitmap* pBitmap)    { return (CBitmap*)SelectObject((CGdiObject*)pBitmap); }

// --- coordinate conversions -----------------------------------------
CPoint CDC::GetViewportOrg() const { POINT pt; ::GetViewportOrgEx(GetAttribDC(), &pt); return CPoint(pt); }
CPoint CDC::SetViewportOrg(POINT point) { POINT pt; ::SetViewportOrgEx(m_hDC, point.x, point.y, &pt); return CPoint(pt); }
CPoint CDC::SetViewportOrg(int x, int y) { POINT pt; ::SetViewportOrgEx(m_hDC, x, y, &pt); return CPoint(pt); }
CPoint CDC::OffsetViewportOrg(int nWidth, int nHeight) { POINT pt; ::OffsetViewportOrgEx(m_hDC, nWidth, nHeight, &pt); return CPoint(pt); }
CSize CDC::GetViewportExt() const { SIZE sz; ::GetViewportExtEx(GetAttribDC(), &sz); return CSize(sz); }
CSize CDC::SetViewportExt(SIZE size) { SIZE sz; ::SetViewportExtEx(m_hDC, size.cx, size.cy, &sz); return CSize(sz); }
CSize CDC::SetViewportExt(int x, int y) { SIZE sz; ::SetViewportExtEx(m_hDC, x, y, &sz); return CSize(sz); }
CSize CDC::ScaleViewportExt(int xNum, int xDenom, int yNum, int yDenom)
{ SIZE sz; ::ScaleViewportExtEx(m_hDC, xNum, xDenom, yNum, yDenom, &sz); return CSize(sz); }
CPoint CDC::GetWindowOrg() const { POINT pt; ::GetWindowOrgEx(GetAttribDC(), &pt); return CPoint(pt); }
CPoint CDC::SetWindowOrg(POINT point) { POINT pt; ::SetWindowOrgEx(m_hDC, point.x, point.y, &pt); return CPoint(pt); }
CPoint CDC::SetWindowOrg(int x, int y) { POINT pt; ::SetWindowOrgEx(m_hDC, x, y, &pt); return CPoint(pt); }
CPoint CDC::OffsetWindowOrg(int nWidth, int nHeight) { POINT pt; ::OffsetWindowOrgEx(m_hDC, nWidth, nHeight, &pt); return CPoint(pt); }
CSize CDC::GetWindowExt() const { SIZE sz; ::GetWindowExtEx(GetAttribDC(), &sz); return CSize(sz); }
CSize CDC::SetWindowExt(SIZE size) { SIZE sz; ::SetWindowExtEx(m_hDC, size.cx, size.cy, &sz); return CSize(sz); }
CSize CDC::SetWindowExt(int x, int y) { SIZE sz; ::SetWindowExtEx(m_hDC, x, y, &sz); return CSize(sz); }
CSize CDC::ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom)
{ SIZE sz; ::ScaleWindowExtEx(m_hDC, xNum, xDenom, yNum, yDenom, &sz); return CSize(sz); }
void CDC::DPtoLP(LPPOINT lpPoints, int nCount) const { ::DPtoLP(GetAttribDC(), lpPoints, nCount); }
void CDC::DPtoLP(LPRECT lpRect) const { ::DPtoLP(GetAttribDC(), (LPPOINT)lpRect, 2); }
void CDC::LPtoDP(LPPOINT lpPoints, int nCount) const { ::LPtoDP(GetAttribDC(), lpPoints, nCount); }
void CDC::LPtoDP(LPRECT lpRect) const { ::LPtoDP(GetAttribDC(), (LPPOINT)lpRect, 2); }

// --- text output ------------------------------------------------------
BOOL CDC::TextOut(int nXPos, int nYPos, LPCTSTR lpszString, int nCount)
{
    if (nCount < 0) nCount = (int)AfxStrLen(lpszString);
    return ::TextOut(m_hDC, nXPos, nYPos, lpszString, nCount);
}
BOOL CDC::TextOut(int nXPos, int nYPos, const CString& str)
{ return TextOut(nXPos, nYPos, (LPCTSTR)str, str.GetLength()); }

BOOL CDC::ExtTextOut(int x, int y, UINT nOptions, LPCRECT lpRect, LPCTSTR lpszString, UINT nCount, int* lpDxWidths)
{
    if (nCount == (UINT)-1) nCount = (UINT)AfxStrLen(lpszString);
    return ::ExtTextOut(m_hDC, x, y, nOptions, lpRect, lpszString, nCount, lpDxWidths);
}
BOOL CDC::ExtTextOut(int x, int y, UINT nOptions, LPCRECT lpRect, const CString& str, int* lpDxWidths)
{ return ExtTextOut(x, y, nOptions, lpRect, (LPCTSTR)str, (UINT)str.GetLength(), lpDxWidths); }

int CDC::DrawText(LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat)
{
    if (nCount < 0) nCount = (int)AfxStrLen(lpszString);
    return ::DrawText(m_hDC, lpszString, nCount, lpRect, nFormat);
}
int CDC::DrawText(const CString& str, LPRECT lpRect, UINT nFormat)
{ return DrawText((LPCTSTR)str, str.GetLength(), lpRect, nFormat); }

int CDC::DrawTextEx(LPRECT lpRect, LPTSTR lpchText, int nCount, UINT nFormat, LPDRAWTEXTPARAMS lpDTParams)
{ return ::DrawTextEx(m_hDC, lpchText, nCount, lpRect, nFormat, lpDTParams); }

BOOL CDC::TabbedTextOut(int x, int y, LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin)
{
    if (nCount < 0) nCount = (int)AfxStrLen(lpszString);
    return ::TabbedTextOut(m_hDC, x, y, lpszString, nCount, nTabPositions, lpnTabStopPositions, nTabOrigin) != 0;
}

BOOL CDC::GetCharWidth(UINT nFirstChar, UINT nLastChar, LPINT lpBuffer) const
{ return ::GetCharWidth(GetAttribDC(), nFirstChar, nLastChar, lpBuffer); }

HFONT CDC::GetCurrentFont() const { return (HFONT)::GetCurrentObject(GetAttribDC(), OBJ_FONT); }

// --- drawing ----------------------------------------------------------
BOOL CDC::MoveTo(int x, int y, CPoint* lpPoint) { POINT pt; BOOL b = ::MoveToEx(m_hDC, x, y, &pt); if (lpPoint) *lpPoint = pt; return b; }
BOOL CDC::MoveTo(POINT point, CPoint* lpPoint) { return MoveTo(point.x, point.y, lpPoint); }
BOOL CDC::LineTo(int x, int y) { return ::LineTo(m_hDC, x, y); }
BOOL CDC::LineTo(POINT point) { return ::LineTo(m_hDC, point.x, point.y); }
BOOL CDC::Arc(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) { return ::Arc(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4); }
BOOL CDC::Arc(LPCRECT lpRect, POINT ptStart, POINT ptEnd)
{ return ::Arc(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y); }
BOOL CDC::ArcTo(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) { return ::ArcTo(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4); }
BOOL CDC::ArcTo(LPCRECT lpRect, POINT ptStart, POINT ptEnd)
{ return ::ArcTo(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y); }
BOOL CDC::AngleArc(int x, int y, int nRadius, float fStartAngle, float fSweepAngle) { return ::AngleArc(m_hDC, x, y, (DWORD)nRadius, fStartAngle, fSweepAngle); }
BOOL CDC::Polyline(LPPOINT lpPoints, int nCount) { return ::Polyline(m_hDC, lpPoints, nCount); }
BOOL CDC::Polygon(LPPOINT lpPoints, int nCount) { return ::Polygon(m_hDC, lpPoints, nCount); }
BOOL CDC::PolyBezier(LPPOINT lpPoints, int nCount) { return ::PolyBezier(m_hDC, lpPoints, (DWORD)nCount); }
BOOL CDC::PolyBezierTo(LPPOINT lpPoints, int nCount) { return ::PolyBezierTo(m_hDC, lpPoints, (DWORD)nCount); }
BOOL CDC::PolyPolygon(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount) { return ::PolyPolygon(m_hDC, lpPoints, lpPolyCounts, nCount); }
BOOL CDC::PolyPolyline(LPPOINT lpPoints, LPDWORD lpPolyPoints, int nCount) { return ::PolyPolyline(m_hDC, lpPoints, lpPolyPoints, (DWORD)nCount); }
BOOL CDC::Rectangle(int x1, int y1, int x2, int y2) { return ::Rectangle(m_hDC, x1, y1, x2, y2); }
BOOL CDC::Rectangle(LPCRECT lpRect) { return ::Rectangle(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom); }
BOOL CDC::RoundRect(int x1, int y1, int x2, int y2, int x3, int y3) { return ::RoundRect(m_hDC, x1, y1, x2, y2, x3, y3); }
BOOL CDC::RoundRect(LPCRECT lpRect, POINT point) { return ::RoundRect(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, point.x, point.y); }
BOOL CDC::Ellipse(int x1, int y1, int x2, int y2) { return ::Ellipse(m_hDC, x1, y1, x2, y2); }
BOOL CDC::Ellipse(LPCRECT lpRect) { return ::Ellipse(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom); }
BOOL CDC::Chord(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) { return ::Chord(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4); }
BOOL CDC::Chord(LPCRECT lpRect, LPCRECT lpArcRect)
{ return ::Chord(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom,
                 lpArcRect->left, lpArcRect->top, lpArcRect->right, lpArcRect->bottom); }
BOOL CDC::Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) { return ::Pie(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4); }
BOOL CDC::Pie(LPCRECT lpRect, LPCRECT lpArcRect)
{ return ::Pie(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom,
               lpArcRect->left, lpArcRect->top, lpArcRect->right, lpArcRect->bottom); }

int CDC::FillRect(LPCRECT lpRect, CBrush* pBrush) { return ::FillRect(m_hDC, lpRect, (HBRUSH)pBrush->GetSafeHandle()); }

void CDC::FillSolidRect(LPCRECT lpRect, COLORREF clr)
{
    COLORREF crOld = SetBkColor(clr);
    ::ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
    SetBkColor(crOld);
}
void CDC::FillSolidRect(int x, int y, int cx, int cy, COLORREF clr)
{
    RECT rc = { x, y, x + cx, y + cy };
    FillSolidRect(&rc, clr);
}
void CDC::FillRgn(CRgn* pRgn, CBrush* pBrush) { ::FillRgn(m_hDC, (HRGN)pRgn->GetSafeHandle(), (HBRUSH)pBrush->GetSafeHandle()); }
void CDC::FrameRect(LPCRECT lpRect, CBrush* pBrush) { ::FrameRect(m_hDC, lpRect, (HBRUSH)pBrush->GetSafeHandle()); }
void CDC::FrameRgn(CRgn* pRgn, CBrush* pBrush, int nWidth, int nHeight)
{ ::FrameRgn(m_hDC, (HRGN)pRgn->GetSafeHandle(), (HBRUSH)pBrush->GetSafeHandle(), nWidth, nHeight); }
void CDC::InvertRect(LPCRECT lpRect) { ::InvertRect(m_hDC, lpRect); }
void CDC::InvertRgn(CRgn* pRgn) { ::InvertRgn(m_hDC, (HRGN)pRgn->GetSafeHandle()); }
void CDC::PaintRgn(CRgn* pRgn) { ::PaintRgn(m_hDC, (HRGN)pRgn->GetSafeHandle()); }
void CDC::DrawEdge(LPRECT lpRect, UINT nEdge, UINT nFlags) { ::DrawEdge(m_hDC, lpRect, nEdge, nFlags); }
void CDC::DrawFrameControl(LPRECT lpRect, UINT nType, UINT nState) { ::DrawFrameControl(m_hDC, lpRect, nType, nState); }
void CDC::DrawFocusRect(LPCRECT lpRect) { ::DrawFocusRect(m_hDC, lpRect); }

void CDC::Draw3dRect(LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
    int cx = lpRect->right - lpRect->left;
    int cy = lpRect->bottom - lpRect->top;
    FillSolidRect(lpRect->left, lpRect->top, cx - 1, 1, clrTopLeft);
    FillSolidRect(lpRect->left, lpRect->top, 1, cy - 1, clrTopLeft);
    FillSolidRect(lpRect->left + cx, lpRect->top, -1, cy, clrBottomRight);
    FillSolidRect(lpRect->left, lpRect->top + cy, cx, -1, clrBottomRight);
}
void CDC::Draw3dRect(int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
    RECT rc = { x, y, x + cx, y + cy };
    Draw3dRect(&rc, clrTopLeft, clrBottomRight);
}

void CDC::GradientFillRect(LPCRECT lpRect, COLORREF clrStart, COLORREF clrEnd, BOOL bVertical)
{
    int r0 = GetRValue(clrStart), g0 = GetGValue(clrStart), b0 = GetBValue(clrStart);
    int r1 = GetRValue(clrEnd),   g1 = GetGValue(clrEnd),   b1 = GetBValue(clrEnd);
    int nSteps = bVertical ? (lpRect->bottom - lpRect->top) : (lpRect->right - lpRect->left);
    if (nSteps <= 1) { FillSolidRect(lpRect, clrStart); return; }
    int nSavedMode = SetBkMode(OPAQUE);
    for (int i = 0; i < nSteps; ++i) {
        int r = r0 + (r1 - r0) * i / (nSteps - 1);
        int g = g0 + (g1 - g0) * i / (nSteps - 1);
        int b = b0 + (b1 - b0) * i / (nSteps - 1);
        SetBkColor(RGB(r, g, b));
        RECT rc;
        if (bVertical) { rc.left = lpRect->left; rc.right = lpRect->right; rc.top = lpRect->top + i; rc.bottom = rc.top + 1; }
        else           { rc.top = lpRect->top;  rc.bottom = lpRect->bottom; rc.left = lpRect->left + i; rc.right = rc.left + 1; }
        ::ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    }
    SetBkMode(nSavedMode);
}

BOOL CDC::DrawIcon(int x, int y, HICON hIcon) { return ::DrawIcon(m_hDC, x, y, hIcon); }
BOOL CDC::DrawIcon(POINT point, HICON hIcon) { return ::DrawIcon(m_hDC, point.x, point.y, hIcon); }
BOOL CDC::DrawIconEx(int x, int y, HICON hIcon, int cxWidth, int cyWidth, UINT nStepIfAniCur, HBRUSH hbrFlickerFreeDraw, UINT nFlags)
{ return ::DrawIconEx(m_hDC, x, y, hIcon, cxWidth, cyWidth, nStepIfAniCur, hbrFlickerFreeDraw, nFlags); }

COLORREF CDC::GetPixel(int x, int y) const { return ::GetPixel(GetAttribDC(), x, y); }
COLORREF CDC::GetPixel(POINT point) const { return ::GetPixel(GetAttribDC(), point.x, point.y); }
COLORREF CDC::SetPixel(int x, int y, COLORREF crColor) { return ::SetPixel(m_hDC, x, y, crColor); }
COLORREF CDC::SetPixel(POINT point, COLORREF crColor) { return ::SetPixel(m_hDC, point.x, point.y, crColor); }
BOOL CDC::FloodFill(int x, int y, COLORREF crColor) { return ::FloodFill(m_hDC, x, y, crColor); }
BOOL CDC::ExtFloodFill(int x, int y, COLORREF crColor, UINT nFillType) { return ::ExtFloodFill(m_hDC, x, y, crColor, nFillType); }

BOOL CDC::BitBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC, int xSrc, int ySrc, DWORD dwRop)
{ return ::BitBlt(m_hDC, x, y, nWidth, nHeight, pSrcDC ? pSrcDC->GetSafeHdc() : NULL, xSrc, ySrc, dwRop); }

BOOL CDC::StretchBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwRop)
{
    ::SetStretchBltMode(m_hDC, (dwRop == HALFTONE) ? HALFTONE : COLORONCOLOR);
    return ::StretchBlt(m_hDC, x, y, nWidth, nHeight, pSrcDC ? pSrcDC->GetSafeHdc() : NULL,
                        xSrc, ySrc, nSrcWidth, nSrcHeight, dwRop);
}

BOOL CDC::MaskBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC, int xSrc, int ySrc,
                  CBitmap& maskBitmap, int xMask, int yMask, DWORD dwRop)
{
    return ::MaskBlt(m_hDC, x, y, nWidth, nHeight, pSrcDC ? pSrcDC->GetSafeHdc() : NULL,
                     xSrc, ySrc, (HBITMAP)maskBitmap.GetSafeHandle(), xMask, yMask, dwRop);
}

BOOL CDC::PlgBlt(LPPOINT lpPoint, CDC* pSrcDC, int xSrc, int ySrc, int nWidth, int nHeight,
                 CBitmap& maskBitmap, int xMask, int yMask)
{
    return ::PlgBlt(m_hDC, lpPoint, pSrcDC ? pSrcDC->GetSafeHdc() : NULL, xSrc, ySrc, nWidth, nHeight,
                    maskBitmap.GetSafeHandle() ? (HBITMAP)maskBitmap.GetSafeHandle() : NULL, xMask, yMask);
}

BOOL CDC::PatBlt(int x, int y, int nWidth, int nHeight, DWORD dwRop)
{ return ::PatBlt(m_hDC, x, y, nWidth, nHeight, dwRop); }

BOOL CDC::TransparentBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
                         int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, UINT crTransparent)
{
    return ::TransparentBlt(m_hDC, x, y, nWidth, nHeight, pSrcDC->GetSafeHdc(),
                            xSrc, ySrc, nSrcWidth, nSrcHeight, crTransparent) ? TRUE : FALSE;
}

BOOL CDC::AlphaBlend(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
                     int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, BYTE bAlpha)
{
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = bAlpha;
    bf.AlphaFormat = 0;
    return ::AlphaBlend(m_hDC, x, y, nWidth, nHeight, pSrcDC->GetSafeHdc(),
                        xSrc, ySrc, nSrcWidth, nSrcHeight, bf) ? TRUE : FALSE;
}

// --- printing ---------------------------------------------------------
BOOL CDC::StartDoc(LPCTSTR lpszDocName)
{
    DOCINFO di;
    memset(&di, 0, sizeof(di));
    di.cbSize = sizeof(di);
    di.lpszDocName = lpszDocName;
    return ::StartDoc(m_hDC, &di) > 0;
}
BOOL CDC::StartDoc(LPDOCINFO lpDocInfo) { return ::StartDoc(m_hDC, lpDocInfo) > 0; }
int CDC::StartPage() { return ::StartPage(m_hDC); }
int CDC::EndPage() { return ::EndPage(m_hDC); }
int CDC::EndDoc() { return ::EndDoc(m_hDC); }
int CDC::AbortDoc() { return ::AbortDoc(m_hDC); }
// ---------------------------------------------------------------------
//  CClientDC / CWindowDC / CPaintDC
// ---------------------------------------------------------------------
CClientDC::CClientDC(CWnd* pWnd) : m_pWnd(pWnd)
{
    m_hDC = ::GetDC(pWnd != NULL ? pWnd->GetSafeHwnd() : NULL);
    m_hAttribDC = m_hDC;
}
CClientDC::~CClientDC()
{
    ::ReleaseDC(m_pWnd != NULL ? m_pWnd->GetSafeHwnd() : NULL, m_hDC);
    m_hDC = NULL; m_hAttribDC = NULL;
}

CWindowDC::CWindowDC(CWnd* pWnd) : m_pWnd(pWnd)
{
    m_hDC = ::GetWindowDC(pWnd != NULL ? pWnd->GetSafeHwnd() : NULL);
    m_hAttribDC = m_hDC;
}
CWindowDC::~CWindowDC()
{
    ::ReleaseDC(m_pWnd != NULL ? m_pWnd->GetSafeHwnd() : NULL, m_hDC);
    m_hDC = NULL; m_hAttribDC = NULL;
}

CPaintDC::CPaintDC(CWnd* pWnd) : m_hWnd(pWnd != NULL ? pWnd->GetSafeHwnd() : NULL)
{
    memset(&m_ps, 0, sizeof(m_ps));
    m_hDC = ::BeginPaint(m_hWnd, &m_ps);
    m_hAttribDC = m_hDC;
}
CPaintDC::~CPaintDC()
{
    ::EndPaint(m_hWnd, &m_ps);
    m_hDC = NULL; m_hAttribDC = NULL;
}

// ---------------------------------------------------------------------
//  CImageList helpers that need CDC / CWnd to be complete
// ---------------------------------------------------------------------
static std::map<HIMAGELIST, CImageList*> s_tempImageLists;

CImageList* CImageList::FromHandle(HIMAGELIST h)
{
    if (h == NULL) return NULL;
    std::map<HIMAGELIST, CImageList*>::iterator it = s_tempImageLists.find(h);
    if (it != s_tempImageLists.end()) return it->second;
    CImageList* p = new CImageList();
    p->m_hImageList = h;
    p->m_bOwner = FALSE;              // the control owns it
    s_tempImageLists[h] = p;
    return p;
}

BOOL CImageList::Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle)
{ return ImageList_Draw(m_hImageList, nImage, pDC->GetSafeHdc(), pt.x, pt.y, nStyle); }
BOOL CImageList::Draw(CDC* pDC, int nImage, int x, int y, UINT nStyle)
{ return ImageList_Draw(m_hImageList, nImage, pDC->GetSafeHdc(), x, y, nStyle); }
