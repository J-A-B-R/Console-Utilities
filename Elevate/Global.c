#include "stdafx.h"
#include "Global.h"


void SetUp()
{
    TCHAR* cmdLine;
    DWORD len;

    // Get the relevant part of the command line and copy it to
    // writable memory as required by CreateProcess
    cmdLine = SkipFirstCmdLineArg(GetCommandLine(), TRUE);
    if ((gCmdLine = StringAllocAndCopy(cmdLine)) == NULL)
        SYS_ERROR();

    gExiting = FALSE;

    len = GetCurrentDirectory(0, NULL);
    gCurrentDirectory = StringAlloc(len);
    GetCurrentDirectory(len + 1, gCurrentDirectory);

    gIpcPipe = INVALID_HANDLE_VALUE;
    gStdInPipe = INVALID_HANDLE_VALUE;
    gStdOutPipe = INVALID_HANDLE_VALUE;
    gStdErrPipe = INVALID_HANDLE_VALUE;
}

void CloseServerPipeSafe(HANDLE handle)
{
    if (handle != INVALID_HANDLE_VALUE) {
        DisconnectNamedPipe(handle);
        CloseHandle(handle);
    }
}

void CloseThreadOrProcessSafe(HANDLE handle)
{
    if (handle != NULL)
        CloseHandle(handle);
}

int CleanUp(int exitCode)
{
    HANDLE threads[3];
    DWORD threadCount = 0;
    DWORD threadExitCode;
    DWORD processExitCode;
    BOOL error = FALSE;
    UINT i;

    // Signal termination to the pipe redirection threads
    gExiting = TRUE;

    // Disconnect and close all existing pipes
    CloseServerPipeSafe(gIpcPipe);
    CloseServerPipeSafe(gStdInPipe);
    CloseServerPipeSafe(gStdOutPipe);
    CloseServerPipeSafe(gStdErrPipe);

    // Wait the termination of all pipe redirection threads and check the termination cause
    if (gStdInThread != NULL)
        threads[threadCount++] = gStdInThread;
    if (gStdOutThread != NULL)
        threads[threadCount++] = gStdOutThread;
    if (gStdErrThread != NULL)
        threads[threadCount++] = gStdErrThread;
    if (threadCount > 0)
        switch (WaitForMultipleObjects(threadCount, threads, TRUE, MAX_WAITING_TIME))
        {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED_0:
            for (i = 0; i < threadCount; i++) {
                if (GetExitCodeThread(threads[i], &threadExitCode) && threadExitCode != ERROR_SUCCESS) {
                    if (threadExitCode == ERROR_NO_DATA)
                        AppError(IDS_PIPE_CLOSED);
                    else {
                        SetLastError(threadExitCode);
                        SysError();
                    }
                    error = TRUE;
                }
            }
            break;
        case WAIT_FAILED:
            SysError();
            error = TRUE;
            break;
        case WAIT_TIMEOUT:
            // NADA
            break;
        };

    // Pass-through the stub process termination error
    if (gStubProcess != NULL && 
        WaitForSingleObject(gStubProcess, MAX_WAITING_TIME) != WAIT_FAILED && 
        GetExitCodeProcess(gStubProcess, &processExitCode) && 
        processExitCode != ERROR_SUCCESS) {
            SetLastError(processExitCode);
            SysError();
            error = TRUE;
    }

    if (error && exitCode == EXIT_SUCCESS)
        exitCode = EXIT_FAILURE;

    // Close the handles to pipe redirection threads and the target process
    CloseThreadOrProcessSafe(gStdInThread);
    CloseThreadOrProcessSafe(gStdOutThread);
    CloseThreadOrProcessSafe(gStdErrThread);
    CloseThreadOrProcessSafe(gStubProcess);
    CloseThreadOrProcessSafe(gTargetProcess);

    // Free allocated memory for strings
    StringFree(gCurrentDirectory);
    StringFree(gCmdLine);

    return exitCode;
}
