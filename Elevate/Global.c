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

void CloseHandleSafe(HANDLE handle)
{
    if (handle != NULL && handle != INVALID_HANDLE_VALUE)
        CloseHandle(handle);
}

int CleanUp(int exitCode)
{
    DWORD processExitCode;
    BOOL error = FALSE;

    // Signal termination to the pipe redirection threads
    SignalHandlingRedirecionPipesForExiting();

    // Close all existing pipes
    // Some programs can get confused if we disconnect before closing
    CloseHandleSafe(gIpcPipe);
    CloseHandleSafe(gStdInPipe);
    CloseHandleSafe(gStdOutPipe);
    CloseHandleSafe(gStdErrPipe);

    // Wait the termination of all pipe redirection threads and check the termination cause
    error = WaitHandlingRedirecionPipesForExiting();

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

    // Close the handles to pipe redirection threads and to the target process
    CloseHandleSafe(gStdInThread);
    CloseHandleSafe(gStdOutThread);
    CloseHandleSafe(gStdErrThread);
    CloseHandleSafe(gStubProcess);
    CloseHandleSafe(gTargetProcess);

    // Free allocated memory for strings
    StringFree(gCurrentDirectory);
    StringFree(gCmdLine);

    return exitCode;
}
