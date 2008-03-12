//-----------------------------------------------------------------------------
//  nmaxdlg.cc
//
//  (c)2004 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include "kernel/ntypes.h"
#include "n3dsutils/nmaxdlg.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxDlg::nMaxDlg(WORD resID, HINSTANCE hInst, HWND hwndParent) :
    hWnd(0)
{
    this->resourceID = resID;
    this->hInstance  = hInst;
    this->hWndParent = hwndParent;
}

//-----------------------------------------------------------------------------
/**
*/
nMaxDlg::~nMaxDlg()
{
    if (this->hWnd)
    {
        Destroy();
    }
}

//-----------------------------------------------------------------------------
/**
    Create dialog.
*/
void nMaxDlg::Create()
{
    HWND hwnd = CreateDialogParam(this->hInstance,
                                  MAKEINTRESOURCE(this->resourceID),
                                  this->hWndParent,
                                  &DlgProc,
                                  (LPARAM)this);
    if (0 == hwnd)
    {
        DWORD dwError = GetLastError();
        dwError = dwError; // disable warning C4189: 'dwError' : local variable is initialized but not referenced
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    this->hWnd = hwnd;
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxDlg::Destroy()
{
    ShowWindow(this->hWnd, SW_HIDE);
    DestroyWindow(this->hWnd);
    this->hWnd = 0;
}

//-----------------------------------------------------------------------------
/**
    Create modal dialog.
*/
int nMaxDlg::DoModal()
{
    int result;    
    #ifdef _WIN64
        #error not implemented
    #else
    #pragma warning( push )
    #pragma warning( disable : 4244 ) //conversion from 'INT_PTR' to 'int', possible loss of data
    result = DialogBoxParam(this->hInstance, 
                            MAKEINTRESOURCE(this->resourceID),
                            this->hWnd, 
                            &DlgProc,
                            (LPARAM)this);
    #pragma warning( pop )
    #endif
    
    if (result < 0)
    {
        DWORD dwError = GetLastError();
        dwError = dwError; // dsiable warninf 'dwError' : local variable is initialized but not referenced
    }

    

    return result;
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxDlg::OnInitDialog()
{
    // empty.
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxDlg::OnClose()
{
    // empty.
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxDlg::OnDestroy()
{
    // empty.
}

//-----------------------------------------------------------------------------
/**
*/
BOOL nMaxDlg::OnCommand(int /*lowParam*/, int /*highParam*/, LPARAM /*lParam*/)
{
    return FALSE;
}

//-----------------------------------------------------------------------------
/**
*/
BOOL nMaxDlg::OnMessage(uint /*msg*/, int /*lowParam*/, int /*highParam*/, long /*lParam*/)
{
    return FALSE;
}

//-----------------------------------------------------------------------------
/**
    Dialog window procedure
*/
BOOL CALLBACK nMaxDlg::DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int id = LOWORD(wParam);
    int code = HIWORD(wParam);

    #ifdef _WIN64
        #error not implemented
    #else
    #pragma warning( push )
    #pragma warning( disable : 4312 ) //'type cast' : conversion from 'LONG' to 'nMaxDlg *' of greater size
    nMaxDlg* dlg = (nMaxDlg*)GetWindowLong(hwnd, GWL_USERDATA);
    //nMaxDlg* dlg = (nMaxDlg*)GetWindowLongPtr(hwnd, GWL_USERDATA);
    #endif
    

    switch(msg)
    {
    case WM_INITDIALOG:
        dlg = (nMaxDlg*)lParam;
        dlg->SetHWnd(hwnd);
        SetWindowLong(hwnd, GWL_USERDATA, (long)lParam);
        dlg->OnInitDialog();
        UpdateWindow(hwnd);
        DefWindowProc(hwnd, msg, wParam, lParam);
        break;

    case WM_COMMAND:
        if (dlg)
        {
            return dlg->OnCommand(id, code, lParam);
        }
        break;

    case WM_CLOSE:
        if (dlg)
        {
            dlg->OnClose();
        }
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_DESTROY:
        if (dlg)
        {
            dlg->OnDestroy();
        }
        EndDialog(hwnd, IDCANCEL);
        break;

    default:
        if (dlg)
        {
            return dlg->OnMessage(msg, id, code, lParam);
        }
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam) ? 1 : 0;
}
