/* Copyright 2013 Juan Burgos. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
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
