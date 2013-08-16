#include "stdafx.h"
#include "Common.h"


#define WRITE_PIPE(h, d, s, w, o) ((WriteFile(h, d, s, w, o)) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED) ? (SetLastError(ERROR_SUCCESS), TRUE) : FALSE)
#define READ_PIPE(h, d, s, r, o) ((ReadFile(h, d, s, r, o)) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED) ? (SetLastError(ERROR_SUCCESS), TRUE) : FALSE)

BOOL SendDword(HANDLE hPipe, DWORD data)
{
    DWORD written;
    BOOL result = WriteFile(hPipe, &data, sizeof(DWORD), &written, NULL);
    return result;
}

BOOL SendData(HANDLE hPipe, LPVOID lpData, DWORD dataSize)
{
    DWORD written;
    BOOL result;

    if (SendDword(hPipe, dataSize))
        result = WriteFile(hPipe, lpData, dataSize, &written, NULL);
    else
        result = FALSE;
    return result;
}

BOOL SendString(HANDLE hPipe, TCHAR* str)
{
    BOOL result = SendData(hPipe, str, (DWORD)(_tcslen(str) + 1) * sizeof(TCHAR));
    return result;
}

BOOL ReceiveDword(HANDLE hPipe, LPDWORD lpData)
{
    DWORD read;

    return ReadFile(hPipe, lpData, sizeof(DWORD), &read, NULL);
}

BOOL ReceiveData(HANDLE hPipe, LPVOID* lppData, LPDWORD dataSize)
{
    DWORD read;
    DWORD size;
    LPVOID lpData;
    BOOL result;

    if (!ReceiveDword(hPipe, &size))
        return FALSE;

    lpData = MemoryAlloc(size, 1, FALSE);
    
    if (lpData == NULL)
        return FALSE;

    result = ReadFile(hPipe, lpData, size, &read, NULL);

    if (result && read != size)
    {
        result = FALSE;
        SetLastError(ERROR_PIPE_DATA_SIZE_MISMATCH);
    }

    if (!result) {
        MemoryFree(lpData);
        return FALSE;
    }

    *dataSize = size;
    *lppData = lpData;
    return TRUE;
}

BOOL ReceiveString(HANDLE hPipe, TCHAR** lpStr)
{
    DWORD read;
    TCHAR* str;
    BOOL result = ReceiveData(hPipe, (LPVOID*)&str, &read);

    if (result && *(str + (read / sizeof(TCHAR)) - 1) != 0) {
        SetLastError(ERROR_PIPE_NOT_NULL_TERMINATED_STRING);
        MemoryFree(str);
        return FALSE;
    }

    *lpStr = str;
    return result;
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
//    ios::sync_with_stdio()
    return 0;
}
