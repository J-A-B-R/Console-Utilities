#include "stdafx.h"
#include "Global.h"


// Any fatal error terminates the target process. As the
// target process is created suspended and the very last
// operation of the stub is resuming it, then this isn't
// a dangerous move
#define KILL_TARGET_PROC(hProc)\
    if (hProc != NULL) {\
        TerminateProcess(hProc, 0);\
        WaitForSingleObject(hProc, MAX_WAITING_TIME);\
    }\

void SYS_ERROR()
{
    DWORD errorCode = GetLastError();

#ifdef _DEBUG
    SysError();
#endif
    KILL_TARGET_PROC(gTargetProcess);
    CleanUp();
    ExitProcess(errorCode);
}

void APP_ERROR(DWORD nMsgId)
{
    MessageBox(NULL, GetString(nMsgId, _T("???")), GetString(IDS_ERROR_CAPTION, _T("")), MB_OK | MB_ICONERROR);
    KILL_TARGET_PROC(gTargetProcess);
    CleanUp();
    ExitProcess(EXIT_FAILURE);
}

void SetUp()
{
    gIpcPipe = INVALID_HANDLE_VALUE;
    gStdInFile = INVALID_HANDLE_VALUE;
    gStdOutFile = INVALID_HANDLE_VALUE;
    gStdErrFile = INVALID_HANDLE_VALUE;
}

void CloseHandleSafe(HANDLE handle)
{
    if (handle != NULL && handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }
}

void CleanUp()
{
    CloseHandleSafe(gIpcPipe);
    CloseHandleSafe(gStdInFile);
    CloseHandleSafe(gStdOutFile);
    CloseHandleSafe(gStdErrFile);
    CloseHandleSafe(gTargetProcess);
    CloseHandleSafe(gTargetThread);

    StringFree(gCurrentDirectory);
    StringFree(gCmdLine);
    StringFree(gStdInFileName);
    StringFree(gStdOutFileName);
    StringFree(gStdErrFileName);
}
