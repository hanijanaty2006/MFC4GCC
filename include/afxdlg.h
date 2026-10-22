// =====================================================================
//  afxdlg.h  --  CDialog + DDX/DDV data exchange for the MFC-GCC clone
// =====================================================================
#ifndef __AFXDLG_H__
#define __AFXDLG_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif
#include "afxwnd.h"

// ---------------------------------------------------------------------
//  CDataExchange -- the "pDX" used by DoDataExchange()
// ---------------------------------------------------------------------
class CDataExchange
{
public:
    CDataExchange(CWnd* pDlg, BOOL bSaveAndValidate);

    // operations
    BOOL m_bSaveAndValidate;   // TRUE => validate and load from the controls
    CWnd* m_pDlgWnd;           // container usually a CDialog

    // helpers (do failure detection)
    HWND PrepareCtrl(int nIDC);
    HWND PrepareEditCtrl(int nIDC);
    void Fail();

    HWND m_hWndLastControl;    // last control used
    BOOL m_bEditLastControl;   // last control was an edit control
};

#define AfxDataExchange(pDX, dlg, save) CDataExchange pDX(dlg, save)

// ---------------------------------------------------------------------
//  CDialog
// ---------------------------------------------------------------------
class CDialog : public CWnd
{
    DECLARE_DYNAMIC(CDialog)
public:
    CDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
    CDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
    CDialog();
    virtual ~CDialog();

    // overridables
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    virtual void PostNcDestroy();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL CheckAutoCenter();

    // operations
    virtual INT_PTR DoModal();
    virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
    virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
    virtual BOOL CreateIndirect(LPCDLGTEMPLATE lpDialogTemplate, CWnd* pParentWnd = NULL,
                                void* lpInit = NULL);
    void EndDialog(INT_PTR nResult);
    void MapDialogRect(LPRECT lpRect) const;
    void NextDlgCtrl() const;
    void PrevDlgCtrl() const;
    int  GetModalResult() const { return m_nModalResult; }
    void SetModalResult(INT_PTR nResult) { m_nModalResult = (int)nResult; }
    UINT GetHelpID() const { return m_nIDHelp; }
    void SetHelpID(UINT nIDR) { m_nIDHelp = nIDR; }

    // implementation
    LPCTSTR m_lpszTemplateName;
    UINT    m_nIDTemplate;
    HGLOBAL m_hDialogTemplate;
    LPCDLGTEMPLATE m_lpDialogTemplate;
    void*   m_lpDialogInit;
    CWnd*   m_pParentWnd;
    UINT    m_nIDHelp;
    int     m_nModalResult;
    BOOL    m_bModal;

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

    afx_msg LRESULT HandleInitDialog(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT HandleSetFont(WPARAM wParam, LPARAM lParam);
    afx_msg void OnClose();
    afx_msg HBRUSH OnCtlColorDlg(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg LRESULT OnNcHitTestDlg(CPoint point);
    afx_msg void OnSysCommandDlg(UINT nID, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};

INT_PTR CALLBACK AfxDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// ---------------------------------------------------------------------
//  DDX_ / DDV_ routines
// ---------------------------------------------------------------------
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, BYTE& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, short& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, int& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, UINT& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, long& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, DWORD& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, LONGLONG& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, float& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, double& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, CString& value);

void AFXAPI DDX_Check(CDataExchange* pDX, int nIDC, int& value);
void AFXAPI DDX_Check(CDataExchange* pDX, int nIDC, bool& value);
void AFXAPI DDX_Radio(CDataExchange* pDX, int nIDC, int& value);

void AFXAPI DDX_CBString(CDataExchange* pDX, int nIDC, CString& value);
void AFXAPI DDX_CBStringExact(CDataExchange* pDX, int nIDC, CString& value);
void AFXAPI DDX_CBIndex(CDataExchange* pDX, int nIDC, int& index);
int  AFXAPI DDX_CBError(CDataExchange* pDX, int nIDC, UINT nIDCErrorString);
void AFXAPI DDX_LBString(CDataExchange* pDX, int nIDC, CString& value);
void AFXAPI DDX_LBStringExact(CDataExchange* pDX, int nIDC, CString& value);
void AFXAPI DDX_LBIndex(CDataExchange* pDX, int nIDC, int& index);
int  AFXAPI DDX_LBError(CDataExchange* pDX, int nIDC, UINT nIDCErrorString);

void AFXAPI DDX_Control(CDataExchange* pDX, int nIDC, CWnd& rControl);

void AFXAPI DDV_MinMaxByte(CDataExchange* pDX, BYTE const& value, BYTE minVal, BYTE maxVal);
void AFXAPI DDV_MinMaxShort(CDataExchange* pDX, short const& value, short minVal, short maxVal);
void AFXAPI DDV_MinMaxInt(CDataExchange* pDX, int const& value, int minVal, int maxVal);
void AFXAPI DDV_MinMaxUInt(CDataExchange* pDX, UINT const& value, UINT minVal, UINT maxVal);
void AFXAPI DDV_MinMaxLong(CDataExchange* pDX, long const& value, long minVal, long maxVal);
void AFXAPI DDV_MinMaxFloat(CDataExchange* pDX, float const& value, float minVal, float maxVal);
void AFXAPI DDV_MinMaxDouble(CDataExchange* pDX, double const& value, double minVal, double maxVal);
void AFXAPI DDV_MaxChars(CDataExchange* pDX, CString const& value, int nChars);

// runs CWnd::DoDataExchange() with validation + error reporting
BOOL AFXAPI AfxDoDataExchange(CWnd* pWnd, BOOL bSaveAndValidate);

// helper used by the DDX routines
void AFXAPI AfxSetWindowText(HWND hWndCtrl, LPCTSTR lpszText);
int  AFXAPI AfxGetWindowText(HWND hWndCtrl, LPTSTR lpszText, int nMaxLen);
BOOL AFXAPI AfxDlgDataExchangeError(CDataExchange* pDX, UINT nIDC, UINT nIDS);

#endif /* __AFXDLG_H__ */
