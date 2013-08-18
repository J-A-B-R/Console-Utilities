/* Copyright (c) 2013 Juan Burgos. All rights reserved.
 *
 * This source is subject to the Apache 2.0 License.
 * Please see the included LICENSE file for more information.
 * All other rights reserved.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "stdafx.h"
#include "Common.h"


int ToWord(TCHAR* lpszWord, WORD* lpnWord, BOOL full, int radix)
{
    TCHAR* ptr;

    *lpnWord = (WORD)_tcstoul(lpszWord, &ptr, radix);
    return (full && *ptr != 0) || errno != 0;
}

BOOL StartsWith(TCHAR* str, TCHAR* prefix)
{
    size_t lenPrefix = _tcslen(prefix);

    return (_tcslen(str) < lenPrefix) ? FALSE : _tcsncmp(str, prefix, lenPrefix) == 0;
}

BOOL IsVistaOrAbove()
{
    OSVERSIONINFO info;

    ZeroMemory(&info, sizeof(OSVERSIONINFO));
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    return GetVersionEx(&info) && info.dwPlatformId == VER_PLATFORM_WIN32_NT && info.dwMajorVersion >= 6;
}

BOOL IsElevated()
{
    HANDLE token;
    TOKEN_ELEVATION te;
    DWORD len = 0;
    BOOL result;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        SysError();

    result = GetTokenInformation(token, TokenElevation, &te, sizeof(TOKEN_ELEVATION), &len);
    CloseHandle(token);

    if (!result)
        SysError();

    return te.TokenIsElevated;
}
