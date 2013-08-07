#include "stdafx.h"
#include "Global.h"
#include "Process.h"


void CheckSuitability()
{
    if (!IsVistaOrAbove())
        APP_ERROR(IDS_ONLY_VISTA_OR_ABOVE);
    if (!IsElevated())
        APP_ERROR(IDS_MUST_BE_ELEVATED);
}

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
    if (!ReceiveString(gIpcPipe, &gCurrentDirectory) ||
        !ReceiveString(gIpcPipe, &gStdInFileName) ||
        !ReceiveString(gIpcPipe, &gStdOutFileName) ||
        !ReceiveString(gIpcPipe, &gStdErrFileName) ||
        !ReceiveString(gIpcPipe, &gCmdLine) ||
        !ReceiveData(gIpcPipe, &gEnvironmentBlock, &gEnvironmentBlockSize))
        SYS_ERROR();
}

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

void ResumeTargetProcess()
{
    if (!ResumeThread(gTargetThread))
        SYS_ERROR();
}
