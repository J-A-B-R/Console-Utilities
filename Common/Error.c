#include "stdafx.h"
#include "Common.h"


#define IDS_ERROR_FORMAT_STR 101
#define UNKNOWN_ERROR_MESSAGE _T("???")


#ifdef _DEBUG
TCHAR ProcessName[MAX_PATH + 1];

TCHAR* GetProcessName()
{
    TCHAR *buffer;
    TCHAR *name;

    if (*ProcessName == 0) {
        buffer = StringAlloc(MAX_PATH);
        GetModuleFileName(NULL, buffer, MAX_PATH + 1);
        if ((name = _tcsrchr(buffer, '\\')) != NULL)
            name++;
        else
            name = buffer;
        _tcscpy_s(ProcessName, MAX_PATH + 1, name);
        StringFree(buffer);
    }
    return ProcessName;
}

void Alert(TCHAR* message)
{
    TCHAR caption[MAX_PATH + 1];
    _stprintf_s(caption, MAX_PATH + 1, _T("ALERT %s"), GetProcessName());
    MessageBox(NULL, message, caption, MB_OK | MB_ICONWARNING);
}
#endif

TCHAR* GetSystemErrorMessage(DWORD dwMessageId)
{
    TCHAR* errorMsg;

    return (FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            dwMessageId,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&errorMsg,
            0,
            NULL))
        ? errorMsg
        : NULL;
}

TCHAR* GetCustomErrorMessage(DWORD dwMessageId)
{
    TCHAR* errorMsg = (TCHAR*)LocalAlloc(LPTR, sizeof(TCHAR) * 11);

    if (errorMsg == NULL)
        return NULL;

    _stprintf_s(errorMsg, 11, _T("[%X]"), dwMessageId);
    return errorMsg;
}

void SysError() {
    DWORD error = GetLastError();
    TCHAR* errorMsg;
    BOOL hasMsg = TRUE;
#ifdef _DEBUG
    TCHAR* caption;
#endif

    if (error == ERROR_SUCCESS)
        return;

    errorMsg = (IS_CUSTOM_ERROR_CODE(error))
        ? GetCustomErrorMessage(error)
        : GetSystemErrorMessage(error);

    if (errorMsg == NULL) {
        errorMsg = _T("???");
        hasMsg = FALSE;
    }

#ifdef _DEBUG
    caption = StringAlloc(MAX_PATH);
    _stprintf_s(caption, MAX_PATH + 1, _T("ERROR %s"), GetProcessName());
    MessageBox(NULL, errorMsg, caption, MB_OK | MB_ICONERROR);
    StringFree(caption);
#endif
    _ftprintf(stderr, GetString(IDS_ERROR_FORMAT_STR, _T("\nERROR: %s\n")) , errorMsg);

    if (hasMsg)
        LocalFree(errorMsg);
}

void ExitSysError()
{
    SysError();
    exit(EXIT_FAILURE);
}

void AppError(UINT nMsgId)
{
    _ftprintf(stderr, GetString(IDS_ERROR_FORMAT_STR, _T("\nERROR: %s\n")) , GetString(nMsgId, _T("???")));
}

void ExitAppError(UINT nMsgId)
{
    AppError(nMsgId);
    exit(EXIT_FAILURE);
}
