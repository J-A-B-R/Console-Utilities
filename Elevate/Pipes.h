#pragma once

HANDLE CreateIpcPipe(TCHAR* lpPipeName);

HANDLE CreateRedirectionPipe(TCHAR* lpPipeName, DWORD nStdHandle);

HANDLE StartHandlingRedirectionPipe(HANDLE hPipe, DWORD nStdHandle);
