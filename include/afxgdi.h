// =====================================================================
//  afxgdi.h  --  GDI wrapper classes for the MFC-GCC clone.
//                CGdiObject / CPen / CBrush / CFont / CBitmap / CRgn /
//                CDC / CClientDC / CPaintDC / CWindowDC
// =====================================================================
#ifndef __AFXGDI_H__
#define __AFXGDI_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif

class CDC;
class CWnd;
class CPen;
class CBrush;
class CFont;
class CBitmap;
class CRgn;
class CImageList;

// GDI handle <-> wrapper bookkeeping (implemented in mfc.cpp).  MFC keeps a
// temporary-object map; we keep a permanent one so that SelectObject() can
// hand back the wrapper that was previously selected.
void       AFXAPI AfxRegisterGdi(HGDIOBJ hObject, class CGdiObject* pObject);
void       AFXAPI AfxUnregisterGdi(HGDIOBJ hObject);
CGdiObject* AFXAPI AfxLookupGdi(HGDIOBJ hObject);

// ---------------------------------------------------------------------
//  CGdiObject
// ---------------------------------------------------------------------
class CGdiObject : public CObject
{
    DECLARE_DYNAMIC(CGdiObject)
public:
    CGdiObject() : m_hObject(NULL), m_bStock(FALSE) {}
    virtual ~CGdiObject();

    HGDIOBJ GetSafeHandle() const { return m_hObject; }
    operator HGDIOBJ() const { return m_hObject; }
    BOOL Attach(HGDIOBJ hObject);
    HGDIOBJ Detach();
    BOOL DeleteObject();
    int GetObject(int nCount, LPVOID lpObject) const;
    UINT GetObjectType() const;
    BOOL Equal(const CGdiObject* pObj) const { return m_hObject == pObj->m_hObject; }
    static CGdiObject* PASCAL FromHandle(HGDIOBJ hObject) { return AfxLookupGdi(hObject); }
    BOOL CreateStockObject(int nIndex);
    BOOL UnrealizeObject();
    CGdiObject& operator=(HGDIOBJ hObject) { m_hObject = hObject; return *this; }

    HGDIOBJ m_hObject;
    BOOL    m_bStock;       // TRUE for stock/system objects (never deleted)
};

// ---------------------------------------------------------------------
//  CPen
// ---------------------------------------------------------------------
class CPen : public CGdiObject
{
    DECLARE_DYNAMIC(CPen)
public:
    CPen();
    CPen(int nPenStyle, int nWidth, COLORREF crColor);
    CPen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush, int nStyleCount = 0, const DWORD* lpStyle = NULL);
    virtual ~CPen();

    BOOL CreatePen(int nPenStyle, int nWidth, COLORREF crColor);
    BOOL CreatePen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush,
                   int nStyleCount = 0, const DWORD* lpStyle = NULL);
    BOOL CreatePenIndirect(LPLOGPEN lpLogPen);
    int GetLogPen(LOGPEN* pLogPen);
    operator HPEN() const { return (HPEN)m_hObject; }
    static CPen* PASCAL FromHandle(HPEN hPen) { return (CPen*)AfxLookupGdi(hPen); }
};

// ---------------------------------------------------------------------
//  CBrush
// ---------------------------------------------------------------------
class CBrush : public CGdiObject
{
    DECLARE_DYNAMIC(CBrush)
public:
    CBrush();
    CBrush(COLORREF crColor);                    // CreateSolidBrush
    CBrush(int nIndex, COLORREF crColor);        // CreateHatchBrush
    CBrush(CBitmap* pBitmap);                    // CreatePatternBrush
    virtual ~CBrush();

    BOOL CreateSolidBrush(COLORREF crColor);
    BOOL CreateHatchBrush(int nIndex, COLORREF crColor);
    BOOL CreatePatternBrush(CBitmap* pBitmap);
    BOOL CreateBrushIndirect(const LOGBRUSH* lpLogBrush);
    BOOL CreateDIBPatternBrush(HGLOBAL hPackedDIB, UINT nUsage = DIB_RGB_COLORS);
    BOOL CreateSysColorBrush(int nIndex);
    int GetLogBrush(LOGBRUSH* pLogBrush);
    operator HBRUSH() const { return (HBRUSH)m_hObject; }
    static CBrush* PASCAL FromHandle(HBRUSH hBrush) { return (CBrush*)AfxLookupGdi(hBrush); }
};

// ---------------------------------------------------------------------
//  CFont
// ---------------------------------------------------------------------
class CFont : public CGdiObject
{
    DECLARE_DYNAMIC(CFont)
public:
    CFont();
    virtual ~CFont();

    BOOL CreateFontIndirect(const LOGFONT* lpLogFont);
    BOOL CreateFont(int nHeight, int nWidth, int nEscapement, int nOrientation,
                    int nWeight, BYTE bItalic, BYTE bUnderline, BYTE cStrikeOut,
                    BYTE nCharSet, BYTE nOutPrecision, BYTE nClipPrecision,
                    BYTE nQuality, BYTE nPitchAndFamily, LPCTSTR lpszFacename);
    BOOL CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, CDC* pDC = NULL);
    BOOL CreatePointFontIndirect(const LOGFONT* lpLogFont, CDC* pDC = NULL);
    int GetLogFont(LOGFONT* pLogFont);
    operator HFONT() const { return (HFONT)m_hObject; }
    static CFont* PASCAL FromHandle(HFONT hFont) { return (CFont*)AfxLookupGdi(hFont); }
};

// ---------------------------------------------------------------------
//  CBitmap
// ---------------------------------------------------------------------
class CBitmap : public CGdiObject
{
    DECLARE_DYNAMIC(CBitmap)
public:
    CBitmap();
    virtual ~CBitmap();

    BOOL LoadBitmap(LPCTSTR lpszResourceName);
    BOOL LoadBitmap(UINT nIDResource);
    BOOL LoadOEMBitmap(UINT nIDBitmap);
    BOOL LoadMappedBitmap(UINT nIdBitmap, UINT nFlags = 0, LPCOLORMAP lpColorMap = NULL, int nNumColors = 0);
    BOOL CreateCompatibleBitmap(CDC* pDC, int nWidth, int nHeight);
    BOOL CreateDiscardableBitmap(CDC* pDC, int nWidth, int nHeight);
    BOOL CreateBitmap(int nWidth, int nHeight, UINT nPlanes, UINT nBitcount, const void* lpBits);
    BOOL CreateBitmapIndirect(LPBITMAP lpBitmap);
    HBITMAP CreateDIBitmap(CDC* pDC, LPBITMAPINFOHEADER lpbmih, DWORD fdwInit,
                           LPCVOID lpbInit, LPBITMAPINFO lpbmi, UINT fuUsage);
    BOOL CreateDIBSection(CDC* pDC, const BITMAPINFO* pbmi, UINT iUsage,
                          void** ppvBits, HANDLE hSection = NULL, DWORD dwOffset = 0);
    int GetBitmap(BITMAP* pBitMap);
    operator HBITMAP() const { return (HBITMAP)m_hObject; }
    static CBitmap* PASCAL FromHandle(HBITMAP hBitmap) { return (CBitmap*)AfxLookupGdi(hBitmap); }
};

// ---------------------------------------------------------------------
//  CRgn
// ---------------------------------------------------------------------
class CRgn : public CGdiObject
{
    DECLARE_DYNAMIC(CRgn)
public:
    CRgn();
    virtual ~CRgn();

    operator HRGN() const { return (HRGN)m_hObject; }
    BOOL CreateRectRgn(int x1, int y1, int x2, int y2);
    BOOL CreateRectRgnIndirect(LPCRECT lpRect);
    BOOL CreateEllipticRgn(int x1, int y1, int x2, int y2);
    BOOL CreateEllipticRgnIndirect(LPCRECT lpRect);
    BOOL CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode);
    BOOL CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyMode);
    BOOL CreateRoundRectRgn(int x1, int y1, int x2, int y2, int x3, int y3);
    BOOL CreateFromPath(CDC* pDC);
    BOOL CreateFromData(const XFORM* lpXForm, HRGN hRgn);
    int CombineRgn(CRgn* pRgn1, CRgn* pRgn2, int nCombineMode);
    int CombineRgn(CRgn* pRgn1, CRgn* pRgn2, int nCombineMode) const;
    int CopyRgn(CRgn* pRgnSrc);
    int EqualRgn(CRgn* pRgn) const;
    int OffsetRgn(POINT point);
    int OffsetRgn(int x, int y);
    int GetRgnBox(LPRECT lpRect) const;
    BOOL PtInRegion(int x, int y) const;
    BOOL PtInRegion(POINT point) const;
    BOOL RectInRegion(LPCRECT lpRect) const;
    int GetRegionData(LPRGNDATA lpRgnData, int nDataSize) const;
    static CRgn* PASCAL FromHandle(HRGN hRgn) { return (CRgn*)AfxLookupGdi(hRgn); }
};

// ---------------------------------------------------------------------
//  CDC
// ---------------------------------------------------------------------
class CDC : public CObject
{
    DECLARE_DYNAMIC(CDC)
public:
    // Attributes
    HDC m_hDC;          // the output DC (drawing)
    HDC m_hAttribDC;    // the attribute DC (queries)

    CDC();
    virtual ~CDC();

    static CDC* PASCAL FromHandle(HDC hDC);
    static void PASCAL DeleteTempMap();
    BOOL Attach(HDC hDC);
    HDC Detach();
    operator HDC() const { return m_hDC; }
    HDC GetSafeHdc() const { return m_hDC; }
    HDC GetOutputSafeHdc() const { return m_hAttribDC ? m_hAttribDC : m_hDC; }
    CWnd* GetWindow() const;
    BOOL IsPrinting() const { return m_bPrinting; }

    // creation / destruction
    BOOL CreateDC(LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName, LPCTSTR lpszOutput, const void* lpInitData);
    BOOL CreateIC(LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName, LPCTSTR lpszOutput, const void* lpInitData);
    BOOL CreateCompatibleDC(CDC* pDC);
    BOOL DeleteDC();

    // device capabilities
    int GetDeviceCaps(int nIndex) const;
    CSize GetTextExtent(LPCTSTR lpszString, int nCount) const;
    CSize GetTextExtent(const CString& str) const;
    CSize GetOutputTextExtent(LPCTSTR lpszString, int nCount) const;
    CSize GetTabbedTextExtent(LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions) const;
    CSize GetOutputTabbedTextExtent(LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions) const;
    CString GetTextFace() const;
    int GetTextFace(int nCount, LPTSTR lpszData) const;
    BOOL GetTextMetrics(LPTEXTMETRIC lpMetrics) const;
    BOOL GetOutputTextMetrics(LPTEXTMETRIC lpMetrics) const;
    int GetTextAlign() const;
    int SetTextAlign(UINT nFlags);
    int GetTextCharacterExtra() const;
    int SetTextCharacterExtra(int nCharExtra);
    int GetMapMode() const;
    int SetMapMode(int nMapMode);
    int GetGraphicsMode() const;
    int SetGraphicsMode(int iMode);

    // drawing attributes
    COLORREF GetTextColor() const;
    COLORREF SetTextColor(COLORREF crColor);
    COLORREF GetBkColor() const;
    COLORREF SetBkColor(COLORREF crColor);
    int GetBkMode() const;
    int SetBkMode(int nBkMode);
    int GetPolyFillMode() const;
    int SetPolyFillMode(int nPolyFillMode);
    int GetROP2() const;
    int SetROP2(int nDrawMode);
    int GetStretchBltMode() const;
    int SetStretchBltMode(int nStretchMode);
    CPoint GetBrushOrg() const;
    CPoint SetBrushOrg(int nXOrg, int nYOrg);
    CPoint SetBrushOrg(POINT point);
    int GetArcDirection() const;
    int SetArcDirection(int nArcDirection);
    int SaveDC();
    BOOL RestoreDC(int nSavedDC);
    int SelectClipRgn(CRgn* pRgn) { return ::SelectClipRgn(m_hDC, (HRGN)(pRgn ? pRgn->GetSafeHandle() : NULL)); }
    int ExtSelectClipRgn(CRgn* pRgn, int nMode);
    int ExcludeClipRect(int x1, int y1, int x2, int y2);
    int ExcludeClipRect(LPCRECT lpRect);
    int IntersectClipRect(int x1, int y1, int x2, int y2);
    int IntersectClipRect(LPCRECT lpRect);
    int OffsetClipRgn(SIZE size);
    int OffsetClipRgn(int x, int y);
    int GetClipBox(LPRECT lpRect) const;
    BOOL PtVisible(POINT point) const;
    BOOL RectVisible(LPCRECT lpRect) const;
    int GetClipRgn(CRgn* pRgn) const;

    // coordinate transformations
    CPoint GetViewportOrg() const;
    CPoint SetViewportOrg(POINT point);
    CPoint SetViewportOrg(int x, int y);
    CPoint OffsetViewportOrg(int nWidth, int nHeight);
    CSize GetViewportExt() const;
    CSize SetViewportExt(SIZE size);
    CSize SetViewportExt(int x, int y);
    CSize ScaleViewportExt(int xNum, int xDenom, int yNum, int yDenom);
    CPoint GetWindowOrg() const;
    CPoint SetWindowOrg(POINT point);
    CPoint SetWindowOrg(int x, int y);
    CPoint OffsetWindowOrg(int nWidth, int nHeight);
    CSize GetWindowExt() const;
    CSize SetWindowExt(SIZE size);
    CSize SetWindowExt(int x, int y);
    CSize ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom);
    void DPtoLP(LPPOINT lpPoints, int nCount = 1) const;
    void DPtoLP(LPRECT lpRect) const;
    void DPtoLP(CPoint& point) const { DPtoLP((LPPOINT)&point, 1); }
    void DPtoLP(CRect& rect) const   { DPtoLP((LPRECT)&rect); }
    void LPtoDP(LPPOINT lpPoints, int nCount = 1) const;
    void LPtoDP(LPRECT lpRect) const;
    void LPtoDP(CPoint& point) const { LPtoDP((LPPOINT)&point, 1); }
    void LPtoDP(CRect& rect) const   { LPtoDP((LPRECT)&rect); }

    // GDI object selection
    CGdiObject* SelectObject(CGdiObject* pObject);
    CGdiObject* SelectStockObject(int nIndex);
    CPen* SelectObject(CPen* pPen);
    CBrush* SelectObject(CBrush* pBrush);
    CFont* SelectObject(CFont* pFont);
    CBitmap* SelectObject(CBitmap* pBitmap);
    int SelectObject(CRgn* pRgn) { return ::SelectClipRgn(m_hDC, (HRGN)(pRgn ? pRgn->GetSafeHandle() : NULL)); }

    // text output
    BOOL TextOut(int nXPos, int nYPos, LPCTSTR lpszString, int nCount = -1);
    BOOL TextOut(int nXPos, int nYPos, const CString& str);
    BOOL ExtTextOut(int x, int y, UINT nOptions, LPCRECT lpRect, LPCTSTR lpszString,
                    UINT nCount = (UINT)-1, int* lpDxWidths = NULL);
    BOOL ExtTextOut(int x, int y, UINT nOptions, LPCRECT lpRect, const CString& str, int* lpDxWidths = NULL);
    int DrawText(LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat);
    int DrawText(const CString& str, LPRECT lpRect, UINT nFormat);
    BOOL TabbedTextOut(int x, int y, LPCTSTR lpszString, int nCount, int nTabPositions,
                       LPINT lpnTabStopPositions, int nTabOrigin);
    CSize GetOutputTextExtent(const CString& str) const { return GetOutputTextExtent(str, str.GetLength()); }
    BOOL GetCharWidth(UINT nFirstChar, UINT nLastChar, LPINT lpBuffer) const;

    // drawing operations
    BOOL MoveTo(int x, int y, CPoint* lpPoint = NULL);
    BOOL MoveTo(POINT point, CPoint* lpPoint = NULL);
    BOOL LineTo(int x, int y);
    BOOL LineTo(POINT point);
    BOOL Arc(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
    BOOL Arc(LPCRECT lpRect, POINT ptStart, POINT ptEnd);
    BOOL ArcTo(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
    BOOL ArcTo(LPCRECT lpRect, POINT ptStart, POINT ptEnd);
    BOOL AngleArc(int x, int y, int nRadius, float fStartAngle, float fSweepAngle);
    BOOL Polyline(LPPOINT lpPoints, int nCount);
    BOOL Polyline(const CPoint* lpPoints, int nCount) { return Polyline((LPPOINT)lpPoints, nCount); }
    BOOL Polygon(LPPOINT lpPoints, int nCount);
    BOOL Polygon(const CPoint* lpPoints, int nCount) { return Polygon((LPPOINT)lpPoints, nCount); }
    BOOL PolyBezier(LPPOINT lpPoints, int nCount);
    BOOL PolyBezierTo(LPPOINT lpPoints, int nCount);
    BOOL PolyPolygon(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount);
    BOOL PolyPolyline(LPPOINT lpPoints, LPDWORD lpPolyPoints, int nCount);
    BOOL Rectangle(int x1, int y1, int x2, int y2);
    BOOL Rectangle(LPCRECT lpRect);
    BOOL RoundRect(int x1, int y1, int x2, int y2, int x3, int y3);
    BOOL RoundRect(LPCRECT lpRect, POINT point);
    BOOL Ellipse(int x1, int y1, int x2, int y2);
    BOOL Ellipse(LPCRECT lpRect);
    BOOL Chord(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
    BOOL Chord(LPCRECT lpRect, LPCRECT lpArcRect);
    BOOL Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
    BOOL Pie(LPCRECT lpRect, LPCRECT lpArcRect);

    // filling / framing
    int FillRect(LPCRECT lpRect, CBrush* pBrush);
    void FillSolidRect(LPCRECT lpRect, COLORREF clr);
    void FillSolidRect(int x, int y, int cx, int cy, COLORREF clr);
    void FillRgn(CRgn* pRgn, CBrush* pBrush);
    void FrameRect(LPCRECT lpRect, CBrush* pBrush);
    void FrameRgn(CRgn* pRgn, CBrush* pBrush, int nWidth, int nHeight);
    void InvertRect(LPCRECT lpRect);
    void InvertRgn(CRgn* pRgn);
    void PaintRgn(CRgn* pRgn);
    void DrawEdge(LPRECT lpRect, UINT nEdge, UINT nFlags);
    void DrawFrameControl(LPRECT lpRect, UINT nType, UINT nState);
    void DrawFocusRect(LPCRECT lpRect);
    void Draw3dRect(LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
    void Draw3dRect(int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
    void GradientFillRect(LPCRECT lpRect, COLORREF clrStart, COLORREF clrEnd, BOOL bVertical = FALSE);
    BOOL DrawIcon(int x, int y, HICON hIcon);
    BOOL DrawIcon(POINT point, HICON hIcon);
    BOOL DrawIconEx(int x, int y, HICON hIcon, int cxWidth = 0, int cyWidth = 0,
                    UINT nStepIfAniCur = 0, HBRUSH hbrFlickerFreeDraw = NULL, UINT nFlags = DI_NORMAL);

    // pixels
    COLORREF GetPixel(int x, int y) const;
    COLORREF GetPixel(POINT point) const;
    COLORREF SetPixel(int x, int y, COLORREF crColor);
    COLORREF SetPixel(POINT point, COLORREF crColor);
    BOOL FloodFill(int x, int y, COLORREF crColor);
    BOOL ExtFloodFill(int x, int y, COLORREF crColor, UINT nFillType);

    // blitting
    BOOL BitBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC, int xSrc, int ySrc, DWORD dwRop);
    BOOL StretchBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC, int xSrc, int ySrc,
                    int nSrcWidth, int nSrcHeight, DWORD dwRop);
    BOOL MaskBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC, int xSrc, int ySrc,
                 CBitmap& maskBitmap, int xMask, int yMask, DWORD dwRop);
    BOOL PlgBlt(LPPOINT lpPoint, CDC* pSrcDC, int xSrc, int ySrc, int nWidth, int nHeight,
                CBitmap& maskBitmap, int xMask, int yMask);
    BOOL PatBlt(int x, int y, int nWidth, int nHeight, DWORD dwRop);
    BOOL TransparentBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
                        int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, UINT crTransparent);
    BOOL AlphaBlend(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
                    int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, BYTE bAlpha = 255);

    // printing helpers
    BOOL StartDoc(LPCTSTR lpszDocName);
    BOOL StartDoc(LPDOCINFO lpDocInfo);
    int StartPage();
    int EndPage();
    int EndDoc();
    int AbortDoc();
    int SetAbortProc(BOOL (CALLBACK* lpfnAbortProc)(HDC, int)) { return ::SetAbortProc(m_hAttribDC, lpfnAbortProc); }

    // misc
    int GetDpiX() const { return GetDeviceCaps(LOGPIXELSX); }
    int GetDpiY() const { return GetDeviceCaps(LOGPIXELSY); }
    int DrawTextEx(LPRECT lpRect, LPTSTR lpchText, int nCount, UINT nFormat, LPDRAWTEXTPARAMS lpDTParams);
    HFONT GetCurrentFont() const;

    BOOL m_bPrinting;
    void SetPrinting(BOOL bPrinting) { m_bPrinting = bPrinting; }
protected:
    HDC GetAttribDC() const { return m_hAttribDC ? m_hAttribDC : m_hDC; }
};

// ---------------------------------------------------------------------
//  CClientDC / CWindowDC / CPaintDC
// ---------------------------------------------------------------------
class CClientDC : public CDC
{
public:
    explicit CClientDC(CWnd* pWnd);
    virtual ~CClientDC();
protected:
    CWnd* m_pWnd;
};

class CWindowDC : public CDC
{
public:
    explicit CWindowDC(CWnd* pWnd);
    virtual ~CWindowDC();
protected:
    CWnd* m_pWnd;
};

class CPaintDC : public CDC
{
public:
    explicit CPaintDC(CWnd* pWnd);
    virtual ~CPaintDC();
    PAINTSTRUCT m_ps;
protected:
    HWND m_hWnd;
};

#endif /* __AFXGDI_H__ */
