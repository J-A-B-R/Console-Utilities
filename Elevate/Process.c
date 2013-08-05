#include "stdafx.h"
#include "Process.h"
#include "Global.h"
#include "Pipes.h"

//#define MAX_DWORD_STRZ_LENGTH 21
//#define BUFFER_LENGTH (MAX_DWORD_STRZ_LENGTH * 2)
//_ultot_s(pid, buffer, BUFFER_LENGTH, 10);

BOOL IsConsoleHandle(DWORD nStdHandle)
{
    return GetFileType(GetStdHandle(nStdHandle)) == FILE_TYPE_CHAR;
}

int ExecuteProgram()
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    TCHAR* cmdLine = SkipFirstCmdLineArg(GetCommandLine(), TRUE);
    DWORD result;

    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    if ((sWritableCmdLine = StringAllocAndCopy(cmdLine)) == NULL)
        SYS_ERROR();

    if (!CreateProcess(NULL, sWritableCmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
        SYS_ERROR();

    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &result);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return result;
}

HANDLE LaunchStub()
{
    SHELLEXECUTEINFO info;

    ZeroMemory(&info, sizeof(SHELLEXECUTEINFO));

    info.cbSize = sizeof(SHELLEXECUTEINFO);
    info.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
    info.lpVerb = _T("runas");
    info.lpFile = STUB_NAME;
    info.lpParameters = sIpcPipeName;
    info.nShow = SW_HIDE;

    if (!ShellExecuteEx(&info))
        SYS_ERROR();

    return info.hProcess;
}

DWORD WINAPI HandleIpcPipe(LPVOID lpParam)
{
    return (ConnectNamedPipe(hIpcPipe, NULL)) ? ERROR_SUCCESS : GetLastError();
}

void PerformIpcCommunication()
{
    LPWCH envBlock;
    DWORD error = ERROR_SUCCESS;
    DWORD bytes;
    int i;

    // communicate current directory and standard handles and environment block
    if (!SendString(sCurrentDirectory) || !SendString(sStdInPipeName) || !SendString(sStdOutPipeName) || !SendString(sStdErrPipeName))
        SYS_ERROR();

    // communicate current environment block
    envBlock = GetEnvironmentStrings();
    for (i = 0; *envBlock != 0 || *(envBlock + 1) != 0; i++);
    if (!SendData(envBlock, (i + 1) * sizeof(WCHAR)))
        error = GetLastError();
    FreeEnvironmentStrings(envBlock);
    if (error != ERROR_SUCCESS) {
        SetLastError(error);
        SYS_ERROR();
    }

    if (!FlushFileBuffers(hIpcPipe))
        SYS_ERROR();

    // receive process ID
    if (!ReceiveDword(&childProcessId))
        SYS_ERROR();

    // Send ACK
    if (!SendDword(childProcessId))
        SYS_ERROR();

    if (!FlushFileBuffers(hIpcPipe))
        SYS_ERROR();
}

HANDLE CreateIpcThread(LPTHREAD_START_ROUTINE lpStartAddress)
{
    DWORD threadId; 
    HANDLE hThread = CreateThread(NULL, 0, HandleIpcPipe, NULL, 0, &threadId);

    if (hThread == NULL)
        SYS_ERROR();

    return hThread;
}

BOOL EstablishStdHandles()
{
}

HANDLE StartIpcCommunication()
{
    DWORD threadId;

    hIpcThread = CreateThread(NULL, 0, HandleIpcPipe, NULL, 0, &threadId);

    if (hIpcThread == NULL)
        SYS_ERROR();
}

HANDLE WaitForIpcCommunication()
{
    DWORD exitCode;

    switch (WaitForSingleObject(hIpcThread, MAX_WAITING_TIME))
    {
    case WAIT_ABANDONED:
    case WAIT_TIMEOUT:
        APP_ERROR(xxx);
    case WAIT_OBJECT_0:
        if (!GetExitCodeThread(hIpcThread, &exitCode))
            SYS_ERROR();
        if (exitCode != ERROR_SUCCESS) {
            SetLastError(exitCode);
            SYS_ERROR()
        } 
        else
            return;
    default:
        SYS_ERROR();
    }
}


int ExecuteStub()
{
    int result = EXIT_SUCCESS;
    HANDLE ipcThread;

    //TODO: sign communication to avoid hijacking

    if (!EstablishStdHandles())
        SYS_ERROR();
   
    if ((hIpcPipe = CreateIpcPipe(sIpcPipeName)) == INVALID_HANDLE_VALUE)
        SYS_ERROR();

    // this thread avoids a race condition where the child process gets to opening de pipe before the parent process connects
    StartIpcCommunication();
    LaunchStub();
    WaitForIpcCommunication();
    StartRedirectionCommunication();
    PerformIpcCommunication();

    // wait for the target process to finish
    WaitForSingleObject();

    GetExitCodeProcess(
    // procesar respuesta
    // cerrar pipes de redirección si procede

    return result;
}
