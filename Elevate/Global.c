#include <stdafx.h>
#include "Global.h"

void SetUp()
{
    bExiting = FALSE;

    DWORD len = GetCurrentDirectory(0, NULL);
    
    sCurrentDirectory = StringAlloc(len);
    GetCurrentDirectory(len + 1, sCurrentDirectory);

    sWritableCmdLine = NULL;
    hIpcPipe = INVALID_HANDLE_VALUE;
    hStdInPipe = INVALID_HANDLE_VALUE;
    hStdOutPipe = INVALID_HANDLE_VALUE;
    hStdErrPipe = INVALID_HANDLE_VALUE;

    hIpcThread = NULL;
    hStdInThread = NULL;
    hStdOutThread = NULL;
    hStdErrThread = NULL;

    childProcessId = 0;
}

void ClosePipeSafe(LPHANDLE lpHandle)
{
    if (lpHandle != NULL && *lpHandle != INVALID_HANDLE_VALUE) {
        DisconnectNamedPipe(*lpHandle);
        CloseHandle(*lpHandle);
        *lpHandle = INVALID_HANDLE_VALUE;
    }
}

void CloseThreadSafe(LPHANDLE lpHandle)
{
    if (lpHandle != NULL && *lpHandle != NULL) {
        CloseHandle(*lpHandle);
        *lpHandle = INVALID_HANDLE_VALUE;
    }
}


void FreeStringSafe(TCHAR** str)
{
    if (str != NULL && *str != NULL) {
        StringFree(*str);
        *str = NULL;
    }
}

void CleanUp()
{
    bExiting = TRUE;

    ClosePipeSafe(&hIpcPipe);
    ClosePipeSafe(&hStdInPipe);
    ClosePipeSafe(&hStdOutPipe);
    ClosePipeSafe(&hStdErrPipe);

    WaitForMultipleObjects();

    CloseThreadSafe(&hIpcThread);
    CloseThreadSafe(&hStdInThread);
    CloseThreadSafe(&hStdOutThread);
    CloseThreadSafe(&hStdErrThread);

    FreeStringSafe(&sCurrentDirectory);
    FreeStringSafe(&sWritableCmdLine);

}
