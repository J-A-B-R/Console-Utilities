#include "stdafx.h"
#include "Process.h"
#include "Global.h"
#include "Pipes.h"


int ExecuteProgram()
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    DWORD result;

    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    if (!CreateProcess(NULL, gCmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
        SYS_ERROR();

    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &result);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return result;
}

void SetIpcPipe()
{
    if ((gIpcPipe = CreateIpcPipe(gIpcPipeName)) == INVALID_HANDLE_VALUE)
        SYS_ERROR();
}

HANDLE EstablishStdHandle(TCHAR* sNameBuffer, TCHAR* sConsoleName, DWORD nStdHandle)
{
    HANDLE h = GetStdHandle(nStdHandle);

    if (h == INVALID_HANDLE_VALUE || GetFileType(h) == FILE_TYPE_CHAR) {
        _tcscpy_s(sNameBuffer, CONSOLE_FILE_NAME_MAX_LENGTH + 1, sConsoleName);
        return INVALID_HANDLE_VALUE;
    }

    return CreateRedirectionPipe(sNameBuffer, nStdHandle);
}

void EstablishStdHandles()
{
    gStdInPipe = EstablishStdHandle(gStdInPipeName, CONSOLE_INPUT_FILE_NAME, STD_INPUT_HANDLE);
    gStdOutPipe = EstablishStdHandle(gStdOutPipeName, CONSOLE_OUTPUT_FILE_NAME, STD_OUTPUT_HANDLE);
    gStdErrPipe = EstablishStdHandle(gStdErrPipeName, CONSOLE_OUTPUT_FILE_NAME, STD_ERROR_HANDLE);
}

void LaunchStub()
{
    SHELLEXECUTEINFO info;

    ZeroMemory(&info, sizeof(SHELLEXECUTEINFO));

    info.cbSize = sizeof(SHELLEXECUTEINFO);
    info.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
    info.lpVerb = _T("runas");
    info.lpFile = STUB_NAME;
    info.lpParameters = gIpcPipeName;
    info.nShow = SW_HIDE;

    if (!ShellExecuteEx(&info))
        SYS_ERROR();

    gStubProcess = info.hProcess;
}

void StartIpcCommunication()
{
    if (!ConnectNamedPipe(gIpcPipe, NULL) && GetLastError() != ERROR_PIPE_CONNECTED)
        SYS_ERROR();
}

void StartRedirectionCommunication()
{
    if (gStdInPipe != INVALID_HANDLE_VALUE)
        gStdInThread = StartHandlingRedirectionPipe(gStdInPipe, STD_INPUT_HANDLE);

    if (gStdOutPipe != INVALID_HANDLE_VALUE)
        gStdOutThread = StartHandlingRedirectionPipe(gStdOutPipe, STD_OUTPUT_HANDLE);

    if (gStdErrPipe != INVALID_HANDLE_VALUE)
        gStdErrThread = StartHandlingRedirectionPipe(gStdErrPipe, STD_ERROR_HANDLE);
}

#define IPC_SYS_ERROR() { if (GetLastError() != ERROR_BROKEN_PIPE) SysError(); exit(CleanUp(EXIT_FAILURE)); }

void PerformIpcCommunication()
{
    TCHAR* envBlock;
    DWORD error = ERROR_SUCCESS;
    DWORD targetProcessId;
    int i;

    // communicate process id, current directory, standard handles names and command line
    if (!SendDword(gIpcPipe, GetCurrentProcessId()) ||
        !SendString(gIpcPipe, gCurrentDirectory) ||
        !SendString(gIpcPipe, gStdInPipeName) ||
        !SendString(gIpcPipe, gStdOutPipeName) ||
        !SendString(gIpcPipe, gStdErrPipeName) ||
        !SendString(gIpcPipe, gCmdLine))
        IPC_SYS_ERROR();

    // communicate current environment block
    envBlock = GetEnvironmentStrings();
    for (i = 0; *(envBlock + i) != 0 || *(envBlock + i + 1) != 0; i++);
    if (!SendData(gIpcPipe, envBlock, (i + 2) * sizeof(TCHAR)))
        error = GetLastError();
    FreeEnvironmentStrings(envBlock);
    if (error != ERROR_SUCCESS) {
        SetLastError(error);
        IPC_SYS_ERROR();
    }

    if (!FlushFileBuffers(gIpcPipe))
        IPC_SYS_ERROR();

    // receive target process id
    if (!ReceiveDword(gIpcPipe, &targetProcessId))
        IPC_SYS_ERROR();

    // we get a handle to the target process before the stub resumes it so
    // we avoid the chance the process terminates before we get its handle
    if ((gTargetProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, targetProcessId)) == NULL)
        IPC_SYS_ERROR();

    // Reply with the received target process id to let the stub to resume
    // the target process
    if (!SendDword(gIpcPipe, targetProcessId))
        IPC_SYS_ERROR();

    if (!FlushFileBuffers(gIpcPipe))
        IPC_SYS_ERROR();

    // Finish communication with stub
    if (!DisconnectNamedPipe(gIpcPipe))
        IPC_SYS_ERROR();
}

DWORD WaitForTargetProcessTermination()
{
    DWORD result;

    if (WaitForSingleObject(gTargetProcess, INFINITE) == WAIT_FAILED)
        SYS_ERROR();

    if (!GetExitCodeProcess(gTargetProcess, &result))
        SYS_ERROR();

    return result;
}

int ExecuteStub()
{
    SetIpcPipe();   
    EstablishStdHandles();
    LaunchStub();
    StartIpcCommunication();
    StartRedirectionCommunication();
    PerformIpcCommunication();
    return WaitForTargetProcessTermination();
}
