#include "stdafx.h"
#include "Global.h"
#include "Pipes.h"


typedef enum {
    Incoming,
    Outgoing
} redirectionPipe_t;

typedef struct {
    HANDLE Incoming;
    HANDLE Outgoing;
    redirectionPipe_t RedirectionPipe;
} REDIRECTION_INFO;


void CreateRandomName(TCHAR* lpName)
{
    UUID guid;
    TCHAR* buffer;

    if (UuidCreateSequential(&guid) == RPC_S_UUID_NO_ADDRESS)
        SYS_ERROR();

    if (UuidToString(&guid, &buffer) == RPC_S_OUT_OF_MEMORY)
        SYS_ERROR();

    _tcscpy_s(lpName, RANDOM_NAME_LENGTH + 1, buffer);
    RpcStringFree(&buffer);
}

HANDLE CreateDedicatedPipe(TCHAR* lpPipeName, DWORD dwOpenMode, BOOL bPipeModeByte)
{
    HANDLE hPipe;

    _tcscpy_s(lpPipeName, PIPE_NAME_PREFIX_LENGTH + 1, PIPE_NAME_PREFIX);
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

DWORD WINAPI HandleRedirectionPipe(LPVOID lpParam)
{
    REDIRECTION_INFO* ri;
    HANDLE incoming, outgoing, pipe;
    DWORD read, written;
    BYTE buffer[PIPE_BUFFER_SIZE];
    DWORD error = ERROR_SUCCESS;
    BOOL eof = FALSE;

    if (lpParam == NULL)
        return ERROR_INVALID_PARAMETER;

    ri = (REDIRECTION_INFO*) lpParam;
    incoming = ri->Incoming;
    outgoing = ri->Outgoing;
    pipe = (ri->RedirectionPipe == Incoming) ? ri->Incoming : ri->Outgoing;
    MemoryFree(ri);
    SetLastError(ERROR_SUCCESS);
    if (ConnectNamedPipe(pipe, NULL) || (error = GetLastError()) == ERROR_PIPE_CONNECTED) {
        while (!gExiting && !eof) {
            if (!ReadFile(incoming, buffer, PIPE_BUFFER_SIZE, &read, NULL))
                break;
            if (read < PIPE_BUFFER_SIZE)
                eof = TRUE;
            if (!WriteFile(outgoing, buffer, read, &written, NULL) || written != read)
                break;
            if (!FlushFileBuffers(outgoing))
                break;
        }
        if (!DisconnectNamedPipe(pipe) && (error == ERROR_SUCCESS || error == ERROR_BROKEN_PIPE))
            error = GetLastError();
    }

    return (error == ERROR_SUCCESS ||
            error == ERROR_BROKEN_PIPE ||
            error == ERROR_PIPE_CONNECTED ||
            error == ERROR_PIPE_NOT_CONNECTED ||
            error == ERROR_INVALID_HANDLE)  // CleanUp is kicking us out
        ? ERROR_SUCCESS
        : error;
}

HANDLE StartHandlingRedirectionPipe(HANDLE pipe, DWORD nStdHandle)
{
    DWORD threadId;
    HANDLE hThread;
    REDIRECTION_INFO* ri = (REDIRECTION_INFO*)MemoryAlloc(1, sizeof(REDIRECTION_INFO));

    if (ri == NULL)
        SYS_ERROR();

    if (nStdHandle == STD_INPUT_HANDLE) {
        ri->Incoming = GetStdHandle(nStdHandle);
        ri->Outgoing = pipe;
        ri->RedirectionPipe = Outgoing;
    } else {
        ri->Incoming = pipe;
        ri->Outgoing = GetStdHandle(nStdHandle);
        ri->RedirectionPipe = Incoming;
    }

    if ((hThread = CreateThread(NULL, 0, HandleRedirectionPipe, ri, 0, &threadId)) == NULL)
        SYS_ERROR();

    return hThread;
}
