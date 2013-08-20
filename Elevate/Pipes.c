/* Copyright 2013 Juan Burgos. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

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
    BOOL isIncomingDiskType;

    if (lpParam == NULL)
        return ERROR_INVALID_PARAMETER;

    ri = (REDIRECTION_INFO*) lpParam;
    incoming = ri->Incoming;
    outgoing = ri->Outgoing;
    pipe = (ri->RedirectionPipe == Incoming) ? ri->Incoming : ri->Outgoing;
    MemoryFree(ri);
    isIncomingDiskType = GetFileType(incoming) == FILE_TYPE_DISK;
    if (ConnectNamedPipe(pipe, NULL) || (error = GetLastError()) == ERROR_PIPE_CONNECTED) {
        while (!gExiting && !eof) {
            if (!ReadFile(incoming, buffer, PIPE_BUFFER_SIZE, &read, NULL)) {
                error = GetLastError();
                break;
            }
            if (isIncomingDiskType && read < PIPE_BUFFER_SIZE)
                eof = TRUE;
            if (!WriteFile(outgoing, buffer, read, &written, NULL) || written != read) {
                error = GetLastError();
                break;
            }
            // some devices don't support this function
            if (!FlushFileBuffers(outgoing) && GetLastError() != ERROR_INVALID_FUNCTION) {
                error = GetLastError();
                break;
            }
        }
        // we close directly the pipe; if we disconnect first, some programs (e.g.: find.exe) will complain
        if (!CloseHandle(pipe))
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
    REDIRECTION_INFO* ri = (REDIRECTION_INFO*)MemoryAlloc(1, sizeof(REDIRECTION_INFO), FALSE);

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

void SignalHandlingRedirecionPipesForExiting()
{
    gExiting = TRUE;
}

BOOL WaitHandlingRedirecionPipesForExiting()
{
    HANDLE threads[3];
    DWORD threadCount = 0;
    DWORD threadExitCode;
    UINT i;

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
                    return FALSE;
                }
            }
            break;
        case WAIT_FAILED:
            SysError();
            return FALSE;
        case WAIT_TIMEOUT:
            // NADA
            break;
        };
    return TRUE;
}