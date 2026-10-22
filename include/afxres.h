// afxres.h -- MFC-compatible resource identifiers for the MFC-GCC clone.
// Values match the real MFC afxres.h so that MFC-style .rc scripts work.
#ifndef __AFXRES_H__
#define __AFXRES_H__
#pragma once

#ifdef RC_INVOKED
#include <windows.h>
#endif

/* Standard About box dialog id (used by CWinApp::OnAppAbout) */
#ifndef AFX_IDD_ABOUTBOX
#define AFX_IDD_ABOUTBOX              100
#endif

/* General style dialog control ID */
#ifndef IDC_STATIC
#define IDC_STATIC      (-1)
#endif

/* Standard commands */
#define ID_FILE_NEW                     0xE100
#define ID_FILE_OPEN                    0xE101
#define ID_FILE_CLOSE                   0xE102
#define ID_FILE_SAVE                    0xE103
#define ID_FILE_SAVE_AS                 0xE104
#define ID_FILE_PAGE_SETUP              0xE105
#define ID_FILE_PRINT                   0xE106
#define ID_FILE_PRINT_PREVIEW           0xE107
#define ID_FILE_PRINT_SETUP             0xE108
#define ID_FILE_MRU_FILE1               0xE110
#define ID_FILE_MRU_FILE2               0xE111
#define ID_FILE_MRU_FILE3               0xE112
#define ID_FILE_MRU_FILE4               0xE113
#define ID_FILE_MRU_FILE5               0xE114
#define ID_FILE_MRU_FILE6               0xE115
#define ID_FILE_MRU_FILE7               0xE116
#define ID_FILE_MRU_FILE8               0xE117
#define ID_FILE_MRU_FILE9               0xE118
#define ID_FILE_MRU_FILE10              0xE119
#define ID_FILE_MRU_FILE11              0xE11A
#define ID_FILE_MRU_FILE12              0xE11B
#define ID_FILE_MRU_FILE13              0xE11C
#define ID_FILE_MRU_FILE14              0xE11D
#define ID_FILE_MRU_FILE15              0xE11E
#define ID_FILE_MRU_FILE16              0xE11F

#define ID_EDIT_CLEAR                   0xE120
#define ID_EDIT_CLEAR_ALL               0xE121
#define ID_EDIT_COPY                    0xE122
#define ID_EDIT_CUT                     0xE123
#define ID_EDIT_FIND                    0xE124
#define ID_EDIT_PASTE                   0xE125
#define ID_EDIT_PASTE_LINK              0xE126
#define ID_EDIT_PASTE_SPECIAL           0xE127
#define ID_EDIT_REPEAT                  0xE128
#define ID_EDIT_REPLACE                 0xE129
#define ID_EDIT_SELECT_ALL              0xE12A
#define ID_EDIT_UNDO                    0xE12B
#define ID_EDIT_REDO                    0xE12C

#define ID_WINDOW_NEW                   0xE130
#define ID_WINDOW_ARRANGE               0xE131
#define ID_WINDOW_CASCADE               0xE132
#define ID_WINDOW_TILE_HORZ             0xE133
#define ID_WINDOW_TILE_VERT             0xE134
#define ID_WINDOW_SPLIT                 0xE135
#define ID_WINDOW_MRU_CHILD1            0xE136
#define ID_WINDOW_MRU_CHILD16           0xE13F

#define ID_APP_ABOUT                    0xE140
#define ID_APP_EXIT                     0xE141
#define ID_HELP_INDEX                   0xE142
#define ID_HELP_FINDER                  0xE143
#define ID_HELP_USING                   0xE144
#define ID_CONTEXT_HELP                 0xE145
#define ID_HELP                         0xE146
#define ID_DEFAULT_HELP                 0xE147

#define ID_NEXT_PANE                    0xE150
#define ID_PREV_PANE                    0xE151

#define ID_VIEW_TOOLBAR                 0xE800
#define ID_VIEW_STATUS_BAR              0xE801
#define ID_VIEW_REBAR                   0xE804

#define ID_RECORD_FIRST                 0xE900
#define ID_RECORD_LAST                  0xE901
#define ID_RECORD_NEXT                  0xE902
#define ID_RECORD_PREV                  0xE903

/* Standard string resources */
#define AFX_IDS_APP_TITLE               0xE000
#define AFX_IDS_IDLEMESSAGE             0xE001
#define AFX_IDS_MRU_FILE                0xE002

/* Document template strings (same layout as MFC: \n separated) */
#define AFX_IDS_DESKACCESSORY           0xF001

/* Message box / error prompt strings */
#define AFX_IDP_FIRST                   0xF100
#define AFX_IDP_INVALID_FILENAME        0xF101
#define AFX_IDP_FAILED_TO_OPEN_DOC      0xF102
#define AFX_IDP_FAILED_TO_SAVE_DOC      0xF103
#define AFX_IDP_ASK_TO_SAVE             0xF104
#define AFX_IDP_FAILED_TO_CREATE_DOC    0xF105
#define AFX_IDP_FILE_TOO_LARGE          0xF106
#define AFX_IDP_FAILED_NO_MESSAGES      0xF107
#define AFX_IDP_COMMAND_FAILURE         0xF108
#define AFX_IDP_PARSE_INT               0xF109
#define AFX_IDP_PARSE_REAL              0xF10A
#define AFX_IDP_PARSE_REAL_RANGE        0xF10B
#define AFX_IDP_PARSE_INT_RANGE         0xF10C
#define AFX_IDP_PARSE_DATETIME          0xF10D
#define AFX_IDP_PARSE_BYTE              0xF10E
#define AFX_IDP_PARSE_UINT              0xF10F
#define AFX_IDP_PARSE_CURRENCY          0xF110
#define AFX_IDP_FAILED_INVALID_FORMAT   0xF111
#define AFX_IDP_FAILED_INVALID_PATH     0xF112
#define AFX_IDP_FAILED_DISK_FULL        0xF113
#define AFX_IDP_FAILED_ACCESS_READ      0xF114
#define AFX_IDP_FAILED_ACCESS_WRITE     0xF115
#define AFX_IDP_FAILED_IO_ERROR_READ    0xF116
#define AFX_IDP_FAILED_IO_ERROR_WRITE   0xF117

/* Dialog control ids used by MFC standard dialogs */
#define AFX_IDC_CONTEXT_HELP            0x2801
#define AFX_IDB_CHECKLISTBOX_95         30996
#define AFX_IDC_LISTBOX                 31000
#define AFX_IDC_COMBO                   31001

/* Toolbar / status bar defaults */
#define AFX_IDW_TOOLBAR                 0xE800
#define AFX_IDW_STATUS_BAR              0xE801
#define AFX_IDW_PREVIEW_BAR             0xE802
#define AFX_IDW_RESIZE_BAR              0xE803
#define AFX_IDW_DOCKBAR_TOP             0xE804
#define AFX_IDW_DOCKBAR_LEFT            0xE805
#define AFX_IDW_DOCKBAR_RIGHT           0xE806
#define AFX_IDW_DOCKBAR_BOTTOM          0xE807
#define AFX_IDW_DOCKBAR_FLOAT           0xE808
#define AFX_IDW_REBAR                   0xE810

/* The first id for user commands */
#define ID_COMMAND_FIRST_USER           0x8000

#endif /* __AFXRES_H__ */
