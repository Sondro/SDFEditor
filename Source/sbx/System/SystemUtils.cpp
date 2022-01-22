/*
 * @file    SystemUtils.h
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "SystemUtils.h"

#include <sbx/FileSystem/FileTypes.h>
#include <sbx/System/CApplication.h>

#include <windows.h>
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC

namespace
{
    // callback function
    INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
    {
        if (uMsg==BFFM_INITIALIZED) SendMessage(hwnd, BFFM_SETSELECTION, TRUE, pData);
        return 0;
    }
}

namespace sbx { namespace sys
{
    bool OpenFileDialog(char* aOutFilePath, uint32_t aMaxLen,  EFileType aType)
    {
        aOutFilePath[0] = '\0';

        //make sure this is commented out in all code (usually stdafx.h)
        // #define WIN32_LEAN_AND_MEAN 

        char lCurrentDir[MAX_PATH];
#ifdef _DEBUG
        GetCurrentDir(lCurrentDir, MAX_PATH);
#else
        GetDocumentsPath(lCurrentDir, MAX_PATH);
#endif

        OPENFILENAME ofn;       // common dialog box structure

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = (HWND)CApplication::GetInstance()->GetNativeHandle();
        ofn.lpstrFile = aOutFilePath;
        ofn.nMaxFile = aMaxLen;
        ofn.lpstrFilter = GetFileTypeFilter(aType);
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = lCurrentDir;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        //ofn.FlagsEx= OFN_EX_NOPLACESBAR;
        
        if (GetOpenFileName(&ofn) == TRUE)
        {
            // use ofn.lpstrFile
            SBX_LOG("Opening File %s", ofn.lpstrFile);
            return  true;
        }

        return false;
    }

    bool SaveFileDialog(char* aOutFilePath, uint32_t aMaxLen, EFileType aType)
    {
        aOutFilePath[0] = '\0';
        //make sure this is commented out in all code (usually stdafx.h)
        // #define WIN32_LEAN_AND_MEAN 

        char lCurrentDir[MAX_PATH];
#ifdef _DEBUG
        GetCurrentDir(lCurrentDir, MAX_PATH);
#else
        GetDocumentsPath(lCurrentDir, MAX_PATH);
#endif

        OPENFILENAME ofn;       // common dialog box structure

        // Initialize OPENFILENAME 
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = (HWND)CApplication::GetInstance()->GetNativeHandle();
        ofn.lpstrFile = aOutFilePath;
        ofn.nMaxFile = aMaxLen;
        ofn.lpstrFilter = GetFileTypeFilter(aType);
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = lCurrentDir;
        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
        //ofn.FlagsEx= OFN_EX_NOPLACESBAR;

        if (GetSaveFileName(&ofn) == TRUE)
        {
            // use ofn.lpstrFile
            SBX_LOG("Saving to File %s", ofn.lpstrFile);
            return  true;
        }

        return false;
    }

    bool BrowsePathDialog(char* aOutPath, uint32_t aMaxLen)
    {
        BROWSEINFO br;
        ZeroMemory(&br, sizeof(BROWSEINFO));
        br.lpfn = BrowseCallbackProc;
        br.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
        br.hwndOwner = (HWND)CApplication::GetInstance()->GetNativeHandle();
        //br.lpszTitle = "Select Path";
        br.lParam = (LPARAM)aOutPath;

        LPITEMIDLIST pidl = NULL;
        if ((pidl = SHBrowseForFolder(&br)) != NULL)
        {
            if (SHGetPathFromIDList(pidl, aOutPath))
            {
                return true;
            }
        }

        return false;
    }

    void GetCurrentDir(char* aOutPath, size_t aMaxLen)
    {
        GetCurrentDirectoryA((DWORD)aMaxLen, aOutPath);
    }

    void GetDocumentsPath(char* aOutPath, size_t aMaxLen)
    {
        HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, aOutPath);

        if (result != S_OK)
        {
            SBX_ERROR("Error getting Documents path");
        }        
    }
}}; //ngr::sys