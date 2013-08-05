#include <stdafx.h>
#include "Global.h"
#include "Pipes.h"


void CreateRandomName(TCHAR* lpName)
{
    UUID guid;
    TCHAR* buffer;

    if (UuidCreateSequential(&guid) == RPC_S_UUID_NO_ADDRESS)
        SYS_ERROR();

    if (UuidToString(&guid, &buffer) == RPC_S_OUT_OF_MEMORY)
        SYS_ERROR();

    _tcscpy_s(lpName, RANDOM_NAME_STRZ_LENGTH, buffer);
    RpcStringFree(&buffer);
}

HANDLE CreateDedicatedPipe(TCHAR* lpPipeName, DWORD dwOpenMode, BOOL bPipeModeByte)
{
    HANDLE hPipe;

    _tcscpy_s(PIPE_NAME_PREFIX, PIPE_NAME_PREFIX_LENGTH + 1, PIPE_NAME_PREFIX);
    CreateRandomName(lpPipeName + PIPE_NAME_PREFIX_LENGTH);
    
    if ((hPipe = CreateNamedPipe(
            lpPipeName,
            dwOpenMode | FILE_FLAG_FIRST_PIPE_INSTANCE,
            ((bPipeModeByte) ? (PIPE_TYPE_BYTE | PIPE_READMODE_BYTE) : (PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE)) | PIPE_REJECT_REMOTE_CLIENTS | PIPE_WAIT,
            1,
            PIPE_BUFFER_SIZE,
            PIPE_BUFFER_SIZE,
            PIPE_DEFAULT_TIME_OUT,
            NULL)) == INVALID_HANDLE_VALUE)
        SYS_ERROR();

    return hPipe;
}

HANDLE CreateIpcPipe(TCHAR* lpPipeName)
{
    return CreateDedicatedPipe(lpPipeName, PIPE_ACCESS_DUPLEX, FALSE);
}

HANDLE CreateRedirectionPipe(TCHAR* lpPipeName, DWORD nStdHandle)
{
    return CreateDedicatedPipe(lpPipeName, (nStdHandle == STD_INPUT_HANDLE) ? PIPE_ACCESS_OUTBOUND : PIPE_ACCESS_INBOUND, TRUE);
}

BOOL SendDword(DWORD data)
{
    DWORD written;

    if (!WriteFile(hIpcPipe, &data, sizeof(DWORD), &written, NULL))
        return FALSE;
}

BOOL SendData(LPVOID lpData, DWORD dataSize)
{
    DWORD written;

    if (!SendDword(dataSize))
        return FALSE;
    if (!WriteFile(hIpcPipe, lpData, dataSize, &written, NULL))
        return FALSE;
    return TRUE;
}

BOOL SendString(TCHAR* pipeName)
{
    DWORD written;

    return SendData(pipeName, (_tcslen(pipeName) + 1) * sizeof(TCHAR));
}

BOOL ReceiveDword(LPDWORD lpData)
{
    DWORD read;

    if (!ReadFile(hIpcPipe, lpData, sizeof(DWORD), &read, NULL))
        return FALSE;
}

BOOL ReceiveData(LPVOID* lppData, LPDWORD dataSize)
{
    DWORD read;

    if (!ReceiveDword(dataSize))
        return FALSE;
    *lppData = MemoryAlloc(*dataSize, 1);
    if (!ReadFile(hIpcPipe, *lppData, *dataSize, &read, NULL))
        return FALSE;
}

struct REDIRECTION_INFO {
    HANDLE Pipe;
    HANDLE Std;
};

HANDLE StartHandlingRedirectionPipe(HANDLE hPipe, DWORD nStdHandle)
{
    DWORD threadId;
    HANDLE hThread;
    LPTHREAD_START_ROUTINE tsr = (nStdHandle == STD_INPUT_HANDLE) ? HandleInRedirectionPipe : HandleOutRedirectionPipe;
    REDIRECTION_INFO* ri = (REDIRECTION_INFO*)malloc(sizeof(REDIRECTION_INFO));

    ri->Pipe = hPipe;
    ri->Std = GetStdHandle(STD_INPUT_HANDLE);

    hThread = CreateThread(NULL, 0, tsr, ri, 0, &threadId);

    if (hThread == NULL)
        SYS_ERROR();
}

BOOL GetRedirectionHandles(LPVOID lpParam, LPHANDLE pipe, LPHANDLE std)
{
    REDIRECTION_INFO* ri;

    if (lpParam)
        return FALSE;

    ri = (REDIRECTION_INFO*) lpParam;
    *pipe = ri->Pipe;
    *std = ri->Std;
    free(ri);
}

DWORD WINAPI HandleInRedirectionPipe(LPVOID lpParam)
{
    HANDLE pipe;
    HANDLE std;

    if (!GetRedirectionHandles(lpParam, &pipe, &std))
        APP_ERROR(0);

    ConnectNamedPipe(pipe, NULL);

    for (;;) {
        // TODO: Implement Read/Write loop
    }
}

DWORD WINAPI HandleOutRedirectionPipe(LPVOID lpParam)
{
    HANDLE pipe;
    HANDLE std;

    if (!GetRedirectionHandles(lpParam, &pipe, &std))
        APP_ERROR(0);

    for (;;) {
        // TODO: Implement Read/Write loop
    }
}
