#include "stdafx.h"
#include "Common.h"


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

BOOL SetCursorPosition(SHORT x, SHORT y, BOOL xRel, BOOL yRel)
{
    HANDLE handle;
    CONSOLE_SCREEN_BUFFER_INFO info;
    COORD coord;

    handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (handle == INVALID_HANDLE_VALUE)
        return FALSE;
    
    if (xRel || yRel) {
        if (!GetConsoleScreenBufferInfo(handle, &info))
            return FALSE;
        if (xRel)
            x = SafeAddShorts(info.dwCursorPosition.X, x);
        if (yRel)
            y = SafeAddShorts(info.dwCursorPosition.Y, y);
    }

    coord.X = (x >= 0) ? x : 0;
    coord.Y = (y >= 0) ? y : 0;

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