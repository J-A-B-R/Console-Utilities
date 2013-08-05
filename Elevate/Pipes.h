#pragma once

HANDLE CreateIpcPipe(TCHAR* lpPipeName);

HANDLE CreateRedirectionPipe(TCHAR* lpPipeName, DWORD nStdHandle);

HANDLE StartHandlingRedirectionPipe(HANDLE hPipe, DWORD nStdHandle);

BOOL SendDword(DWORD data);

BOOL SendData(LPVOID lpData, DWORD dataSize);

BOOL SendString(TCHAR* pipeName);

BOOL ReceiveDword(LPDWORD lpData);

BOOL ReceiveData(LPVOID* lppData, LPDWORD dataSize);
