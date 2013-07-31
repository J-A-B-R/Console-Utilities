#include "stdafx.h"

#define IDR_HELP_TEXT 101
#define IDS_ERROR_FORMAT_STR 101
#define TRY_HELP(x) if (!(x)) return _tprintf(_T("No Help"));

#define RESERVED_MARKER 0x4A4142526A616272

#ifdef _DEBUG
void Alert(TCHAR* message)
{
    MessageBox(NULL, message, _T("Alerta"), MB_OK);
}
#endif

// Expects lpszCommandLine to be non null
TCHAR* SkipFirstCmdArg(TCHAR* lpszCommandLine, BOOL nCorrectExtraWhiteSpace)
{
    TCHAR k = *lpszCommandLine;
    BOOL quoted = FALSE;
    WORD i = 0;

    while (k != 0) {
        if (k == '"')
            quoted = !quoted;
        else if (k == ' ' && !quoted)
            // The check is necessary because when launched from visual studio, the
            // separation between the first and second argument is one white space,
            // but when launched from the console, the separation is two white spaces.
            // Of course, if the first character of the second argument is a white
            // space, it will be lost when launching from the visual studio debugger.
            return lpszCommandLine + i + ((nCorrectExtraWhiteSpace && *(lpszCommandLine + i + 1) == ' ') ? 2 : 1);

        k = *(lpszCommandLine + ++i);
    }

    return lpszCommandLine + i;
}

// Expects the resource string to be null terminated 
TCHAR* GetString(UINT nMsgId, TCHAR* lpszDefaultMsg)
{
    TCHAR* message;

    return (LoadString(0, nMsgId, (TCHAR*)&message, 0)) ? message : (lpszDefaultMsg) ? lpszDefaultMsg : _T(""); 
}

// Expects the resource to be a null terminated unicode text with or without BOM
int PrintUsage()
{
    HRSRC rc;
    HGLOBAL rcData;
    DWORD size;
    wchar_t* text;

    TRY_HELP(rc = FindResource(NULL, MAKEINTRESOURCE(IDR_HELP_TEXT), _T("HELP_TEXT")));
    TRY_HELP(rcData = LoadResource(NULL, rc));
    TRY_HELP(size = SizeofResource(NULL, rc));
    TRY_HELP(text = (wchar_t*)(LockResource(rcData)));
    TRY_HELP(text[(size - 1) / sizeof(wchar_t)] == 0);

    // printf functions doesn't like BOM
    if (*text == 0xfeff || *text == 0xfffe)
        text++;

    wprintf(text);
    return EXIT_SUCCESS;
}

void SysError() {
    TCHAR* errorMsg;
    DWORD error = GetLastError();

    // In the context of this suite of console programs, this error means
    // the standard streams are being redirected, so we ignore the error
    if (error == ERROR_INVALID_HANDLE)
        return;

    if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&errorMsg,
            0,
            NULL))
        errorMsg = _T("???");

#if defined(_DEBUG)
    MessageBox(NULL, errorMsg, _T("ERROR"), MB_OK);
#endif
    _ftprintf(stderr, GetString(IDS_ERROR_FORMAT_STR, _T("\nERROR: %s\n")) , errorMsg);
    exit(EXIT_FAILURE);
}

void AppError(UINT nMsgId)
{
    _ftprintf(stderr, GetString(IDS_ERROR_FORMAT_STR, _T("\nERROR: %s\n")) , GetString(nMsgId, _T("???")));
    exit(EXIT_FAILURE);
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

int ToWord(TCHAR* lpszWord, WORD* lpnWord, BOOL full, int radix)
{
    TCHAR* ptr;

    *lpnWord = (WORD)_tcstoul(lpszWord, &ptr, radix);
    return (full && *ptr != 0) || errno != 0;
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

int RedirectStdFileToStdHandle(DWORD nStdHandle)
{
    int hConHandle;
    HANDLE lStdHandle;
    FILE* fp;
    TCHAR* mode;

    switch (nStdHandle)
    {
        case STD_OUTPUT_HANDLE:
        case STD_ERROR_HANDLE:
            mode = _T("w");
            break;
        case STD_INPUT_HANDLE:
            mode = _T("r");
        default:
            return 1;
    }

    if ((lStdHandle = GetStdHandle(nStdHandle)) == INVALID_HANDLE_VALUE)
        return 3;
    if ((hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT)) == -1)
        return 3;
    if ((fp = _tfdopen(hConHandle, mode)) == NULL)
        return 4;
    switch (nStdHandle)
    {
        case STD_OUTPUT_HANDLE:
            *stdout = *fp;
            break;
        case STD_INPUT_HANDLE:
            *stdin = *fp;
            break;
        case STD_ERROR_HANDLE:
            *stderr = *fp;
    }
    setvbuf(fp, NULL, _IONBF, 0);
    return 0;
}
