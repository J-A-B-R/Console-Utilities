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
#include "Process.h"


// Check that the stub is launched in Vista or above and is elevated
void CheckSuitability()
{
    if (!IsVistaOrAbove())
        APP_ERROR(IDS_ONLY_VISTA_OR_ABOVE);
    if (!IsElevated())
        APP_ERROR(IDS_MUST_BE_ELEVATED);
}

// Start communication with the non-elevated launcher
void StartIpcCommunication(TCHAR* lpCmdLine)
{
    if (!StartsWith(lpCmdLine, PIPE_NAME_PREFIX))
        APP_ERROR(IDS_WRONG_PIPE_NAME);

    if (!WaitNamedPipe(lpCmdLine, NMPWAIT_USE_DEFAULT_WAIT))
        APP_ERROR(IDS_COMM_ERROR);

    if ((gIpcPipe = CreateFile(lpCmdLine, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE)
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
            APP_ERROR(IDS_COMM_ERROR);
        else
            SYS_ERROR();
}

// Get current directory, standard handles names and environment block
void ReceiveIpcData()
{
    if (!ReceiveDword(gIpcPipe, &gLauncherProcessId) ||
        !ReceiveString(gIpcPipe, &gCurrentDirectory) ||
        !ReceiveString(gIpcPipe, &gStdInFileName) ||
        !ReceiveString(gIpcPipe, &gStdOutFileName) ||
        !ReceiveString(gIpcPipe, &gStdErrFileName) ||
        !ReceiveString(gIpcPipe, &gCmdLine) ||
        !ReceiveData(gIpcPipe, &gEnvironmentBlock, &gEnvironmentBlockSize))
        SYS_ERROR();
}

// Open standard handle to console device or to redirection pipe
HANDLE OpenStdFile(TCHAR* stdFileName, DWORD desiredAccess)
{
    HANDLE handle;
    SECURITY_ATTRIBUTES sa;

    // if the console devices don't have read and write access, weird behavior ensues
    if (!_tcsicmp(stdFileName, CONSOLE_INPUT_FILE_NAME) || !_tcsicmp(stdFileName, CONSOLE_OUTPUT_FILE_NAME))
        desiredAccess = GENERIC_READ | GENERIC_WRITE;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if ((handle = CreateFile(
            stdFileName,
            desiredAccess,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            &sa,
            OPEN_EXISTING,
            0,
            NULL)
        ) == INVALID_HANDLE_VALUE)
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
            APP_ERROR(IDS_COMM_ERROR);
        else
            SYS_ERROR();

    return handle;
}

// Set standard handles, attach to parent console
void PrepareForLaunching()
{
    if (!AttachConsole(ATTACH_PARENT_PROCESS))
        AllocConsole();

    gStdInFile = OpenStdFile(gStdInFileName, GENERIC_READ);
    gStdOutFile = OpenStdFile(gStdOutFileName, GENERIC_WRITE);
    gStdErrFile = OpenStdFile(gStdErrFileName, GENERIC_WRITE);
}

// Create the target process in suspended state with the correct standard
// handles
void CreateTargetProcess()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
#ifdef UNICODE
    DWORD creationFlags = CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT;
#else
    DWORD creationFlags = CREATE_SUSPENDED;
#endif

    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    si.cb = sizeof(STARTUPINFO);
    si.hStdInput = gStdInFile;
    si.hStdOutput = gStdOutFile;
    si.hStdError = gStdErrFile;
    si.dwFlags = STARTF_USESTDHANDLES;

    if (!CreateProcess(NULL, gCmdLine, NULL, NULL, TRUE, creationFlags, gEnvironmentBlock, gCurrentDirectory, &si, &pi))
        SYS_ERROR();

    gTargetProcessId = pi.dwProcessId;
    gTargetProcess = pi.hProcess;
    gTargetThread = pi.hThread;
}

// Sends the target process ID and waits the acknowledgment
void FinishIpcCommunication()
{
    DWORD data;

    if (!SendDword(gIpcPipe, gTargetProcessId))
        SYS_ERROR();
    if (!FlushFileBuffers(gIpcPipe))
        SYS_ERROR();
    if (!ReceiveDword(gIpcPipe, &data))
        SYS_ERROR();
    if (gTargetProcessId != data)
        APP_ERROR(IDS_COMM_ERROR);
}

// Resumes the target process and waits its termination
void ResumeAndWaitForTargetProcess()
{
    if (!ResumeThread(gTargetThread))
        SYS_ERROR();

    // Waits for target process termination to avoid breaking the process tree
    // up to the launcher. Other than that, the stub has finished its job, so
    // further errors are irrelevant
    // Trying to create the target process with the launcher as parent process
    // through PROC_THREAD_ATTRIBUTE_PARENT_PROCESS attribute to get rid of the
    // stub process, results in the target process loosing its elevated status
    // and the breaking of any redirection pipes
    WaitForSingleObject(gTargetProcess, INFINITE);
}
