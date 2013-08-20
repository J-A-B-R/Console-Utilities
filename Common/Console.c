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


BOOL IsConsoleHandle(HANDLE hHandle)
{
    DWORD lastError = GetLastError();
    DWORD mode;
    BOOL result = GetConsoleMode(hHandle, &mode);

    SetLastError(lastError);
    return result;
}

WORD MergeAttributes(TCHAR* lpszNewAttrs, WORD nCurrentAttrs)
{
    WORD attrs;

    if (*lpszNewAttrs == '.') {
        return (_tcslen(lpszNewAttrs) == 2 && !ToWord(lpszNewAttrs + 1, &attrs, 1, 16))
            ? (nCurrentAttrs & 0xf0) | attrs
            : ATTRIBUTE_ERROR;
    } 
    else if (*(lpszNewAttrs + 1) == '.') {
        return (_tcslen(lpszNewAttrs) == 2 && !ToWord(lpszNewAttrs, &attrs, 0, 16))
            ? (nCurrentAttrs & 0xf) | (attrs << 4)
            : ATTRIBUTE_ERROR;
    }
    else
    {
        return (!ToWord(lpszNewAttrs, &attrs, 1, 16) && attrs <= 0xff)
            ? attrs
            : ATTRIBUTE_ERROR;
    }
}

SHORT SafeAddShorts(SHORT a, SHORT b)
{
    INT tmp = a + b;
    return (tmp < SHRT_MIN) ? SHRT_MIN : (tmp > SHRT_MAX) ? SHRT_MAX : (SHORT)tmp;
}

BOOL SetCursorPosition(SHORT x, SHORT y, cursorCoordType_t xRel, cursorCoordType_t yRel)
{
    HANDLE handle;
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    COORD coord;

    handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (handle == INVALID_HANDLE_VALUE)
        return FALSE;

    info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    if (!GetConsoleScreenBufferInfoEx(handle, &info))
        return FALSE;

    if (xRel == PositionRelative)
        x = SafeAddShorts(info.dwCursorPosition.X, x);
    else if (xRel == LeftTopRelative)
        x = SafeAddShorts(info.srWindow.Left, x);
    else if (xRel == RightBottomRelative)
        x = SafeAddShorts(info.srWindow.Right, x);

    if (yRel == PositionRelative)
        y = SafeAddShorts(info.dwCursorPosition.Y, y);
    else if (yRel == LeftTopRelative)
        y = SafeAddShorts(info.srWindow.Top, y);
    else if (yRel == RightBottomRelative)
        y = SafeAddShorts(info.srWindow.Bottom, y);

    coord.X = (x < 0) ? 0 : (x > info.dwSize.X) ? info.dwSize.X : x;
    coord.Y = (y < 0) ? 0 : (y > info.dwSize.Y) ? info.dwSize.Y : y;

    return SetConsoleCursorPosition(handle, coord);
}

WORD GetAttributes()
{
    HANDLE handle;
    CONSOLE_SCREEN_BUFFER_INFO info;

    handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (handle == INVALID_HANDLE_VALUE)
        return ATTRIBUTE_ERROR;

    return (GetConsoleScreenBufferInfo(handle, &info)) ? info.wAttributes : ATTRIBUTE_ERROR;
}

BOOL SetAttributes(WORD attr)
{
    HANDLE handle;

    handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (handle == INVALID_HANDLE_VALUE)
        return FALSE;

    return SetConsoleTextAttribute(handle, attr);
}
