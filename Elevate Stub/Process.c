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
    else
        gRedirectionPresent = TRUE;

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

// Creates an attribute list for attribute parent process
PPROC_THREAD_ATTRIBUTE_LIST PrepareAttributeList()
{
    HANDLE process;
    SIZE_T size = 0;
    PPROC_THREAD_ATTRIBUTE_LIST attrList;

    // Reparenting the target process brakes the redirection pipes
    if (gRedirectionPresent)
        return NULL;

    if (!(process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, gLauncherProcessId)))
        return NULL;

    if ((!InitializeProcThreadAttributeList(NULL, 1, 0, &size) && GetLastError() != ERROR_INSUFFICIENT_BUFFER) || size == 0)
        return NULL;

    if ((attrList = (PPROC_THREAD_ATTRIBUTE_LIST)MemoryAlloc(size, 1, TRUE)) == NULL)
        return NULL;

    if (!InitializeProcThreadAttributeList(attrList, 1, 0, &size)) {
        MemoryFree(attrList);
        return NULL;
    }

    if (!UpdateProcThreadAttribute(attrList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &process, sizeof(process), NULL, NULL)) {
        MemoryFree(attrList);
        return NULL;
    }

    return attrList;
}

// Create the target process in suspended state with the correct standard
// handles and does a best effort to make the non-elevated launcher as
// the parent process
void CreateTargetProcess()
{
    STARTUPINFOEX sie;
    LPSTARTUPINFO si;
    PROCESS_INFORMATION pi;
    BOOL result;
#ifdef UNICODE
    DWORD creationFlags = CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT;
#else
    DWORD creationFlags = CREATE_SUSPENDED;
#endif

    ZeroMemory(&sie, sizeof(STARTUPINFOEX));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    sie.StartupInfo.hStdInput = gStdInFile;
    sie.StartupInfo.hStdOutput = gStdOutFile;
    sie.StartupInfo.hStdError = gStdErrFile;
    sie.StartupInfo.dwFlags = STARTF_USESTDHANDLES;

    if ((sie.lpAttributeList = PrepareAttributeList()) != NULL) {
        creationFlags |= EXTENDED_STARTUPINFO_PRESENT;
        sie.StartupInfo.cb = sizeof(STARTUPINFOEX);
        si = (LPSTARTUPINFO)&sie;
    } else {
        sie.StartupInfo.cb = sizeof(STARTUPINFO);
        si = &sie.StartupInfo;
    }

    result = CreateProcess(NULL, gCmdLine, NULL, NULL, TRUE, creationFlags, gEnvironmentBlock, gCurrentDirectory, si, &pi);
    MemoryFree(sie.lpAttributeList);
    if (!result)
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

// Resumes the target process
void ResumeAndWaitTargetProcess()
{
    if (!ResumeThread(gTargetThread))
        SYS_ERROR();

    // Waits for target process termination if it wasn't reparented
    // to avoid breaking the process tree up to the launcher
    if (gRedirectionPresent)
        WaitForSingleObject(gTargetProcess, INFINITE);
}
