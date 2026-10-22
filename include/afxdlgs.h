// =====================================================================
//  afxdlgs.h  --  Common dialog wrappers for the MFC-GCC clone:
//                 CFileDialog, CColorDialog, CFontDialog, CPrintDialog,
//                 CPageSetupDialog, CFindReplaceDialog,
//                 CPropertySheet / CPropertyPage
// =====================================================================
#ifndef __AFXDLGS_H__
#define __AFXDLGS_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif
#include "afxwnd.h"
#include "afxdlg.h"
#include <prsht.h>

#ifndef PITCH_FONTTYPE_MASK
#define PITCH_FONTTYPE_MASK   0x000F
#endif
#ifndef SCRIPT_FONTTYPE_MASK
#define SCRIPT_FONTTYPE_MASK  0x0F00
#endif
#ifndef SYMBOL_FONTTYPE
#define SYMBOL_FONTTYPE       0x0008
#endif
#ifndef DEFAULT_FONTTYPE
#define DEFAULT_FONTTYPE      0x0002
#endif

class CPropertySheet;

// ---------------------------------------------------------------------
//  CFileDialog
// ---------------------------------------------------------------------
class CFileDialog : public CDialog
{
    DECLARE_DYNAMIC(CFileDialog)
public:
    CFileDialog(BOOL bOpenFileDialog,
                LPCTSTR lpszDefExt = NULL,
                LPCTSTR lpszFileName = NULL,
                DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                LPCTSTR lpszFilter = NULL,
                CWnd* pParentWnd = NULL);
    virtual ~CFileDialog();

    virtual INT_PTR DoModal();
    virtual void OnFileName();       // called when the file name changes
    virtual void OnFolderChange();
    virtual void OnLBSelChangedNotify(UINT nCodeBox, UINT nCurSel, UINT nPrevSel);
    virtual void OnTypeChange();
    virtual UINT OnShareViolation(LPCTSTR lpszPathName);
    virtual BOOL OnFileNameOK();
    virtual void OnOK();
    virtual void OnCancel();

    CString GetPathName() const;
    CString GetFileName() const;
    CString GetFileExt() const;
    CString GetFileTitle() const;
    CString GetFolderPath() const;
    CString GetSpec() const;
    BOOL GetReadOnlyPref() const;
    POSITION GetStartPosition() const;
    CString GetNextPathName(POSITION& pos) const;

    OPENFILENAME m_ofn;      // the real Win32 structure

protected:
    CString m_strFileName;
    CString m_strFilter;
    CString m_strDefExt;
    TCHAR   m_szFileBuffer[_MAX_PATH * 4];
    TCHAR   m_szFileTitle[_MAX_PATH];
    BOOL    m_bOpenFileDialog;
    BOOL    m_bMultiSelect;
    mutable CString m_strLastDir;

    friend UINT_PTR CALLBACK AfxCommDlgProc(HWND, UINT, WPARAM, LPARAM);
    DECLARE_MESSAGE_MAP()
};

// ---------------------------------------------------------------------
//  CColorDialog
// ---------------------------------------------------------------------
class CColorDialog : public CDialog
{
    DECLARE_DYNAMIC(CColorDialog)
public:
    CColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0, CWnd* pParentWnd = NULL);
    static COLORREF GetColor();
    COLORREF GetSelectedColor() const { return m_cc.rgbResult; }
    void SetCurrentColor(COLORREF clr) { m_cc.rgbResult = clr; }
    virtual INT_PTR DoModal();

    CHOOSECOLOR m_cc;
protected:
    static COLORREF PASCAL* GetCustomColors();
};

// ---------------------------------------------------------------------
//  CFontDialog
// ---------------------------------------------------------------------
class CFontDialog : public CDialog
{
    DECLARE_DYNAMIC(CFontDialog)
public:
    CFontDialog(const LOGFONT* lplfInitial = NULL,
                DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
                CDC* pdcPrinter = NULL,
                CWnd* pParentWnd = NULL);

    void GetCurrentFont(LOGFONT* lplf);
    CString GetFaceName() const   { return CString(m_lf.lfFaceName); }
    CString GetStyleName() const  { return CString(); }   // CHOOSEFONT has no style name on gcc
    int GetSize() const           { return (int)(m_cf.iPointSize / 10); }
    int GetWeight() const         { return (int)m_lf.lfWeight; }
    BOOL IsItalic() const         { return m_lf.lfItalic ? TRUE : FALSE; }
    BOOL IsBold() const           { return m_lf.lfWeight >= FW_BOLD ? TRUE : FALSE; }
    BOOL IsStrikeOut() const      { return m_lf.lfStrikeOut ? TRUE : FALSE; }
    BOOL IsUnderline() const      { return m_lf.lfUnderline ? TRUE : FALSE; }
    int GetPitch() const          { return m_cf.nFontType & PITCH_FONTTYPE_MASK; }
    int GetScript() const         { return m_cf.nFontType & SCRIPT_FONTTYPE_MASK; }
    BOOL IsFixedPitch() const     { return (m_cf.nFontType & FIXED_PITCH) != 0; }
    BOOL IsRegular() const        { return (m_cf.nFontType & REGULAR_FONTTYPE) != 0; }
    BOOL IsSymbol() const         { return (m_cf.nFontType & SYMBOL_FONTTYPE) != 0; }
    BOOL IsScreenFont() const     { return (m_cf.nFontType & SCREEN_FONTTYPE) != 0; }
    BOOL IsPrinterFont() const    { return (m_cf.nFontType & PRINTER_FONTTYPE) != 0; }
    COLORREF GetColor() const     { return m_cf.rgbColors; }
    virtual INT_PTR DoModal();

    LOGFONT m_lf;
    CHOOSEFONT m_cf;
};

// ---------------------------------------------------------------------
//  CPrintDialog
// ---------------------------------------------------------------------
class CPrintDialog : public CDialog
{
    DECLARE_DYNAMIC(CPrintDialog)
public:
    CPrintDialog(BOOL bPrintSetupOnly = TRUE,
                 DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES |
                                 PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION,
                 CWnd* pParentWnd = NULL);
    virtual ~CPrintDialog();

    virtual INT_PTR DoModal();
    BOOL GetDefaults();
    void AttachPrinter(HDC hDC, BOOL bHandles = FALSE);
    HDC GetPrinterDC() const;
    HDC CreatePrinterDC();
    int GetCopies() const;
    BOOL PrintSelection() const;
    BOOL PrintRange() const;
    BOOL PrintAll() const;
    int GetFromPage() const;
    int GetToPage() const;
    DEVMODE* GetDevMode() const;
    CString GetPortName() const;
    CString GetDriverName() const;
    CString GetDeviceName() const;
    BOOL DoPrintDialog(CDocument* pDoc, CView* pView = NULL);

    PRINTDLG m_pd;
protected:
    BOOL m_bHandleDC;
    DECLARE_MESSAGE_MAP()
};

// ---------------------------------------------------------------------
//  CPageSetupDialog
// ---------------------------------------------------------------------
class CPageSetupDialog : public CDialog
{
    DECLARE_DYNAMIC(CPageSetupDialog)
public:
    CPageSetupDialog(DWORD dwFlags = PSD_MARGINS | PSD_INWININIINTLMEASURE,
                     CWnd* pParentWnd = NULL);
    virtual INT_PTR DoModal();
    void GetMargins(LPRECT lpRectMargins, LPRECT lpRectMinMargins) const;
    CSize GetPaperSize() const;
    void GetDevMode(LPDEVMODE* ppDevMode) const;
    PAGESETUPDLG m_psd;
};

// ---------------------------------------------------------------------
//  CFindReplaceDialog (modeless)
// ---------------------------------------------------------------------
class CFindReplaceDialog : public CDialog
{
    DECLARE_DYNAMIC(CFindReplaceDialog)
public:
    CFindReplaceDialog();
    virtual ~CFindReplaceDialog();

    BOOL Create(BOOL bFindDialogOnly, LPCTSTR lpszFindWhat,
                LPCTSTR lpszReplaceWith = NULL, DWORD dwFlags = 0,
                CWnd* pParentWnd = NULL);
    static UINT GetFindReplaceMsg();
    static CFindReplaceDialog* PASCAL GetNotifier(LPARAM lParam);

    BOOL IsTerminating() const { return m_bTerminating; }
    CString GetFindString() const   { return m_fr.lpstrFindWhat; }
    CString GetReplaceString() const{ return m_fr.lpstrReplaceWith; }
    BOOL SearchDown() const     { return (m_fr.Flags & FR_DOWN) != 0; }
    BOOL FindNext() const       { return (m_fr.Flags & FR_FINDNEXT) != 0; }
    BOOL ReplaceCurrent() const { return (m_fr.Flags & FR_REPLACE) != 0; }
    BOOL ReplaceAll() const     { return (m_fr.Flags & FR_REPLACEALL) != 0; }
    BOOL MatchCase() const      { return (m_fr.Flags & FR_MATCHCASE) != 0; }
    BOOL MatchWholeWord() const { return (m_fr.Flags & FR_WHOLEWORD) != 0; }
    DWORD GetFlags() const      { return m_fr.Flags; }
    virtual void PostNcDestroy();

    FINDREPLACE m_fr;
protected:
    TCHAR m_szFindWhat[256];
    TCHAR m_szReplaceWith[256];
    BOOL  m_bTerminating;
    DECLARE_MESSAGE_MAP()
};

// ---------------------------------------------------------------------
//  CPropertyPage / CPropertySheet
// ---------------------------------------------------------------------
class CPropertyPage : public CDialog
{
    DECLARE_DYNAMIC(CPropertyPage)
public:
    CPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
    CPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
    CPropertyPage();
    virtual ~CPropertyPage();

    void Construct(UINT nIDTemplate, UINT nIDCaption = 0);
    void Construct(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);
    void SetModified(BOOL bChanged = TRUE);
    LRESULT Apply();
    LRESULT QuerySiblings(WPARAM wParam, LPARAM lParam);
    void CancelToClose();
    void SetWizardButtons(DWORD dwFlags);
    CPropertySheet* GetParentSheet() const { return m_pParentSheet; }

    virtual BOOL OnApply();
    virtual void OnReset();
    virtual BOOL OnQueryCancel();
    virtual BOOL OnWizardBack();
    virtual BOOL OnWizardFinish();
    virtual LRESULT OnWizardNext();
    virtual LRESULT OnWizardFinishEx();
    virtual BOOL OnSetActive();
    virtual BOOL OnKillActive();
    virtual void OnHelp();

    PROPSHEETPAGE m_psp;
protected:
    CPropertySheet* m_pParentSheet;
    void CommonConstruct(LPCTSTR lpszTemplateName, UINT nIDCaption);
    friend class CPropertySheet;
};

class CPropertySheet : public CWnd
{
    DECLARE_DYNAMIC(CPropertySheet)
public:
    CPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CPropertySheet();
    virtual ~CPropertySheet();

    void Construct(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    void Construct(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

    int AddPage(CPropertyPage* pPage);
    void RemovePage(CPropertyPage* pPage);
    void RemovePage(int nPage);
    int GetPageCount() const { return (int)m_pages.size(); }
    CPropertyPage* GetPage(int nPage) const;
    int GetActiveIndex() const;
    CPropertyPage* GetActivePage() const;
    void SetActivePage(int nPage);
    void SetActivePage(CPropertyPage* pPage);
    void SetTitle(LPCTSTR lpszText, UINT nStyle = 0);
    void SetWizardMode();
    void EnableApplyButton(BOOL bEnable = TRUE);
    void SetFinishText(LPCTSTR lpszText);
    void SetDefaultButton();
    void EnableHelp();
    INT_PTR DoModal();
    BOOL Create(CWnd* pParentWnd = NULL, DWORD dwStyle = (DWORD)-1, DWORD dwExStyle = 0);
    void EndDialog(int nEndID);
    HWND GetTabControl() const;
    void MapDialogRect(LPRECT lpRect) const;
    void Apply();
    void PressButton(int nButton);
    void SetPsHeader(LPCTSTR lpszText) { (void)lpszText; }

    HPROPSHEETPAGE* GetPageHandles();
    void BuildPropPageArray();

    DWORD m_dwFlags;
protected:
    void CommonConstruct(LPCTSTR pszCaption, UINT iSelectPage);
    std::vector<CPropertyPage*> m_pages;
    std::vector<HPROPSHEETPAGE> m_hPages;
    CString m_strCaption;
    int     m_iSelectPage;
    BOOL    m_bWizard;
    friend class CPropertyPage;
    friend int CALLBACK AfxPropSheetCallback(HWND, UINT, LPARAM);
};

#endif /* __AFXDLGS_H__ */
