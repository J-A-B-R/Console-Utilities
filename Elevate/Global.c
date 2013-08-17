#include "stdafx.h"
#include "Global.h"
#include "Pipes.h"


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
    DWORD processExitCode;
    BOOL error = FALSE;

    // Signal termination to the pipe redirection threads
    SignalHandlingRedirecionPipeEnding();

    // Disconnect and close all existing pipes
    CloseServerPipeSafe(gIpcPipe);
    CloseServerPipeSafe(gStdInPipe);
    CloseServerPipeSafe(gStdOutPipe);
    CloseServerPipeSafe(gStdErrPipe);

    // Wait the termination of all pipe redirection threads and check the termination cause
    error = WaitHandlingRedirecionPipeEnding();

    // Pass-through the stub process termination error
    if (gStubProcess != NULL && 
        WaitForSingleObject(gStubProcess, MAX_WAITING_TIME) != WAIT_FAILED && 
        GetExitCodeProcess(gStubProcess, &processExitCode) && 
        processExitCode != ERROR_SUCCESS) {
            SetLastError(processExitCode);
            SysError();
            error = TRUE;
    }

    // if this program or the stub had errors and the target process returned
    // a success exit code, then exit with failure
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
